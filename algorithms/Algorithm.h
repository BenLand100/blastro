#pragma once
#include "core/WorkspaceRegistry.h"
#include <string>
#include <map>

namespace blastro {

class Algorithm {
public:
    virtual ~Algorithm() = default;
    
    virtual std::string name() const = 0;
    
    // Executes the algorithm, mutating or creating elements in the workspace
    virtual void execute(WorkspaceRegistry& workspace, const std::map<std::string, std::string>& config) = 0;
};

} // namespace blastro
