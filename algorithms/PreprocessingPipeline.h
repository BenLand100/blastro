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

#pragma once
#include "Algorithm.h"
#include "io/FitsIO.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace blastro {

struct PreprocessingFile {
    std::string filepath;
    double exposure = 0.0;
    std::string filter = "None";
    std::string imageType = "Light";
    int binningX = 1;
    int binningY = 1;
    int width = 0;
    int height = 0;
    double gain = 0.0;
};

struct PreprocessingGroup {
    std::string type; // "Bias", "Dark", "Flat", "Light"
    double exposure = 0.0;
    std::string filter = "None";
    int binningX = 1;
    int binningY = 1;
    int width = 0;
    int height = 0;
    double gain = 0.0;
    std::vector<std::string> filepaths;
};

class PreprocessingPipeline : public Algorithm {
public:
    PreprocessingPipeline() = default;
    ~PreprocessingPipeline() override = default;

    std::string name() const override { return "PreprocessingPipeline"; }
    
    using StepProgressCallback = std::function<void(int stepIndex, int percent, double elapsedSeconds, bool finished, bool success)>;
    void setStepCallback(StepProgressCallback cb) { m_stepCallback = cb; }
    void setCancelCallback(std::function<bool()> cb) { m_cancelCallback = cb; }
    static bool isCancelled();

    void execute(WorkspaceRegistry& workspace, 
                 const std::map<std::string, std::string>& config, 
                 ProgressCallback progress = nullptr) override;

    // Helper to group file paths based on their FITS headers
    static std::vector<PreprocessingGroup> groupFiles(
        const std::vector<std::string>& biasPaths,
        const std::vector<std::string>& darkPaths,
        const std::vector<std::string>& flatPaths,
        const std::vector<std::string>& lightPaths,
        double expTolerance = 0.5
    );

    static std::vector<std::string> getPlannedSteps(const std::map<std::string, std::string>& config);

private:
    StepProgressCallback m_stepCallback = nullptr;
    std::function<bool()> m_cancelCallback = nullptr;
    static std::function<bool()> s_cancelCallback;
};

} // namespace blastro

