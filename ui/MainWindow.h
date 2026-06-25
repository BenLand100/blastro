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

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

    void loadAndShowPlugin(const QString& path);
    QMdiSubWindow* createPluginSubWindow(QWidget* widget, const QString& title);
    bool executePCLProcessOnActiveImage(const QString& processId, void* hProcess);
    void testProcessOnImage(const QString& pluginPath, const QString& imagePath);
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
    void onSubWindowActivated(QMdiSubWindow* window);
    void updateStatusReadout(int x, int y, bool isRGB, const std::vector<float>& values);
    
    void executeAlgorithmSlot(const std::string& name, const std::map<std::string, std::string>& config);
    
    // Plugins
    void onLoadPCLModule();

private:
    void createMenus();
    void addImageToWorkspace(const QString& name, const WorkspaceElement& element);

    WorkspaceRegistry m_workspace;
    WorkspaceArea* m_workspaceArea;
    int m_imageCounter;
    QLabel* m_statusReadout = nullptr;
    ImageView* m_connectedImageView = nullptr;
    QProgressBar* m_progressBar = nullptr;
    bool m_algorithmRunning = false;

    std::unique_ptr<PCLBridge> m_pclBridge;

    // Menus
    QMenu* m_fileMenu;
    QMenu* m_algoMenu;

    // Actions
    QAction* m_openAct;
    QAction* m_openBatchAct;
    QAction* m_addToBatchAct;
    QAction* m_saveBatchAct;
    QAction* m_saveAct;
    QAction* m_exitAct;
    QAction* m_pixelMathAct;
    QAction* m_stackingAct;
    QAction* m_calibrationAct;
    QAction* m_loadPluginAct = nullptr;
};

} // namespace blastro
