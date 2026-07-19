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

#pragma once
#include "GrayscaleImage.h"
#include "RGBImage.h"
#include "ImageBatch.h"
#include <unordered_map>
#include <string>
#include <variant>
#include <vector>
#include <mutex>
#include <functional>

namespace blastro {

using WorkspaceElement = std::variant<GrayscaleImagePtr, RGBImagePtr, ImageBatchPtr>;

class WorkspaceRegistry {
public:
    using ElementCallback = std::function<void(const std::string& name)>;
    using RenameCallback = std::function<void(const std::string& oldName, const std::string& newName)>;

    WorkspaceRegistry() = default;
    ~WorkspaceRegistry() = default;
    
    void setElementRegisteredCallback(ElementCallback cb) { std::lock_guard<std::mutex> lock(m_mutex); m_onRegister = cb; }
    void setElementUnregisteredCallback(ElementCallback cb) { std::lock_guard<std::mutex> lock(m_mutex); m_onUnregister = cb; }
    void setElementRenamedCallback(RenameCallback cb) { std::lock_guard<std::mutex> lock(m_mutex); m_onRename = cb; }

    bool registerElement(const std::string& name, WorkspaceElement element);
    bool unregisterElement(const std::string& name);
    bool renameElement(const std::string& oldName, const std::string& newName);
    
    bool contains(const std::string& name) const;
    WorkspaceElement getElement(const std::string& name) const;
    
    std::vector<std::string> elementNames() const;

private:
    std::unordered_map<std::string, WorkspaceElement> m_registry;
    mutable std::mutex m_mutex;

    ElementCallback m_onRegister;
    ElementCallback m_onUnregister;
    RenameCallback m_onRename;
};

} // namespace blastro
