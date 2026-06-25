#include "PixelMathAlgorithm.h"
#define exprtk_disable_string_capabilities // Optimize exprtk
#include "thirdparty/exprtk.hpp"
#include <stdexcept>
#include <iostream>

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

void PixelMathAlgorithm::execute(WorkspaceRegistry& workspace, const std::map<std::string, std::string>& config) {
    std::string exprR = config.count("expr_r") ? config.at("expr_r") : "";
    std::string exprG = config.count("expr_g") ? config.at("expr_g") : "";
    std::string exprB = config.count("expr_b") ? config.at("expr_b") : "";
    std::string exprK = config.count("expr_k") ? config.at("expr_k") : "";
    std::string outputName = config.count("output_name") ? config.at("output_name") : "PixelMath_Output";
    bool isRGB = config.count("color_space") ? (config.at("color_space") == "RGB") : true;

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

    // 2. Setup exprtk symbol table
    exprtk::symbol_table<double> symbolTable;
    
    double xVal = 0.0;
    double yVal = 0.0;
    double wVal = width;
    double hVal = height;

    symbolTable.add_variable("x", xVal);
    symbolTable.add_variable("y", yVal);
    symbolTable.add_variable("w", wVal);
    symbolTable.add_variable("h", hVal);
    symbolTable.add_constants(); // adds pi, e, etc.

    // Bind workspace image variables
    for (auto& bind : bindings) {
        // Simple name binds to current channel value (active channel during loop)
        symbolTable.add_variable(bind.name, bind.currentVal);
        
        // Explicit channel binds
        symbolTable.add_variable(bind.name + "_r", bind.currentR);
        symbolTable.add_variable(bind.name + "_g", bind.currentG);
        symbolTable.add_variable(bind.name + "_b", bind.currentB);
        symbolTable.add_variable(bind.name + "_k", bind.currentVal); // Grayscale alias
    }

    // 3. Compile expressions
    exprtk::expression<double> expressionR, expressionG, expressionB, expressionK;
    exprtk::parser<double> parser;

    if (isRGB) {
        if (!exprR.empty()) {
            expressionR.register_symbol_table(symbolTable);
            if (!parser.compile(exprR, expressionR)) {
                throw std::runtime_error("Failed to compile Red expression: " + parser.error());
            }
        }
        if (!exprG.empty()) {
            expressionG.register_symbol_table(symbolTable);
            if (!parser.compile(exprG, expressionG)) {
                throw std::runtime_error("Failed to compile Green expression: " + parser.error());
            }
        }
        if (!exprB.empty()) {
            expressionB.register_symbol_table(symbolTable);
            if (!parser.compile(exprB, expressionB)) {
                throw std::runtime_error("Failed to compile Blue expression: " + parser.error());
            }
        }
    } else {
        expressionK.register_symbol_table(symbolTable);
        if (!parser.compile(exprK, expressionK)) {
            throw std::runtime_error("Failed to compile Grayscale expression: " + parser.error());
        }
    }

    // 4. Create output image
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

    // 5. Pixel loop
    for (int y = 0; y < height; ++y) {
        yVal = y;
        for (int x = 0; x < width; ++x) {
            xVal = x;

            // Update bindings for current pixel coordinate
            for (auto& bind : bindings) {
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
                    // Default generic value is average or luminance
                    bind.currentVal = (bind.currentR + bind.currentG + bind.currentB) / 3.0;
                } else if (bind.batchImage) {
                    // Just read first frame for simplicity inside math, or active frame
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
                    // In Red channel expression, the generic image variable represents the Red channel of the source
                    for (auto& bind : bindings) {
                        bind.currentVal = bind.currentR;
                    }
                    outRGB->r()->buffer()->setPixel(x, y, static_cast<float>(expressionR.value()));
                }
                // Green
                if (!exprG.empty()) {
                    // In Green channel expression, the generic image variable represents the Green channel of the source
                    for (auto& bind : bindings) {
                        bind.currentVal = bind.currentG;
                    }
                    outRGB->g()->buffer()->setPixel(x, y, static_cast<float>(expressionG.value()));
                }
                // Blue
                if (!exprB.empty()) {
                    // In Blue channel expression, the generic image variable represents the Blue channel of the source
                    for (auto& bind : bindings) {
                        bind.currentVal = bind.currentB;
                    }
                    outRGB->b()->buffer()->setPixel(x, y, static_cast<float>(expressionB.value()));
                }
            } else {
                outK->buffer()->setPixel(x, y, static_cast<float>(expressionK.value()));
            }
        }
    }

    // 6. Register back to the workspace (removing any existing element with same name first)
    if (workspace.contains(outputName)) {
        workspace.unregisterElement(outputName);
    }
    workspace.registerElement(outputName, outputElem);
}

} // namespace blastro
