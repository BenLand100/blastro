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
#include "core/WorkspaceRegistry.h"
#include "WorkspaceArea.h"
#include "ImageView.h"
#include "core/PCLBridge.h"
#include "core/ProjectSerializer.h"
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QMdiSubWindow>
#include <QProgressBar>
#include <QThread>
#include <QCloseEvent>
#include <QObject>
#include <memory>
#include <unordered_map>

// Forward declarations for persistent algorithm dialogs
namespace blastro {
class StretchingDialog;
class BackgroundExtractionDialog;
class StackingDialog;
class RegisterDialog;
class AlignDialog;
class DebayerDialog;
class CalibrationDialog;
class PixelMathDialog;
}

namespace blastro {

class AlgorithmWorker : public QObject {
    Q_OBJECT
public:
    AlgorithmWorker(const std::string& name,
                    const std::map<std::string, std::string>& config,
                    WorkspaceRegistry& workspace,
                    QObject* parent = nullptr)
        : QObject(parent), m_name(name), m_config(config), m_workspace(workspace) {}
    ~AlgorithmWorker() override = default;

public slots:
    void run();

signals:
    void progressUpdated(int percent);
    void finished(bool success, QString errorMsg);

private:
    std::string m_name;
    std::map<std::string, std::string> m_config;
    WorkspaceRegistry& m_workspace;
};

class PCLProcessWorker : public QObject {
    Q_OBJECT
public:
    PCLProcessWorker(const QString& processId,
                     void* hProcess,
                     const std::vector<ImageBufferPtr>& buffers,
                     PCLBridge* bridge,
                     QObject* parent = nullptr)
        : QObject(parent), m_processId(processId), m_hProcess(hProcess), m_buffers(buffers), m_bridge(bridge) {}
    ~PCLProcessWorker() override = default;

public slots:
    void run();

signals:
    void progressUpdated(int percent);
    void finished(bool success, QString errorMsg);

private:
    QString m_processId;
    void* m_hProcess;
    std::vector<ImageBufferPtr> m_buffers;
    PCLBridge* m_bridge;
};

class PreprocessingWizardDialog;

/// Options parsed from the command line and passed to MainWindow::applyStartupOptions().
struct StartupOptions {
    bool noRestore  = false;   ///< --no-restore : skip loading last_session.json
    QString projectPath;       ///< --project <path> : open this project on launch
    QString sessionPath;       ///< --session <path> : load this specific session file
};

class MainWindow : public QMainWindow {
    Q_OBJECT
    friend class PreprocessingWizardDialog;
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

    /// Called (once, after show()) to apply command-line startup options.
    void applyStartupOptions(const StartupOptions& opts);

    /// Open a project by directory path. Sets CWD, loads workspace, restores state.
    bool openProject(const QString& projectDir);

    /// Save current project (requires m_projectPath to be set).
    bool saveProject();

    /// Save project to a new directory (Save As…).
    bool saveProjectAs(const QString& projectDir);

    void loadAndShowPlugin(const QString& path);
    QMdiSubWindow* createPluginSubWindow(QWidget* widget, const QString& title);
    QMdiSubWindow* createPCLPluginSubWindow(QWidget* widget, const QString& processId, const QString& title);
    bool executePCLProcessOnActiveImage(const QString& processId, void* hProcess);
    void testProcessOnImage(const QString& pluginPath, const QString& imagePath);
    void testRegisterOnCube(const QString& cubePath, int refFrameIdx = 0, const QString& detectionMethod = "centroid");
    bool loadImageDirectly(const QString& filepath, const QString& refName);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onOpenImage();
    void onOpenBatch();
    void onAddToBatch();
    void onSaveActiveBatch();
    void onSaveActiveImage();
    void onOpenPixelMath();
    void onOpenStacking();
    void onOpenCalibration();
    void onOpenDebayer();
    void onOpenRegister();
    void onOpenAlign();
    void onOpenBackgroundExtraction();
    void onOpenPpw();
    void onOpenGhs();
    void onSubWindowActivated(QMdiSubWindow* window);
    void updateStatusReadout(int x, int y, bool isRGB, const std::vector<float>& values);
    
    void executeAlgorithmSlot(const std::string& name, const std::map<std::string, std::string>& config);

    // Project / session
    void onNewProject();
    void onOpenProject();
    void onSaveProject();
    void onSaveProjectAs();

    // Batch reload (invoked via QMetaObject by ProjectSerializer)
    Q_INVOKABLE void loadBatchPaths(const QStringList& paths, const QString& name);
    
    // Plugins
    void onLoadPCLModule();
    void onCheckForUpdates();

private slots:
    void updateWindowMenu();
    void restoreProcessConsole();
    void onRenameElement(const QString& oldName, const QString& newName);
    void onOpenPreferences();

    void ensurePCLBridge();
    void setProcessingState(bool processing);

    // Image operations
    void onUndo();
    void onRedo();
    void onFlipVertical();
    void onMirrorHorizontal();
    void onRotate90CW();
    void onRotate90CCW();
    void onRotate180();
    void onCrop();
    void onToggleShowBgeControlPoints(bool checked);
    void updateImageMenuState();

private:
    void createMenus();
    void addImageToWorkspace(const QString& name, const WorkspaceElement& element);

    /// Build a DialogSet pointing to all persistent dialog members.
    DialogSet buildDialogSet() const;

    /// Returns the default session file path (in app config dir).
    static QString defaultSessionPath();

    WorkspaceRegistry m_workspace;
    WorkspaceArea* m_workspaceArea;
    int m_imageCounter;
    QString m_projectPath;  ///< Directory of the currently open project (empty = none)
    QLabel* m_statusReadout = nullptr;
    QLabel* m_statusLabel = nullptr;
    ImageView* m_connectedImageView = nullptr;
    WorkspaceImageWindow* m_connectedImageWindow = nullptr;
    QProgressBar* m_progressBar = nullptr;
    void showStatusMessage(const QString& message, int timeout = 0);
    bool m_algorithmRunning = false;

    std::unique_ptr<PCLBridge> m_pclBridge;
    std::unordered_map<QString, QMdiSubWindow*> m_openPCLInterfaces;

    void addPCLProcessToMenu(const QString& processId);
    void openPCLInterface(const QString& processId);

    // Persistent algorithm dialogs (owned by MainWindow)
    StretchingDialog*          m_stretchingDlg  = nullptr;
    BackgroundExtractionDialog* m_bgeDlg        = nullptr;
    StackingDialog*            m_stackingDlg    = nullptr;
    RegisterDialog*            m_registerDlg    = nullptr;
    AlignDialog*               m_alignDlg       = nullptr;
    DebayerDialog*             m_debayerDlg     = nullptr;
    CalibrationDialog*         m_calibrationDlg = nullptr;
    PixelMathDialog*           m_pixelMathDlg   = nullptr;
    PreprocessingWizardDialog* m_ppwDlg         = nullptr;

    // Persistent algorithm dialog subwindows
    QMdiSubWindow*             m_stretchingSub  = nullptr;
    QMdiSubWindow*             m_bgeSub         = nullptr;
    QMdiSubWindow*             m_stackingSub    = nullptr;
    QMdiSubWindow*             m_registerSub    = nullptr;
    QMdiSubWindow*             m_alignSub       = nullptr;
    QMdiSubWindow*             m_debayerSub     = nullptr;
    QMdiSubWindow*             m_calibrationSub = nullptr;
    QMdiSubWindow*             m_pixelMathSub   = nullptr;
    QMdiSubWindow*             m_ppwSub         = nullptr;

    // Menus
    QMenu* m_fileMenu;
    QMenu* m_imageMenu;
    QMenu* m_algoMenu;
    QMenu* m_windowMenu;

    // Actions
    QAction* m_undoAct;
    QAction* m_redoAct;
    QAction* m_flipVertAct;
    QAction* m_mirrorHorizAct;
    QAction* m_rotate90CWAct;
    QAction* m_rotate90CCWAct;
    QAction* m_rotate180Act;
    QAction* m_cropAct;
    QAction* m_showBgeControlPointsAct;
    QAction* m_openAct;
    QAction* m_openBatchAct;
    QAction* m_addToBatchAct;
    QAction* m_saveBatchAct;
    QAction* m_saveAct;
    QAction* m_exitAct;
    QAction* m_pixelMathAct;
    QAction* m_stackingAct;
    QAction* m_calibrationAct;
    QAction* m_debayerAct;
    QAction* m_registerAct;
    QAction* m_alignAct;
    QAction* m_backgroundAct;
    QAction* m_stretchAct;
    QAction* m_newProjectAct    = nullptr;
    QAction* m_openProjectAct   = nullptr;
    QAction* m_saveProjectAct   = nullptr;
    QAction* m_saveProjectAsAct = nullptr;
    QAction* m_loadPluginAct    = nullptr;
    QAction* m_checkForUpdatesAct = nullptr;
    QAction* m_pclSeparatorBelow  = nullptr;
};

} // namespace blastro
