/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "RegisterAlgorithm.h"
#include "ConstellationMatcher.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include "core/Logger.h"
#include "core/Preferences.h"
#include "PreprocessingPipeline.h"
#include <stdexcept>
#include <iostream>
#include <atomic>
#include <QElapsedTimer>
#include <omp.h>
#include <algorithm>
#include <limits>

namespace blastro {

void RegisterAlgorithm::execute(WorkspaceRegistry& workspace, 
                               const std::map<std::string, std::string>& config, 
                               ProgressCallback progress) {
    std::string inputName = config.at("input_name");
    
    int refFrameIdx = -1;
    if (config.count("ref_frame_index") && !config.at("ref_frame_index").empty()) {
        refFrameIdx = std::stoi(config.at("ref_frame_index"));
    }
    
    std::string refStrategy = config.count("reference_strategy") ? config.at("reference_strategy") : "snr";
    int maxStars = config.count("max_stars") ? std::stoi(config.at("max_stars")) : 500;
    double matchTolerance = config.count("match_tolerance") ? std::stod(config.at("match_tolerance")) : 1.5;
    bool useAffine = (config.count("transformation_model") && config.at("transformation_model") == "affine");

    WorkspaceElement inputElem = workspace.getElement(inputName);
    if (!std::holds_alternative<ImageBatchPtr>(inputElem)) {
        throw std::runtime_error("Star Registration requires an Image Batch as input");
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

    QElapsedTimer totalTimer;
    totalTimer.start();

    // 1. Choose the Reference Frame if not explicitly set
    if (refFrameIdx < 0 || refFrameIdx >= count) {
        double bestVal = (refStrategy == "fwhm") ? std::numeric_limits<double>::infinity() : -1.0;
        for (int i = 0; i < count; ++i) {
            if (!batch->isFrameSelected(i)) continue;
            auto meta = batch->frameMetadata(i);
            if (meta.starCount < 3) continue;
            if (refStrategy == "fwhm") {
                if (meta.fwhm < bestVal && meta.fwhm > 0.0) {
                    bestVal = meta.fwhm;
                    refFrameIdx = i;
                }
            } else {
                if (meta.snr > bestVal) {
                    bestVal = meta.snr;
                    refFrameIdx = i;
                }
            }
        }
    }

    // Fallback to first selected frame
    if (refFrameIdx < 0 || refFrameIdx >= count) {
        for (int i = 0; i < count; ++i) {
            if (batch->isFrameSelected(i)) {
                refFrameIdx = i;
                break;
            }
        }
    }

    if (refFrameIdx < 0 || refFrameIdx >= count) {
        throw std::runtime_error("No valid selected frames found to register");
    }

    Logger::header("Register", QString("Starting execution. Target batch: %1, chosen reference frame index: %2, reference strategy: %3, threads: %4")
                   .arg(QString::fromStdString(inputName))
                   .arg(refFrameIdx)
                   .arg(QString::fromStdString(refStrategy))
                   .arg(threads));

    // 2. Prepare Reference Frame Stars
    auto refMeta = batch->frameMetadata(refFrameIdx);
    std::vector<Star> refStarsForMatching = refMeta.stars;
    std::sort(refStarsForMatching.begin(), refStarsForMatching.end(), [](const Star& a, const Star& b) {
        return a.peak > b.peak;
    });
    if (refStarsForMatching.size() > static_cast<size_t>(maxStars)) {
        refStarsForMatching.resize(maxStars);
    }

    if (refStarsForMatching.size() < 3) {
        throw std::runtime_error("Reference frame has too few stars (" + std::to_string(refStarsForMatching.size()) + ") for registration. Run StarFinding first.");
    }

    Logger::info("Register", QString("Reference frame index %1 has %2 stars for matching.")
                 .arg(refFrameIdx).arg(refStarsForMatching.size()));

    // Set identity transformation on reference frame
    refMeta.registered = true;
    refMeta.dx = 0.0;
    refMeta.dy = 0.0;
    refMeta.theta = 0.0;
    refMeta.transform = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
    batch->setFrameMetadata(refFrameIdx, refMeta);

    // 3. Match other frames
    std::atomic<int> processedFrames(0);
    bool cancelled = false;

    #pragma omp parallel for shared(cancelled)
    for (int i = 0; i < count; ++i) {
        if (cancelled) continue;

        if (PreprocessingPipeline::isCancelled()) {
            cancelled = true;
            continue;
        }

        if (i == refFrameIdx) {
            if (progress) {
                int localProcessed = ++processedFrames;
                progress(static_cast<int>(10.0 + 90.0 * localProcessed / count));
            }
            continue;
        }

        if (!batch->isFrameSelected(i)) {
            if (progress) {
                int localProcessed = ++processedFrames;
                progress(static_cast<int>(10.0 + 90.0 * localProcessed / count));
            }
            continue;
        }

        try {
            QElapsedTimer frameTimer;
            frameTimer.start();

            auto meta = batch->frameMetadata(i);
            std::vector<Star> targetStarsForMatching = meta.stars;
            std::sort(targetStarsForMatching.begin(), targetStarsForMatching.end(), [](const Star& a, const Star& b) {
                return a.peak > b.peak;
            });
            if (targetStarsForMatching.size() > static_cast<size_t>(maxStars)) {
                targetStarsForMatching.resize(maxStars);
            }

            if (targetStarsForMatching.size() < 3) {
                Logger::warning("Register", QString("Frame %1 has too few stars (%2). Frame auto-deselected.")
                                .arg(i).arg(targetStarsForMatching.size()));
                batch->setFrameSelected(i, false);
                if (progress) {
                    int localProcessed = ++processedFrames;
                    progress(static_cast<int>(10.0 + 90.0 * localProcessed / count));
                }
                continue;
            }

            auto alignRes = ConstellationMatcher::match(refStarsForMatching, targetStarsForMatching, 7, matchTolerance, useAffine);
            if (alignRes.success) {
                meta.registered = true;
                meta.dx = alignRes.dx;
                meta.dy = alignRes.dy;
                meta.theta = alignRes.theta;
                meta.transform = alignRes.transform;
                batch->setFrameMetadata(i, meta);

                Logger::info("Register", QString("Registered frame %1/%2 (%3) -> dx=%4, dy=%5, theta=%6 (matched %7 stars, RMS=%8, took %9 ms)")
                             .arg(i + 1).arg(count)
                             .arg(QString::fromStdString(batch->frameName(i)))
                             .arg(alignRes.dx).arg(alignRes.dy).arg(alignRes.theta)
                             .arg(alignRes.matchedStars).arg(alignRes.rmsError).arg(frameTimer.elapsed()));
            } else {
                Logger::warning("Register", QString("Constellation matching failed for frame %1. Frame auto-deselected.")
                                .arg(i));
                batch->setFrameSelected(i, false);
            }

        } catch (const std::exception& e) {
            Logger::warning("Register", QString("Exception registering frame %1: %2. Frame auto-deselected.")
                            .arg(i).arg(e.what()));
            batch->setFrameSelected(i, false);
        }

        if (progress) {
            int localProcessed = ++processedFrames;
            progress(static_cast<int>(10.0 + 90.0 * localProcessed / count));
        }
    }

    if (cancelled || PreprocessingPipeline::isCancelled()) {
        throw std::runtime_error("Star Registration cancelled by user.");
    }

    if (progress) progress(100);
    Logger::success("Register", QString("Star Registration completed in %1 ms.")
                    .arg(totalTimer.elapsed()));
}

} // namespace blastro
