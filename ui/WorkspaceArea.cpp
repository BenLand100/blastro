#include "WorkspaceArea.h"
#include "WorkspaceImageWindow.h"
#include <QMdiSubWindow>
#include <QVBoxLayout>
#include <QMessageBox>

namespace blastro {

WorkspaceArea::WorkspaceArea(QWidget* parent)
    : QMdiArea(parent) {
    setViewMode(QMdiArea::SubWindowView);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setStyleSheet("background-color: #121212;");
}

QMdiSubWindow* WorkspaceArea::addElementView(const QString& name, const WorkspaceElement& element) {
    if (m_subWindows.contains(name)) {
        // Bring existing window to focus
        m_subWindows[name]->setFocus();
        return m_subWindows[name];
    }

    WorkspaceImageWindow* viewWidget = new WorkspaceImageWindow(name, element, this);

    // Create MDI subwindow
    QMdiSubWindow* sub = addSubWindow(viewWidget);
    sub->setWindowTitle(name);
    sub->setAttribute(Qt::WA_DeleteOnClose);
    sub->resize(600, 450);
    
    // Connect window destroyed signal to clean up our local map
    connect(sub, &QObject::destroyed, this, [this, name]() {
        m_subWindows.remove(name);
    });

    m_subWindows[name] = sub;
    sub->show();
    return sub;
}

void WorkspaceArea::removeElementView(const QString& name) {
    if (m_subWindows.contains(name)) {
        QMdiSubWindow* sub = m_subWindows[name];
        m_subWindows.remove(name);
        sub->close();
    }
}

void WorkspaceArea::renameElementView(const QString& oldName, const QString& newName) {
    if (m_subWindows.contains(oldName)) {
        QMdiSubWindow* sub = m_subWindows[oldName];
        m_subWindows.remove(oldName);
        m_subWindows[newName] = sub;
        sub->setWindowTitle(newName);
        
        // Update destruction connection
        disconnect(sub, &QObject::destroyed, this, nullptr);
        connect(sub, &QObject::destroyed, this, [this, newName]() {
            m_subWindows.remove(newName);
        });
    }
}

ImageVariant WorkspaceArea::getActiveImage() const {
    QMdiSubWindow* activeSub = activeSubWindow();
    if (!activeSub) return ImageVariant();

    QWidget* widget = activeSub->widget();
    if (auto win = qobject_cast<WorkspaceImageWindow*>(widget)) {
        return win->currentImage();
    }
    
    return ImageVariant();
}

QString WorkspaceArea::getActiveImageName() const {
    QMdiSubWindow* activeSub = activeSubWindow();
    if (!activeSub) return QString();
    return activeSub->windowTitle();
}

} // namespace blastro
