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

#include "CalibrationAlgorithm.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include "core/ImageBuffer.h"
#include "core/TempDirectory.h"
#include "io/FitsIO.h"
#include <cmath>
#include <stdexcept>
#include <vector>
#include <atomic>
#include <mutex>
#include "core/Logger.h"
#include <QElapsedTimer>
#include <omp.h>
#include "core/Preferences.h"
#include "PreprocessingPipeline.h"

namespace blastro {

static GrayscaleImagePtr calibrateChannel(GrayscaleImagePtr input,
                                         GrayscaleImagePtr bias,
                                         GrayscaleImagePtr dark,
                                         GrayscaleImagePtr flat,
                                         float flatMean = 0.0f,
                                         std::function<void(int)> progressSub = nullptr) {
    if (!input) {
        throw std::runtime_error("Input channel is null");
    }

    int w = input->width();
    int h = input->height();

    // Verify dimension consistency for any provided calibration frames
    if (bias && (bias->width() != w || bias->height() != h)) {
        throw std::runtime_error("Bias image dimensions do not match the input image");
    }
    if (dark && (dark->width() != w || dark->height() != h)) {
        throw std::runtime_error("Dark image dimensions do not match the input image");
    }
    if (flat && (flat->width() != w || flat->height() != h)) {
        throw std::runtime_error("Flat image dimensions do not match the input image");
    }

    auto outBuffer = std::make_shared<ImageBuffer>(w, h);
    float* outData = outBuffer->data();
    const float* inData = input->buffer()->data();

    const float* biasData = bias ? bias->buffer()->data() : nullptr;
    const float* darkData = dark ? dark->buffer()->data() : nullptr;
    const float* flatData = flat ? flat->buffer()->data() : nullptr;

    // Calculate flat mean for normalization if not pre-calculated
    if (flatData && flatMean <= 0.0f) {
        double sum = 0.0;
        int count = 0;
        int numPixels = w * h;
        #pragma omp parallel for reduction(+:sum, count)
        for (int i = 0; i < numPixels; ++i) {
            float val = flatData[i];
            if (!std::isnan(val)) {
                sum += val;
                count++;
            }
        }
        flatMean = (count > 0) ? static_cast<float>(sum / count) : 1.0f;
        if (std::abs(flatMean) < 1e-6f) {
            flatMean = 1.0f; // Prevent division-by-zero during normalization
        }
    }

    float invFlatMean = 1.0f;
    if (flatData) {
        invFlatMean = 1.0f / flatMean;
    }

    int numPixels = w * h;
    std::atomic<int> processed(0);
    int lastPercent = 0;
    std::mutex progressMutex;

    #pragma omp parallel for
    for (int i = 0; i < numPixels; ++i) {
        float val = inData[i];
        if (darkData) val -= darkData[i];
        if (biasData) val -= biasData[i];

        if (flatData) {
            // Apply normalized flat: Calibrated = (Input - Dark - Bias) / (Flat / FlatMean)
            float fVal = flatData[i] * invFlatMean;
            if (std::abs(fVal) < 1e-6f) {
                outData[i] = 0.0f; // Division-by-zero safety
            } else {
                outData[i] = val / fVal;
            }
        } else {
            outData[i] = val;
        }

        // Thread-safe, throttled progress updates in 2% steps (every 1/50th of total pixels)
        if (progressSub) {
            int localProcessed = ++processed;
            if (localProcessed % (numPixels / 50) == 0 || localProcessed == numPixels) {
                int percent = static_cast<int>(100.0 * localProcessed / numPixels);
                std::lock_guard<std::mutex> lock(progressMutex);
                if (percent > lastPercent) {
                    lastPercent = percent;
                    progressSub(percent);
                }
            }
        }
    }

    // Ensure final 100% progress update is sent
    if (progressSub && lastPercent < 100) {
        progressSub(100);
    }

    return std::make_shared<GrayscaleImage>(outBuffer);
}

void CalibrationAlgorithm::execute(WorkspaceRegistry& workspace, 
                                   const std::map<std::string, std::string>& config, 
                                   ProgressCallback progress) {
    QElapsedTimer totalTimer;
    totalTimer.start();

    std::string inputName = config.at("input_name");
    std::string outputName = config.at("output_name");
    
    std::string biasName = config.at("bias_name");
    std::string darkName = config.at("dark_name");
    std::string flatName = config.at("flat_name");

    int threads = config.count("threads") ? std::stoi(config.at("threads")) : -1;
    if (threads <= 0) {
        threads = Preferences::instance().getThreadCount();
    }
    if (threads > 0) {
        omp_set_num_threads(threads);
    }

    Logger::header("Calibration", QString("Starting execution. Input: %1, output: %2, threads: %3")
                   .arg(QString::fromStdString(inputName))
                   .arg(QString::fromStdString(outputName))
                   .arg(threads));
    Logger::info("Calibration", QString("Calibration frames - Bias: %1, Dark: %2, Flat: %3")
                 .arg(biasName.empty() ? "<None>" : QString::fromStdString(biasName))
                 .arg(darkName.empty() ? "<None>" : QString::fromStdString(darkName))
                 .arg(flatName.empty() ? "<None>" : QString::fromStdString(flatName)));

    WorkspaceElement inputElem = workspace.getElement(inputName);

    // 1. Resolve optional Bias frame (can be Grayscale or RGB)
    GrayscaleImagePtr biasR = nullptr, biasG = nullptr, biasB = nullptr;
    if (!biasName.empty() && workspace.contains(biasName)) {
        WorkspaceElement elem = workspace.getElement(biasName);
        if (std::holds_alternative<GrayscaleImagePtr>(elem)) {
            auto img = std::get<GrayscaleImagePtr>(elem);
            biasR = biasG = biasB = img;
        } else if (std::holds_alternative<RGBImagePtr>(elem)) {
            auto img = std::get<RGBImagePtr>(elem);
            biasR = img->r();
            biasG = img->g();
            biasB = img->b();
        }
    }

    // 2. Resolve optional Dark frame
    GrayscaleImagePtr darkR = nullptr, darkG = nullptr, darkB = nullptr;
    if (!darkName.empty() && workspace.contains(darkName)) {
        WorkspaceElement elem = workspace.getElement(darkName);
        if (std::holds_alternative<GrayscaleImagePtr>(elem)) {
            auto img = std::get<GrayscaleImagePtr>(elem);
            darkR = darkG = darkB = img;
        } else if (std::holds_alternative<RGBImagePtr>(elem)) {
            auto img = std::get<RGBImagePtr>(elem);
            darkR = img->r();
            darkG = img->g();
            darkB = img->b();
        }
    }

    // 3. Resolve optional Flat frame and pre-calculate channel means once
    GrayscaleImagePtr flatR = nullptr, flatG = nullptr, flatB = nullptr;
    float flatMeanR = 0.0f, flatMeanG = 0.0f, flatMeanB = 0.0f;
    if (!flatName.empty() && workspace.contains(flatName)) {
        WorkspaceElement elem = workspace.getElement(flatName);
        if (std::holds_alternative<GrayscaleImagePtr>(elem)) {
            auto img = std::get<GrayscaleImagePtr>(elem);
            flatR = flatG = flatB = img;
        } else if (std::holds_alternative<RGBImagePtr>(elem)) {
            auto img = std::get<RGBImagePtr>(elem);
            flatR = img->r();
            flatG = img->g();
            flatB = img->b();
        }

        auto calcMean = [](GrayscaleImagePtr flatImg, const QString& channelLabel) -> float {
            if (!flatImg) return 1.0f;
            QElapsedTimer meanTimer;
            meanTimer.start();
            int w = flatImg->width();
            int h = flatImg->height();
            int numPixels = w * h;
            const float* flatData = flatImg->buffer()->data();
            double sum = 0.0;
            int count = 0;
            #pragma omp parallel for reduction(+:sum, count)
            for (int i = 0; i < numPixels; ++i) {
                float val = flatData[i];
                if (!std::isnan(val)) {
                    sum += val;
                    count++;
                }
            }
            float mean = (count > 0) ? static_cast<float>(sum / count) : 1.0f;
            mean = std::abs(mean) < 1e-6f ? 1.0f : mean;
            Logger::info("Calibration", QString("Calculated master flat %1 mean: %2 (took %3 ms)")
                         .arg(channelLabel).arg(mean).arg(meanTimer.elapsed()));
            return mean;
        };

        flatMeanR = calcMean(flatR, "Red");
        flatMeanG = calcMean(flatG, "Green");
        flatMeanB = calcMean(flatB, "Blue");
    }

    // 4. Perform Calibration based on input type
    if (std::holds_alternative<ImageBatchPtr>(inputElem)) {
        auto inputBatch = std::get<ImageBatchPtr>(inputElem);
        int count = inputBatch->count();
        Logger::info("Calibration", QString("Calibrating batch of %1 frames...").arg(count));
        
        std::string tempDir = TempDirectory::createTempDir("calibrated");
        if (tempDir.empty()) {
            throw std::runtime_error("Failed to create intermediate directory for calibration");
        }

        std::vector<std::string> names(count);
        std::vector<std::string> filepaths(count);
        
        // Dynamically estimate memory usage per frame
        ImageVariant firstFrame = inputBatch->getImage(0);
        int channels = std::holds_alternative<RGBImagePtr>(firstFrame) ? 3 : 1;
        int frameW = 0, frameH = 0;
        if (channels == 3) {
            auto rgb = std::get<RGBImagePtr>(firstFrame);
            frameW = rgb->width();
            frameH = rgb->height();
        } else {
            auto gray = std::get<GrayscaleImagePtr>(firstFrame);
            frameW = gray->width();
            frameH = gray->height();
        }
        inputBatch->clearCache(0); // Evict first frame immediately

        double memPerFrameBytes = 2.0 * channels * frameW * frameH * 4.0; // Input + Output
        double maxRamBytes = Preferences::instance().getMaxRamUsage() * 1024.0 * 1024.0 * 1024.0;
        int maxParallelFrames = static_cast<int>(maxRamBytes / memPerFrameBytes);
        if (maxParallelFrames < 1) maxParallelFrames = 1;
        int parallelFrames = std::min({maxParallelFrames, count, threads});

        Logger::info("Calibration", QString("Parallel frame calibration: using %1 threads based on RAM limit of %2 GB (requires %3 MB per frame)")
                     .arg(parallelFrames)
                     .arg(Preferences::instance().getMaxRamUsage())
                     .arg(static_cast<int>(memPerFrameBytes / (1024.0 * 1024.0))));

        std::atomic<int> completedFrames(0);
        std::mutex progressMutex;

        bool cancelled = false;
        #pragma omp parallel for num_threads(parallelFrames) schedule(dynamic) shared(cancelled)
        for (int i = 0; i < count; ++i) {
            if (cancelled) continue;

            if (PreprocessingPipeline::isCancelled()) {
                cancelled = true;
                continue;
            }

            QElapsedTimer frameTimer;
            frameTimer.start();
            
            ImageVariant frame = inputBatch->getImage(i);
            ImageVariant calibratedFrame;

            bool frameRGB = std::holds_alternative<RGBImagePtr>(frame);
            if (frameRGB) {
                auto rgb = std::get<RGBImagePtr>(frame);
                auto calR = calibrateChannel(rgb->r(), biasR, darkR, flatR, flatMeanR);
                auto calG = calibrateChannel(rgb->g(), biasG, darkG, flatG, flatMeanG);
                auto calB = calibrateChannel(rgb->b(), biasB, darkB, flatB, flatMeanB);
                auto calRgb = std::make_shared<RGBImage>(calR, calG, calB);
                calRgb->setMetadata(rgb->metadata());
                calibratedFrame = calRgb;
            } else {
                auto gray = std::get<GrayscaleImagePtr>(frame);
                auto calGray = calibrateChannel(gray, biasR, darkR, flatR, flatMeanR);
                calGray->setMetadata(gray->metadata());
                calibratedFrame = calGray;
            }

            // Generate intermediate filename: e.g. light_001_calibrated.fits
            std::string origPath = inputBatch->frameFilepath(i);
            std::string outFilename = TempDirectory::getIntermediateFileName(origPath, "_calibrated", i);
            std::string fullOutPath = tempDir + "/" + outFilename;

            FitsIO writer;
            if (!writer.writeImage(fullOutPath, calibratedFrame)) {
                Logger::error("Calibration", QString("Failed to write intermediate calibrated frame to %1").arg(QString::fromStdString(fullOutPath)));
            }

            names[i] = inputBatch->frameName(i) + "_calibrated";
            filepaths[i] = fullOutPath;

            // Evict frame from RAM to keep memory usage flat
            inputBatch->clearCache(i);

            Logger::info("Calibration", QString("Frame %1/%2 (%3) calibrated and written to disk in %4 ms")
                         .arg(i + 1).arg(count)
                         .arg(QString::fromStdString(inputBatch->frameName(i)))
                         .arg(frameTimer.elapsed()));

            int done = ++completedFrames;
            if (progress) {
                std::lock_guard<std::mutex> lock(progressMutex);
                progress(static_cast<int>(100.0 * done / count));
            }
        }

        if (cancelled || PreprocessingPipeline::isCancelled()) {
            throw std::runtime_error("Preprocessing cancelled by user.");
        }

        // Return a disk-backed ImageBatch pointing to the temporary FITS files
        auto calibBatch = std::make_shared<ImageBatch>(
            count,
            [filepaths](int index) -> ImageVariant {
                FitsIO reader;
                return reader.readImage(filepaths[index]);
            },
            names,
            filepaths
        );

        for (int i = 0; i < count; ++i) {
            calibBatch->setFrameSelected(i, inputBatch->isFrameSelected(i));
            calibBatch->setFrameMetadata(i, inputBatch->frameMetadata(i));
        }

        bool visible = config.count("visible") ? (config.at("visible") == "true") : true;
        if (progress) progress(100);
        workspace.registerElement(outputName, calibBatch, visible);
        Logger::success("Calibration", QString("Batch calibration of %1 frames completed in %2 ms (avg %3 ms per frame)")
                        .arg(count).arg(totalTimer.elapsed()).arg(totalTimer.elapsed() / (count > 0 ? count : 1)));
    } else {
        // Single image calibration
        Logger::info("Calibration", "Calibrating single image...");
        bool visible = config.count("visible") ? (config.at("visible") == "true") : true;
        bool isRGB = std::holds_alternative<RGBImagePtr>(inputElem);
        if (isRGB) {
            auto rgb = std::get<RGBImagePtr>(inputElem);
            auto calR = calibrateChannel(rgb->r(), biasR, darkR, flatR, flatMeanR, [progress](int p) { if (progress) progress(p / 3); });
            auto calG = calibrateChannel(rgb->g(), biasG, darkG, flatG, flatMeanG, [progress](int p) { if (progress) progress(33 + p / 3); });
            auto calB = calibrateChannel(rgb->b(), biasB, darkB, flatB, flatMeanB, [progress](int p) { if (progress) progress(66 + p / 3); });
            auto calRGB = std::make_shared<RGBImage>(calR, calG, calB);
            calRGB->setMetadata(rgb->metadata());
            workspace.registerElement(outputName, calRGB, visible);
        } else {
            auto gray = std::get<GrayscaleImagePtr>(inputElem);
            auto calGray = calibrateChannel(gray, biasR, darkR, flatR, flatMeanR, progress);
            calGray->setMetadata(gray->metadata());
            workspace.registerElement(outputName, calGray, visible);
        }
        Logger::success("Calibration", QString("Single image calibration completed in %1 ms").arg(totalTimer.elapsed()));
    }
    Logger::success("Calibration", QString("Finished calibration. Registered output: %1")
                    .arg(QString::fromStdString(outputName)));
}

} // namespace blastro
