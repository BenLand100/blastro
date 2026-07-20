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
#include "ImageView.h"
#include "BatchImageWidget.h"
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMap>

namespace blastro {

class WorkspaceImageWindow;

class WorkspaceArea : public QMdiArea {
    Q_OBJECT
public:
    explicit WorkspaceArea(QWidget* parent = nullptr);
    ~WorkspaceArea() override = default;

    // Adds a workspace element visually as an MDI window (visible or hidden)
    QMdiSubWindow* addElementView(const QString& name, const WorkspaceElement& element, bool visible = true);
    
    // Shows an existing element view, creating it if needed
    void showElementView(const QString& name);

    // Checks if an element view is currently visible on screen
    bool isElementViewVisible(const QString& name) const;

    // Removes the view of a workspace element
    void removeElementView(const QString& name);
    
    // Renames an existing view's window title
    void renameElementView(const QString& oldName, const QString& newName);

    // Retrieves a WorkspaceImageWindow pointer by name
    WorkspaceImageWindow* getImageWindow(const QString& name) const;
    
    // Retrieves the currently active image view (handles single image or active frame of batch)
    ImageVariant getActiveImage() const;
    QString getActiveImageName() const;

signals:
    void elementRenameRequested(const QString& oldName, const QString& newName);
    void elementClosed(const QString& name);

private:
    QMap<QString, QMdiSubWindow*> m_subWindows;
    int m_cascadeIndex = 0;
    int m_cascadeColumn = 0;
};

} // namespace blastro
