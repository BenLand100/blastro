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

#include "PixelMathAlgorithm.h"
#define exprtk_disable_string_capabilities // Optimize exprtk
#include "thirdparty/exprtk.hpp"
#include <stdexcept>
#include <iostream>
#include <set>
#include <cctype>
#include <omp.h>
#include "core/Logger.h"
#include <QElapsedTimer>
#include "core/Preferences.h"

namespace blastro {

// Helper structure to bind workspace variables to exprtk values
struct ImageBindings {
    std::string name;
    std::string varName;
    GrayscaleImagePtr grayImage;
    RGBImagePtr rgbImage;
    ImageBatchPtr batchImage;
    
    // Extracted pointers for fast access
    float* grayData = nullptr;
    float* rData = nullptr;
    float* gData = nullptr;
    float* bData = nullptr;

    // Current pixel value pointer/reference for exprtk
    double currentVal = 0.0;
    double currentR = 0.0;
    double currentG = 0.0;
    double currentB = 0.0;
};

static ImageMetadata getFirstBindingMetadata(const std::vector<ImageBindings>& bindings) {
    ImageMetadata meta;
    if (bindings.empty()) return meta;
    const auto& bind = bindings[0];
    if (bind.grayImage) {
        meta = bind.grayImage->metadata();
    } else if (bind.rgbImage) {
        meta = bind.rgbImage->metadata();
    } else if (bind.batchImage && bind.batchImage->count() > 0) {
        meta = bind.batchImage->frameMetadata(0).baseMetadata;
    }
    return meta;
}

static bool isIdentifierReferenced(const std::string& varName, const std::vector<std::string>& expressions) {
    std::vector<std::string> suffixes = {
        "", "_r", "_g", "_b", "_k", "_red", "_green", "_blue", "_gray",
        "_R", "_G", "_B", "_K"
    };
    for (const auto& expr : expressions) {
        if (expr.empty()) continue;
        for (const auto& suffix : suffixes) {
            std::string target = varName + suffix;
            size_t pos = 0;
            while ((pos = expr.find(target, pos)) != std::string::npos) {
                bool startOk = (pos == 0) || (!std::isalnum(static_cast<unsigned char>(expr[pos - 1])) && expr[pos - 1] != '_');
                bool endOk = (pos + target.length() == expr.length()) || 
                             (!std::isalnum(static_cast<unsigned char>(expr[pos + target.length()])) && expr[pos + target.length()] != '_');
                if (startOk && endOk) {
                    return true;
                }
                pos += target.length();
            }
        }
    }
    return false;
}

void PixelMathAlgorithm::execute(WorkspaceRegistry& workspace, 
                                 const std::map<std::string, std::string>& config, 
                                 ProgressCallback progress) {
    std::string exprR = config.count("expr_r") ? config.at("expr_r") : "";
    std::string exprG = config.count("expr_g") ? config.at("expr_g") : "";
    std::string exprB = config.count("expr_b") ? config.at("expr_b") : "";
    std::string exprK = config.count("expr_k") ? config.at("expr_k") : "";
    std::string outputName = config.count("output_name") ? config.at("output_name") : "PixelMath_Output";
    bool isRGB = config.count("color_space") ? (config.at("color_space") == "RGB") : true;

    int threads = config.count("threads") ? std::stoi(config.at("threads")) : -1;
    if (threads <= 0) {
        threads = Preferences::instance().getThreadCount();
    }
    if (threads > 0) {
        omp_set_num_threads(threads);
    }

    Logger::header("PixelMath", QString("Starting execution. Output color space: %1, target output name: %2, threads: %3")
                   .arg(isRGB ? "RGB" : "Grayscale")
                   .arg(QString::fromStdString(outputName))
                   .arg(threads));

    QElapsedTimer totalTimer;
    totalTimer.start();
    if (isRGB) {
        Logger::info("PixelMath", QString("Expressions: R = %1, G = %2, B = %3")
                     .arg(QString::fromStdString(exprR))
                     .arg(QString::fromStdString(exprG))
                     .arg(QString::fromStdString(exprB)));
    } else {
        Logger::info("PixelMath", QString("Expression: K = %1")
                     .arg(QString::fromStdString(exprK)));
    }

    if (isRGB && exprR.empty() && exprG.empty() && exprB.empty()) {
        throw std::runtime_error("At least one RGB expression must be provided for RGB mode");
    }
    if (!isRGB && exprK.empty()) {
        throw std::runtime_error("Grayscale expression (K) must be provided for Grayscale mode");
    }

    // 1. Gather all active elements in the workspace and prepare their bindings
    std::vector<std::string> names = workspace.elementNames();
    std::map<std::string, std::string> varMap = getSanitizedVariableMap(names);
    std::set<std::string> baseVarNames;
    for (const auto& pair : varMap) {
        baseVarNames.insert(pair.second);
    }

    // Preprocess expressions to replace backticked names with sanitized names
    std::string cleanExprR = preprocessExpression(exprR, varMap);
    std::string cleanExprG = preprocessExpression(exprG, varMap);
    std::string cleanExprB = preprocessExpression(exprB, varMap);
    std::string cleanExprK = preprocessExpression(exprK, varMap);

    std::vector<ImageBindings> bindings;
    
    int width = 0;
    int height = 0;

    for (const auto& name : names) {
        std::string varName = varMap[name];
        bool referenced = isIdentifierReferenced(varName, {cleanExprR, cleanExprG, cleanExprB, cleanExprK});
        if (!referenced) continue;

        WorkspaceElement elem = workspace.getElement(name);
        ImageBindings bind;
        bind.name = name;
        bind.varName = varName;

        int imgW = 0;
        int imgH = 0;

        if (std::holds_alternative<GrayscaleImagePtr>(elem)) {
            bind.grayImage = std::get<GrayscaleImagePtr>(elem);
            bind.grayData = bind.grayImage->buffer()->data();
            imgW = bind.grayImage->width();
            imgH = bind.grayImage->height();
        } else if (std::holds_alternative<RGBImagePtr>(elem)) {
            bind.rgbImage = std::get<RGBImagePtr>(elem);
            bind.rData = bind.rgbImage->r()->buffer()->data();
            bind.gData = bind.rgbImage->g()->buffer()->data();
            bind.bData = bind.rgbImage->b()->buffer()->data();
            imgW = bind.rgbImage->width();
            imgH = bind.rgbImage->height();
        } else if (std::holds_alternative<ImageBatchPtr>(elem)) {
            bind.batchImage = std::get<ImageBatchPtr>(elem);
            if (bind.batchImage->count() > 0) {
                auto firstFrame = bind.batchImage->getImage(0);
                if (std::holds_alternative<GrayscaleImagePtr>(firstFrame)) {
                    auto img = std::get<GrayscaleImagePtr>(firstFrame);
                    bind.grayData = img->buffer()->data();
                    imgW = img->width();
                    imgH = img->height();
                } else if (std::holds_alternative<RGBImagePtr>(firstFrame)) {
                    auto img = std::get<RGBImagePtr>(firstFrame);
                    bind.rData = img->r()->buffer()->data();
                    bind.gData = img->g()->buffer()->data();
                    bind.bData = img->b()->buffer()->data();
                    imgW = img->width();
                    imgH = img->height();
                }
            }
        }

        if (imgW > 0 && imgH > 0) {
            if (width == 0) {
                width = imgW;
                height = imgH;
            } else if (imgW != width || imgH != height) {
                throw std::runtime_error("Image dimension mismatch in PixelMath: '" + name + 
                                         "' is " + std::to_string(imgW) + "x" + std::to_string(imgH) + 
                                         ", but target dimensions are " + std::to_string(width) + "x" + std::to_string(height));
            }
        }
        bindings.push_back(bind);
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

        // 1. Add image variables and channel suffixes first for top-level precedence!
        for (const auto& bind : bindings) {
            dummySymbolTable.add_variable(bind.varName, dummyVal);
            
            auto registerChannel = [&](const std::string& suffix, double& val) {
                std::string targetSymbol = bind.varName + suffix;
                if (baseVarNames.count(targetSymbol) == 0) {
                    dummySymbolTable.add_variable(targetSymbol, val);
                }
            };

            registerChannel("_r", dummyVal);
            registerChannel("_g", dummyVal);
            registerChannel("_b", dummyVal);
            registerChannel("_k", dummyVal);

            registerChannel("_red", dummyVal);
            registerChannel("_green", dummyVal);
            registerChannel("_blue", dummyVal);
            registerChannel("_gray", dummyVal);

            registerChannel("_R", dummyVal);
            registerChannel("_G", dummyVal);
            registerChannel("_B", dummyVal);
            registerChannel("_K", dummyVal);
        }

        // 2. Add x, y, w, h only if they don't conflict with image names
        if (baseVarNames.count("x") == 0) dummySymbolTable.add_variable("x", dummyVal);
        if (baseVarNames.count("y") == 0) dummySymbolTable.add_variable("y", dummyVal);
        if (baseVarNames.count("w") == 0) dummySymbolTable.add_variable("w", dummyVal);
        if (baseVarNames.count("h") == 0) dummySymbolTable.add_variable("h", dummyVal);

        // 3. Add constants (pi, e, etc.), which will not overwrite existing image variables
        dummySymbolTable.add_constants();

        exprtk::expression<double> dummyExpr;
        exprtk::parser<double> dummyParser;

        if (isRGB) {
            if (!cleanExprR.empty()) {
                dummyExpr.register_symbol_table(dummySymbolTable);
                if (!dummyParser.compile(cleanExprR, dummyExpr)) {
                    throw std::runtime_error("Failed to compile Red expression: " + dummyParser.error());
                }
            }
            if (!cleanExprG.empty()) {
                dummyExpr.register_symbol_table(dummySymbolTable);
                if (!dummyParser.compile(cleanExprG, dummyExpr)) {
                    throw std::runtime_error("Failed to compile Green expression: " + dummyParser.error());
                }
            }
            if (!cleanExprB.empty()) {
                dummyExpr.register_symbol_table(dummySymbolTable);
                if (!dummyParser.compile(cleanExprB, dummyExpr)) {
                    throw std::runtime_error("Failed to compile Blue expression: " + dummyParser.error());
                }
            }
        } else {
            dummyExpr.register_symbol_table(dummySymbolTable);
            if (!dummyParser.compile(cleanExprK, dummyExpr)) {
                throw std::runtime_error("Failed to compile Grayscale expression: " + dummyParser.error());
            }
        }
    }

    // 3. Create output image
    WorkspaceElement outputElem;
    GrayscaleImagePtr outK = nullptr;
    RGBImagePtr outRGB = nullptr;

    ImageMetadata inputMeta = getFirstBindingMetadata(bindings);

    if (isRGB) {
        outRGB = std::make_shared<RGBImage>(width, height);
        outRGB->setMetadata(inputMeta);
        outputElem = outRGB;
    } else {
        outK = std::make_shared<GrayscaleImage>(width, height);
        outK->setMetadata(inputMeta);
        outputElem = outK;
    }

    float* outKData = nullptr;
    float* outRGB_R = nullptr;
    float* outRGB_G = nullptr;
    float* outRGB_B = nullptr;

    if (isRGB) {
        outRGB_R = outRGB->r()->buffer()->data();
        outRGB_G = outRGB->g()->buffer()->data();
        outRGB_B = outRGB->b()->buffer()->data();
    } else {
        outKData = outK->buffer()->data();
    }

    int totalPixels = width * height;

    // 4. Parallel Pixel loop with thread-local exprtk expressions
    #pragma omp parallel

    {
        double threadX = 0.0;
        double threadY = 0.0;
        double threadW = width;
        double threadH = height;

        std::vector<ImageBindings> threadBindings = bindings;

        exprtk::symbol_table<double> threadSymbolTable;

        // 1. Add image variables and channel suffixes first for top-level precedence!
        for (auto& bind : threadBindings) {
            threadSymbolTable.add_variable(bind.varName, bind.currentVal);

            auto registerChannel = [&](const std::string& suffix, double& val) {
                std::string targetSymbol = bind.varName + suffix;
                if (baseVarNames.count(targetSymbol) == 0) {
                    threadSymbolTable.add_variable(targetSymbol, val);
                }
            };

            registerChannel("_r", bind.currentR);
            registerChannel("_g", bind.currentG);
            registerChannel("_b", bind.currentB);
            registerChannel("_k", bind.currentVal);

            registerChannel("_red", bind.currentR);
            registerChannel("_green", bind.currentG);
            registerChannel("_blue", bind.currentB);
            registerChannel("_gray", bind.currentVal);

            registerChannel("_R", bind.currentR);
            registerChannel("_G", bind.currentG);
            registerChannel("_B", bind.currentB);
            registerChannel("_K", bind.currentVal);
        }

        // 2. Add x, y, w, h only if they don't conflict with image names
        if (baseVarNames.count("x") == 0) threadSymbolTable.add_variable("x", threadX);
        if (baseVarNames.count("y") == 0) threadSymbolTable.add_variable("y", threadY);
        if (baseVarNames.count("w") == 0) threadSymbolTable.add_variable("w", threadW);
        if (baseVarNames.count("h") == 0) threadSymbolTable.add_variable("h", threadH);

        // 3. Add constants (pi, e, etc.), which will not overwrite existing image variables
        threadSymbolTable.add_constants();

        exprtk::expression<double> threadExprR, threadExprG, threadExprB, threadExprK;
        exprtk::parser<double> threadParser;

        if (isRGB) {
            if (!cleanExprR.empty()) {
                threadExprR.register_symbol_table(threadSymbolTable);
                threadParser.compile(cleanExprR, threadExprR);
            }
            if (!cleanExprG.empty()) {
                threadExprG.register_symbol_table(threadSymbolTable);
                threadParser.compile(cleanExprG, threadExprG);
            }
            if (!cleanExprB.empty()) {
                threadExprB.register_symbol_table(threadSymbolTable);
                threadParser.compile(cleanExprB, threadExprB);
            }
        } else {
            threadExprK.register_symbol_table(threadSymbolTable);
            threadParser.compile(cleanExprK, threadExprK);
        }

        #pragma omp for schedule(static)
        for (int i = 0; i < totalPixels; ++i) {
            int y = i / width;
            int x = i % width;
            threadY = y;
            threadX = x;
            
            // Report progress from the master thread only
            if (progress && omp_get_thread_num() == 0 && (i % (width * 10) == 0)) {
                progress(static_cast<int>(100.0 * y / height));
            }
            
            // Update bindings for current pixel coordinate in thread-local space
            for (auto& bind : threadBindings) {
                if (bind.grayData) {
                    float val = bind.grayData[i];
                    bind.currentVal = val;
                    bind.currentR = val;
                    bind.currentG = val;
                    bind.currentB = val;
                } else if (bind.rData && bind.gData && bind.bData) {
                    bind.currentR = bind.rData[i];
                    bind.currentG = bind.gData[i];
                    bind.currentB = bind.bData[i];
                    bind.currentVal = (bind.currentR + bind.currentG + bind.currentB) / 3.0;
                }
            }

            // Evaluate expressions and write to output
            if (isRGB) {
                // Red
                if (!exprR.empty()) {
                    for (auto& bind : threadBindings) {
                        bind.currentVal = bind.currentR;
                    }
                    outRGB_R[i] = static_cast<float>(threadExprR.value());
                }
                // Green
                if (!exprG.empty()) {
                    for (auto& bind : threadBindings) {
                        bind.currentVal = bind.currentG;
                    }
                    outRGB_G[i] = static_cast<float>(threadExprG.value());
                }
                // Blue
                if (!exprB.empty()) {
                    for (auto& bind : threadBindings) {
                        bind.currentVal = bind.currentB;
                    }
                    outRGB_B[i] = static_cast<float>(threadExprB.value());
                }
            } else {
                outKData[i] = static_cast<float>(threadExprK.value());
            }
        }
    }

    // 6. Register back to the workspace (removing any existing element with same name first)
    bool visible = config.count("visible") ? (config.at("visible") == "true") : true;
    if (workspace.contains(outputName)) {
        workspace.unregisterElement(outputName);
    }
    workspace.registerElement(outputName, outputElem, visible);
    if (progress) progress(100);
    Logger::success("PixelMath", QString("Finished execution in %1 ms. Registered output: %2")
                    .arg(totalTimer.elapsed()).arg(QString::fromStdString(outputName)));
}

std::string PixelMathAlgorithm::sanitizeIdentifier(const std::string& name) {
    if (name.empty()) return "img";
    std::string clean;
    if (!std::isalpha(static_cast<unsigned char>(name[0]))) {
        clean += "img_";
    }
    for (char c : name) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
            clean += c;
        } else {
            clean += '_';
        }
    }
    return clean;
}

std::map<std::string, std::string> PixelMathAlgorithm::getSanitizedVariableMap(const std::vector<std::string>& workspaceNames) {
    std::map<std::string, std::string> mapping;
    std::set<std::string> usedVarNames;
    for (const auto& rawName : workspaceNames) {
        std::string clean = sanitizeIdentifier(rawName);
        std::string varName = clean;
        int counter = 1;
        while (usedVarNames.count(varName)) {
            varName = clean + "_" + std::to_string(counter++);
        }
        usedVarNames.insert(varName);
        mapping[rawName] = varName;
    }
    return mapping;
}

std::string PixelMathAlgorithm::preprocessExpression(const std::string& expr, const std::map<std::string, std::string>& varMap) {
    std::string result;
    size_t i = 0;
    while (i < expr.length()) {
        if (expr[i] == '`') {
            size_t start = i + 1;
            size_t end = expr.find('`', start);
            if (end == std::string::npos) {
                throw std::runtime_error("Unmatched backtick in expression starting at position " + std::to_string(i));
            }
            std::string rawName = expr.substr(start, end - start);
            if (varMap.count(rawName) == 0) {
                throw std::runtime_error("Referenced image '" + rawName + "' is not present in the workspace registry");
            }
            result += varMap.at(rawName);
            i = end + 1;
        } else {
            result += expr[i];
            i++;
        }
    }
    return result;
}

} // namespace blastro
