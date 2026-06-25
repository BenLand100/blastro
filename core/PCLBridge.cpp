#include "core/PCLBridge.h"
#include "ui/MainWindow.h"
#include "core/PCLStubs.h"
#include <pcl/api/APIInterface.h>
#include <dlfcn.h>
#include <QFile>
#include <QSettings>
#include <QFont>
#include <QFontMetrics>
#include <QApplication>
#include <QDebug>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <algorithm>
#include <thread>
#include <chrono>
#include <QWidget>
#include <QDialog>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QSlider>
#include <QComboBox>
#include <QSpinBox>
#include <QVariant>
#include <QLineEdit>

namespace blastro {

// ----------------------------------------------------------------------------
// Global Registry State
// ----------------------------------------------------------------------------

static std::unordered_map<meta_process_handle, PCLProcessInfo> g_processes;
static std::unordered_map<std::string, meta_process_handle> g_processIdToHandle;
static meta_process_handle g_currentDefiningProcess = nullptr;

// Module lifecycle callbacks
static pcl::module_on_load_routine g_moduleOnLoad = nullptr;
static pcl::module_on_unload_routine g_moduleOnUnload = nullptr;

// Interface registry
struct PCLInterfaceInfo {
    QString id;
    meta_interface_handle hMeta = nullptr;
    pcl::interface_initialization_routine initFn = nullptr;
    pcl::interface_launch_routine launchFn = nullptr;
};

static std::unordered_map<meta_interface_handle, PCLInterfaceInfo> g_interfaces;
static std::unordered_map<std::string, meta_interface_handle> g_interfaceIdToHandle;
static meta_interface_handle g_currentDefiningInterface = nullptr;

// Map from interface/process ID to the top-level QWidget handle created for it
static std::unordered_map<std::string, control_handle> g_interfaceControls;

// Map from Qt widget pointer (control_handle) to the PCL client object pointer (api_handle).
// The PCL Create* functions pass `client = this` (the PCL UIObject pointer).
// PCL event dispatchers cast hSender back to Button*/Slider*/etc. and access m_handlers,
// so we MUST pass the PCL client pointer as hSender, not the Qt widget pointer.
static std::unordered_map<control_handle, api_handle> g_widgetToClient;

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
static PCLImageMock* g_activeImage = nullptr;

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

// ============================================================================
// Forward Declarations for Settings, Globals, and Parameter Mocks
// ============================================================================
static api_bool mock_ReadSettingsString(const char16_type* keyPath, char16_type* value, pcl::size_type maxLen);
static api_bool mock_WriteSettingsString(const char16_type* keyPath, const char16_type* value);
static api_bool mock_ReadSettingsFlag(const char16_type* keyPath, api_bool* value);
static api_bool mock_WriteSettingsFlag(const char16_type* keyPath, api_bool value);
static api_bool mock_ReadSettingsReal(const char16_type* keyPath, double* value);
static api_bool mock_WriteSettingsReal(const char16_type* keyPath, double value);
static api_bool mock_ReadSettingsInteger(const char16_type* keyPath, int32* value);
static api_bool mock_WriteSettingsInteger(const char16_type* keyPath, int32 value);
static api_bool mock_GetGlobalString(const char16_type* globalKey, char16_type* value, pcl::size_type maxLen);
static int32 mock_GetGlobalInteger(const char16_type* globalKey);

static void mock_BeginParameterDefinition(meta_parameter_handle hParam, const char* paramId, uint32 type);
static void mock_SetParameterProcessVersionRange(uint32 minVer, uint32 maxVer);
static void mock_SetParameterRequired(api_bool required);
static void mock_SetParameterReadOnly(api_bool readOnly);
static void mock_SetParameterLockRoutine(void* routine);
static void mock_SetParameterAllocationRoutine(void* routine);
static void mock_SetParameterLengthQueryRoutine(void* routine);
static void mock_SetDefaultBooleanValue(api_bool value);
static void mock_SetDefaultNumericValue(double value);
static void mock_SetValidNumericRange(double minVal, double maxVal);
static void mock_SetPrecision(int32 precision);
static void mock_SetScientificNotation(api_bool scientific);
static void mock_EndParameterDefinition();

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

static pcl::process_default_interface_selection_routine g_defaultInterfaceSelection = nullptr;
static pcl::process_interface_selection_routine g_interfaceSelection = nullptr;

static void mock_SetProcessDefaultInterfaceSelectionRoutine(pcl::process_default_interface_selection_routine f) {
    g_defaultInterfaceSelection = f;
    qDebug() << "[PCL Bridge] Default interface selection routine registered.";
}

static void mock_SetProcessInterfaceSelectionRoutine(pcl::process_interface_selection_routine f) {
    g_interfaceSelection = f;
    qDebug() << "[PCL Bridge] Interface selection routine registered.";
}

static void mock_EndProcessDefinition() {
    g_currentDefiningProcess = nullptr;
}

static const PCLImageMock* resolveImageMock(const_image_handle h) {
    if (!h) return nullptr;
    return reinterpret_cast<const PCLImageMock*>(h);
}

static PCLImageMock* resolveImageMockMutable(image_handle h) {
    if (!h) return nullptr;
    return reinterpret_cast<PCLImageMock*>(h);
}

static api_bool mock_GetImageGeometry(const_image_handle h, uint32* w, uint32* h_out, uint32* n) {
    const auto* img = resolveImageMock(h);
    if (!img) return false;
    if (w) *w = img->width;
    if (h_out) *h_out = img->height;
    if (n) *n = img->numChannels;
    return true;
}

static api_bool mock_GetImageFormat(const_image_handle h, uint32* nbits, api_bool* flt) {
    const auto* img = resolveImageMock(h);
    if (!img) return false;
    if (nbits) *nbits = img->bitsPerSample;
    if (flt) *flt = img->isFloat;
    return true;
}

static api_bool mock_GetImagePixelData(image_handle h, void*** data) {
    auto* img = resolveImageMockMutable(h);
    if (!img || !data) return false;
    img->updatePointers();
    *data = img->channelDataPointers.data();
    return true;
}

static api_bool mock_SetImagePixelData(image_handle, void**) {
    return true;
}

static api_bool mock_SetImageGeometry(image_handle h, uint32 w, uint32 h_out, uint32 n) {
    qDebug() << "[PCL Bridge] mock_SetImageGeometry called: image =" << h << "w =" << w << "h =" << h_out << "n =" << n;
    auto* img = resolveImageMockMutable(h);
    if (img) {
        img->width = w;
        img->height = h_out;
        img->numChannels = n;
        if (img->wrappedBuffers.empty()) {
            img->ownedChannels.resize(n);
            for (uint32 i = 0; i < n; ++i) {
                img->ownedChannels[i].resize(w * h_out, 0.0f);
            }
        }
        img->updatePointers();
        return true;
    }
    return false;
}

static api_bool mock_SetImageColorSpace(image_handle h, uint32 cs) {
    qDebug() << "[PCL Bridge] mock_SetImageColorSpace called: image =" << h << "colorSpace =" << cs;
    auto* img = resolveImageMockMutable(h);
    if (img) {
        img->colorSpace = cs;
        return true;
    }
    return false;
}

static image_handle mock_GetViewImage(view_handle hView) {
    if (!hView) return nullptr;
    auto* view = reinterpret_cast<PCLViewMock*>(hView);
    return view->hImage;
}

static api_bool mock_GetViewScreenTransferFunctions(const_view_handle hView, double* m, double* c0, double* c1, double* r0, double* r1) {
    qDebug() << "[PCL Bridge] mock_GetViewScreenTransferFunctions called: view =" << hView;

    // Compute per-channel STF parameters from the actual image data.
    // PixInsight autostretch: c0 = max(0, median - 2.8*MAD), midtone m places background at targetBkg.
    // We compute statistics for channels 0..N-1 and set channel 3 as the average (luminance joint channel).

    const double targetBkg = 0.25; // PixInsight default target background level
    const double kMAD = 2.8;       // Shadow clipping factor

    int nChan = g_activeImage ? (int)g_activeImage->numChannels : 3;

    // Autostretch: midtone balance is solved analytically below — no MTF lambda needed.

    // Initialize all 4 channels to neutral defaults
    for (int i = 0; i < 4; ++i) {
        if (m)  m[i]  = 0.5;
        if (c0) c0[i] = 0.0;
        if (c1) c1[i] = 1.0;
        if (r0) r0[i] = 0.0;
        if (r1) r1[i] = 1.0;
    }

    if (!g_activeImage) return true;

    double sumM[3]  = {0,0,0};
    double sumC0[3] = {0,0,0};
    int validChans = 0;

    for (int c = 0; c < nChan && c < 3; ++c) {
        const float* data = nullptr;
        int N = g_activeImage->width * g_activeImage->height;
        if (!g_activeImage->wrappedBuffers.empty() && c < (int)g_activeImage->wrappedBuffers.size()) {
            data = g_activeImage->wrappedBuffers[c]->data();
        } else if (c < (int)g_activeImage->ownedChannels.size()) {
            data = g_activeImage->ownedChannels[c].data();
        }
        if (!data || N == 0) continue;

        // Compute mean and a robust MAD approximation using sampled pixels
        int sampleStep = std::max(1, N / 10000);
        double sum = 0;
        int count = 0;
        for (int p = 0; p < N; p += sampleStep) { sum += data[p]; ++count; }
        double mean = count > 0 ? sum / count : 0.0;

        // Approximate MAD as mean absolute deviation from mean (fast, no sort needed)
        double madSum = 0;
        for (int p = 0; p < N; p += sampleStep) { madSum += std::abs(data[p] - mean); }
        double mad = count > 0 ? madSum / count : 0.01;

        // Compute shadow clipping
        double shadows = std::max(0.0, mean - kMAD * mad);
        double clippedMean = (mean - shadows) / (1.0 - shadows);

        // Midtone balance: find m such that MTF(clippedMean, m) = targetBkg
        // Solving: targetBkg*(2*m-1)*x - targetBkg*m = (m-1)*x
        // => m = (targetBkg*x - x) / (targetBkg*2*x - targetBkg - x) ... algebraically:
        // m = (x - targetBkg*x) / (x - 2*targetBkg*x + targetBkg)
        //   = x*(1-targetBkg) / (x*(1-2*targetBkg) + targetBkg)
        double midtone = 0.5;
        if (clippedMean > 0.0 && clippedMean < 1.0) {
            double x = clippedMean;
            double d = x * (1.0 - 2.0*targetBkg) + targetBkg;
            if (std::abs(d) > 1e-10)
                midtone = std::max(0.001, std::min(0.999, x*(1.0 - targetBkg) / d));
        }

        if (m)  m[c]  = midtone;
        if (c0) c0[c] = shadows;
        if (c1) c1[c] = 1.0;
        if (r0) r0[c] = 0.0;
        if (r1) r1[c] = 1.0;

        sumM[c]  = midtone;
        sumC0[c] = shadows;
        ++validChans;

        qDebug() << "[PCL Bridge] STF ch" << c << ": mean=" << mean << "mad=" << mad
                 << "shadows=" << shadows << "midtone=" << midtone;
    }

    // Channel 3: joint luminance — average of R/G/B
    if (validChans > 0) {
        double avgM  = 0, avgC0 = 0;
        for (int c = 0; c < validChans; ++c) { avgM += sumM[c]; avgC0 += sumC0[c]; }
        avgM  /= validChans;
        avgC0 /= validChans;
        if (m)  m[3]  = avgM;
        if (c0) c0[3] = avgC0;
        if (c1) c1[3] = 1.0;
        if (r0) r0[3] = 0.0;
        if (r1) r1[3] = 1.0;
    }

    return true;
}

static api_bool mock_GetViewScreenTransferFunctionsEnabled(view_handle hView) {
    qDebug() << "[PCL Bridge] mock_GetViewScreenTransferFunctionsEnabled called: view =" << hView;
    return true; // STF always enabled — DeepSNR needs it to calibrate noise model
}

static api_bool mock_GetImageColorSpace(const_image_handle h, uint32* cs) {
    if (!h) return false;
    auto* img = reinterpret_cast<const PCLImageMock*>(h);
    if (cs) *cs = img->colorSpace;
    return true;
}

static api_bool mock_LockView(const_view_handle hView) {
    qDebug() << "[PCL Bridge] mock_LockView called: view =" << hView;
    return true;
}

static api_bool mock_UnlockView(const_view_handle hView) {
    qDebug() << "[PCL Bridge] mock_UnlockView called: view =" << hView;
    return true;
}

static api_bool mock_AttachToImage(image_handle hImage, const_view_handle hView, api_bool readOnly) {
    qDebug() << "[PCL Bridge] mock_AttachToImage called: image =" << hImage << "view =" << hView << "readOnly =" << readOnly;
    if (!hImage) {
        qWarning() << "[PCL Bridge] mock_AttachToImage: null image handle passed!";
        return false;
    }
    
    if (g_activeImage) {
        qDebug() << "[PCL Bridge] mock_AttachToImage: successfully associated SharedImage" << hImage << "with active image mock" << g_activeImage;
    } else {
        qWarning() << "[PCL Bridge] mock_AttachToImage: no active image mock registered!";
        return false;
    }
    
    return true;
}

static api_bool mock_GetImageRGBWS(const_image_handle hImage, api_RGBWS* rgbws) {
    qDebug() << "[PCL Bridge] mock_GetImageRGBWS called: image =" << hImage << "rgbws =" << rgbws;
    if (!rgbws) return false;
    
    rgbws->gamma = 2.2f;
    rgbws->isSRGBGamma = true;
    
    // Default sRGB chromaticity coordinates (Rx, Gx, Bx) and (Ry, Gy, By)
    rgbws->x[0] = 0.64f;  rgbws->y[0] = 0.33f;
    rgbws->x[1] = 0.30f;  rgbws->y[1] = 0.60f;
    rgbws->x[2] = 0.15f;  rgbws->y[2] = 0.06f;
    
    // Default sRGB luminance coefficients
    rgbws->Y[0] = 0.212639f;
    rgbws->Y[1] = 0.715168f;
    rgbws->Y[2] = 0.072193f;
    
    return true;
}

static api_bool mock_DetachFromImage(image_handle hImage) {
    qDebug() << "[PCL Bridge] mock_DetachFromImage called: image =" << hImage;
    return true;
}

static api_bool mock_Abort() {
    qDebug() << "[PCL Bridge] mock_Abort called";
    return true;
}

static api_bool mock_DisableAbort() {
    qDebug() << "[PCL Bridge] mock_DisableAbort called";
    return true;
}

static api_bool mock_EnableAbort() {
    qDebug() << "[PCL Bridge] mock_EnableAbort called";
    return true;
}

static void fillPropertyValue(api_property_value* value, const std::string& id) {
    if (!value) return;
    std::memset(value, 0, sizeof(api_property_value));
    
    int numChannels = g_activeImage ? g_activeImage->numChannels : 3;
    
    if (id == "MAD") {
        static std::vector<double> s_mad(16, 0.01);
        value->type = 0x0000000000280000; // VTYPE_DVECTOR
        value->dimX = numChannels;
        value->data.blockValue = s_mad.data();
    } else if (id == "Median") {
        static std::vector<double> s_median(16, 0.1);
        value->type = 0x0000000000280000; // VTYPE_DVECTOR
        value->dimX = numChannels;
        value->data.blockValue = s_median.data();
    } else if (id == "Mean") {
        static std::vector<double> s_mean(16, 0.1);
        value->type = 0x0000000000280000; // VTYPE_DVECTOR
        value->dimX = numChannels;
        value->data.blockValue = s_mean.data();
    } else {
        value->type = 0; // VTYPE_INVALID
    }
}

static api_bool mock_GetViewPropertyExists(api_handle hModule, const_view_handle hView, const char* id, uint64* type) {
    qDebug() << "[PCL Bridge] mock_GetViewPropertyExists called: view =" << hView << "id =" << id;
    std::string propId = id ? id : "";
    if (type) {
        if (propId == "MAD" || propId == "Median" || propId == "Mean") {
            *type = 0x0000000000280000; // VTYPE_DVECTOR
        } else {
            *type = 0; // VTYPE_INVALID
        }
    }
    return true;
}

static api_bool mock_ComputeViewProperty(api_handle hModule, view_handle hView, const char* id, api_bool notify, api_property_value* value) {
    qDebug() << "[PCL Bridge] mock_ComputeViewProperty called: view =" << hView << "id =" << id;
    std::string propId = id ? id : "";
    fillPropertyValue(value, propId);
    return true;
}

static api_bool mock_GetViewPropertyValue(api_handle hModule, const_view_handle hView, const char* id, api_property_value* value) {
    qDebug() << "[PCL Bridge] mock_GetViewPropertyValue called: view =" << hView << "id =" << id;
    std::string propId = id ? id : "";
    fillPropertyValue(value, propId);
    return true;
}

static api_bool mock_SetViewPropertyValue(api_handle hModule, view_handle hView, const char* id, const api_property_value* value, uint32 flags, api_bool notify) {
    qDebug() << "[PCL Bridge] mock_SetViewPropertyValue called: view =" << hView << "id =" << id;
    return true;
}

// ----------------------------------------------------------------------------
// Mock Module & Interface lifecycle routines
// ----------------------------------------------------------------------------

static void mock_SetModuleOnLoadRoutine(pcl::module_on_load_routine f) {
    g_moduleOnLoad = f;
    qDebug() << "[PCL Bridge] Module OnLoad routine registered.";
}

static void mock_SetModuleOnUnloadRoutine(pcl::module_on_unload_routine f) {
    g_moduleOnUnload = f;
    qDebug() << "[PCL Bridge] Module OnUnload routine registered.";
}

static void mock_BeginInterfaceDefinition(meta_interface_handle hMeta, const char* ifaceId, uint32 flags) {
    g_currentDefiningInterface = hMeta;
    PCLInterfaceInfo info;
    info.id = QString::fromUtf8(ifaceId);
    info.hMeta = hMeta;
    g_interfaces[hMeta] = info;
    g_interfaceIdToHandle[ifaceId] = hMeta;
    qDebug() << "[PCL Bridge] Defining interface:" << ifaceId;
}

static void mock_SetInterfaceInitializationRoutine(pcl::interface_initialization_routine f) {
    if (g_currentDefiningInterface) {
        g_interfaces[g_currentDefiningInterface].initFn = f;
        qDebug() << "[PCL Bridge] Interface initialization routine set for:" << g_interfaces[g_currentDefiningInterface].id;
    }
}

static void mock_SetInterfaceLaunchRoutine(pcl::interface_launch_routine f) {
    if (g_currentDefiningInterface) {
        g_interfaces[g_currentDefiningInterface].launchFn = f;
        qDebug() << "[PCL Bridge] Interface launch routine set for:" << g_interfaces[g_currentDefiningInterface].id;
    }
}

static void mock_EndInterfaceDefinition() {
    g_currentDefiningInterface = nullptr;
}

// ----------------------------------------------------------------------------
// Mock UI Controls & layouts C-API
// ----------------------------------------------------------------------------

static control_handle mock_CreateControl(api_handle hModule, api_handle client, control_handle parent, uint32 flags) {
    QWidget* parentWidget = reinterpret_cast<QWidget*>(parent);
    QWidget* w = new QWidget(parentWidget);
    control_handle ch = reinterpret_cast<control_handle>(w);
    if (client) g_widgetToClient[ch] = client;
    qDebug() << "[PCL Bridge] CreateControl: client =" << client << "parent =" << parentWidget << "-> QWidget =" << w;
    if (!parentWidget) {
        qDebug() << "[PCL Bridge] Detected top-level control creation. Associating with pending interface.";
        for (const auto& pair : g_interfaces) {
            std::string id = pair.second.id.toStdString();
            if (g_interfaceControls.find(id) == g_interfaceControls.end()) {
                g_interfaceControls[id] = ch;
                qDebug() << "[PCL Bridge] Associated top-level control" << w << "with interface ID:" << pair.second.id;
                break;
            }
        }
    }
    return ch;
}

static void mock_SetControlParent(control_handle h, control_handle parent) {
    QWidget* w = reinterpret_cast<QWidget*>(h);
    QWidget* p = reinterpret_cast<QWidget*>(parent);
    qDebug() << "[PCL Bridge] SetControlParent:" << w << "parent =" << p;
    if (w) {
        w->setParent(p);
    }
}

static void mock_SetControlSizer(control_handle h, sizer_handle s) {
    QWidget* w = reinterpret_cast<QWidget*>(h);
    QLayout* lay = reinterpret_cast<QLayout*>(s);
    qDebug() << "[PCL Bridge] SetControlSizer:" << w << "layout =" << lay;
    if (w && lay) {
        w->setLayout(lay);
    }
}

static void mock_SetControlMinSize(control_handle h, int32 w, int32 h_size) {
    QWidget* widget = reinterpret_cast<QWidget*>(h);
    qDebug() << "[PCL Bridge] SetControlMinSize:" << widget << "width =" << w << "height =" << h_size;
    if (widget) {
        if (w >= 0 && h_size >= 0) {
            widget->setMinimumSize(w, h_size);
        } else if (w >= 0) {
            widget->setMinimumWidth(w);
        } else if (h_size >= 0) {
            widget->setMinimumHeight(h_size);
        }
    }
}

static void mock_SetControlMaxSize(control_handle h, int32 w, int32 h_size) {
    QWidget* widget = reinterpret_cast<QWidget*>(h);
    qDebug() << "[PCL Bridge] SetControlMaxSize:" << widget << "width =" << w << "height =" << h_size;
    if (widget) {
        if (w >= 0 && h_size >= 0) {
            widget->setMaximumSize(w, h_size);
        } else if (w >= 0) {
            widget->setMaximumWidth(w);
        } else if (h_size >= 0) {
            widget->setMaximumHeight(h_size);
        }
    }
}

static void mock_AdjustControlToContents(control_handle h) {
    QWidget* widget = reinterpret_cast<QWidget*>(h);
    qDebug() << "[PCL Bridge] AdjustControlToContents:" << widget;
    if (widget) {
        widget->adjustSize();
    }
}

static control_handle mock_CreateDialog(api_handle hModule, api_handle client, control_handle parent, uint32 flags) {
    QWidget* parentWidget = reinterpret_cast<QWidget*>(parent);
    QDialog* dlg = new QDialog(parentWidget);
    control_handle ch = reinterpret_cast<control_handle>(dlg);
    if (client) g_widgetToClient[ch] = client;
    qDebug() << "[PCL Bridge] CreateDialog: client =" << client << "parent =" << parentWidget << "-> QDialog =" << dlg;
    if (!parentWidget) {
        qDebug() << "[PCL Bridge] Detected top-level dialog creation. Associating with pending interface.";
        for (const auto& pair : g_interfaces) {
            std::string id = pair.second.id.toStdString();
            if (g_interfaceControls.find(id) == g_interfaceControls.end()) {
                g_interfaceControls[id] = ch;
                qDebug() << "[PCL Bridge] Associated top-level dialog" << dlg << "with interface ID:" << pair.second.id;
                break;
            }
        }
    }
    return ch;
}

static sizer_handle mock_CreateSizer(api_handle hModule, api_bool vertical) {
    QLayout* lay = nullptr;
    if (vertical) {
        lay = new QVBoxLayout();
    } else {
        lay = new QHBoxLayout();
    }
    qDebug() << "[PCL Bridge] CreateSizer: vertical =" << (bool)vertical << "-> QLayout =" << lay;
    return reinterpret_cast<sizer_handle>(lay);
}

static void mock_InsertSizerControl(sizer_handle s, int32 index, control_handle c, int32 stretch, int32 alignment) {
    QBoxLayout* lay = reinterpret_cast<QBoxLayout*>(s);
    QWidget* w = reinterpret_cast<QWidget*>(c);
    qDebug() << "[PCL Bridge] InsertSizerControl: layout =" << lay << "widget =" << w << "index =" << index;
    if (lay && w) {
        if (index < 0) {
            lay->addWidget(w, stretch);
        } else {
            lay->insertWidget(index, w, stretch);
        }
    }
}

static void mock_InsertSizer(sizer_handle s, int32 index, sizer_handle childSizer, int32 stretch) {
    QBoxLayout* lay = reinterpret_cast<QBoxLayout*>(s);
    QLayout* childLay = reinterpret_cast<QLayout*>(childSizer);
    qDebug() << "[PCL Bridge] InsertSizer: layout =" << lay << "child layout =" << childLay << "index =" << index;
    if (lay && childLay) {
        if (index < 0) {
            lay->addLayout(childLay, stretch);
        } else {
            lay->insertLayout(index, childLay, stretch);
        }
    }
}

static void mock_InsertSizerSpacing(sizer_handle s, int32 index, int32 size) {
    QBoxLayout* lay = reinterpret_cast<QBoxLayout*>(s);
    qDebug() << "[PCL Bridge] InsertSizerSpacing: layout =" << lay << "size =" << size;
    if (lay) {
        if (index < 0) {
            lay->addSpacing(size);
        } else {
            lay->insertSpacing(index, size);
        }
    }
}

static void mock_InsertSizerStretch(sizer_handle s, int32 index, int32 stretch) {
    QBoxLayout* lay = reinterpret_cast<QBoxLayout*>(s);
    qDebug() << "[PCL Bridge] InsertSizerStretch: layout =" << lay << "stretch =" << stretch;
    if (lay) {
        if (index < 0) {
            lay->addStretch(stretch);
        } else {
            lay->insertStretch(index, stretch);
        }
    }
}

static void mock_SetSizerMargin(sizer_handle s, int32 margin) {
    QLayout* lay = reinterpret_cast<QLayout*>(s);
    qDebug() << "[PCL Bridge] SetSizerMargin: layout =" << lay << "margin =" << margin;
    if (lay) {
        lay->setContentsMargins(margin, margin, margin, margin);
    }
}

static void mock_SetSizerSpacing(sizer_handle s, int32 spacing) {
    QBoxLayout* lay = reinterpret_cast<QBoxLayout*>(s);
    qDebug() << "[PCL Bridge] SetSizerSpacing: layout =" << lay << "spacing =" << spacing;
    if (lay) {
        lay->setSpacing(spacing);
    }
}

static control_handle mock_CreateGroupBox(api_handle hModule, api_handle client, const char16_type* title, control_handle parent, uint32 flags) {
    QWidget* parentWidget = reinterpret_cast<QWidget*>(parent);
    QGroupBox* gb = new QGroupBox(parentWidget);
    if (title) {
        gb->setTitle(QString::fromUtf16(reinterpret_cast<const char16_t*>(title)));
    }
    control_handle ch = reinterpret_cast<control_handle>(gb);
    if (client) g_widgetToClient[ch] = client;
    qDebug() << "[PCL Bridge] CreateGroupBox: client =" << client << "title =" << gb->title() << "-> QGroupBox =" << gb;
    return ch;
}

static control_handle mock_CreatePushButton(api_handle hModule, api_handle client, const char16_type* text, const_bitmap_handle bitmap, control_handle parent, uint32 flags) {
    QWidget* parentWidget = reinterpret_cast<QWidget*>(parent);
    QPushButton* btn = new QPushButton(parentWidget);
    if (text) {
        btn->setText(QString::fromUtf16(reinterpret_cast<const char16_t*>(text)));
    }
    control_handle ch = reinterpret_cast<control_handle>(btn);
    if (client) g_widgetToClient[ch] = client;
    qDebug() << "[PCL Bridge] CreatePushButton: client =" << client << "text =" << btn->text() << "-> QPushButton =" << btn;
    return ch;
}

static control_handle mock_CreateCheckBox(api_handle hModule, api_handle client, const char16_type* text, control_handle parent, uint32 flags) {
    QWidget* parentWidget = reinterpret_cast<QWidget*>(parent);
    QCheckBox* cb = new QCheckBox(parentWidget);
    if (text) {
        cb->setText(QString::fromUtf16(reinterpret_cast<const char16_t*>(text)));
    }
    control_handle ch = reinterpret_cast<control_handle>(cb);
    if (client) g_widgetToClient[ch] = client;
    qDebug() << "[PCL Bridge] CreateCheckBox: client =" << client << "text =" << cb->text() << "-> QCheckBox =" << cb;
    return ch;
}

static void mock_SetButtonText(control_handle h, const char16_type* text) {
    QAbstractButton* btn = reinterpret_cast<QAbstractButton*>(h);
    QString str = text ? QString::fromUtf16(reinterpret_cast<const char16_t*>(text)) : QString();
    qDebug() << "[PCL Bridge] SetButtonText:" << btn << "text =" << str;
    if (btn) {
        btn->setText(str);
    }
}

static control_handle mock_CreateComboBox(api_handle hModule, api_handle client, control_handle parent, uint32 flags) {
    QWidget* parentWidget = reinterpret_cast<QWidget*>(parent);
    QComboBox* cb = new QComboBox(parentWidget);
    control_handle ch = reinterpret_cast<control_handle>(cb);
    if (client) g_widgetToClient[ch] = client;
    qDebug() << "[PCL Bridge] CreateComboBox: client =" << client << "parent =" << parentWidget << "-> QComboBox =" << cb;
    return ch;
}

static void mock_InsertComboBoxItem(control_handle h, int32 index, const char16_type* text, const_bitmap_handle bitmap) {
    QComboBox* cb = reinterpret_cast<QComboBox*>(h);
    QString str = text ? QString::fromUtf16(reinterpret_cast<const char16_t*>(text)) : QString();
    qDebug() << "[PCL Bridge] InsertComboBoxItem:" << cb << "index =" << index << "text =" << str;
    if (cb) {
        cb->insertItem(index, str);
    }
}

static void mock_ClearComboBox(control_handle h) {
    QComboBox* cb = reinterpret_cast<QComboBox*>(h);
    qDebug() << "[PCL Bridge] ClearComboBox:" << cb;
    if (cb) {
        cb->clear();
    }
}

static void mock_SetComboBoxCurrentItem(control_handle h, int32 index) {
    QComboBox* cb = reinterpret_cast<QComboBox*>(h);
    qDebug() << "[PCL Bridge] SetComboBoxCurrentItem:" << cb << "index =" << index;
    if (cb) {
        cb->blockSignals(true);
        cb->setCurrentIndex(index);
        cb->blockSignals(false);
    }
}

static int32 mock_GetComboBoxCurrentItem(const_control_handle h) {
    const QComboBox* cb = reinterpret_cast<const QComboBox*>(h);
    int32 idx = cb ? cb->currentIndex() : -1;
    qDebug() << "[PCL Bridge] GetComboBoxCurrentItem:" << cb << "->" << idx;
    return idx;
}

static control_handle mock_CreateSpinBox(api_handle hModule, api_handle client, control_handle parent, uint32 flags) {
    QWidget* parentWidget = reinterpret_cast<QWidget*>(parent);
    QSpinBox* sb = new QSpinBox(parentWidget);
    control_handle ch = reinterpret_cast<control_handle>(sb);
    if (client) g_widgetToClient[ch] = client;
    qDebug() << "[PCL Bridge] CreateSpinBox: client =" << client << "parent =" << parentWidget << "-> QSpinBox =" << sb;
    return ch;
}

static void mock_SetSpinBoxRange(control_handle h, int32 minVal, int32 maxVal) {
    QSpinBox* sb = reinterpret_cast<QSpinBox*>(h);
    qDebug() << "[PCL Bridge] SetSpinBoxRange:" << sb << "range = [" << minVal << "," << maxVal << "]";
    if (sb) {
        sb->blockSignals(true);
        sb->setRange(minVal, maxVal);
        sb->blockSignals(false);
    }
}

static void mock_SetSpinBoxValue(control_handle h, int32 val) {
    QSpinBox* sb = reinterpret_cast<QSpinBox*>(h);
    qDebug() << "[PCL Bridge] SetSpinBoxValue:" << sb << "value =" << val;
    if (sb) {
        sb->blockSignals(true);
        sb->setValue(val);
        sb->blockSignals(false);
    }
}

static int32 mock_GetSpinBoxValue(const_control_handle h) {
    const QSpinBox* sb = reinterpret_cast<const QSpinBox*>(h);
    int32 val = sb ? sb->value() : 0;
    qDebug() << "[PCL Bridge] GetSpinBoxValue:" << sb << "->" << val;
    return val;
}

static control_handle mock_CreateLabel(api_handle hModule, api_handle client, const char16_type* text, control_handle parent, uint32 flags) {
    QWidget* parentWidget = reinterpret_cast<QWidget*>(parent);
    QLabel* lbl = new QLabel(parentWidget);
    if (text) {
        lbl->setText(QString::fromUtf16(reinterpret_cast<const char16_t*>(text)));
    }
    control_handle ch = reinterpret_cast<control_handle>(lbl);
    if (client) g_widgetToClient[ch] = client;
    qDebug() << "[PCL Bridge] CreateLabel: client =" << client << "text =" << lbl->text() << "-> QLabel =" << lbl;
    return ch;
}

static void mock_SetLabelText(control_handle h, const char16_type* text) {
    QLabel* lbl = reinterpret_cast<QLabel*>(h);
    QString str = text ? QString::fromUtf16(reinterpret_cast<const char16_t*>(text)) : QString();
    qDebug() << "[PCL Bridge] SetLabelText:" << lbl << "text =" << str;
    if (lbl) {
        lbl->setText(str);
    }
}

static control_handle mock_CreateSlider(api_handle hModule, api_handle client, api_bool vertical, control_handle parent, uint32 flags) {
    QWidget* parentWidget = reinterpret_cast<QWidget*>(parent);
    QSlider* sl = new QSlider(vertical ? Qt::Vertical : Qt::Horizontal, parentWidget);
    control_handle ch = reinterpret_cast<control_handle>(sl);
    if (client) g_widgetToClient[ch] = client;
    qDebug() << "[PCL Bridge] CreateSlider: client =" << client << "vertical =" << (bool)vertical << "-> QSlider =" << sl;
    return ch;
}


static control_handle mock_CreateEdit(api_handle hModule, api_handle client, const char16_type* text, control_handle parent, uint32 flags) {
    QWidget* parentWidget = reinterpret_cast<QWidget*>(parent);
    QLineEdit* edit = new QLineEdit(parentWidget);
    if (text) {
        edit->setText(QString::fromUtf16(reinterpret_cast<const char16_t*>(text)));
    }
    control_handle ch = reinterpret_cast<control_handle>(edit);
    if (client) g_widgetToClient[ch] = client;
    qDebug() << "[PCL Bridge] CreateEdit: client =" << client << "parent =" << parentWidget << "text =" << edit->text() << "-> QLineEdit =" << edit;
    return ch;
}

static api_bool mock_GetEditText(const_control_handle h, char16_type* text, pcl::size_type* length) {
    const QLineEdit* edit = reinterpret_cast<const QLineEdit*>(h);
    QString str = edit ? edit->text() : QString();
    qDebug() << "[PCL Bridge] GetEditText:" << edit << "->" << str;
    if (!length) return false;
    if (!text) {
        *length = str.length();
        return true;
    }
    pcl::size_type copyLen = std::min(*length, (pcl::size_type)str.length());
    std::memcpy(text, str.utf16(), copyLen * sizeof(char16_t));
    text[copyLen] = 0;
    *length = copyLen;
    return true;
}

static void mock_SetEditText(control_handle h, const char16_type* text) {
    QLineEdit* edit = reinterpret_cast<QLineEdit*>(h);
    QString str = text ? QString::fromUtf16(reinterpret_cast<const char16_t*>(text)) : QString();
    qDebug() << "[PCL Bridge] SetEditText:" << edit << "text =" << str;
    if (edit) {
        edit->blockSignals(true);
        edit->setText(str);
        edit->blockSignals(false);
    }
}

static api_bool mock_GetEditReadOnly(const_control_handle h) {
    const QLineEdit* edit = reinterpret_cast<const QLineEdit*>(h);
    return edit ? edit->isReadOnly() : false;
}

static void mock_SetEditReadOnly(control_handle h, api_bool readOnly) {
    QLineEdit* edit = reinterpret_cast<QLineEdit*>(h);
    qDebug() << "[PCL Bridge] SetEditReadOnly:" << edit << "readOnly =" << (bool)readOnly;
    if (edit) {
        edit->setReadOnly(readOnly);
    }
}

static pcl::size_type mock_GetUIObjectRefCount(const_api_handle h) {
    return h ? 1 : 0;
}

struct PCLThreadMock {
    api_handle client = nullptr;
    pcl::thread_exec_routine execFn = nullptr;
    bool active = false;
    uint32 status = 0;
};

static std::unordered_map<api_handle, PCLThreadMock*> g_threads;
static thread_local thread_handle g_currentThread = nullptr;

static thread_handle mock_CreateThread(api_handle hModule, api_handle client, uint32 flags) {
    qDebug() << "[PCL Bridge] mock_CreateThread called: client =" << client;
    auto* thread = new PCLThreadMock();
    thread->client = client;
    g_threads[client] = thread;
    return client;
}

static api_bool mock_SetThreadExecRoutine(thread_handle hThread, pcl::thread_exec_routine execFn) {
    qDebug() << "[PCL Bridge] mock_SetThreadExecRoutine called: thread =" << hThread;
    auto it = g_threads.find(hThread);
    if (it != g_threads.end()) {
        it->second->execFn = execFn;
        return true;
    }
    return false;
}

static void mock_StartThread(thread_handle hThread, uint32 priority) {
    qDebug() << "[PCL Bridge] mock_StartThread called: thread =" << hThread << "priority =" << priority;
    auto it = g_threads.find(hThread);
    if (it != g_threads.end()) {
        auto* thread = it->second;
        if (thread && thread->execFn) {
            thread->active = true;
            qDebug() << "[PCL Bridge] Spawning background thread asynchronously...";
            std::thread([hThread]() {
                g_currentThread = hThread;
                auto itInner = g_threads.find(hThread);
                if (itInner != g_threads.end()) {
                    auto* threadInner = itInner->second;
                    if (threadInner && threadInner->execFn) {
                        try {
                            threadInner->execFn(hThread);
                        } catch (...) {
                            qWarning() << "[PCL Bridge] Exception in background thread";
                        }
                        threadInner->active = false;
                        qDebug() << "[PCL Bridge] Background thread completed.";
                    }
                }
            }).detach();
        }
    }
}

static api_bool mock_IsThreadActive(const_thread_handle hThread) {
    auto it = g_threads.find(const_cast<thread_handle>(hThread));
    if (it != g_threads.end()) {
        return it->second->active;
    }
    return false;
}

static api_bool mock_WaitThread(thread_handle hThread, uint32 msec) {
    qDebug() << "[PCL Bridge] mock_WaitThread called: thread =" << hThread << "msec =" << msec;
    auto it = g_threads.find(hThread);
    if (it != g_threads.end()) {
        auto* thread = it->second;
        if (thread) {
            if (msec == 0xFFFFFFFF) {
                while (thread->active) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
            } else {
                uint32 elapsed = 0;
                while (thread->active && elapsed < msec) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    elapsed += 5;
                }
            }
        }
    }
    return true;
}

static void mock_SleepThread(thread_handle hThread, uint32 msec) {
    qDebug() << "[PCL Bridge] mock_SleepThread called: thread =" << hThread << "msec =" << msec;
    std::this_thread::sleep_for(std::chrono::milliseconds(msec));
}

static uint32 mock_GetThreadStatus(const_thread_handle hThread) {
    auto it = g_threads.find(const_cast<thread_handle>(hThread));
    if (it != g_threads.end()) {
        return it->second->status;
    }
    return 0;
}

static void mock_SetThreadStatus(thread_handle hThread, uint32 status) {
    auto it = g_threads.find(hThread);
    if (it != g_threads.end()) {
        it->second->status = status;
    }
}

static void mock_KillThread(thread_handle hThread) {
    qDebug() << "[PCL Bridge] mock_KillThread called: thread =" << hThread;
    auto it = g_threads.find(hThread);
    if (it != g_threads.end()) {
        it->second->active = false;
    }
}

static thread_handle mock_GetCurrentThread() {
    return g_currentThread;
}

static void mock_AppendThreadConsoleOutputText(thread_handle, const char16_type* text, api_bool appendNewline) {
    QString str = QString::fromUtf16(reinterpret_cast<const char16_t*>(text));
    if (appendNewline) {
        qDebug().noquote() << "[PCL Thread Console]" << str;
    } else {
        std::cout << str.toStdString();
        std::flush(std::cout);
    }
}

static api_bool mock_GetSizerDisplayPixelRatio(const_sizer_handle h, double* ratio) {
    qDebug() << "[PCL Bridge] GetSizerDisplayPixelRatio:" << h;
    if (ratio) {
        *ratio = 1.0;
    }
    return true;
}

static api_bool mock_AttachToUIObject(api_handle hModule, api_handle hUIObject) {
    qDebug() << "[PCL Bridge] AttachToUIObject: UIObject =" << hUIObject;
    return true;
}

static api_bool mock_DetachFromUIObject(api_handle hModule, api_handle hUIObject) {
    qDebug() << "[PCL Bridge] DetachFromUIObject: UIObject =" << hUIObject;
    return true;
}

static void mock_SetLabelAlignment(control_handle h, int32 align) {
    QLabel* lbl = reinterpret_cast<QLabel*>(h);
    qDebug() << "[PCL Bridge] SetLabelAlignment:" << lbl << "align =" << align;
    if (lbl) {
        Qt::Alignment qtAlign = Qt::AlignLeft | Qt::AlignVCenter;
        if (align == 1) qtAlign = Qt::AlignCenter;
        else if (align == 2) qtAlign = Qt::AlignRight | Qt::AlignVCenter;
        lbl->setAlignment(qtAlign);
    }
}

static api_bool mock_SetEventRoutine(control_handle h, api_handle receiver, void* routine) {
    qDebug() << "[PCL Bridge] SetEventRoutine: control =" << h << "receiver =" << receiver << "routine =" << routine;
    return true;
}

static api_bool mock_SetSliderValueUpdatedEventRoutine(control_handle h, api_handle receiver, pcl::value_event_routine routine) {
    QSlider* slider = reinterpret_cast<QSlider*>(h);
    qDebug() << "[PCL Bridge] SetSliderValueUpdatedEventRoutine: slider =" << slider << "receiver =" << receiver << "routine =" << routine;
    if (slider) {
        slider->setProperty("valueUpdatedRoutine", QVariant::fromValue(reinterpret_cast<void*>(routine)));
        slider->setProperty("valueUpdatedReceiver", QVariant::fromValue(reinterpret_cast<void*>(receiver)));
        // Store the PCL sender (client) so the dispatcher can cast it back to Slider*
        auto clientIt = g_widgetToClient.find(h);
        slider->setProperty("pclSender", QVariant::fromValue(clientIt != g_widgetToClient.end() ? clientIt->second : (api_handle)h));
        
        slider->disconnect(slider, &QSlider::valueChanged, nullptr, nullptr);
        
        QObject::connect(slider, &QSlider::valueChanged, [slider](int value) {
            void* r = slider->property("valueUpdatedRoutine").value<void*>();
            void* rec = slider->property("valueUpdatedReceiver").value<void*>();
            void* snd = slider->property("pclSender").value<void*>();
            if (r) {
                auto callback = reinterpret_cast<pcl::value_event_routine>(r);
                callback(reinterpret_cast<control_handle>(snd), reinterpret_cast<control_handle>(rec), value);
            }
        });
        return true;
    }
    return false;
}

static api_bool mock_SetEditCompletedEventRoutine(control_handle h, api_handle receiver, pcl::event_routine routine) {
    QLineEdit* edit = reinterpret_cast<QLineEdit*>(h);
    qDebug() << "[PCL Bridge] SetEditCompletedEventRoutine: edit =" << edit << "receiver =" << receiver << "routine =" << routine;
    if (edit) {
        edit->setProperty("completedRoutine", QVariant::fromValue(reinterpret_cast<void*>(routine)));
        edit->setProperty("completedReceiver", QVariant::fromValue(reinterpret_cast<void*>(receiver)));
        auto clientIt = g_widgetToClient.find(h);
        edit->setProperty("pclSender", QVariant::fromValue(clientIt != g_widgetToClient.end() ? clientIt->second : (api_handle)h));
        
        edit->disconnect(edit, &QLineEdit::editingFinished, nullptr, nullptr);
        
        QObject::connect(edit, &QLineEdit::editingFinished, [edit]() {
            void* r = edit->property("completedRoutine").value<void*>();
            void* rec = edit->property("completedReceiver").value<void*>();
            void* snd = edit->property("pclSender").value<void*>();
            if (r) {
                auto callback = reinterpret_cast<pcl::event_routine>(r);
                callback(reinterpret_cast<control_handle>(snd), reinterpret_cast<control_handle>(rec));
            }
        });
        return true;
    }
    return false;
}

static api_bool mock_SetTextUpdatedEventRoutine(control_handle h, api_handle receiver, pcl::unicode_event_routine routine) {
    QLineEdit* edit = reinterpret_cast<QLineEdit*>(h);
    qDebug() << "[PCL Bridge] SetTextUpdatedEventRoutine: edit =" << edit << "receiver =" << receiver << "routine =" << routine;
    if (edit) {
        edit->setProperty("textUpdatedRoutine", QVariant::fromValue(reinterpret_cast<void*>(routine)));
        edit->setProperty("textUpdatedReceiver", QVariant::fromValue(reinterpret_cast<void*>(receiver)));
        auto clientIt = g_widgetToClient.find(h);
        edit->setProperty("pclSender", QVariant::fromValue(clientIt != g_widgetToClient.end() ? clientIt->second : (api_handle)h));
        
        edit->disconnect(edit, &QLineEdit::textChanged, nullptr, nullptr);
        
        QObject::connect(edit, &QLineEdit::textChanged, [edit](const QString& text) {
            void* r = edit->property("textUpdatedRoutine").value<void*>();
            void* rec = edit->property("textUpdatedReceiver").value<void*>();
            void* snd = edit->property("pclSender").value<void*>();
            if (r) {
                auto callback = reinterpret_cast<pcl::unicode_event_routine>(r);
                callback(reinterpret_cast<control_handle>(snd), reinterpret_cast<control_handle>(rec), reinterpret_cast<const char16_type*>(text.utf16()));
            }
        });
        return true;
    }
    return false;
}

static api_bool mock_SetComboBoxItemSelectedEventRoutine(control_handle h, api_handle receiver, pcl::value_event_routine routine) {
    QComboBox* combo = reinterpret_cast<QComboBox*>(h);
    qDebug() << "[PCL Bridge] SetComboBoxItemSelectedEventRoutine: combo =" << combo << "receiver =" << receiver << "routine =" << routine;
    if (combo) {
        combo->setProperty("itemSelectedRoutine", QVariant::fromValue(reinterpret_cast<void*>(routine)));
        combo->setProperty("itemSelectedReceiver", QVariant::fromValue(reinterpret_cast<void*>(receiver)));
        auto clientIt = g_widgetToClient.find(h);
        combo->setProperty("pclSender", QVariant::fromValue(clientIt != g_widgetToClient.end() ? clientIt->second : (api_handle)h));
        
        combo->disconnect(combo, &QComboBox::currentIndexChanged, nullptr, nullptr);
        
        QObject::connect(combo, &QComboBox::currentIndexChanged, [combo](int index) {
            void* r = combo->property("itemSelectedRoutine").value<void*>();
            void* rec = combo->property("itemSelectedReceiver").value<void*>();
            void* snd = combo->property("pclSender").value<void*>();
            if (r) {
                auto callback = reinterpret_cast<pcl::value_event_routine>(r);
                callback(reinterpret_cast<control_handle>(snd), reinterpret_cast<control_handle>(rec), index);
            }
        });
        return true;
    }
    return false;
}

static api_bool mock_SetButtonCheckEventRoutine(control_handle h, api_handle receiver, pcl::button_check_event_routine routine) {
    QCheckBox* check = reinterpret_cast<QCheckBox*>(h);
    qDebug() << "[PCL Bridge] SetButtonCheckEventRoutine: check =" << check << "receiver =" << receiver << "routine =" << routine;
    if (check) {
        check->setProperty("checkRoutine", QVariant::fromValue(reinterpret_cast<void*>(routine)));
        check->setProperty("checkReceiver", QVariant::fromValue(reinterpret_cast<void*>(receiver)));
        auto clientIt = g_widgetToClient.find(h);
        check->setProperty("pclSender", QVariant::fromValue(clientIt != g_widgetToClient.end() ? clientIt->second : (api_handle)h));
        
        check->disconnect(check, &QCheckBox::stateChanged, nullptr, nullptr);
        
        QObject::connect(check, &QCheckBox::stateChanged, [check](int state) {
            void* r = check->property("checkRoutine").value<void*>();
            void* rec = check->property("checkReceiver").value<void*>();
            void* snd = check->property("pclSender").value<void*>();
            if (r) {
                auto callback = reinterpret_cast<pcl::button_check_event_routine>(r);
                int32 pclState = 0;
                if (state == Qt::Checked) pclState = 1;
                else if (state == Qt::PartiallyChecked) pclState = 2;
                callback(reinterpret_cast<control_handle>(snd), reinterpret_cast<control_handle>(rec), pclState);
            }
        });
        return true;
    }
    return false;
}

static api_bool mock_SetButtonClickEventRoutine(control_handle h, api_handle receiver, pcl::button_click_event_routine routine) {
    QPushButton* btn = reinterpret_cast<QPushButton*>(h);
    qDebug() << "[PCL Bridge] SetButtonClickEventRoutine: btn =" << btn << "receiver =" << receiver << "routine =" << routine;
    if (btn) {
        btn->setProperty("clickRoutine", QVariant::fromValue(reinterpret_cast<void*>(routine)));
        btn->setProperty("clickReceiver", QVariant::fromValue(reinterpret_cast<void*>(receiver)));
        auto clientIt = g_widgetToClient.find(h);
        btn->setProperty("pclSender", QVariant::fromValue(clientIt != g_widgetToClient.end() ? clientIt->second : (api_handle)h));
        
        btn->disconnect(btn, &QPushButton::clicked, nullptr, nullptr);
        
        QObject::connect(btn, &QPushButton::clicked, [btn](bool checked) {
            void* r = btn->property("clickRoutine").value<void*>();
            void* rec = btn->property("clickReceiver").value<void*>();
            void* snd = btn->property("pclSender").value<void*>();
            if (r) {
                auto callback = reinterpret_cast<pcl::button_click_event_routine>(r);
                callback(reinterpret_cast<control_handle>(snd), reinterpret_cast<control_handle>(rec), checked);
            }
        });
        return true;
    }
    return false;
}

static void mock_GetClientRect(const_control_handle h, int32* x1, int32* y1, int32* x2, int32* y2) {
    const QWidget* w = reinterpret_cast<const QWidget*>(h);
    qDebug() << "[PCL Bridge] GetClientRect:" << w;
    if (w) {
        if (x1) *x1 = 0;
        if (y1) *y1 = 0;
        if (x2) *x2 = w->width();
        if (y2) *y2 = w->height();
    }
}

static void mock_SetControlFixedSize(control_handle h, int32 w, int32 h_size) {
    QWidget* widget = reinterpret_cast<QWidget*>(h);
    qDebug() << "[PCL Bridge] SetControlFixedSize:" << widget << "width =" << w << "height =" << h_size;
    if (widget) {
        if (w >= 0 && h_size >= 0) {
            widget->setFixedSize(w, h_size);
        } else if (w >= 0) {
            widget->setFixedWidth(w);
        } else if (h_size >= 0) {
            widget->setFixedHeight(h_size);
        }
    }
}

static void mock_SetControlBackgroundColor(control_handle h, uint32 color) {
    QWidget* widget = reinterpret_cast<QWidget*>(h);
    qDebug() << "[PCL Bridge] SetControlBackgroundColor:" << widget << "color =" << color;
    if (widget) {
        QPalette pal = widget->palette();
        // Pack color is typically ARGB or RGBA. Let's assume standard QColor construction from packed QRgb
        pal.setColor(QPalette::Window, QColor::fromRgba(color));
        widget->setAutoFillBackground(true);
        widget->setPalette(pal);
    }
}

static void mock_SetControlVisible(control_handle h, api_bool visible) {
    QWidget* widget = reinterpret_cast<QWidget*>(h);
    qDebug() << "[PCL Bridge] SetControlVisible:" << widget << "visible =" << (bool)visible;
    if (widget) {
        widget->setVisible(visible);
    }
}

static void mock_SetControlEnabled(control_handle h, api_bool enabled) {
    QWidget* widget = reinterpret_cast<QWidget*>(h);
    qDebug() << "[PCL Bridge] SetControlEnabled:" << widget << "enabled =" << (bool)enabled;
    if (widget) {
        widget->setEnabled(enabled);
    }
}

static void mock_EnsureControlLayoutUpdated(control_handle h) {
    QWidget* widget = reinterpret_cast<QWidget*>(h);
    qDebug() << "[PCL Bridge] EnsureControlLayoutUpdated:" << widget;
    if (widget) {
        widget->adjustSize();
        qApp->processEvents();
    }
}

static void mock_SetChildControlToFocus(control_handle h, control_handle child) {
    QWidget* widget = reinterpret_cast<QWidget*>(h);
    QWidget* childWidget = reinterpret_cast<QWidget*>(child);
    qDebug() << "[PCL Bridge] SetChildControlToFocus: parent =" << widget << "child =" << childWidget;
    if (childWidget) {
        childWidget->setFocus();
    }
}

static api_bool mock_SetEditValidatingRegExp(control_handle h, const char16_type* pattern, api_bool caseSensitive) {
    QLineEdit* edit = reinterpret_cast<QLineEdit*>(h);
    QString regexStr = pattern ? QString::fromUtf16(reinterpret_cast<const char16_t*>(pattern)) : QString();
    qDebug() << "[PCL Bridge] SetEditValidatingRegExp:" << edit << "pattern =" << regexStr << "caseSensitive =" << (bool)caseSensitive;
    return true;
}

static void mock_SetSliderRange(control_handle h, int32 minVal, int32 maxVal) {
    QSlider* sl = reinterpret_cast<QSlider*>(h);
    qDebug() << "[PCL Bridge] SetSliderRange:" << sl << "range = [" << minVal << "," << maxVal << "]";
    if (sl) {
        sl->blockSignals(true);
        sl->setRange(minVal, maxVal);
        sl->blockSignals(false);
    }
}

static void mock_GetSliderRange(const_control_handle h, int32* minVal, int32* maxVal) {
    const QSlider* sl = reinterpret_cast<const QSlider*>(h);
    if (sl) {
        if (minVal) *minVal = sl->minimum();
        if (maxVal) *maxVal = sl->maximum();
    }
    qDebug() << "[PCL Bridge] GetSliderRange:" << sl << "->" << (minVal ? *minVal : 0) << "," << (maxVal ? *maxVal : 0);
}

static void mock_SetSliderValue(control_handle h, int32 val) {
    QSlider* sl = reinterpret_cast<QSlider*>(h);
    qDebug() << "[PCL Bridge] SetSliderValue:" << sl << "value =" << val;
    if (sl) {
        sl->blockSignals(true);
        sl->setValue(val);
        sl->blockSignals(false);
    }
}

static int32 mock_GetSliderValue(const_control_handle h) {
    const QSlider* sl = reinterpret_cast<const QSlider*>(h);
    int32 val = sl ? sl->value() : 0;
    qDebug() << "[PCL Bridge] GetSliderValue:" << sl << "->" << val;
    return val;
}

static void mock_SetSliderStepSize(control_handle h, int32 step) {
    QSlider* sl = reinterpret_cast<QSlider*>(h);
    if (sl) {
        sl->setSingleStep(step);
    }
}

static int32 mock_GetSliderStepSize(const_control_handle h) {
    const QSlider* sl = reinterpret_cast<const QSlider*>(h);
    return sl ? sl->singleStep() : 1;
}

static void mock_SetSliderPageSize(control_handle h, int32 page) {
    QSlider* sl = reinterpret_cast<QSlider*>(h);
    if (sl) {
        sl->setPageStep(page);
    }
}

static int32 mock_GetSliderPageSize(const_control_handle h) {
    const QSlider* sl = reinterpret_cast<const QSlider*>(h);
    return sl ? sl->pageStep() : 10;
}

static void mock_SetSliderTickInterval(control_handle h, int32 interval) {
    QSlider* sl = reinterpret_cast<QSlider*>(h);
    if (sl) {
        sl->setTickInterval(interval);
    }
}

static int32 mock_GetSliderTickInterval(const_control_handle h) {
    const QSlider* sl = reinterpret_cast<const QSlider*>(h);
    return sl ? sl->tickInterval() : 0;
}

static void mock_SetSliderTickStyle(control_handle h, int32 style) {
    QSlider* sl = reinterpret_cast<QSlider*>(h);
    if (sl) {
        sl->setTickPosition(style ? QSlider::TicksBelow : QSlider::NoTicks);
    }
}

static int32 mock_GetSliderTickStyle(const_control_handle h) {
    const QSlider* sl = reinterpret_cast<const QSlider*>(h);
    return sl ? (sl->tickPosition() == QSlider::NoTicks ? 0 : 1) : 0;
}

static void mock_SetSliderTrackingEnabled(control_handle h, api_bool enable) {
    QSlider* sl = reinterpret_cast<QSlider*>(h);
    if (sl) {
        sl->setTracking(enable);
    }
}

static api_bool mock_GetSliderTrackingEnabled(const_control_handle h) {
    const QSlider* sl = reinterpret_cast<const QSlider*>(h);
    return sl ? sl->hasTracking() : true;
}

static api_bool mock_GetControlDisplayPixelRatio(const_control_handle h, double* ratio) {
    qDebug() << "[PCL Bridge] GetControlDisplayPixelRatio:" << h;
    if (ratio) {
        *ratio = 1.0;
    }
    return true;
}

static font_handle mock_GetControlFont(const_control_handle h) {
    qDebug() << "[PCL Bridge] GetControlFont:" << h;
    return reinterpret_cast<font_handle>(1);
}

static int32 mock_GetStringPixelWidth(const_font_handle f, const char16_type* text) {
    QString str = text ? QString::fromUtf16(reinterpret_cast<const char16_t*>(text)) : QString();
    QFont font = QApplication::font();
    QFontMetrics fm(font);
    int32 w = fm.horizontalAdvance(str);
    qDebug() << "[PCL Bridge] GetStringPixelWidth: text =" << str << "->" << w;
    return w;
}

static int32 mock_GetComboBoxLength(const_control_handle h) {
    const QComboBox* cb = reinterpret_cast<const QComboBox*>(h);
    int32 len = cb ? cb->count() : 0;
    qDebug() << "[PCL Bridge] GetComboBoxLength:" << cb << "->" << len;
    return len;
}

static void mock_SetButtonChecked(control_handle h, uint32 checked) {
    QAbstractButton* btn = reinterpret_cast<QAbstractButton*>(h);
    qDebug() << "[PCL Bridge] SetButtonChecked:" << btn << "checked =" << checked;
    if (btn) {
        btn->blockSignals(true);
        btn->setChecked(checked != 0);
        btn->blockSignals(false);
    }
}

static uint32 mock_GetButtonChecked(const_control_handle h) {
    const QAbstractButton* btn = reinterpret_cast<const QAbstractButton*>(h);
    uint32 val = btn ? (btn->isChecked() ? 1 : 0) : 0;
    qDebug() << "[PCL Bridge] GetButtonChecked:" << btn << "->" << val;
    return val;
}

static void mock_SetWindowTitle(control_handle h, const char16_type* title) {
    QWidget* w = reinterpret_cast<QWidget*>(h);
    QString str = title ? QString::fromUtf16(reinterpret_cast<const char16_t*>(title)) : QString();
    qDebug() << "[PCL Bridge] SetWindowTitle:" << w << "title =" << str;
    if (w) {
        w->setWindowTitle(str);
    }
}

static void mock_SetWindowToolTip(control_handle h, const char16_type* text) {
    QWidget* w = reinterpret_cast<QWidget*>(h);
    QString str = text ? QString::fromUtf16(reinterpret_cast<const char16_t*>(text)) : QString();
    qDebug() << "[PCL Bridge] SetWindowToolTip:" << w << "text =" << str;
    if (w) {
        w->setToolTip(str);
    }
}

static void mock_GetControlPosition(const_control_handle h, int32* x, int32* y) {
    const QWidget* w = reinterpret_cast<const QWidget*>(h);
    if (w) {
        if (x) *x = w->x();
        if (y) *y = w->y();
    }
    qDebug() << "[PCL Bridge] GetControlPosition:" << w << "->" << (x ? *x : 0) << "," << (y ? *y : 0);
}

static void mock_SetControlPosition(control_handle h, int32 x, int32 y) {
    QWidget* w = reinterpret_cast<QWidget*>(h);
    qDebug() << "[PCL Bridge] SetControlPosition:" << w << "x =" << x << "y =" << y;
    if (w) {
        w->move(x, y);
    }
}
static api_bool mock_GetViewId(const_view_handle hView, char* id, pcl::size_type* length) {
    if (!hView) return false;
    const auto* view = reinterpret_cast<const PCLViewMock*>(hView);
    std::string str = view->id.toStdString();
    qDebug() << "[PCL Bridge] GetViewId/FullId: view =" << view << "id =" << view->id;
    if (!length) return false;
    if (!id) {
        *length = str.length();
        return true;
    }
    pcl::size_type copyLen = std::min(*length, (pcl::size_type)str.length());
    std::memcpy(id, str.c_str(), copyLen);
    id[copyLen] = 0;
    *length = copyLen;
    return true;
}

static void mock_GetViewLocks(const_view_handle hView, api_bool* readLocked, api_bool* writeLocked) {
    qDebug() << "[PCL Bridge] mock_GetViewLocks called: view =" << hView;
    if (readLocked) *readLocked = true;
    if (writeLocked) *writeLocked = true;
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
    overridePCLStub("Global/Abort", reinterpret_cast<void*>(mock_Abort));
    overridePCLStub("Global/DisableAbort", reinterpret_cast<void*>(mock_DisableAbort));
    overridePCLStub("Global/EnableAbort", reinterpret_cast<void*>(mock_EnableAbort));

    overridePCLStub("Process/GetParameterValue", reinterpret_cast<void*>(mock_GetParameterValue));
    overridePCLStub("Process/SetParameterValue", reinterpret_cast<void*>(mock_SetParameterValue));

    overridePCLStub("ModuleDefinition/EnterModuleDefinitionContext", reinterpret_cast<void*>(mock_EnterModuleDefinitionContext));
    overridePCLStub("ModuleDefinition/IsModuleDefinitionContextActive", reinterpret_cast<void*>(mock_IsModuleDefinitionContextActive));
    overridePCLStub("ModuleDefinition/ExitModuleDefinitionContext", reinterpret_cast<void*>(mock_ExitModuleDefinitionContext));
    overridePCLStub("ModuleDefinition/SetModuleOnLoadRoutine", reinterpret_cast<void*>(mock_SetModuleOnLoadRoutine));
    overridePCLStub("ModuleDefinition/SetModuleOnUnloadRoutine", reinterpret_cast<void*>(mock_SetModuleOnUnloadRoutine));

    overridePCLStub("ProcessDefinition/EnterProcessDefinitionContext", reinterpret_cast<void*>(mock_EnterProcessDefinitionContext));
    overridePCLStub("ProcessDefinition/IsProcessDefinitionContextActive", reinterpret_cast<void*>(mock_IsProcessDefinitionContextActive));
    overridePCLStub("ProcessDefinition/BeginProcessDefinition", reinterpret_cast<void*>(mock_BeginProcessDefinition));
    overridePCLStub("ProcessDefinition/SetProcessCategory", reinterpret_cast<void*>(mock_SetProcessCategory));
    overridePCLStub("ProcessDefinition/SetProcessCreationRoutine", reinterpret_cast<void*>(mock_SetProcessCreationRoutine));
    overridePCLStub("ProcessDefinition/SetProcessDestructionRoutine", reinterpret_cast<void*>(mock_SetProcessDestructionRoutine));
    overridePCLStub("ProcessDefinition/SetProcessInitializationRoutine", reinterpret_cast<void*>(mock_SetProcessInitializationRoutine));
    overridePCLStub("ProcessDefinition/SetProcessExecutionRoutine", reinterpret_cast<void*>(mock_SetProcessExecutionRoutine));
    overridePCLStub("ProcessDefinition/SetProcessGlobalExecutionRoutine", reinterpret_cast<void*>(mock_SetProcessGlobalExecutionRoutine));
    overridePCLStub("ProcessDefinition/SetProcessDefaultInterfaceSelectionRoutine", reinterpret_cast<void*>(mock_SetProcessDefaultInterfaceSelectionRoutine));
    overridePCLStub("ProcessDefinition/SetProcessInterfaceSelectionRoutine", reinterpret_cast<void*>(mock_SetProcessInterfaceSelectionRoutine));
    overridePCLStub("ProcessDefinition/EndProcessDefinition", reinterpret_cast<void*>(mock_EndProcessDefinition));
    overridePCLStub("ProcessDefinition/ExitProcessDefinitionContext", reinterpret_cast<void*>(mock_ExitProcessDefinitionContext));

    overridePCLStub("InterfaceDefinition/BeginInterfaceDefinition", reinterpret_cast<void*>(mock_BeginInterfaceDefinition));
    overridePCLStub("InterfaceDefinition/SetInterfaceInitializationRoutine", reinterpret_cast<void*>(mock_SetInterfaceInitializationRoutine));
    overridePCLStub("InterfaceDefinition/SetInterfaceLaunchRoutine", reinterpret_cast<void*>(mock_SetInterfaceLaunchRoutine));
    overridePCLStub("InterfaceDefinition/EndInterfaceDefinition", reinterpret_cast<void*>(mock_EndInterfaceDefinition));

    overridePCLStub("Control/CreateControl", reinterpret_cast<void*>(mock_CreateControl));
    overridePCLStub("Control/SetControlParent", reinterpret_cast<void*>(mock_SetControlParent));
    overridePCLStub("Control/SetControlSizer", reinterpret_cast<void*>(mock_SetControlSizer));
    overridePCLStub("Control/SetControlMinSize", reinterpret_cast<void*>(mock_SetControlMinSize));
    overridePCLStub("Control/SetControlMaxSize", reinterpret_cast<void*>(mock_SetControlMaxSize));
    overridePCLStub("Control/AdjustControlToContents", reinterpret_cast<void*>(mock_AdjustControlToContents));
    overridePCLStub("Control/GetClientRect", reinterpret_cast<void*>(mock_GetClientRect));
    overridePCLStub("Control/SetControlFixedSize", reinterpret_cast<void*>(mock_SetControlFixedSize));
    overridePCLStub("Control/GetControlDisplayPixelRatio", reinterpret_cast<void*>(mock_GetControlDisplayPixelRatio));
    overridePCLStub("Control/SetControlBackgroundColor", reinterpret_cast<void*>(mock_SetControlBackgroundColor));
    overridePCLStub("Control/GetControlFont", reinterpret_cast<void*>(mock_GetControlFont));
    overridePCLStub("Control/SetWindowTitle", reinterpret_cast<void*>(mock_SetWindowTitle));
    overridePCLStub("Control/SetWindowToolTip", reinterpret_cast<void*>(mock_SetWindowToolTip));
    overridePCLStub("Control/GetControlPosition", reinterpret_cast<void*>(mock_GetControlPosition));
    overridePCLStub("Control/SetControlPosition", reinterpret_cast<void*>(mock_SetControlPosition));
    overridePCLStub("Control/SetControlVisible", reinterpret_cast<void*>(mock_SetControlVisible));
    overridePCLStub("Control/SetControlEnabled", reinterpret_cast<void*>(mock_SetControlEnabled));
    overridePCLStub("Control/EnsureControlLayoutUpdated", reinterpret_cast<void*>(mock_EnsureControlLayoutUpdated));
    overridePCLStub("Font/GetStringPixelWidth", reinterpret_cast<void*>(mock_GetStringPixelWidth));
    overridePCLStub("Control/SetChildControlToFocus", reinterpret_cast<void*>(mock_SetChildControlToFocus));
    overridePCLStub("Control/SetChildCreateEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetChildDestroyEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetCloseEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetDestroyEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetEnterEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetFileDragEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetFileDropEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetGetFocusEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetHideEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetKeyPressEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetKeyReleaseEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetLeaveEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetLoseFocusEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetMouseDoubleClickEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetMouseMoveEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetMousePressEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetMouseReleaseEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetMoveEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetPaintEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetResizeEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetShowEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetViewDragEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetViewDropEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Control/SetWheelEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));

    overridePCLStub("Dialog/CreateDialog", reinterpret_cast<void*>(mock_CreateDialog));

    overridePCLStub("Sizer/CreateSizer", reinterpret_cast<void*>(mock_CreateSizer));
    overridePCLStub("Sizer/InsertSizerControl", reinterpret_cast<void*>(mock_InsertSizerControl));
    overridePCLStub("Sizer/InsertSizer", reinterpret_cast<void*>(mock_InsertSizer));
    overridePCLStub("Sizer/InsertSizerSpacing", reinterpret_cast<void*>(mock_InsertSizerSpacing));
    overridePCLStub("Sizer/InsertSizerStretch", reinterpret_cast<void*>(mock_InsertSizerStretch));
    overridePCLStub("Sizer/SetSizerMargin", reinterpret_cast<void*>(mock_SetSizerMargin));
    overridePCLStub("Sizer/SetSizerSpacing", reinterpret_cast<void*>(mock_SetSizerSpacing));
    overridePCLStub("Sizer/GetSizerDisplayPixelRatio", reinterpret_cast<void*>(mock_GetSizerDisplayPixelRatio));

    overridePCLStub("GroupBox/CreateGroupBox", reinterpret_cast<void*>(mock_CreateGroupBox));
    overridePCLStub("GroupBox/SetGroupBoxCheckEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));

    overridePCLStub("Button/CreatePushButton", reinterpret_cast<void*>(mock_CreatePushButton));
    overridePCLStub("Button/CreateCheckBox", reinterpret_cast<void*>(mock_CreateCheckBox));
    overridePCLStub("Button/SetButtonText", reinterpret_cast<void*>(mock_SetButtonText));
    overridePCLStub("Button/SetButtonCheckEventRoutine", reinterpret_cast<void*>(mock_SetButtonCheckEventRoutine));
    overridePCLStub("Button/SetButtonChecked", reinterpret_cast<void*>(mock_SetButtonChecked));
    overridePCLStub("Button/GetButtonChecked", reinterpret_cast<void*>(mock_GetButtonChecked));
    overridePCLStub("Button/SetButtonClickEventRoutine", reinterpret_cast<void*>(mock_SetButtonClickEventRoutine));
    overridePCLStub("Button/SetButtonPressEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Button/SetButtonReleaseEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));

    overridePCLStub("ComboBox/CreateComboBox", reinterpret_cast<void*>(mock_CreateComboBox));
    overridePCLStub("ComboBox/InsertComboBoxItem", reinterpret_cast<void*>(mock_InsertComboBoxItem));
    overridePCLStub("ComboBox/ClearComboBox", reinterpret_cast<void*>(mock_ClearComboBox));
    overridePCLStub("ComboBox/SetComboBoxCurrentItem", reinterpret_cast<void*>(mock_SetComboBoxCurrentItem));
    overridePCLStub("ComboBox/GetComboBoxCurrentItem", reinterpret_cast<void*>(mock_GetComboBoxCurrentItem));
    overridePCLStub("ComboBox/SetComboBoxEditTextUpdatedEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("ComboBox/SetComboBoxItemHighlightedEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("ComboBox/SetComboBoxItemSelectedEventRoutine", reinterpret_cast<void*>(mock_SetComboBoxItemSelectedEventRoutine));
    overridePCLStub("ComboBox/GetComboBoxLength", reinterpret_cast<void*>(mock_GetComboBoxLength));

    overridePCLStub("SpinBox/CreateSpinBox", reinterpret_cast<void*>(mock_CreateSpinBox));
    overridePCLStub("SpinBox/SetSpinBoxRange", reinterpret_cast<void*>(mock_SetSpinBoxRange));
    overridePCLStub("SpinBox/SetSpinBoxValue", reinterpret_cast<void*>(mock_SetSpinBoxValue));
    overridePCLStub("SpinBox/GetSpinBoxValue", reinterpret_cast<void*>(mock_GetSpinBoxValue));

    overridePCLStub("Label/CreateLabel", reinterpret_cast<void*>(mock_CreateLabel));
    overridePCLStub("Label/SetLabelText", reinterpret_cast<void*>(mock_SetLabelText));
    overridePCLStub("Label/SetLabelAlignment", reinterpret_cast<void*>(mock_SetLabelAlignment));

    overridePCLStub("Slider/CreateSlider", reinterpret_cast<void*>(mock_CreateSlider));
    overridePCLStub("Slider/GetSliderValue", reinterpret_cast<void*>(mock_GetSliderValue));
    overridePCLStub("Slider/SetSliderValue", reinterpret_cast<void*>(mock_SetSliderValue));
    overridePCLStub("Slider/GetSliderRange", reinterpret_cast<void*>(mock_GetSliderRange));
    overridePCLStub("Slider/SetSliderRange", reinterpret_cast<void*>(mock_SetSliderRange));
    overridePCLStub("Slider/GetSliderStepSize", reinterpret_cast<void*>(mock_GetSliderStepSize));
    overridePCLStub("Slider/SetSliderStepSize", reinterpret_cast<void*>(mock_SetSliderStepSize));
    overridePCLStub("Slider/GetSliderPageSize", reinterpret_cast<void*>(mock_GetSliderPageSize));
    overridePCLStub("Slider/SetSliderPageSize", reinterpret_cast<void*>(mock_SetSliderPageSize));
    overridePCLStub("Slider/GetSliderTickInterval", reinterpret_cast<void*>(mock_GetSliderTickInterval));
    overridePCLStub("Slider/SetSliderTickInterval", reinterpret_cast<void*>(mock_SetSliderTickInterval));
    overridePCLStub("Slider/GetSliderTickStyle", reinterpret_cast<void*>(mock_GetSliderTickStyle));
    overridePCLStub("Slider/SetSliderTickStyle", reinterpret_cast<void*>(mock_SetSliderTickStyle));
    overridePCLStub("Slider/GetSliderTrackingEnabled", reinterpret_cast<void*>(mock_GetSliderTrackingEnabled));
    overridePCLStub("Slider/SetSliderTrackingEnabled", reinterpret_cast<void*>(mock_SetSliderTrackingEnabled));
    overridePCLStub("Slider/SetSliderValueUpdatedEventRoutine", reinterpret_cast<void*>(mock_SetSliderValueUpdatedEventRoutine));
    overridePCLStub("Slider/SetSliderRangeUpdatedEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));

    overridePCLStub("Edit/CreateEdit", reinterpret_cast<void*>(mock_CreateEdit));
    overridePCLStub("Edit/GetEditText", reinterpret_cast<void*>(mock_GetEditText));
    overridePCLStub("Edit/SetEditText", reinterpret_cast<void*>(mock_SetEditText));
    overridePCLStub("Edit/GetEditReadOnly", reinterpret_cast<void*>(mock_GetEditReadOnly));
    overridePCLStub("Edit/SetEditReadOnly", reinterpret_cast<void*>(mock_SetEditReadOnly));
    overridePCLStub("Edit/SetCaretPositionUpdatedEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Edit/SetEditCompletedEventRoutine", reinterpret_cast<void*>(mock_SetEditCompletedEventRoutine));
    overridePCLStub("Edit/SetReturnPressedEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Edit/SetSelectionUpdatedEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Edit/SetTextUpdatedEventRoutine", reinterpret_cast<void*>(mock_SetTextUpdatedEventRoutine));
    overridePCLStub("Edit/SetEditValidatingRegExp", reinterpret_cast<void*>(mock_SetEditValidatingRegExp));

    overridePCLStub("Dialog/SetExecuteDialogEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));
    overridePCLStub("Dialog/SetReturnDialogEventRoutine", reinterpret_cast<void*>(mock_SetEventRoutine));

    overridePCLStub("UI/AttachToUIObject", reinterpret_cast<void*>(mock_AttachToUIObject));
    overridePCLStub("UI/DetachFromUIObject", reinterpret_cast<void*>(mock_DetachFromUIObject));
    overridePCLStub("UI/GetUIObjectRefCount", reinterpret_cast<void*>(mock_GetUIObjectRefCount));
    overridePCLStub("Thread/CreateThread", reinterpret_cast<void*>(mock_CreateThread));
    overridePCLStub("Thread/StartThread", reinterpret_cast<void*>(mock_StartThread));
    overridePCLStub("Thread/KillThread", reinterpret_cast<void*>(mock_KillThread));
    overridePCLStub("Thread/IsThreadActive", reinterpret_cast<void*>(mock_IsThreadActive));
    overridePCLStub("Thread/WaitThread", reinterpret_cast<void*>(mock_WaitThread));
    overridePCLStub("Thread/SleepThread", reinterpret_cast<void*>(mock_SleepThread));
    overridePCLStub("Thread/GetThreadStatus", reinterpret_cast<void*>(mock_GetThreadStatus));
    overridePCLStub("Thread/SetThreadStatus", reinterpret_cast<void*>(mock_SetThreadStatus));
    overridePCLStub("Thread/GetCurrentThread", reinterpret_cast<void*>(mock_GetCurrentThread));
    overridePCLStub("Thread/SetThreadExecRoutine", reinterpret_cast<void*>(mock_SetThreadExecRoutine));
    overridePCLStub("Thread/AppendThreadConsoleOutputText", reinterpret_cast<void*>(mock_AppendThreadConsoleOutputText));

    overridePCLStub("SharedImage/GetImageGeometry", reinterpret_cast<void*>(mock_GetImageGeometry));
    overridePCLStub("SharedImage/GetImageFormat", reinterpret_cast<void*>(mock_GetImageFormat));
    overridePCLStub("SharedImage/GetImagePixelData", reinterpret_cast<void*>(mock_GetImagePixelData));
    overridePCLStub("SharedImage/SetImagePixelData", reinterpret_cast<void*>(mock_SetImagePixelData));
    overridePCLStub("SharedImage/SetImageGeometry", reinterpret_cast<void*>(mock_SetImageGeometry));
    overridePCLStub("SharedImage/SetImageColorSpace", reinterpret_cast<void*>(mock_SetImageColorSpace));
    overridePCLStub("SharedImage/GetImageColorSpace", reinterpret_cast<void*>(mock_GetImageColorSpace));
    overridePCLStub("SharedImage/AttachToImage", reinterpret_cast<void*>(mock_AttachToImage));
    overridePCLStub("SharedImage/GetImageRGBWS", reinterpret_cast<void*>(mock_GetImageRGBWS));
    overridePCLStub("SharedImage/DetachFromImage", reinterpret_cast<void*>(mock_DetachFromImage));

    overridePCLStub("View/GetViewImage", reinterpret_cast<void*>(mock_GetViewImage));
    overridePCLStub("View/GetViewId", reinterpret_cast<void*>(mock_GetViewId));
    overridePCLStub("View/GetViewFullId", reinterpret_cast<void*>(mock_GetViewId));
    overridePCLStub("View/GetViewLocks", reinterpret_cast<void*>(mock_GetViewLocks));
    overridePCLStub("View/LockView", reinterpret_cast<void*>(mock_LockView));
    overridePCLStub("View/UnlockView", reinterpret_cast<void*>(mock_UnlockView));
    overridePCLStub("View/GetViewScreenTransferFunctions", reinterpret_cast<void*>(mock_GetViewScreenTransferFunctions));
    overridePCLStub("View/GetViewScreenTransferFunctionsEnabled", reinterpret_cast<void*>(mock_GetViewScreenTransferFunctionsEnabled));
    overridePCLStub("View/GetViewPropertyExists", reinterpret_cast<void*>(mock_GetViewPropertyExists));
    overridePCLStub("View/ComputeViewProperty", reinterpret_cast<void*>(mock_ComputeViewProperty));
    overridePCLStub("View/GetViewPropertyValue", reinterpret_cast<void*>(mock_GetViewPropertyValue));
    overridePCLStub("View/SetViewPropertyValue", reinterpret_cast<void*>(mock_SetViewPropertyValue));

    // Global Settings & Preferences
    overridePCLStub("Global/ReadSettingsString", reinterpret_cast<void*>(mock_ReadSettingsString));
    overridePCLStub("Global/WriteSettingsString", reinterpret_cast<void*>(mock_WriteSettingsString));
    overridePCLStub("Global/ReadSettingsFlag", reinterpret_cast<void*>(mock_ReadSettingsFlag));
    overridePCLStub("Global/WriteSettingsFlag", reinterpret_cast<void*>(mock_WriteSettingsFlag));
    overridePCLStub("Global/ReadSettingsReal", reinterpret_cast<void*>(mock_ReadSettingsReal));
    overridePCLStub("Global/WriteSettingsReal", reinterpret_cast<void*>(mock_WriteSettingsReal));
    overridePCLStub("Global/ReadSettingsInteger", reinterpret_cast<void*>(mock_ReadSettingsInteger));
    overridePCLStub("Global/WriteSettingsInteger", reinterpret_cast<void*>(mock_WriteSettingsInteger));
    overridePCLStub("Global/GetGlobalString", reinterpret_cast<void*>(mock_GetGlobalString));
    overridePCLStub("Global/GetGlobalInteger", reinterpret_cast<void*>(mock_GetGlobalInteger));

    // Process & Parameter definitions
    overridePCLStub("ProcessDefinition/BeginParameterDefinition", reinterpret_cast<void*>(mock_BeginParameterDefinition));
    overridePCLStub("ProcessDefinition/SetParameterProcessVersionRange", reinterpret_cast<void*>(mock_SetParameterProcessVersionRange));
    overridePCLStub("ProcessDefinition/SetParameterRequired", reinterpret_cast<void*>(mock_SetParameterRequired));
    overridePCLStub("ProcessDefinition/SetParameterReadOnly", reinterpret_cast<void*>(mock_SetParameterReadOnly));
    overridePCLStub("ProcessDefinition/SetParameterLockRoutine", reinterpret_cast<void*>(mock_SetParameterLockRoutine));
    overridePCLStub("ProcessDefinition/SetParameterAllocationRoutine", reinterpret_cast<void*>(mock_SetParameterAllocationRoutine));
    overridePCLStub("ProcessDefinition/SetParameterLengthQueryRoutine", reinterpret_cast<void*>(mock_SetParameterLengthQueryRoutine));
    overridePCLStub("ProcessDefinition/SetDefaultBooleanValue", reinterpret_cast<void*>(mock_SetDefaultBooleanValue));
    overridePCLStub("ProcessDefinition/SetDefaultNumericValue", reinterpret_cast<void*>(mock_SetDefaultNumericValue));
    overridePCLStub("ProcessDefinition/SetValidNumericRange", reinterpret_cast<void*>(mock_SetValidNumericRange));
    overridePCLStub("ProcessDefinition/SetPrecision", reinterpret_cast<void*>(mock_SetPrecision));
    overridePCLStub("ProcessDefinition/SetScientificNotation", reinterpret_cast<void*>(mock_SetScientificNotation));
    overridePCLStub("ProcessDefinition/EndParameterDefinition", reinterpret_cast<void*>(mock_EndParameterDefinition));
}

bool PCLBridge::loadModule(const QString& path) {
    unloadModule();

    // Dynamically pre-load tensorflow dependencies if we are loading StarXTerminator.
    // This keeps the core BLastro binary entirely decoupled from TensorFlow while resolving
    // the plugin's runtime shared library dependencies.
    if (path.contains("StarXTerminator", Qt::CaseInsensitive)) {
        qDebug() << "[PCL Bridge] Detected StarXTerminator. Pre-loading TensorFlow dependencies...";
        
        // Find relative lib directory based on the application path
        QString appDir = QCoreApplication::applicationDirPath();
        QString tfFrameworkPath = appDir + "/lib/libtensorflow_framework.so.2";
        QString tfPath = appDir + "/lib/libtensorflow.so.2";

        // Fallback to development/build directory structures if run from build/
        if (!QFile::exists(tfPath)) {
            tfFrameworkPath = appDir + "/../lib/libtensorflow_framework.so.2";
            tfPath = appDir + "/../lib/libtensorflow.so.2";
        }

        qDebug() << "[PCL Bridge] Loading TensorFlow framework from:" << tfFrameworkPath;
        void* tfFrameHandle = dlopen(tfFrameworkPath.toUtf8().constData(), RTLD_LAZY | RTLD_GLOBAL);
        if (!tfFrameHandle) {
            qWarning() << "[PCL Bridge] Failed to pre-load libtensorflow_framework.so.2:" << dlerror();
        }

        qDebug() << "[PCL Bridge] Loading TensorFlow from:" << tfPath;
        void* tfHandle = dlopen(tfPath.toUtf8().constData(), RTLD_LAZY | RTLD_GLOBAL);
        if (!tfHandle) {
            qWarning() << "[PCL Bridge] Failed to pre-load libtensorflow.so.2:" << dlerror();
        }
    }

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

    // 2. Initialize the module now that the meta-objects are instantiated so it resolves stubs to our overrides!
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

    // Call the module's OnLoad routine if registered!
    if (g_moduleOnLoad) {
        qDebug() << "[PCL Bridge] Invoking module OnLoad routine...";
        g_moduleOnLoad();
        qDebug() << "[PCL Bridge] Module OnLoad routine finished.";
    }

    return true;
}

void PCLBridge::unloadModule() {
    if (m_libHandle) {
        // Call the module's OnUnload routine if registered!
        if (g_moduleOnUnload) {
            qDebug() << "[PCL Bridge] Invoking module OnUnload routine...";
            g_moduleOnUnload();
            g_moduleOnUnload = nullptr;
        }
        g_moduleOnLoad = nullptr;

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
        g_interfaceControls.clear();
        for (auto& pair : g_threads) {
            delete pair.second;
        }
        g_threads.clear();
        g_currentThread = nullptr;
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
    g_activeImage = &imgMock;

    PCLViewMock viewMock;
    viewMock.id = "BLastroActiveView";
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
    g_activeImage = nullptr;

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

bool PCLBridge::executeProcessInstance(const QString& processId, void* hProcess, std::vector<ImageBufferPtr>& buffers) {
    if (!m_initialized) {
        qWarning() << "[PCL Bridge] Cannot execute process instance, no module loaded/initialized.";
        return false;
    }

    if (!hProcess) {
        qWarning() << "[PCL Bridge] Cannot execute process instance, null process handle.";
        return false;
    }

    if (buffers.empty()) {
        qWarning() << "[PCL Bridge] Cannot execute process instance, no image buffers provided.";
        return false;
    }

    auto idIt = g_processIdToHandle.find(processId.toStdString());
    if (idIt == g_processIdToHandle.end()) {
        qWarning() << "[PCL Bridge] Process ID not found:" << processId;
        return false;
    }

    meta_process_handle hMeta = idIt->second;
    const auto& info = g_processes[hMeta];

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
    g_activeImage = &imgMock;

    PCLViewMock viewMock;
    viewMock.id = "BLastroActiveView";
    viewMock.hImage = &imgMock;

    qDebug() << "[PCL Bridge] Executing process instance on image:" << imgMock.width << "x" << imgMock.height << "with" << imgMock.numChannels << "channels";
    bool success = false;
    if (info.executeFn) {
        success = info.executeFn(&viewMock, hProcess);
    } else if (info.executeGlobalFn) {
        qWarning() << "[PCL Bridge] Process only supports global execution, executing globally...";
        success = info.executeGlobalFn(hProcess);
    } else {
        qWarning() << "[PCL Bridge] Process has no execution routine!";
    }

    qDebug() << "[PCL Bridge] Process instance execution finished, status:" << (success ? "Success" : "Failed");
    g_activeImage = nullptr;
    return success;
}

bool PCLBridge::launchInterface(const QString& processId, QWidget* parentWindow) {
    if (!m_initialized) {
        qWarning() << "[PCL Bridge] Cannot launch interface, no module loaded/initialized.";
        return false;
    }

    std::string idStr = processId.toStdString();
    auto idIt = g_interfaceIdToHandle.find(idStr);
    if (idIt == g_interfaceIdToHandle.end()) {
        qWarning() << "[PCL Bridge] Interface ID not found:" << processId;
        return false;
    }

    meta_interface_handle hMeta = idIt->second;
    const auto& info = g_interfaces[hMeta];

    if (!info.initFn) {
        qWarning() << "[PCL Bridge] Interface has no initialization routine:" << processId;
        return false;
    }

    qDebug() << "[PCL Bridge] Launching interface for:" << processId;

    // Create the host widget (main MDI window content)
    QWidget* hostWidget = new QWidget();
    hostWidget->setWindowTitle(processId + " Process Interface");
    hostWidget->resize(600, 450);

    // Create a container widget specifically for the plugin's controls
    QWidget* pluginContainer = new QWidget(hostWidget);

    // Create the Apply button
    QPushButton* applyButton = new QPushButton("Apply to Active Image", hostWidget);
    applyButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #007acc;"
        "   border: 1px solid #005999;"
        "   border-radius: 4px;"
        "   color: #ffffff;"
        "   font-weight: bold;"
        "   padding: 8px 16px;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #008be5;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #005999;"
        "}"
    );

    // Create a vertical layout for the host widget
    QVBoxLayout* mainLayout = new QVBoxLayout(hostWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    // Add the plugin container and the apply button to the main layout
    mainLayout->addWidget(pluginContainer, 1); // Give plugin container stretch factor 1
    
    // Add a thin horizontal separator line
    QFrame* separator = new QFrame(hostWidget);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("background-color: #333; max-height: 1px;");
    mainLayout->addWidget(separator);
    
    mainLayout->addWidget(applyButton, 0, Qt::AlignRight); // Align button to the right

    // Apply dark theme stylesheet to match application dark mode
    hostWidget->setStyleSheet(
        "QWidget { background-color: #121212; color: #ffffff; }"
        "QPushButton { background-color: #2a2a2a; border: 1px solid #444; border-radius: 4px; padding: 6px 12px; color: #ffffff; }"
        "QPushButton:hover { background-color: #3a3a3a; }"
        "QGroupBox { border: 1px solid #444; border-radius: 6px; margin-top: 12px; padding: 12px; font-weight: bold; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 8px; padding: 0 3px; }"
        "QLabel { color: #dddddd; }"
        "QComboBox { background-color: #2a2a2a; border: 1px solid #444; border-radius: 4px; padding: 4px; color: #ffffff; }"
        "QSpinBox { background-color: #2a2a2a; border: 1px solid #444; border-radius: 4px; padding: 4px; color: #ffffff; }"
        "QSlider::groove:horizontal { border: 1px solid #444; height: 6px; background: #2a2a2a; border-radius: 3px; }"
        "QSlider::handle:horizontal { background: #3a8ee6; width: 14px; margin: -4px 0; border-radius: 7px; }"
    );

    // 1. Call the interface's initialization callback!
    // The C-API initialization routine expects (interface_handle, control_handle)
    interface_handle hInterface = const_cast<void*>(hMeta);
    control_handle hParentControl = reinterpret_cast<control_handle>(pluginContainer);

    qDebug() << "[PCL Bridge] Calling interface initialization callback (initFn)...";
    info.initFn(hInterface, hParentControl);
    qDebug() << "[PCL Bridge] Interface initialization callback completed.";

    // Instantiate process handle
    auto procIdIt = g_processIdToHandle.find(idStr);
    meta_process_handle hMetaProcess = nullptr;
    process_handle hProcess = nullptr;
    if (procIdIt != g_processIdToHandle.end()) {
        hMetaProcess = procIdIt->second;
        const auto& procInfo = g_processes[hMetaProcess];
        if (procInfo.createFn) {
            hProcess = procInfo.createFn(hMetaProcess);
            if (procInfo.initFn) {
                procInfo.initFn(hProcess);
            }
        }
    }

    // 2. Call the interface's launch routine (launchFn) if present!
    if (info.launchFn) {
        qDebug() << "[PCL Bridge] Calling interface launch routine (launchFn)...";
        api_bool dynamic = false;
        uint32 flags = 0;
        bool launchOk = info.launchFn(hInterface, hMetaProcess, hProcess, &dynamic, &flags);
        qDebug() << "[PCL Bridge] Interface launch routine returned:" << launchOk << ", dynamic:" << (bool)dynamic;
    }

    // Connect the Apply button to execute the process on the active image
    QObject::connect(applyButton, &QPushButton::clicked, [this, processId, hProcess, parentWindow]() {
        MainWindow* mainWin = qobject_cast<MainWindow*>(parentWindow);
        if (mainWin) {
            mainWin->executePCLProcessOnActiveImage(processId, hProcess);
        } else {
            qWarning() << "[PCL Bridge] Cannot apply process: parent window is not MainWindow.";
        }
    });

    // Connect host widget destruction to process cleanup
    auto procIdItDest = g_processIdToHandle.find(idStr);
    meta_process_handle hMetaDest = (procIdItDest != g_processIdToHandle.end()) ? procIdItDest->second : nullptr;
    auto destInfo = (hMetaDest != nullptr) ? g_processes[hMetaDest] : PCLProcessInfo();

    QObject::connect(hostWidget, &QObject::destroyed, [destInfo, hProcess]() {
        qDebug() << "[PCL Bridge] Host widget destroyed. Cleaning up process instance...";
        if (hProcess && destInfo.destroyFn) {
            destInfo.destroyFn(hProcess);
        }
        g_processParameters.erase(hProcess);
    });

    MainWindow* mainWin = qobject_cast<MainWindow*>(parentWindow);
    if (mainWin) {
        mainWin->createPluginSubWindow(hostWidget, processId + " Process Interface");
    } else {
        hostWidget->show();
    }

    return true;
}

#include <QSettings>

static ::QSettings& getBLastroSettings() {
    static ::QSettings settings("BLastro", "BLastro");
    return settings;
}

static QString getSettingsKey(const char16_type* keyPath) {
    if (!keyPath) return QString();
    return QString::fromUtf16(reinterpret_cast<const char16_t*>(keyPath));
}

static api_bool mock_ReadSettingsString(const char16_type* keyPath, char16_type* value, pcl::size_type maxLen) {
    QString key = getSettingsKey(keyPath);
    QVariant val = getBLastroSettings().value(key);
    qDebug() << "[PCL Settings] ReadSettingsString:" << key << "->" << val;
    if (val.isValid() && value && maxLen > 0) {
        QString str = val.toString();
        int len = qMin(static_cast<int>(maxLen) - 1, str.length());
        std::memcpy(value, str.utf16(), len * sizeof(char16_type));
        value[len] = 0;
        return true;
    }
    return false;
}

static api_bool mock_WriteSettingsString(const char16_type* keyPath, const char16_type* value) {
    QString key = getSettingsKey(keyPath);
    QString val = value ? QString::fromUtf16(reinterpret_cast<const char16_t*>(value)) : QString();
    qDebug() << "[PCL Settings] WriteSettingsString:" << key << "<-" << val;
    getBLastroSettings().setValue(key, val);
    return true;
}

static api_bool mock_ReadSettingsFlag(const char16_type* keyPath, api_bool* value) {
    QString key = getSettingsKey(keyPath);
    QVariant val = getBLastroSettings().value(key);
    qDebug() << "[PCL Settings] ReadSettingsFlag:" << key << "->" << val;
    if (val.isValid() && value) {
        *value = val.toBool();
        return true;
    }
    return false;
}

static api_bool mock_WriteSettingsFlag(const char16_type* keyPath, api_bool value) {
    QString key = getSettingsKey(keyPath);
    qDebug() << "[PCL Settings] WriteSettingsFlag:" << key << "<-" << (bool)value;
    getBLastroSettings().setValue(key, (bool)value);
    return true;
}

static api_bool mock_ReadSettingsReal(const char16_type* keyPath, double* value) {
    QString key = getSettingsKey(keyPath);
    QVariant val = getBLastroSettings().value(key);
    qDebug() << "[PCL Settings] ReadSettingsReal:" << key << "->" << val;
    if (val.isValid() && value) {
        *value = val.toDouble();
        return true;
    }
    return false;
}

static api_bool mock_WriteSettingsReal(const char16_type* keyPath, double value) {
    QString key = getSettingsKey(keyPath);
    qDebug() << "[PCL Settings] WriteSettingsReal:" << key << "<-" << value;
    getBLastroSettings().setValue(key, value);
    return true;
}

static api_bool mock_ReadSettingsInteger(const char16_type* keyPath, int32* value) {
    QString key = getSettingsKey(keyPath);
    QVariant val = getBLastroSettings().value(key);
    qDebug() << "[PCL Settings] ReadSettingsInteger:" << key << "->" << val;
    if (val.isValid() && value) {
        *value = val.toInt();
        return true;
    }
    return false;
}

static api_bool mock_WriteSettingsInteger(const char16_type* keyPath, int32 value) {
    QString key = getSettingsKey(keyPath);
    qDebug() << "[PCL Settings] WriteSettingsInteger:" << key << "<-" << value;
    getBLastroSettings().setValue(key, value);
    return true;
}

static api_bool mock_GetGlobalString(const char16_type* globalKey, char16_type* value, pcl::size_type maxLen) {
    QString key = getSettingsKey(globalKey);
    qDebug() << "[PCL Global] GetGlobalString:" << key;
    // Provide sensible default strings if requested
    QString result = "";
    if (key.contains("License", Qt::CaseInsensitive)) {
        result = "BLastro-Community-License";
    }
    if (!result.isEmpty() && value && maxLen > 0) {
        int len = qMin(static_cast<int>(maxLen) - 1, result.length());
        std::memcpy(value, result.utf16(), len * sizeof(char16_type));
        value[len] = 0;
        return true;
    }
    return false;
}

static int32 mock_GetGlobalInteger(const char16_type* globalKey) {
    QString key = getSettingsKey(globalKey);
    qDebug() << "[PCL Global] GetGlobalInteger:" << key;
    return 0;
}

// ============================================================================
// Process / Parameter Metadata Definition Mocks
// ============================================================================
static void mock_BeginParameterDefinition(meta_parameter_handle hParam, const char* paramId, uint32 type) {
    qDebug() << "[PCL ProcessDef] BeginParameterDefinition:" << paramId << "type =" << type;
}

static void mock_SetParameterProcessVersionRange(uint32 minVer, uint32 maxVer) {
    qDebug() << "[PCL ProcessDef] SetParameterProcessVersionRange:" << minVer << "to" << maxVer;
}

static void mock_SetParameterRequired(api_bool required) {
    qDebug() << "[PCL ProcessDef] SetParameterRequired:" << (bool)required;
}

static void mock_SetParameterReadOnly(api_bool readOnly) {
    qDebug() << "[PCL ProcessDef] SetParameterReadOnly:" << (bool)readOnly;
}

static void mock_SetParameterLockRoutine(void* routine) {
    qDebug() << "[PCL ProcessDef] SetParameterLockRoutine:" << routine;
}

static void mock_SetParameterAllocationRoutine(void* routine) {
    qDebug() << "[PCL ProcessDef] SetParameterAllocationRoutine:" << routine;
}

static void mock_SetParameterLengthQueryRoutine(void* routine) {
    qDebug() << "[PCL ProcessDef] SetParameterLengthQueryRoutine:" << routine;
}

static void mock_SetDefaultBooleanValue(api_bool value) {
    qDebug() << "[PCL ProcessDef] SetDefaultBooleanValue:" << (bool)value;
}

static void mock_SetDefaultNumericValue(double value) {
    qDebug() << "[PCL ProcessDef] SetDefaultNumericValue:" << value;
}

static void mock_SetValidNumericRange(double minVal, double maxVal) {
    qDebug() << "[PCL ProcessDef] SetValidNumericRange:" << minVal << "to" << maxVal;
}

static void mock_SetPrecision(int32 precision) {
    qDebug() << "[PCL ProcessDef] SetPrecision:" << precision;
}

static void mock_SetScientificNotation(api_bool scientific) {
    qDebug() << "[PCL ProcessDef] SetScientificNotation:" << (bool)scientific;
}

static void mock_EndParameterDefinition() {
    qDebug() << "[PCL ProcessDef] EndParameterDefinition";
}

} // namespace blastro
