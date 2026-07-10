/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "BackgroundExtractionAlgorithm.h"
#include "BackgroundExtractor.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include "core/TempDirectory.h"
#include "io/FitsIO.h"
#include <stdexcept>
#include <iostream>
#include <QDebug>
#include <omp.h>
#include <QElapsedTimer>
#include "core/Preferences.h"
#include "core/Logger.h"

namespace blastro {

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
    double sigmaCut   = config.count("sigma_cut")   ? std::stod(config.at("sigma_cut"))   : 3.0;
    bool   equalize   = config.count("equalize")    ? (config.at("equalize") == "true")   : true;
    double sampleFrac = config.count("sample_frac") ? std::stod(config.at("sample_frac")) : 0.01;
    double huberDelta = config.count("huber_delta") ? std::stod(config.at("huber_delta")) : 5.0;
    std::string method = config.count("method") ? config.at("method") : "Polynomial";
    double rbfSmoothing = config.count("rbf_smoothing") ? std::stod(config.at("rbf_smoothing")) : 0.0;

    int threads = config.count("threads") ? std::stoi(config.at("threads")) : -1;
    if (threads <= 0) {
        threads = Preferences::instance().getThreadCount();
    }
    if (threads > 0) {
        omp_set_num_threads(threads);
    }

    Logger::header("BackgroundExtraction", QString("Starting BGE execution. Input: %1, Output: %2, Method: %3, Order: %4, RbfSmoothing: %5, SigmaCut: %6, Equalize: %7, Threads: %8")
                   .arg(QString::fromStdString(inputName))
                   .arg(QString::fromStdString(outputName))
                   .arg(QString::fromStdString(method))
                   .arg(order).arg(rbfSmoothing).arg(sigmaCut).arg(equalize).arg(threads));

    QElapsedTimer totalTimer;
    totalTimer.start();

    WorkspaceElement inputElem = workspace.getElement(inputName);
    if (progress) progress(5);

    // -----------------------------------------------------------------------
    // Batch mode: process each selected frame, write to disk, return new batch
    // -----------------------------------------------------------------------
    if (std::holds_alternative<ImageBatchPtr>(inputElem)) {
        auto inputBatch = std::get<ImageBatchPtr>(inputElem);
        int count = inputBatch->count();

        std::string tempDir = TempDirectory::createTempDir("bge");
        if (tempDir.empty())
            throw std::runtime_error("Failed to create intermediate directory for background extraction");

        std::vector<std::string> names(count);
        std::vector<std::string> filepaths(count);
        FitsIO writer;

        for (int i = 0; i < count; ++i) {
            if (progress)
                progress(static_cast<int>(5.0 + 90.0 * i / count));

            if (!inputBatch->isFrameSelected(i)) {
                names[i]     = inputBatch->frameName(i) + "_unselected";
                filepaths[i] = inputBatch->frameFilepath(i);
                continue;
            }

            ImageVariant frame = inputBatch->getImage(i);
            ImageVariant extracted;

            if (std::holds_alternative<GrayscaleImagePtr>(frame)) {
                auto gray = std::get<GrayscaleImagePtr>(frame);
                auto res = BackgroundExtractor::extractGrayscale(
                    gray,
                    order, sigmaCut, sampleFrac, huberDelta, equalize,
                    nullptr, 0, 100, method, rbfSmoothing);
                res->setMetadata(gray->metadata());
                extracted = res;
            } else if (std::holds_alternative<RGBImagePtr>(frame)) {
                auto rgb = std::get<RGBImagePtr>(frame);
                auto res = BackgroundExtractor::extractRGB(
                    rgb,
                    order, sigmaCut, sampleFrac, huberDelta, equalize,
                    nullptr, method, rbfSmoothing);
                res->setMetadata(rgb->metadata());
                extracted = res;
            } else {
                throw std::runtime_error("Unsupported frame type in batch background extraction");
            }

            std::string origPath    = inputBatch->frameFilepath(i);
            std::string outFilename = TempDirectory::getIntermediateFileName(origPath, "_bge", i);
            std::string fullOutPath = tempDir + "/" + outFilename;

            if (!writer.writeImage(fullOutPath, extracted))
                throw std::runtime_error("Failed to write background-extracted frame to " + fullOutPath);

            names[i]     = inputBatch->frameName(i) + "_bge";
            filepaths[i] = fullOutPath;

            inputBatch->clearCache(i);
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

        Logger::success("BackgroundExtraction", QString("Finished batch BGE (%1 frames) in %2 ms (avg %3 ms per frame). Registered output: %4")
                        .arg(count).arg(totalTimer.elapsed()).arg(totalTimer.elapsed() / (count > 0 ? count : 1))
                        .arg(QString::fromStdString(outputName)));
        return;
    }

    // -----------------------------------------------------------------------
    // Single-image mode
    // -----------------------------------------------------------------------
    WorkspaceElement outputElem;
    if (std::holds_alternative<GrayscaleImagePtr>(inputElem)) {
        auto gray = std::get<GrayscaleImagePtr>(inputElem);
        auto res = BackgroundExtractor::extractGrayscale(
            gray,
            order, sigmaCut, sampleFrac, huberDelta, equalize,
            progress, 5, 95, method, rbfSmoothing);
        res->setMetadata(gray->metadata());
        outputElem = res;
    } else if (std::holds_alternative<RGBImagePtr>(inputElem)) {
        auto rgb = std::get<RGBImagePtr>(inputElem);
        auto res = BackgroundExtractor::extractRGB(
            rgb,
            order, sigmaCut, sampleFrac, huberDelta, equalize,
            progress, method, rbfSmoothing);
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
