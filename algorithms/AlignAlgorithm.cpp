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

static std::array<double, 6> invertAffine(const std::array<double, 6>& T) {
    double a = T[0], b = T[1], tx = T[2];
    double c = T[3], d = T[4], ty = T[5];
    double det = a * d - b * c;
    if (std::abs(det) < 1e-9) {
        return {1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
    }
    double inv_det = 1.0 / det;
    double inv_a = d * inv_det;
    double inv_b = -b * inv_det;
    double inv_tx = (b * ty - d * tx) * inv_det;
    double inv_c = -c * inv_det;
    double inv_d = a * inv_det;
    double inv_ty = (c * tx - a * ty) * inv_det;
    return {inv_a, inv_b, inv_tx, inv_c, inv_d, inv_ty};
}

static std::array<double, 6> multiplyAffine(const std::array<double, 6>& A, const std::array<double, 6>& B) {
    double a  = A[0]*B[0] + A[1]*B[3];
    double b  = A[0]*B[1] + A[1]*B[4];
    double tx = A[0]*B[2] + A[1]*B[5] + A[2];
    
    double c  = A[3]*B[0] + A[4]*B[3];
    double d  = A[3]*B[1] + A[4]*B[4];
    double ty = A[3]*B[2] + A[4]*B[5] + A[5];
    
    return {a, b, tx, c, d, ty};
}

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
    std::string interpolation = config.count("interpolation_method") ? config.at("interpolation_method") : "bilinear";

    Logger::header("Align", QString("Starting execution. Input batch: %1, output batch: %2, drizzle scale: %3, threads: %4, interpolation: %5")
                   .arg(QString::fromStdString(inputName))
                   .arg(QString::fromStdString(outputName))
                   .arg(drizzleScale).arg(threads).arg(QString::fromStdString(interpolation)));

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
    std::array<double, 6> T_ref = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
    bool hasRef = false;

    if (config.count("ref_dx") && config.count("ref_dy") && config.count("ref_theta")) {
        refDx = std::stod(config.at("ref_dx"));
        refDy = std::stod(config.at("ref_dy"));
        refTheta = std::stod(config.at("ref_theta"));
        double cosRef = std::cos(refTheta);
        double sinRef = std::sin(refTheta);
        T_ref = {cosRef, -sinRef, refDx, sinRef, cosRef, refDy};
        hasRef = true;
        Logger::info("Align", QString("Using custom reference offsets: dx=%1, dy=%2, theta=%3")
                     .arg(refDx).arg(refDy).arg(refTheta));
    } else if (refMode == "average_center") {
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
            T_ref = centerMeta.transform;
            hasRef = true;
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

        // Adjust offsets if a reference frame is selected
        std::array<double, 6> targetTransform = meta.transform;

        if (hasRef) {
            std::array<double, 6> invRef = invertAffine(T_ref);
            targetTransform = multiplyAffine(invRef, meta.transform);
        }

        double targetDx = targetTransform[2];
        double targetDy = targetTransform[5];
        double targetTheta = std::atan2(targetTransform[3] - targetTransform[1], targetTransform[0] + targetTransform[4]);

        // Perform warping (backward-mapping interpolation, applying drizzle)
        if (std::holds_alternative<GrayscaleImagePtr>(frame)) {
            auto gray = std::get<GrayscaleImagePtr>(frame);
            if (interpolation == "drizzle") {
                double dropShrink = config.count("drop_shrink") ? std::stod(config.at("drop_shrink")) : 1.0;
                auto drizzleResult = Warping::warpDrizzleGrayscale(gray, targetTransform, drizzleScale, dropShrink);
                auto dataImg = drizzleResult.first;
                auto weightImg = drizzleResult.second;
                if (dataImg && weightImg) {
                    dataImg->setMetadata(gray->metadata());
                    weightImg->setMetadata(gray->metadata());
                    
                    std::string origPath = batch->frameFilepath(i);
                    std::string outFilename = TempDirectory::getIntermediateFileName(origPath, "_aligned", i);
                    std::string fullOutPath = tempDir + "/" + outFilename;

                    FitsIO writer;
                    if (!writer.writeDrizzleImage(fullOutPath, dataImg, weightImg)) {
                        throw std::runtime_error("Failed to write temporary aligned drizzle frame to " + fullOutPath);
                    }
                    
                    names[i] = batch->frameName(i) + "_aligned";
                    filepaths[i] = fullOutPath;
                    
                    FrameMetadata finalMeta = meta;
                    finalMeta.dx = targetDx;
                    finalMeta.dy = targetDy;
                    finalMeta.theta = targetTheta;
                    finalMeta.transform = targetTransform;
                    finalMetadata[i] = finalMeta;

                    if (evictCache) {
                        batch->clearCache(i);
                    }
                    continue;
                }
            } else {
                auto warped = Warping::warpGrayscale(gray, targetTransform, drizzleScale, interpolation);
                warped->setMetadata(gray->metadata());
                alignedFrame = warped;
            }
        } else if (std::holds_alternative<RGBImagePtr>(frame)) {
            auto rgb = std::get<RGBImagePtr>(frame);
            if (interpolation == "drizzle") {
                double dropShrink = config.count("drop_shrink") ? std::stod(config.at("drop_shrink")) : 1.0;
                
                auto rResult = Warping::warpDrizzleGrayscale(rgb->r(), targetTransform, drizzleScale, dropShrink);
                auto gResult = Warping::warpDrizzleGrayscale(rgb->g(), targetTransform, drizzleScale, dropShrink);
                auto bResult = Warping::warpDrizzleGrayscale(rgb->b(), targetTransform, drizzleScale, dropShrink);
                
                if (rResult.first && gResult.first && bResult.first) {
                    std::string origPath = batch->frameFilepath(i);
                    std::string outFilename = TempDirectory::getIntermediateFileName(origPath, "_aligned", i);
                    std::string fullOutPath = tempDir + "/" + outFilename;

                    FitsIO writer;
                    if (!writer.writeDrizzleRGBImage(fullOutPath, 
                            rResult.first, rResult.second,
                            gResult.first, gResult.second,
                            bResult.first, bResult.second)) {
                        throw std::runtime_error("Failed to write temporary aligned RGB drizzle frame to " + fullOutPath);
                    }
                    
                    names[i] = batch->frameName(i) + "_aligned";
                    filepaths[i] = fullOutPath;
                    
                    FrameMetadata finalMeta = meta;
                    finalMeta.dx = targetDx;
                    finalMeta.dy = targetDy;
                    finalMeta.theta = targetTheta;
                    finalMeta.transform = targetTransform;
                    finalMetadata[i] = finalMeta;

                    if (evictCache) {
                        batch->clearCache(i);
                    }
                    continue;
                }
            } else {
                auto warped = Warping::warpRGB(rgb, targetTransform, drizzleScale, interpolation);
                warped->setMetadata(rgb->metadata());
                alignedFrame = warped;
            }
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
        finalMeta.transform = targetTransform;
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
