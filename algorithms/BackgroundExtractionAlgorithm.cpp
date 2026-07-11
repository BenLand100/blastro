/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "BackgroundExtractionAlgorithm.h"
#include "BackgroundExtractor.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include "core/TempDirectory.h"
#include "core/MathUtils.h"
#include "io/FitsIO.h"
#include <stdexcept>
#include <iostream>
#include <QDebug>
#include <omp.h>
#include <QElapsedTimer>
#include <atomic>
#include "core/Preferences.h"
#include "core/Logger.h"

namespace blastro {

static GrayscaleImagePtr getExtractionChannel(const ImageVariant& img) {
    if (std::holds_alternative<GrayscaleImagePtr>(img)) {
        return std::get<GrayscaleImagePtr>(img);
    } else if (std::holds_alternative<RGBImagePtr>(img)) {
        return std::get<RGBImagePtr>(img)->g();
    }
    return nullptr;
}

void BackgroundExtractionAlgorithm::execute(WorkspaceRegistry& workspace,
                                            const std::map<std::string, std::string>& config,
                                            ProgressCallback progress) {
    std::string inputName  = config.count("input_name")  ? config.at("input_name")  : "";
    std::string outputName = config.count("output_name") ? config.at("output_name") : "";

    if (inputName.empty()) {
        auto names = workspace.elementNames();
        if (names.empty())
            throw std::runtime_error("No images in the workspace to extract background from");
        inputName = names[0];
    }
    if (outputName.empty())
        outputName = inputName + "_neutralized";

    int    order      = config.count("order")       ? std::stoi(config.at("order"))       : 3;
    bool   normalize  = config.count("normalize")   ? (config.at("normalize") == "true")  : true;
    int gridSize = config.count("grid_size") ? std::stoi(config.at("grid_size")) : 10;
    double huberDelta = config.count("huber_delta") ? std::stod(config.at("huber_delta")) : 5.0;
    std::string method = config.count("method") ? config.at("method") : "Polynomial";
    double rbfSmoothing = config.count("rbf_smoothing") ? std::stod(config.at("rbf_smoothing")) : 0.0;
    bool autoExclude = config.count("auto_exclude") ? (config.at("auto_exclude") == "true") : true;
    double maxDeviation = config.count("max_deviation") ? std::stod(config.at("max_deviation")) : 3.0;
    double maxStructure = config.count("max_structure") ? std::stod(config.at("max_structure")) : 1.5;
    double effectiveMaxDeviation = autoExclude ? maxDeviation : 9999.0;
    double effectiveMaxStructure = autoExclude ? maxStructure : 9999.0;

    int threads = config.count("threads") ? std::stoi(config.at("threads")) : -1;
    if (threads <= 0) {
        threads = Preferences::instance().getThreadCount();
    }
    if (threads > 0) {
        omp_set_num_threads(threads);
    }

    Logger::header("BackgroundExtraction", QString("Starting BGE execution. Input: %1, Output: %2, Method: %3, Order: %4, RbfSmoothing: %5, Normalize: %6, AutoExclude: %7, MaxDeviation: %8, MaxStructure: %9, Threads: %10")
                   .arg(QString::fromStdString(inputName))
                   .arg(QString::fromStdString(outputName))
                   .arg(QString::fromStdString(method))
                   .arg(order).arg(rbfSmoothing).arg(normalize).arg(autoExclude).arg(maxDeviation).arg(maxStructure).arg(threads));

    QElapsedTimer totalTimer;
    totalTimer.start();

    WorkspaceElement inputElem = workspace.getElement(inputName);
    if (progress) progress(5);

    if (std::holds_alternative<ImageBatchPtr>(inputElem)) {
        auto inputBatch = std::get<ImageBatchPtr>(inputElem);
        int count = inputBatch->count();

        // 1. Identify Reference Frame
        int refFrameIdx = -1;
        if (config.count("ref_frame_index") && !config.at("ref_frame_index").empty()) {
            refFrameIdx = std::stoi(config.at("ref_frame_index"));
        }
        if (refFrameIdx < 0 || refFrameIdx >= count) {
            for (int i = 0; i < count; ++i) {
                if (inputBatch->isFrameSelected(i)) {
                    auto meta = inputBatch->frameMetadata(i);
                    if (meta.registered && std::abs(meta.dx) < 1e-5 && std::abs(meta.dy) < 1e-5 && std::abs(meta.theta) < 1e-5) {
                        refFrameIdx = i;
                        break;
                    }
                }
            }
        }
        if (refFrameIdx < 0 || refFrameIdx >= count) {
            for (int i = 0; i < count; ++i) {
                if (inputBatch->isFrameSelected(i)) {
                    refFrameIdx = i;
                    break;
                }
            }
        }
        if (refFrameIdx < 0 || refFrameIdx >= count) {
            throw std::runtime_error("No valid frames selected in the batch");
        }

        Logger::info("BackgroundExtraction", QString("Selected reference frame index %1 for sample points").arg(refFrameIdx));

        // 2. Establish sample points on reference frame
        ImageVariant refFrame = inputBatch->getImage(refFrameIdx);
        auto refMeta = inputBatch->frameMetadata(refFrameIdx);
        auto refChannel = getExtractionChannel(refFrame);
        if (!refChannel) {
            throw std::runtime_error("Failed to extract reference channel from reference frame");
        }

        std::vector<std::pair<double, double>> refPts = refChannel->buffer()->bgeControlPoints();
        if (refPts.empty()) {
            Logger::info("BackgroundExtraction", "No manual control points on reference frame. Auto-generating sample grid...");
            refPts = BackgroundExtractor::generateGridPoints(refChannel, gridSize, gridSize, effectiveMaxDeviation, effectiveMaxStructure);
        }

        Logger::info("BackgroundExtraction", QString("Established %1 sample points on reference frame").arg(refPts.size()));
        inputBatch->clearCache(refFrameIdx);

        std::string tempDir = TempDirectory::createTempDir("bge");
        if (tempDir.empty())
            throw std::runtime_error("Failed to create intermediate directory for background extraction");

        std::vector<std::string> names(count);
        std::vector<std::string> filepaths(count);

        std::atomic<int> completedFrames{0};
        std::string exceptionMsg;
        std::atomic<bool> hasFailed{false};

        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < count; ++i) {
            if (hasFailed) continue;

            try {
                if (!inputBatch->isFrameSelected(i)) {
                    names[i]     = inputBatch->frameName(i) + "_unselected";
                    filepaths[i] = inputBatch->frameFilepath(i);
                    int completed = ++completedFrames;
                    if (progress) {
                        #pragma omp critical
                        progress(static_cast<int>(5.0 + 90.0 * completed / count));
                    }
                    continue;
                }

                ImageVariant frame = inputBatch->getImage(i);
                auto meta = inputBatch->frameMetadata(i);

                // Transform sample points if registered, otherwise reuse as-is
                std::vector<std::pair<double, double>> targetPts;
                if (meta.registered) {
                    auto invTrans = MathUtils::invertAffine(meta.transform);
                    auto tRefToTarget = MathUtils::multiplyAffine(invTrans, refMeta.transform);
                    for (const auto& pt : refPts) {
                        auto transPt = MathUtils::transformPoint(tRefToTarget, pt.first, pt.second);
                        targetPts.push_back(transPt);
                    }
                } else {
                    targetPts = refPts;
                }

                ImageVariant extracted;
                std::string frameName = inputBatch->frameName(i);
                if (std::holds_alternative<GrayscaleImagePtr>(frame)) {
                    auto gray = std::get<GrayscaleImagePtr>(frame);
                    auto res = BackgroundExtractor::extractGrayscale(
                        gray,
                        order, gridSize, huberDelta, normalize,
                        nullptr, 0, 100, method, rbfSmoothing, targetPts, effectiveMaxDeviation, effectiveMaxStructure, frameName);
                    res->setMetadata(gray->metadata());
                    extracted = res;
                } else if (std::holds_alternative<RGBImagePtr>(frame)) {
                    auto rgb = std::get<RGBImagePtr>(frame);
                    auto res = BackgroundExtractor::extractRGB(
                        rgb,
                        order, gridSize, huberDelta, normalize,
                        nullptr, method, rbfSmoothing, targetPts, effectiveMaxDeviation, effectiveMaxStructure, frameName);
                    res->setMetadata(rgb->metadata());
                    extracted = res;
                } else {
                    throw std::runtime_error("Unsupported frame type in batch background extraction");
                }

                std::string origPath    = inputBatch->frameFilepath(i);
                std::string outFilename = TempDirectory::getIntermediateFileName(origPath, "_bge", i);
                std::string fullOutPath = tempDir + "/" + outFilename;

                FitsIO writer;
                if (!writer.writeImage(fullOutPath, extracted))
                    throw std::runtime_error("Failed to write background-extracted frame to " + fullOutPath);

                names[i]     = inputBatch->frameName(i) + "_bge";
                filepaths[i] = fullOutPath;

                inputBatch->clearCache(i);

                int completed = ++completedFrames;
                if (progress) {
                    #pragma omp critical
                    progress(static_cast<int>(5.0 + 90.0 * completed / count));
                }
            } catch (const std::exception& e) {
                hasFailed = true;
                #pragma omp critical
                exceptionMsg = e.what();
            } catch (...) {
                hasFailed = true;
                #pragma omp critical
                exceptionMsg = "Unknown error occurred during parallel background extraction";
            }
        }

        if (hasFailed) {
            throw std::runtime_error(exceptionMsg);
        }

        auto outBatch = std::make_shared<ImageBatch>(
            count,
            [filepaths](int index) -> ImageVariant {
                FitsIO reader;
                return reader.readImage(filepaths[index]);
            },
            names, filepaths);

        for (int i = 0; i < count; ++i) {
            outBatch->setFrameSelected(i, inputBatch->isFrameSelected(i));
            outBatch->setFrameMetadata(i, inputBatch->frameMetadata(i));
        }

        if (progress) progress(100);
        if (workspace.contains(outputName))
            workspace.unregisterElement(outputName);
        workspace.registerElement(outputName, outBatch);

        Logger::success("BackgroundExtraction", QString("Finished batch BGE (%1 frames) in %2 ms. Registered output: %3")
                        .arg(count).arg(totalTimer.elapsed()).arg(QString::fromStdString(outputName)));
        return;
    }

    // Single-image mode
    WorkspaceElement outputElem;
    if (std::holds_alternative<GrayscaleImagePtr>(inputElem)) {
        auto gray = std::get<GrayscaleImagePtr>(inputElem);
        auto res = BackgroundExtractor::extractGrayscale(
            gray,
            order, gridSize, huberDelta, normalize,
            progress, 5, 95, method, rbfSmoothing, {}, effectiveMaxDeviation, effectiveMaxStructure, inputName);
        res->setMetadata(gray->metadata());
        outputElem = res;
    } else if (std::holds_alternative<RGBImagePtr>(inputElem)) {
        auto rgb = std::get<RGBImagePtr>(inputElem);
        auto res = BackgroundExtractor::extractRGB(
            rgb,
            order, gridSize, huberDelta, normalize,
            progress, method, rbfSmoothing, {}, effectiveMaxDeviation, effectiveMaxStructure, inputName);
        res->setMetadata(rgb->metadata());
        outputElem = res;
    } else {
        throw std::runtime_error("Background extraction is not supported on this element type");
    }

    if (workspace.contains(outputName))
        workspace.unregisterElement(outputName);
    workspace.registerElement(outputName, outputElem);

    if (progress) progress(100);
    Logger::success("BackgroundExtraction", QString("Finished BGE in %1 ms. Registered output: %2")
                    .arg(totalTimer.elapsed()).arg(QString::fromStdString(outputName)));
}

} // namespace blastro
