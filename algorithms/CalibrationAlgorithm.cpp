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
#include <QDebug>

namespace blastro {

static GrayscaleImagePtr calibrateChannel(GrayscaleImagePtr input,
                                         GrayscaleImagePtr bias,
                                         GrayscaleImagePtr dark,
                                         GrayscaleImagePtr flat,
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

    // Calculate flat mean for normalization to preserve overall signal level
    float flatMean = 1.0f;
    if (flatData) {
        double sum = 0.0;
        int numPixels = w * h;
        for (int i = 0; i < numPixels; ++i) {
            sum += flatData[i];
        }
        flatMean = static_cast<float>(sum / numPixels);
        if (std::abs(flatMean) < 1e-6f) {
            flatMean = 1.0f; // Prevent division-by-zero during normalization
        }
    }

    int numPixels = w * h;
    for (int i = 0; i < numPixels; ++i) {
        if (progressSub && (i % (numPixels / 20) == 0)) {
            progressSub(static_cast<int>(100.0 * i / numPixels));
        }
        float val = inData[i];
        if (darkData) val -= darkData[i];
        if (biasData) val -= biasData[i];

        if (flatData) {
            // Apply normalized flat: Calibrated = (Input - Dark - Bias) / (Flat / FlatMean)
            float fVal = flatData[i] / flatMean;
            if (std::abs(fVal) < 1e-6f) {
                outData[i] = 0.0f; // Division-by-zero safety
            } else {
                outData[i] = val / fVal;
            }
        } else {
            outData[i] = val;
        }
    }

    return std::make_shared<GrayscaleImage>(outBuffer);
}

void CalibrationAlgorithm::execute(WorkspaceRegistry& workspace, 
                                   const std::map<std::string, std::string>& config, 
                                   ProgressCallback progress) {
    std::string inputName = config.at("input_name");
    std::string outputName = config.at("output_name");
    
    std::string biasName = config.at("bias_name");
    std::string darkName = config.at("dark_name");
    std::string flatName = config.at("flat_name");

    qInfo() << "[Calibration] Starting execution. Input:" << QString::fromStdString(inputName) << ", output:" << QString::fromStdString(outputName);
    qInfo() << "[Calibration] Calibration frames - Bias:" << (biasName.empty() ? "<None>" : QString::fromStdString(biasName))
            << ", Dark:" << (darkName.empty() ? "<None>" : QString::fromStdString(darkName))
            << ", Flat:" << (flatName.empty() ? "<None>" : QString::fromStdString(flatName));

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

    // 3. Resolve optional Flat frame
    GrayscaleImagePtr flatR = nullptr, flatG = nullptr, flatB = nullptr;
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
    }

    // 4. Perform Calibration based on input type
    if (std::holds_alternative<ImageBatchPtr>(inputElem)) {
        auto inputBatch = std::get<ImageBatchPtr>(inputElem);
        int count = inputBatch->count();
        
        std::string tempDir = TempDirectory::createTempDir("calibrated");
        if (tempDir.empty()) {
            throw std::runtime_error("Failed to create intermediate directory for calibration");
        }

        std::vector<std::string> names(count);
        std::vector<std::string> filepaths(count);
        
        FitsIO writer;

        for (int i = 0; i < count; ++i) {
            ImageVariant frame = inputBatch->getImage(i);
            ImageVariant calibratedFrame;

            bool frameRGB = std::holds_alternative<RGBImagePtr>(frame);
            if (frameRGB) {
                auto rgb = std::get<RGBImagePtr>(frame);
                auto calR = calibrateChannel(rgb->r(), biasR, darkR, flatR);
                auto calG = calibrateChannel(rgb->g(), biasG, darkG, flatG);
                auto calB = calibrateChannel(rgb->b(), biasB, darkB, flatB);
                calibratedFrame = std::make_shared<RGBImage>(calR, calG, calB);
            } else {
                auto gray = std::get<GrayscaleImagePtr>(frame);
                calibratedFrame = calibrateChannel(gray, biasR, darkR, flatR);
            }

            // Generate intermediate filename: e.g. light_001_calibrated.fits
            std::string origPath = inputBatch->frameFilepath(i);
            std::string outFilename = TempDirectory::getIntermediateFileName(origPath, "_calibrated", i);
            std::string fullOutPath = tempDir + "/" + outFilename;

            if (!writer.writeImage(fullOutPath, calibratedFrame)) {
                throw std::runtime_error("Failed to write intermediate calibrated frame to " + fullOutPath);
            }

            names[i] = inputBatch->frameName(i) + "_calibrated";
            filepaths[i] = fullOutPath;

            // Evict frame from RAM to keep memory usage flat
            inputBatch->clearCache(i);

            if (progress) {
                progress(static_cast<int>(100.0 * i / count));
            }
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

        if (progress) progress(100);
        workspace.registerElement(outputName, calibBatch);
    } else {
        // Single image calibration
        bool isRGB = std::holds_alternative<RGBImagePtr>(inputElem);
        if (isRGB) {
            auto rgb = std::get<RGBImagePtr>(inputElem);
            auto calR = calibrateChannel(rgb->r(), biasR, darkR, flatR, [progress](int p) { if (progress) progress(p / 3); });
            auto calG = calibrateChannel(rgb->g(), biasG, darkG, flatG, [progress](int p) { if (progress) progress(33 + p / 3); });
            auto calB = calibrateChannel(rgb->b(), biasB, darkB, flatB, [progress](int p) { if (progress) progress(66 + p / 3); });
            auto calRGB = std::make_shared<RGBImage>(calR, calG, calB);
            workspace.registerElement(outputName, calRGB);
        } else {
            auto gray = std::get<GrayscaleImagePtr>(inputElem);
            auto calGray = calibrateChannel(gray, biasR, darkR, flatR, progress);
            workspace.registerElement(outputName, calGray);
        }
    }
    qInfo() << "[Calibration] Finished calibration. Registered output:" << QString::fromStdString(outputName);
}

} // namespace blastro
