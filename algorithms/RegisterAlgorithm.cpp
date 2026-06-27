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
    double snrMin = std::stod(config.at("snr_min"));
    double minFwhm = std::stod(config.at("min_fwhm"));
    int maxStars = std::stoi(config.at("max_stars"));
    double maxEccentricity = std::stod(config.at("max_eccentricity"));
    double matchTolerance = std::stod(config.at("match_tolerance"));
    bool fastFit = (detectionMethod == "centroid");

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

    std::vector<Star> refStars = StarFinder::findStars(refChannel, maxStars, snrMin, fastFit, 10, minFwhm, maxEccentricity);
    if (refStars.size() < 3) {
        throw std::runtime_error("Failed to register reference frame: detected only " + 
                                 std::to_string(refStars.size()) + " stars (need at least 3)");
    }

    Logger::success("Register", QString("Reference frame index %1 registered successfully with %2 stars.")
                    .arg(refFrameIdx).arg(refStars.size()));

    // Compute average FWHM and SNR from reference stars
    double refSumFwhm = 0.0;
    double refSumSnr = 0.0;
    for (const auto& star : refStars) {
        refSumFwhm += star.fwhm;
        double bg = std::max(1e-6, star.background);
        refSumSnr += star.peak / bg;
    }
    double refAvgFwhm = refStars.empty() ? 0.0 : (refSumFwhm / refStars.size());
    double refAvgSnr = refStars.empty() ? 0.0 : (refSumSnr / refStars.size());

    // Save reference frame metadata
    FrameMetadata refMeta;
    refMeta.registered = true;
    refMeta.dx = 0.0;
    refMeta.dy = 0.0;
    refMeta.theta = 0.0;
    refMeta.starCount = refStars.size();
    refMeta.fwhm = refAvgFwhm;
    refMeta.snr = refAvgSnr;
    refMeta.qualityScore = refStars.size(); // baseline
    refMeta.stars = refStars;
    batch->setFrameMetadata(refFrameIdx, refMeta);

    // 2. Loop through all other selected frames and register them against the reference frame
    std::atomic<int> processedFrames(0);

    #pragma omp parallel for
    for (int i = 0; i < count; ++i) {
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

            // Detect stars in target frame
            std::vector<Star> targetStars = StarFinder::findStars(targetChannel, maxStars, snrMin, fastFit, 10, minFwhm, maxEccentricity);
            
            if (targetStars.size() < 3) {
                Logger::warning("Register", QString("Frame %1 failed: only %2 stars detected. Frame auto-deselected.")
                                .arg(i).arg(targetStars.size()));
                batch->setFrameSelected(i, false); // auto-deselect failed frames
                batch->clearCache(i);
                if (progress) {
                    int localProcessed = ++processedFrames;
                    progress(static_cast<int>(5.0 + 95.0 * localProcessed / count));
                }
                continue;
            }

            // Match constellations to reference frame
            auto alignRes = ConstellationMatcher::match(refStars, targetStars, 7, matchTolerance);
            
            if (alignRes.success) {
                // Compute average FWHM and SNR from target stars
                double sumFwhm = 0.0;
                double sumSnr = 0.0;
                for (const auto& star : targetStars) {
                    sumFwhm += star.fwhm;
                    double bg = std::max(1e-6, star.background);
                    sumSnr += star.peak / bg;
                }
                double avgFwhm = targetStars.empty() ? 0.0 : (sumFwhm / targetStars.size());
                double avgSnr = targetStars.empty() ? 0.0 : (sumSnr / targetStars.size());

                FrameMetadata meta;
                meta.registered = true;
                meta.dx = alignRes.dx;
                meta.dy = alignRes.dy;
                meta.theta = alignRes.theta;
                meta.starCount = alignRes.matchedStars;
                meta.fwhm = avgFwhm;
                meta.snr = avgSnr;
                meta.qualityScore = alignRes.matchedStars / (1.0 + alignRes.rmsError);
                meta.stars = targetStars;
                
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

    if (progress) progress(100);
    Logger::success("Register", QString("Star Registration completed in %1 ms (avg %2 ms per frame).")
                    .arg(totalTimer.elapsed()).arg(totalTimer.elapsed() / (count > 0 ? count : 1)));
}

} // namespace blastro
