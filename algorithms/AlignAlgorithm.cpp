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

#include "AlignAlgorithm.h"
#include "Warping.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include "core/TempDirectory.h"
#include "io/FitsIO.h"
#include <stdexcept>
#include <iostream>
#include <omp.h>
#include <QDebug>
#include <QElapsedTimer>
#include "core/Preferences.h"
#include "core/Logger.h"
#include "PreprocessingPipeline.h"

namespace blastro {

void AlignAlgorithm::execute(WorkspaceRegistry& workspace, 
                             const std::map<std::string, std::string>& config, 
                             ProgressCallback progress) {
    std::string inputName = config.at("input_name");
    std::string outputName = config.at("output_name");
    double drizzleScale = std::stod(config.at("drizzle_scale"));
    int threads = config.count("threads") ? std::stoi(config.at("threads")) : -1;
    if (threads <= 0) {
        threads = Preferences::instance().getThreadCount();
    }
    bool evictCache = config.at("evict_cache") == "true";

    Logger::header("Align", QString("Starting execution. Input batch: %1, output batch: %2, drizzle scale: %3, threads: %4")
                   .arg(QString::fromStdString(inputName))
                   .arg(QString::fromStdString(outputName))
                   .arg(drizzleScale).arg(threads));

    QElapsedTimer totalTimer;
    totalTimer.start();

    // Set thread count for OpenMP parallel loops
    if (threads > 0) {
        omp_set_num_threads(threads);
    }

    WorkspaceElement inputElem = workspace.getElement(inputName);
    if (!std::holds_alternative<ImageBatchPtr>(inputElem)) {
        throw std::runtime_error("Image Alignment requires an Image Batch as input");
    }

    auto batch = std::get<ImageBatchPtr>(inputElem);
    int count = batch->count();

    // Determine alignment reference mode
    std::string refMode = "average_center";
    if (config.find("reference_mode") != config.end()) {
        refMode = config.at("reference_mode");
    }

    int centerFrameIdx = -1;
    double refDx = 0.0;
    double refDy = 0.0;
    double refTheta = 0.0;

    if (refMode == "average_center") {
        double sumDx = 0.0;
        double sumDy = 0.0;
        int validCount = 0;
        for (int i = 0; i < count; ++i) {
            if (batch->isFrameSelected(i)) {
                FrameMetadata meta = batch->frameMetadata(i);
                if (meta.registered) {
                    sumDx += meta.dx;
                    sumDy += meta.dy;
                    validCount++;
                }
            }
        }
        if (validCount > 0) {
            double avgDx = sumDx / validCount;
            double avgDy = sumDy / validCount;
            double minDist = std::numeric_limits<double>::max();
            for (int i = 0; i < count; ++i) {
                if (batch->isFrameSelected(i)) {
                    FrameMetadata meta = batch->frameMetadata(i);
                    if (meta.registered) {
                        double dist = (meta.dx - avgDx) * (meta.dx - avgDx) + 
                                      (meta.dy - avgDy) * (meta.dy - avgDy);
                        if (dist < minDist) {
                            minDist = dist;
                            centerFrameIdx = i;
                        }
                    }
                }
            }
        }
        if (centerFrameIdx != -1) {
            FrameMetadata centerMeta = batch->frameMetadata(centerFrameIdx);
            refDx = centerMeta.dx;
            refDy = centerMeta.dy;
            refTheta = centerMeta.theta;
            Logger::info("Align", QString("Average Center Mode: Selected frame %1 (%2) as center reference (offsets: dx=%3, dy=%4, theta=%5)")
                         .arg(centerFrameIdx)
                         .arg(QString::fromStdString(batch->frameName(centerFrameIdx)))
                         .arg(refDx).arg(refDy).arg(refTheta));
        } else {
            Logger::warning("Align", "Average Center Mode requested, but no registered/selected frames found. Falling back to registration reference.");
        }
    }

    // 1. Create a unique temporary directory to write the aligned FITS frames
    std::string tempDir = TempDirectory::createTempDir("aligned");
    if (tempDir.empty()) {
        throw std::runtime_error("Failed to create temporary directory for aligned frames");
    }

    std::vector<std::string> names(count);
    std::vector<std::string> filepaths(count);
    std::vector<FrameMetadata> finalMetadata(count);

    // 2. Warp each frame sequentially and write to disk
    for (int i = 0; i < count; ++i) {
        if (PreprocessingPipeline::isCancelled()) {
            throw std::runtime_error("Preprocessing cancelled by user.");
        }

        if (progress) {
            progress(static_cast<int>(100.0 * i / count));
        }

        // We only warp selected frames.
        if (!batch->isFrameSelected(i)) {
            names[i] = batch->frameName(i) + "_unselected";
            filepaths[i] = batch->frameFilepath(i);
            finalMetadata[i] = batch->frameMetadata(i);
            continue;
        }

        FrameMetadata meta = batch->frameMetadata(i);
        if (!meta.registered) {
            Logger::warning("Align", QString("Frame %1 (%2) is selected but not registered. Skipping.")
                            .arg(i).arg(QString::fromStdString(batch->frameName(i))));
            names[i] = batch->frameName(i) + "_unregistered";
            filepaths[i] = batch->frameFilepath(i);
            finalMetadata[i] = meta;
            continue;
        }

        ImageVariant frame = batch->getImage(i);
        ImageVariant alignedFrame;

        // Adjust offsets if a centermost reference frame is selected
        double targetDx = meta.dx;
        double targetDy = meta.dy;
        double targetTheta = meta.theta;

        if (centerFrameIdx != -1) {
            double cosRef = std::cos(refTheta);
            double sinRef = std::sin(refTheta);
            double diffDx = meta.dx - refDx;
            double diffDy = meta.dy - refDy;

            targetTheta = meta.theta - refTheta;
            targetDx = diffDx * cosRef + diffDy * sinRef;
            targetDy = -diffDx * sinRef + diffDy * cosRef;
        }

        // Perform warping (backward-mapping bilinear interpolation, applying drizzle)
        if (std::holds_alternative<GrayscaleImagePtr>(frame)) {
            auto gray = std::get<GrayscaleImagePtr>(frame);
            alignedFrame = Warping::warpGrayscale(gray, targetDx, targetDy, targetTheta, drizzleScale);
        } else if (std::holds_alternative<RGBImagePtr>(frame)) {
            auto rgb = std::get<RGBImagePtr>(frame);
            alignedFrame = Warping::warpRGB(rgb, targetDx, targetDy, targetTheta, drizzleScale);
        }

        // Save aligned frame to a temporary FITS file
        std::string origPath = batch->frameFilepath(i);
        std::string outFilename = TempDirectory::getIntermediateFileName(origPath, "_aligned", i);
        std::string fullOutPath = tempDir + "/" + outFilename;

        FitsIO writer;
        if (!writer.writeImage(fullOutPath, alignedFrame)) {
            throw std::runtime_error("Failed to write temporary aligned frame to " + fullOutPath);
        }

        names[i] = batch->frameName(i) + "_aligned";
        filepaths[i] = fullOutPath;
        
        FrameMetadata finalMeta = meta;
        finalMeta.dx = targetDx;
        finalMeta.dy = targetDy;
        finalMeta.theta = targetTheta;
        finalMetadata[i] = finalMeta;

        // Memory optimization: evict the loaded source frame from RAM
        if (evictCache) {
            batch->clearCache(i);
        }
    }

    // 3. Create a new disk-backed ImageBatch pointing to the aligned FITS files
    auto alignedBatch = std::make_shared<ImageBatch>(
        count,
        [filepaths](int index) -> ImageVariant {
            FitsIO reader;
            return reader.readImage(filepaths[index]);
        },
        names,
        filepaths
    );

    // Copy over the metadata to the new aligned batch
    for (int i = 0; i < count; ++i) {
        alignedBatch->setFrameMetadata(i, finalMetadata[i]);
        alignedBatch->setFrameSelected(i, batch->isFrameSelected(i));
    }

    if (progress) progress(100);
    
    if (workspace.contains(outputName)) {
        workspace.unregisterElement(outputName);
    }
    workspace.registerElement(outputName, alignedBatch);
    Logger::success("Align", QString("Finished alignment. Aligned %1 frames in %2 ms (avg %3 ms per frame). Registered output batch: %4")
                    .arg(count).arg(totalTimer.elapsed()).arg(totalTimer.elapsed() / (count > 0 ? count : 1))
                    .arg(QString::fromStdString(outputName)));
}

} // namespace blastro
