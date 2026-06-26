#pragma once
#include "Algorithm.h"

namespace blastro {

class RegisterAlgorithm : public Algorithm {
public:
    RegisterAlgorithm() = default;
    ~RegisterAlgorithm() override = default;

    std::string name() const override { return "Register"; }
    void execute(WorkspaceRegistry& workspace, 
                 const std::map<std::string, std::string>& config, 
                 ProgressCallback progress = nullptr) override;
};

} // namespace blastro
