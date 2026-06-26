#include "StackingAlgorithm.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include "core/ImageBuffer.h"
#include "core/TempDirectory.h"
#include "io/FitsIO.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <QDebug>

namespace blastro {

static ImageVariant cropImagePatch(const ImageVariant& image, int xStart, int yStart, int patchW, int patchH) {
    if (std::holds_alternative<GrayscaleImagePtr>(image)) {
        auto img = std::get<GrayscaleImagePtr>(image);
        auto patch = std::make_shared<GrayscaleImage>(patchW, patchH);
        auto srcBuf = img->buffer();
        auto dstBuf = patch->buffer();
        for (int y = 0; y < patchH; ++y) {
            for (int x = 0; x < patchW; ++x) {
                dstBuf->setPixel(x, y, srcBuf->pixel(xStart + x, yStart + y));
            }
        }
        return patch;
    } else {
        auto img = std::get<RGBImagePtr>(image);
        auto patchR = std::make_shared<GrayscaleImage>(patchW, patchH);
        auto patchG = std::make_shared<GrayscaleImage>(patchW, patchH);
        auto patchB = std::make_shared<GrayscaleImage>(patchW, patchH);
        auto srcR = img->r()->buffer();
        auto srcG = img->g()->buffer();
        auto srcB = img->b()->buffer();
        auto dstR = patchR->buffer();
        auto dstG = patchG->buffer();
        auto dstB = patchB->buffer();
        for (int y = 0; y < patchH; ++y) {
            for (int x = 0; x < patchW; ++x) {
                dstR->setPixel(x, y, srcR->pixel(xStart + x, yStart + y));
                dstG->setPixel(x, y, srcG->pixel(xStart + x, yStart + y));
                dstB->setPixel(x, y, srcB->pixel(xStart + x, yStart + y));
            }
        }
        return std::make_shared<RGBImage>(patchR, patchG, patchB);
    }
}

static ImageVariant loadPatch(ImageBatchPtr batch, int idx, int xStart, int yStart, int patchW, int patchH, FitsIO& fits) {
    std::string filepath = batch->frameFilepath(idx);
    if (!filepath.empty()) {
        try {
            return fits.readImagePatch(filepath, xStart, yStart, patchW, patchH);
        } catch (const std::exception& e) {
            qWarning() << "[Stacking] Failed to read patch from disk for" << QString::fromStdString(filepath) << ":" << e.what() << ". Falling back to in-memory crop.";
        }
    }
    // Fallback: load full image from batch and crop
    ImageVariant fullImg = batch->getImage(idx);
    return cropImagePatch(fullImg, xStart, yStart, patchW, patchH);
}

static GrayscaleImagePtr stackChannels(const std::vector<GrayscaleImagePtr>& channels,
                                       const std::string& method,
                                       const std::string& rejection,
                                       double sigmaLow, double sigmaHigh,
                                       double quantileLow, double quantileHigh,
                                       std::function<void(int)> progressSub = nullptr) {
    if (channels.empty()) {
        throw std::runtime_error("No channels provided for stacking");
    }

    int numFrames = channels.size();
    int width = channels[0]->width();
    int height = channels[0]->height();
    int numPixels = width * height;

    auto outBuffer = std::make_shared<ImageBuffer>(width, height);
    float* outData = outBuffer->data();

    // Cache raw pointers to channel data for high-performance pixel access
    std::vector<const float*> rawDataPointers(numFrames);
    for (int f = 0; f < numFrames; ++f) {
        rawDataPointers[f] = channels[f]->buffer()->data();
    }

    int progressStep = std::max(1, numPixels / 20);

    #pragma omp parallel
    {
        // Thread-local vectors allocated once per thread to eliminate heap allocation overhead in the hot loop!
        std::vector<float> threadPixelValues(numFrames);
        std::vector<float> threadActiveValues(numFrames);
        std::vector<float> filtered;
        filtered.reserve(numFrames);

        #pragma omp for schedule(static)
        for (int p = 0; p < numPixels; ++p) {
            if (progressSub && (p % progressStep == 0)) {
                #pragma omp critical
                {
                    progressSub(static_cast<int>(100.0 * p / numPixels));
                }
            }

            // Collect non-NaN pixel values across all selected frames
            int n = 0;
            for (int f = 0; f < numFrames; ++f) {
                float val = rawDataPointers[f][p];
                if (!std::isnan(val)) {
                    threadPixelValues[n++] = val;
                }
            }

            threadActiveValues.resize(n);
            for (int i = 0; i < n; ++i) {
                threadActiveValues[i] = threadPixelValues[i];
            }

            // Apply rejection methods
            if (rejection == "quantile" && n > 1) {
                std::sort(threadActiveValues.begin(), threadActiveValues.end());
                int rejectLow = static_cast<int>(n * quantileLow);
                int rejectHigh = static_cast<int>(n * quantileHigh);
                if (rejectLow + rejectHigh >= n) {
                    int mid = n / 2;
                    threadActiveValues = { threadActiveValues[mid] };
                } else {
                    threadActiveValues = std::vector<float>(threadActiveValues.begin() + rejectLow, threadActiveValues.end() - rejectHigh);
                }
            } else if ((rejection == "sigma" || rejection == "winsorized_sigma") && n > 2) {
                // Iterative sigma clipping (2 iterations)
                for (int iter = 0; iter < 2; ++iter) {
                    int currSize = threadActiveValues.size();
                    if (currSize <= 2) break;

                    // Compute mean
                    double sum = 0.0;
                    for (float val : threadActiveValues) sum += val;
                    double mean = sum / currSize;

                    // Compute standard deviation
                    double sumSqDiff = 0.0;
                    for (float val : threadActiveValues) {
                        double diff = val - mean;
                        sumSqDiff += diff * diff;
                    }
                    double stddev = std::sqrt(sumSqDiff / (currSize - 1));
                    if (stddev < 1e-8) break; // Terminate if values are homogeneous

                    double lowBound = mean - sigmaLow * stddev;
                    double highBound = mean + sigmaHigh * stddev;

                    if (rejection == "sigma") {
                        filtered.clear();
                        for (float val : threadActiveValues) {
                            if (val >= lowBound && val <= highBound) {
                                filtered.push_back(val);
                            }
                        }
                        if (filtered.empty()) {
                            break; // Fallback to current set if all would be rejected
                        }
                        threadActiveValues = filtered;
                    } else {
                        // Winsorization: replace outlying values with bounds
                        for (int i = 0; i < currSize; ++i) {
                            if (threadActiveValues[i] < lowBound) {
                                threadActiveValues[i] = static_cast<float>(lowBound);
                            } else if (threadActiveValues[i] > highBound) {
                                threadActiveValues[i] = static_cast<float>(highBound);
                            }
                        }
                    }
                }
            }

            int m = threadActiveValues.size();
            if (m == 0) {
                outData[p] = 0.0f;
                continue;
            }

            // Apply stacking methods
            if (method == "average") {
                double sum = 0.0;
                for (float val : threadActiveValues) sum += val;
                outData[p] = static_cast<float>(sum / m);
            } else if (method == "median") {
                std::sort(threadActiveValues.begin(), threadActiveValues.end());
                if (m % 2 == 1) {
                    outData[p] = threadActiveValues[m / 2];
                } else {
                    outData[p] = (threadActiveValues[m / 2 - 1] + threadActiveValues[m / 2]) * 0.5f;
                }
            } else if (method == "max") {
                outData[p] = *std::max_element(threadActiveValues.begin(), threadActiveValues.end());
            } else if (method == "min") {
                outData[p] = *std::min_element(threadActiveValues.begin(), threadActiveValues.end());
            } else {
                outData[p] = threadActiveValues[0];
            }
        }
    }

    return std::make_shared<GrayscaleImage>(outBuffer);
}

void StackingAlgorithm::execute(WorkspaceRegistry& workspace, 
                                 const std::map<std::string, std::string>& config, 
                                 ProgressCallback progress) {
    std::string inputName = config.at("input_name");
    std::string outputName = config.at("output_name");
    std::string method = config.at("method");
    std::string rejection = config.at("rejection");

    double sigmaLow = std::stod(config.at("sigma_low"));
    double sigmaHigh = std::stod(config.at("sigma_high"));
    double quantileLow = std::stod(config.at("quantile_low"));
    double quantileHigh = std::stod(config.at("quantile_high"));

    std::string mode = config.count("stacking_mode") ? config.at("stacking_mode") : "chunked";
    int patchSize = config.count("patch_size") ? std::stoi(config.at("patch_size")) : 1024;

    WorkspaceElement inputElem = workspace.getElement(inputName);
    if (!std::holds_alternative<ImageBatchPtr>(inputElem)) {
        throw std::runtime_error("Input workspace element is not an image batch");
    }
    ImageBatchPtr batch = std::get<ImageBatchPtr>(inputElem);

    // Filter only selected frames
    std::vector<int> selectedIndices;
    for (int i = 0; i < batch->count(); ++i) {
        if (batch->isFrameSelected(i)) {
            selectedIndices.push_back(i);
        }
    }
    if (selectedIndices.empty()) {
        throw std::runtime_error("No selected frames in the batch to stack");
    }

    int numSelected = selectedIndices.size();

    qInfo() << "[Stacking] Starting execution. Input batch:" << QString::fromStdString(inputName)
            << ", output master:" << QString::fromStdString(outputName)
            << ", method:" << QString::fromStdString(method)
            << ", rejection:" << QString::fromStdString(rejection)
            << ", mode:" << QString::fromStdString(mode);
    qInfo() << "[Stacking] Total frames in batch:" << batch->count() << ", selected frames for stacking:" << numSelected;
    if (rejection == "sigma" || rejection == "winsorized_sigma") {
        qInfo() << "[Stacking] Rejection thresholds: low =" << sigmaLow << ", high =" << sigmaHigh;
    } else if (rejection == "quantile") {
        qInfo() << "[Stacking] Rejection quantiles: low =" << quantileLow << ", high =" << quantileHigh;
    }

    // Check if the first frame is RGB to determine color mode
    ImageVariant firstFrame = batch->getImage(selectedIndices[0]);
    bool isRGB = std::holds_alternative<RGBImagePtr>(firstFrame);
    
    int width = 0;
    int height = 0;
    if (isRGB) {
        auto rgb = std::get<RGBImagePtr>(firstFrame);
        width = rgb->width();
        height = rgb->height();
    } else {
        auto gray = std::get<GrayscaleImagePtr>(firstFrame);
        width = gray->width();
        height = gray->height();
    }

    if (mode == "chunked") {
        std::string tempDir = TempDirectory::createTempDir("stacking");
        if (tempDir.empty()) {
            throw std::runtime_error("Failed to create temporary directory for stacking");
        }
        std::string tempOutPath = tempDir + "/" + outputName + ".fits";

        qInfo() << "[Stacking] Running in 2D Chunked mode with patch size:" << patchSize;
        qInfo() << "[Stacking] Pre-allocating master FITS at:" << QString::fromStdString(tempOutPath);

        // Pre-allocate the master FITS file on disk
        FitsIO fits;
        if (isRGB) {
            auto blankR = std::make_shared<GrayscaleImage>(width, height);
            auto blankG = std::make_shared<GrayscaleImage>(width, height);
            auto blankB = std::make_shared<GrayscaleImage>(width, height);
            auto blankRGB = std::make_shared<RGBImage>(blankR, blankG, blankB);
            if (!fits.writeImage(tempOutPath, blankRGB)) {
                throw std::runtime_error("Failed to initialize blank RGB FITS file at " + tempOutPath);
            }
        } else {
            auto blankGray = std::make_shared<GrayscaleImage>(width, height);
            if (!fits.writeImage(tempOutPath, blankGray)) {
                throw std::runtime_error("Failed to initialize blank Grayscale FITS file at " + tempOutPath);
            }
        }

        int numXPatches = (width + patchSize - 1) / patchSize;
        int numYPatches = (height + patchSize - 1) / patchSize;
        int totalPatches = numXPatches * numYPatches;
        int patchCount = 0;

        for (int py = 0; py < numYPatches; ++py) {
            int yStart = py * patchSize;
            int patchH = std::min(patchSize, height - yStart);

            for (int px = 0; px < numXPatches; ++px) {
                int xStart = px * patchSize;
                int patchW = std::min(patchSize, width - xStart);

                if (progress) {
                    progress(static_cast<int>(100.0 * patchCount / totalPatches));
                }

                if (isRGB) {
                    std::vector<GrayscaleImagePtr> rPatches(numSelected);
                    std::vector<GrayscaleImagePtr> gPatches(numSelected);
                    std::vector<GrayscaleImagePtr> bPatches(numSelected);

                    std::string errorMsg;
                    #pragma omp parallel
                    {
                        FitsIO threadFits;
                        #pragma omp for schedule(dynamic)
                        for (int i = 0; i < numSelected; ++i) {
                            if (!errorMsg.empty()) continue;
                            try {
                                int idx = selectedIndices[i];
                                ImageVariant patchVar = loadPatch(batch, idx, xStart, yStart, patchW, patchH, threadFits);
                                if (!std::holds_alternative<RGBImagePtr>(patchVar)) {
                                    throw std::runtime_error("Expected RGB patch from frame");
                                }
                                auto rgbPatch = std::get<RGBImagePtr>(patchVar);
                                rPatches[i] = rgbPatch->r();
                                gPatches[i] = rgbPatch->g();
                                bPatches[i] = rgbPatch->b();
                            } catch (const std::exception& e) {
                                #pragma omp critical
                                {
                                    if (errorMsg.empty()) {
                                        errorMsg = e.what();
                                    }
                                }
                            }
                        }
                    }

                    if (!errorMsg.empty()) {
                        throw std::runtime_error(errorMsg);
                    }

                    auto stackedR = stackChannels(rPatches, method, rejection, sigmaLow, sigmaHigh, quantileLow, quantileHigh, nullptr);
                    auto stackedG = stackChannels(gPatches, method, rejection, sigmaLow, sigmaHigh, quantileLow, quantileHigh, nullptr);
                    auto stackedB = stackChannels(bPatches, method, rejection, sigmaLow, sigmaHigh, quantileLow, quantileHigh, nullptr);

                    auto stackedRGBPatch = std::make_shared<RGBImage>(stackedR, stackedG, stackedB);
                    if (!fits.writeImagePatch(tempOutPath, stackedRGBPatch, xStart, yStart)) {
                        throw std::runtime_error("Failed to write stacked patch to " + tempOutPath);
                    }
                } else {
                    std::vector<GrayscaleImagePtr> patches(numSelected);

                    std::string errorMsg;
                    #pragma omp parallel
                    {
                        FitsIO threadFits;
                        #pragma omp for schedule(dynamic)
                        for (int i = 0; i < numSelected; ++i) {
                            if (!errorMsg.empty()) continue;
                            try {
                                int idx = selectedIndices[i];
                                ImageVariant patchVar = loadPatch(batch, idx, xStart, yStart, patchW, patchH, threadFits);
                                if (!std::holds_alternative<GrayscaleImagePtr>(patchVar)) {
                                    throw std::runtime_error("Expected Grayscale patch from frame");
                                }
                                patches[i] = std::get<GrayscaleImagePtr>(patchVar);
                            } catch (const std::exception& e) {
                                #pragma omp critical
                                {
                                    if (errorMsg.empty()) {
                                        errorMsg = e.what();
                                    }
                                }
                            }
                        }
                    }

                    if (!errorMsg.empty()) {
                        throw std::runtime_error(errorMsg);
                    }

                    auto stackedPatch = stackChannels(patches, method, rejection, sigmaLow, sigmaHigh, quantileLow, quantileHigh, nullptr);
                    if (!fits.writeImagePatch(tempOutPath, stackedPatch, xStart, yStart)) {
                        throw std::runtime_error("Failed to write stacked patch to " + tempOutPath);
                    }
                }

                patchCount++;
            }
        }

        if (progress) {
            progress(100);
        }

        // Load the completed master back into memory and register it
        qInfo() << "[Stacking] Loading completed master from disk...";
        ImageVariant finalMaster = fits.readImage(tempOutPath);
        WorkspaceElement elem = std::visit([](auto&& arg) -> WorkspaceElement {
            return arg;
        }, finalMaster);
        workspace.registerElement(outputName, elem);
        qInfo() << "[Stacking] Finished chunked stacking. Registered output master:" << QString::fromStdString(outputName);
    } else {
        // Full RAM stacking (original method)
        if (isRGB) {
            // Collect Red, Green, Blue channels independently across all selected frames
            std::vector<GrayscaleImagePtr> rChannels;
            std::vector<GrayscaleImagePtr> gChannels;
            std::vector<GrayscaleImagePtr> bChannels;

            rChannels.reserve(numSelected);
            gChannels.reserve(numSelected);
            bChannels.reserve(numSelected);

            for (int idx : selectedIndices) {
                ImageVariant frame = batch->getImage(idx);
                if (!std::holds_alternative<RGBImagePtr>(frame)) {
                    throw std::runtime_error("Inconsistent batch: mixed RGB and Grayscale frames");
                }
                auto rgb = std::get<RGBImagePtr>(frame);
                rChannels.push_back(rgb->r());
                gChannels.push_back(rgb->g());
                bChannels.push_back(rgb->b());
            }

            // Stack each channel
            auto stackedR = stackChannels(rChannels, method, rejection, sigmaLow, sigmaHigh, quantileLow, quantileHigh,
                                          [progress](int p) { if (progress) progress(p / 3); });
            auto stackedG = stackChannels(gChannels, method, rejection, sigmaLow, sigmaHigh, quantileLow, quantileHigh,
                                          [progress](int p) { if (progress) progress(33 + p / 3); });
            auto stackedB = stackChannels(bChannels, method, rejection, sigmaLow, sigmaHigh, quantileLow, quantileHigh,
                                          [progress](int p) { if (progress) progress(66 + p / 3); });

            auto stackedRGB = std::make_shared<RGBImage>(stackedR, stackedG, stackedB);
            workspace.registerElement(outputName, stackedRGB);
        } else {
            // Grayscale stacking
            std::vector<GrayscaleImagePtr> channels;
            channels.reserve(numSelected);

            for (int idx : selectedIndices) {
                ImageVariant frame = batch->getImage(idx);
                if (!std::holds_alternative<GrayscaleImagePtr>(frame)) {
                    throw std::runtime_error("Inconsistent batch: mixed RGB and Grayscale frames");
                }
                channels.push_back(std::get<GrayscaleImagePtr>(frame));
            }

            auto stackedGray = stackChannels(channels, method, rejection, sigmaLow, sigmaHigh, quantileLow, quantileHigh, progress);
            workspace.registerElement(outputName, stackedGray);
        }
        qInfo() << "[Stacking] Finished RAM stacking. Registered output master:" << QString::fromStdString(outputName);
    }
}

} // namespace blastro
