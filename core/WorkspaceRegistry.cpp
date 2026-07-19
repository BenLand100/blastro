/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "WorkspaceRegistry.h"
#include "Logger.h"
#include <algorithm>

namespace blastro {

bool WorkspaceRegistry::registerElement(const std::string& name, WorkspaceElement element) {
    if (name.empty()) return false;
    ElementCallback cb;
    std::string typeStr = "Unknown Element";
    std::string details;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_registry[name] = element;
        cb = m_onRegister;

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
    }

    Logger::info("Workspace", QString("Registered %1: '%2' (%3)")
                 .arg(QString::fromStdString(typeStr))
                 .arg(QString::fromStdString(name))
                 .arg(QString::fromStdString(details)));

    if (cb) {
        cb(name);
    }
    return true;
}

bool WorkspaceRegistry::unregisterElement(const std::string& name) {
    ElementCallback cb;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_registry.find(name);
        if (it == m_registry.end()) return false;
        m_registry.erase(it);
        cb = m_onUnregister;
    }

    Logger::info("Workspace", QString("Unregistered '%1'").arg(QString::fromStdString(name)));

    if (cb) {
        cb(name);
    }
    return true;
}

bool WorkspaceRegistry::renameElement(const std::string& oldName, const std::string& newName) {
    if (newName.empty() || oldName == newName) return false;
    RenameCallback cb;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_registry.find(oldName);
        if (it == m_registry.end()) return false;
        if (m_registry.find(newName) != m_registry.end()) return false; // Target name exists
        
        auto element = it->second;
        m_registry.erase(it);
        m_registry[newName] = element;
        cb = m_onRename;
    }

    Logger::info("Workspace", QString("Renamed '%1' to '%2'")
                 .arg(QString::fromStdString(oldName))
                 .arg(QString::fromStdString(newName)));

    if (cb) {
        cb(oldName, newName);
    }
    return true;
}

bool WorkspaceRegistry::contains(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_registry.find(name) != m_registry.end();
}

WorkspaceElement WorkspaceRegistry::getElement(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_registry.find(name);
    if (it != m_registry.end()) {
        return it->second;
    }
    return WorkspaceElement(); // Returns valueless or empty variant
}

std::vector<std::string> WorkspaceRegistry::elementNames() const {
    std::lock_guard<std::mutex> lock(m_mutex);
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
