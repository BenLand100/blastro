#pragma once
#include "Algorithm.h"

namespace blastro {

class GhsAlgorithmWrapper : public Algorithm {
public:
    GhsAlgorithmWrapper() = default;
    ~GhsAlgorithmWrapper() override = default;

    std::string name() const override { return "Ghs"; }
    void execute(WorkspaceRegistry& workspace, 
                 const std::map<std::string, std::string>& config, 
                 ProgressCallback progress = nullptr) override;
};

} // namespace blastro
