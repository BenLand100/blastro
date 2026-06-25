#pragma once
#include "GrayscaleImage.h"
#include "RGBImage.h"
#include "ImageBatch.h"
#include <unordered_map>
#include <string>
#include <variant>
#include <vector>

namespace blastro {

using WorkspaceElement = std::variant<GrayscaleImagePtr, RGBImagePtr, ImageBatchPtr>;

class WorkspaceRegistry {
public:
    WorkspaceRegistry() = default;
    ~WorkspaceRegistry() = default;
    
    bool registerElement(const std::string& name, WorkspaceElement element);
    bool unregisterElement(const std::string& name);
    bool renameElement(const std::string& oldName, const std::string& newName);
    
    bool contains(const std::string& name) const;
    WorkspaceElement getElement(const std::string& name) const;
    
    std::vector<std::string> elementNames() const;

private:
    std::unordered_map<std::string, WorkspaceElement> m_registry;
};

} // namespace blastro
