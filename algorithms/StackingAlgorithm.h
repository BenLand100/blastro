#pragma once
#include "Algorithm.h"

namespace blastro {

class StackingAlgorithm : public Algorithm {
public:
    StackingAlgorithm() = default;
    ~StackingAlgorithm() override = default;

    std::string name() const override { return "Stacking"; }
    void execute(WorkspaceRegistry& workspace, 
                 const std::map<std::string, std::string>& config, 
                 ProgressCallback progress = nullptr) override;
};

} // namespace blastro
