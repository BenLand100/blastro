#pragma once
#include "Algorithm.h"

namespace blastro {

class PixelMathAlgorithm : public Algorithm {
public:
    PixelMathAlgorithm() = default;
    ~PixelMathAlgorithm() override = default;

    std::string name() const override { return "PixelMath"; }
    
    void execute(WorkspaceRegistry& workspace, const std::map<std::string, std::string>& config) override;
};

} // namespace blastro
