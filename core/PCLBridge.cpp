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

#include "core/PCLBridge.h"
#include "core/Logger.h"
#include "core/PCLStubs.h"
#include "core/Preferences.h"
#include "ui/MainWindow.h"
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QFontMetrics>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProcess>
#include <QProgressDialog>
#include <QPushButton>
#include <QRegularExpression>
#include <QSettings>
#include <QSlider>
#include <QSpinBox>
#include <QThread>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QVariant>
#include <QWidget>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <dlfcn.h>
#include <iostream>
#include <mutex>
#include <optional>
#include <pcl/api/APIInterface.h>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace blastro {

static bool g_verbosePCL = false;

class PCLBridgeLogger {
public:
  PCLBridgeLogger(Logger::Level level, bool enabled = true)
      : m_level(level), m_enabled(enabled), m_dbg(&m_buffer) {
    m_dbg.noquote();
  }

  PCLBridgeLogger &operator()() { return *this; }

  template <typename T> PCLBridgeLogger &operator<<(const T &t) {
    if (m_enabled) {
      m_dbg << t;
    }
    return *this;
  }

  // Support QDebug manipulators like std::endl or Qt-specific formatting
  // manipulators
  PCLBridgeLogger &operator<<(QDebug &(*f)(QDebug &)) {
    if (m_enabled) {
      m_dbg << f;
    }
    return *this;
  }

  PCLBridgeLogger &noquote() {
    if (m_enabled) {
      m_dbg.noquote();
    }
    return *this;
  }

  ~PCLBridgeLogger() {
    if (m_enabled && !m_buffer.isEmpty()) {
      QString msg = m_buffer.trimmed();
      if (msg.startsWith("[PCL Bridge]")) {
        msg = msg.mid(12).trimmed();
      } else if (msg.startsWith("[PCL Console]")) {
        msg = msg.mid(13).trimmed();
      } else if (msg.startsWith("[PCL Thread Console]")) {
        msg = msg.mid(20).trimmed();
      }
      Logger::log(m_level, "PCL Bridge", msg);
    }
  }

private:
  Logger::Level m_level;
  bool m_enabled;
  QString m_buffer;
  QDebug m_dbg;
};

} // namespace blastro

#undef qDebug
#define qDebug                                                                 \
  ::blastro::PCLBridgeLogger(::blastro::Logger::Level::Info,                   \
                             ::blastro::g_verbosePCL)
#undef qInfo
#define qInfo ::blastro::PCLBridgeLogger(::blastro::Logger::Level::Info)
#undef qWarning
#define qWarning ::blastro::PCLBridgeLogger(::blastro::Logger::Level::Warning)

namespace blastro {

// ----------------------------------------------------------------------------
// Global Registry State
// ----------------------------------------------------------------------------

static std::unordered_map<meta_process_handle, PCLProcessInfo> g_processes;
static std::unordered_map<std::string, meta_process_handle> g_processIdToHandle;
static meta_process_handle g_currentDefiningProcess = nullptr;

// Module lifecycle tracking
static PCLModuleInfo *g_currentLoadingModule = nullptr;

// Interface registry
struct PCLInterfaceInfo {
  QString id;
  meta_interface_handle hMeta = nullptr;
  pcl::interface_initialization_routine initFn = nullptr;
  pcl::interface_launch_routine launchFn = nullptr;
  pcl::interface_control_routine editPreferencesFn = nullptr;
  pcl::interface_process_import_routine importProcessFn = nullptr;
  pcl::global_notification_routine globalPrefUpdatedFn = nullptr;
  pcl::interface_process_validation_routine validateProcessFn = nullptr;
};

static std::unordered_map<meta_interface_handle, PCLInterfaceInfo> g_interfaces;
static std::unordered_map<std::string, meta_interface_handle>
    g_interfaceIdToHandle;
static meta_interface_handle g_currentDefiningInterface = nullptr;

// Map from interface/process ID to the top-level QWidget handle created for it
static std::unordered_map<std::string, control_handle> g_interfaceControls;

// Map from Qt widget pointer (control_handle) to the PCL client object pointer
// (api_handle). The PCL Create* functions pass `client = this` (the PCL
// UIObject pointer). PCL event dispatchers cast hSender back to
// Button*/Slider*/etc. and access m_handlers, so we MUST pass the PCL client
// pointer as hSender, not the Qt widget pointer.
static std::unordered_map<control_handle, api_handle> g_widgetToClient;

struct ParameterKey {
  meta_parameter_handle param;
  pcl::size_type rowIndex;

  bool operator==(const ParameterKey &o) const {
    return param == o.param && rowIndex == o.rowIndex;
  }
};

struct ParameterValue {
  std::vector<uint8_t> data;
  uint32 type = 0;
};

} // namespace blastro

namespace std {
template <> struct hash<blastro::ParameterKey> {
  size_t operator()(const blastro::ParameterKey &k) const {
    return hash<const void *>()(k.param) ^ hash<pcl::size_type>()(k.rowIndex);
  }
};
} // namespace std

namespace blastro {

static std::unordered_map<process_handle,
                          std::unordered_map<ParameterKey, ParameterValue>>
    g_processParameters;
static PCLImageMock *g_activeImage = nullptr;
static PCLBridge *g_pclBridgeInstance = nullptr;
static QString g_activeExecutingProcessId;

struct ProcessExecutionGuard {
  ProcessExecutionGuard(const QString &processId) {
    g_activeExecutingProcessId = processId;
  }
  ~ProcessExecutionGuard() {
    g_activeExecutingProcessId.clear();
  }
};

static std::unordered_set<PCLImageMock *> g_heapImages;
static std::unordered_map<image_handle, PCLImageMock *> g_attachedImages;

// ----------------------------------------------------------------------------
// PixelTraits LUT Mock
// ----------------------------------------------------------------------------
static api_pixtraits_lut s_lut;

// Undefine conflicting PCL macros to prevent clashes in our custom
// implementation
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
  if (initialized)
    return;

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
static api_bool mock_ReadSettingsString(api_handle hModule, char16_type **value,
                                        const char *key, api_bool global);
static api_bool mock_WriteSettingsString(api_handle hModule,
                                         const char16_type *value,
                                         const char *key, api_bool global);
static api_bool mock_ReadSettingsFlag(api_handle hModule, api_bool *value,
                                      const char *key, api_bool global);
static api_bool mock_WriteSettingsFlag(api_handle hModule, api_bool value,
                                       const char *key, api_bool global);
static api_bool mock_ReadSettingsReal(api_handle hModule, double *value,
                                      const char *key, api_bool global);
static api_bool mock_WriteSettingsReal(api_handle hModule, double value,
                                       const char *key, api_bool global);
static api_bool mock_ReadSettingsInteger(api_handle hModule, int32 *value,
                                         const char *key, api_bool global);
static api_bool mock_WriteSettingsInteger(api_handle hModule, int32 value,
                                          const char *key, api_bool global);
static api_bool mock_ReadSettingsUnsignedInteger(api_handle hModule,
                                                 uint32 *value, const char *key,
                                                 api_bool global);
static api_bool mock_WriteSettingsUnsignedInteger(api_handle hModule,
                                                  uint32 value, const char *key,
                                                  api_bool global);
static api_bool mock_GetGlobalString(const char *globalKey, char16_type *value,
                                     pcl::size_type *maxLen);
static api_bool mock_GetGlobalInteger(const char *globalKey, void *value,
                                      api_bool isSigned);
static api_bool mock_GetGlobalFlag(const char *globalKey, api_bool *value);
static api_bool mock_GetGlobalReal(const char *globalKey, double *value);
static uint32 mock_GetProcessStatus();
static void mock_SetRealTimePreviewGenerationStartNotificationRoutine(
    pcl::global_notification_routine f);
static void mock_SetRealTimePreviewGenerationFinishNotificationRoutine(
    pcl::global_notification_routine f);
static void mock_ExitInterfaceDefinitionContext();
static void
mock_SetInterfaceEditPreferencesRoutine(pcl::interface_control_routine f);
static void
mock_SetInterfaceProcessImportRoutine(pcl::interface_process_import_routine f);
static void mock_SetGlobalPreferencesUpdatedNotificationRoutine(
    pcl::global_notification_routine f);
static void mock_SetInterfaceProcessValidationRoutine(
    pcl::interface_process_validation_routine f);
static void mock_SetProcessInterfaceValidationRoutine(
    pcl::process_interface_validation_routine f);
static api_bool mock_GetControlResourcePixelRatio(const_control_handle h,
                                                  double *ratio);
static control_handle mock_CreateViewList(api_handle hModule, api_handle client,
                                          control_handle parent, uint32 flags);
static void mock_RegenerateViewList(control_handle h, api_bool mainViews,
                                    api_bool previews,
                                    api_bool realTimePreview);
static void mock_GetViewListContents(const_control_handle h,
                                     api_bool *mainViews, api_bool *previews,
                                     api_bool *realTimePreview);
static int32 mock_ExecuteDialog(control_handle h);
static api_bool mock_GetEditValid(const_control_handle h);
static void mock_SetButtonDefaultEnabled(control_handle h, api_bool enable);
static void mock_ReturnDialog(control_handle h, int32 result);
static api_bool
mock_ExecuteOpenFileDialog(char16_type *fileName, const char16_type *caption,
                           const char16_type *initialPath,
                           const char16_type *filters,
                           const char16_type *selectedExtension);
static api_bool mock_ExecuteOpenMultipleFilesDialog(
    char16_type *firstFileName, ::file_enumeration_callback callback,
    void *callbackData, const char16_type *caption,
    const char16_type *initialPath, const char16_type *filters,
    const char16_type *selectedExtension);
static api_bool mock_ExecuteSaveFileDialog(char16_type *filePath,
                                           const char16_type *caption,
                                           const char16_type *initialPath,
                                           const char16_type *filters,
                                           const char16_type *selectedExtension,
                                           api_bool overwritePrompt);
static api_bool mock_ExecuteGetDirectoryDialog(char16_type *dirPath,
                                               const char16_type *caption,
                                               const char16_type *initialPath);
static void
mock_SetProcessEditPreferencesRoutine(pcl::process_edit_preferences_routine f);
static api_bool mock_EditProcessPreferences(meta_process_handle hMetaProcess);
static void mock_SetProcessClassInitializationRoutine(
    pcl::process_class_initialization_routine f);
static void mock_SetProcessExecutionPreferencesRoutine(
    pcl::process_execution_preferences_routine f);

// Timer mocks
static timer_handle mock_CreateTimer(api_handle hModule, api_handle client,
                                     uint32 flags);
static void mock_GetTimerInterval(const_timer_handle h, uint32 *msec);
static void mock_SetTimerInterval(timer_handle h, uint32 msec);
static api_bool mock_GetTimerSingleShot(const_timer_handle h);
static void mock_SetTimerSingleShot(timer_handle h, api_bool singleShot);
static api_bool mock_IsTimerActive(const_timer_handle h);
static api_bool mock_StartTimer(timer_handle h);
static void mock_StopTimer(timer_handle h);
static api_bool
mock_SetTimerNotifyEventRoutine(timer_handle h, api_handle receiver,
                                pcl::timer_event_routine routine);

static void mock_BeginParameterDefinition(meta_parameter_handle hParam,
                                          const char *paramId, uint32 type);
static void mock_SetParameterProcessVersionRange(uint32 minVer, uint32 maxVer);

// ToolButton Mocks
static control_handle
mock_CreateToolButton(api_handle hModule, api_handle client,
                      const char16_type *text, const_bitmap_handle bitmap,
                      api_bool checkable, control_handle parent, uint32 flags);
static void mock_SetToolButtonCheckable(control_handle h, api_bool checkable);
static api_bool mock_GetToolButtonCheckable(const_control_handle h);
static void mock_SetParameterRequired(api_bool required);
static void mock_SetParameterReadOnly(api_bool readOnly);
static void mock_SetParameterLockRoutine(void *routine);
static void mock_SetParameterAllocationRoutine(void *routine);
static void mock_SetParameterLengthQueryRoutine(void *routine);
static void mock_SetDefaultBooleanValue(api_bool value);
static void mock_SetDefaultNumericValue(double value);
static void mock_SetValidNumericRange(double minVal, double maxVal);
static void mock_SetPrecision(int32 precision);
static void mock_SetScientificNotation(api_bool scientific);
static void mock_EndParameterDefinition();

static const api_pixtraits_lut *mock_GetPixelTraitsLUT(uint32 version) {
  initPixelTraitsLUT();
  return &s_lut;
}

static void mock_GetPixInsightVersion(uint32 *major, uint32 *minor,
                                      uint32 *release, uint32 *revision,
                                      uint32 *betaRelease,
                                      uint32 *confidentialRelease,
                                      uint32 *leVersion, char *langCode) {
  if (major)
    *major = 1;
  if (minor)
    *minor = 8;
  if (release)
    *release = 9;
  if (revision)
    *revision = 1;
  if (betaRelease)
    *betaRelease = 0;
  if (confidentialRelease)
    *confidentialRelease = 0;
  if (leVersion)
    *leVersion = 0;
  if (langCode)
    std::strcpy(langCode, "en");
}

static void *mock_Allocate(pcl::size_type size) { return std::malloc(size); }

static api_bool mock_Deallocate(void *ptr) {
  std::free(ptr);
  return true;
}

static console_handle mock_GetConsole() {
  return reinterpret_cast<console_handle>(1);
}

static api_bool mock_ValidateConsole(const_console_handle) { return true; }

static QString g_currentConsoleLine;
static int g_consoleCursorPos = 0;
static bool g_currentLineLogged = false;
static std::mutex g_consoleMutex;

static QString cleanConsoleTags(QString text) {
  // 1. Process conditional breaks <cbr> and <cbr/>
  int cbrIdx;
  while ((cbrIdx = text.indexOf("<cbr")) != -1) {
    int endTag = text.indexOf('>', cbrIdx);
    if (endTag == -1)
      break;
    int tagLen = endTag - cbrIdx + 1;
    text.replace(cbrIdx, tagLen, "\n");
  }

  // 2. Process standard breaks <br> and <br/>
  int brIdx;
  while ((brIdx = text.indexOf("<br")) != -1) {
    int endTag = text.indexOf('>', brIdx);
    if (endTag == -1)
      break;
    text.replace(brIdx, endTag - brIdx + 1, "\n");
  }

  // 3. Strip cosmetic/positioning tags
  static const QStringList tagsToStrip = {"<end>", "<beg>",  "<eol>",
                                          "<bol>", "<bwd>",  "<fwd>",
                                          "<up>",  "<down>", "<bsp>"};
  for (const auto &tag : tagsToStrip) {
    text.replace(tag, "");
  }

  // 4. Intercept progress bar tags like <* pbr 50 *>
  int startIdx = 0;
  while ((startIdx = text.indexOf("<*", startIdx)) != -1) {
    int endIdx = text.indexOf("*>", startIdx);
    if (endIdx == -1)
      break;

    QString tag = text.mid(startIdx + 2, endIdx - startIdx - 2).trimmed();
    if (tag.startsWith("pbr ") || tag.startsWith("pbr_")) {
      QString valStr = tag.mid(4).trimmed();
      if (valStr.endsWith('%'))
        valStr.chop(1);
      bool ok = false;
      double val = valStr.toDouble(&ok);
      if (ok) {
        int pct = 0;
        if (val <= 1.0 && val > 0.0) {
          pct = static_cast<int>(val * 100.0);
        } else {
          pct = static_cast<int>(val);
        }
        if (pct >= 0 && pct <= 100) {
          if (g_pclBridgeInstance) {
            emit g_pclBridgeInstance->progressUpdated(pct);
          }
        }
      }
    }
    text.remove(startIdx, endIdx - startIdx + 2);
  }
  return text;
}

static void writeConsoleBuffer(const QString &str, bool appendNewline) {
  std::lock_guard<std::mutex> lock(g_consoleMutex);

  QString cleanStr = cleanConsoleTags(str);
  bool lineStateChanged = false;

  QString channel = "PCL";
  if (!g_activeExecutingProcessId.isEmpty()) {
    channel = g_activeExecutingProcessId;
  } else if (g_pclBridgeInstance && g_pclBridgeInstance->moduleDescription() &&
             g_pclBridgeInstance->moduleDescription()->name) {
    channel = QString::fromUtf8(g_pclBridgeInstance->moduleDescription()->name);
  }

  for (int i = 0; i < cleanStr.length(); ++i) {
    QChar c = cleanStr[i];
    if (c == '\n') {
      // Finalize the current line
      if (g_currentLineLogged) {
        Logger::info(channel, "\r" + g_currentConsoleLine);
      } else if (!g_currentConsoleLine.isEmpty()) {
        Logger::info(channel, g_currentConsoleLine);
      }
      g_currentConsoleLine.clear();
      g_consoleCursorPos = 0;
      g_currentLineLogged = false;
      lineStateChanged = false;
    } else if (c == '\r') {
      g_consoleCursorPos = 0;
      lineStateChanged = true;
    } else if (c == '\b') {
      if (g_consoleCursorPos > 0) {
        g_consoleCursorPos--;
      }
      lineStateChanged = true;
    } else {
      // Overwrite or append
      if (g_consoleCursorPos < g_currentConsoleLine.length()) {
        g_currentConsoleLine[g_consoleCursorPos] = c;
      } else {
        while (g_currentConsoleLine.length() < g_consoleCursorPos) {
          g_currentConsoleLine.append(' ');
        }
        g_currentConsoleLine.append(c);
      }
      g_consoleCursorPos++;
      lineStateChanged = true;
    }
  }

  if (appendNewline) {
    if (g_currentLineLogged) {
      Logger::info(channel, "\r" + g_currentConsoleLine);
    } else if (!g_currentConsoleLine.isEmpty()) {
      Logger::info(channel, g_currentConsoleLine);
    }
    g_currentConsoleLine.clear();
    g_consoleCursorPos = 0;
    g_currentLineLogged = false;
  } else if (lineStateChanged && !g_currentConsoleLine.isEmpty()) {
    // Parse progress percentage from current state
    static QRegularExpression pctRe("(\\d+)%");
    auto match = pctRe.match(g_currentConsoleLine);
    if (match.hasMatch()) {
      bool ok = false;
      int pct = match.captured(1).toInt(&ok);
      if (ok && pct >= 0 && pct <= 100) {
        if (g_pclBridgeInstance) {
          emit g_pclBridgeInstance->progressUpdated(pct);
        }
      }
    }

    // Print/update in LogWindow
    if (g_currentLineLogged) {
      Logger::info(channel, "\r" + g_currentConsoleLine);
    } else {
      Logger::info(channel, g_currentConsoleLine);
      g_currentLineLogged = true;
    }
  }
}

static api_bool mock_WriteConsole(console_handle, const char16_type *text,
                                  api_bool appendNewline) {
  QString str = QString::fromUtf16(reinterpret_cast<const char16_t *>(text));
  writeConsoleBuffer(str, appendNewline);
  return true;
}

static api_bool mock_FlushConsole(console_handle) { return true; }

static api_bool mock_ShowConsole(console_handle, api_bool) { return true; }

static void mock_ProcessEvents(api_bool excludeUserInputEvents) {
  if (qApp && QThread::currentThread() == qApp->thread()) {
    qApp->processEvents(excludeUserInputEvents
                            ? QEventLoop::ExcludeUserInputEvents
                            : QEventLoop::AllEvents);
  }
}

static api_bool mock_GetParameterValue(const_process_handle hProcess,
                                       meta_parameter_handle hParam,
                                       pcl::size_type rowIndex, uint32 *parType,
                                       void *value, pcl::size_type *length) {
  auto procIt = g_processParameters.find(const_cast<process_handle>(hProcess));
  if (procIt == g_processParameters.end())
    return false;

  ParameterKey key{hParam, rowIndex};
  auto paramIt = procIt->second.find(key);
  if (paramIt == procIt->second.end())
    return false;

  const auto &val = paramIt->second;
  if (parType)
    *parType = val.type;

  if (value == nullptr) {
    if (length)
      *length = val.data.size();
    return true;
  }

  if (length) {
    pcl::size_type copyLen = std::min(*length, (pcl::size_type)val.data.size());
    std::memcpy(value, val.data.data(), copyLen);
    *length = copyLen;
  }
  return true;
}

static api_bool mock_SetParameterValue(process_handle hProcess,
                                       meta_parameter_handle hParam,
                                       pcl::size_type rowIndex,
                                       const void *value,
                                       pcl::size_type length) {
  ParameterKey key{hParam, rowIndex};
  ParameterValue val;
  const uint8_t *bytePtr = reinterpret_cast<const uint8_t *>(value);
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

static void mock_BeginProcessDefinition(meta_process_handle hMeta,
                                        const char *procId) {
  g_currentDefiningProcess = hMeta;
  PCLProcessInfo info;
  info.id = QString::fromUtf8(procId);
  info.hMeta = hMeta;
  g_processes[hMeta] = info;
  g_processIdToHandle[procId] = hMeta;
  qDebug() << "[PCL Bridge] Defining process:" << procId;
}

static void mock_SetProcessCategory(const char *cat) {
  qDebug() << "[PCL Bridge] Process category:" << cat;
}

static void mock_SetProcessCreationRoutine(pcl::process_creation_routine f) {
  if (g_currentDefiningProcess)
    g_processes[g_currentDefiningProcess].createFn = f;
}

static void
mock_SetProcessDestructionRoutine(pcl::process_destruction_routine f) {
  if (g_currentDefiningProcess)
    g_processes[g_currentDefiningProcess].destroyFn = f;
}

static void
mock_SetProcessInitializationRoutine(pcl::process_initialization_routine f) {
  if (g_currentDefiningProcess)
    g_processes[g_currentDefiningProcess].initFn = f;
}

static void mock_SetProcessExecutionRoutine(pcl::process_execution_routine f) {
  if (g_currentDefiningProcess)
    g_processes[g_currentDefiningProcess].executeFn = f;
}

static void
mock_SetProcessGlobalExecutionRoutine(pcl::process_global_execution_routine f) {
  if (g_currentDefiningProcess)
    g_processes[g_currentDefiningProcess].executeGlobalFn = f;
}

static pcl::process_default_interface_selection_routine
    g_defaultInterfaceSelection = nullptr;
static pcl::process_interface_selection_routine g_interfaceSelection = nullptr;

static void mock_SetProcessDefaultInterfaceSelectionRoutine(
    pcl::process_default_interface_selection_routine f) {
  g_defaultInterfaceSelection = f;
  qDebug() << "[PCL Bridge] Default interface selection routine registered.";
}

static void mock_SetProcessInterfaceSelectionRoutine(
    pcl::process_interface_selection_routine f) {
  g_interfaceSelection = f;
  qDebug() << "[PCL Bridge] Interface selection routine registered.";
}

static void mock_EndProcessDefinition() { g_currentDefiningProcess = nullptr; }

static bool isViewMock(const void *h) {
  if (!h)
    return false;
  const auto *v = reinterpret_cast<const PCLViewMock *>(h);
  return v->magic == 0x56494557;
}

static PCLImageMock *resolveImageMockMutable(image_handle h) {
  if (!h)
    return nullptr;

  // Check if it's an attached client handle
  auto it = g_attachedImages.find(h);
  if (it != g_attachedImages.end()) {
    return it->second;
  }

  // Check if it's a heap-allocated image mock
  auto *img = reinterpret_cast<PCLImageMock *>(h);
  if (g_heapImages.count(img)) {
    return img;
  }

  // Fallback to active image if the handle matches g_activeImage
  if (img == g_activeImage) {
    return g_activeImage;
  }

  return nullptr;
}

static const PCLImageMock *resolveImageMock(const_image_handle h) {
  return resolveImageMockMutable(const_cast<image_handle>(h));
}

static api_bool mock_GetImageGeometry(const_image_handle h, uint32 *w,
                                      uint32 *h_out, uint32 *n) {
  const auto *img = resolveImageMock(h);
  if (!img)
    return false;
  if (w)
    *w = img->width;
  if (h_out)
    *h_out = img->height;
  if (n)
    *n = img->numChannels;
  return true;
}

static api_bool mock_GetImageFormat(const_image_handle h, uint32 *nbits,
                                    api_bool *flt) {
  const auto *img = resolveImageMock(h);
  if (!img)
    return false;
  if (nbits)
    *nbits = img->bitsPerSample;
  if (flt)
    *flt = img->isFloat;
  return true;
}

static api_bool mock_GetImagePixelData(image_handle h, void ***data) {
  auto *img = resolveImageMockMutable(h);
  if (!img || !data)
    return false;
  img->updatePointers();
  *data = img->channelDataPointers.data();
  return true;
}

static api_bool mock_SetImagePixelData(image_handle, void **) { return true; }

static api_bool mock_SetImageGeometry(image_handle h, uint32 w, uint32 h_out,
                                      uint32 n) {
  qDebug() << "[PCL Bridge] mock_SetImageGeometry called: image =" << h
           << "w =" << w << "h =" << h_out << "n =" << n;
  auto *img = resolveImageMockMutable(h);
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
  qDebug() << "[PCL Bridge] mock_SetImageColorSpace called: image =" << h
           << "colorSpace =" << cs;
  auto *img = resolveImageMockMutable(h);
  if (img) {
    img->colorSpace = cs;
    return true;
  }
  return false;
}

static image_handle mock_GetViewImage(view_handle hView) {
  if (!hView)
    return nullptr;
  auto *view = reinterpret_cast<PCLViewMock *>(hView);
  return view->hImage;
}

static window_handle mock_GetViewParentWindow(const_view_handle hView) {
  qDebug() << "[PCL Bridge] mock_GetViewParentWindow called: view =" << hView;
  if (!hView)
    return nullptr;
  auto *view = reinterpret_cast<const PCLViewMock *>(hView);
  return const_cast<PCLWindowMock *>(view->parentWindow);
}

static view_handle mock_GetImageWindowMainView(const_window_handle hWindow) {
  qDebug() << "[PCL Bridge] mock_GetImageWindowMainView called: window ="
           << hWindow;
  if (!hWindow)
    return nullptr;
  auto *window = reinterpret_cast<const PCLWindowMock *>(hWindow);
  return window->mainView;
}

static view_handle mock_GetImageWindowCurrentView(const_window_handle hWindow) {
  qDebug() << "[PCL Bridge] mock_GetImageWindowCurrentView called: window ="
           << hWindow;
  if (!hWindow)
    return nullptr;
  auto *window = reinterpret_cast<const PCLWindowMock *>(hWindow);
  return window->mainView;
}

static api_bool mock_GetViewScreenTransferFunctions(const_view_handle hView,
                                                    double *m, double *c0,
                                                    double *c1, double *r0,
                                                    double *r1) {
  qDebug() << "[PCL Bridge] mock_GetViewScreenTransferFunctions called: view ="
           << hView;

  // Compute per-channel STF parameters from the actual image data.
  // PixInsight autostretch: c0 = max(0, median - 2.8*MAD), midtone m places
  // background at targetBkg. We compute statistics for channels 0..N-1 and set
  // channel 3 as the average (luminance joint channel).

  const double targetBkg = 0.25; // PixInsight default target background level
  const double kMAD = 2.8;       // Shadow clipping factor

  int nChan = g_activeImage ? (int)g_activeImage->numChannels : 3;

  // Autostretch: midtone balance is solved analytically below — no MTF lambda
  // needed.

  // Initialize all 4 channels to neutral defaults
  for (int i = 0; i < 4; ++i) {
    if (m)
      m[i] = 0.5;
    if (c0)
      c0[i] = 0.0;
    if (c1)
      c1[i] = 1.0;
    if (r0)
      r0[i] = 0.0;
    if (r1)
      r1[i] = 1.0;
  }

  if (!g_activeImage)
    return true;

  double sumM[3] = {0, 0, 0};
  double sumC0[3] = {0, 0, 0};
  int validChans = 0;

  for (int c = 0; c < nChan && c < 3; ++c) {
    const float *data = nullptr;
    int N = g_activeImage->width * g_activeImage->height;
    if (!g_activeImage->wrappedBuffers.empty() &&
        c < (int)g_activeImage->wrappedBuffers.size()) {
      data = g_activeImage->wrappedBuffers[c]->data();
    } else if (c < (int)g_activeImage->ownedChannels.size()) {
      data = g_activeImage->ownedChannels[c].data();
    }
    if (!data || N == 0)
      continue;

    // Compute mean and a robust MAD approximation using sampled pixels
    int sampleStep = std::max(1, N / 10000);
    double sum = 0;
    int count = 0;
    for (int p = 0; p < N; p += sampleStep) {
      sum += data[p];
      ++count;
    }
    double mean = count > 0 ? sum / count : 0.0;

    // Approximate MAD as mean absolute deviation from mean (fast, no sort
    // needed)
    double madSum = 0;
    for (int p = 0; p < N; p += sampleStep) {
      madSum += std::abs(data[p] - mean);
    }
    double mad = count > 0 ? madSum / count : 0.01;

    // Compute shadow clipping
    double shadows = std::max(0.0, mean - kMAD * mad);
    double clippedMean = (mean - shadows) / (1.0 - shadows);

    // Midtone balance: find m such that MTF(clippedMean, m) = targetBkg
    // Solving: targetBkg*(2*m-1)*x - targetBkg*m = (m-1)*x
    // => m = (targetBkg*x - x) / (targetBkg*2*x - targetBkg - x) ...
    // algebraically: m = (x - targetBkg*x) / (x - 2*targetBkg*x + targetBkg)
    //   = x*(1-targetBkg) / (x*(1-2*targetBkg) + targetBkg)
    double midtone = 0.5;
    if (clippedMean > 0.0 && clippedMean < 1.0) {
      double x = clippedMean;
      double d = x * (1.0 - 2.0 * targetBkg) + targetBkg;
      if (std::abs(d) > 1e-10)
        midtone = std::max(0.001, std::min(0.999, x * (1.0 - targetBkg) / d));
    }

    if (m)
      m[c] = midtone;
    if (c0)
      c0[c] = shadows;
    if (c1)
      c1[c] = 1.0;
    if (r0)
      r0[c] = 0.0;
    if (r1)
      r1[c] = 1.0;

    sumM[c] = midtone;
    sumC0[c] = shadows;
    ++validChans;

    qDebug() << "[PCL Bridge] STF ch" << c << ": mean=" << mean << "mad=" << mad
             << "shadows=" << shadows << "midtone=" << midtone;
  }

  // Channel 3: joint luminance — average of R/G/B
  if (validChans > 0) {
    double avgM = 0, avgC0 = 0;
    for (int c = 0; c < validChans; ++c) {
      avgM += sumM[c];
      avgC0 += sumC0[c];
    }
    avgM /= validChans;
    avgC0 /= validChans;
    if (m)
      m[3] = avgM;
    if (c0)
      c0[3] = avgC0;
    if (c1)
      c1[3] = 1.0;
    if (r0)
      r0[3] = 0.0;
    if (r1)
      r1[3] = 1.0;
  }

  return true;
}

static api_bool mock_GetViewScreenTransferFunctionsEnabled(view_handle hView) {
  qDebug() << "[PCL Bridge] mock_GetViewScreenTransferFunctionsEnabled called: "
              "view ="
           << hView;
  return true; // STF always enabled — DeepSNR needs it to calibrate noise model
}

static api_bool mock_GetImageColorSpace(const_image_handle h, uint32 *cs) {
  if (!h)
    return false;
  auto *img = reinterpret_cast<const PCLImageMock *>(h);
  if (cs)
    *cs = img->colorSpace;
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

static image_handle mock_CreateImage(uint32 w, uint32 h, uint32 n, uint32 nbits,
                                     api_bool flt, uint32 cs, void *) {
  qDebug() << "[PCL Bridge] mock_CreateImage called: w =" << w << "h =" << h
           << "n =" << n;
  auto *img = new PCLImageMock();
  img->width = w;
  img->height = h;
  img->numChannels = n;
  img->bitsPerSample = nbits;
  img->isFloat = flt;
  img->colorSpace = cs;
  img->ownedChannels.resize(n);
  for (uint32 i = 0; i < n; ++i) {
    img->ownedChannels[i].resize(w * h, 0.0f);
  }
  img->updatePointers();
  g_heapImages.insert(img);
  return reinterpret_cast<image_handle>(img);
}

static api_bool mock_AttachToImage(image_handle hImage, const_view_handle hView,
                                   api_bool readOnly) {
  qDebug() << "[PCL Bridge] mock_AttachToImage called: image =" << hImage
           << "view =" << hView << "readOnly =" << readOnly;
  if (!hImage) {
    qWarning() << "[PCL Bridge] mock_AttachToImage: null image handle passed!";
    return false;
  }

  PCLImageMock *source = nullptr;
  if (hView) {
    if (isViewMock(hView)) {
      auto *view = reinterpret_cast<const PCLViewMock *>(hView);
      source = view->hImage;
    } else {
      auto *srcImg =
          reinterpret_cast<PCLImageMock *>(const_cast<void *>(hView));
      if (g_heapImages.count(srcImg) || srcImg == g_activeImage) {
        source = srcImg;
      }
    }
  }
  if (!source) {
    source = g_activeImage;
  }

  if (!source) {
    qWarning()
        << "[PCL Bridge] mock_AttachToImage: failed to find source image mock!";
    return false;
  }

  auto *target = resolveImageMockMutable(hImage);
  if (target) {
    target->width = source->width;
    target->height = source->height;
    target->numChannels = source->numChannels;
    target->bitsPerSample = source->bitsPerSample;
    target->isFloat = source->isFloat;
    target->colorSpace = source->colorSpace;
    target->wrappedBuffers = source->wrappedBuffers;
    target->ownedChannels = source->ownedChannels;
    target->updatePointers();
    qDebug() << "[PCL Bridge] mock_AttachToImage: successfully copied source "
                "image to target mock";
  }

  g_attachedImages[hImage] = source;
  qDebug() << "[PCL Bridge] mock_AttachToImage: mapped client handle" << hImage
           << "to source mock" << source;

  return true;
}

static api_bool mock_GetImageRGBWS(const_image_handle hImage,
                                   api_RGBWS *rgbws) {
  qDebug() << "[PCL Bridge] mock_GetImageRGBWS called: image =" << hImage
           << "rgbws =" << rgbws;
  if (!rgbws)
    return false;

  rgbws->gamma = 2.2f;
  rgbws->isSRGBGamma = true;

  // Default sRGB chromaticity coordinates (Rx, Gx, Bx) and (Ry, Gy, By)
  rgbws->x[0] = 0.64f;
  rgbws->y[0] = 0.33f;
  rgbws->x[1] = 0.30f;
  rgbws->y[1] = 0.60f;
  rgbws->x[2] = 0.15f;
  rgbws->y[2] = 0.06f;

  // Default sRGB luminance coefficients
  rgbws->Y[0] = 0.212639f;
  rgbws->Y[1] = 0.715168f;
  rgbws->Y[2] = 0.072193f;

  return true;
}

static api_bool mock_DetachFromImage(image_handle hImage) {
  qDebug() << "[PCL Bridge] mock_DetachFromImage called: image =" << hImage;
  g_attachedImages.erase(hImage);
  auto *img = reinterpret_cast<PCLImageMock *>(hImage);
  if (img && g_heapImages.count(img)) {
    g_heapImages.erase(img);
    delete img;
  }
  return true;
}

static bool g_abortEnabled = false;
static bool g_abortRequested = false;

static api_bool mock_Abort() {
  qDebug() << "[PCL Bridge] mock_Abort called";
  g_abortRequested = true;
  return true;
}

static api_bool mock_DisableAbort() {
  qDebug() << "[PCL Bridge] mock_DisableAbort called";
  g_abortEnabled = false;
  return true;
}

static api_bool mock_EnableAbort() {
  qDebug() << "[PCL Bridge] mock_EnableAbort called";
  g_abortEnabled = true;
  return true;
}

static uint32 mock_GetProcessStatus() {
  uint32 status = 0;
  if (g_abortEnabled)
    status |= 0x40000000;
  if (g_abortRequested)
    status |= 0x80000000;
  return status;
}

static void fillPropertyValue(api_property_value *value,
                              const std::string &id) {
  if (!value)
    return;
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

static api_bool mock_GetViewPropertyExists(api_handle hModule,
                                           const_view_handle hView,
                                           const char *id, uint64 *type) {
  qDebug() << "[PCL Bridge] mock_GetViewPropertyExists called: view =" << hView
           << "id =" << id;
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

static api_bool mock_ComputeViewProperty(api_handle hModule, view_handle hView,
                                         const char *id, api_bool notify,
                                         api_property_value *value) {
  qDebug() << "[PCL Bridge] mock_ComputeViewProperty called: view =" << hView
           << "id =" << id;
  std::string propId = id ? id : "";
  fillPropertyValue(value, propId);
  return true;
}

static api_bool mock_GetViewPropertyValue(api_handle hModule,
                                          const_view_handle hView,
                                          const char *id,
                                          api_property_value *value) {
  qDebug() << "[PCL Bridge] mock_GetViewPropertyValue called: view =" << hView
           << "id =" << id;
  std::string propId = id ? id : "";
  fillPropertyValue(value, propId);
  return true;
}

static api_bool mock_SetViewPropertyValue(api_handle hModule, view_handle hView,
                                          const char *id,
                                          const api_property_value *value,
                                          uint32 flags, api_bool notify) {
  qDebug() << "[PCL Bridge] mock_SetViewPropertyValue called: view =" << hView
           << "id =" << id;
  return true;
}

// ----------------------------------------------------------------------------
// Mock Module & Interface lifecycle routines
// ----------------------------------------------------------------------------

static void mock_SetModuleOnLoadRoutine(pcl::module_on_load_routine f) {
  if (g_currentLoadingModule) {
    g_currentLoadingModule->onLoadFn = f;
  }
  qDebug() << "[PCL Bridge] Module OnLoad routine registered.";
}

static void mock_SetModuleOnUnloadRoutine(pcl::module_on_unload_routine f) {
  if (g_currentLoadingModule) {
    g_currentLoadingModule->onUnloadFn = f;
  }
  qDebug() << "[PCL Bridge] Module OnUnload routine registered.";
}

static void mock_BeginInterfaceDefinition(meta_interface_handle hMeta,
                                          const char *ifaceId, uint32 flags) {
  g_currentDefiningInterface = hMeta;
  PCLInterfaceInfo info;
  info.id = QString::fromUtf8(ifaceId);
  info.hMeta = hMeta;
  g_interfaces[hMeta] = info;
  g_interfaceIdToHandle[ifaceId] = hMeta;
  qDebug() << "[PCL Bridge] Defining interface:" << ifaceId;
}

static void mock_SetInterfaceInitializationRoutine(
    pcl::interface_initialization_routine f) {
  if (g_currentDefiningInterface) {
    g_interfaces[g_currentDefiningInterface].initFn = f;
    qDebug() << "[PCL Bridge] Interface initialization routine set for:"
             << g_interfaces[g_currentDefiningInterface].id;
  }
}

static void mock_SetInterfaceLaunchRoutine(pcl::interface_launch_routine f) {
  if (g_currentDefiningInterface) {
    g_interfaces[g_currentDefiningInterface].launchFn = f;
    qDebug() << "[PCL Bridge] Interface launch routine set for:"
             << g_interfaces[g_currentDefiningInterface].id;
  }
}

static void
mock_SetInterfaceEditPreferencesRoutine(pcl::interface_control_routine f) {
  if (g_currentDefiningInterface) {
    g_interfaces[g_currentDefiningInterface].editPreferencesFn = f;
    qDebug() << "[PCL Bridge] Interface edit preferences routine set for:"
             << g_interfaces[g_currentDefiningInterface].id;
  }
}

static void
mock_SetInterfaceProcessImportRoutine(pcl::interface_process_import_routine f) {
  if (g_currentDefiningInterface) {
    g_interfaces[g_currentDefiningInterface].importProcessFn = f;
    qDebug() << "[PCL Bridge] Interface process import routine set for:"
             << g_interfaces[g_currentDefiningInterface].id;
  }
}

static void mock_SetGlobalPreferencesUpdatedNotificationRoutine(
    pcl::global_notification_routine f) {
  if (g_currentDefiningInterface) {
    g_interfaces[g_currentDefiningInterface].globalPrefUpdatedFn = f;
    qDebug() << "[PCL Bridge] Global preferences updated notification routine "
                "set for:"
             << g_interfaces[g_currentDefiningInterface].id;
  }
}

static void mock_SetInterfaceProcessValidationRoutine(
    pcl::interface_process_validation_routine f) {
  if (g_currentDefiningInterface) {
    g_interfaces[g_currentDefiningInterface].validateProcessFn = f;
    qDebug() << "[PCL Bridge] Interface process validation routine set for:"
             << g_interfaces[g_currentDefiningInterface].id;
  }
}

static void mock_SetProcessInterfaceValidationRoutine(
    pcl::process_interface_validation_routine f) {
  if (g_currentDefiningProcess) {
    g_processes[g_currentDefiningProcess].validateInterfaceFn = f;
    qDebug() << "[PCL Bridge] Process interface validation routine set for:"
             << g_processes[g_currentDefiningProcess].id;
  }
}

static void mock_EndInterfaceDefinition() {
  g_currentDefiningInterface = nullptr;
}

// ----------------------------------------------------------------------------
// Mock UI Controls & layouts C-API
// ----------------------------------------------------------------------------

static control_handle mock_CreateControl(api_handle hModule, api_handle client,
                                         control_handle parent, uint32 flags) {
  QWidget *parentWidget = reinterpret_cast<QWidget *>(parent);
  QWidget *w = new QWidget(parentWidget);
  control_handle ch = reinterpret_cast<control_handle>(w);
  if (client)
    g_widgetToClient[ch] = client;
  qDebug() << "[PCL Bridge] CreateControl: client =" << client
           << "parent =" << parentWidget << "-> QWidget =" << w;
  if (!parentWidget) {
    qDebug() << "[PCL Bridge] Detected top-level control creation. Associating "
                "with pending interface.";
    for (const auto &pair : g_interfaces) {
      std::string id = pair.second.id.toStdString();
      if (g_interfaceControls.find(id) == g_interfaceControls.end()) {
        g_interfaceControls[id] = ch;
        qDebug() << "[PCL Bridge] Associated top-level control" << w
                 << "with interface ID:" << pair.second.id;
        break;
      }
    }
  }
  return ch;
}

static void mock_SetControlParent(control_handle h, control_handle parent) {
  QWidget *w = reinterpret_cast<QWidget *>(h);
  QWidget *p = reinterpret_cast<QWidget *>(parent);
  qDebug() << "[PCL Bridge] SetControlParent:" << w << "parent =" << p;
  if (w) {
    w->setParent(p);
  }
}

static void mock_SetControlSizer(control_handle h, sizer_handle s) {
  QWidget *w = reinterpret_cast<QWidget *>(h);
  QLayout *lay = reinterpret_cast<QLayout *>(s);
  qDebug() << "[PCL Bridge] SetControlSizer:" << w << "layout =" << lay;
  if (w && lay) {
    w->setLayout(lay);
  }
}

static void mock_SetControlMinSize(control_handle h, int32 w, int32 h_size) {
  QWidget *widget = reinterpret_cast<QWidget *>(h);
  qDebug() << "[PCL Bridge] SetControlMinSize:" << widget << "width =" << w
           << "height =" << h_size;
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
  QWidget *widget = reinterpret_cast<QWidget *>(h);
  qDebug() << "[PCL Bridge] SetControlMaxSize:" << widget << "width =" << w
           << "height =" << h_size;
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
  QWidget *widget = reinterpret_cast<QWidget *>(h);
  qDebug() << "[PCL Bridge] AdjustControlToContents:" << widget;
  if (widget) {
    widget->adjustSize();
  }
}

static control_handle mock_CreateDialog(api_handle hModule, api_handle client,
                                        control_handle parent, uint32 flags) {
  QWidget *parentWidget = reinterpret_cast<QWidget *>(parent);
  QDialog *dlg = new QDialog(parentWidget);
  control_handle ch = reinterpret_cast<control_handle>(dlg);
  if (client)
    g_widgetToClient[ch] = client;
  qDebug() << "[PCL Bridge] CreateDialog: client =" << client
           << "parent =" << parentWidget << "-> QDialog =" << dlg;
  if (!parentWidget) {
    qDebug() << "[PCL Bridge] Detected top-level dialog creation. Associating "
                "with pending interface.";
    for (const auto &pair : g_interfaces) {
      std::string id = pair.second.id.toStdString();
      if (g_interfaceControls.find(id) == g_interfaceControls.end()) {
        g_interfaceControls[id] = ch;
        qDebug() << "[PCL Bridge] Associated top-level dialog" << dlg
                 << "with interface ID:" << pair.second.id;
        break;
      }
    }
  }
  return ch;
}

static sizer_handle mock_CreateSizer(api_handle hModule, api_bool vertical) {
  QLayout *lay = nullptr;
  if (vertical) {
    lay = new QVBoxLayout();
  } else {
    lay = new QHBoxLayout();
  }
  qDebug() << "[PCL Bridge] CreateSizer: vertical =" << (bool)vertical
           << "-> QLayout =" << lay;
  return reinterpret_cast<sizer_handle>(lay);
}

static void mock_InsertSizerControl(sizer_handle s, int32 index,
                                    control_handle c, int32 stretch,
                                    int32 alignment) {
  QBoxLayout *lay = reinterpret_cast<QBoxLayout *>(s);
  QWidget *w = reinterpret_cast<QWidget *>(c);
  qDebug() << "[PCL Bridge] InsertSizerControl: layout =" << lay
           << "widget =" << w << "index =" << index;
  if (lay && w) {
    if (index < 0) {
      lay->addWidget(w, stretch);
    } else {
      lay->insertWidget(index, w, stretch);
    }
  }
}

static void mock_InsertSizer(sizer_handle s, int32 index,
                             sizer_handle childSizer, int32 stretch) {
  QBoxLayout *lay = reinterpret_cast<QBoxLayout *>(s);
  QLayout *childLay = reinterpret_cast<QLayout *>(childSizer);
  qDebug() << "[PCL Bridge] InsertSizer: layout =" << lay
           << "child layout =" << childLay << "index =" << index;
  if (lay && childLay) {
    if (index < 0) {
      lay->addLayout(childLay, stretch);
    } else {
      lay->insertLayout(index, childLay, stretch);
    }
  }
}

static void mock_InsertSizerSpacing(sizer_handle s, int32 index, int32 size) {
  QBoxLayout *lay = reinterpret_cast<QBoxLayout *>(s);
  qDebug() << "[PCL Bridge] InsertSizerSpacing: layout =" << lay
           << "size =" << size;
  if (lay) {
    if (index < 0) {
      lay->addSpacing(size);
    } else {
      lay->insertSpacing(index, size);
    }
  }
}

static void mock_InsertSizerStretch(sizer_handle s, int32 index,
                                    int32 stretch) {
  QBoxLayout *lay = reinterpret_cast<QBoxLayout *>(s);
  qDebug() << "[PCL Bridge] InsertSizerStretch: layout =" << lay
           << "stretch =" << stretch;
  if (lay) {
    if (index < 0) {
      lay->addStretch(stretch);
    } else {
      lay->insertStretch(index, stretch);
    }
  }
}

static void mock_SetSizerMargin(sizer_handle s, int32 margin) {
  QLayout *lay = reinterpret_cast<QLayout *>(s);
  qDebug() << "[PCL Bridge] SetSizerMargin: layout =" << lay
           << "margin =" << margin;
  if (lay) {
    lay->setContentsMargins(margin, margin, margin, margin);
  }
}

static void mock_SetSizerSpacing(sizer_handle s, int32 spacing) {
  QBoxLayout *lay = reinterpret_cast<QBoxLayout *>(s);
  qDebug() << "[PCL Bridge] SetSizerSpacing: layout =" << lay
           << "spacing =" << spacing;
  if (lay) {
    lay->setSpacing(spacing);
  }
}

static control_handle mock_CreateGroupBox(api_handle hModule, api_handle client,
                                          const char16_type *title,
                                          control_handle parent, uint32 flags) {
  QWidget *parentWidget = reinterpret_cast<QWidget *>(parent);
  QGroupBox *gb = new QGroupBox(parentWidget);
  if (title) {
    gb->setTitle(QString::fromUtf16(reinterpret_cast<const char16_t *>(title)));
  }
  control_handle ch = reinterpret_cast<control_handle>(gb);
  if (client)
    g_widgetToClient[ch] = client;
  qDebug() << "[PCL Bridge] CreateGroupBox: client =" << client
           << "title =" << gb->title() << "-> QGroupBox =" << gb;
  return ch;
}

static control_handle
mock_CreatePushButton(api_handle hModule, api_handle client,
                      const char16_type *text, const_bitmap_handle bitmap,
                      control_handle parent, uint32 flags) {
  QWidget *parentWidget = reinterpret_cast<QWidget *>(parent);
  QPushButton *btn = new QPushButton(parentWidget);
  if (text) {
    btn->setText(QString::fromUtf16(reinterpret_cast<const char16_t *>(text)));
  }
  control_handle ch = reinterpret_cast<control_handle>(btn);
  if (client)
    g_widgetToClient[ch] = client;
  qDebug() << "[PCL Bridge] CreatePushButton: client =" << client
           << "text =" << btn->text() << "-> QPushButton =" << btn;
  return ch;
}

#include <QToolButton>

static control_handle
mock_CreateToolButton(api_handle hModule, api_handle client,
                      const char16_type *text, const_bitmap_handle bitmap,
                      api_bool checkable, control_handle parent, uint32 flags) {
  QWidget *parentWidget = reinterpret_cast<QWidget *>(parent);
  ::QToolButton *btn = new ::QToolButton(parentWidget);
  if (text) {
    btn->setText(QString::fromUtf16(reinterpret_cast<const char16_t *>(text)));
  }
  btn->setCheckable(checkable);
  control_handle ch = reinterpret_cast<control_handle>(btn);
  if (client)
    g_widgetToClient[ch] = client;
  qDebug() << "[PCL Bridge] CreateToolButton: client =" << client
           << "text =" << btn->text() << "checkable =" << (bool)checkable
           << "-> QToolButton =" << btn;
  return ch;
}

static void mock_SetToolButtonCheckable(control_handle h, api_bool checkable) {
  ::QToolButton *btn = reinterpret_cast<::QToolButton *>(h);
  qDebug() << "[PCL Bridge] SetToolButtonCheckable:" << btn
           << "checkable =" << (bool)checkable;
  if (btn) {
    btn->setCheckable(checkable);
  }
}

static api_bool mock_GetToolButtonCheckable(const_control_handle h) {
  const ::QToolButton *btn = reinterpret_cast<const ::QToolButton *>(h);
  return btn ? btn->isCheckable() : false;
}

static control_handle mock_CreateCheckBox(api_handle hModule, api_handle client,
                                          const char16_type *text,
                                          control_handle parent, uint32 flags) {
  QWidget *parentWidget = reinterpret_cast<QWidget *>(parent);
  QCheckBox *cb = new QCheckBox(parentWidget);
  if (text) {
    cb->setText(QString::fromUtf16(reinterpret_cast<const char16_t *>(text)));
  }
  control_handle ch = reinterpret_cast<control_handle>(cb);
  if (client)
    g_widgetToClient[ch] = client;
  qDebug() << "[PCL Bridge] CreateCheckBox: client =" << client
           << "text =" << cb->text() << "-> QCheckBox =" << cb;
  return ch;
}

static void mock_SetButtonText(control_handle h, const char16_type *text) {
  QAbstractButton *btn = reinterpret_cast<QAbstractButton *>(h);
  QString str =
      text ? QString::fromUtf16(reinterpret_cast<const char16_t *>(text))
           : QString();
  qDebug() << "[PCL Bridge] SetButtonText:" << btn << "text =" << str;
  if (btn) {
    btn->setText(str);
  }
}

static control_handle mock_CreateComboBox(api_handle hModule, api_handle client,
                                          control_handle parent, uint32 flags) {
  QWidget *parentWidget = reinterpret_cast<QWidget *>(parent);
  QComboBox *cb = new QComboBox(parentWidget);
  control_handle ch = reinterpret_cast<control_handle>(cb);
  if (client)
    g_widgetToClient[ch] = client;
  qDebug() << "[PCL Bridge] CreateComboBox: client =" << client
           << "parent =" << parentWidget << "-> QComboBox =" << cb;
  return ch;
}

static void mock_InsertComboBoxItem(control_handle h, int32 index,
                                    const char16_type *text,
                                    const_bitmap_handle bitmap) {
  QComboBox *cb = reinterpret_cast<QComboBox *>(h);
  QString str =
      text ? QString::fromUtf16(reinterpret_cast<const char16_t *>(text))
           : QString();
  qDebug() << "[PCL Bridge] InsertComboBoxItem:" << cb << "index =" << index
           << "text =" << str;
  if (cb) {
    cb->insertItem(index, str);
  }
}

static void mock_ClearComboBox(control_handle h) {
  QComboBox *cb = reinterpret_cast<QComboBox *>(h);
  qDebug() << "[PCL Bridge] ClearComboBox:" << cb;
  if (cb) {
    cb->clear();
  }
}

static void mock_SetComboBoxCurrentItem(control_handle h, int32 index) {
  QComboBox *cb = reinterpret_cast<QComboBox *>(h);
  qDebug() << "[PCL Bridge] SetComboBoxCurrentItem:" << cb
           << "index =" << index;
  if (cb) {
    cb->blockSignals(true);
    cb->setCurrentIndex(index);
    cb->blockSignals(false);
  }
}

static int32 mock_GetComboBoxCurrentItem(const_control_handle h) {
  const QComboBox *cb = reinterpret_cast<const QComboBox *>(h);
  int32 idx = cb ? cb->currentIndex() : -1;
  qDebug() << "[PCL Bridge] GetComboBoxCurrentItem:" << cb << "->" << idx;
  return idx;
}

static control_handle mock_CreateSpinBox(api_handle hModule, api_handle client,
                                         control_handle parent, uint32 flags) {
  QWidget *parentWidget = reinterpret_cast<QWidget *>(parent);
  QSpinBox *sb = new QSpinBox(parentWidget);
  control_handle ch = reinterpret_cast<control_handle>(sb);
  if (client)
    g_widgetToClient[ch] = client;
  qDebug() << "[PCL Bridge] CreateSpinBox: client =" << client
           << "parent =" << parentWidget << "-> QSpinBox =" << sb;
  return ch;
}

static void mock_SetSpinBoxRange(control_handle h, int32 minVal, int32 maxVal) {
  QSpinBox *sb = reinterpret_cast<QSpinBox *>(h);
  qDebug() << "[PCL Bridge] SetSpinBoxRange:" << sb << "range = [" << minVal
           << "," << maxVal << "]";
  if (sb) {
    sb->blockSignals(true);
    sb->setRange(minVal, maxVal);
    sb->blockSignals(false);
  }
}

static void mock_SetSpinBoxValue(control_handle h, int32 val) {
  QSpinBox *sb = reinterpret_cast<QSpinBox *>(h);
  qDebug() << "[PCL Bridge] SetSpinBoxValue:" << sb << "value =" << val;
  if (sb) {
    sb->blockSignals(true);
    sb->setValue(val);
    sb->blockSignals(false);
  }
}

static int32 mock_GetSpinBoxValue(const_control_handle h) {
  const QSpinBox *sb = reinterpret_cast<const QSpinBox *>(h);
  int32 val = sb ? sb->value() : 0;
  qDebug() << "[PCL Bridge] GetSpinBoxValue:" << sb << "->" << val;
  return val;
}

static control_handle mock_CreateLabel(api_handle hModule, api_handle client,
                                       const char16_type *text,
                                       control_handle parent, uint32 flags) {
  QWidget *parentWidget = reinterpret_cast<QWidget *>(parent);
  QLabel *lbl = new QLabel(parentWidget);
  if (text) {
    lbl->setText(QString::fromUtf16(reinterpret_cast<const char16_t *>(text)));
  }
  control_handle ch = reinterpret_cast<control_handle>(lbl);
  if (client)
    g_widgetToClient[ch] = client;
  qDebug() << "[PCL Bridge] CreateLabel: client =" << client
           << "text =" << lbl->text() << "-> QLabel =" << lbl;
  return ch;
}

static void mock_SetLabelText(control_handle h, const char16_type *text) {
  QLabel *lbl = reinterpret_cast<QLabel *>(h);
  QString str =
      text ? QString::fromUtf16(reinterpret_cast<const char16_t *>(text))
           : QString();
  qDebug() << "[PCL Bridge] SetLabelText:" << lbl << "text =" << str;
  if (lbl) {
    lbl->setText(str);
  }
}

static control_handle mock_CreateSlider(api_handle hModule, api_handle client,
                                        api_bool vertical,
                                        control_handle parent, uint32 flags) {
  QWidget *parentWidget = reinterpret_cast<QWidget *>(parent);
  QSlider *sl =
      new QSlider(vertical ? Qt::Vertical : Qt::Horizontal, parentWidget);
  control_handle ch = reinterpret_cast<control_handle>(sl);
  if (client)
    g_widgetToClient[ch] = client;
  qDebug() << "[PCL Bridge] CreateSlider: client =" << client
           << "vertical =" << (bool)vertical << "-> QSlider =" << sl;
  return ch;
}

static control_handle mock_CreateEdit(api_handle hModule, api_handle client,
                                      const char16_type *text,
                                      control_handle parent, uint32 flags) {
  QWidget *parentWidget = reinterpret_cast<QWidget *>(parent);
  QLineEdit *edit = new QLineEdit(parentWidget);
  if (text) {
    edit->setText(QString::fromUtf16(reinterpret_cast<const char16_t *>(text)));
  }
  control_handle ch = reinterpret_cast<control_handle>(edit);
  if (client)
    g_widgetToClient[ch] = client;
  qDebug() << "[PCL Bridge] CreateEdit: client =" << client
           << "parent =" << parentWidget << "text =" << edit->text()
           << "-> QLineEdit =" << edit;
  return ch;
}

static api_bool mock_GetEditText(const_control_handle h, char16_type *text,
                                 pcl::size_type *length) {
  const QLineEdit *edit = reinterpret_cast<const QLineEdit *>(h);
  QString str = edit ? edit->text() : QString();
  qDebug() << "[PCL Bridge] GetEditText:" << edit << "->" << str;
  if (!length)
    return false;
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

static void mock_SetEditText(control_handle h, const char16_type *text) {
  QLineEdit *edit = reinterpret_cast<QLineEdit *>(h);
  QString str =
      text ? QString::fromUtf16(reinterpret_cast<const char16_t *>(text))
           : QString();
  qDebug() << "[PCL Bridge] SetEditText:" << edit << "text =" << str;
  if (edit) {
    edit->blockSignals(true);
    edit->setText(str);
    edit->blockSignals(false);
  }
}

static api_bool mock_GetEditReadOnly(const_control_handle h) {
  const QLineEdit *edit = reinterpret_cast<const QLineEdit *>(h);
  return edit ? edit->isReadOnly() : false;
}

static void mock_SetEditReadOnly(control_handle h, api_bool readOnly) {
  QLineEdit *edit = reinterpret_cast<QLineEdit *>(h);
  qDebug() << "[PCL Bridge] SetEditReadOnly:" << edit
           << "readOnly =" << (bool)readOnly;
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

static std::unordered_map<api_handle, PCLThreadMock *> g_threads;
static thread_local thread_handle g_currentThread = nullptr;

static thread_handle mock_CreateThread(api_handle hModule, api_handle client,
                                       uint32 flags) {
  qDebug() << "[PCL Bridge] mock_CreateThread called: client =" << client;
  auto *thread = new PCLThreadMock();
  thread->client = client;
  g_threads[client] = thread;
  return client;
}

static api_bool mock_SetThreadExecRoutine(thread_handle hThread,
                                          pcl::thread_exec_routine execFn) {
  qDebug() << "[PCL Bridge] mock_SetThreadExecRoutine called: thread ="
           << hThread;
  auto it = g_threads.find(hThread);
  if (it != g_threads.end()) {
    it->second->execFn = execFn;
    return true;
  }
  return false;
}

static void mock_StartThread(thread_handle hThread, uint32 priority) {
  qDebug() << "[PCL Bridge] mock_StartThread called: thread =" << hThread
           << "priority =" << priority;
  auto it = g_threads.find(hThread);
  if (it != g_threads.end()) {
    auto *thread = it->second;
    if (thread && thread->execFn) {
      thread->active = true;
      qDebug() << "[PCL Bridge] Spawning background thread asynchronously...";
      std::thread([hThread]() {
        g_currentThread = hThread;
        auto itInner = g_threads.find(hThread);
        if (itInner != g_threads.end()) {
          auto *threadInner = itInner->second;
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
  qDebug() << "[PCL Bridge] mock_WaitThread called: thread =" << hThread
           << "msec =" << msec;
  auto it = g_threads.find(hThread);
  if (it != g_threads.end()) {
    auto *thread = it->second;
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
  qDebug() << "[PCL Bridge] mock_SleepThread called: thread =" << hThread
           << "msec =" << msec;
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

static thread_handle mock_GetCurrentThread() { return g_currentThread; }

static void mock_AppendThreadConsoleOutputText(thread_handle,
                                               const char16_type *text,
                                               api_bool appendNewline) {
  QString str = QString::fromUtf16(reinterpret_cast<const char16_t *>(text));
  writeConsoleBuffer(str, appendNewline);
}

static api_bool mock_GetSizerDisplayPixelRatio(const_sizer_handle h,
                                               double *ratio) {
  qDebug() << "[PCL Bridge] GetSizerDisplayPixelRatio:" << h;
  if (ratio) {
    *ratio = 1.0;
  }
  return true;
}

static api_bool mock_AttachToUIObject(api_handle hModule,
                                      api_handle hUIObject) {
  qDebug() << "[PCL Bridge] AttachToUIObject: UIObject =" << hUIObject;
  return true;
}

static api_bool mock_DetachFromUIObject(api_handle hModule,
                                        api_handle hUIObject) {
  qDebug() << "[PCL Bridge] DetachFromUIObject: UIObject =" << hUIObject;
  return true;
}

static void mock_SetLabelAlignment(control_handle h, int32 align) {
  QLabel *lbl = reinterpret_cast<QLabel *>(h);
  qDebug() << "[PCL Bridge] SetLabelAlignment:" << lbl << "align =" << align;
  if (lbl) {
    Qt::Alignment qtAlign = Qt::AlignLeft | Qt::AlignVCenter;
    if (align == 1)
      qtAlign = Qt::AlignCenter;
    else if (align == 2)
      qtAlign = Qt::AlignRight | Qt::AlignVCenter;
    lbl->setAlignment(qtAlign);
  }
}

static api_bool mock_SetEventRoutine(control_handle h, api_handle receiver,
                                     void *routine) {
  qDebug() << "[PCL Bridge] SetEventRoutine: control =" << h
           << "receiver =" << receiver << "routine =" << routine;
  return true;
}

static api_bool
mock_SetSliderValueUpdatedEventRoutine(control_handle h, api_handle receiver,
                                       pcl::value_event_routine routine) {
  QSlider *slider = reinterpret_cast<QSlider *>(h);
  qDebug() << "[PCL Bridge] SetSliderValueUpdatedEventRoutine: slider ="
           << slider << "receiver =" << receiver << "routine =" << routine;
  if (slider) {
    slider->setProperty("valueUpdatedRoutine",
                        QVariant::fromValue(reinterpret_cast<void *>(routine)));
    slider->setProperty(
        "valueUpdatedReceiver",
        QVariant::fromValue(reinterpret_cast<void *>(receiver)));
    // Store the PCL sender (client) so the dispatcher can cast it back to
    // Slider*
    auto clientIt = g_widgetToClient.find(h);
    slider->setProperty("pclSender",
                        QVariant::fromValue(clientIt != g_widgetToClient.end()
                                                ? clientIt->second
                                                : (api_handle)h));

    slider->disconnect(slider, &QSlider::valueChanged, nullptr, nullptr);

    QObject::connect(slider, &QSlider::valueChanged, [slider](int value) {
      void *r = slider->property("valueUpdatedRoutine").value<void *>();
      void *rec = slider->property("valueUpdatedReceiver").value<void *>();
      void *snd = slider->property("pclSender").value<void *>();
      if (r) {
        auto callback = reinterpret_cast<pcl::value_event_routine>(r);
        callback(reinterpret_cast<control_handle>(snd),
                 reinterpret_cast<control_handle>(rec), value);
      }
    });
    return true;
  }
  return false;
}

static api_bool mock_SetEditCompletedEventRoutine(control_handle h,
                                                  api_handle receiver,
                                                  pcl::event_routine routine) {
  QLineEdit *edit = reinterpret_cast<QLineEdit *>(h);
  qDebug() << "[PCL Bridge] SetEditCompletedEventRoutine: edit =" << edit
           << "receiver =" << receiver << "routine =" << routine;
  if (edit) {
    edit->setProperty("completedRoutine",
                      QVariant::fromValue(reinterpret_cast<void *>(routine)));
    edit->setProperty("completedReceiver",
                      QVariant::fromValue(reinterpret_cast<void *>(receiver)));
    auto clientIt = g_widgetToClient.find(h);
    edit->setProperty("pclSender",
                      QVariant::fromValue(clientIt != g_widgetToClient.end()
                                              ? clientIt->second
                                              : (api_handle)h));

    edit->disconnect(edit, &QLineEdit::editingFinished, nullptr, nullptr);

    QObject::connect(edit, &QLineEdit::editingFinished, [edit]() {
      void *r = edit->property("completedRoutine").value<void *>();
      void *rec = edit->property("completedReceiver").value<void *>();
      void *snd = edit->property("pclSender").value<void *>();
      if (r) {
        auto callback = reinterpret_cast<pcl::event_routine>(r);
        callback(reinterpret_cast<control_handle>(snd),
                 reinterpret_cast<control_handle>(rec));
      }
    });
    return true;
  }
  return false;
}

static api_bool
mock_SetTextUpdatedEventRoutine(control_handle h, api_handle receiver,
                                pcl::unicode_event_routine routine) {
  QLineEdit *edit = reinterpret_cast<QLineEdit *>(h);
  qDebug() << "[PCL Bridge] SetTextUpdatedEventRoutine: edit =" << edit
           << "receiver =" << receiver << "routine =" << routine;
  if (edit) {
    edit->setProperty("textUpdatedRoutine",
                      QVariant::fromValue(reinterpret_cast<void *>(routine)));
    edit->setProperty("textUpdatedReceiver",
                      QVariant::fromValue(reinterpret_cast<void *>(receiver)));
    auto clientIt = g_widgetToClient.find(h);
    edit->setProperty("pclSender",
                      QVariant::fromValue(clientIt != g_widgetToClient.end()
                                              ? clientIt->second
                                              : (api_handle)h));

    edit->disconnect(edit, &QLineEdit::textChanged, nullptr, nullptr);

    QObject::connect(
        edit, &QLineEdit::textChanged, [edit](const QString &text) {
          void *r = edit->property("textUpdatedRoutine").value<void *>();
          void *rec = edit->property("textUpdatedReceiver").value<void *>();
          void *snd = edit->property("pclSender").value<void *>();
          if (r) {
            auto callback = reinterpret_cast<pcl::unicode_event_routine>(r);
            callback(reinterpret_cast<control_handle>(snd),
                     reinterpret_cast<control_handle>(rec),
                     reinterpret_cast<const char16_type *>(text.utf16()));
          }
        });
    return true;
  }
  return false;
}

static api_bool
mock_SetComboBoxItemSelectedEventRoutine(control_handle h, api_handle receiver,
                                         pcl::value_event_routine routine) {
  QComboBox *combo = reinterpret_cast<QComboBox *>(h);
  qDebug() << "[PCL Bridge] SetComboBoxItemSelectedEventRoutine: combo ="
           << combo << "receiver =" << receiver << "routine =" << routine;
  if (combo) {
    combo->setProperty("itemSelectedRoutine",
                       QVariant::fromValue(reinterpret_cast<void *>(routine)));
    combo->setProperty("itemSelectedReceiver",
                       QVariant::fromValue(reinterpret_cast<void *>(receiver)));
    auto clientIt = g_widgetToClient.find(h);
    combo->setProperty("pclSender",
                       QVariant::fromValue(clientIt != g_widgetToClient.end()
                                               ? clientIt->second
                                               : (api_handle)h));

    combo->disconnect(combo, &QComboBox::currentIndexChanged, nullptr, nullptr);

    QObject::connect(
        combo, &QComboBox::currentIndexChanged, [combo](int index) {
          void *r = combo->property("itemSelectedRoutine").value<void *>();
          void *rec = combo->property("itemSelectedReceiver").value<void *>();
          void *snd = combo->property("pclSender").value<void *>();
          if (r) {
            auto callback = reinterpret_cast<pcl::value_event_routine>(r);
            callback(reinterpret_cast<control_handle>(snd),
                     reinterpret_cast<control_handle>(rec), index);
          }
        });
    return true;
  }
  return false;
}

static api_bool
mock_SetButtonCheckEventRoutine(control_handle h, api_handle receiver,
                                pcl::button_check_event_routine routine) {
  QCheckBox *check = reinterpret_cast<QCheckBox *>(h);
  qDebug() << "[PCL Bridge] SetButtonCheckEventRoutine: check =" << check
           << "receiver =" << receiver << "routine =" << routine;
  if (check) {
    check->setProperty("checkRoutine",
                       QVariant::fromValue(reinterpret_cast<void *>(routine)));
    check->setProperty("checkReceiver",
                       QVariant::fromValue(reinterpret_cast<void *>(receiver)));
    auto clientIt = g_widgetToClient.find(h);
    check->setProperty("pclSender",
                       QVariant::fromValue(clientIt != g_widgetToClient.end()
                                               ? clientIt->second
                                               : (api_handle)h));

    check->disconnect(check, &QCheckBox::stateChanged, nullptr, nullptr);

    QObject::connect(check, &QCheckBox::stateChanged, [check](int state) {
      void *r = check->property("checkRoutine").value<void *>();
      void *rec = check->property("checkReceiver").value<void *>();
      void *snd = check->property("pclSender").value<void *>();
      if (r) {
        auto callback = reinterpret_cast<pcl::button_check_event_routine>(r);
        int32 pclState = 0;
        if (state == Qt::Checked)
          pclState = 1;
        else if (state == Qt::PartiallyChecked)
          pclState = 2;
        callback(reinterpret_cast<control_handle>(snd),
                 reinterpret_cast<control_handle>(rec), pclState);
      }
    });
    return true;
  }
  return false;
}

static api_bool
mock_SetButtonClickEventRoutine(control_handle h, api_handle receiver,
                                pcl::button_click_event_routine routine) {
  QPushButton *btn = reinterpret_cast<QPushButton *>(h);
  qDebug() << "[PCL Bridge] SetButtonClickEventRoutine: btn =" << btn
           << "receiver =" << receiver << "routine =" << routine;
  if (btn) {
    btn->setProperty("clickRoutine",
                     QVariant::fromValue(reinterpret_cast<void *>(routine)));
    btn->setProperty("clickReceiver",
                     QVariant::fromValue(reinterpret_cast<void *>(receiver)));
    auto clientIt = g_widgetToClient.find(h);
    btn->setProperty("pclSender",
                     QVariant::fromValue(clientIt != g_widgetToClient.end()
                                             ? clientIt->second
                                             : (api_handle)h));

    btn->disconnect(btn, &QPushButton::clicked, nullptr, nullptr);

    QObject::connect(btn, &QPushButton::clicked, [btn](bool checked) {
      void *r = btn->property("clickRoutine").value<void *>();
      void *rec = btn->property("clickReceiver").value<void *>();
      void *snd = btn->property("pclSender").value<void *>();
      if (r) {
        auto callback = reinterpret_cast<pcl::button_click_event_routine>(r);
        callback(reinterpret_cast<control_handle>(snd),
                 reinterpret_cast<control_handle>(rec), checked);
      }
    });
    return true;
  }
  return false;
}

static void mock_GetClientRect(const_control_handle h, int32 *x1, int32 *y1,
                               int32 *x2, int32 *y2) {
  const QWidget *w = reinterpret_cast<const QWidget *>(h);
  qDebug() << "[PCL Bridge] GetClientRect:" << w;
  if (w) {
    if (x1)
      *x1 = 0;
    if (y1)
      *y1 = 0;
    if (x2)
      *x2 = w->width();
    if (y2)
      *y2 = w->height();
  }
}

static void mock_SetControlFixedSize(control_handle h, int32 w, int32 h_size) {
  QWidget *widget = reinterpret_cast<QWidget *>(h);
  qDebug() << "[PCL Bridge] SetControlFixedSize:" << widget << "width =" << w
           << "height =" << h_size;
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
  QWidget *widget = reinterpret_cast<QWidget *>(h);
  qDebug() << "[PCL Bridge] SetControlBackgroundColor:" << widget
           << "color =" << color;
  if (widget) {
    QPalette pal = widget->palette();
    // Pack color is typically ARGB or RGBA. Let's assume standard QColor
    // construction from packed QRgb
    pal.setColor(QPalette::Window, QColor::fromRgba(color));
    widget->setAutoFillBackground(true);
    widget->setPalette(pal);
  }
}

static void mock_SetControlVisible(control_handle h, api_bool visible) {
  QWidget *widget = reinterpret_cast<QWidget *>(h);
  qDebug() << "[PCL Bridge] SetControlVisible:" << widget
           << "visible =" << (bool)visible;
  if (widget) {
    widget->setVisible(visible);
  }
}

static void mock_SetControlEnabled(control_handle h, api_bool enabled) {
  QWidget *widget = reinterpret_cast<QWidget *>(h);
  qDebug() << "[PCL Bridge] SetControlEnabled:" << widget
           << "enabled =" << (bool)enabled;
  if (widget) {
    widget->setEnabled(enabled);
  }
}

static void mock_EnsureControlLayoutUpdated(control_handle h) {
  QWidget *widget = reinterpret_cast<QWidget *>(h);
  qDebug() << "[PCL Bridge] EnsureControlLayoutUpdated:" << widget;
  if (widget) {
    widget->adjustSize();
    qApp->processEvents();
  }
}

static void mock_SetChildControlToFocus(control_handle h,
                                        control_handle child) {
  QWidget *widget = reinterpret_cast<QWidget *>(h);
  QWidget *childWidget = reinterpret_cast<QWidget *>(child);
  qDebug() << "[PCL Bridge] SetChildControlToFocus: parent =" << widget
           << "child =" << childWidget;
  if (childWidget) {
    childWidget->setFocus();
  }
}

#include <QRegularExpression>
#include <QRegularExpressionValidator>

static api_bool mock_SetEditValidatingRegExp(control_handle h,
                                             const char16_type *pattern,
                                             api_bool caseSensitive) {
  QLineEdit *edit = reinterpret_cast<QLineEdit *>(h);
  QString regexStr =
      pattern ? QString::fromUtf16(reinterpret_cast<const char16_t *>(pattern))
              : QString();
  qDebug() << "[PCL Bridge] SetEditValidatingRegExp:" << edit
           << "pattern =" << regexStr
           << "caseSensitive =" << (bool)caseSensitive;
  if (edit && !regexStr.isEmpty()) {
    QRegularExpression regex(
        regexStr, caseSensitive ? QRegularExpression::NoPatternOption
                                : QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionValidator *val =
        new QRegularExpressionValidator(regex, edit);
    edit->setValidator(val);
  }
  return true;
}

static void mock_SetSliderRange(control_handle h, int32 minVal, int32 maxVal) {
  QSlider *sl = reinterpret_cast<QSlider *>(h);
  qDebug() << "[PCL Bridge] SetSliderRange:" << sl << "range = [" << minVal
           << "," << maxVal << "]";
  if (sl) {
    sl->blockSignals(true);
    sl->setRange(minVal, maxVal);
    sl->blockSignals(false);
  }
}

static void mock_GetSliderRange(const_control_handle h, int32 *minVal,
                                int32 *maxVal) {
  const QSlider *sl = reinterpret_cast<const QSlider *>(h);
  if (sl) {
    if (minVal)
      *minVal = sl->minimum();
    if (maxVal)
      *maxVal = sl->maximum();
  }
  qDebug() << "[PCL Bridge] GetSliderRange:" << sl << "->"
           << (minVal ? *minVal : 0) << "," << (maxVal ? *maxVal : 0);
}

static void mock_SetSliderValue(control_handle h, int32 val) {
  QSlider *sl = reinterpret_cast<QSlider *>(h);
  qDebug() << "[PCL Bridge] SetSliderValue:" << sl << "value =" << val;
  if (sl) {
    sl->blockSignals(true);
    sl->setValue(val);
    sl->blockSignals(false);
  }
}

static int32 mock_GetSliderValue(const_control_handle h) {
  const QSlider *sl = reinterpret_cast<const QSlider *>(h);
  int32 val = sl ? sl->value() : 0;
  qDebug() << "[PCL Bridge] GetSliderValue:" << sl << "->" << val;
  return val;
}

static void mock_SetSliderStepSize(control_handle h, int32 step) {
  QSlider *sl = reinterpret_cast<QSlider *>(h);
  if (sl) {
    sl->setSingleStep(step);
  }
}

static int32 mock_GetSliderStepSize(const_control_handle h) {
  const QSlider *sl = reinterpret_cast<const QSlider *>(h);
  return sl ? sl->singleStep() : 1;
}

static void mock_SetSliderPageSize(control_handle h, int32 page) {
  QSlider *sl = reinterpret_cast<QSlider *>(h);
  if (sl) {
    sl->setPageStep(page);
  }
}

static int32 mock_GetSliderPageSize(const_control_handle h) {
  const QSlider *sl = reinterpret_cast<const QSlider *>(h);
  return sl ? sl->pageStep() : 10;
}

static void mock_SetSliderTickInterval(control_handle h, int32 interval) {
  QSlider *sl = reinterpret_cast<QSlider *>(h);
  if (sl) {
    sl->setTickInterval(interval);
  }
}

static int32 mock_GetSliderTickInterval(const_control_handle h) {
  const QSlider *sl = reinterpret_cast<const QSlider *>(h);
  return sl ? sl->tickInterval() : 0;
}

static void mock_SetSliderTickStyle(control_handle h, int32 style) {
  QSlider *sl = reinterpret_cast<QSlider *>(h);
  if (sl) {
    sl->setTickPosition(style ? QSlider::TicksBelow : QSlider::NoTicks);
  }
}

static int32 mock_GetSliderTickStyle(const_control_handle h) {
  const QSlider *sl = reinterpret_cast<const QSlider *>(h);
  return sl ? (sl->tickPosition() == QSlider::NoTicks ? 0 : 1) : 0;
}

static void mock_SetSliderTrackingEnabled(control_handle h, api_bool enable) {
  QSlider *sl = reinterpret_cast<QSlider *>(h);
  if (sl) {
    sl->setTracking(enable);
  }
}

static api_bool mock_GetSliderTrackingEnabled(const_control_handle h) {
  const QSlider *sl = reinterpret_cast<const QSlider *>(h);
  return sl ? sl->hasTracking() : true;
}

static api_bool mock_GetControlDisplayPixelRatio(const_control_handle h,
                                                 double *ratio) {
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

static int32 mock_GetStringPixelWidth(const_font_handle f,
                                      const char16_type *text) {
  QString str =
      text ? QString::fromUtf16(reinterpret_cast<const char16_t *>(text))
           : QString();
  QFont font = QApplication::font();
  QFontMetrics fm(font);
  int32 w = fm.horizontalAdvance(str);
  qDebug() << "[PCL Bridge] GetStringPixelWidth: text =" << str << "->" << w;
  return w;
}

static int32 mock_GetComboBoxLength(const_control_handle h) {
  const QComboBox *cb = reinterpret_cast<const QComboBox *>(h);
  int32 len = cb ? cb->count() : 0;
  qDebug() << "[PCL Bridge] GetComboBoxLength:" << cb << "->" << len;
  return len;
}

static void mock_SetButtonChecked(control_handle h, uint32 checked) {
  QAbstractButton *btn = reinterpret_cast<QAbstractButton *>(h);
  qDebug() << "[PCL Bridge] SetButtonChecked:" << btn << "checked =" << checked;
  if (btn) {
    btn->blockSignals(true);
    btn->setChecked(checked != 0);
    btn->blockSignals(false);
  }
}

static uint32 mock_GetButtonChecked(const_control_handle h) {
  const QAbstractButton *btn = reinterpret_cast<const QAbstractButton *>(h);
  uint32 val = btn ? (btn->isChecked() ? 1 : 0) : 0;
  qDebug() << "[PCL Bridge] GetButtonChecked:" << btn << "->" << val;
  return val;
}

static void mock_SetWindowTitle(control_handle h, const char16_type *title) {
  QWidget *w = reinterpret_cast<QWidget *>(h);
  QString str =
      title ? QString::fromUtf16(reinterpret_cast<const char16_t *>(title))
            : QString();
  qDebug() << "[PCL Bridge] SetWindowTitle:" << w << "title =" << str;
  if (w) {
    w->setWindowTitle(str);
  }
}

static void mock_SetWindowToolTip(control_handle h, const char16_type *text) {
  QWidget *w = reinterpret_cast<QWidget *>(h);
  QString str =
      text ? QString::fromUtf16(reinterpret_cast<const char16_t *>(text))
           : QString();
  qDebug() << "[PCL Bridge] SetWindowToolTip:" << w << "text =" << str;
  if (w) {
    w->setToolTip(str);
  }
}

static void mock_GetControlPosition(const_control_handle h, int32 *x,
                                    int32 *y) {
  const QWidget *w = reinterpret_cast<const QWidget *>(h);
  if (w) {
    if (x)
      *x = w->x();
    if (y)
      *y = w->y();
  }
  qDebug() << "[PCL Bridge] GetControlPosition:" << w << "->" << (x ? *x : 0)
           << "," << (y ? *y : 0);
}

static void mock_SetControlPosition(control_handle h, int32 x, int32 y) {
  QWidget *w = reinterpret_cast<QWidget *>(h);
  qDebug() << "[PCL Bridge] SetControlPosition:" << w << "x =" << x
           << "y =" << y;
  if (w) {
    w->move(x, y);
  }
}

static void mock_SetControlFocusStyle(control_handle h, int32 style) {
  QWidget *widget = reinterpret_cast<QWidget *>(h);
  qDebug() << "[PCL Bridge] SetControlFocusStyle:" << widget << "style =" << style;
}

static int32 mock_GetCharPixelWidth(const_font_handle f, int32 charCode) {
  QString str = QString(QChar(static_cast<char32_t>(charCode)));
  QFont font = QApplication::font();
  QFontMetrics fm(font);
  int32 w = fm.horizontalAdvance(str);
  qDebug() << "[PCL Bridge] GetCharPixelWidth: charCode =" << charCode << "->" << w;
  return w;
}

static void mock_SetSizerControlAlignment(sizer_handle s, control_handle c,
                                          int32 alignment) {
  QBoxLayout *lay = reinterpret_cast<QBoxLayout *>(s);
  QWidget *w = reinterpret_cast<QWidget *>(c);
  qDebug() << "[PCL Bridge] SetSizerControlAlignment: layout =" << lay
           << "widget =" << w << "alignment =" << alignment;
  if (lay && w) {
    lay->setAlignment(w, Qt::Alignment(alignment));
  }
}

static void mock_GetControlMinSize(const_control_handle h, int32 *w, int32 *h_size) {
  const QWidget *widget = reinterpret_cast<const QWidget *>(h);
  if (widget) {
    if (w)
      *w = widget->minimumWidth();
    if (h_size)
      *h_size = widget->minimumHeight();
  } else {
    if (w)
      *w = 0;
    if (h_size)
      *h_size = 0;
  }
  qDebug() << "[PCL Bridge] GetControlMinSize:" << widget
           << "->" << (w ? *w : 0) << "," << (h_size ? *h_size : 0);
}

static void mock_GetControlMaxSize(const_control_handle h, int32 *w, int32 *h_size) {
  const QWidget *widget = reinterpret_cast<const QWidget *>(h);
  if (widget) {
    if (w)
      *w = widget->maximumWidth();
    if (h_size)
      *h_size = widget->maximumHeight();
  } else {
    if (w)
      *w = 0;
    if (h_size)
      *h_size = 0;
  }
  qDebug() << "[PCL Bridge] GetControlMaxSize:" << widget
           << "->" << (w ? *w : 0) << "," << (h_size ? *h_size : 0);
}

static void mock_SetControlSize(control_handle h, int32 w, int32 h_size) {
  QWidget *widget = reinterpret_cast<QWidget *>(h);
  qDebug() << "[PCL Bridge] SetControlSize:" << widget << "width =" << w
           << "height =" << h_size;
  if (widget) {
    if (w >= 0 && h_size >= 0) {
      widget->resize(w, h_size);
    } else if (w >= 0) {
      widget->resize(w, widget->height());
    } else if (h_size >= 0) {
      widget->resize(widget->width(), h_size);
    }
  }
}

static api_bool mock_GetControlVisible(const_control_handle h) {
  const QWidget *widget = reinterpret_cast<const QWidget *>(h);
  bool visible = widget ? widget->isVisible() : false;
  qDebug() << "[PCL Bridge] GetControlVisible:" << widget << "->" << visible;
  return visible;
}

static api_bool mock_GetViewId(const_view_handle hView, char *id,
                               pcl::size_type *length) {
  if (!hView)
    return false;
  const auto *view = reinterpret_cast<const PCLViewMock *>(hView);
  std::string str = view->id.toStdString();
  qDebug() << "[PCL Bridge] GetViewId/FullId: view =" << view
           << "id =" << view->id;
  if (!length)
    return false;
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

static void mock_GetViewLocks(const_view_handle hView, api_bool *readLocked,
                              api_bool *writeLocked) {
  qDebug() << "[PCL Bridge] mock_GetViewLocks called: view =" << hView;
  if (readLocked)
    *readLocked = true;
  if (writeLocked)
    *writeLocked = true;
}

// ----------------------------------------------------------------------------
// PCLBridge Implementation
// ----------------------------------------------------------------------------

PCLBridge::PCLBridge(QObject *parent) : QObject(parent) {
  g_pclBridgeInstance = this;
  g_verbosePCL = (qEnvironmentVariableIntValue("BLASTRO_DEBUG_PCL") == 1);
}

PCLBridge::~PCLBridge() {
  unloadModule();
  if (g_pclBridgeInstance == this) {
    g_pclBridgeInstance = nullptr;
  }
}

void PCLBridge::setupOverrides() {
  overridePCLStub("Global/GetPixelTraitsLUT",
                  reinterpret_cast<void *>(mock_GetPixelTraitsLUT));
  overridePCLStub("Global/GetPixInsightVersion",
                  reinterpret_cast<void *>(mock_GetPixInsightVersion));
  overridePCLStub("Global/Allocate", reinterpret_cast<void *>(mock_Allocate));
  overridePCLStub("Global/Deallocate",
                  reinterpret_cast<void *>(mock_Deallocate));
  overridePCLStub("Global/GetConsole",
                  reinterpret_cast<void *>(mock_GetConsole));
  overridePCLStub("Global/ValidateConsole",
                  reinterpret_cast<void *>(mock_ValidateConsole));
  overridePCLStub("Global/WriteConsole",
                  reinterpret_cast<void *>(mock_WriteConsole));
  overridePCLStub("Global/FlushConsole",
                  reinterpret_cast<void *>(mock_FlushConsole));
  overridePCLStub("Global/ShowConsole",
                  reinterpret_cast<void *>(mock_ShowConsole));
  overridePCLStub("Global/ProcessEvents",
                  reinterpret_cast<void *>(mock_ProcessEvents));
  overridePCLStub("Global/Abort", reinterpret_cast<void *>(mock_Abort));
  overridePCLStub("Global/DisableAbort",
                  reinterpret_cast<void *>(mock_DisableAbort));
  overridePCLStub("Global/EnableAbort",
                  reinterpret_cast<void *>(mock_EnableAbort));

  overridePCLStub("Process/GetParameterValue",
                  reinterpret_cast<void *>(mock_GetParameterValue));
  overridePCLStub("Process/SetParameterValue",
                  reinterpret_cast<void *>(mock_SetParameterValue));

  overridePCLStub("ModuleDefinition/EnterModuleDefinitionContext",
                  reinterpret_cast<void *>(mock_EnterModuleDefinitionContext));
  overridePCLStub(
      "ModuleDefinition/IsModuleDefinitionContextActive",
      reinterpret_cast<void *>(mock_IsModuleDefinitionContextActive));
  overridePCLStub("ModuleDefinition/ExitModuleDefinitionContext",
                  reinterpret_cast<void *>(mock_ExitModuleDefinitionContext));
  overridePCLStub("ModuleDefinition/SetModuleOnLoadRoutine",
                  reinterpret_cast<void *>(mock_SetModuleOnLoadRoutine));
  overridePCLStub("ModuleDefinition/SetModuleOnUnloadRoutine",
                  reinterpret_cast<void *>(mock_SetModuleOnUnloadRoutine));

  overridePCLStub("ProcessDefinition/EnterProcessDefinitionContext",
                  reinterpret_cast<void *>(mock_EnterProcessDefinitionContext));
  overridePCLStub(
      "ProcessDefinition/IsProcessDefinitionContextActive",
      reinterpret_cast<void *>(mock_IsProcessDefinitionContextActive));
  overridePCLStub("ProcessDefinition/BeginProcessDefinition",
                  reinterpret_cast<void *>(mock_BeginProcessDefinition));
  overridePCLStub("ProcessDefinition/SetProcessCategory",
                  reinterpret_cast<void *>(mock_SetProcessCategory));
  overridePCLStub("ProcessDefinition/SetProcessCreationRoutine",
                  reinterpret_cast<void *>(mock_SetProcessCreationRoutine));
  overridePCLStub("ProcessDefinition/SetProcessDestructionRoutine",
                  reinterpret_cast<void *>(mock_SetProcessDestructionRoutine));
  overridePCLStub(
      "ProcessDefinition/SetProcessInitializationRoutine",
      reinterpret_cast<void *>(mock_SetProcessInitializationRoutine));
  overridePCLStub("ProcessDefinition/SetProcessExecutionRoutine",
                  reinterpret_cast<void *>(mock_SetProcessExecutionRoutine));
  overridePCLStub(
      "ProcessDefinition/SetProcessGlobalExecutionRoutine",
      reinterpret_cast<void *>(mock_SetProcessGlobalExecutionRoutine));
  overridePCLStub(
      "ProcessDefinition/SetProcessDefaultInterfaceSelectionRoutine",
      reinterpret_cast<void *>(
          mock_SetProcessDefaultInterfaceSelectionRoutine));
  overridePCLStub(
      "ProcessDefinition/SetProcessInterfaceSelectionRoutine",
      reinterpret_cast<void *>(mock_SetProcessInterfaceSelectionRoutine));
  overridePCLStub("ProcessDefinition/EndProcessDefinition",
                  reinterpret_cast<void *>(mock_EndProcessDefinition));
  overridePCLStub("ProcessDefinition/ExitProcessDefinitionContext",
                  reinterpret_cast<void *>(mock_ExitProcessDefinitionContext));

  overridePCLStub("InterfaceDefinition/BeginInterfaceDefinition",
                  reinterpret_cast<void *>(mock_BeginInterfaceDefinition));
  overridePCLStub(
      "InterfaceDefinition/SetInterfaceInitializationRoutine",
      reinterpret_cast<void *>(mock_SetInterfaceInitializationRoutine));
  overridePCLStub("InterfaceDefinition/SetInterfaceLaunchRoutine",
                  reinterpret_cast<void *>(mock_SetInterfaceLaunchRoutine));
  overridePCLStub("InterfaceDefinition/EndInterfaceDefinition",
                  reinterpret_cast<void *>(mock_EndInterfaceDefinition));

  overridePCLStub("Control/CreateControl",
                  reinterpret_cast<void *>(mock_CreateControl));
  overridePCLStub("Control/SetControlParent",
                  reinterpret_cast<void *>(mock_SetControlParent));
  overridePCLStub("Control/SetControlSizer",
                  reinterpret_cast<void *>(mock_SetControlSizer));
  overridePCLStub("Control/SetControlMinSize",
                  reinterpret_cast<void *>(mock_SetControlMinSize));
  overridePCLStub("Control/SetControlMaxSize",
                  reinterpret_cast<void *>(mock_SetControlMaxSize));
  overridePCLStub("Control/AdjustControlToContents",
                  reinterpret_cast<void *>(mock_AdjustControlToContents));
  overridePCLStub("Control/GetClientRect",
                  reinterpret_cast<void *>(mock_GetClientRect));
  overridePCLStub("Control/SetControlFixedSize",
                  reinterpret_cast<void *>(mock_SetControlFixedSize));
  overridePCLStub("Control/GetControlDisplayPixelRatio",
                  reinterpret_cast<void *>(mock_GetControlDisplayPixelRatio));
  overridePCLStub("Control/SetControlBackgroundColor",
                  reinterpret_cast<void *>(mock_SetControlBackgroundColor));
  overridePCLStub("Control/GetControlFont",
                  reinterpret_cast<void *>(mock_GetControlFont));
  overridePCLStub("Control/SetWindowTitle",
                  reinterpret_cast<void *>(mock_SetWindowTitle));
  overridePCLStub("Control/SetWindowToolTip",
                  reinterpret_cast<void *>(mock_SetWindowToolTip));
  overridePCLStub("Control/GetControlPosition",
                  reinterpret_cast<void *>(mock_GetControlPosition));
  overridePCLStub("Control/SetControlPosition",
                  reinterpret_cast<void *>(mock_SetControlPosition));
  overridePCLStub("Control/SetControlVisible",
                  reinterpret_cast<void *>(mock_SetControlVisible));
  overridePCLStub("Control/SetControlEnabled",
                  reinterpret_cast<void *>(mock_SetControlEnabled));
  overridePCLStub("Control/EnsureControlLayoutUpdated",
                  reinterpret_cast<void *>(mock_EnsureControlLayoutUpdated));
  overridePCLStub("Control/SetControlFocusStyle",
                  reinterpret_cast<void *>(mock_SetControlFocusStyle));
  overridePCLStub("Font/GetStringPixelWidth",
                  reinterpret_cast<void *>(mock_GetStringPixelWidth));
  overridePCLStub("Font/GetCharPixelWidth",
                  reinterpret_cast<void *>(mock_GetCharPixelWidth));
  overridePCLStub("Sizer/SetSizerControlAlignment",
                  reinterpret_cast<void *>(mock_SetSizerControlAlignment));
  overridePCLStub("Control/GetControlMinSize",
                  reinterpret_cast<void *>(mock_GetControlMinSize));
  overridePCLStub("Control/GetControlMaxSize",
                  reinterpret_cast<void *>(mock_GetControlMaxSize));
  overridePCLStub("Control/SetControlSize",
                  reinterpret_cast<void *>(mock_SetControlSize));
  overridePCLStub("Control/GetControlVisible",
                  reinterpret_cast<void *>(mock_GetControlVisible));
  overridePCLStub("Control/SetChildControlToFocus",
                  reinterpret_cast<void *>(mock_SetChildControlToFocus));
  overridePCLStub("Control/SetChildCreateEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetChildDestroyEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetCloseEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetDestroyEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetEnterEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetFileDragEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetFileDropEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetGetFocusEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetHideEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetKeyPressEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetKeyReleaseEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetLeaveEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetLoseFocusEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetMouseDoubleClickEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetMouseMoveEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetMousePressEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetMouseReleaseEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetMoveEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetPaintEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetResizeEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetShowEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetViewDragEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetViewDropEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Control/SetWheelEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));

  overridePCLStub("Dialog/CreateDialog",
                  reinterpret_cast<void *>(mock_CreateDialog));

  overridePCLStub("Sizer/CreateSizer",
                  reinterpret_cast<void *>(mock_CreateSizer));
  overridePCLStub("Sizer/InsertSizerControl",
                  reinterpret_cast<void *>(mock_InsertSizerControl));
  overridePCLStub("Sizer/InsertSizer",
                  reinterpret_cast<void *>(mock_InsertSizer));
  overridePCLStub("Sizer/InsertSizerSpacing",
                  reinterpret_cast<void *>(mock_InsertSizerSpacing));
  overridePCLStub("Sizer/InsertSizerStretch",
                  reinterpret_cast<void *>(mock_InsertSizerStretch));
  overridePCLStub("Sizer/SetSizerMargin",
                  reinterpret_cast<void *>(mock_SetSizerMargin));
  overridePCLStub("Sizer/SetSizerSpacing",
                  reinterpret_cast<void *>(mock_SetSizerSpacing));
  overridePCLStub("Sizer/GetSizerDisplayPixelRatio",
                  reinterpret_cast<void *>(mock_GetSizerDisplayPixelRatio));

  overridePCLStub("GroupBox/CreateGroupBox",
                  reinterpret_cast<void *>(mock_CreateGroupBox));
  overridePCLStub("GroupBox/SetGroupBoxCheckEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));

  overridePCLStub("Button/CreatePushButton",
                  reinterpret_cast<void *>(mock_CreatePushButton));
  overridePCLStub("Button/CreateToolButton",
                  reinterpret_cast<void *>(mock_CreateToolButton));
  overridePCLStub("Button/SetToolButtonCheckable",
                  reinterpret_cast<void *>(mock_SetToolButtonCheckable));
  overridePCLStub("Button/GetToolButtonCheckable",
                  reinterpret_cast<void *>(mock_GetToolButtonCheckable));
  overridePCLStub("Button/CreateCheckBox",
                  reinterpret_cast<void *>(mock_CreateCheckBox));
  overridePCLStub("Button/SetButtonText",
                  reinterpret_cast<void *>(mock_SetButtonText));
  overridePCLStub("Button/SetButtonCheckEventRoutine",
                  reinterpret_cast<void *>(mock_SetButtonCheckEventRoutine));
  overridePCLStub("Button/SetButtonChecked",
                  reinterpret_cast<void *>(mock_SetButtonChecked));
  overridePCLStub("Button/GetButtonChecked",
                  reinterpret_cast<void *>(mock_GetButtonChecked));
  overridePCLStub("Button/SetButtonClickEventRoutine",
                  reinterpret_cast<void *>(mock_SetButtonClickEventRoutine));
  overridePCLStub("Button/SetButtonPressEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Button/SetButtonReleaseEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));

  overridePCLStub("ComboBox/CreateComboBox",
                  reinterpret_cast<void *>(mock_CreateComboBox));
  overridePCLStub("ComboBox/InsertComboBoxItem",
                  reinterpret_cast<void *>(mock_InsertComboBoxItem));
  overridePCLStub("ComboBox/ClearComboBox",
                  reinterpret_cast<void *>(mock_ClearComboBox));
  overridePCLStub("ComboBox/SetComboBoxCurrentItem",
                  reinterpret_cast<void *>(mock_SetComboBoxCurrentItem));
  overridePCLStub("ComboBox/GetComboBoxCurrentItem",
                  reinterpret_cast<void *>(mock_GetComboBoxCurrentItem));
  overridePCLStub("ComboBox/SetComboBoxEditTextUpdatedEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("ComboBox/SetComboBoxItemHighlightedEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub(
      "ComboBox/SetComboBoxItemSelectedEventRoutine",
      reinterpret_cast<void *>(mock_SetComboBoxItemSelectedEventRoutine));
  overridePCLStub("ComboBox/GetComboBoxLength",
                  reinterpret_cast<void *>(mock_GetComboBoxLength));

  overridePCLStub("SpinBox/CreateSpinBox",
                  reinterpret_cast<void *>(mock_CreateSpinBox));
  overridePCLStub("SpinBox/SetSpinBoxRange",
                  reinterpret_cast<void *>(mock_SetSpinBoxRange));
  overridePCLStub("SpinBox/SetSpinBoxValue",
                  reinterpret_cast<void *>(mock_SetSpinBoxValue));
  overridePCLStub("SpinBox/GetSpinBoxValue",
                  reinterpret_cast<void *>(mock_GetSpinBoxValue));

  overridePCLStub("Label/CreateLabel",
                  reinterpret_cast<void *>(mock_CreateLabel));
  overridePCLStub("Label/SetLabelText",
                  reinterpret_cast<void *>(mock_SetLabelText));
  overridePCLStub("Label/SetLabelAlignment",
                  reinterpret_cast<void *>(mock_SetLabelAlignment));

  overridePCLStub("Slider/CreateSlider",
                  reinterpret_cast<void *>(mock_CreateSlider));
  overridePCLStub("Slider/GetSliderValue",
                  reinterpret_cast<void *>(mock_GetSliderValue));
  overridePCLStub("Slider/SetSliderValue",
                  reinterpret_cast<void *>(mock_SetSliderValue));
  overridePCLStub("Slider/GetSliderRange",
                  reinterpret_cast<void *>(mock_GetSliderRange));
  overridePCLStub("Slider/SetSliderRange",
                  reinterpret_cast<void *>(mock_SetSliderRange));
  overridePCLStub("Slider/GetSliderStepSize",
                  reinterpret_cast<void *>(mock_GetSliderStepSize));
  overridePCLStub("Slider/SetSliderStepSize",
                  reinterpret_cast<void *>(mock_SetSliderStepSize));
  overridePCLStub("Slider/GetSliderPageSize",
                  reinterpret_cast<void *>(mock_GetSliderPageSize));
  overridePCLStub("Slider/SetSliderPageSize",
                  reinterpret_cast<void *>(mock_SetSliderPageSize));
  overridePCLStub("Slider/GetSliderTickInterval",
                  reinterpret_cast<void *>(mock_GetSliderTickInterval));
  overridePCLStub("Slider/SetSliderTickInterval",
                  reinterpret_cast<void *>(mock_SetSliderTickInterval));
  overridePCLStub("Slider/GetSliderTickStyle",
                  reinterpret_cast<void *>(mock_GetSliderTickStyle));
  overridePCLStub("Slider/SetSliderTickStyle",
                  reinterpret_cast<void *>(mock_SetSliderTickStyle));
  overridePCLStub("Slider/GetSliderTrackingEnabled",
                  reinterpret_cast<void *>(mock_GetSliderTrackingEnabled));
  overridePCLStub("Slider/SetSliderTrackingEnabled",
                  reinterpret_cast<void *>(mock_SetSliderTrackingEnabled));
  overridePCLStub(
      "Slider/SetSliderValueUpdatedEventRoutine",
      reinterpret_cast<void *>(mock_SetSliderValueUpdatedEventRoutine));
  overridePCLStub("Slider/SetSliderRangeUpdatedEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));

  overridePCLStub("Edit/CreateEdit", reinterpret_cast<void *>(mock_CreateEdit));
  overridePCLStub("Edit/GetEditText",
                  reinterpret_cast<void *>(mock_GetEditText));
  overridePCLStub("Edit/SetEditText",
                  reinterpret_cast<void *>(mock_SetEditText));
  overridePCLStub("Edit/GetEditReadOnly",
                  reinterpret_cast<void *>(mock_GetEditReadOnly));
  overridePCLStub("Edit/SetEditReadOnly",
                  reinterpret_cast<void *>(mock_SetEditReadOnly));
  overridePCLStub("Edit/SetCaretPositionUpdatedEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Edit/SetEditCompletedEventRoutine",
                  reinterpret_cast<void *>(mock_SetEditCompletedEventRoutine));
  overridePCLStub("Edit/SetReturnPressedEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Edit/SetSelectionUpdatedEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Edit/SetTextUpdatedEventRoutine",
                  reinterpret_cast<void *>(mock_SetTextUpdatedEventRoutine));
  overridePCLStub("Edit/SetEditValidatingRegExp",
                  reinterpret_cast<void *>(mock_SetEditValidatingRegExp));

  overridePCLStub("Dialog/SetExecuteDialogEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("Dialog/SetReturnDialogEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));

  overridePCLStub("UI/AttachToUIObject",
                  reinterpret_cast<void *>(mock_AttachToUIObject));
  overridePCLStub("UI/DetachFromUIObject",
                  reinterpret_cast<void *>(mock_DetachFromUIObject));
  overridePCLStub("UI/GetUIObjectRefCount",
                  reinterpret_cast<void *>(mock_GetUIObjectRefCount));
  overridePCLStub("Thread/CreateThread",
                  reinterpret_cast<void *>(mock_CreateThread));
  overridePCLStub("Thread/StartThread",
                  reinterpret_cast<void *>(mock_StartThread));
  overridePCLStub("Thread/KillThread",
                  reinterpret_cast<void *>(mock_KillThread));
  overridePCLStub("Thread/IsThreadActive",
                  reinterpret_cast<void *>(mock_IsThreadActive));
  overridePCLStub("Thread/WaitThread",
                  reinterpret_cast<void *>(mock_WaitThread));
  overridePCLStub("Thread/SleepThread",
                  reinterpret_cast<void *>(mock_SleepThread));
  overridePCLStub("Thread/GetThreadStatus",
                  reinterpret_cast<void *>(mock_GetThreadStatus));
  overridePCLStub("Thread/SetThreadStatus",
                  reinterpret_cast<void *>(mock_SetThreadStatus));
  overridePCLStub("Thread/GetCurrentThread",
                  reinterpret_cast<void *>(mock_GetCurrentThread));
  overridePCLStub("Thread/SetThreadExecRoutine",
                  reinterpret_cast<void *>(mock_SetThreadExecRoutine));
  overridePCLStub("Thread/AppendThreadConsoleOutputText",
                  reinterpret_cast<void *>(mock_AppendThreadConsoleOutputText));

  overridePCLStub("SharedImage/CreateImage",
                  reinterpret_cast<void *>(mock_CreateImage));
  overridePCLStub("SharedImage/GetImageGeometry",
                  reinterpret_cast<void *>(mock_GetImageGeometry));
  overridePCLStub("SharedImage/GetImageFormat",
                  reinterpret_cast<void *>(mock_GetImageFormat));
  overridePCLStub("SharedImage/GetImagePixelData",
                  reinterpret_cast<void *>(mock_GetImagePixelData));
  overridePCLStub("SharedImage/SetImagePixelData",
                  reinterpret_cast<void *>(mock_SetImagePixelData));
  overridePCLStub("SharedImage/SetImageGeometry",
                  reinterpret_cast<void *>(mock_SetImageGeometry));
  overridePCLStub("SharedImage/SetImageColorSpace",
                  reinterpret_cast<void *>(mock_SetImageColorSpace));
  overridePCLStub("SharedImage/GetImageColorSpace",
                  reinterpret_cast<void *>(mock_GetImageColorSpace));
  overridePCLStub("SharedImage/AttachToImage",
                  reinterpret_cast<void *>(mock_AttachToImage));
  overridePCLStub("SharedImage/GetImageRGBWS",
                  reinterpret_cast<void *>(mock_GetImageRGBWS));
  overridePCLStub("SharedImage/DetachFromImage",
                  reinterpret_cast<void *>(mock_DetachFromImage));

  overridePCLStub("View/GetViewImage",
                  reinterpret_cast<void *>(mock_GetViewImage));
  overridePCLStub("View/GetViewId", reinterpret_cast<void *>(mock_GetViewId));
  overridePCLStub("View/GetViewFullId",
                  reinterpret_cast<void *>(mock_GetViewId));
  overridePCLStub("View/GetViewLocks",
                  reinterpret_cast<void *>(mock_GetViewLocks));
  overridePCLStub("View/LockView", reinterpret_cast<void *>(mock_LockView));
  overridePCLStub("View/UnlockView", reinterpret_cast<void *>(mock_UnlockView));
  overridePCLStub(
      "View/GetViewScreenTransferFunctions",
      reinterpret_cast<void *>(mock_GetViewScreenTransferFunctions));
  overridePCLStub(
      "View/GetViewScreenTransferFunctionsEnabled",
      reinterpret_cast<void *>(mock_GetViewScreenTransferFunctionsEnabled));
  overridePCLStub("View/GetViewPropertyExists",
                  reinterpret_cast<void *>(mock_GetViewPropertyExists));
  overridePCLStub("View/ComputeViewProperty",
                  reinterpret_cast<void *>(mock_ComputeViewProperty));
  overridePCLStub("View/GetViewPropertyValue",
                  reinterpret_cast<void *>(mock_GetViewPropertyValue));
  overridePCLStub("View/SetViewPropertyValue",
                  reinterpret_cast<void *>(mock_SetViewPropertyValue));
  overridePCLStub("View/GetViewParentWindow",
                  reinterpret_cast<void *>(mock_GetViewParentWindow));
  overridePCLStub("ImageWindow/GetImageWindowMainView",
                  reinterpret_cast<void *>(mock_GetImageWindowMainView));
  overridePCLStub("ImageWindow/GetImageWindowCurrentView",
                  reinterpret_cast<void *>(mock_GetImageWindowCurrentView));

  // Global Settings & Preferences
  overridePCLStub("Global/ReadSettingsString",
                  reinterpret_cast<void *>(mock_ReadSettingsString));
  overridePCLStub("Global/WriteSettingsString",
                  reinterpret_cast<void *>(mock_WriteSettingsString));
  overridePCLStub("Global/ReadSettingsFlag",
                  reinterpret_cast<void *>(mock_ReadSettingsFlag));
  overridePCLStub("Global/WriteSettingsFlag",
                  reinterpret_cast<void *>(mock_WriteSettingsFlag));
  overridePCLStub("Global/ReadSettingsReal",
                  reinterpret_cast<void *>(mock_ReadSettingsReal));
  overridePCLStub("Global/WriteSettingsReal",
                  reinterpret_cast<void *>(mock_WriteSettingsReal));
  overridePCLStub("Global/ReadSettingsInteger",
                  reinterpret_cast<void *>(mock_ReadSettingsInteger));
  overridePCLStub("Global/WriteSettingsInteger",
                  reinterpret_cast<void *>(mock_WriteSettingsInteger));
  overridePCLStub("Global/ReadSettingsUnsignedInteger",
                  reinterpret_cast<void *>(mock_ReadSettingsUnsignedInteger));
  overridePCLStub("Global/WriteSettingsUnsignedInteger",
                  reinterpret_cast<void *>(mock_WriteSettingsUnsignedInteger));
  overridePCLStub("Global/GetGlobalString",
                  reinterpret_cast<void *>(mock_GetGlobalString));
  overridePCLStub("Global/GetGlobalInteger",
                  reinterpret_cast<void *>(mock_GetGlobalInteger));
  overridePCLStub(
      "InterfaceDefinition/SetInterfaceEditPreferencesRoutine",
      reinterpret_cast<void *>(mock_SetInterfaceEditPreferencesRoutine));
  overridePCLStub(
      "InterfaceDefinition/SetInterfaceProcessImportRoutine",
      reinterpret_cast<void *>(mock_SetInterfaceProcessImportRoutine));
  overridePCLStub(
      "InterfaceDefinition/SetGlobalPreferencesUpdatedNotificationRoutine",
      reinterpret_cast<void *>(
          mock_SetGlobalPreferencesUpdatedNotificationRoutine));
  overridePCLStub(
      "InterfaceDefinition/SetInterfaceProcessValidationRoutine",
      reinterpret_cast<void *>(mock_SetInterfaceProcessValidationRoutine));
  overridePCLStub(
      "ProcessDefinition/SetProcessInterfaceValidationRoutine",
      reinterpret_cast<void *>(mock_SetProcessInterfaceValidationRoutine));

  // Control scaling & ViewList controls
  overridePCLStub("Control/GetControlResourcePixelRatio",
                  reinterpret_cast<void *>(mock_GetControlResourcePixelRatio));
  overridePCLStub("ViewList/CreateViewList",
                  reinterpret_cast<void *>(mock_CreateViewList));
  overridePCLStub("ViewList/RegenerateViewList",
                  reinterpret_cast<void *>(mock_RegenerateViewList));
  overridePCLStub("ViewList/GetViewListContents",
                  reinterpret_cast<void *>(mock_GetViewListContents));
  overridePCLStub("ViewList/SetViewListViewSelectedEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));
  overridePCLStub("ViewList/SetViewListCurrentViewUpdatedEventRoutine",
                  reinterpret_cast<void *>(mock_SetEventRoutine));

  // Dialog execution & validation hooks
  overridePCLStub("Dialog/ExecuteDialog",
                  reinterpret_cast<void *>(mock_ExecuteDialog));
  overridePCLStub("Dialog/ReturnDialog",
                  reinterpret_cast<void *>(mock_ReturnDialog));
  overridePCLStub("Edit/GetEditValid",
                  reinterpret_cast<void *>(mock_GetEditValid));
  overridePCLStub("Button/SetButtonDefaultEnabled",
                  reinterpret_cast<void *>(mock_SetButtonDefaultEnabled));

  // File / directory picker dialogs
  overridePCLStub("Dialog/ExecuteOpenFileDialog",
                  reinterpret_cast<void *>(mock_ExecuteOpenFileDialog));
  overridePCLStub(
      "Dialog/ExecuteOpenMultipleFilesDialog",
      reinterpret_cast<void *>(mock_ExecuteOpenMultipleFilesDialog));
  overridePCLStub("Dialog/ExecuteSaveFileDialog",
                  reinterpret_cast<void *>(mock_ExecuteSaveFileDialog));
  overridePCLStub("Dialog/ExecuteGetDirectoryDialog",
                  reinterpret_cast<void *>(mock_ExecuteGetDirectoryDialog));

  // Process & Parameter definitions
  overridePCLStub("ProcessDefinition/BeginParameterDefinition",
                  reinterpret_cast<void *>(mock_BeginParameterDefinition));
  overridePCLStub(
      "ProcessDefinition/SetParameterProcessVersionRange",
      reinterpret_cast<void *>(mock_SetParameterProcessVersionRange));
  overridePCLStub("ProcessDefinition/SetParameterRequired",
                  reinterpret_cast<void *>(mock_SetParameterRequired));
  overridePCLStub("ProcessDefinition/SetParameterReadOnly",
                  reinterpret_cast<void *>(mock_SetParameterReadOnly));
  overridePCLStub("ProcessDefinition/SetParameterLockRoutine",
                  reinterpret_cast<void *>(mock_SetParameterLockRoutine));
  overridePCLStub("ProcessDefinition/SetParameterAllocationRoutine",
                  reinterpret_cast<void *>(mock_SetParameterAllocationRoutine));
  overridePCLStub(
      "ProcessDefinition/SetParameterLengthQueryRoutine",
      reinterpret_cast<void *>(mock_SetParameterLengthQueryRoutine));
  overridePCLStub("ProcessDefinition/SetDefaultBooleanValue",
                  reinterpret_cast<void *>(mock_SetDefaultBooleanValue));
  overridePCLStub("ProcessDefinition/SetDefaultNumericValue",
                  reinterpret_cast<void *>(mock_SetDefaultNumericValue));
  overridePCLStub("ProcessDefinition/SetValidNumericRange",
                  reinterpret_cast<void *>(mock_SetValidNumericRange));
  overridePCLStub("ProcessDefinition/SetPrecision",
                  reinterpret_cast<void *>(mock_SetPrecision));
  overridePCLStub("ProcessDefinition/SetScientificNotation",
                  reinterpret_cast<void *>(mock_SetScientificNotation));
  overridePCLStub("ProcessDefinition/EndParameterDefinition",
                  reinterpret_cast<void *>(mock_EndParameterDefinition));
  overridePCLStub(
      "ProcessDefinition/SetProcessEditPreferencesRoutine",
      reinterpret_cast<void *>(mock_SetProcessEditPreferencesRoutine));
  overridePCLStub("Process/EditProcessPreferences",
                  reinterpret_cast<void *>(mock_EditProcessPreferences));
  overridePCLStub(
      "ProcessDefinition/SetProcessClassInitializationRoutine",
      reinterpret_cast<void *>(mock_SetProcessClassInitializationRoutine));
  overridePCLStub(
      "ProcessDefinition/SetProcessExecutionPreferencesRoutine",
      reinterpret_cast<void *>(mock_SetProcessExecutionPreferencesRoutine));

  // Timer API
  overridePCLStub("Timer/CreateTimer",
                  reinterpret_cast<void *>(mock_CreateTimer));
  overridePCLStub("Timer/GetTimerInterval",
                  reinterpret_cast<void *>(mock_GetTimerInterval));
  overridePCLStub("Timer/SetTimerInterval",
                  reinterpret_cast<void *>(mock_SetTimerInterval));
  overridePCLStub("Timer/GetTimerSingleShot",
                  reinterpret_cast<void *>(mock_GetTimerSingleShot));
  overridePCLStub("Timer/SetTimerSingleShot",
                  reinterpret_cast<void *>(mock_SetTimerSingleShot));
  overridePCLStub("Timer/IsTimerActive",
                  reinterpret_cast<void *>(mock_IsTimerActive));
  overridePCLStub("Timer/StartTimer",
                  reinterpret_cast<void *>(mock_StartTimer));
  overridePCLStub("Timer/StopTimer", reinterpret_cast<void *>(mock_StopTimer));
  overridePCLStub("Timer/SetTimerNotifyEventRoutine",
                  reinterpret_cast<void *>(mock_SetTimerNotifyEventRoutine));

  // Global and Process status enhancements
  overridePCLStub("Global/GetProcessStatus",
                  reinterpret_cast<void *>(mock_GetProcessStatus));
  overridePCLStub("Global/GetGlobalFlag",
                  reinterpret_cast<void *>(mock_GetGlobalFlag));
  overridePCLStub("Global/GetGlobalReal",
                  reinterpret_cast<void *>(mock_GetGlobalReal));

  // UI/Preview Notification Stubs
  overridePCLStub(
      "InterfaceDefinition/"
      "SetRealTimePreviewGenerationStartNotificationRoutine",
      reinterpret_cast<void *>(
          mock_SetRealTimePreviewGenerationStartNotificationRoutine));
  overridePCLStub(
      "InterfaceDefinition/"
      "SetRealTimePreviewGenerationFinishNotificationRoutine",
      reinterpret_cast<void *>(
          mock_SetRealTimePreviewGenerationFinishNotificationRoutine));
  overridePCLStub(
      "InterfaceDefinition/ExitInterfaceDefinitionContext",
      reinterpret_cast<void *>(mock_ExitInterfaceDefinitionContext));
}

static bool downloadAndExtractTensorFlow() {
  QString tfDownloadUrl = QString::fromStdString(
      Preferences::instance().getPclTensorflowDownloadUrl());
  if (tfDownloadUrl.isEmpty()) {
    tfDownloadUrl =
        "https://download.starnetastro.com/pixinsight/legacy/"
        "tensorflow-runtime/pixinsight_tensorflow_runtime_linux_TF_x64.zip";
  }

  QString tfLibDir =
      QString::fromStdString(Preferences::instance().getPclLibFolder());
  QDir().mkpath(tfLibDir);

  QProgressDialog progress(
      "Downloading TensorFlow Runtime...\nThis may take a moment.", "Cancel", 0,
      0, nullptr);
  progress.setWindowTitle("Downloading Dependencies");
  progress.setWindowModality(Qt::ApplicationModal);
  progress.setMinimumDuration(0);
  progress.setValue(0);
  progress.show();
  QCoreApplication::processEvents();

  QString tempZip = QDir::temp().filePath("tensorflow_runtime_linux.zip");
  QFile::remove(tempZip);

  QProcess curlProc;
  curlProc.start("curl", QStringList() << "-L" << "-f" << "-o" << tempZip
                                       << tfDownloadUrl);

  while (!curlProc.waitForFinished(100)) {
    QCoreApplication::processEvents();
    if (progress.wasCanceled()) {
      curlProc.kill();
      curlProc.waitForFinished();
      QFile::remove(tempZip);
      return false;
    }
  }

  if (curlProc.exitCode() != 0 || !QFile::exists(tempZip)) {
    qWarning() << "[PCL Bridge] TensorFlow download failed with exit code:"
               << curlProc.exitCode();
    QFile::remove(tempZip);
    return false;
  }

  progress.setLabelText("Extracting TensorFlow Runtime...");
  QCoreApplication::processEvents();

  QProcess unzipProc;
  unzipProc.start("unzip", QStringList()
                               << "-o" << tempZip << "-d" << tfLibDir);

  while (!unzipProc.waitForFinished(100)) {
    QCoreApplication::processEvents();
  }

  QFile::remove(tempZip);

  if (unzipProc.exitCode() != 0) {
    qWarning() << "[PCL Bridge] TensorFlow extraction failed with exit code:"
               << unzipProc.exitCode();
    return false;
  }

  // Scan recursively for libtensorflow.so.2 and libtensorflow_framework.so.2
  // and move them to tfLibDir
  QDirIterator it(tfLibDir,
                  QStringList()
                      << "libtensorflow.so.2" << "libtensorflow_framework.so.2",
                  QDir::Files, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    QString foundPath = it.next();
    QFileInfo fi(foundPath);
    QString destPath = QDir(tfLibDir).filePath(fi.fileName());
    if (QDir(tfLibDir).absolutePath() != fi.absoluteDir().absolutePath()) {
      QFile::remove(destPath);
      QFile::rename(foundPath, destPath);
    }
  }

  qInfo() << "[PCL Bridge] TensorFlow runtime downloaded and extracted "
             "successfully to:"
          << tfLibDir;
  return true;
}

void PCLBridge::preloadLibraries(const QString &libDir) {
  if (libDir.isEmpty()) {
    return;
  }
  QDir dir(libDir);
  if (!dir.exists()) {
    return;
  }
  qInfo() << "[PCL Bridge] Scanning and preloading libraries from:" << libDir;

  // Track canonical paths to avoid dlopening the same physical library multiple times via symlinks/aliases
  QSet<QString> loadedCanonicalPaths;

  // 1. Explicitly pre-load TensorFlow runtime if available in libDir
  QString tfFrameworkPath = dir.filePath("libtensorflow_framework.so.2");
  if (!QFile::exists(tfFrameworkPath)) {
    tfFrameworkPath = dir.filePath("libtensorflow_framework.so");
  }
  QString tfPath = dir.filePath("libtensorflow.so.2");
  if (!QFile::exists(tfPath)) {
    tfPath = dir.filePath("libtensorflow.so");
  }

  if (QFile::exists(tfFrameworkPath)) {
    QFileInfo fi(tfFrameworkPath);
    QString canonical = fi.canonicalFilePath();
    if (!canonical.isEmpty() && !loadedCanonicalPaths.contains(canonical)) {
      qDebug() << "[PCL Bridge] Pre-loading TensorFlow framework from:" << tfFrameworkPath;
      void *handle = dlopen(tfFrameworkPath.toUtf8().constData(), RTLD_LAZY | RTLD_GLOBAL);
      if (handle) {
        loadedCanonicalPaths.insert(canonical);
      } else {
        qWarning() << "[PCL Bridge] Failed to pre-load tensorflow framework:" << dlerror();
      }
    }
  }

  if (QFile::exists(tfPath)) {
    QFileInfo fi(tfPath);
    QString canonical = fi.canonicalFilePath();
    if (!canonical.isEmpty() && !loadedCanonicalPaths.contains(canonical)) {
      qDebug() << "[PCL Bridge] Pre-loading TensorFlow from:" << tfPath;
      void *handle = dlopen(tfPath.toUtf8().constData(), RTLD_LAZY | RTLD_GLOBAL);
      if (handle) {
        loadedCanonicalPaths.insert(canonical);
      } else {
        qWarning() << "[PCL Bridge] Failed to pre-load tensorflow:" << dlerror();
      }
    }
  }

  // 2. Pre-load remaining helper shared libraries, skipping symlinks, TensorFlow duplicates, and internal CUDA sub-libraries
  QStringList filters;
  filters << "*.so" << "*.so.*";

  QDirIterator it(libDir, filters, QDir::Files, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    QString libPath = it.next();
    QFileInfo fi(libPath);

    // Skip symlinks to avoid duplicate dlopen calls on versioned aliases
    if (fi.isSymLink()) {
      continue;
    }

    QString canonical = fi.canonicalFilePath();
    if (canonical.isEmpty() || loadedCanonicalPaths.contains(canonical)) {
      continue;
    }

    QString fileName = fi.fileName();
    // Skip internal CUDA / cuDNN / ONNX provider sub-libraries that cause out-of-order initialization crashes
    if (fileName.startsWith("libcudnn_") || fileName.startsWith("libcublas") ||
        fileName.startsWith("libnccl") || fileName.startsWith("libnv") ||
        fileName.startsWith("libtensorflow") || fileName.startsWith("libonnxruntime_providers_")) {
      continue;
    }

    qDebug() << "[PCL Bridge] Pre-loading library:" << libPath;
    void *handle = dlopen(libPath.toUtf8().constData(), RTLD_LAZY | RTLD_GLOBAL);
    if (handle) {
      loadedCanonicalPaths.insert(canonical);
    } else {
      qDebug() << "[PCL Bridge] Note: Could not dlopen" << libPath << "-" << dlerror();
    }
  }
}

bool PCLBridge::loadModule(const QString &path) {
  // Check if this module is already loaded to avoid duplicates
  for (const auto &mod : m_modules) {
    if (mod.filepath == path) {
      qDebug() << "[PCL Bridge] Module is already loaded:" << path;
      return true;
    }
  }

  // Dynamically pre-load TensorFlow dependencies for RC-Astro *XTerminator
  // plugins.
  {
    QString tfLibDir =
        QString::fromStdString(Preferences::instance().getPclLibFolder());
    QString tfFrameworkPath =
        QDir(tfLibDir).filePath("libtensorflow_framework.so.2");
    QString tfPath = QDir(tfLibDir).filePath("libtensorflow.so.2");

    bool tfFound = QFile::exists(tfPath) && QFile::exists(tfFrameworkPath);
    if (!tfFound) {
      qInfo() << "[PCL Bridge] TensorFlow dependencies not found. Initiating "
                 "automatic download...";
      if (downloadAndExtractTensorFlow()) {
        // Re-evaluate paths after download
        tfFrameworkPath =
            QDir(tfLibDir).filePath("libtensorflow_framework.so.2");
        tfPath = QDir(tfLibDir).filePath("libtensorflow.so.2");
        tfFound = QFile::exists(tfPath) && QFile::exists(tfFrameworkPath);
      }
    }

    if (tfFound) {
      qDebug() << "[PCL Bridge] Pre-loading TensorFlow framework from:"
               << tfFrameworkPath;
      void *tfFrameHandle =
          dlopen(tfFrameworkPath.toUtf8().constData(), RTLD_LAZY | RTLD_GLOBAL);
      if (!tfFrameHandle) {
        qWarning()
            << "[PCL Bridge] Failed to pre-load libtensorflow_framework.so.2:"
            << dlerror();
      }

      qDebug() << "[PCL Bridge] Pre-loading TensorFlow from:" << tfPath;
      void *tfHandle =
          dlopen(tfPath.toUtf8().constData(), RTLD_LAZY | RTLD_GLOBAL);
      if (!tfHandle) {
        qWarning() << "[PCL Bridge] Failed to pre-load libtensorflow.so.2:"
                   << dlerror();
      }
    } else {
      qWarning() << "[PCL Bridge] TensorFlow libraries were not loaded. "
                    "StarNet or XTerminator modules may fail to execute.";
    }
  }

  qDebug() << "[PCL Bridge] Attempting to dlopen:" << path;
  void *libHandle = dlopen(path.toUtf8().constData(), RTLD_LAZY | RTLD_GLOBAL);
  if (!libHandle) {
    qWarning() << "[PCL Bridge] dlopen failed:" << dlerror();
    return false;
  }

  // Resolve entry points
  typedef uint32 (*IdentifyRoutine)(api_module_description **, int32);
  typedef uint32 (*InitializeRoutine)(api_handle, function_resolver, uint32,
                                      void *);
  typedef int32 (*InstallRoutine)(int32);

  IdentifyRoutine identify = reinterpret_cast<IdentifyRoutine>(
      dlsym(libHandle, "IdentifyPixInsightModule"));
  InitializeRoutine initialize = reinterpret_cast<InitializeRoutine>(
      dlsym(libHandle, "InitializePixInsightModule"));
  InstallRoutine install = reinterpret_cast<InstallRoutine>(
      dlsym(libHandle, "InstallPixInsightModule"));

  if (!identify || !initialize) {
    qWarning()
        << "[PCL Bridge] Missing mandatory PMIDN or PMINI symbols in module.";
    dlclose(libHandle);
    return false;
  }

  // Create a new module info structure and set it as the current loading
  // context
  PCLModuleInfo newModule;
  newModule.libHandle = libHandle;
  newModule.filepath = path;
  g_currentLoadingModule = &newModule;

  // 1. Install with FullInstall first to instantiate the MetaModule and all
  // process/parameter meta-objects!
  if (install) {
    qDebug() << "[PCL Bridge] Running InstallPixInsightModule (FullInstall)...";
    int32 installStatus = install(0); // InstallMode::FullInstall
    if (installStatus != 0) {
      qWarning()
          << "[PCL Bridge] InstallPixInsightModule (FullInstall) failed, code:"
          << installStatus;
      dlclose(libHandle);
      g_currentLoadingModule = nullptr;
      return false;
    }
    qDebug() << "[PCL Bridge] Meta-objects instantiated successfully.";
  } else {
    qWarning() << "[PCL Bridge] Module has no installation entry point "
                  "(PMINS), cannot initialize.";
    dlclose(libHandle);
    g_currentLoadingModule = nullptr;
    return false;
  }

  // 2. Initialize the stubs and overrides now that the meta-objects are
  // instantiated
  if (!m_initialized) {
    initPCLStubs();
    setupOverrides();
    m_initialized = true;
  }

  api_handle hModule = reinterpret_cast<api_handle>(this);
  uint32 status = initialize(hModule, &PCLBridge::resolveCoreFunction,
                             PCL_API_Version, nullptr);
  if (status != 0) {
    qWarning() << "[PCL Bridge] InitializePixInsightModule failed, code:"
               << status;
    dlclose(libHandle);
    g_currentLoadingModule = nullptr;
    return false;
  }

  // 3. Identify the module to get description metadata!
  qDebug() << "[PCL Bridge] Running IdentifyPixInsightModule...";
  status = identify(&newModule.description, 0);
  if (status != 0) {
    qWarning() << "[PCL Bridge] IdentifyPixInsightModule phase 0 failed, code:"
               << status;
    dlclose(libHandle);
    g_currentLoadingModule = nullptr;
    return false;
  }

  status = identify(&newModule.description, 1);
  if (status != 0) {
    qWarning() << "[PCL Bridge] IdentifyPixInsightModule phase 1 failed, code:"
               << status;
    dlclose(libHandle);
    g_currentLoadingModule = nullptr;
    return false;
  }

  if (!newModule.description) {
    qWarning()
        << "[PCL Bridge] Module description is null after identification.";
    dlclose(libHandle);
    g_currentLoadingModule = nullptr;
    return false;
  }

  // Print metadata
  qInfo().noquote()
      << QString("[PCL Bridge] Loaded Module: %1 API Rev: %2 Version: %3")
             .arg(QString::fromUtf8(newModule.description->name))
             .arg(QString::number(newModule.description->apiVersion, 16))
             .arg(QString::fromUtf8(newModule.description->versionInfo));

  // Call the module's OnLoad routine if registered!
  if (newModule.onLoadFn) {
    qDebug() << "[PCL Bridge] Invoking module OnLoad routine...";
    newModule.onLoadFn();
    qDebug() << "[PCL Bridge] Module OnLoad routine finished.";
  }

  // Call the class initialization and execution preferences routines for all
  // registered processes
  for (auto &pair : g_processes) {
    if (pair.second.classInitFn) {
      qDebug()
          << "[PCL Bridge] Invoking process class initialization routine for:"
          << pair.second.id;
      pair.second.classInitFn(pair.first);
      qDebug() << "[PCL Bridge] Process class initialization routine finished.";
    }
    if (pair.second.executionPreferencesFn) {
      qDebug()
          << "[PCL Bridge] Invoking process execution preferences routine for:"
          << pair.second.id;
      pair.second.executionPreferencesFn(pair.first);
      qDebug()
          << "[PCL Bridge] Process execution preferences routine finished.";
    }
  }

  m_modules.push_back(newModule);
  m_description = newModule.description;
  g_currentLoadingModule = nullptr;

  return true;
}

void PCLBridge::unloadModule() {
  for (auto &mod : m_modules) {
    if (mod.libHandle) {
      // Call the module's OnUnload routine if registered!
      if (mod.onUnloadFn) {
        qDebug() << "[PCL Bridge] Invoking module OnUnload routine for:"
                 << mod.filepath;
        mod.onUnloadFn();
      }

      // If identify is loaded, call PMIDN phase 0xff for cleanup
      typedef uint32 (*IdentifyRoutine)(api_module_description **, int32);
      IdentifyRoutine identify = reinterpret_cast<IdentifyRoutine>(
          dlsym(mod.libHandle, "IdentifyPixInsightModule"));
      if (identify && mod.description) {
        identify(&mod.description, 0xff);
      }

      dlclose(mod.libHandle);
      qInfo() << "[PCL Bridge] Module unloaded:" << mod.filepath;
    }
  }

  m_modules.clear();
  m_description = nullptr;
  m_initialized = false;

  g_processes.clear();
  g_processIdToHandle.clear();
  g_interfaces.clear();
  g_interfaceIdToHandle.clear();
  g_interfaceControls.clear();

  for (auto &pair : g_threads) {
    delete pair.second;
  }
  g_threads.clear();
  g_currentThread = nullptr;
  g_activeImage = nullptr;
}

bool PCLBridge::executeProcess(const QString &processId,
                               std::vector<ImageBufferPtr> &buffers) {
  if (!m_initialized) {
    qWarning()
        << "[PCL Bridge] Cannot execute process, no module loaded/initialized.";
    return false;
  }

  if (buffers.empty()) {
    qWarning()
        << "[PCL Bridge] Cannot execute process, no image buffers provided.";
    return false;
  }

  auto idIt = g_processIdToHandle.find(processId.toStdString());
  if (idIt == g_processIdToHandle.end()) {
    qWarning() << "[PCL Bridge] Process ID not found:" << processId;
    return false;
  }

  meta_process_handle hMeta = idIt->second;
  const auto &info = g_processes[hMeta];

  if (!info.createFn) {
    qWarning() << "[PCL Bridge] Process has no creation routine:" << processId;
    return false;
  }

  ProcessExecutionGuard execGuard(processId);

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
  viewMock.magic = 0x56494557;
  viewMock.id = "BLastroActiveView";
  viewMock.hImage = &imgMock;

  PCLWindowMock windowMock;
  windowMock.mainView = &viewMock;
  viewMock.parentWindow = &windowMock;

  qDebug() << "[PCL Bridge] Executing process on image:" << imgMock.width << "x"
           << imgMock.height << "with" << imgMock.numChannels << "channels";
  bool success = false;
  if (info.executeFn) {
    success = info.executeFn(&viewMock, hProcess);
  } else if (info.executeGlobalFn) {
    qWarning() << "[PCL Bridge] Process only supports global execution, "
                  "executing globally...";
    success = info.executeGlobalFn(hProcess);
  } else {
    qWarning() << "[PCL Bridge] Process has no execution routine!";
  }

  qDebug() << "[PCL Bridge] Process execution finished, status:"
           << (success ? "Success" : "Failed");

  // Clean up
  if (info.destroyFn) {
    info.destroyFn(hProcess);
  }
  g_processParameters.erase(hProcess);
  g_activeImage = nullptr;

  return success;
}

bool PCLBridge::isProcessRegistered(const QString &processId) const {
  return g_processIdToHandle.find(processId.toStdString()) !=
         g_processIdToHandle.end();
}

std::vector<QString> PCLBridge::registeredProcesses() const {
  std::vector<QString> list;
  for (const auto &pair : g_processIdToHandle) {
    list.push_back(QString::fromStdString(pair.first));
  }
  return list;
}

void *PCLBridge::resolveCoreFunction(const char *funcName) {
  return getPCLStub(funcName);
}

bool PCLBridge::executeProcessInstance(const QString &processId, void *hProcess,
                                       std::vector<ImageBufferPtr> &buffers) {
  if (!m_initialized) {
    qWarning() << "[PCL Bridge] Cannot execute process instance, no module "
                  "loaded/initialized.";
    return false;
  }

  if (!hProcess) {
    qWarning()
        << "[PCL Bridge] Cannot execute process instance, null process handle.";
    return false;
  }

  if (buffers.empty()) {
    qWarning() << "[PCL Bridge] Cannot execute process instance, no image "
                  "buffers provided.";
    return false;
  }

  auto idIt = g_processIdToHandle.find(processId.toStdString());
  if (idIt == g_processIdToHandle.end()) {
    qWarning() << "[PCL Bridge] Process ID not found:" << processId;
    return false;
  }

  meta_process_handle hMeta = idIt->second;
  const auto &info = g_processes[hMeta];

  ProcessExecutionGuard execGuard(processId);

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
  viewMock.magic = 0x56494557;
  viewMock.id = "BLastroActiveView";
  viewMock.hImage = &imgMock;

  PCLWindowMock windowMock;
  windowMock.mainView = &viewMock;
  viewMock.parentWindow = &windowMock;

  qDebug() << "[PCL Bridge] Executing process instance on image:"
           << imgMock.width << "x" << imgMock.height << "with"
           << imgMock.numChannels << "channels";
  bool success = false;
  if (info.executeFn) {
    success = info.executeFn(&viewMock, hProcess);
  } else if (info.executeGlobalFn) {
    qWarning() << "[PCL Bridge] Process only supports global execution, "
                  "executing globally...";
    success = info.executeGlobalFn(hProcess);
  } else {
    qWarning() << "[PCL Bridge] Process has no execution routine!";
  }

  qDebug() << "[PCL Bridge] Process instance execution finished, status:"
           << (success ? "Success" : "Failed");
  g_activeImage = nullptr;
  return success;
}

bool PCLBridge::launchInterface(const QString &processId,
                                QWidget *parentWindow) {
  if (!m_initialized) {
    qWarning() << "[PCL Bridge] Cannot launch interface, no module "
                  "loaded/initialized.";
    return false;
  }

  std::string idStr = processId.toStdString();
  auto idIt = g_interfaceIdToHandle.find(idStr);
  if (idIt == g_interfaceIdToHandle.end()) {
    qWarning() << "[PCL Bridge] Interface ID not found:" << processId;
    return false;
  }

  meta_interface_handle hMeta = idIt->second;
  const auto &info = g_interfaces[hMeta];

  if (!info.initFn) {
    qWarning() << "[PCL Bridge] Interface has no initialization routine:"
               << processId;
    return false;
  }

  qDebug() << "[PCL Bridge] Launching interface for:" << processId;

  // Create the host widget (main MDI window content)
  QWidget *hostWidget = new QWidget();
  hostWidget->setWindowTitle(processId + " Process Interface");
  hostWidget->resize(600, 450);

  // Create a container widget specifically for the plugin's controls
  QWidget *pluginContainer = new QWidget(hostWidget);

  // Create the Apply button
  QPushButton *applyButton =
      new QPushButton("Apply to Active Image", hostWidget);
  applyButton->setStyleSheet("QPushButton {"
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
                             "}");

  // Create a vertical layout for the host widget
  QVBoxLayout *mainLayout = new QVBoxLayout(hostWidget);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(10);

  // Add the plugin container and the apply button to the main layout
  mainLayout->addWidget(pluginContainer,
                        1); // Give plugin container stretch factor 1

  // Create a horizontal layout for the bottom button bar
  QHBoxLayout *buttonBar = new QHBoxLayout();
  QPushButton *prefsButton = nullptr;

  bool processHasPrefs = false;
  pcl::process_edit_preferences_routine procEditPrefsFn = nullptr;
  auto procPrefsIdIt = g_processIdToHandle.find(idStr);
  if (procPrefsIdIt != g_processIdToHandle.end()) {
    meta_process_handle hMetaProcess = procPrefsIdIt->second;
    const auto &procInfo = g_processes[hMetaProcess];
    processHasPrefs = procInfo.hasEditPreferences;
    procEditPrefsFn = procInfo.editPreferencesFn;
  }

  if (info.editPreferencesFn || processHasPrefs) {
    prefsButton =
        new QPushButton(QString::fromUtf8("🔧 Preferences"), hostWidget);
    prefsButton->setStyleSheet("QPushButton {"
                               "   background-color: #2a2a2a;"
                               "   border: 1px solid #444;"
                               "   border-radius: 4px;"
                               "   color: #ffffff;"
                               "   font-weight: bold;"
                               "   padding: 8px 16px;"
                               "   font-size: 12px;"
                               "}"
                               "QPushButton:hover {"
                               "   background-color: #3a3a3a;"
                               "}"
                               "QPushButton:pressed {"
                               "   background-color: #1a1a1a;"
                               "}");
    buttonBar->addWidget(prefsButton);
  }
  buttonBar->addStretch();
  buttonBar->addWidget(applyButton);
  mainLayout->addLayout(buttonBar);

  // Apply dark theme stylesheet to match application dark mode
  hostWidget->setStyleSheet(
      "QWidget { background-color: #121212; color: #ffffff; }"
      "QWidget:disabled { color: #555555; }"
      "QPushButton { background-color: #2a2a2a; border: 1px solid #444; "
      "border-radius: 4px; padding: 6px 12px; color: #ffffff; }"
      "QPushButton:disabled { background-color: #1a1a1a; color: #555555; "
      "border-color: #222; }"
      "QPushButton:hover { background-color: #3a3a3a; }"
      "QPushButton:hover:disabled { background-color: #1a1a1a; }"
      "QGroupBox { border: 1px solid #444; border-radius: 6px; margin-top: "
      "12px; padding: 12px; font-weight: bold; }"
      "QGroupBox:disabled { border-color: #222; color: #555555; }"
      "QGroupBox::title { subcontrol-origin: margin; left: 8px; padding: 0 "
      "3px; }"
      "QLabel { color: #dddddd; }"
      "QLabel:disabled { color: #555555; }"
      "QComboBox { background-color: #2a2a2a; border: 1px solid #444; "
      "border-radius: 4px; padding: 4px; color: #ffffff; }"
      "QComboBox:disabled { background-color: #1a1a1a; color: #555555; "
      "border-color: #222; }"
      "QSpinBox { background-color: #2a2a2a; border: 1px solid #444; "
      "border-radius: 4px; padding: 4px; color: #ffffff; }"
      "QSpinBox:disabled { background-color: #1a1a1a; color: #555555; "
      "border-color: #222; }"
      "QSlider::groove:horizontal { border: 1px solid #444; height: 6px; "
      "background: #2a2a2a; border-radius: 3px; }"
      "QSlider::groove:horizontal:disabled { border-color: #222; background: "
      "#1a1a1a; }"
      "QSlider::handle:horizontal { background: #3a8ee6; width: 14px; margin: "
      "-4px 0; border-radius: 7px; }"
      "QSlider::handle:horizontal:disabled { background: #555555; }");

  // 1. Call the interface's initialization callback!
  // The C-API initialization routine expects (interface_handle, control_handle)
  interface_handle hInterface = const_cast<void *>(hMeta);
  control_handle hParentControl =
      reinterpret_cast<control_handle>(pluginContainer);

  qDebug()
      << "[PCL Bridge] Calling interface initialization callback (initFn)...";
  info.initFn(hInterface, hParentControl);
  qDebug() << "[PCL Bridge] Interface initialization callback completed.";

  // If the interface registered a global preferences updated callback, notify
  // it at startup so it can initialize its licensing and other global
  // preference-dependent states.
  if (info.globalPrefUpdatedFn) {
    qDebug() << "[PCL Bridge] Calling interface global preferences updated "
                "notification...";
    info.globalPrefUpdatedFn(hInterface);
    qDebug() << "[PCL Bridge] Interface global preferences updated "
                "notification completed.";
  }

  // Instantiate process handle
  auto procIdIt = g_processIdToHandle.find(idStr);
  meta_process_handle hMetaProcess = nullptr;
  process_handle hProcess = nullptr;
  if (procIdIt != g_processIdToHandle.end()) {
    hMetaProcess = procIdIt->second;
    const auto &procInfo = g_processes[hMetaProcess];
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
    bool launchOk =
        info.launchFn(hInterface, hMetaProcess, hProcess, &dynamic, &flags);
    qDebug() << "[PCL Bridge] Interface launch routine returned:" << launchOk
             << ", dynamic:" << (bool)dynamic;

    // 3. Import the process parameters into the interface!
    if (launchOk && info.importProcessFn && hProcess) {
      qDebug() << "[PCL Bridge] Calling interface process import routine "
                  "(importProcessFn)...";
      info.importProcessFn(hInterface, hProcess);
      qDebug() << "[PCL Bridge] Interface process import routine completed.";

      // Perform process validation at startup so the plugin runs its
      // license-check and unlocks!
      if (info.validateProcessFn) {
        qDebug() << "[PCL Bridge] Calling interface process validation routine "
                    "(validateProcessFn)...";
        std::vector<char16_type> errorBuf(1024, 0);
        api_bool valid = info.validateProcessFn(
            hInterface, hProcess, errorBuf.data(), errorBuf.size() - 1);
        qDebug()
            << "[PCL Bridge] Interface process validation routine returned:"
            << (bool)valid;
      }

      // Perform process-interface validation!
      if (hMetaProcess) {
        const auto &procInfo = g_processes[hMetaProcess];
        if (procInfo.validateInterfaceFn) {
          qDebug() << "[PCL Bridge] Calling process-interface validation "
                      "routine (validateInterfaceFn)...";
          api_bool valid = procInfo.validateInterfaceFn(hProcess, hInterface);
          qDebug()
              << "[PCL Bridge] Process-interface validation routine returned:"
              << (bool)valid;
        }
      }
    }
  }

  // Connect the Apply button to execute the process on the active image
  QObject::connect(
      applyButton, &QPushButton::clicked,
      [this, processId, hProcess, parentWindow]() {
        MainWindow *mainWin = qobject_cast<MainWindow *>(parentWindow);
        if (mainWin) {
          mainWin->executePCLProcessOnActiveImage(processId, hProcess);
        } else {
          qWarning() << "[PCL Bridge] Cannot apply process: parent window is "
                        "not MainWindow.";
        }
      });

  // Connect the Preferences button if available
  if (prefsButton) {
    QObject::connect(
        prefsButton, &QPushButton::clicked,
        [info, hInterface, procEditPrefsFn, hMetaProcess, hProcess]() {
          if (info.editPreferencesFn) {
            qDebug() << "[PCL Bridge] Invoking interface edit preferences "
                        "routine...";
            info.editPreferencesFn(hInterface);
            qDebug()
                << "[PCL Bridge] Interface edit preferences routine finished.";
          } else if (procEditPrefsFn) {
            qDebug()
                << "[PCL Bridge] Invoking process edit preferences routine...";
            procEditPrefsFn(hMetaProcess);
            qDebug()
                << "[PCL Bridge] Process edit preferences routine finished.";
          }

          // Notify the interface that global preferences have been updated
          if (info.globalPrefUpdatedFn) {
            qDebug() << "[PCL Bridge] Calling interface global preferences "
                        "updated notification after edit...";
            info.globalPrefUpdatedFn(hInterface);
          }

          // Re-validate process after preferences edit!
          if (info.validateProcessFn && hProcess) {
            qDebug() << "[PCL Bridge] Calling interface process validation "
                        "routine after edit...";
            std::vector<char16_type> errorBuf(1024, 0);
            info.validateProcessFn(hInterface, hProcess, errorBuf.data(),
                                   errorBuf.size() - 1);
          }
          if (hMetaProcess && hProcess) {
            auto it = g_processes.find(hMetaProcess);
            if (it != g_processes.end() && it->second.validateInterfaceFn) {
              qDebug() << "[PCL Bridge] Calling process-interface validation "
                          "routine after edit...";
              it->second.validateInterfaceFn(hProcess, hInterface);
            }
          }

          // Also notify the process that execution preferences might have
          // changed!
          if (hMetaProcess) {
            auto it = g_processes.find(hMetaProcess);
            if (it != g_processes.end() && it->second.executionPreferencesFn) {
              qDebug() << "[PCL Bridge] Invoking process execution preferences "
                          "routine after edit...";
              it->second.executionPreferencesFn(hMetaProcess);
              qDebug() << "[PCL Bridge] Process execution preferences routine "
                          "finished.";
            }
          }
        });
  }

  // Connect host widget destruction to process cleanup
  auto procIdItDest = g_processIdToHandle.find(idStr);
  meta_process_handle hMetaDest = (procIdItDest != g_processIdToHandle.end())
                                      ? procIdItDest->second
                                      : nullptr;
  auto destInfo =
      (hMetaDest != nullptr) ? g_processes[hMetaDest] : PCLProcessInfo();

  QObject::connect(hostWidget, &QObject::destroyed, [destInfo, hProcess]() {
    qDebug() << "[PCL Bridge] Host widget destroyed. Cleaning up process "
                "instance...";
    if (hProcess && destInfo.destroyFn) {
      destInfo.destroyFn(hProcess);
    }
    g_processParameters.erase(hProcess);
  });

  MainWindow *mainWin = qobject_cast<MainWindow *>(parentWindow);
  if (mainWin) {
    mainWin->createPCLPluginSubWindow(hostWidget, processId,
                                      processId + " Process Interface");
  } else {
    hostWidget->show();
  }

  return true;
}

#include <QSettings>

static ::QSettings &getBLastroSettings() {
  static ::QSettings settings("BLastro", "BLastro");
  return settings;
}

static api_bool mock_ReadSettingsString(api_handle hModule, char16_type **value,
                                        const char *key, api_bool global) {
  if (!key || !value)
    return false;
  QString qkey = QString::fromUtf8(key);

  QVariant val = getBLastroSettings().value(qkey);
  qDebug() << "[PCL Settings] ReadSettingsString:" << qkey << "->" << val;
  if (val.isValid()) {
    QString str = val.toString();
    int len = str.length();
    char16_type *buf = reinterpret_cast<char16_type *>(
        mock_Allocate((len + 1) * sizeof(char16_type)));
    if (!buf)
      return false;
    std::memcpy(buf, str.utf16(), len * sizeof(char16_type));
    buf[len] = 0;
    *value = buf;
    return true;
  }
  return false;
}

static api_bool mock_WriteSettingsString(api_handle hModule,
                                         const char16_type *value,
                                         const char *key, api_bool global) {
  if (!key)
    return false;
  QString qkey = QString::fromUtf8(key);
  QString val =
      value ? QString::fromUtf16(reinterpret_cast<const char16_t *>(value))
            : QString();
  qDebug() << "[PCL Settings] WriteSettingsString:" << qkey << "<-" << val;
  getBLastroSettings().setValue(qkey, val);
  return true;
}

static api_bool mock_ReadSettingsFlag(api_handle hModule, api_bool *value,
                                      const char *key, api_bool global) {
  if (!key || !value)
    return false;
  QString qkey = QString::fromUtf8(key);
  QVariant val = getBLastroSettings().value(qkey);
  qDebug() << "[PCL Settings] ReadSettingsFlag:" << qkey << "->" << val;
  if (val.isValid()) {
    *value = val.toBool();
    return true;
  }
  return false;
}

static api_bool mock_WriteSettingsFlag(api_handle hModule, api_bool value,
                                       const char *key, api_bool global) {
  if (!key)
    return false;
  QString qkey = QString::fromUtf8(key);
  qDebug() << "[PCL Settings] WriteSettingsFlag:" << qkey << "<-"
           << (bool)value;
  getBLastroSettings().setValue(qkey, (bool)value);
  return true;
}

static api_bool mock_ReadSettingsReal(api_handle hModule, double *value,
                                      const char *key, api_bool global) {
  if (!key || !value)
    return false;
  QString qkey = QString::fromUtf8(key);
  QVariant val = getBLastroSettings().value(qkey);
  qDebug() << "[PCL Settings] ReadSettingsReal:" << qkey << "->" << val;
  if (val.isValid()) {
    *value = val.toDouble();
    return true;
  }
  return false;
}

static api_bool mock_WriteSettingsReal(api_handle hModule, double value,
                                       const char *key, api_bool global) {
  if (!key)
    return false;
  QString qkey = QString::fromUtf8(key);
  qDebug() << "[PCL Settings] WriteSettingsReal:" << qkey << "<-" << value;
  getBLastroSettings().setValue(qkey, value);
  return true;
}

static api_bool mock_ReadSettingsInteger(api_handle hModule, int32 *value,
                                         const char *key, api_bool global) {
  if (!key || !value)
    return false;
  QString qkey = QString::fromUtf8(key);
  QVariant val = getBLastroSettings().value(qkey);
  qDebug() << "[PCL Settings] ReadSettingsInteger:" << qkey << "->" << val;
  if (val.isValid()) {
    *value = val.toInt();
    return true;
  }
  return false;
}

static api_bool mock_WriteSettingsInteger(api_handle hModule, int32 value,
                                          const char *key, api_bool global) {
  if (!key)
    return false;
  QString qkey = QString::fromUtf8(key);
  qDebug() << "[PCL Settings] WriteSettingsInteger:" << qkey << "<-" << value;
  getBLastroSettings().setValue(qkey, value);
  return true;
}

static api_bool mock_ReadSettingsUnsignedInteger(api_handle hModule,
                                                 uint32 *value, const char *key,
                                                 api_bool global) {
  if (!key || !value)
    return false;
  QString qkey = QString::fromUtf8(key);
  QVariant val = getBLastroSettings().value(qkey);
  qDebug() << "[PCL Settings] ReadSettingsUnsignedInteger:" << qkey << "->"
           << val;
  if (val.isValid()) {
    *value = val.toUInt();
    return true;
  }
  return false;
}

static api_bool mock_WriteSettingsUnsignedInteger(api_handle hModule,
                                                  uint32 value, const char *key,
                                                  api_bool global) {
  if (!key)
    return false;
  QString qkey = QString::fromUtf8(key);
  qDebug() << "[PCL Settings] WriteSettingsUnsignedInteger:" << qkey << "<-"
           << value;
  getBLastroSettings().setValue(qkey, value);
  return true;
}

static api_bool mock_GetGlobalString(const char *globalKey, char16_type *value,
                                     pcl::size_type *maxLen) {
  if (!globalKey || !maxLen)
    return false;
  QString key = QString::fromUtf8(globalKey);
  // Suppress spammy license checks from logs
  if (!key.contains("License")) {
    qDebug() << "[PCL Global] GetGlobalString:" << key;
  }
  QString result = "";
  if (key.contains("License", Qt::CaseInsensitive)) {
    result = "BLastro-Community-License";
  } else if (key == "Application/LibraryDirectory") {
    result = "/opt/PixInsight/library";
  } else if (key == "Application/BaseDirectory") {
    result = "/opt/PixInsight";
  }

  if (result.isEmpty())
    return false;

  // Length query
  if (!value) {
    *maxLen = result.length() + 1;
    return true;
  }

  // Actual string fetch
  if (*maxLen > 0) {
    int len = qMin(static_cast<int>(*maxLen) - 1, result.length());
    std::memcpy(value, result.utf16(), len * sizeof(char16_type));
    value[len] = 0;
    *maxLen = len;
    return true;
  }
  return false;
}

static api_bool mock_GetGlobalInteger(const char *globalKey, void *value,
                                      api_bool isSigned) {
  if (!globalKey || !value)
    return false;
  QString key = QString::fromUtf8(globalKey);
  qDebug() << "[PCL Global] GetGlobalInteger:" << key
           << "isSigned =" << (bool)isSigned;

  int val = 0;
  if (key == "Workspace/PrimaryScreenCenterX") {
    val = 960;
  } else if (key == "Workspace/PrimaryScreenCenterY") {
    val = 540;
  }

  if (isSigned) {
    *reinterpret_cast<int32 *>(value) = val;
  } else {
    *reinterpret_cast<uint32 *>(value) = val;
  }
  return true;
}

static api_bool mock_GetGlobalFlag(const char *globalKey, api_bool *value) {
  if (!globalKey)
    return false;
  QString key = QString::fromUtf8(globalKey);
  qDebug() << "[PCL Global] GetGlobalFlag:" << key;

  bool val = false;
  bool found = false;

  if (key == "Application/ParallelProcessing" ||
      key == "Application/UseParallelProcessing") {
    val = true;
    found = true;
  } else if (key == "Application/UseGPU") {
    val = false;
    found = true;
  } else if (key == "Application/UseAVX" || key == "Application/UseAVX2" ||
             key == "Application/UseAVX512") {
    val = true;
    found = true;
  }

  if (found) {
    if (value) {
      *value = val;
    }
    return true;
  }
  return false;
}

static api_bool mock_GetGlobalReal(const char *globalKey, double *value) {
  if (!globalKey || !value)
    return false;
  QString key = QString::fromUtf8(globalKey);
  qDebug() << "[PCL Global] GetGlobalReal:" << key;

  double val = 0.0;
  bool found = false;

  if (key == "Workspace/PrimaryScreenDPI") {
    val = 96.0;
    found = true;
  } else if (key == "Workspace/PrimaryScreenPixelRatio" ||
             key == "Workspace/DisplayPixelRatio") {
    val = 1.0;
    found = true;
  }

  if (found) {
    *value = val;
    return true;
  }
  return false;
}

static void mock_SetRealTimePreviewGenerationStartNotificationRoutine(
    pcl::global_notification_routine) {
  qDebug()
      << "[PCL Bridge] "
         "mock_SetRealTimePreviewGenerationStartNotificationRoutine called";
}

static void mock_SetRealTimePreviewGenerationFinishNotificationRoutine(
    pcl::global_notification_routine) {
  qDebug()
      << "[PCL Bridge] "
         "mock_SetRealTimePreviewGenerationFinishNotificationRoutine called";
}

static void mock_ExitInterfaceDefinitionContext() {
  qDebug() << "[PCL Bridge] mock_ExitInterfaceDefinitionContext called";
}

// ============================================================================
// Process / Parameter Metadata Definition Mocks
// ============================================================================
static void mock_BeginParameterDefinition(meta_parameter_handle hParam,
                                          const char *paramId, uint32 type) {
  qDebug() << "[PCL ProcessDef] BeginParameterDefinition:" << paramId
           << "type =" << type;
}

static void mock_SetParameterProcessVersionRange(uint32 minVer, uint32 maxVer) {
  qDebug() << "[PCL ProcessDef] SetParameterProcessVersionRange:" << minVer
           << "to" << maxVer;
}

static void mock_SetParameterRequired(api_bool required) {
  qDebug() << "[PCL ProcessDef] SetParameterRequired:" << (bool)required;
}

static void mock_SetParameterReadOnly(api_bool readOnly) {
  qDebug() << "[PCL ProcessDef] SetParameterReadOnly:" << (bool)readOnly;
}

static void mock_SetParameterLockRoutine(void *routine) {
  qDebug() << "[PCL ProcessDef] SetParameterLockRoutine:" << routine;
}

static void mock_SetParameterAllocationRoutine(void *routine) {
  qDebug() << "[PCL ProcessDef] SetParameterAllocationRoutine:" << routine;
}

static void mock_SetParameterLengthQueryRoutine(void *routine) {
  qDebug() << "[PCL ProcessDef] SetParameterLengthQueryRoutine:" << routine;
}

static void mock_SetDefaultBooleanValue(api_bool value) {
  qDebug() << "[PCL ProcessDef] SetDefaultBooleanValue:" << (bool)value;
}

static void mock_SetDefaultNumericValue(double value) {
  qDebug() << "[PCL ProcessDef] SetDefaultNumericValue:" << value;
}

static void mock_SetValidNumericRange(double minVal, double maxVal) {
  qDebug() << "[PCL ProcessDef] SetValidNumericRange:" << minVal << "to"
           << maxVal;
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

static api_bool mock_GetControlResourcePixelRatio(const_control_handle h,
                                                  double *ratio) {
  if (ratio) {
    *ratio = 1.0;
    return true;
  }
  return false;
}

static control_handle mock_CreateViewList(api_handle hModule, api_handle client,
                                          control_handle parent, uint32 flags) {
  QWidget *parentWidget = reinterpret_cast<QWidget *>(parent);
  QComboBox *cb = new QComboBox(parentWidget);
  cb->addItem("Active Image View");
  control_handle ch = reinterpret_cast<control_handle>(cb);
  if (client)
    g_widgetToClient[ch] = client;
  qDebug() << "[PCL Bridge] CreateViewList (mocked as QComboBox): client ="
           << client << "parent =" << parentWidget << "-> QComboBox =" << cb;
  return ch;
}

static void mock_RegenerateViewList(control_handle h, api_bool mainViews,
                                    api_bool previews,
                                    api_bool realTimePreview) {
  qDebug() << "[PCL Bridge] RegenerateViewList:" << h;
}

static void mock_GetViewListContents(const_control_handle h,
                                     api_bool *mainViews, api_bool *previews,
                                     api_bool *realTimePreview) {
  qDebug() << "[PCL Bridge] GetViewListContents:" << h;
  if (mainViews)
    *mainViews = true;
  if (previews)
    *previews = false;
  if (realTimePreview)
    *realTimePreview = false;
}

static int32 mock_ExecuteDialog(control_handle h) {
  ::QDialog *dlg = reinterpret_cast<::QDialog *>(h);
  qDebug() << "[PCL Bridge] ExecuteDialog:" << dlg;
  if (dlg) {
    return dlg->exec();
  }
  return 0;
}

static api_bool mock_GetEditValid(const_control_handle h) {
  const ::QLineEdit *edit = reinterpret_cast<const ::QLineEdit *>(h);
  if (edit) {
    if (edit->validator()) {
      QString text = edit->text();
      int pos = 0;
      return edit->validator()->validate(text, pos) == QValidator::Acceptable;
    }
    return true;
  }
  return false;
}

static void mock_SetButtonDefaultEnabled(control_handle h, api_bool enable) {
  QPushButton *btn = reinterpret_cast<QPushButton *>(h);
  qDebug() << "[PCL Bridge] SetButtonDefaultEnabled:" << btn << (bool)enable;
  if (btn) {
    btn->setDefault(enable);
  }
}

static void mock_ReturnDialog(control_handle h, int32 result) {
  ::QDialog *dlg = reinterpret_cast<::QDialog *>(h);
  qDebug() << "[PCL Bridge] ReturnDialog:" << dlg << "result =" << result;
  if (dlg) {
    dlg->done(result);
  }
}

static void
mock_SetProcessEditPreferencesRoutine(pcl::process_edit_preferences_routine f) {
  if (g_currentDefiningProcess) {
    g_processes[g_currentDefiningProcess].editPreferencesFn = f;
    g_processes[g_currentDefiningProcess].hasEditPreferences = (f != nullptr);
    qDebug() << "[PCL Bridge] Process edit preferences routine set for:"
             << g_processes[g_currentDefiningProcess].id
             << "hasEditPreferences ="
             << g_processes[g_currentDefiningProcess].hasEditPreferences;
  }
}

static api_bool mock_EditProcessPreferences(meta_process_handle hMetaProcess) {
  qDebug()
      << "[PCL Bridge] mock_EditProcessPreferences called for meta-process:"
      << hMetaProcess;
  if (hMetaProcess) {
    auto it = g_processes.find(hMetaProcess);
    if (it != g_processes.end() && it->second.editPreferencesFn) {
      qDebug() << "[PCL Bridge] Invoking process edit preferences routine...";
      api_bool ok = it->second.editPreferencesFn(hMetaProcess);
      qDebug() << "[PCL Bridge] Process edit preferences routine returned:"
               << ok;
      return ok;
    }
  }
  return false;
}

static void mock_SetProcessClassInitializationRoutine(
    pcl::process_class_initialization_routine f) {
  if (g_currentDefiningProcess) {
    g_processes[g_currentDefiningProcess].classInitFn = f;
    qDebug() << "[PCL Bridge] Process class initialization routine set for:"
             << g_processes[g_currentDefiningProcess].id;
  }
}

static void mock_SetProcessExecutionPreferencesRoutine(
    pcl::process_execution_preferences_routine f) {
  if (g_currentDefiningProcess) {
    g_processes[g_currentDefiningProcess].executionPreferencesFn = f;
    qDebug() << "[PCL Bridge] Process execution preferences routine set for:"
             << g_processes[g_currentDefiningProcess].id;
  }
}

// ---------------------------------------------------------------------------
// File / directory picker dialogs
// ---------------------------------------------------------------------------

// Helper: convert a PCL char16_type* string (UTF-16) to a QString
static QString char16ToString(const char16_type *s) {
  if (!s)
    return {};
  return QString::fromUtf16(reinterpret_cast<const char16_t *>(s));
}

// Helper: write a QString back into a caller-owned char16_type buffer
static void stringToChar16(const QString &src, char16_type *dst) {
  if (!dst)
    return;
  const ushort *utf16 = src.utf16();
  int len = src.size();
  for (int i = 0; i < len; ++i)
    dst[i] = static_cast<char16_type>(utf16[i]);
  dst[len] = 0;
}

// Convert PCL "Name (*.ext *.ext2);;..." filters to Qt "Name (*.ext
// *.ext2);;..." (already compatible)
static QString pclFiltersToQt(const char16_type *filters) {
  return char16ToString(filters);
}

static api_bool
mock_ExecuteOpenFileDialog(char16_type *fileName, const char16_type *caption,
                           const char16_type *initialPath,
                           const char16_type *filters,
                           const char16_type * /*selectedExtension*/) {
  QString cap = char16ToString(caption);
  QString init = char16ToString(initialPath);
  QString filt = pclFiltersToQt(filters);
  qDebug() << "[PCL Bridge] ExecuteOpenFileDialog: caption =" << cap
           << "initial =" << init;
  QString chosen = QFileDialog::getOpenFileName(nullptr, cap, init, filt);
  qDebug() << "[PCL Bridge] ExecuteOpenFileDialog: chosen =" << chosen;
  if (chosen.isEmpty())
    return false;
  stringToChar16(chosen, fileName);
  // Verify write back by reading from the buffer
  QString verify = char16ToString(fileName);
  qDebug() << "[PCL Bridge] ExecuteOpenFileDialog: written to buffer ="
           << verify;
  return true;
}

static api_bool mock_ExecuteOpenMultipleFilesDialog(
    char16_type * /*firstFileName*/, ::file_enumeration_callback callback,
    void *callbackData, const char16_type *caption,
    const char16_type *initialPath, const char16_type *filters,
    const char16_type * /*selectedExtension*/) {
  QString cap = char16ToString(caption);
  QString init = char16ToString(initialPath);
  QString filt = pclFiltersToQt(filters);
  qDebug() << "[PCL Bridge] ExecuteOpenMultipleFilesDialog: caption =" << cap;
  QStringList chosen = QFileDialog::getOpenFileNames(nullptr, cap, init, filt);
  if (chosen.isEmpty())
    return false;
  for (const QString &path : chosen) {
    const ushort *utf16 = path.utf16();
    int len = path.size();
    std::vector<char16_type> buf(len + 1);
    for (int i = 0; i < len; ++i)
      buf[i] = static_cast<char16_type>(utf16[i]);
    buf[len] = 0;
    if (callback) {
      api_bool cont = callback(
          reinterpret_cast<const char16_type *>(buf.data()), callbackData);
      if (!cont)
        break;
    }
  }
  return true;
}

static api_bool mock_ExecuteSaveFileDialog(
    char16_type *filePath, const char16_type *caption,
    const char16_type *initialPath, const char16_type *filters,
    const char16_type * /*selectedExtension*/, api_bool /*overwritePrompt*/) {
  QString cap = char16ToString(caption);
  QString init = char16ToString(initialPath);
  QString filt = pclFiltersToQt(filters);
  qDebug() << "[PCL Bridge] ExecuteSaveFileDialog: caption =" << cap;
  QString chosen = QFileDialog::getSaveFileName(nullptr, cap, init, filt);
  if (chosen.isEmpty())
    return false;
  stringToChar16(chosen, filePath);
  return true;
}

static api_bool mock_ExecuteGetDirectoryDialog(char16_type *dirPath,
                                               const char16_type *caption,
                                               const char16_type *initialPath) {
  QString cap = char16ToString(caption);
  QString init = char16ToString(initialPath);
  qDebug() << "[PCL Bridge] ExecuteGetDirectoryDialog: caption =" << cap;
  QString chosen = QFileDialog::getExistingDirectory(nullptr, cap, init);
  if (chosen.isEmpty())
    return false;
  stringToChar16(chosen, dirPath);
  return true;
}

} // namespace blastro

// ==========================================================================
// PCL Timer implementation — backed by QTimer
// ==========================================================================

namespace blastro {

struct PCLTimer {
  QTimer *qtimer = nullptr;
  api_handle receiver = nullptr;
  pcl::timer_event_routine callback = nullptr;
  timer_handle self = nullptr;

  PCLTimer() {
    qtimer = new QTimer();
    self = reinterpret_cast<timer_handle>(this);
    // Lambda captures `this` so the callback can be fired without Q_OBJECT/MOC
    QObject::connect(qtimer, &QTimer::timeout, [this]() {
      if (callback)
        callback(self, reinterpret_cast<control_handle>(receiver));
    });
  }

  ~PCLTimer() {
    qtimer->stop();
    delete qtimer;
    qtimer = nullptr;
  }
};

static timer_handle mock_CreateTimer(api_handle /*hModule*/,
                                     api_handle /*client*/, uint32 /*flags*/) {
  PCLTimer *t = new PCLTimer();
  qDebug() << "[PCL Bridge] CreateTimer ->" << reinterpret_cast<void *>(t);
  return reinterpret_cast<timer_handle>(t);
}

static void mock_GetTimerInterval(const_timer_handle h, uint32 *msec) {
  if (!h || !msec)
    return;
  const PCLTimer *t = reinterpret_cast<const PCLTimer *>(h);
  *msec = static_cast<uint32>(t->qtimer->interval());
}

static void mock_SetTimerInterval(timer_handle h, uint32 msec) {
  if (!h)
    return;
  PCLTimer *t = reinterpret_cast<PCLTimer *>(h);
  qDebug() << "[PCL Bridge] SetTimerInterval" << msec << "ms";
  t->qtimer->setInterval(static_cast<int>(msec));
}

static api_bool mock_GetTimerSingleShot(const_timer_handle h) {
  if (!h)
    return false;
  const PCLTimer *t = reinterpret_cast<const PCLTimer *>(h);
  return t->qtimer->isSingleShot();
}

static void mock_SetTimerSingleShot(timer_handle h, api_bool singleShot) {
  if (!h)
    return;
  PCLTimer *t = reinterpret_cast<PCLTimer *>(h);
  qDebug() << "[PCL Bridge] SetTimerSingleShot" << (bool)singleShot;
  t->qtimer->setSingleShot(singleShot);
}

static api_bool mock_IsTimerActive(const_timer_handle h) {
  if (!h)
    return false;
  const PCLTimer *t = reinterpret_cast<const PCLTimer *>(h);
  return t->qtimer->isActive();
}

static api_bool mock_StartTimer(timer_handle h) {
  if (!h)
    return false;
  PCLTimer *t = reinterpret_cast<PCLTimer *>(h);
  qDebug() << "[PCL Bridge] StartTimer interval =" << t->qtimer->interval()
           << "ms";
  t->qtimer->start();
  return true;
}

static void mock_StopTimer(timer_handle h) {
  if (!h)
    return;
  PCLTimer *t = reinterpret_cast<PCLTimer *>(h);
  qDebug() << "[PCL Bridge] StopTimer";
  t->qtimer->stop();
}

static api_bool
mock_SetTimerNotifyEventRoutine(timer_handle h, api_handle receiver,
                                pcl::timer_event_routine routine) {
  if (!h)
    return false;
  PCLTimer *t = reinterpret_cast<PCLTimer *>(h);
  t->receiver = receiver;
  t->callback = routine;
  qDebug() << "[PCL Bridge] SetTimerNotifyEventRoutine receiver =" << receiver
           << "routine =" << (bool)routine;
  return true;
}

} // namespace blastro
