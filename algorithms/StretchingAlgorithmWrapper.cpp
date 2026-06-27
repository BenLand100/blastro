#include "StretchingAlgorithmWrapper.h"
#include "StretchingAlgorithm.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include <stdexcept>
#include <iostream>
#include "core/Logger.h"
#include <omp.h>
#include <QElapsedTimer>
#include "core/Preferences.h"

namespace blastro {

void StretchingAlgorithmWrapper::execute(WorkspaceRegistry& workspace, 
                                         const std::map<std::string, std::string>& config, 
                                         ProgressCallback progress) {
    std::string inputName = config.count("input_name") ? config.at("input_name") : "";
    std::string outputName = config.count("output_name") ? config.at("output_name") : "";

    if (inputName.empty()) {
        auto names = workspace.elementNames();
        if (names.empty()) {
            throw std::runtime_error("No images in the workspace to stretch");
        }
        inputName = names[0];
    }
    if (outputName.empty()) {
        outputName = inputName + "_stretched";
    }

    std::string mode = config.count("mode") ? config.at("mode") : "ghs";
    bool colorPreserving = config.count("color_preserving") ? (config.at("color_preserving") == "true") : true;

    int threads = config.count("threads") ? std::stoi(config.at("threads")) : -1;
    if (threads <= 0) {
        threads = Preferences::instance().getThreadCount();
    }
    if (threads > 0) {
        omp_set_num_threads(threads);
    }

    QElapsedTimer totalTimer;
    totalTimer.start();

    WorkspaceElement inputElem = workspace.getElement(inputName);
    WorkspaceElement outputElem;

    if (progress) progress(20);

    if (mode == "histogram") {
        double blackpoint = config.count("blackpoint") ? std::stod(config.at("blackpoint")) : 0.0;
        double whitepoint = config.count("whitepoint") ? std::stod(config.at("whitepoint")) : 1.0;
        double midpoint = config.count("midpoint") ? std::stod(config.at("midpoint")) : 0.5;

        Logger::header("Stretching", QString("Starting HT stretch. Input: %1, output: %2, B: %3, W: %4, M: %5, color preserving: %6, threads: %7")
                       .arg(QString::fromStdString(inputName))
                       .arg(QString::fromStdString(outputName))
                       .arg(blackpoint).arg(whitepoint).arg(midpoint)
                       .arg(colorPreserving ? "true" : "false")
                       .arg(threads));

        if (std::holds_alternative<GrayscaleImagePtr>(inputElem)) {
            auto gray = std::get<GrayscaleImagePtr>(inputElem);
            outputElem = StretchingAlgorithm::stretchHistogramGrayscale(gray, blackpoint, whitepoint, midpoint);
        } else if (std::holds_alternative<RGBImagePtr>(inputElem)) {
            auto rgb = std::get<RGBImagePtr>(inputElem);
            outputElem = StretchingAlgorithm::stretchHistogramRGB(rgb, blackpoint, whitepoint, midpoint, colorPreserving);
        } else {
            throw std::runtime_error("Stretch is not supported on batches in this mode");
        }
    } else { // ghs
        double lowPoint = config.count("low_point") ? std::stod(config.at("low_point")) : 0.0;
        double highPoint = config.count("high_point") ? std::stod(config.at("high_point")) : 1.0;
        double symmetryPoint = config.count("symmetry_point") ? std::stod(config.at("symmetry_point")) : 0.5;
        double stretchFactor = config.count("stretch_factor") ? std::stod(config.at("stretch_factor")) : 3.0;
        double shadowProtect = config.count("shadow_protect") ? std::stod(config.at("shadow_protect")) : 0.0;
        double highlightProtect = config.count("highlight_protect") ? std::stod(config.at("highlight_protect")) : 0.0;
        int form = config.count("form") ? std::stoi(config.at("form")) : 1;

        Logger::header("Stretching", QString("Starting GHS stretch. Input: %1, output: %2, SP: %3, D: %4, shadowProtect: %5, highlightProtect: %6, color preserving: %7, threads: %8")
                       .arg(QString::fromStdString(inputName))
                       .arg(QString::fromStdString(outputName))
                       .arg(symmetryPoint).arg(stretchFactor).arg(shadowProtect).arg(highlightProtect)
                       .arg(colorPreserving ? "true" : "false")
                       .arg(threads));

        if (std::holds_alternative<GrayscaleImagePtr>(inputElem)) {
            auto gray = std::get<GrayscaleImagePtr>(inputElem);
            outputElem = StretchingAlgorithm::stretchGhsGrayscale(gray, lowPoint, highPoint, symmetryPoint, stretchFactor, shadowProtect, highlightProtect, form);
        } else if (std::holds_alternative<RGBImagePtr>(inputElem)) {
            auto rgb = std::get<RGBImagePtr>(inputElem);
            outputElem = StretchingAlgorithm::stretchGhsRGB(rgb, lowPoint, highPoint, symmetryPoint, stretchFactor, shadowProtect, highlightProtect, form, colorPreserving);
        } else {
            throw std::runtime_error("Stretch is not supported on batches in this mode");
        }
    }

    if (progress) progress(80);

    if (workspace.contains(outputName)) {
        workspace.unregisterElement(outputName);
    }
    workspace.registerElement(outputName, outputElem);

    if (progress) progress(100);
    Logger::success("Stretching", QString("Finished stretch in %1 ms. Registered output: %2")
                    .arg(totalTimer.elapsed()).arg(QString::fromStdString(outputName)));
}

} // namespace blastro
