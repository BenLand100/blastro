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
    connect(viewWidget, &WorkspaceImageWindow::renameRequested, this, &WorkspaceArea::elementRenameRequested);

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

        if (auto* win = qobject_cast<WorkspaceImageWindow*>(sub->widget())) {
            win->updateName(newName);
        }
    }
}

ImageVariant WorkspaceArea::getActiveImage() const {
    // Search in activation history order (most recently active first)
    QList<QMdiSubWindow*> windows = subWindowList(QMdiArea::ActivationHistoryOrder);
    for (QMdiSubWindow* sub : windows) {
        if (!sub) continue;
        QWidget* widget = sub->widget();
        if (auto win = qobject_cast<WorkspaceImageWindow*>(widget)) {
            return win->currentImage();
        }
    }
    return ImageVariant();
}

QString WorkspaceArea::getActiveImageName() const {
    // Search in activation history order (most recently active first)
    QList<QMdiSubWindow*> windows = subWindowList(QMdiArea::ActivationHistoryOrder);
    for (QMdiSubWindow* sub : windows) {
        if (!sub) continue;
        QWidget* widget = sub->widget();
        if (auto win = qobject_cast<WorkspaceImageWindow*>(widget)) {
            return sub->windowTitle();
        }
    }
    return QString();
}

WorkspaceImageWindow* WorkspaceArea::getImageWindow(const QString& name) const {
    if (m_subWindows.contains(name)) {
        QMdiSubWindow* sub = m_subWindows[name];
        if (sub) {
            return qobject_cast<WorkspaceImageWindow*>(sub->widget());
        }
    }
    return nullptr;
}

} // namespace blastro
