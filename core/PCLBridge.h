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

#include <QObject>
#include <QString>
#include <pcl/api/APIDefs.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include "core/ImageBuffer.h"

class QWidget;

namespace blastro {

// Mock structure representing a PCL image
struct PCLImageMock {
    uint32 magic = 0x494d474d; // 'IMGM'
    uint32 width = 0;
    uint32 height = 0;
    uint32 numChannels = 0;
    uint32 bitsPerSample = 32; // 32-bit float
    bool isFloat = true;
    uint32 colorSpace = 0; // 0 = grayscale, 1 = RGB

    // Pointers that we pass to PCL (array of pointers to channel data)
    std::vector<void*> channelDataPointers;

    // If we wrap existing BLastro image buffers (direct access)
    std::vector<ImageBufferPtr> wrappedBuffers;

    // If PCL creates the image, we allocate and own the channels here
    std::vector<std::vector<float>> ownedChannels;

    void updatePointers() {
        channelDataPointers.clear();
        if (!wrappedBuffers.empty()) {
            for (auto& buf : wrappedBuffers) {
                channelDataPointers.push_back(buf->data());
            }
        } else {
            for (auto& chan : ownedChannels) {
                channelDataPointers.push_back(chan.data());
            }
        }
    }
};

struct PCLWindowMock;

// Mock structure representing a PCL view
struct PCLViewMock {
    uint32 magic = 0x56494557; // 'VIEW'
    QString id;
    PCLImageMock* hImage = nullptr;
    PCLWindowMock* parentWindow = nullptr;
};

// Mock structure representing a PCL window
struct PCLWindowMock {
    PCLViewMock* mainView = nullptr;
};

// Information about a process registered by the PCL module
struct PCLProcessInfo {
    QString id;
    meta_process_handle hMeta = nullptr;
    pcl::process_creation_routine createFn = nullptr;
    pcl::process_destruction_routine destroyFn = nullptr;
    pcl::process_initialization_routine initFn = nullptr;
    pcl::process_execution_routine executeFn = nullptr;
    pcl::process_global_execution_routine executeGlobalFn = nullptr;
    pcl::process_edit_preferences_routine editPreferencesFn = nullptr;
    pcl::process_class_initialization_routine classInitFn = nullptr;
    pcl::process_execution_preferences_routine executionPreferencesFn = nullptr;
    pcl::process_interface_validation_routine validateInterfaceFn = nullptr;
    bool hasEditPreferences = false;
};

struct PCLModuleInfo {
    void* libHandle = nullptr;
    api_module_description* description = nullptr;
    pcl::module_on_load_routine onLoadFn = nullptr;
    pcl::module_on_unload_routine onUnloadFn = nullptr;
    QString filepath;
};

class PCLBridge : public QObject {
    Q_OBJECT
public:
    PCLBridge(QObject* parent = nullptr);
    ~PCLBridge();

    // Dynamically load a PCL .so module
    bool loadModule(const QString& path);

    // Pre-load all shared libraries in a directory recursively
    static void preloadLibraries(const QString& libDir);
    
    // Unload the module and free resources
    void unloadModule();

    // Get the loaded module description metadata
    const api_module_description* moduleDescription() const { return m_description; }

    // Execute a registered process by ID on a set of BLastro image buffers
    bool executeProcess(const QString& processId, std::vector<ImageBufferPtr>& buffers);

    // Execute a registered process instance by ID on a set of BLastro image buffers
    bool executeProcessInstance(const QString& processId, void* hProcess, std::vector<ImageBufferPtr>& buffers);

    // Launch a process interface in a host-provided parent widget/dialog
    bool launchInterface(const QString& processId, QWidget* parentWindow);

    // Check if a process ID is registered
    bool isProcessRegistered(const QString& processId) const;

    // Get all registered process IDs in this module
    std::vector<QString> registeredProcesses() const;

    // Static function resolver callback passed to the PCL module
    static void* resolveCoreFunction(const char* funcName);

signals:
    void progressUpdated(int percent);

private:
    std::vector<PCLModuleInfo> m_modules;
    api_module_description* m_description = nullptr;
    bool m_initialized = false;

    // Helper to register our mock implementations over the generated stubs
    void setupOverrides();
};

} // namespace blastro

