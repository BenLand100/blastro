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

#include "PreprocessingPipeline.h"
#include "ImageOperations.h"
#include "StackingAlgorithm.h"
#include "CalibrationAlgorithm.h"
#include "DebayerAlgorithm.h"
#include "RegisterAlgorithm.h"
#include "StarFindingAlgorithm.h"
#include "BackgroundExtractionAlgorithm.h"
#include "AlignAlgorithm.h"
#include "PlatesolveAlgorithm.h"
#include "core/Logger.h"
#include "core/TempDirectory.h"
#include "core/MathUtils.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include "StarFinder.h"
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QElapsedTimer>
#include <limits>
#include "ConstellationMatcher.h"
#include <algorithm>
#include <set>
#include <cmath>
#include <sstream>

namespace blastro {



static GrayscaleImagePtr getRegistrationChannel(const ImageVariant& img) {
    if (std::holds_alternative<GrayscaleImagePtr>(img)) {
        return std::get<GrayscaleImagePtr>(img);
    } else if (std::holds_alternative<RGBImagePtr>(img)) {
        return std::get<RGBImagePtr>(img)->g();
    }
    return nullptr;
}


std::function<bool()> PreprocessingPipeline::s_cancelCallback = nullptr;

bool PreprocessingPipeline::isCancelled() {
    if (s_cancelCallback) {
        return s_cancelCallback();
    }
    return false;
}

static std::vector<std::string> splitString(const std::string& str, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::vector<PreprocessingGroup> PreprocessingPipeline::groupFiles(
    const std::vector<std::string>& biasPaths,
    const std::vector<std::string>& darkPaths,
    const std::vector<std::string>& flatPaths,
    const std::vector<std::string>& lightPaths,
    double expTolerance
) {
    std::vector<PreprocessingGroup> groups;

    auto addFileToGroup = [&](const std::string& filepath, const std::string& defaultType) {
        FitsHeaderInfo info;
        if (!FitsIO::readHeaderInfo(filepath, info)) {
            info.width = 0;
            info.height = 0;
            info.exposureTime = 0.0;
            info.filter = "None";
            info.imageType = defaultType;
            info.binningX = 1;
            info.binningY = 1;
            info.gain = 0.0;
        }
        if (info.imageType.empty()) info.imageType = defaultType;

        bool found = false;
        for (auto& g : groups) {
            if (g.type == info.imageType &&
                g.binningX == info.binningX &&
                g.binningY == info.binningY &&
                g.width == info.width &&
                g.height == info.height &&
                std::abs(g.gain - info.gain) < 1e-4) 
            {
                if (g.type == "Bias") {
                    found = true;
                } else if (g.type == "Dark") {
                    if (std::abs(g.exposure - info.exposureTime) <= expTolerance) {
                        found = true;
                    }
                } else if (g.type == "Flat") {
                    if (g.filter == info.filter) {
                        found = true;
                    }
                } else if (g.type == "Light") {
                    if (g.filter == info.filter && std::abs(g.exposure - info.exposureTime) <= expTolerance) {
                        found = true;
                    }
                }

                if (found) {
                    g.filepaths.push_back(filepath);
                    break;
                }
            }
        }

        if (!found) {
            PreprocessingGroup newGroup;
            newGroup.type = info.imageType;
            newGroup.exposure = info.exposureTime;
            newGroup.filter = info.filter;
            newGroup.binningX = info.binningX;
            newGroup.binningY = info.binningY;
            newGroup.width = info.width;
            newGroup.height = info.height;
            newGroup.gain = info.gain;
            newGroup.filepaths.push_back(filepath);
            groups.push_back(newGroup);
        }
    };

    for (const auto& p : biasPaths) addFileToGroup(p, "Bias");
    for (const auto& p : darkPaths) addFileToGroup(p, "Dark");
    for (const auto& p : flatPaths) addFileToGroup(p, "Flat");
    for (const auto& p : lightPaths) addFileToGroup(p, "Light");

    return groups;
}


void PreprocessingPipeline::execute(WorkspaceRegistry& workspace, 
                                     const std::map<std::string, std::string>& config, 
                                     ProgressCallback progress) {
    struct CancelScope {
        CancelScope(std::function<bool()> cb) {
            s_cancelCallback = cb;
        }
        ~CancelScope() {
            s_cancelCallback = nullptr;
        }
    };
    CancelScope cancelScope(m_cancelCallback);

    std::string stage = config.count("stage") ? config.at("stage") : "calibrate_register";
    std::string outDir = config.count("out_dir") ? config.at("out_dir") : ".";

    // Ensure output directory exists
    QDir dir(QString::fromStdString(outDir));
    if (!dir.exists()) {
        if (dir.mkpath(".")) {
            Logger::info("Preprocessing", QString("Created output directory: %1").arg(QString::fromStdString(outDir)));
        } else {
            Logger::error("Preprocessing", QString("Failed to create output directory: %1").arg(QString::fromStdString(outDir)));
            throw std::runtime_error("Failed to create output directory: " + outDir);
        }
    }

    double drizzleScale = config.count("drizzle_scale") ? std::stod(config.at("drizzle_scale")) : 1.0;
    double dropShrink = config.count("drop_shrink") ? std::stod(config.at("drop_shrink")) : 1.0;
    bool keepIntermediate = config.count("keep_intermediate") ? (config.at("keep_intermediate") == "true") : false;
    bool overwriteMasters = config.count("overwrite_masters") ? (config.at("overwrite_masters") == "true") : false;
    bool openCalibStacks = config.count("open_calib_stacks") ? (config.at("open_calib_stacks") == "true") : false;
    bool openLightMasters = config.count("open_light_masters") ? (config.at("open_light_masters") == "true") : true;

    FitsIO fits;

    std::vector<std::string> steps = getPlannedSteps(config);
    int totalSteps = steps.size();
    if (totalSteps == 0) totalSteps = 1;
    int currentStepIndex = 0;
    QElapsedTimer stepTimer;

    auto runStep = [&](Algorithm& alg, const std::map<std::string, std::string>& algConfig, std::function<void()> postOp = nullptr) {
        if (m_cancelCallback && m_cancelCallback()) {
            throw std::runtime_error("Preprocessing cancelled by user.");
        }
        if (m_stepCallback) {
            m_stepCallback(currentStepIndex, 0, 0.0, false, false, false);
        }
        stepTimer.start();
        
        try {
            alg.execute(workspace, algConfig, [this, &stepTimer, &progress, currentStepIndex, totalSteps](int pct) {
                if (m_stepCallback) {
                    m_stepCallback(currentStepIndex, pct, stepTimer.elapsed() / 1000.0, false, false, false);
                }
                if (progress) {
                    progress((currentStepIndex * 100 + pct) / totalSteps);
                }
            });
            
            if (postOp) {
                postOp();
            }

            if (m_stepCallback) {
                m_stepCallback(currentStepIndex, 100, stepTimer.elapsed() / 1000.0, true, true, false);
            }
            if (progress) {
                progress((currentStepIndex + 1) * 100 / totalSteps);
            }
            currentStepIndex++;
        } catch (...) {
            if (m_stepCallback) {
                m_stepCallback(currentStepIndex, 100, stepTimer.elapsed() / 1000.0, true, false, false);
            }
            throw;
        }

        if (m_cancelCallback && m_cancelCallback()) {
            throw std::runtime_error("Preprocessing cancelled by user.");
        }
    };

    if (stage == "calibrate_register") {
        Logger::header("Preprocessing", "Starting Calibration & Registration Stage");

        bool strictDark = config.count("strict_dark") ? (config.at("strict_dark") == "true") : false;
        double expTolerance = config.count("exp_tolerance") ? std::stod(config.at("exp_tolerance")) : 0.5;
        bool debayer = config.count("debayer") ? (config.at("debayer") == "true") : false;
        std::string bayerPattern = config.count("bayer_pattern") ? config.at("bayer_pattern") : "RGGB";
        std::string debayerMethod = config.count("debayer_method") ? config.at("debayer_method") : "bilinear";

        std::vector<std::string> biasFiles = splitString(config.count("bias_files") ? config.at("bias_files") : "", ';');
        std::vector<std::string> darkFiles = splitString(config.count("dark_files") ? config.at("dark_files") : "", ';');
        std::vector<std::string> flatFiles = splitString(config.count("flat_files") ? config.at("flat_files") : "", ';');
        std::vector<std::string> lightFiles = splitString(config.count("light_files") ? config.at("light_files") : "", ';');

        std::vector<PreprocessingGroup> groups = groupFiles(biasFiles, darkFiles, flatFiles, lightFiles, expTolerance);

        std::string outPrefix = config.count("out_prefix") ? config.at("out_prefix") : "";
        if (!outPrefix.empty() && outPrefix.back() != '_') {
            outPrefix += "_";
        }

        // Analyze staged groups for naming compression
        bool hasMultipleBinnings = false;
        bool hasMultipleResolutions = false;
        bool hasMultipleGains = false;
        std::map<std::string, std::set<double>> filterGains;

        if (!groups.empty()) {
            int firstBinX = groups[0].binningX;
            int firstBinY = groups[0].binningY;
            int firstW = groups[0].width;
            int firstH = groups[0].height;
            std::set<double> allGains;
            
            for (const auto& g : groups) {
                if (g.binningX != firstBinX || g.binningY != firstBinY) hasMultipleBinnings = true;
                if (g.width != firstW || g.height != firstH) hasMultipleResolutions = true;
                allGains.insert(g.gain);
                filterGains[g.filter].insert(g.gain);
            }
            hasMultipleGains = (allGains.size() > 1);
        }

        auto hasMultipleGainsForFilter = [&](const std::string& filter) -> bool {
            if (filterGains.count(filter)) {
                return filterGains[filter].size() > 1;
            }
            return false;
        };

        auto getShortNameKey = [&](const PreprocessingGroup& g, bool includeFilter, bool includeExposure) {
            std::string key = "";
            if (hasMultipleBinnings) {
                key += std::to_string(g.binningX) + "x" + std::to_string(g.binningY);
            }
            if (hasMultipleResolutions) {
                if (!key.empty()) key += "_";
                key += std::to_string(g.width) + "x" + std::to_string(g.height);
            }
            
            bool showGain = false;
            if (g.filter != "None") {
                showGain = hasMultipleGainsForFilter(g.filter);
            } else {
                showGain = hasMultipleGains;
            }
            
            if (showGain) {
                if (!key.empty()) key += "_";
                key += "g" + std::to_string(static_cast<int>(g.gain));
            }
            if (includeFilter && g.filter != "None") {
                if (!key.empty()) key += "_";
                key += g.filter;
            }
            if (includeExposure) {
                if (!key.empty()) key += "_";
                if (g.exposure < 1.0) {
                    key += std::to_string(static_cast<int>(std::round(g.exposure * 1000.0))) + "ms";
                } else {
                    key += std::to_string(static_cast<int>(std::round(g.exposure))) + "s";
                }
            }
            return key;
        };

        // Find/group masters
        std::map<std::string, std::string> masterBiasNames; // key: binningX_binningY_width_height
        std::map<std::string, std::string> masterDarkNames; // key: binningX_binningY_width_height_exposure
        std::map<std::string, std::string> masterFlatNames; // key: binningX_binningY_width_height_filter

        // 1. Stack Biases
        for (const auto& g : groups) {
            if (g.type == "Bias") {
                std::string key = std::to_string(g.binningX) + "_" + std::to_string(g.binningY) + "_" + std::to_string(g.width) + "_" + std::to_string(g.height) + "_g" + std::to_string(static_cast<int>(g.gain));
                std::string shortKey = getShortNameKey(g, false, false);
                std::string masterName = outPrefix + "master_bias" + (shortKey.empty() ? "" : "_" + shortKey);
                std::string masterPath = outDir + "/" + masterName + ".fits";

                if (!overwriteMasters && QFileInfo::exists(QString::fromStdString(masterPath))) {
                    Logger::info("Preprocessing", QString("Found cached master bias at %1. Loading it.").arg(QString::fromStdString(masterPath)));
                    Logger::info("Preprocessing", QString("Step '%1' skipped: using cached master.").arg(QString::fromStdString(steps[currentStepIndex])));
                    auto masterImg = fits.readImage(masterPath);
                    workspace.registerElement(masterName, std::visit([](auto&& arg) -> WorkspaceElement { return arg; }, masterImg), openCalibStacks);
                    masterBiasNames[key] = masterName;
                    if (m_stepCallback) {
                        m_stepCallback(currentStepIndex, 100, 0.0, true, true, true);
                    }
                    if (progress) {
                        progress((currentStepIndex + 1) * 100 / totalSteps);
                    }
                    currentStepIndex++;
                } else {
                    Logger::info("Preprocessing", QString("Stacking Master Bias for %1x%2 bin %3 gain %4").arg(g.width).arg(g.height).arg(g.binningX).arg(g.gain));

                    auto batch = fits.readBatch(g.filepaths);
                    std::string tempBatchName = "temp_bias_batch_" + key;
                    workspace.registerElement(tempBatchName, batch, false);

                    StackingAlgorithm stacker;
                    runStep(stacker, {
                        {"input_name", tempBatchName},
                        {"output_name", masterName},
                        {"visible", openCalibStacks ? "true" : "false"},
                        {"method", config.count("bias_dark_stack_method") ? config.at("bias_dark_stack_method") : "average"},
                        {"rejection", config.count("bias_dark_rejection") ? config.at("bias_dark_rejection") : "sigmaclip"},
                        {"sigma_low", config.count("bias_dark_sigma_low") ? config.at("bias_dark_sigma_low") : "3.0"},
                        {"sigma_high", config.count("bias_dark_sigma_high") ? config.at("bias_dark_sigma_high") : "3.0"},
                        {"scale_additive", config.count("bias_dark_scale_additive") ? config.at("bias_dark_scale_additive") : "false"},
                        {"scale_multiplicative", config.count("bias_dark_scale_multiplicative") ? config.at("bias_dark_scale_multiplicative") : "false"},
                        {"quantile_low", "0.2"},
                        {"quantile_high", "0.2"}
                    }, [&]() {
                        workspace.unregisterElement(tempBatchName);
                        masterBiasNames[key] = masterName;
                        auto masterElem = workspace.getElement(masterName);
                        fits.writeImage(masterPath, std::holds_alternative<GrayscaleImagePtr>(masterElem) ? ImageVariant{std::get<GrayscaleImagePtr>(masterElem)} : ImageVariant{std::get<RGBImagePtr>(masterElem)});
                    });
                }
            }
        }

        // 2. Stack Darks
        for (const auto& g : groups) {
            if (g.type == "Dark") {
                std::string key = std::to_string(g.binningX) + "_" + std::to_string(g.binningY) + "_" + std::to_string(g.width) + "_" + std::to_string(g.height) + "_g" + std::to_string(static_cast<int>(g.gain)) + "_" + std::to_string(static_cast<int>(g.exposure));
                std::string shortKey = getShortNameKey(g, false, true);
                std::string masterName = outPrefix + "master_dark" + (shortKey.empty() ? "" : "_" + shortKey);
                std::string masterPath = outDir + "/" + masterName + ".fits";

                if (!overwriteMasters && QFileInfo::exists(QString::fromStdString(masterPath))) {
                    Logger::info("Preprocessing", QString("Found cached master dark at %1. Loading it.").arg(QString::fromStdString(masterPath)));
                    Logger::info("Preprocessing", QString("Step '%1' skipped: using cached master.").arg(QString::fromStdString(steps[currentStepIndex])));
                    auto masterImg = fits.readImage(masterPath);
                    workspace.registerElement(masterName, std::visit([](auto&& arg) -> WorkspaceElement { return arg; }, masterImg), openCalibStacks);
                    masterDarkNames[key] = masterName;
                    if (m_stepCallback) {
                        m_stepCallback(currentStepIndex, 100, 0.0, true, true, true);
                    }
                    if (progress) {
                        progress((currentStepIndex + 1) * 100 / totalSteps);
                    }
                    currentStepIndex++;
                } else {
                    Logger::info("Preprocessing", QString("Stacking Master Dark for %1s exposure gain %2").arg(g.exposure).arg(g.gain));

                    auto batch = fits.readBatch(g.filepaths);
                    std::string tempBatchName = "temp_dark_batch_" + key;
                    workspace.registerElement(tempBatchName, batch, false);

                    StackingAlgorithm stacker;
                    runStep(stacker, {
                        {"input_name", tempBatchName},
                        {"output_name", masterName},
                        {"visible", openCalibStacks ? "true" : "false"},
                        {"method", config.count("bias_dark_stack_method") ? config.at("bias_dark_stack_method") : "average"},
                        {"rejection", config.count("bias_dark_rejection") ? config.at("bias_dark_rejection") : "sigmaclip"},
                        {"sigma_low", config.count("bias_dark_sigma_low") ? config.at("bias_dark_sigma_low") : "3.0"},
                        {"sigma_high", config.count("bias_dark_sigma_high") ? config.at("bias_dark_sigma_high") : "3.0"},
                        {"scale_additive", config.count("bias_dark_scale_additive") ? config.at("bias_dark_scale_additive") : "false"},
                        {"scale_multiplicative", config.count("bias_dark_scale_multiplicative") ? config.at("bias_dark_scale_multiplicative") : "false"},
                        {"quantile_low", "0.2"},
                        {"quantile_high", "0.2"}
                    }, [&]() {
                        workspace.unregisterElement(tempBatchName);
                        masterDarkNames[key] = masterName;
                        auto masterElem = workspace.getElement(masterName);
                        fits.writeImage(masterPath, std::holds_alternative<GrayscaleImagePtr>(masterElem) ? ImageVariant{std::get<GrayscaleImagePtr>(masterElem)} : ImageVariant{std::get<RGBImagePtr>(masterElem)});
                    });
                }
            }
        }

        // 3. Stack Flats
        for (const auto& g : groups) {
            if (g.type == "Flat") {
                std::string key = std::to_string(g.binningX) + "_" + std::to_string(g.binningY) + "_" + std::to_string(g.width) + "_" + std::to_string(g.height) + "_g" + std::to_string(static_cast<int>(g.gain)) + "_" + g.filter;
                std::string shortKey = getShortNameKey(g, true, false);
                std::string masterName = outPrefix + "master_flat" + (shortKey.empty() ? "" : "_" + shortKey);
                std::string masterPath = outDir + "/" + masterName + ".fits";

                if (!overwriteMasters && QFileInfo::exists(QString::fromStdString(masterPath))) {
                    Logger::info("Preprocessing", QString("Found cached master flat at %1. Loading it.").arg(QString::fromStdString(masterPath)));
                    auto masterImg = fits.readImage(masterPath);
                    workspace.registerElement(masterName, std::visit([](auto&& arg) -> WorkspaceElement { return arg; }, masterImg), openCalibStacks);
                    masterFlatNames[key] = masterName;
                    for (int step = 0; step < 2; ++step) {
                        Logger::info("Preprocessing", QString("Step '%1' skipped: using cached master.").arg(QString::fromStdString(steps[currentStepIndex])));
                        if (m_stepCallback) {
                            m_stepCallback(currentStepIndex, 100, 0.0, true, true, true);
                        }
                        if (progress) {
                            progress((currentStepIndex + 1) * 100 / totalSteps);
                        }
                        currentStepIndex++;
                    }
                } else {
                    Logger::info("Preprocessing", QString("Calibrating and Stacking Master Flat for filter %1 gain %2").arg(QString::fromStdString(g.filter)).arg(g.gain));

                    // Resolve matching calibration master for flats
                    std::string matchBiasKey = std::to_string(g.binningX) + "_" + std::to_string(g.binningY) + "_" + std::to_string(g.width) + "_" + std::to_string(g.height) + "_g" + std::to_string(static_cast<int>(g.gain));
                    std::string biasMaster = masterBiasNames.count(matchBiasKey) ? masterBiasNames[matchBiasKey] : "";

                    std::string darkMaster = "";
                    double bestDarkExp = -1.0;
                    for (const auto& pair : masterDarkNames) {
                        if (pair.first.find(matchBiasKey) == 0) { // Same dimensions/binning/gain
                            double darkExp = std::stod(splitString(pair.first, '_').back());
                            
                            bool match = false;
                            if (strictDark) {
                                match = (std::abs(darkExp - g.exposure) <= expTolerance);
                            } else {
                                match = (darkExp <= g.exposure + expTolerance);
                            }
                            
                            if (match) {
                                if (darkExp > bestDarkExp) {
                                    bestDarkExp = darkExp;
                                    darkMaster = pair.second;
                                }
                            }
                        }
                    }

                    std::string finalDark = "";
                    std::string finalBias = "";
                    if (!darkMaster.empty()) {
                        finalDark = darkMaster;
                    } else if (!biasMaster.empty()) {
                        finalBias = biasMaster;
                    }

                    auto batch = fits.readBatch(g.filepaths);
                    std::string tempBatchName = "temp_flat_batch_" + key;
                    workspace.registerElement(tempBatchName, batch, false);

                    std::string calibBatchName = "calib_flat_batch_" + key;

                    if (m_cancelCallback && m_cancelCallback()) {
                        throw std::runtime_error("Preprocessing cancelled by user.");
                    }

                    // 1. Calibrate Flats
                    CalibrationAlgorithm calibrator;
                    runStep(calibrator, {
                        {"input_name", tempBatchName},
                        {"output_name", calibBatchName},
                        {"visible", "false"},
                        {"bias_name", finalBias},
                        {"dark_name", finalDark},
                        {"flat_name", ""} // No flat for flats
                    }, [&]() {
                        if (keepIntermediate) {
                            relocateBatchFiles(workspace, calibBatchName, "flat_calib", outDir, true);
                        }
                    });

                    // 2. Stack Flats
                    StackingAlgorithm stacker;
                    runStep(stacker, {
                        {"input_name", calibBatchName},
                        {"output_name", masterName},
                        {"visible", openCalibStacks ? "true" : "false"},
                        {"method", config.count("flat_stack_method") ? config.at("flat_stack_method") : "average"},
                        {"rejection", config.count("flat_rejection") ? config.at("flat_rejection") : "sigmaclip"},
                        {"sigma_low", config.count("flat_sigma_low") ? config.at("flat_sigma_low") : "3.0"},
                        {"sigma_high", config.count("flat_sigma_high") ? config.at("flat_sigma_high") : "3.0"},
                        {"scale_additive", config.count("flat_scale_additive") ? config.at("flat_scale_additive") : "false"},
                        {"scale_multiplicative", config.count("flat_scale_multiplicative") ? config.at("flat_scale_multiplicative") : "false"},
                        {"quantile_low", "0.2"},
                        {"quantile_high", "0.2"}
                    }, [&]() {
                        workspace.unregisterElement(tempBatchName);
                        if (!keepIntermediate) {
                            workspace.unregisterElement(calibBatchName);
                        }

                        auto masterElem = workspace.getElement(masterName);
                        masterFlatNames[key] = masterName;
                        fits.writeImage(masterPath, std::holds_alternative<GrayscaleImagePtr>(masterElem) ? ImageVariant{std::get<GrayscaleImagePtr>(masterElem)} : ImageVariant{std::get<RGBImagePtr>(masterElem)});
                    });
                }
            }
        }

        // 4. Calibrate & Register Lights
        for (const auto& g : groups) {
            if (g.type == "Light") {
                std::string key = std::to_string(g.binningX) + "_" + std::to_string(g.binningY) + "_" + std::to_string(g.width) + "_" + std::to_string(g.height) + "_g" + std::to_string(static_cast<int>(g.gain)) + "_" + g.filter + "_" + std::to_string(static_cast<int>(g.exposure));
                std::string calibBatchName = "calib_light_batch_" + key;

                Logger::info("Preprocessing", QString("Calibrating Light batch for filter %1 gain %2").arg(QString::fromStdString(g.filter)).arg(g.gain));

                std::string matchBiasKey = std::to_string(g.binningX) + "_" + std::to_string(g.binningY) + "_" + std::to_string(g.width) + "_" + std::to_string(g.height) + "_g" + std::to_string(static_cast<int>(g.gain));
                std::string biasMaster = masterBiasNames.count(matchBiasKey) ? masterBiasNames[matchBiasKey] : "";

                std::string darkMaster = "";
                double bestDarkExp = -1.0;
                for (const auto& pair : masterDarkNames) {
                    if (pair.first.find(matchBiasKey) == 0) {
                        double darkExp = std::stod(splitString(pair.first, '_').back());
                        
                        bool match = false;
                        if (strictDark) {
                            match = (std::abs(darkExp - g.exposure) <= expTolerance);
                        } else {
                            match = (darkExp <= g.exposure + expTolerance);
                        }
                        
                        if (match) {
                            if (darkExp > bestDarkExp) {
                                bestDarkExp = darkExp;
                                darkMaster = pair.second;
                            }
                        }
                    }
                }

                std::string finalDark = "";
                std::string finalBias = "";
                if (!darkMaster.empty()) {
                    finalDark = darkMaster;
                } else if (!biasMaster.empty()) {
                    finalBias = biasMaster;
                }

                // Find matching master flat
                std::string flatMaster = "";
                std::string matchFlatKey = matchBiasKey + "_" + g.filter;
                if (masterFlatNames.count(matchFlatKey)) {
                    flatMaster = masterFlatNames[matchFlatKey];
                } else {
                    std::string keyPrefix = std::to_string(g.binningX) + "_" + std::to_string(g.binningY) + "_" + std::to_string(g.width) + "_" + std::to_string(g.height) + "_g";
                    std::string keySuffix = "_" + g.filter;
                    for (const auto& pair : masterFlatNames) {
                        if (pair.first.find(keyPrefix) == 0 && 
                            pair.first.length() >= keySuffix.length() &&
                            pair.first.compare(pair.first.length() - keySuffix.length(), keySuffix.length(), keySuffix) == 0) {
                            flatMaster = pair.second;
                            break;
                        }
                    }
                }

                auto batch = fits.readBatch(g.filepaths);
                std::string tempBatchName = "temp_light_batch_" + key;
                workspace.registerElement(tempBatchName, batch, false);

                CalibrationAlgorithm calibrator;
                runStep(calibrator, {
                    {"input_name", tempBatchName},
                    {"output_name", calibBatchName},
                    {"visible", "false"},
                    {"bias_name", finalBias},
                    {"dark_name", finalDark},
                    {"flat_name", flatMaster}
                }, [&]() {
                    workspace.unregisterElement(tempBatchName);
                    if (keepIntermediate) {
                        relocateBatchFiles(workspace, calibBatchName, "light_calib", outDir, true);
                    }
                });

                std::string registerInputName = calibBatchName;

                // Optionally Debayer
                if (debayer) {
                    std::string debayerBatchName = "debayer_light_batch_" + key;
                    Logger::info("Preprocessing", "Debayering calibrated lights...");
                    DebayerAlgorithm debayerer;
                    runStep(debayerer, {
                        {"input_name", calibBatchName},
                        {"output_name", debayerBatchName},
                        {"visible", "false"},
                        {"pattern", bayerPattern},
                        {"method", debayerMethod},
                        {"green_equalize", config.count("green_equalize") ? config.at("green_equalize") : "false"}
                    }, [&]() {
                        if (keepIntermediate) {
                            relocateBatchFiles(workspace, debayerBatchName, "light_calib", outDir, true);
                        } else {
                            workspace.unregisterElement(calibBatchName);
                        }
                    });
                    registerInputName = debayerBatchName;
                }

                std::string starMinSnr = config.count("star_min_snr") ? config.at("star_min_snr") : "4.0";
                std::string starMinFwhm = config.count("star_min_fwhm") ? config.at("star_min_fwhm") : "1.5";
                std::string starDetectMethod = config.count("star_detection_method") ? config.at("star_detection_method") : "adaptive";
                std::string starMaxStars = config.count("star_max_stars") ? config.at("star_max_stars") : "10000";
                std::string starMaxRefinedStars = config.count("star_max_refined_stars") ? config.at("star_max_refined_stars") : "250";
                std::string starMaxEcc = config.count("star_max_eccentricity") ? config.at("star_max_eccentricity") : "0.9";
                std::string transModel = config.count("transformation_model") ? config.at("transformation_model") : "rigid";
                std::string registerMaxStars = config.count("register_max_stars") ? config.at("register_max_stars") : starMaxRefinedStars;

                // Star finding
                Logger::info("Preprocessing", "Detecting stars in calibrated light frames...");
                StarFindingAlgorithm finder;
                runStep(finder, {
                    {"input_name", registerInputName},
                    {"detection_method", starDetectMethod},
                    {"snr_min", starMinSnr},
                    {"min_fwhm", starMinFwhm},
                    {"max_stars", starMaxStars},
                    {"max_refined_stars", starMaxRefinedStars},
                    {"max_eccentricity", starMaxEcc}
                });

                // Register batch
                Logger::info("Preprocessing", "Registering calibrated light frames...");
                RegisterAlgorithm registerer;
                runStep(registerer, {
                    {"input_name", registerInputName},
                    {"ref_frame_index", config.count("ref_frame_index") ? config.at("ref_frame_index") : ""},
                    {"reference_strategy", config.count("reference_strategy") ? config.at("reference_strategy") : "snr"},
                    {"max_stars", registerMaxStars},
                    {"match_tolerance", config.count("match_tolerance") ? config.at("match_tolerance") : "1.5"},
                    {"transformation_model", transModel}
                });


                std::string finalBatchName = outPrefix + "preprocessed_lights_" + g.filter;
                workspace.renameElement(registerInputName, finalBatchName);
                Logger::info("Preprocessing", QString("Registered Lights batch as '%1' in workspace.").arg(QString::fromStdString(finalBatchName)));
            }
        }

    } else if (stage == "align_stack") {
        Logger::header("Preprocessing", "Starting Alignment & Stacking Stage");

        std::string registeredBatchName = config.at("registered_light_batch_name");
        std::vector<std::string> batchNames = splitString(registeredBatchName, ';');

        bool alignMutually = config.count("align_mutually") ? (config.at("align_mutually") == "true") : true;

        struct BatchRegistration {
            std::string name;
            int bestFrameIdx;
            double maxSnr;
            std::vector<Star> refStars;
        };
        std::vector<BatchRegistration> batchRegs;

        bool runBGE = config.count("run_bge") ? (config.at("run_bge") == "true") : true;

        // 1. Finalize Registration
        if (m_stepCallback) {
            m_stepCallback(currentStepIndex, 0, 0.0, false, false, false);
        }
        stepTimer.start();

        std::vector<std::string> currentBatchNames = batchNames;

        // Pass 1: Find local reference frame and ensure stars exist for each batch
        for (const auto& name : batchNames) {
            if (m_cancelCallback && m_cancelCallback()) {
                throw std::runtime_error("Preprocessing cancelled by user.");
            }
            if (!workspace.contains(name)) {
                throw std::runtime_error("Workspace does not contain preprocessed batch: " + name);
            }

            auto batch = std::get<ImageBatchPtr>(workspace.getElement(name));
            int count = batch->count();

            int bestFrameIdx = -1;
            double maxSnr = -1.0;

            for (int i = 0; i < count; ++i) {
                if (batch->isFrameSelected(i)) {
                    auto meta = batch->frameMetadata(i);
                    if (meta.registered && meta.snr > maxSnr) {
                        maxSnr = meta.snr;
                        bestFrameIdx = i;
                    }
                }
            }

            if (bestFrameIdx == -1) {
                for (int i = 0; i < count; ++i) {
                    if (batch->isFrameSelected(i)) {
                        bestFrameIdx = i;
                        break;
                    }
                }
            }

            if (bestFrameIdx == -1) {
                throw std::runtime_error("No approved frames found in batch " + name + " for alignment.");
            }

            Logger::info("Preprocessing", QString("Selected local reference frame: index %1 (SNR = %2) for batch %3")
                         .arg(bestFrameIdx).arg(maxSnr).arg(QString::fromStdString(name)));

            FrameMetadata bestFrameMeta = batch->frameMetadata(bestFrameIdx);
            if (!bestFrameMeta.registered) {
                bestFrameMeta.registered = true;
                bestFrameMeta.transform = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
                bestFrameMeta.dx = 0.0;
                bestFrameMeta.dy = 0.0;
                bestFrameMeta.theta = 0.0;
                batch->setFrameMetadata(bestFrameIdx, bestFrameMeta);
            }

            // Retrieve registered stars of the local reference frame.
            std::vector<Star> refStars = bestFrameMeta.stars;
            if (refStars.size() < 3) {
                Logger::info("Preprocessing", QString("Detecting stars on local reference frame (index %1) for mutual alignment...")
                             .arg(bestFrameIdx));
                ImageVariant refFrame = batch->getImage(bestFrameIdx);
                auto refChannel = getRegistrationChannel(refFrame);
                if (!refChannel) {
                    throw std::runtime_error("Failed to extract registration channel from reference frame " + std::to_string(bestFrameIdx));
                }
                std::string starMinSnr = config.count("star_min_snr") ? config.at("star_min_snr") : "4.0";
                std::string starMinFwhm = config.count("star_min_fwhm") ? config.at("star_min_fwhm") : "1.5";
                std::string starDetectMethod = config.count("star_detection_method") ? config.at("star_detection_method") : "adaptive";
                std::string starMaxStars = config.count("star_max_stars") ? config.at("star_max_stars") : "10000";
                std::string starMaxRefinedStars = config.count("star_max_refined_stars") ? config.at("star_max_refined_stars") : "250";
                std::string starMaxEcc = config.count("star_max_eccentricity") ? config.at("star_max_eccentricity") : "0.9";

                double snrMin = std::stod(starMinSnr);
                double minFwhm = std::stod(starMinFwhm);
                int maxStars = std::stoi(starMaxStars);
                int maxRefined = std::stoi(starMaxRefinedStars);
                double maxEcc = std::stod(starMaxEcc);

                refStars = StarFinder::findStars(refChannel, maxStars, snrMin, starDetectMethod, 10, minFwhm, maxEcc, maxRefined);

                bestFrameMeta.stars = refStars;
                batch->setFrameMetadata(bestFrameIdx, bestFrameMeta);
            }

            batchRegs.push_back({name, bestFrameIdx, maxSnr, refStars});
        }

        // Pass 2: Find global reference batch (highest SNR reference frame overall)
        std::string globalRefBatchName = "";
        std::vector<Star> globalRefStars;
        double globalMaxSnr = -1.0;

        for (const auto& reg : batchRegs) {
            if (reg.maxSnr > globalMaxSnr) {
                globalMaxSnr = reg.maxSnr;
                globalRefBatchName = reg.name;
                globalRefStars = reg.refStars;
            }
        }

        if (globalRefBatchName.empty() && !batchRegs.empty()) {
            globalRefBatchName = batchRegs[0].name;
            globalRefStars = batchRegs[0].refStars;
        }

        Logger::info("Preprocessing", QString("Selected global reference batch: '%1' (SNR = %2)")
                     .arg(QString::fromStdString(globalRefBatchName)).arg(globalMaxSnr));

        // Pass 3: Compute and apply final transformations for each batch
        double matchTolerance = std::stod(config.count("match_tolerance") ? config.at("match_tolerance") : "1.5");
        bool useAffine = (config.count("transformation_model") && config.at("transformation_model") == "affine");
        int maxStars = std::stoi(config.count("star_max_refined_stars") ? config.at("star_max_refined_stars") : "250");

        for (const auto& reg : batchRegs) {
            if (m_cancelCallback && m_cancelCallback()) {
                throw std::runtime_error("Preprocessing cancelled by user.");
            }

            auto batch = std::get<ImageBatchPtr>(workspace.getElement(reg.name));
            int count = batch->count();

            // Find T_mutual that maps this batch's local ref to the global ref
            std::array<double, 6> T_mutual = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
            if (alignMutually && reg.name != globalRefBatchName) {
                Logger::info("Preprocessing", QString("Matching reference frame of batch '%1' to global reference...")
                             .arg(QString::fromStdString(reg.name)));
                auto alignRes = ConstellationMatcher::match(globalRefStars, reg.refStars, 7, matchTolerance, useAffine, maxStars);
                if (!alignRes.success) {
                    throw std::runtime_error("Failed to mutually align reference frame of batch " + reg.name + 
                                             " to the global reference frame. Verify that the frames overlap and contain matching star patterns.");
                }
                T_mutual = alignRes.transform;
                Logger::success("Preprocessing", QString("Mutual alignment of batch '%1' successful: dx=%2, dy=%3, theta=%4")
                                .arg(QString::fromStdString(reg.name)).arg(alignRes.dx).arg(alignRes.dy).arg(alignRes.theta));
            }

            // We compose the transformation: T_final = T_mutual * T_local_inv * T_original
            FrameMetadata localRefMeta = batch->frameMetadata(reg.bestFrameIdx);
            std::array<double, 6> T_local_inv = MathUtils::invertAffine(localRefMeta.transform);
            std::array<double, 6> T_combined = MathUtils::multiplyAffine(T_mutual, T_local_inv);

            // Apply combined transform to all approved frames in the batch
            for (int i = 0; i < count; ++i) {
                if (batch->isFrameSelected(i)) {
                    FrameMetadata meta = batch->frameMetadata(i);
                    if (meta.registered) {
                        std::array<double, 6> T_final = MathUtils::multiplyAffine(T_combined, meta.transform);
                        meta.transform = T_final;
                        meta.dx = T_final[2];
                        meta.dy = T_final[5];
                        meta.theta = std::atan2(T_final[3] - T_final[1], T_final[0] + T_final[4]);
                        batch->setFrameMetadata(i, meta);
                    }
                }
            }
        }

        // Calculate global average center offsets if needed
        bool hasCustomRef = false;
        double globalRefDx = 0.0;
        double globalRefDy = 0.0;
        double globalRefTheta = 0.0;
        std::string alignRefMode = config.count("align_ref_mode") ? config.at("align_ref_mode") : "average_center";

        if (alignMutually && !globalRefBatchName.empty()) {
            if (alignRefMode == "average_center") {
                auto globalBatch = std::get<ImageBatchPtr>(workspace.getElement(globalRefBatchName));
                int globalCount = globalBatch->count();
                double sumDx = 0.0;
                double sumDy = 0.0;
                int validCount = 0;
                for (int i = 0; i < globalCount; ++i) {
                    if (globalBatch->isFrameSelected(i)) {
                        FrameMetadata meta = globalBatch->frameMetadata(i);
                        if (meta.registered) {
                            sumDx += meta.dx;
                            sumDy += meta.dy;
                            validCount++;
                        }
                    }
                }
                if (validCount > 0) {
                    double avgDx = sumDx / validCount;
                    double avgDy = sumDy / validCount;
                    double minDist = std::numeric_limits<double>::max();
                    int centerFrameIdx = -1;
                    for (int i = 0; i < globalCount; ++i) {
                        if (globalBatch->isFrameSelected(i)) {
                            FrameMetadata meta = globalBatch->frameMetadata(i);
                            if (meta.registered) {
                                double dist = (meta.dx - avgDx) * (meta.dx - avgDx) + 
                                              (meta.dy - avgDy) * (meta.dy - avgDy);
                                if (dist < minDist) {
                                    minDist = dist;
                                    centerFrameIdx = i;
                                }
                            }
                        }
                    }
                    if (centerFrameIdx != -1) {
                        FrameMetadata centerMeta = globalBatch->frameMetadata(centerFrameIdx);
                        globalRefDx = centerMeta.dx;
                        globalRefDy = centerMeta.dy;
                        globalRefTheta = centerMeta.theta;
                        hasCustomRef = true;
                        Logger::info("Preprocessing", QString("Global Average Center Mode: Selected frame %1 in batch '%2' as center reference (offsets relative to global ref: dx=%3, dy=%4, theta=%5)")
                                     .arg(centerFrameIdx)
                                     .arg(QString::fromStdString(globalRefBatchName))
                                     .arg(globalRefDx).arg(globalRefDy).arg(globalRefTheta));
                    }
                }
            } else if (alignRefMode == "registration") {
                globalRefDx = 0.0;
                globalRefDy = 0.0;
                globalRefTheta = 0.0;
                hasCustomRef = true;
            }
        }

        if (m_stepCallback) {
            m_stepCallback(currentStepIndex, 100, stepTimer.elapsed() / 1000.0, true, true, false);
        }
        if (progress) {
            progress((currentStepIndex + 1) * 100 / totalSteps);
        }
        currentStepIndex++;

        // 1.5 Background Normalization Phase (BGE)
        if (runBGE) {
            for (size_t k = 0; k < batchNames.size(); ++k) {
                std::string name = batchNames[k];
                std::string bgeBatchName = name + "_bge";
                int bestFrameIdx = batchRegs[k].bestFrameIdx;
                
                int cols = config.count("bge_grid_cols") ? std::stoi(config.at("bge_grid_cols")) : 10;
                int rows = config.count("bge_grid_rows") ? std::stoi(config.at("bge_grid_rows")) : 10;
                int gridSize = std::max(cols, rows);

                BackgroundExtractionAlgorithm bge;
                std::map<std::string, std::string> bgeConfig = {
                    {"input_name", name},
                    {"output_name", bgeBatchName},
                    {"visible", "false"},
                    {"order", config.count("bge_poly_order") ? config.at("bge_poly_order") : "3"},
                    {"grid_size", std::to_string(gridSize)},
                    {"method", config.count("bge_model_method") ? config.at("bge_model_method") : "Polynomial"},
                    {"rbf_smoothing", config.count("bge_rbf_smoothing") ? config.at("bge_rbf_smoothing") : "0.01"},
                    {"auto_exclude", config.count("bge_auto_exclude") ? config.at("bge_auto_exclude") : "false"},
                    {"max_deviation", config.count("bge_max_deviation") ? config.at("bge_max_deviation") : "3.0"},
                    {"max_structure", config.count("bge_max_structure") ? config.at("bge_max_structure") : "1.5"},
                    {"normalize", "true"},
                    {"ref_frame_index", std::to_string(bestFrameIdx)}
                };
                
                Logger::info("Preprocessing", QString("Running Background Normalization on batch '%1'...").arg(QString::fromStdString(name)));
                runStep(bge, bgeConfig, [&]() {
                    currentBatchNames[k] = bgeBatchName;
                    batchRegs[k].name = bgeBatchName;
                });
            }
        }

        std::vector<std::string> finalMasterNames;
        std::vector<std::vector<ImageOperations::FrameTransformInfo>> allBatchFrameTransforms;

        // 3. Align and Stack Phase: Align and stack each batch
        for (size_t k = 0; k < currentBatchNames.size(); ++k) {
            std::string name = currentBatchNames[k];
            std::string origName = batchNames[k];

            if (m_cancelCallback && m_cancelCallback()) {
                throw std::runtime_error("Preprocessing cancelled by user.");
            }

            std::string alignedBatchName = name + "_aligned";
            Logger::info("Preprocessing", QString("Aligning light frames to reference for batch %1...").arg(QString::fromStdString(name)));

            std::string interpMethod = config.count("interpolation_method") ? config.at("interpolation_method") : "bilinear";
            std::map<std::string, std::string> alignConfig = {
                {"input_name", name},
                {"output_name", alignedBatchName},
                {"visible", "false"},
                {"drizzle_scale", std::to_string(drizzleScale)},
                {"reference_mode", alignRefMode},
                {"interpolation_method", interpMethod},
                {"drop_shrink", std::to_string(dropShrink)},
                {"evict_cache", "true"}
            };
            if (hasCustomRef) {
                alignConfig["ref_dx"] = std::to_string(globalRefDx);
                alignConfig["ref_dy"] = std::to_string(globalRefDy);
                alignConfig["ref_theta"] = std::to_string(globalRefTheta);
            }

            AlignAlgorithm aligner;
            runStep(aligner, alignConfig, [&]() {
                if (keepIntermediate) {
                    relocateBatchFiles(workspace, alignedBatchName, "light_align", outDir, true);
                }
            });

            std::vector<ImageOperations::FrameTransformInfo> batchFrames;
            std::string sourceBatchName = workspace.contains(alignedBatchName) ? alignedBatchName : (workspace.contains(name) ? name : "");
            if (!sourceBatchName.empty()) {
                auto batch = std::get<ImageBatchPtr>(workspace.getElement(sourceBatchName));
                int count = batch->count();
                for (int i = 0; i < count; ++i) {
                    if (batch->isFrameSelected(i)) {
                        FrameMetadata meta = batch->frameMetadata(i);
                        if (meta.registered) {
                            std::array<double, 6> T_target = meta.transform;
                            auto imgVar = batch->getImage(i);
                            int w = 0, h = 0;
                            if (std::holds_alternative<GrayscaleImagePtr>(imgVar)) {
                                auto g = std::get<GrayscaleImagePtr>(imgVar);
                                if (g) { w = g->width(); h = g->height(); }
                            } else if (std::holds_alternative<RGBImagePtr>(imgVar)) {
                                auto rgb = std::get<RGBImagePtr>(imgVar);
                                if (rgb) { w = rgb->width(); h = rgb->height(); }
                            }
                            if (w > 0 && h > 0) {
                                batchFrames.push_back({T_target, w, h});
                            }
                        }
                    }
                }
            }
            allBatchFrameTransforms.push_back(batchFrames);

            std::string finalMasterName = origName + "_stacked";
            if (workspace.contains(finalMasterName)) {
                std::string base = finalMasterName;
                int suffix = 1;
                std::string candidate = base + "_" + std::to_string(suffix);
                while (workspace.contains(candidate)) {
                    suffix++;
                    candidate = base + "_" + std::to_string(suffix);
                }
                finalMasterName = candidate;
            }
            finalMasterNames.push_back(finalMasterName);

            Logger::info("Preprocessing", QString("Stacking final preprocessed master light for batch %1...").arg(QString::fromStdString(name)));
            
            bool scaleAdditive = config.count("light_scale_additive") ? (config.at("light_scale_additive") == "true") : true;
            bool scaleMultiplicative = config.count("light_scale_multiplicative") ? (config.at("light_scale_multiplicative") == "true") : true;
            bool autocrop = config.count("autocrop") ? (config.at("autocrop") == "true") : false;
            bool correctPedestal = config.count("correct_pedestal") ? (config.at("correct_pedestal") == "true") : false;
            bool clampMax = config.count("clamp_max") ? (config.at("clamp_max") == "true") : false;
            bool platesolveStacks = config.count("platesolve_stacks") ? (config.at("platesolve_stacks") == "true") : false;
            bool saveInStacking = !autocrop && !correctPedestal && !clampMax && !platesolveStacks;

            StackingAlgorithm stacker;
            runStep(stacker, {
                {"input_name", alignedBatchName},
                {"output_name", finalMasterName},
                {"visible", openLightMasters ? "true" : "false"},
                {"method", config.count("light_stack_method") ? config.at("light_stack_method") : "average"},
                {"rejection", config.count("light_rejection") ? config.at("light_rejection") : "winsorized"},
                {"sigma_low", config.count("light_sigma_low") ? config.at("light_sigma_low") : "3.0"},
                {"sigma_high", config.count("light_sigma_high") ? config.at("light_sigma_high") : "3.0"},
                {"quantile_low", "0.2"},
                {"quantile_high", "0.2"},
                {"weight_method", config.count("light_weight_method") ? config.at("light_weight_method") : "none"},
                {"scale_additive", scaleAdditive ? "true" : "false"},
                {"scale_multiplicative", scaleMultiplicative ? "true" : "false"}
            }, [&]() {
                if (!keepIntermediate) {
                    if (workspace.contains(alignedBatchName)) {
                        workspace.unregisterElement(alignedBatchName);
                    }
                    if (workspace.contains(name)) {
                        workspace.unregisterElement(name);
                    }
                    if (workspace.contains(origName) && origName != finalMasterName) {
                        workspace.unregisterElement(origName);
                    }
                }
                if (saveInStacking) {
                    auto masterElem = workspace.getElement(finalMasterName);
                    std::string finalOutPath = outDir + "/" + finalMasterName + ".fits";
                    fits.writeImage(finalOutPath, std::holds_alternative<GrayscaleImagePtr>(masterElem) ? ImageVariant{std::get<GrayscaleImagePtr>(masterElem)} : ImageVariant{std::get<RGBImagePtr>(masterElem)});
                    Logger::info("Preprocessing", QString("Saved final stacked master light to: %1").arg(QString::fromStdString(finalOutPath)));
                }
            });
        }

        bool autocrop = config.count("autocrop") ? (config.at("autocrop") == "true") : false;
        bool correctPedestal = config.count("correct_pedestal") ? (config.at("correct_pedestal") == "true") : false;
        bool clampMax = config.count("clamp_max") ? (config.at("clamp_max") == "true") : false;
        bool platesolveStacks = config.count("platesolve_stacks") ? (config.at("platesolve_stacks") == "true") : false;

        auto toWorkspaceElement = [](const ImageVariant& var) -> WorkspaceElement {
            if (std::holds_alternative<GrayscaleImagePtr>(var)) {
                return std::get<GrayscaleImagePtr>(var);
            } else if (std::holds_alternative<RGBImagePtr>(var)) {
                return std::get<RGBImagePtr>(var);
            }
            return WorkspaceElement();
        };

        auto toImageVariant = [](const WorkspaceElement& elem) -> ImageVariant {
            if (std::holds_alternative<GrayscaleImagePtr>(elem)) {
                return std::get<GrayscaleImagePtr>(elem);
            } else if (std::holds_alternative<RGBImagePtr>(elem)) {
                return std::get<RGBImagePtr>(elem);
            }
            return ImageVariant();
        };

        // 4. Autocrop Phase
        if (autocrop) {
            if (alignMutually) {
                if (m_cancelCallback && m_cancelCallback()) {
                    throw std::runtime_error("Preprocessing cancelled by user.");
                }
                if (m_stepCallback) {
                    m_stepCallback(currentStepIndex, 0, 0.0, false, false, false);
                }
                stepTimer.start();

                try {
                    std::vector<ImageOperations::FrameTransformInfo> allFrames;
                    int targetW = 0;
                    int targetH = 0;

                    for (size_t k = 0; k < currentBatchNames.size(); ++k) {
                        if (k < allBatchFrameTransforms.size()) {
                            for (const auto& ft : allBatchFrameTransforms[k]) {
                                allFrames.push_back(ft);
                            }
                        }

                        if (targetW == 0 && targetH == 0 && k < finalMasterNames.size() && workspace.contains(finalMasterNames[k])) {
                            auto masterElem = workspace.getElement(finalMasterNames[k]);
                            if (std::holds_alternative<GrayscaleImagePtr>(masterElem)) {
                                targetW = std::get<GrayscaleImagePtr>(masterElem)->width();
                                targetH = std::get<GrayscaleImagePtr>(masterElem)->height();
                            } else if (std::holds_alternative<RGBImagePtr>(masterElem)) {
                                targetW = std::get<RGBImagePtr>(masterElem)->width();
                                targetH = std::get<RGBImagePtr>(masterElem)->height();
                            }
                        }
                    }

                    QRect cropRect = ImageOperations::findLargestBoundingRectangle(allFrames, targetW, targetH, drizzleScale);

                    bool performCrop = (cropRect.width() > 0 && cropRect.height() > 0 &&
                                        (cropRect.width() < targetW || cropRect.height() < targetH || cropRect.x() > 0 || cropRect.y() > 0));

                    if (performCrop) {
                        Logger::info("Preprocessing", QString("Autocrop (mutually aligned): Crop rectangle (%1, %2, %3x%4) out of target (%5x%6), keeping %7% area")
                                     .arg(cropRect.x()).arg(cropRect.y()).arg(cropRect.width()).arg(cropRect.height())
                                     .arg(targetW).arg(targetH)
                                     .arg(QString::number(100.0 * (cropRect.width() * cropRect.height()) / (targetW * targetH), 'f', 1)));

                        for (const auto& masterName : finalMasterNames) {
                            if (workspace.contains(masterName)) {
                                auto masterElem = workspace.getElement(masterName);
                                auto croppedVar = ImageOperations::crop(toImageVariant(masterElem), cropRect);
                                workspace.registerElement(masterName, toWorkspaceElement(croppedVar), openLightMasters);

                                if (!correctPedestal && !clampMax && !platesolveStacks) {
                                    std::string finalOutPath = outDir + "/" + masterName + ".fits";
                                    fits.writeImage(finalOutPath, croppedVar);
                                    Logger::info("Preprocessing", QString("Saved autocropped stacked master light to: %1").arg(QString::fromStdString(finalOutPath)));
                                }
                            }
                        }
                    } else {
                        Logger::info("Preprocessing", "Autocrop (mutually aligned): Full frame is covered across all stacks, no crop needed.");
                    }

                    if (m_stepCallback) {
                        m_stepCallback(currentStepIndex, 100, stepTimer.elapsed() / 1000.0, true, true, false);
                    }
                    if (progress) {
                        progress((currentStepIndex + 1) * 100 / totalSteps);
                    }
                    currentStepIndex++;
                } catch (...) {
                    if (m_stepCallback) {
                        m_stepCallback(currentStepIndex, 100, stepTimer.elapsed() / 1000.0, true, false, false);
                    }
                    throw;
                }
            } else {
                for (size_t k = 0; k < currentBatchNames.size(); ++k) {
                    if (m_cancelCallback && m_cancelCallback()) {
                        throw std::runtime_error("Preprocessing cancelled by user.");
                    }
                    if (m_stepCallback) {
                        m_stepCallback(currentStepIndex, 0, 0.0, false, false, false);
                    }
                    stepTimer.start();

                    try {
                        std::string masterName = finalMasterNames[k];

                        std::vector<ImageOperations::FrameTransformInfo> stackFrames;
                        if (k < allBatchFrameTransforms.size()) {
                            stackFrames = allBatchFrameTransforms[k];
                        }
                        int targetW = 0;
                        int targetH = 0;

                        if (workspace.contains(masterName)) {
                            auto masterElem = workspace.getElement(masterName);
                            if (std::holds_alternative<GrayscaleImagePtr>(masterElem)) {
                                targetW = std::get<GrayscaleImagePtr>(masterElem)->width();
                                targetH = std::get<GrayscaleImagePtr>(masterElem)->height();
                            } else if (std::holds_alternative<RGBImagePtr>(masterElem)) {
                                targetW = std::get<RGBImagePtr>(masterElem)->width();
                                targetH = std::get<RGBImagePtr>(masterElem)->height();
                            }
                        }

                        QRect cropRect = ImageOperations::findLargestBoundingRectangle(stackFrames, targetW, targetH, drizzleScale);

                        bool performCrop = (cropRect.width() > 0 && cropRect.height() > 0 &&
                                            (cropRect.width() < targetW || cropRect.height() < targetH || cropRect.x() > 0 || cropRect.y() > 0));

                        if (performCrop && workspace.contains(masterName)) {
                            Logger::info("Preprocessing", QString("Autocrop (%1): Crop rectangle (%2, %3, %4x%5) out of target (%6x%7), keeping %8% area")
                                         .arg(QString::fromStdString(masterName))
                                         .arg(cropRect.x()).arg(cropRect.y()).arg(cropRect.width()).arg(cropRect.height())
                                         .arg(targetW).arg(targetH)
                                         .arg(QString::number(100.0 * (cropRect.width() * cropRect.height()) / (targetW * targetH), 'f', 1)));

                            auto masterElem = workspace.getElement(masterName);
                            auto croppedVar = ImageOperations::crop(toImageVariant(masterElem), cropRect);
                            workspace.registerElement(masterName, toWorkspaceElement(croppedVar), openLightMasters);

                            if (!correctPedestal && !clampMax && !platesolveStacks) {
                                std::string finalOutPath = outDir + "/" + masterName + ".fits";
                                fits.writeImage(finalOutPath, croppedVar);
                                Logger::info("Preprocessing", QString("Saved autocropped stacked master light to: %1").arg(QString::fromStdString(finalOutPath)));
                            }
                        } else {
                            Logger::info("Preprocessing", QString("Autocrop (%1): Full frame is covered, no crop needed.").arg(QString::fromStdString(masterName)));
                        }

                        if (m_stepCallback) {
                            m_stepCallback(currentStepIndex, 100, stepTimer.elapsed() / 1000.0, true, true, false);
                        }
                        if (progress) {
                            progress((currentStepIndex + 1) * 100 / totalSteps);
                        }
                        currentStepIndex++;
                    } catch (...) {
                        if (m_stepCallback) {
                            m_stepCallback(currentStepIndex, 100, stepTimer.elapsed() / 1000.0, true, false, false);
                        }
                        throw;
                    }
                }
            }
        }

        // 5. Pedestal / Clamp Max Phase
        if (correctPedestal || clampMax) {
            for (const auto& finalMasterName : finalMasterNames) {
                if (m_cancelCallback && m_cancelCallback()) {
                    throw std::runtime_error("Preprocessing cancelled by user.");
                }
                if (m_stepCallback) {
                    m_stepCallback(currentStepIndex, 0, 0.0, false, false, false);
                }
                stepTimer.start();
                try {
                    auto masterElem = workspace.getElement(finalMasterName);
                    std::vector<ImageBuffer*> buffers;
                    if (std::holds_alternative<GrayscaleImagePtr>(masterElem)) {
                        buffers.push_back(std::get<GrayscaleImagePtr>(masterElem)->buffer().get());
                    } else if (std::holds_alternative<RGBImagePtr>(masterElem)) {
                        auto rgb = std::get<RGBImagePtr>(masterElem);
                        buffers.push_back(rgb->r()->buffer().get());
                        buffers.push_back(rgb->g()->buffer().get());
                        buffers.push_back(rgb->b()->buffer().get());
                    }

                    if (correctPedestal) {
                        float globalMin = std::numeric_limits<float>::max();
                        for (auto* buf : buffers) {
                            float* data = buf->data();
                            int n = buf->width() * buf->height();
                            for (int i = 0; i < n; ++i) {
                                float val = data[i];
                                if (!std::isnan(val) && val < globalMin) {
                                    globalMin = val;
                                }
                            }
                        }

                        if (globalMin < 0.0f) {
                            float addOffset = -globalMin;
                            Logger::info("Preprocessing", QString("Correcting pedestal for '%1': adding %2 (negative minimum offset)")
                                         .arg(QString::fromStdString(finalMasterName)).arg(addOffset));
                            for (auto* buf : buffers) {
                                float* data = buf->data();
                                int n = buf->width() * buf->height();
                                #pragma omp parallel for
                                for (int i = 0; i < n; ++i) {
                                    if (!std::isnan(data[i])) {
                                        data[i] += addOffset;
                                    }
                                }
                            }
                        }
                    }

                    if (clampMax) {
                        Logger::info("Preprocessing", QString("Clamping maximum values to 1.0 for '%1'")
                                     .arg(QString::fromStdString(finalMasterName)));
                        for (auto* buf : buffers) {
                            float* data = buf->data();
                            int n = buf->width() * buf->height();
                            #pragma omp parallel for
                            for (int i = 0; i < n; ++i) {
                                if (!std::isnan(data[i]) && data[i] > 1.0f) {
                                    data[i] = 1.0f;
                                }
                            }
                        }
                    }

                    if (!platesolveStacks) {
                        std::string finalOutPath = outDir + "/" + finalMasterName + ".fits";
                        fits.writeImage(finalOutPath, std::holds_alternative<GrayscaleImagePtr>(masterElem) ? ImageVariant{std::get<GrayscaleImagePtr>(masterElem)} : ImageVariant{std::get<RGBImagePtr>(masterElem)});
                        Logger::info("Preprocessing", QString("Saved final stacked master light to: %1").arg(QString::fromStdString(finalOutPath)));
                    }

                    if (m_stepCallback) {
                        m_stepCallback(currentStepIndex, 100, stepTimer.elapsed() / 1000.0, true, true, false);
                    }
                    if (progress) {
                        progress((currentStepIndex + 1) * 100 / totalSteps);
                    }
                    currentStepIndex++;
                } catch (...) {
                    if (m_stepCallback) {
                        m_stepCallback(currentStepIndex, 100, stepTimer.elapsed() / 1000.0, true, false, false);
                    }
                    throw;
                }
            }
        }

        // 6. Platesolve Phase
        if (platesolveStacks) {
            for (const auto& finalMasterName : finalMasterNames) {
                Logger::info("Preprocessing", QString("Platesolving stacked master '%1'...").arg(QString::fromStdString(finalMasterName)));
                bool blindSolve = config.count("platesolve_blind") ? (config.at("platesolve_blind") == "true") : true;
                std::string raHint = blindSolve ? "-1.0" : (config.count("platesolve_ra_hint") ? config.at("platesolve_ra_hint") : "-1.0");
                std::string decHint = blindSolve ? "-99.0" : (config.count("platesolve_dec_hint") ? config.at("platesolve_dec_hint") : "-99.0");

                PlatesolveAlgorithm solverAlg;
                runStep(solverAlg, {
                    {"input_name", finalMasterName},
                    {"solver", config.count("platesolve_solver") ? config.at("platesolve_solver") : "astap"},
                    {"ra_hint", raHint},
                    {"dec_hint", decHint},
                    {"focal_length", config.count("platesolve_focal_length") ? config.at("platesolve_focal_length") : "0.0"},
                    {"pixel_size", config.count("platesolve_pixel_size") ? config.at("platesolve_pixel_size") : "0.0"}
                }, [&]() {
                    auto masterElem = workspace.getElement(finalMasterName);
                    std::string finalOutPath = outDir + "/" + finalMasterName + ".fits";
                    fits.writeImage(finalOutPath, std::holds_alternative<GrayscaleImagePtr>(masterElem) ? ImageVariant{std::get<GrayscaleImagePtr>(masterElem)} : ImageVariant{std::get<RGBImagePtr>(masterElem)});
                    Logger::info("Preprocessing", QString("Saved final stacked master light to: %1").arg(QString::fromStdString(finalOutPath)));
                });
            }
        }
    }
}

std::vector<std::string> PreprocessingPipeline::getPlannedSteps(const std::map<std::string, std::string>& config) {
    std::string stage = config.count("stage") ? config.at("stage") : "calibrate_register";
    std::vector<std::string> steps;

    if (stage == "calibrate_register") {
        double expTolerance = config.count("exp_tolerance") ? std::stod(config.at("exp_tolerance")) : 0.5;
        bool debayer = config.count("debayer") ? (config.at("debayer") == "true") : false;

        std::vector<std::string> biasFiles = splitString(config.count("bias_files") ? config.at("bias_files") : "", ';');
        std::vector<std::string> darkFiles = splitString(config.count("dark_files") ? config.at("dark_files") : "", ';');
        std::vector<std::string> flatFiles = splitString(config.count("flat_files") ? config.at("flat_files") : "", ';');
        std::vector<std::string> lightFiles = splitString(config.count("light_files") ? config.at("light_files") : "", ';');

        std::vector<PreprocessingGroup> groups = groupFiles(biasFiles, darkFiles, flatFiles, lightFiles, expTolerance);

        std::string outPrefix = config.count("out_prefix") ? config.at("out_prefix") : "";
        if (!outPrefix.empty() && outPrefix.back() != '_') {
            outPrefix += "_";
        }

        // Analyze staged groups for naming compression
        bool hasMultipleBinnings = false;
        bool hasMultipleResolutions = false;
        bool hasMultipleGains = false;
        std::map<std::string, std::set<double>> filterGains;

        if (!groups.empty()) {
            int firstBinX = groups[0].binningX;
            int firstBinY = groups[0].binningY;
            int firstW = groups[0].width;
            int firstH = groups[0].height;
            std::set<double> allGains;
            
            for (const auto& g : groups) {
                if (g.binningX != firstBinX || g.binningY != firstBinY) hasMultipleBinnings = true;
                if (g.width != firstW || g.height != firstH) hasMultipleResolutions = true;
                allGains.insert(g.gain);
                filterGains[g.filter].insert(g.gain);
            }
            hasMultipleGains = (allGains.size() > 1);
        }

        auto hasMultipleGainsForFilter = [&](const std::string& filter) -> bool {
            if (filterGains.count(filter)) {
                return filterGains[filter].size() > 1;
            }
            return false;
        };

        auto getShortNameKey = [&](const PreprocessingGroup& g, bool includeFilter, bool includeExposure) {
            std::string key = "";
            if (hasMultipleBinnings) {
                key += std::to_string(g.binningX) + "x" + std::to_string(g.binningY);
            }
            if (hasMultipleResolutions) {
                if (!key.empty()) key += "_";
                key += std::to_string(g.width) + "x" + std::to_string(g.height);
            }
            
            bool showGain = false;
            if (g.filter != "None") {
                showGain = hasMultipleGainsForFilter(g.filter);
            } else {
                showGain = hasMultipleGains;
            }
            
            if (showGain) {
                if (!key.empty()) key += "_";
                key += "g" + std::to_string(static_cast<int>(g.gain));
            }
            if (includeFilter && g.filter != "None") {
                if (!key.empty()) key += "_";
                key += g.filter;
            }
            if (includeExposure) {
                if (!key.empty()) key += "_";
                if (g.exposure < 1.0) {
                    key += std::to_string(static_cast<int>(std::round(g.exposure * 1000.0))) + "ms";
                } else {
                    key += std::to_string(static_cast<int>(std::round(g.exposure))) + "s";
                }
            }
            return key;
        };

        // 1. Stack Biases
        for (const auto& g : groups) {
            if (g.type == "Bias") {
                std::string shortKey = getShortNameKey(g, false, false);
                std::string masterName = outPrefix + "master_bias" + (shortKey.empty() ? "" : "_" + shortKey);
                steps.push_back("Stack " + masterName);
            }
        }
        // 2. Stack Darks
        for (const auto& g : groups) {
            if (g.type == "Dark") {
                std::string shortKey = getShortNameKey(g, false, true);
                std::string masterName = outPrefix + "master_dark" + (shortKey.empty() ? "" : "_" + shortKey);
                steps.push_back("Stack " + masterName);
            }
        }
        // 3. Stack Flats
        for (const auto& g : groups) {
            if (g.type == "Flat") {
                std::string shortKey = getShortNameKey(g, true, false);
                std::string masterName = outPrefix + "master_flat" + (shortKey.empty() ? "" : "_" + shortKey);
                steps.push_back("Calibrate Flats -> " + masterName);
                steps.push_back("Stack " + masterName);
            }
        }
        // 4. Calibrate & Register Lights
        for (const auto& g : groups) {
            if (g.type == "Light") {
                std::string finalBatchName = outPrefix + "preprocessed_lights_" + g.filter;
                steps.push_back("Calibrate Lights -> " + finalBatchName);
                if (debayer) {
                    steps.push_back("Debayer Lights -> " + finalBatchName);
                }
                steps.push_back("Star Finding -> " + finalBatchName);
                steps.push_back("Register Lights -> " + finalBatchName);
            }
        }
    } else if (stage == "align_stack") {
        std::vector<std::string> batchNames = splitString(config.count("registered_light_batch_name") ? config.at("registered_light_batch_name") : "", ';');
        bool runBGE = config.count("run_bge") ? (config.at("run_bge") == "true") : true;
        bool alignMutually = config.count("align_mutually") ? (config.at("align_mutually") == "true") : true;

        std::vector<std::string> filters;
        for (const auto& name : batchNames) {
            std::string filter = name;
            size_t pos = filter.find("preprocessed_lights_");
            if (pos != std::string::npos) {
                filter = filter.substr(pos + 20);
            }
            filters.push_back(filter);
        }

        // 1. Finalize Registration
        steps.push_back("Finalize Registration");
        
        // 1.5 Background Normalization
        if (runBGE) {
            for (const auto& filter : filters) {
                steps.push_back("Background Normalization " + filter);
            }
        }

        // 3. Align, Stack, Autocrop, Postprocess, and Platesolve
        bool autocrop = config.count("autocrop") ? (config.at("autocrop") == "true") : false;
        bool correctPedestal = config.count("correct_pedestal") ? (config.at("correct_pedestal") == "true") : false;
        bool clampMax = config.count("clamp_max") ? (config.at("clamp_max") == "true") : false;
        bool platesolveStacks = config.count("platesolve_stacks") ? (config.at("platesolve_stacks") == "true") : false;

        for (const auto& filter : filters) {
            steps.push_back("Align " + filter + " Frames");
            steps.push_back("Stack " + filter + " Frames");
        }
        if (autocrop) {
            if (alignMutually) {
                steps.push_back("Autocrop Stacks");
            } else {
                for (const auto& filter : filters) {
                    steps.push_back("Autocrop " + filter);
                }
            }
        }
        if (correctPedestal || clampMax) {
            for (const auto& filter : filters) {
                steps.push_back("Postprocess " + filter);
            }
        }
        if (platesolveStacks) {
            for (const auto& filter : filters) {
                steps.push_back("Platesolve " + filter);
            }
        }
    }

    return steps;
}

void PreprocessingPipeline::relocateBatchFiles(WorkspaceRegistry& workspace,
                                               const std::string& batchName,
                                               const std::string& subDirName,
                                               const std::string& outDir,
                                               bool moveInsteadOfCopy) {
    if (!workspace.contains(batchName)) return;
    auto batch = std::get<ImageBatchPtr>(workspace.getElement(batchName));
    
    QDir baseDir(QString::fromStdString(outDir));
    QDir targetDir(baseDir.absoluteFilePath(QString::fromStdString(subDirName)));
    if (!targetDir.exists()) {
        targetDir.mkpath(".");
    }
    
    std::set<std::string> processedOldPaths;
    for (int i = 0; i < batch->count(); ++i) {
        std::string oldPath = batch->frameFilepath(i);
        if (oldPath.empty()) continue;
        
        QFileInfo oldInfo(QString::fromStdString(oldPath));
        if (oldInfo.absolutePath() == targetDir.absolutePath()) {
            processedOldPaths.insert(oldPath);
            continue;
        }

        QString newPath = targetDir.absoluteFilePath(oldInfo.fileName());
        std::string newPathStr = newPath.toStdString();
        
        if (processedOldPaths.count(oldPath)) {
            batch->setFrameFilepath(i, newPathStr);
            batch->setFrameLoader(i, [newPathStr]() { FitsIO reader; return reader.readImage(newPathStr); });
            continue;
        }
        processedOldPaths.insert(oldPath);
        
        if (moveInsteadOfCopy) {
            if (QFile::rename(QString::fromStdString(oldPath), newPath)) {
                batch->setFrameFilepath(i, newPathStr);
                batch->setFrameLoader(i, [newPathStr]() { FitsIO reader; return reader.readImage(newPathStr); });
            } else {
                if (QFile::exists(newPath)) {
                    QFile::remove(newPath);
                }
                if (QFile::rename(QString::fromStdString(oldPath), newPath)) {
                    batch->setFrameFilepath(i, newPathStr);
                    batch->setFrameLoader(i, [newPathStr]() { FitsIO reader; return reader.readImage(newPathStr); });
                } else if (QFile::copy(QString::fromStdString(oldPath), newPath)) {
                    QFile::remove(QString::fromStdString(oldPath));
                    batch->setFrameFilepath(i, newPathStr);
                    batch->setFrameLoader(i, [newPathStr]() { FitsIO reader; return reader.readImage(newPathStr); });
                }
            }
        } else {
            if (QFile::exists(newPath)) {
                QFile::remove(newPath);
            }
            if (QFile::copy(QString::fromStdString(oldPath), newPath)) {
                batch->setFrameFilepath(i, newPathStr);
                batch->setFrameLoader(i, [newPathStr]() { FitsIO reader; return reader.readImage(newPathStr); });
            }
        }
    }
}

} // namespace blastro

