/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "StarFindingAlgorithm.h"
#include "StarFinder.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include "core/Logger.h"
#include "core/Preferences.h"
#include "PreprocessingPipeline.h"
#include <stdexcept>
#include <QElapsedTimer>
#include <atomic>
#include <omp.h>
#include <algorithm>

namespace blastro {

static GrayscaleImagePtr getDetectionChannel(const ImageVariant& img) {
    if (std::holds_alternative<GrayscaleImagePtr>(img)) {
        return std::get<GrayscaleImagePtr>(img);
    } else if (std::holds_alternative<RGBImagePtr>(img)) {
        return std::get<RGBImagePtr>(img)->g();
    }
    return nullptr;
}

void StarFindingAlgorithm::execute(WorkspaceRegistry& workspace, 
                                  const std::map<std::string, std::string>& config, 
                                  ProgressCallback progress) {
    std::string inputName = config.at("input_name");
    std::string detectionMethod = config.count("detection_method") ? config.at("detection_method") : "adaptive";
    if (detectionMethod == "starfinder") {
        detectionMethod = "adaptive";
    }
    double snrMin = config.count("snr_min") ? std::stod(config.at("snr_min")) : 4.0;
    double minFwhm = config.count("min_fwhm") ? std::stod(config.at("min_fwhm")) : 1.5;
    int maxStars = config.count("max_stars") ? std::stoi(config.at("max_stars")) : 10000;
    int maxRefinedStars = config.count("max_refined_stars") ? std::stoi(config.at("max_refined_stars")) : 250;
    double maxEccentricity = config.count("max_eccentricity") ? std::stod(config.at("max_eccentricity")) : 0.9;

    WorkspaceElement inputElem = workspace.getElement(inputName);
    if (!std::holds_alternative<ImageBatchPtr>(inputElem)) {
        throw std::runtime_error("Star Finding requires an Image Batch as input");
    }

    auto batch = std::get<ImageBatchPtr>(inputElem);
    int count = batch->count();

    int threads = config.count("threads") ? std::stoi(config.at("threads")) : -1;
    if (threads <= 0) {
        threads = Preferences::instance().getThreadCount();
    }
    if (threads > 0) {
        omp_set_num_threads(threads);
    }

    Logger::header("StarFinding", QString("Starting execution. Target batch: %1, detection method: %2, min SNR: %3, min FWHM: %4, max stars: %5, max refined: %6, threads: %7")
                   .arg(QString::fromStdString(inputName))
                   .arg(QString::fromStdString(detectionMethod))
                   .arg(snrMin)
                   .arg(minFwhm)
                   .arg(maxStars)
                   .arg(maxRefinedStars)
                   .arg(threads));

    QElapsedTimer totalTimer;
    totalTimer.start();

    std::atomic<int> processedFrames(0);
    bool cancelled = false;

    #pragma omp parallel for shared(cancelled)
    for (int i = 0; i < count; ++i) {
        if (cancelled) continue;

        if (PreprocessingPipeline::isCancelled()) {
            cancelled = true;
            continue;
        }

        if (!batch->isFrameSelected(i)) {
            if (progress) {
                int localProcessed = ++processedFrames;
                progress(static_cast<int>(100.0 * localProcessed / count));
            }
            continue;
        }

        try {
            QElapsedTimer frameTimer;
            frameTimer.start();

            ImageVariant frame = batch->getImage(i);
            auto channel = getDetectionChannel(frame);
            if (!channel) {
                batch->clearCache(i);
                if (progress) {
                    int localProcessed = ++processedFrames;
                    progress(static_cast<int>(100.0 * localProcessed / count));
                }
                continue;
            }

            std::vector<Star> detectedStars = StarFinder::findStars(channel, maxStars, snrMin, detectionMethod, 10, minFwhm, maxEccentricity, maxRefinedStars);

            // Compute statistics
            double sumFwhm = 0.0;
            double sumSnr = 0.0;
            for (const auto& star : detectedStars) {
                sumFwhm += star.fwhm;
                sumSnr += star.snr;
            }
            double avgFwhm = detectedStars.empty() ? 0.0 : (sumFwhm / detectedStars.size());
            double avgSnr = detectedStars.empty() ? 0.0 : (sumSnr / detectedStars.size());

            FrameMetadata meta = batch->frameMetadata(i);
            meta.starCount = detectedStars.size();
            meta.fwhm = avgFwhm;
            meta.snr = avgSnr;
            meta.stars = detectedStars;
            batch->setFrameMetadata(i, meta);

            Logger::info("StarFinding", QString("Detected %1 stars (avg FWHM=%2, avg SNR=%3) in frame %4/%5 (%6) in %7 ms")
                         .arg(detectedStars.size())
                         .arg(avgFwhm, 0, 'f', 2)
                         .arg(avgSnr, 0, 'f', 2)
                         .arg(i + 1).arg(count)
                         .arg(QString::fromStdString(batch->frameName(i)))
                         .arg(frameTimer.elapsed()));

        } catch (const std::exception& e) {
            Logger::warning("StarFinding", QString("Exception in frame %1: %2. Auto-deselecting.")
                            .arg(i).arg(e.what()));
            batch->setFrameSelected(i, false);
        }

        batch->clearCache(i);

        if (progress) {
            int localProcessed = ++processedFrames;
            progress(static_cast<int>(100.0 * localProcessed / count));
        }
    }

    if (cancelled || PreprocessingPipeline::isCancelled()) {
        throw std::runtime_error("Star Finding cancelled by user.");
    }

    if (progress) progress(100);
    Logger::success("StarFinding", QString("Star Finding completed in %1 ms.")
                    .arg(totalTimer.elapsed()));
}

} // namespace blastro
