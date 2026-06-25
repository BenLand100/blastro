#pragma once
#include "core/WorkspaceRegistry.h"
#include <string>
#include <map>
#include <functional>

namespace blastro {

using ProgressCallback = std::function<void(int percent)>;

class Algorithm {
public:
    virtual ~Algorithm() = default;
    
    virtual std::string name() const = 0;
    
    // Executes the algorithm, mutating or creating elements in the workspace
    virtual void execute(WorkspaceRegistry& workspace, 
                         const std::map<std::string, std::string>& config, 
                         ProgressCallback progress = nullptr) = 0;
};

} // namespace blastro
