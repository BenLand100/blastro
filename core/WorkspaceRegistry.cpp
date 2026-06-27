#include "WorkspaceRegistry.h"
#include "Logger.h"
#include <algorithm>

namespace blastro {

bool WorkspaceRegistry::registerElement(const std::string& name, WorkspaceElement element) {
    if (name.empty()) return false;
    if (m_registry.find(name) != m_registry.end()) return false; // Already exists
    
    m_registry[name] = element;

    std::string typeStr = "Unknown Element";
    std::string details;
    if (std::holds_alternative<GrayscaleImagePtr>(element)) {
        auto img = std::get<GrayscaleImagePtr>(element);
        typeStr = "Grayscale Image";
        details = img ? (std::to_string(img->width()) + "x" + std::to_string(img->height())) : "null";
    } else if (std::holds_alternative<RGBImagePtr>(element)) {
        auto img = std::get<RGBImagePtr>(element);
        typeStr = "RGB Image";
        details = img ? (std::to_string(img->width()) + "x" + std::to_string(img->height())) : "null";
    } else if (std::holds_alternative<ImageBatchPtr>(element)) {
        auto batch = std::get<ImageBatchPtr>(element);
        typeStr = "Image Batch";
        details = batch ? (std::to_string(batch->count()) + " frames") : "null";
    }

    Logger::info("Workspace", QString("Registered %1: '%2' (%3)")
                 .arg(QString::fromStdString(typeStr))
                 .arg(QString::fromStdString(name))
                 .arg(QString::fromStdString(details)));

    return true;
}

bool WorkspaceRegistry::unregisterElement(const std::string& name) {
    auto it = m_registry.find(name);
    if (it == m_registry.end()) return false;
    
    m_registry.erase(it);

    Logger::info("Workspace", QString("Unregistered '%1'").arg(QString::fromStdString(name)));

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

    Logger::info("Workspace", QString("Renamed '%1' to '%2'")
                 .arg(QString::fromStdString(oldName))
                 .arg(QString::fromStdString(newName)));

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
