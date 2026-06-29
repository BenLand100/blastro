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

#include "DebayerAlgorithm.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include "core/TempDirectory.h"
#include "io/FitsIO.h"
#include <cmath>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <QFileInfo>
#include "core/Logger.h"
#include <omp.h>
#include <QElapsedTimer>
#include "core/Preferences.h"
#include "PreprocessingPipeline.h"

namespace blastro {

// Helper to perform debayering on a single grayscale image
static RGBImagePtr debayerFrame(GrayscaleImagePtr src, const std::string& pattern, const std::string& method, bool greenEqualize) {
    if (!src) return nullptr;

    int w = src->width();
    int h = src->height();
    const float* inData = src->buffer()->data();

    // Determine offsets based on Bayer pattern
    // RGGB offsets (default):
    // R: (0,0), G1: (1,0), G2: (0,1), B: (1,1)
    int rOffsetX = 0, rOffsetY = 0;
    int g1OffsetX = 1, g1OffsetY = 0;
    int g2OffsetX = 0, g2OffsetY = 1;
    int bOffsetX = 1, bOffsetY = 1;

    if (pattern == "BGGR") {
        rOffsetX = 1; rOffsetY = 1;
        g1OffsetX = 0; g1OffsetY = 1;
        g2OffsetX = 1; g2OffsetY = 0;
        bOffsetX = 0; bOffsetY = 0;
    } else if (pattern == "GBRG") {
        rOffsetX = 0; rOffsetY = 1;
        g1OffsetX = 0; g1OffsetY = 0;
        g2OffsetX = 1; g2OffsetY = 1;
        bOffsetX = 1; bOffsetY = 0;
    } else if (pattern == "GRBG") {
        rOffsetX = 1; rOffsetY = 0;
        g1OffsetX = 0; g1OffsetY = 0;
        g2OffsetX = 1; g2OffsetY = 1;
        bOffsetX = 0; bOffsetY = 1;
    }

    if (method == "superpixel") {
        // Superpixel method: halves resolution
        int outW = w / 2;
        int outH = h / 2;
        auto outRGB = std::make_shared<RGBImage>(outW, outH);
        
        float* rData = outRGB->r()->buffer()->data();
        float* gData = outRGB->g()->buffer()->data();
        float* bData = outRGB->b()->buffer()->data();

        #pragma omp parallel for collapse(2)
        for (int y = 0; y < outH; ++y) {
            for (int x = 0; x < outW; ++x) {
                int inY0 = 2 * y;
                int inX0 = 2 * x;

                float rVal = inData[(inY0 + rOffsetY) * w + (inX0 + rOffsetX)];
                float g1Val = inData[(inY0 + g1OffsetY) * w + (inX0 + g1OffsetX)];
                float g2Val = inData[(inY0 + g2OffsetY) * w + (inX0 + g2OffsetX)];
                float bVal = inData[(inY0 + bOffsetY) * w + (inX0 + bOffsetX)];

                int outIdx = y * outW + x;
                rData[outIdx] = rVal;
                gData[outIdx] = greenEqualize ? (g1Val + g2Val) * 0.5f : g1Val; // standard average
                bData[outIdx] = bVal;
            }
        }
        return outRGB;
    } else {
        // Bilinear interpolation: preserves full resolution
        auto outRGB = std::make_shared<RGBImage>(w, h);
        float* rData = outRGB->r()->buffer()->data();
        float* gData = outRGB->g()->buffer()->data();
        float* bData = outRGB->b()->buffer()->data();

        auto getPixelSafe = [inData, w, h](int px, int py) -> float {
            int cx = std::clamp(px, 0, w - 1);
            int cy = std::clamp(py, 0, h - 1);
            return inData[cy * w + cx];
        };

        #pragma omp parallel for collapse(2)
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int pxMode = x % 2;
                int pyMode = y % 2;

                float rVal = 0.0f, gVal = 0.0f, bVal = 0.0f;

                // Determine pixel type based on coordinate and offsets
                bool isR = (pxMode == rOffsetX && pyMode == rOffsetY);
                bool isB = (pxMode == bOffsetX && pyMode == bOffsetY);
                bool isG1 = (pxMode == g1OffsetX && pyMode == g1OffsetY);
                bool isG2 = (pxMode == g2OffsetX && pyMode == g2OffsetY);

                if (isR) {
                    rVal = getPixelSafe(x, y);
                    gVal = (getPixelSafe(x-1, y) + getPixelSafe(x+1, y) + getPixelSafe(x, y-1) + getPixelSafe(x, y+1)) * 0.25f;
                    bVal = (getPixelSafe(x-1, y-1) + getPixelSafe(x+1, y-1) + getPixelSafe(x-1, y+1) + getPixelSafe(x+1, y+1)) * 0.25f;
                } else if (isB) {
                    rVal = (getPixelSafe(x-1, y-1) + getPixelSafe(x+1, y-1) + getPixelSafe(x-1, y+1) + getPixelSafe(x+1, y+1)) * 0.25f;
                    gVal = (getPixelSafe(x-1, y) + getPixelSafe(x+1, y) + getPixelSafe(x, y-1) + getPixelSafe(x, y+1)) * 0.25f;
                    bVal = getPixelSafe(x, y);
                } else if (isG1 || isG2) {
                    gVal = getPixelSafe(x, y);

                    // For G1, R is horizontal neighbors, B is vertical (or vice versa depending on pattern)
                    // Let's determine neighbors dynamically:
                    // If R is adjacent horizontally, then B is vertical
                    bool rIsHorizontal = (g1OffsetX == 1 && rOffsetX == 0) || (g1OffsetX == 0 && rOffsetX == 1);
                    if ((isG1 && rIsHorizontal) || (isG2 && !rIsHorizontal)) {
                        rVal = (getPixelSafe(x-1, y) + getPixelSafe(x+1, y)) * 0.5f;
                        bVal = (getPixelSafe(x, y-1) + getPixelSafe(x, y+1)) * 0.5f;
                    } else {
                        rVal = (getPixelSafe(x, y-1) + getPixelSafe(x, y+1)) * 0.5f;
                        bVal = (getPixelSafe(x-1, y) + getPixelSafe(x+1, y)) * 0.5f;
                    }
                }

                int outIdx = y * w + x;
                rData[outIdx] = rVal;
                gData[outIdx] = gVal;
                bData[outIdx] = bVal;
            }
        }
        return outRGB;
    }
}

void DebayerAlgorithm::execute(WorkspaceRegistry& workspace, 
                               const std::map<std::string, std::string>& config, 
                               ProgressCallback progress) {
    std::string inputName = config.at("input_name");
    std::string outputName = config.at("output_name");
    std::string pattern = config.at("pattern");
    std::string method = config.at("method");
    bool greenEqualize = config.at("green_equalize") == "true";

    int threads = config.count("threads") ? std::stoi(config.at("threads")) : -1;
    if (threads <= 0) {
        threads = Preferences::instance().getThreadCount();
    }
    if (threads > 0) {
        omp_set_num_threads(threads);
    }

    Logger::header("Debayer", QString("Starting execution. Input: %1, output: %2, pattern: %3, method: %4, green equalize: %5, threads: %6")
                   .arg(QString::fromStdString(inputName))
                   .arg(QString::fromStdString(outputName))
                   .arg(QString::fromStdString(pattern))
                   .arg(QString::fromStdString(method))
                   .arg(greenEqualize ? "true" : "false")
                   .arg(threads));

    QElapsedTimer totalTimer;
    totalTimer.start();

    WorkspaceElement inputElem = workspace.getElement(inputName);

    if (std::holds_alternative<ImageBatchPtr>(inputElem)) {
        auto inputBatch = std::get<ImageBatchPtr>(inputElem);
        int count = inputBatch->count();

        // 1. Create a unique temporary directory to save debayered files
        std::string tempDir = TempDirectory::createTempDir("debayered");
        if (tempDir.empty()) {
            throw std::runtime_error("Failed to create temporary directory for debayered frames");
        }

        std::vector<std::string> names(count);
        std::vector<std::string> filepaths(count);

        // 2. Debayer each frame sequentially and save to disk
        for (int i = 0; i < count; ++i) {
            if (PreprocessingPipeline::isCancelled()) {
                throw std::runtime_error("Preprocessing cancelled by user.");
            }

            if (progress) {
                progress(static_cast<int>(100.0 * i / count));
            }

            ImageVariant frame = inputBatch->getImage(i);
            GrayscaleImagePtr grayFrame = nullptr;
            if (std::holds_alternative<GrayscaleImagePtr>(frame)) {
                grayFrame = std::get<GrayscaleImagePtr>(frame);
            } else {
                throw std::runtime_error("Debayer input frame is not a Grayscale image");
            }

            auto debFrame = debayerFrame(grayFrame, pattern, method, greenEqualize);
            
            std::string origPath = inputBatch->frameFilepath(i);
            std::string outFilename = TempDirectory::getIntermediateFileName(origPath, "_debayered", i);
            std::string fullOutPath = tempDir + "/" + outFilename;

            FitsIO writer;
            if (!writer.writeImage(fullOutPath, debFrame)) {
                throw std::runtime_error("Failed to write temporary debayered frame to " + fullOutPath);
            }

            names[i] = inputBatch->frameName(i) + "_debayered";
            filepaths[i] = fullOutPath;
        }

        // 3. Return a disk-backed ImageBatch pointing to the temporary FITS files
        auto debayeredBatch = std::make_shared<ImageBatch>(
            count,
            [filepaths](int index) -> ImageVariant {
                FitsIO reader;
                return reader.readImage(filepaths[index]);
            },
            names,
            filepaths
        );

        for (int i = 0; i < count; ++i) {
            debayeredBatch->setFrameSelected(i, inputBatch->isFrameSelected(i));
            debayeredBatch->setFrameMetadata(i, inputBatch->frameMetadata(i));
        }

        if (progress) progress(100);
        workspace.registerElement(outputName, debayeredBatch);
        Logger::success("Debayer", QString("Finished batch debayering (%1 frames) in %2 ms (avg %3 ms per frame). Registered output: %4")
                        .arg(count).arg(totalTimer.elapsed()).arg(totalTimer.elapsed() / (count > 0 ? count : 1))
                        .arg(QString::fromStdString(outputName)));
    } else {
        // Single image debayering
        GrayscaleImagePtr gray = nullptr;
        if (std::holds_alternative<GrayscaleImagePtr>(inputElem)) {
            gray = std::get<GrayscaleImagePtr>(inputElem);
        } else {
            throw std::runtime_error("Debayer input is not a Grayscale image");
        }

        if (progress) progress(20);
        auto debRGB = debayerFrame(gray, pattern, method, greenEqualize);
        if (progress) progress(80);
        
        if (workspace.contains(outputName)) {
            workspace.unregisterElement(outputName);
        }
        workspace.registerElement(outputName, debRGB);
        if (progress) progress(100);
    }
    Logger::success("Debayer", QString("Finished debayering in %1 ms. Registered output: %2")
                    .arg(totalTimer.elapsed()).arg(QString::fromStdString(outputName)));
}

} // namespace blastro
