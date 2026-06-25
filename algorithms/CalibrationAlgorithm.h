#pragma once
#include "Algorithm.h"

namespace blastro {

class CalibrationAlgorithm : public Algorithm {
public:
    CalibrationAlgorithm() = default;
    ~CalibrationAlgorithm() override = default;

    std::string name() const override { return "Calibration"; }
    void execute(WorkspaceRegistry& workspace, 
                 const std::map<std::string, std::string>& config, 
                 ProgressCallback progress = nullptr) override;
};

} // namespace blastro
