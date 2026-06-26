#pragma once
#include "Algorithm.h"

namespace blastro {

class DebayerAlgorithm : public Algorithm {
public:
    DebayerAlgorithm() = default;
    ~DebayerAlgorithm() override = default;

    std::string name() const override { return "Debayer"; }
    void execute(WorkspaceRegistry& workspace, 
                 const std::map<std::string, std::string>& config, 
                 ProgressCallback progress = nullptr) override;
};

} // namespace blastro
