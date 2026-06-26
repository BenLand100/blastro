#include "PixelMathAlgorithm.h"
#define exprtk_disable_string_capabilities // Optimize exprtk
#include "thirdparty/exprtk.hpp"
#include <stdexcept>
#include <iostream>
#include <omp.h>
#include <QDebug>

namespace blastro {

// Helper structure to bind workspace variables to exprtk values
struct ImageBindings {
    std::string name;
    GrayscaleImagePtr grayImage;
    RGBImagePtr rgbImage;
    ImageBatchPtr batchImage;
    
    // Current pixel value pointer/reference for exprtk
    double currentVal = 0.0;
    double currentR = 0.0;
    double currentG = 0.0;
    double currentB = 0.0;
};

void PixelMathAlgorithm::execute(WorkspaceRegistry& workspace, 
                                 const std::map<std::string, std::string>& config, 
                                 ProgressCallback progress) {
    std::string exprR = config.count("expr_r") ? config.at("expr_r") : "";
    std::string exprG = config.count("expr_g") ? config.at("expr_g") : "";
    std::string exprB = config.count("expr_b") ? config.at("expr_b") : "";
    std::string exprK = config.count("expr_k") ? config.at("expr_k") : "";
    std::string outputName = config.count("output_name") ? config.at("output_name") : "PixelMath_Output";
    bool isRGB = config.count("color_space") ? (config.at("color_space") == "RGB") : true;

    qInfo() << "[PixelMath] Starting execution. Output color space:" << (isRGB ? "RGB" : "Grayscale") << ", target output name:" << QString::fromStdString(outputName);
    if (isRGB) {
        qInfo() << "[PixelMath] Expressions: R =" << QString::fromStdString(exprR) << ", G =" << QString::fromStdString(exprG) << ", B =" << QString::fromStdString(exprB);
    } else {
        qInfo() << "[PixelMath] Expression: K =" << QString::fromStdString(exprK);
    }

    if (isRGB && exprR.empty() && exprG.empty() && exprB.empty()) {
        throw std::runtime_error("At least one RGB expression must be provided for RGB mode");
    }
    if (!isRGB && exprK.empty()) {
        throw std::runtime_error("Grayscale expression (K) must be provided for Grayscale mode");
    }

    // 1. Gather all active elements in the workspace and prepare their bindings
    std::vector<std::string> names = workspace.elementNames();
    std::vector<ImageBindings> bindings;
    
    int width = 0;
    int height = 0;

    for (const auto& name : names) {
        WorkspaceElement elem = workspace.getElement(name);
        ImageBindings bind;
        bind.name = name;

        if (std::holds_alternative<GrayscaleImagePtr>(elem)) {
            bind.grayImage = std::get<GrayscaleImagePtr>(elem);
            if (width == 0) {
                width = bind.grayImage->width();
                height = bind.grayImage->height();
            }
            bindings.push_back(bind);
        } else if (std::holds_alternative<RGBImagePtr>(elem)) {
            bind.rgbImage = std::get<RGBImagePtr>(elem);
            if (width == 0) {
                width = bind.rgbImage->width();
                height = bind.rgbImage->height();
            }
            bindings.push_back(bind);
        } else if (std::holds_alternative<ImageBatchPtr>(elem)) {
            bind.batchImage = std::get<ImageBatchPtr>(elem);
            // Get frame 0 for sizing if possible
            if (bind.batchImage->count() > 0) {
                auto firstFrame = bind.batchImage->getImage(0);
                if (std::holds_alternative<GrayscaleImagePtr>(firstFrame)) {
                    auto img = std::get<GrayscaleImagePtr>(firstFrame);
                    if (width == 0) {
                        width = img->width();
                        height = img->height();
                    }
                } else if (std::holds_alternative<RGBImagePtr>(firstFrame)) {
                    auto img = std::get<RGBImagePtr>(firstFrame);
                    if (width == 0) {
                        width = img->width();
                        height = img->height();
                    }
                }
            }
            bindings.push_back(bind);
        }
    }

    // Default dimensions if no images are in the workspace
    if (width <= 0 || height <= 0) {
        width = config.count("width") ? std::stoi(config.at("width")) : 800;
        height = config.count("height") ? std::stoi(config.at("height")) : 600;
    }

    // 2. Validate expressions sequentially first to ensure syntax correctness
    {
        exprtk::symbol_table<double> dummySymbolTable;
        double dummyVal = 0.0;
        dummySymbolTable.add_variable("x", dummyVal);
        dummySymbolTable.add_variable("y", dummyVal);
        dummySymbolTable.add_variable("w", dummyVal);
        dummySymbolTable.add_variable("h", dummyVal);
        dummySymbolTable.add_constants();
        for (const auto& bind : bindings) {
            dummySymbolTable.add_variable(bind.name, dummyVal);
            dummySymbolTable.add_variable(bind.name + "_r", dummyVal);
            dummySymbolTable.add_variable(bind.name + "_g", dummyVal);
            dummySymbolTable.add_variable(bind.name + "_b", dummyVal);
            dummySymbolTable.add_variable(bind.name + "_k", dummyVal);
        }

        exprtk::expression<double> dummyExpr;
        exprtk::parser<double> dummyParser;

        if (isRGB) {
            if (!exprR.empty()) {
                dummyExpr.register_symbol_table(dummySymbolTable);
                if (!dummyParser.compile(exprR, dummyExpr)) {
                    throw std::runtime_error("Failed to compile Red expression: " + dummyParser.error());
                }
            }
            if (!exprG.empty()) {
                dummyExpr.register_symbol_table(dummySymbolTable);
                if (!dummyParser.compile(exprG, dummyExpr)) {
                    throw std::runtime_error("Failed to compile Green expression: " + dummyParser.error());
                }
            }
            if (!exprB.empty()) {
                dummyExpr.register_symbol_table(dummySymbolTable);
                if (!dummyParser.compile(exprB, dummyExpr)) {
                    throw std::runtime_error("Failed to compile Blue expression: " + dummyParser.error());
                }
            }
        } else {
            dummyExpr.register_symbol_table(dummySymbolTable);
            if (!dummyParser.compile(exprK, dummyExpr)) {
                throw std::runtime_error("Failed to compile Grayscale expression: " + dummyParser.error());
            }
        }
    }

    // 3. Create output image
    WorkspaceElement outputElem;
    GrayscaleImagePtr outK = nullptr;
    RGBImagePtr outRGB = nullptr;

    if (isRGB) {
        outRGB = std::make_shared<RGBImage>(width, height);
        outputElem = outRGB;
    } else {
        outK = std::make_shared<GrayscaleImage>(width, height);
        outputElem = outK;
    }

    // 4. Parallel Pixel loop with thread-local exprtk expressions
    #pragma omp parallel
    {
        double threadX = 0.0;
        double threadY = 0.0;
        double threadW = width;
        double threadH = height;

        std::vector<ImageBindings> threadBindings = bindings;

        exprtk::symbol_table<double> threadSymbolTable;
        threadSymbolTable.add_variable("x", threadX);
        threadSymbolTable.add_variable("y", threadY);
        threadSymbolTable.add_variable("w", threadW);
        threadSymbolTable.add_variable("h", threadH);
        threadSymbolTable.add_constants();

        for (auto& bind : threadBindings) {
            threadSymbolTable.add_variable(bind.name, bind.currentVal);
            threadSymbolTable.add_variable(bind.name + "_r", bind.currentR);
            threadSymbolTable.add_variable(bind.name + "_g", bind.currentG);
            threadSymbolTable.add_variable(bind.name + "_b", bind.currentB);
            threadSymbolTable.add_variable(bind.name + "_k", bind.currentVal);
        }

        exprtk::expression<double> threadExprR, threadExprG, threadExprB, threadExprK;
        exprtk::parser<double> threadParser;

        if (isRGB) {
            if (!exprR.empty()) {
                threadExprR.register_symbol_table(threadSymbolTable);
                threadParser.compile(exprR, threadExprR);
            }
            if (!exprG.empty()) {
                threadExprG.register_symbol_table(threadSymbolTable);
                threadParser.compile(exprG, threadExprG);
            }
            if (!exprB.empty()) {
                threadExprB.register_symbol_table(threadSymbolTable);
                threadParser.compile(exprB, threadExprB);
            }
        } else {
            threadExprK.register_symbol_table(threadSymbolTable);
            threadParser.compile(exprK, threadExprK);
        }

        #pragma omp for schedule(static)
        for (int y = 0; y < height; ++y) {
            threadY = y;
            
            // Report progress from the master thread only
            if (progress && omp_get_thread_num() == 0 && (y % 10 == 0)) {
                progress(static_cast<int>(100.0 * y / height));
            }
            
            for (int x = 0; x < width; ++x) {
                threadX = x;

                // Update bindings for current pixel coordinate in thread-local space
                for (auto& bind : threadBindings) {
                    if (bind.grayImage) {
                        float val = bind.grayImage->buffer()->pixel(x, y);
                        bind.currentVal = val;
                        bind.currentR = val;
                        bind.currentG = val;
                        bind.currentB = val;
                    } else if (bind.rgbImage) {
                        bind.currentR = bind.rgbImage->r()->buffer()->pixel(x, y);
                        bind.currentG = bind.rgbImage->g()->buffer()->pixel(x, y);
                        bind.currentB = bind.rgbImage->b()->buffer()->pixel(x, y);
                        bind.currentVal = (bind.currentR + bind.currentG + bind.currentB) / 3.0;
                    } else if (bind.batchImage) {
                        auto frame = bind.batchImage->getImage(0);
                        if (std::holds_alternative<GrayscaleImagePtr>(frame)) {
                            float val = std::get<GrayscaleImagePtr>(frame)->buffer()->pixel(x, y);
                            bind.currentVal = val;
                            bind.currentR = val;
                            bind.currentG = val;
                            bind.currentB = val;
                        } else if (std::holds_alternative<RGBImagePtr>(frame)) {
                            auto rgb = std::get<RGBImagePtr>(frame);
                            bind.currentR = rgb->r()->buffer()->pixel(x, y);
                            bind.currentG = rgb->g()->buffer()->pixel(x, y);
                            bind.currentB = rgb->b()->buffer()->pixel(x, y);
                            bind.currentVal = (bind.currentR + bind.currentG + bind.currentB) / 3.0;
                        }
                    }
                }

                // Evaluate expressions and write to output
                if (isRGB) {
                    // Red
                    if (!exprR.empty()) {
                        for (auto& bind : threadBindings) {
                            bind.currentVal = bind.currentR;
                        }
                        outRGB->r()->buffer()->setPixel(x, y, static_cast<float>(threadExprR.value()));
                    }
                    // Green
                    if (!exprG.empty()) {
                        for (auto& bind : threadBindings) {
                            bind.currentVal = bind.currentG;
                        }
                        outRGB->g()->buffer()->setPixel(x, y, static_cast<float>(threadExprG.value()));
                    }
                    // Blue
                    if (!exprB.empty()) {
                        for (auto& bind : threadBindings) {
                            bind.currentVal = bind.currentB;
                        }
                        outRGB->b()->buffer()->setPixel(x, y, static_cast<float>(threadExprB.value()));
                    }
                } else {
                    outK->buffer()->setPixel(x, y, static_cast<float>(threadExprK.value()));
                }
            }
        }
    }

    // 6. Register back to the workspace (removing any existing element with same name first)
    if (workspace.contains(outputName)) {
        workspace.unregisterElement(outputName);
    }
    workspace.registerElement(outputName, outputElem);
    qInfo() << "[PixelMath] Finished execution. Registered output image:" << QString::fromStdString(outputName);
}

} // namespace blastro
