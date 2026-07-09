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
#include "HistogramWidget.h"
#include <QWidget>
#include <QJsonObject>
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>

namespace blastro {

class WorkspaceImageWindow : public QWidget {
    Q_OBJECT
public:
    WorkspaceImageWindow(const QString& name, const WorkspaceElement& element, QWidget* parent = nullptr);
    ~WorkspaceImageWindow() override = default;

    ImageVariant currentImage() const;
    ImageVariant originalImage() const;
    ImageView* imageView() const { return m_imageView; }
    WorkspaceElement element() const { return m_element; }
    QWidget* viewportWidget() const { return m_viewportWidget; }
    QString name() const { return m_name; }

    void updateName(const QString& newName);
    void notifyImageUpdated();
    void setUpdatesSuspended(bool suspended);

    void setElement(const WorkspaceElement& element, bool preserveZoom = false);

    // Undo/Redo support
    void undo();
    void redo();
    bool canUndo() const { return !m_undoStack.empty(); }
    bool canRedo() const { return !m_redoStack.empty(); }
    void saveUndoState();
    void clearUndoRedo();

    // Serialization
    QJsonObject serializeWindowState() const;
    void restoreWindowState(const QJsonObject& obj);

    // Live Preview Support
    void setPreviewImage(const ImageVariant& previewImage);
    void restoreOriginalImage();
    void commitPreviewImage(const ImageVariant& finalImage);

signals:
    void renameRequested(const QString& oldName, const QString& newName);
    void undoRedoStateChanged();

private slots:
    void onModeButtonClicked(int id);
    void onStretchParamsChangedInWidget(double b, double w, double m);
    void onStretchParamsChangedInView(double b, double w, double m);
    void onFrameChanged(int index);
    void onRenameClicked();

private:
    void updateHistogram();

    QString m_name;
    WorkspaceElement m_element;
    
    ImageView* m_imageView;
    QWidget* m_viewportWidget; // ImageView or BatchImageWidget

    ImageVariant m_originalImageForPreview;
    bool m_hasPreviewActive = false;

    // UI elements
    QWidget* m_headerBar;
    QButtonGroup* m_modeGroup;
    QPushButton* m_normalBtn;
    QPushButton* m_stretchBtn;
    QPushButton* m_autoBtn;
    QPushButton* m_localHistBtn;
    QPushButton* m_expandHistBtn;
    QPushButton* m_nameBtn;

    // Collapsible Histogram layout elements
    QWidget* m_headerHistContainer;
    QWidget* m_expandedHistBar;
    bool m_histExpanded = false;
    int m_currentFrame = 0;  // tracks current batch frame for serialization

    // Channel selection buttons for RGB Images
    QButtonGroup* m_channelGroup;
    QPushButton* m_rChanBtn;
    QPushButton* m_gChanBtn;
    QPushButton* m_bChanBtn;
    QPushButton* m_rgbChanBtn;

    HistogramWidget* m_histogramWidget;

    struct UndoState {
        ImageVariant image;
    };
    std::vector<UndoState> m_undoStack;
    std::vector<UndoState> m_redoStack;
    const size_t m_maxUndoDepth = 10;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void onExpandHistClicked();
    void updateNameLabelText();
};

} // namespace blastro
