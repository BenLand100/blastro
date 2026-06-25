#pragma once
#include "core/WorkspaceRegistry.h"
#include "ImageView.h"
#include "BatchImageWidget.h"
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMap>

namespace blastro {

class WorkspaceArea : public QMdiArea {
    Q_OBJECT
public:
    explicit WorkspaceArea(QWidget* parent = nullptr);
    ~WorkspaceArea() override = default;

    // Adds a workspace element visually as an MDI window
    QMdiSubWindow* addElementView(const QString& name, const WorkspaceElement& element);
    
    // Removes the view of a workspace element
    void removeElementView(const QString& name);
    
    // Renames an existing view's window title
    void renameElementView(const QString& oldName, const QString& newName);
    
    // Retrieves the currently active image view (handles single image or active frame of batch)
    ImageVariant getActiveImage() const;
    QString getActiveImageName() const;

private:
    QMap<QString, QMdiSubWindow*> m_subWindows;
};

} // namespace blastro
