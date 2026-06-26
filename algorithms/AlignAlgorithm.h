#pragma once
#include "Algorithm.h"

namespace blastro {

class AlignAlgorithm : public Algorithm {
public:
    AlignAlgorithm() = default;
    ~AlignAlgorithm() override = default;

    std::string name() const override { return "Align"; }
    void execute(WorkspaceRegistry& workspace, 
                 const std::map<std::string, std::string>& config, 
                 ProgressCallback progress = nullptr) override;
};

} // namespace blastro
