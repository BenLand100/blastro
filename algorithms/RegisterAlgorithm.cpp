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

#include "RegisterAlgorithm.h"
#include "StarFinder.h"
#include "ConstellationMatcher.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include <stdexcept>
#include <iostream>
#include "core/Logger.h"
#include <omp.h>
#include <QElapsedTimer>
#include <atomic>
#include "core/Preferences.h"
#include "PreprocessingPipeline.h"

namespace blastro {

// Helper to extract a single grayscale channel from an ImageVariant for star detection
static GrayscaleImagePtr getRegistrationChannel(const ImageVariant& img) {
    if (std::holds_alternative<GrayscaleImagePtr>(img)) {
        return std::get<GrayscaleImagePtr>(img);
    } else if (std::holds_alternative<RGBImagePtr>(img)) {
        // Use Green channel (channel 1) by default for astrophotography registration
        return std::get<RGBImagePtr>(img)->g();
    }
    return nullptr;
}

void RegisterAlgorithm::execute(WorkspaceRegistry& workspace, 
                               const std::map<std::string, std::string>& config, 
                               ProgressCallback progress) {
    std::string inputName = config.at("input_name");
    int refFrameIdx = std::stoi(config.at("ref_frame_index"));
    std::string detectionMethod = config.at("detection_method");
    if (detectionMethod == "starfinder") {
        detectionMethod = "adaptive";
    }
    double snrMin = std::stod(config.at("snr_min"));
    double minFwhm = std::stod(config.at("min_fwhm"));
    int maxStars = std::stoi(config.at("max_stars"));
    double maxEccentricity = std::stod(config.at("max_eccentricity"));
    double matchTolerance = std::stod(config.at("match_tolerance"));
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

    Logger::header("Register", QString("Starting execution. Target batch: %1, reference frame index: %2, detection method: %3, min SNR: %4, min FWHM: %5, threads: %6")
                   .arg(QString::fromStdString(inputName))
                   .arg(refFrameIdx)
                   .arg(QString::fromStdString(detectionMethod))
                   .arg(snrMin)
                   .arg(minFwhm)
                   .arg(threads));

    QElapsedTimer totalTimer;
    totalTimer.start();

    if (refFrameIdx < 0 || refFrameIdx >= count) {
        throw std::out_of_range("Reference frame index is out of bounds");
    }

    // 1. Detect stars in the Reference Frame
    if (progress) {
        progress(5); // Star detection on reference frame starting
    }

    ImageVariant refFrame = batch->getImage(refFrameIdx);
    auto refChannel = getRegistrationChannel(refFrame);
    if (!refChannel) {
        throw std::runtime_error("Failed to extract registration channel from reference frame");
    }

    // Detect all stars (up to 10,000) for statistics in Adaptive mode, but slice top maxStars for matching
    int maxStarsDetect = (detectionMethod == "adaptive") ? 10000 : maxStars;
    std::vector<Star> refStars = StarFinder::findStars(refChannel, maxStarsDetect, snrMin, detectionMethod, 10, minFwhm, maxEccentricity);
    
    std::vector<Star> refStarsForMatching = refStars;
    if (detectionMethod == "adaptive") {
        // Sort stars descending by peak brightness
        std::sort(refStarsForMatching.begin(), refStarsForMatching.end(), [](const Star& a, const Star& b) {
            return a.peak > b.peak;
        });
        if (refStarsForMatching.size() > static_cast<size_t>(maxStars)) {
            refStarsForMatching.resize(maxStars);
        }
    }

    if (refStarsForMatching.size() < 3) {
        throw std::runtime_error("Failed to register reference frame: detected only " + 
                                 std::to_string(refStarsForMatching.size()) + " stars (need at least 3)");
    }

    Logger::success("Register", QString("Reference frame index %1 registered successfully with %2 stars for matching (total detected: %3).")
                    .arg(refFrameIdx).arg(refStarsForMatching.size()).arg(refStars.size()));

    // Compute average FWHM and SNR from all detected reference stars
    double refSumFwhm = 0.0;
    double refSumSnr = 0.0;
    for (const auto& star : refStars) {
        refSumFwhm += star.fwhm;
        refSumSnr += star.snr;
    }
    double refAvgFwhm = refStars.empty() ? 0.0 : (refSumFwhm / refStars.size());
    double refAvgSnr = refStars.empty() ? 0.0 : (refSumSnr / refStars.size());

    // Save reference frame metadata
    FrameMetadata refMeta;
    refMeta.registered = true;
    refMeta.dx = 0.0;
    refMeta.dy = 0.0;
    refMeta.theta = 0.0;
    refMeta.transform = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
    refMeta.starCount = refStars.size();
    refMeta.fwhm = refAvgFwhm;
    refMeta.snr = refAvgSnr;
    refMeta.stars = refStars;
    batch->setFrameMetadata(refFrameIdx, refMeta);


    // 2. Loop through all other selected frames and register them against the reference frame
    std::atomic<int> processedFrames(0);

    bool cancelled = false;
    #pragma omp parallel for shared(cancelled)
    for (int i = 0; i < count; ++i) {
        if (cancelled) continue;

        if (PreprocessingPipeline::isCancelled()) {
            cancelled = true;
            continue;
        }

        // Reference frame is already registered (0 offset)
        if (i == refFrameIdx) {
            if (progress) {
                int localProcessed = ++processedFrames;
                progress(static_cast<int>(5.0 + 95.0 * localProcessed / count));
            }
            continue;
        }

        // Skip unselected frames
        if (!batch->isFrameSelected(i)) {
            if (progress) {
                int localProcessed = ++processedFrames;
                progress(static_cast<int>(5.0 + 95.0 * localProcessed / count));
            }
            continue;
        }

        try {
            QElapsedTimer frameTimer;
            frameTimer.start();

            ImageVariant targetFrame = batch->getImage(i);
            auto targetChannel = getRegistrationChannel(targetFrame);
            if (!targetChannel) {
                batch->clearCache(i);
                if (progress) {
                    int localProcessed = ++processedFrames;
                    progress(static_cast<int>(5.0 + 95.0 * localProcessed / count));
                }
                continue;
            }

            // Detect stars in target frame (up to 10,000 in Adaptive mode)
            int maxStarsDetect = (detectionMethod == "adaptive") ? 10000 : maxStars;
            std::vector<Star> targetStars = StarFinder::findStars(targetChannel, maxStarsDetect, snrMin, detectionMethod, 10, minFwhm, maxEccentricity);
            
            std::vector<Star> targetStarsForMatching = targetStars;
            if (detectionMethod == "adaptive") {
                // Sort stars descending by peak brightness
                std::sort(targetStarsForMatching.begin(), targetStarsForMatching.end(), [](const Star& a, const Star& b) {
                    return a.peak > b.peak;
                });
                if (targetStarsForMatching.size() > static_cast<size_t>(maxStars)) {
                    targetStarsForMatching.resize(maxStars);
                }
            }

            if (targetStarsForMatching.size() < 3) {
                Logger::warning("Register", QString("Frame %1 failed: only %2 stars detected (matching subset: %3). Frame auto-deselected.")
                                .arg(i).arg(targetStars.size()).arg(targetStarsForMatching.size()));
                batch->setFrameSelected(i, false); // auto-deselect failed frames
                batch->clearCache(i);
                if (progress) {
                    int localProcessed = ++processedFrames;
                    progress(static_cast<int>(5.0 + 95.0 * localProcessed / count));
                }
                continue;
            }

            // Match constellations to reference frame using brightest subsets
            auto alignRes = ConstellationMatcher::match(refStarsForMatching, targetStarsForMatching, 7, matchTolerance, useAffine);
            
            if (alignRes.success) {
                // Compute average FWHM and SNR from all detected target stars
                double sumFwhm = 0.0;
                double sumSnr = 0.0;
                for (const auto& star : targetStars) {
                    sumFwhm += star.fwhm;
                    sumSnr += star.snr;
                }
                double avgFwhm = targetStars.empty() ? 0.0 : (sumFwhm / targetStars.size());
                double avgSnr = targetStars.empty() ? 0.0 : (sumSnr / targetStars.size());

                FrameMetadata meta;
                meta.registered = true;
                meta.dx = alignRes.dx;
                meta.dy = alignRes.dy;
                meta.theta = alignRes.theta;
                meta.transform = alignRes.transform;
                meta.starCount = targetStars.size(); // Total detected stars count!
                meta.fwhm = avgFwhm;
                meta.snr = avgSnr;
                meta.stars = targetStars; // All detected stars!
                
                batch->setFrameMetadata(i, meta);
                
                Logger::info("Register", QString("Registered frame %1/%2 (%3) -> dx=%4, dy=%5, theta=%6 (matched %7 stars, RMS=%8, took %9 ms)")
                             .arg(i + 1).arg(count)
                             .arg(QString::fromStdString(batch->frameName(i)))
                             .arg(alignRes.dx).arg(alignRes.dy).arg(alignRes.theta)
                             .arg(alignRes.matchedStars).arg(alignRes.rmsError).arg(frameTimer.elapsed()));
            } else {
                Logger::warning("Register", QString("Constellation matching failed for frame %1. Frame auto-deselected.")
                                .arg(i));
                batch->setFrameSelected(i, false); // auto-deselect failed frames
            }
        } catch (const std::exception& e) {
            Logger::warning("Register", QString("Exception registering frame %1: %2. Frame auto-deselected.")
                            .arg(i).arg(e.what()));
            batch->setFrameSelected(i, false);
        }

        // Evict frame from RAM to keep memory usage flat
        batch->clearCache(i);

        if (progress) {
            int localProcessed = ++processedFrames;
            progress(static_cast<int>(5.0 + 95.0 * localProcessed / count));
        }
    }

    if (cancelled || PreprocessingPipeline::isCancelled()) {
        throw std::runtime_error("Preprocessing cancelled by user.");
    }

    if (progress) progress(100);
    Logger::success("Register", QString("Star Registration completed in %1 ms (avg %2 ms per frame).")
                    .arg(totalTimer.elapsed()).arg(totalTimer.elapsed() / (count > 0 ? count : 1)));
}

} // namespace blastro
