#include "RegisterAlgorithm.h"
#include "StarFinder.h"
#include "ConstellationMatcher.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include <stdexcept>
#include <iostream>
#include <QDebug>

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

    qInfo() << "[Register] Starting execution. Target batch:" << QString::fromStdString(inputName) << ", reference frame index:" << refFrameIdx << ", detection method:" << QString::fromStdString(detectionMethod) << ", min SNR:" << snrMin << ", min FWHM:" << minFwhm;

    // Advanced parameters
    int maxStars = std::stoi(config.at("max_stars"));
    double maxEccentricity = std::stod(config.at("max_eccentricity"));
    double matchTolerance = std::stod(config.at("match_tolerance"));
    // Note: simplex parameters are used in StarFinder if fitting
    bool fastFit = (detectionMethod == "centroid");

    WorkspaceElement inputElem = workspace.getElement(inputName);
    if (!std::holds_alternative<ImageBatchPtr>(inputElem)) {
        throw std::runtime_error("Star Registration requires an Image Batch as input");
    }

    auto batch = std::get<ImageBatchPtr>(inputElem);
    int count = batch->count();

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

    qInfo() << "[Register] Reference frame index" << refFrameIdx << "registered successfully with" << refStars.size() << "stars.";

    // Save reference frame metadata
    FrameMetadata refMeta;
    refMeta.registered = true;
    refMeta.dx = 0.0;
    refMeta.dy = 0.0;
    refMeta.theta = 0.0;
    refMeta.starCount = refStars.size();
    refMeta.qualityScore = refStars.size(); // baseline
    batch->setFrameMetadata(refFrameIdx, refMeta);

    // 2. Loop through all other selected frames and register them against the reference frame
    for (int i = 0; i < count; ++i) {
        if (progress) {
            progress(static_cast<int>(5.0 + 95.0 * i / count));
        }

        // Reference frame is already registered (0 offset)
        if (i == refFrameIdx) {
            continue;
        }

        // Skip unselected frames
        if (!batch->isFrameSelected(i)) {
            continue;
        }

        try {
            ImageVariant targetFrame = batch->getImage(i);
            auto targetChannel = getRegistrationChannel(targetFrame);
            if (!targetChannel) continue;

            // Detect stars in target frame
            std::vector<Star> targetStars = StarFinder::findStars(targetChannel, maxStars, snrMin, fastFit, 10, minFwhm, maxEccentricity);
            qInfo() << "[Register] Frame" << i << "detected" << targetStars.size() << "stars";
            if (targetStars.size() < 3) {
                qWarning() << "[Register] Frame" << i << "failed: only" << targetStars.size() << "stars detected. Frame auto-deselected.";
                batch->setFrameSelected(i, false); // auto-deselect failed frames
                continue;
            }

            // Match constellations to reference frame
            auto alignRes = ConstellationMatcher::match(refStars, targetStars, 7, matchTolerance);
            
            if (alignRes.success) {
                FrameMetadata meta;
                meta.registered = true;
                meta.dx = alignRes.dx;
                meta.dy = alignRes.dy;
                meta.theta = alignRes.theta;
                meta.starCount = alignRes.matchedStars;
                meta.qualityScore = alignRes.matchedStars / (1.0 + alignRes.rmsError);
                
                batch->setFrameMetadata(i, meta);
                
                qInfo() << "[Register] Registered frame" << i << "(" << QString::fromStdString(batch->frameName(i)) << ") -> dx=" << alignRes.dx << ", dy=" << alignRes.dy << ", theta=" << alignRes.theta << "(matched" << alignRes.matchedStars << "stars, RMS=" << alignRes.rmsError << ")";
            } else {
                qWarning() << "[Register] Constellation matching failed for frame" << i << ". Frame auto-deselected.";
                batch->setFrameSelected(i, false); // auto-deselect failed frames
            }
        } catch (const std::exception& e) {
            qWarning() << "[Register] Exception registering frame" << i << ":" << e.what() << ". Frame auto-deselected.";
            batch->setFrameSelected(i, false);
        }
    }

    if (progress) progress(100);
    qInfo() << "[Register] Star Registration completed.";
}

} // namespace blastro
