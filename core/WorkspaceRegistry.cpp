#include "WorkspaceRegistry.h"
#include <algorithm>

namespace blastro {

bool WorkspaceRegistry::registerElement(const std::string& name, WorkspaceElement element) {
    if (name.empty()) return false;
    if (m_registry.find(name) != m_registry.end()) return false; // Already exists
    
    m_registry[name] = element;
    return true;
}

bool WorkspaceRegistry::unregisterElement(const std::string& name) {
    auto it = m_registry.find(name);
    if (it == m_registry.end()) return false;
    
    m_registry.erase(it);
    return true;
}

bool WorkspaceRegistry::renameElement(const std::string& oldName, const std::string& newName) {
    if (newName.empty() || oldName == newName) return false;
    
    auto it = m_registry.find(oldName);
    if (it == m_registry.end()) return false;
    
    if (m_registry.find(newName) != m_registry.end()) return false; // Target name exists
    
    auto element = it->second;
    m_registry.erase(it);
    m_registry[newName] = element;
    return true;
}

bool WorkspaceRegistry::contains(const std::string& name) const {
    return m_registry.find(name) != m_registry.end();
}

WorkspaceElement WorkspaceRegistry::getElement(const std::string& name) const {
    auto it = m_registry.find(name);
    if (it != m_registry.end()) {
        return it->second;
    }
    return WorkspaceElement(); // Returns valueless or empty variant
}

std::vector<std::string> WorkspaceRegistry::elementNames() const {
    std::vector<std::string> names;
    names.reserve(m_registry.size());
    for (const auto& pair : m_registry) {
        names.push_back(pair.first);
    }
    // Sort names for predictable display order
    std::sort(names.begin(), names.end());
    return names;
}

} // namespace blastro
