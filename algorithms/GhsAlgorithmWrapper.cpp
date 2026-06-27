#include "GhsAlgorithmWrapper.h"
#include "GhsAlgorithm.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include <stdexcept>
#include <iostream>
#include "core/Logger.h"
#include <omp.h>
#include <QElapsedTimer>
#include "core/Preferences.h"

namespace blastro {

void GhsAlgorithmWrapper::execute(WorkspaceRegistry& workspace, 
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

    double stretchFactor = config.count("stretch_factor") ? std::stod(config.at("stretch_factor")) : 3.0;
    double symmetryPoint = config.count("symmetry_point") ? std::stod(config.at("symmetry_point")) : 0.1;
    double localStretch = config.count("local_stretch") ? std::stod(config.at("local_stretch")) : 0.0;
    bool colorPreserving = config.count("color_preserving") ? (config.at("color_preserving") == "true") : true;
    int form = config.count("local_stretch") ? static_cast<int>(localStretch) : 1;

    int threads = config.count("threads") ? std::stoi(config.at("threads")) : -1;
    if (threads <= 0) {
        threads = Preferences::instance().getThreadCount();
    }
    if (threads > 0) {
        omp_set_num_threads(threads);
    }

    Logger::header("GHS", QString("Starting stretch execution. Input: %1, output: %2, stretch factor (d): %3, symmetry point (sp): %4, local stretch (b/form): %5, color preserving: %6, threads: %7")
                   .arg(QString::fromStdString(inputName))
                   .arg(QString::fromStdString(outputName))
                   .arg(stretchFactor)
                   .arg(symmetryPoint)
                   .arg(form)
                   .arg(colorPreserving ? "true" : "false")
                   .arg(threads));

    QElapsedTimer totalTimer;
    totalTimer.start();

    WorkspaceElement inputElem = workspace.getElement(inputName);

    if (progress) progress(20);

    WorkspaceElement outputElem;
    if (std::holds_alternative<GrayscaleImagePtr>(inputElem)) {
        auto gray = std::get<GrayscaleImagePtr>(inputElem);
        outputElem = GhsAlgorithm::stretchGrayscale(gray, 0.0, 1.0, symmetryPoint, stretchFactor, form);
    } else if (std::holds_alternative<RGBImagePtr>(inputElem)) {
        auto rgb = std::get<RGBImagePtr>(inputElem);
        outputElem = GhsAlgorithm::stretchRGB(rgb, 0.0, 1.0, symmetryPoint, stretchFactor, form, colorPreserving);
    } else {
        throw std::runtime_error("GHS stretch is not supported on batches in this mode");
    }

    if (progress) progress(80);

    if (workspace.contains(outputName)) {
        workspace.unregisterElement(outputName);
    }
    workspace.registerElement(outputName, outputElem);

    if (progress) progress(100);
    Logger::success("GHS", QString("Finished GHS stretch in %1 ms. Registered output: %2")
                    .arg(totalTimer.elapsed()).arg(QString::fromStdString(outputName)));
}

} // namespace blastro
