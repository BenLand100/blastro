#include "StackingAlgorithm.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include "core/ImageBuffer.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace blastro {

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

    // Pre-allocate a vector for pixel values to reuse across iterations
    std::vector<float> pixelValues(numFrames);

    for (int p = 0; p < numPixels; ++p) {
        if (progressSub && (p % (numPixels / 20) == 0)) {
            progressSub(static_cast<int>(100.0 * p / numPixels));
        }
        // Collect pixel values across all selected frames
        for (int f = 0; f < numFrames; ++f) {
            pixelValues[f] = rawDataPointers[f][p];
        }

        std::vector<float> activeValues = pixelValues;
        int n = activeValues.size();

        // Apply rejection methods
        if (rejection == "quantile" && n > 1) {
            std::sort(activeValues.begin(), activeValues.end());
            int rejectLow = static_cast<int>(n * quantileLow);
            int rejectHigh = static_cast<int>(n * quantileHigh);
            if (rejectLow + rejectHigh >= n) {
                // Keep at least the median element if rejection is too aggressive
                int mid = n / 2;
                activeValues = { activeValues[mid] };
            } else {
                activeValues = std::vector<float>(activeValues.begin() + rejectLow, activeValues.end() - rejectHigh);
            }
        } else if ((rejection == "sigma" || rejection == "winsorized_sigma") && n > 2) {
            // Iterative sigma clipping (2 iterations)
            for (int iter = 0; iter < 2; ++iter) {
                int currSize = activeValues.size();
                if (currSize <= 2) break;

                // Compute mean
                double sum = 0.0;
                for (float val : activeValues) sum += val;
                double mean = sum / currSize;

                // Compute standard deviation
                double sumSqDiff = 0.0;
                for (float val : activeValues) {
                    double diff = val - mean;
                    sumSqDiff += diff * diff;
                }
                double stddev = std::sqrt(sumSqDiff / (currSize - 1));
                if (stddev < 1e-8) break; // Terminate if values are homogeneous

                double lowBound = mean - sigmaLow * stddev;
                double highBound = mean + sigmaHigh * stddev;

                if (rejection == "sigma") {
                    std::vector<float> filtered;
                    for (float val : activeValues) {
                        if (val >= lowBound && val <= highBound) {
                            filtered.push_back(val);
                        }
                    }
                    if (filtered.empty()) {
                        break; // Fallback to current set if all would be rejected
                    }
                    activeValues = filtered;
                } else {
                    // Winsorization: replace outlying values with bounds
                    for (int i = 0; i < currSize; ++i) {
                        if (activeValues[i] < lowBound) {
                            activeValues[i] = static_cast<float>(lowBound);
                        } else if (activeValues[i] > highBound) {
                            activeValues[i] = static_cast<float>(highBound);
                        }
                    }
                }
            }
        }

        int m = activeValues.size();
        if (m == 0) {
            outData[p] = 0.0f;
            continue;
        }

        // Apply stacking methods
        if (method == "average") {
            double sum = 0.0;
            for (float val : activeValues) sum += val;
            outData[p] = static_cast<float>(sum / m);
        } else if (method == "median") {
            std::sort(activeValues.begin(), activeValues.end());
            if (m % 2 == 1) {
                outData[p] = activeValues[m / 2];
            } else {
                outData[p] = (activeValues[m / 2 - 1] + activeValues[m / 2]) * 0.5f;
            }
        } else if (method == "max") {
            outData[p] = *std::max_element(activeValues.begin(), activeValues.end());
        } else if (method == "min") {
            outData[p] = *std::min_element(activeValues.begin(), activeValues.end());
        } else {
            outData[p] = activeValues[0];
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

    // Check if the first frame is RGB to determine color mode
    ImageVariant firstFrame = batch->getImage(selectedIndices[0]);
    bool isRGB = std::holds_alternative<RGBImagePtr>(firstFrame);

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
}

} // namespace blastro
