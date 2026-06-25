#pragma once
#include "core/WorkspaceRegistry.h"
#include "WorkspaceArea.h"
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

namespace blastro {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onOpenImage();
    void onOpenBatch();
    void onSaveActiveImage();
    void onOpenPixelMath();
    
    void executeAlgorithmSlot(const std::string& name, const std::map<std::string, std::string>& config);

private:
    void createMenus();
    void addImageToWorkspace(const QString& name, const WorkspaceElement& element);

    WorkspaceRegistry m_workspace;
    WorkspaceArea* m_workspaceArea;
    int m_imageCounter;

    // Menus
    QMenu* m_fileMenu;
    QMenu* m_algoMenu;

    // Actions
    QAction* m_openAct;
    QAction* m_openBatchAct;
    QAction* m_saveAct;
    QAction* m_exitAct;
    QAction* m_pixelMathAct;
};

} // namespace blastro
