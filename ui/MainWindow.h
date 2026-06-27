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

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

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
    void onOpenGhs();
    void onSubWindowActivated(QMdiSubWindow* window);
    void updateStatusReadout(int x, int y, bool isRGB, const std::vector<float>& values);
    
    void executeAlgorithmSlot(const std::string& name, const std::map<std::string, std::string>& config);
    
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
    void updateImageMenuState();

private:
    void createMenus();
    void addImageToWorkspace(const QString& name, const WorkspaceElement& element);

    WorkspaceRegistry m_workspace;
    WorkspaceArea* m_workspaceArea;
    int m_imageCounter;
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
    QAction* m_loadPluginAct = nullptr;
    QAction* m_checkForUpdatesAct = nullptr;
    QAction* m_pclSeparatorBelow = nullptr;
};

} // namespace blastro
