#pragma once
#include "Algorithm.h"

namespace blastro {

class BackgroundExtractionAlgorithm : public Algorithm {
public:
    BackgroundExtractionAlgorithm() = default;
    ~BackgroundExtractionAlgorithm() override = default;

    std::string name() const override { return "BackgroundExtraction"; }
    void execute(WorkspaceRegistry& workspace, 
                 const std::map<std::string, std::string>& config, 
                 ProgressCallback progress = nullptr) override;
};

} // namespace blastro
