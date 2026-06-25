#pragma once
#include "core/WorkspaceRegistry.h"
#include "WorkspaceArea.h"
#include "ImageView.h"
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QMdiSubWindow>

namespace blastro {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onOpenImage();
    void onOpenBatch();
    void onAddToBatch();
    void onSaveActiveBatch();
    void onSaveActiveImage();
    void onOpenPixelMath();
    void onSubWindowActivated(QMdiSubWindow* window);
    void updateStatusReadout(int x, int y, bool isRGB, const std::vector<float>& values);
    
    void executeAlgorithmSlot(const std::string& name, const std::map<std::string, std::string>& config);

private:
    void createMenus();
    void addImageToWorkspace(const QString& name, const WorkspaceElement& element);

    WorkspaceRegistry m_workspace;
    WorkspaceArea* m_workspaceArea;
    int m_imageCounter;
    QLabel* m_statusReadout = nullptr;
    ImageView* m_connectedImageView = nullptr;

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
};

} // namespace blastro
