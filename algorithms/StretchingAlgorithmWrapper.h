#pragma once
#include "Algorithm.h"

namespace blastro {

class StretchingAlgorithmWrapper : public Algorithm {
public:
    StretchingAlgorithmWrapper() = default;
    ~StretchingAlgorithmWrapper() override = default;

    std::string name() const override { return "Stretching"; }
    void execute(WorkspaceRegistry& workspace, 
                 const std::map<std::string, std::string>& config, 
                 ProgressCallback progress = nullptr) override;
};

} // namespace blastro
