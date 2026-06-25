#include "core/PCLBridge.h"
#include "core/PCLStubs.h"
#include <pcl/api/APIInterface.h>
#include <dlfcn.h>
#include <QDebug>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <algorithm>

namespace blastro {

// ----------------------------------------------------------------------------
// Global Registry State
// ----------------------------------------------------------------------------

static std::unordered_map<meta_process_handle, PCLProcessInfo> g_processes;
static std::unordered_map<std::string, meta_process_handle> g_processIdToHandle;
static meta_process_handle g_currentDefiningProcess = nullptr;

struct ParameterKey {
    meta_parameter_handle param;
    pcl::size_type rowIndex;

    bool operator==(const ParameterKey& o) const {
        return param == o.param && rowIndex == o.rowIndex;
    }
};

struct ParameterValue {
    std::vector<uint8_t> data;
    uint32 type = 0;
};

} // namespace blastro

namespace std {
template <>
struct hash<blastro::ParameterKey> {
    size_t operator()(const blastro::ParameterKey& k) const {
        return hash<const void*>()(k.param) ^ hash<pcl::size_type>()(k.rowIndex);
    }
};
} // namespace std

namespace blastro {

static std::unordered_map<process_handle, std::unordered_map<ParameterKey, ParameterValue>> g_processParameters;

// ----------------------------------------------------------------------------
// PixelTraits LUT Mock
// ----------------------------------------------------------------------------
static api_pixtraits_lut s_lut;

// Undefine conflicting PCL macros to prevent clashes in our custom implementation
#undef uint8_to_uint16
#undef uint8_to_uint20
#undef uint8_to_uint24
#undef uint8_to_uint32
#undef uint16_to_uint8
#undef uint16_to_uint20
#undef uint16_to_uint24
#undef uint16_to_uint32
#undef uint20_to_uint8
#undef uint20_to_uint16
#undef uint20_to_uint32

static void initPixelTraitsLUT() {
    static bool initialized = false;
    if (initialized) return;

    constexpr double uint8_max_d = 255.0;
    constexpr double uint16_max_d = 65535.0;
    constexpr double uint20_max_d = 1048575.0; // (1 << 20) - 1

    constexpr double uint8_to_uint16 = uint16_max_d / uint8_max_d;
    constexpr double uint8_to_uint20 = uint20_max_d / uint8_max_d;
    constexpr double uint8_to_uint24 = 16777215.0 / uint8_max_d;
    constexpr double uint8_to_uint32 = 4294967295.0 / uint8_max_d;

    constexpr double uint16_to_uint8 = uint8_max_d / uint16_max_d;
    constexpr double uint16_to_uint20 = uint20_max_d / uint16_max_d;
    constexpr double uint16_to_uint24 = 16777215.0 / uint16_max_d;
    constexpr double uint16_to_uint32 = 4294967295.0 / uint16_max_d;

    constexpr double uint20_to_uint8 = uint8_max_d / uint20_max_d;
    constexpr double uint20_to_uint16 = uint16_max_d / uint20_max_d;
    constexpr double uint20_to_uint32 = 4294967295.0 / uint20_max_d;

    // 8-bit conversions
    for (uint32_t i = 0; i <= 255; ++i) {
        s_lut.pFLUT8[i] = (float)(i / uint8_max_d);
        s_lut.pFLUTA[i] = (float)(i / 65025.0);
        s_lut.p1FLUT8[i] = (float)(1.0 - i / uint8_max_d);
        s_lut.pDLUT8[i] = i / uint8_max_d;
        s_lut.pDLUTA[i] = i / 65025.0;
        s_lut.p1DLUT8[i] = 1.0 - i / uint8_max_d;
        s_lut.p16LUT8[i] = (uint16_t)(i * uint8_to_uint16);
        s_lut.p20LUT8[i] = (uint32_t)(i * uint8_to_uint20 + 0.5);
        s_lut.p24LUT8[i] = (uint32_t)(i * uint8_to_uint24 + 0.5);
        s_lut.p32LUT8[i] = (uint32_t)(i * uint8_to_uint32 + 0.5);
    }

    // 16-bit conversions
    for (uint32_t i = 0; i <= 65535; ++i) {
        s_lut.pFLUT16[i] = (float)(i / uint16_max_d);
        s_lut.pDLUT16[i] = i / uint16_max_d;
        s_lut.p8LUT16[i] = (uint8_t)(i * uint16_to_uint8 + 0.5);
        s_lut.p20LUT16[i] = (uint32_t)(i * uint16_to_uint20 + 0.5);
        s_lut.p24LUT16[i] = (uint32_t)(i * uint16_to_uint24 + 0.5);
        s_lut.p32LUT16[i] = (uint32_t)(i * uint16_to_uint32 + 0.5);
    }

    // 20-bit conversions (up to 1048575)
    for (uint32_t i = 0; i <= 1048575; ++i) {
        s_lut.pFLUT20[i] = (float)(i / uint20_max_d);
        s_lut.pDLUT20[i] = i / uint20_max_d;
        s_lut.p8LUT20[i] = (uint8_t)(i * uint20_to_uint8 + 0.5);
        s_lut.p16LUT20[i] = (uint16_t)(i * uint20_to_uint16 + 0.5);
        s_lut.p32LUT20[i] = (uint32_t)(i * uint20_to_uint32 + 0.5);
    }

    initialized = true;
}

// ----------------------------------------------------------------------------
// Mock API Implementations
// ----------------------------------------------------------------------------

static const api_pixtraits_lut* mock_GetPixelTraitsLUT(uint32 version) {
    initPixelTraitsLUT();
    return &s_lut;
}

static void mock_GetPixInsightVersion(uint32* major, uint32* minor, uint32* release, uint32* revision,
                                      uint32* betaRelease, uint32* confidentialRelease,
                                      uint32* leVersion, char* langCode) {
    if (major) *major = 1;
    if (minor) *minor = 8;
    if (release) *release = 9;
    if (revision) *revision = 1;
    if (betaRelease) *betaRelease = 0;
    if (confidentialRelease) *confidentialRelease = 0;
    if (leVersion) *leVersion = 0;
    if (langCode) std::strcpy(langCode, "en");
}

static void* mock_Allocate(pcl::size_type size) {
    return std::malloc(size);
}

static api_bool mock_Deallocate(void* ptr) {
    std::free(ptr);
    return true;
}

static console_handle mock_GetConsole() {
    return reinterpret_cast<console_handle>(1);
}

static api_bool mock_ValidateConsole(const_console_handle) {
    return true;
}

static api_bool mock_WriteConsole(console_handle, const char16_type* text, api_bool appendNewline) {
    QString str = QString::fromUtf16(reinterpret_cast<const char16_t*>(text));
    if (appendNewline) {
        qDebug().noquote() << "[PCL Console]" << str;
    } else {
        std::cout << str.toStdString();
        std::flush(std::cout);
    }
    return true;
}

static api_bool mock_FlushConsole(console_handle) {
    return true;
}

static api_bool mock_ShowConsole(console_handle, api_bool) {
    return true;
}

static void mock_ProcessEvents(api_bool) {}

static api_bool mock_GetParameterValue(const_process_handle hProcess, meta_parameter_handle hParam,
                                       pcl::size_type rowIndex, uint32* parType, void* value, pcl::size_type* length) {
    auto procIt = g_processParameters.find(const_cast<process_handle>(hProcess));
    if (procIt == g_processParameters.end()) return false;

    ParameterKey key{hParam, rowIndex};
    auto paramIt = procIt->second.find(key);
    if (paramIt == procIt->second.end()) return false;

    const auto& val = paramIt->second;
    if (parType) *parType = val.type;

    if (value == nullptr) {
        if (length) *length = val.data.size();
        return true;
    }

    if (length) {
        pcl::size_type copyLen = std::min(*length, (pcl::size_type)val.data.size());
        std::memcpy(value, val.data.data(), copyLen);
        *length = copyLen;
    }
    return true;
}

static api_bool mock_SetParameterValue(process_handle hProcess, meta_parameter_handle hParam,
                                       pcl::size_type rowIndex, const void* value, pcl::size_type length) {
    ParameterKey key{hParam, rowIndex};
    ParameterValue val;
    const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(value);
    val.data.assign(bytePtr, bytePtr + length);
    val.type = 0;
    g_processParameters[hProcess][key] = val;
    return true;
}

static void mock_EnterModuleDefinitionContext() {}
static api_bool mock_IsModuleDefinitionContextActive() { return true; }
static void mock_ExitModuleDefinitionContext() {}

static void mock_EnterProcessDefinitionContext() {}
static api_bool mock_IsProcessDefinitionContextActive() { return true; }
static void mock_ExitProcessDefinitionContext() {}

static void mock_BeginProcessDefinition(meta_process_handle hMeta, const char* procId) {
    g_currentDefiningProcess = hMeta;
    PCLProcessInfo info;
    info.id = QString::fromUtf8(procId);
    info.hMeta = hMeta;
    g_processes[hMeta] = info;
    g_processIdToHandle[procId] = hMeta;
    qDebug() << "[PCL Bridge] Defining process:" << procId;
}

static void mock_SetProcessCategory(const char* cat) {
    qDebug() << "[PCL Bridge] Process category:" << cat;
}

static void mock_SetProcessCreationRoutine(pcl::process_creation_routine f) {
    if (g_currentDefiningProcess) g_processes[g_currentDefiningProcess].createFn = f;
}

static void mock_SetProcessDestructionRoutine(pcl::process_destruction_routine f) {
    if (g_currentDefiningProcess) g_processes[g_currentDefiningProcess].destroyFn = f;
}

static void mock_SetProcessInitializationRoutine(pcl::process_initialization_routine f) {
    if (g_currentDefiningProcess) g_processes[g_currentDefiningProcess].initFn = f;
}

static void mock_SetProcessExecutionRoutine(pcl::process_execution_routine f) {
    if (g_currentDefiningProcess) g_processes[g_currentDefiningProcess].executeFn = f;
}

static void mock_SetProcessGlobalExecutionRoutine(pcl::process_global_execution_routine f) {
    if (g_currentDefiningProcess) g_processes[g_currentDefiningProcess].executeGlobalFn = f;
}

static void mock_EndProcessDefinition() {
    g_currentDefiningProcess = nullptr;
}

static api_bool mock_GetImageGeometry(const_image_handle h, uint32* w, uint32* h_out, uint32* n) {
    if (!h) return false;
    auto* img = reinterpret_cast<const PCLImageMock*>(h);
    if (w) *w = img->width;
    if (h_out) *h_out = img->height;
    if (n) *n = img->numChannels;
    return true;
}

static api_bool mock_GetImageFormat(const_image_handle h, uint32* nbits, api_bool* flt) {
    if (!h) return false;
    auto* img = reinterpret_cast<const PCLImageMock*>(h);
    if (nbits) *nbits = img->bitsPerSample;
    if (flt) *flt = img->isFloat;
    return true;
}

static api_bool mock_GetImagePixelData(image_handle h, void*** data) {
    if (!h || !data) return false;
    auto* img = reinterpret_cast<PCLImageMock*>(h);
    img->updatePointers();
    *data = img->channelDataPointers.data();
    return true;
}

static api_bool mock_SetImagePixelData(image_handle, void**) {
    return true;
}

static image_handle mock_GetViewImage(view_handle hView) {
    if (!hView) return nullptr;
    auto* view = reinterpret_cast<PCLViewMock*>(hView);
    return view->hImage;
}

static api_bool mock_GetImageColorSpace(const_image_handle h, uint32* cs) {
    if (!h) return false;
    auto* img = reinterpret_cast<const PCLImageMock*>(h);
    if (cs) *cs = img->colorSpace;
    return true;
}

// ----------------------------------------------------------------------------
// PCLBridge Implementation
// ----------------------------------------------------------------------------

PCLBridge::PCLBridge() {}

PCLBridge::~PCLBridge() {
    unloadModule();
}

void PCLBridge::setupOverrides() {
    overridePCLStub("Global/GetPixelTraitsLUT", reinterpret_cast<void*>(mock_GetPixelTraitsLUT));
    overridePCLStub("Global/GetPixInsightVersion", reinterpret_cast<void*>(mock_GetPixInsightVersion));
    overridePCLStub("Global/Allocate", reinterpret_cast<void*>(mock_Allocate));
    overridePCLStub("Global/Deallocate", reinterpret_cast<void*>(mock_Deallocate));
    overridePCLStub("Global/GetConsole", reinterpret_cast<void*>(mock_GetConsole));
    overridePCLStub("Global/ValidateConsole", reinterpret_cast<void*>(mock_ValidateConsole));
    overridePCLStub("Global/WriteConsole", reinterpret_cast<void*>(mock_WriteConsole));
    overridePCLStub("Global/FlushConsole", reinterpret_cast<void*>(mock_FlushConsole));
    overridePCLStub("Global/ShowConsole", reinterpret_cast<void*>(mock_ShowConsole));
    overridePCLStub("Global/ProcessEvents", reinterpret_cast<void*>(mock_ProcessEvents));

    overridePCLStub("Process/GetParameterValue", reinterpret_cast<void*>(mock_GetParameterValue));
    overridePCLStub("Process/SetParameterValue", reinterpret_cast<void*>(mock_SetParameterValue));

    overridePCLStub("ModuleDefinition/EnterModuleDefinitionContext", reinterpret_cast<void*>(mock_EnterModuleDefinitionContext));
    overridePCLStub("ModuleDefinition/IsModuleDefinitionContextActive", reinterpret_cast<void*>(mock_IsModuleDefinitionContextActive));
    overridePCLStub("ModuleDefinition/ExitModuleDefinitionContext", reinterpret_cast<void*>(mock_ExitModuleDefinitionContext));

    overridePCLStub("ProcessDefinition/EnterProcessDefinitionContext", reinterpret_cast<void*>(mock_EnterProcessDefinitionContext));
    overridePCLStub("ProcessDefinition/IsProcessDefinitionContextActive", reinterpret_cast<void*>(mock_IsProcessDefinitionContextActive));
    overridePCLStub("ProcessDefinition/BeginProcessDefinition", reinterpret_cast<void*>(mock_BeginProcessDefinition));
    overridePCLStub("ProcessDefinition/SetProcessCategory", reinterpret_cast<void*>(mock_SetProcessCategory));
    overridePCLStub("ProcessDefinition/SetProcessCreationRoutine", reinterpret_cast<void*>(mock_SetProcessCreationRoutine));
    overridePCLStub("ProcessDefinition/SetProcessDestructionRoutine", reinterpret_cast<void*>(mock_SetProcessDestructionRoutine));
    overridePCLStub("ProcessDefinition/SetProcessInitializationRoutine", reinterpret_cast<void*>(mock_SetProcessInitializationRoutine));
    overridePCLStub("ProcessDefinition/SetProcessExecutionRoutine", reinterpret_cast<void*>(mock_SetProcessExecutionRoutine));
    overridePCLStub("ProcessDefinition/SetProcessGlobalExecutionRoutine", reinterpret_cast<void*>(mock_SetProcessGlobalExecutionRoutine));
    overridePCLStub("ProcessDefinition/EndProcessDefinition", reinterpret_cast<void*>(mock_EndProcessDefinition));
    overridePCLStub("ProcessDefinition/ExitProcessDefinitionContext", reinterpret_cast<void*>(mock_ExitProcessDefinitionContext));

    overridePCLStub("SharedImage/GetImageGeometry", reinterpret_cast<void*>(mock_GetImageGeometry));
    overridePCLStub("SharedImage/GetImageFormat", reinterpret_cast<void*>(mock_GetImageFormat));
    overridePCLStub("SharedImage/GetImagePixelData", reinterpret_cast<void*>(mock_GetImagePixelData));
    overridePCLStub("SharedImage/SetImagePixelData", reinterpret_cast<void*>(mock_SetImagePixelData));
    overridePCLStub("SharedImage/GetImageColorSpace", reinterpret_cast<void*>(mock_GetImageColorSpace));

    overridePCLStub("View/GetViewImage", reinterpret_cast<void*>(mock_GetViewImage));
}

bool PCLBridge::loadModule(const QString& path) {
    unloadModule();

    qDebug() << "[PCL Bridge] Attempting to dlopen:" << path;
    m_libHandle = dlopen(path.toUtf8().constData(), RTLD_LAZY | RTLD_GLOBAL);
    if (!m_libHandle) {
        qWarning() << "[PCL Bridge] dlopen failed:" << dlerror();
        return false;
    }

    // Resolve entry points
    typedef uint32 (*IdentifyRoutine)(api_module_description**, int32);
    typedef uint32 (*InitializeRoutine)(api_handle, function_resolver, uint32, void*);
    typedef int32 (*InstallRoutine)(int32);

    IdentifyRoutine identify = reinterpret_cast<IdentifyRoutine>(dlsym(m_libHandle, "IdentifyPixInsightModule"));
    InitializeRoutine initialize = reinterpret_cast<InitializeRoutine>(dlsym(m_libHandle, "InitializePixInsightModule"));
    InstallRoutine install = reinterpret_cast<InstallRoutine>(dlsym(m_libHandle, "InstallPixInsightModule"));

    if (!identify || !initialize) {
        qWarning() << "[PCL Bridge] Missing mandatory PMIDN or PMINI symbols in module.";
        unloadModule();
        return false;
    }

    // 1. Install with FullInstall first to instantiate the MetaModule and all process/parameter meta-objects!
    if (install) {
        qDebug() << "[PCL Bridge] Running InstallPixInsightModule (FullInstall)...";
        int32 installStatus = install(0); // InstallMode::FullInstall
        if (installStatus != 0) {
            qWarning() << "[PCL Bridge] InstallPixInsightModule (FullInstall) failed, code:" << installStatus;
            unloadModule();
            return false;
        }
        qDebug() << "[PCL Bridge] Meta-objects instantiated successfully.";
    } else {
        qWarning() << "[PCL Bridge] Module has no installation entry point (PMINS), cannot initialize.";
        unloadModule();
        return false;
    }

    // 2. Initialize the module now that the meta-objects are instantiated!
    initPCLStubs();
    setupOverrides();

    api_handle hModule = reinterpret_cast<api_handle>(this);
    uint32 status = initialize(hModule, &PCLBridge::resolveCoreFunction, PCL_API_Version, nullptr);
    if (status != 0) {
        qWarning() << "[PCL Bridge] InitializePixInsightModule failed, code:" << status;
        unloadModule();
        return false;
    }

    m_initialized = true;

    // 3. Identify the module to get description metadata!
    qDebug() << "[PCL Bridge] Running IdentifyPixInsightModule...";
    status = identify(&m_description, 0);
    if (status != 0) {
        qWarning() << "[PCL Bridge] IdentifyPixInsightModule phase 0 failed, code:" << status;
        unloadModule();
        return false;
    }

    status = identify(&m_description, 1);
    if (status != 0) {
        qWarning() << "[PCL Bridge] IdentifyPixInsightModule phase 1 failed, code:" << status;
        unloadModule();
        return false;
    }

    if (!m_description) {
        qWarning() << "[PCL Bridge] Module description is null after identification.";
        unloadModule();
        return false;
    }

    // Print metadata
    qDebug() << "[PCL Bridge] Module Identified Successfully:";
    qDebug() << "  Name:       " << m_description->name;
    qDebug() << "  Version:    " << m_description->versionInfo;
    qDebug() << "  API Version:" << QString::number(m_description->apiVersion, 16);

    return true;
}

void PCLBridge::unloadModule() {
    if (m_libHandle) {
        // If identify is loaded, call PMIDN phase 0xff for cleanup
        typedef uint32 (*IdentifyRoutine)(api_module_description**, int32);
        IdentifyRoutine identify = reinterpret_cast<IdentifyRoutine>(dlsym(m_libHandle, "IdentifyPixInsightModule"));
        if (identify && m_description) {
            identify(&m_description, 0xff);
        }
        
        dlclose(m_libHandle);
        m_libHandle = nullptr;
        m_description = nullptr;
        m_initialized = false;
        qDebug() << "[PCL Bridge] Module unloaded.";
    }
}

bool PCLBridge::executeProcess(const QString& processId, std::vector<ImageBufferPtr>& buffers) {
    if (!m_initialized) {
        qWarning() << "[PCL Bridge] Cannot execute process, no module loaded/initialized.";
        return false;
    }

    if (buffers.empty()) {
        qWarning() << "[PCL Bridge] Cannot execute process, no image buffers provided.";
        return false;
    }

    auto idIt = g_processIdToHandle.find(processId.toStdString());
    if (idIt == g_processIdToHandle.end()) {
        qWarning() << "[PCL Bridge] Process ID not found:" << processId;
        return false;
    }

    meta_process_handle hMeta = idIt->second;
    const auto& info = g_processes[hMeta];

    if (!info.createFn) {
        qWarning() << "[PCL Bridge] Process has no creation routine:" << processId;
        return false;
    }

    qDebug() << "[PCL Bridge] Creating process instance for:" << processId;
    process_handle hProcess = info.createFn(hMeta);
    if (!hProcess) {
        qWarning() << "[PCL Bridge] Failed to instantiate process:" << processId;
        return false;
    }

    if (info.initFn) {
        qDebug() << "[PCL Bridge] Initializing process instance...";
        info.initFn(hProcess);
    }

    // Build the mock image and view structures
    PCLImageMock imgMock;
    imgMock.width = buffers[0]->width();
    imgMock.height = buffers[0]->height();
    imgMock.numChannels = buffers.size();
    imgMock.bitsPerSample = 32;
    imgMock.isFloat = true;
    imgMock.colorSpace = (buffers.size() == 1) ? 0 : 1;
    imgMock.wrappedBuffers = buffers;
    imgMock.updatePointers();

    PCLViewMock viewMock;
    viewMock.id = "BlastroActiveView";
    viewMock.hImage = &imgMock;

    qDebug() << "[PCL Bridge] Executing process on image:" << imgMock.width << "x" << imgMock.height << "with" << imgMock.numChannels << "channels";
    bool success = false;
    if (info.executeFn) {
        success = info.executeFn(&viewMock, hProcess);
    } else if (info.executeGlobalFn) {
        qWarning() << "[PCL Bridge] Process only supports global execution, executing globally...";
        success = info.executeGlobalFn(hProcess);
    } else {
        qWarning() << "[PCL Bridge] Process has no execution routine!";
    }

    qDebug() << "[PCL Bridge] Process execution finished, status:" << (success ? "Success" : "Failed");

    // Clean up
    if (info.destroyFn) {
        info.destroyFn(hProcess);
    }
    g_processParameters.erase(hProcess);

    return success;
}

bool PCLBridge::isProcessRegistered(const QString& processId) const {
    return g_processIdToHandle.find(processId.toStdString()) != g_processIdToHandle.end();
}

std::vector<QString> PCLBridge::registeredProcesses() const {
    std::vector<QString> list;
    for (const auto& pair : g_processIdToHandle) {
        list.push_back(QString::fromStdString(pair.first));
    }
    return list;
}

void* PCLBridge::resolveCoreFunction(const char* funcName) {
    return getPCLStub(funcName);
}

} // namespace blastro
