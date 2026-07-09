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
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include "algorithms/StarFinder.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QJsonObject>
#include <QWheelEvent>

namespace blastro {

class ImageView : public QGraphicsView {
    Q_OBJECT
public:
    enum DisplayMode {
        Normal,
        Stretch,
        Autostretch,
        LocalHist
    };

    enum ChannelMode {
        RGB_ALL,
        RED_ONLY,
        GREEN_ONLY,
        BLUE_ONLY
    };

    explicit ImageView(QWidget* parent = nullptr);
    ~ImageView() override = default;

    void setImage(const ImageVariant& image, bool preserveStretch = false, bool preserveZoom = false);
    ImageVariant currentImage() const { return m_currentImage; }
    int autoStretchLevel() const { return m_autoStretchLevel; }
    int localHistLevel() const { return m_localHistLevel; }

    void setUpdatesSuspended(bool suspended);
    bool isUpdatesSuspended() const { return m_updatesSuspended; }
    
    // Channel mode
    ChannelMode channelMode() const { return m_channelMode; }
    void setChannelMode(ChannelMode mode);
    
    // Display modes and stretching
    DisplayMode displayMode() const { return m_displayMode; }
    void setDisplayMode(DisplayMode mode);
    void setAutoStretchLevel(int level);
    void setLocalHistLevel(int level);
    
    double blackpoint() const { return m_blackpoint; }
    double whitepoint() const { return m_whitepoint; }
    double midpoint() const { return m_midpoint; }
    void setStretchParams(double b, double w, double m);
    void runAutostretch();
    
    std::vector<int> getHistogram(int bins = 256) const;

    void zoomIn();
    void zoomOut();
    void resetZoom();
    bool fitToWindow();

    // Serialization
    QJsonObject serializeViewState() const;
    void restoreViewState(const QJsonObject& obj);

    void setFrameSelectedStatus(bool selected);

    QRect selectionRect() const { return m_selectionRect; }
    bool hasSelection() const { return m_hasSelection; }
    void clearSelection();
    QSize currentImageSize() const;

    // Star/Constellation Visualization
    void setShowStars(bool show);
    void setShowConstellations(bool show);
    void setStars(const std::vector<Star>& stars);

    // BGE control points editing mode
    bool bgeMode() const { return m_bgeMode; }
    void setBgeMode(bool enabled);
    std::vector<std::pair<double, double>> getBgeControlPoints() const;
    void setBgeControlPoints(const std::vector<std::pair<double, double>>& pts);
    bool showBgeControlPoints() const { return m_showBgeControlPoints; }
    void setShowBgeControlPoints(bool show, bool manual = false);

signals:
    void stretchParamsChanged(double b, double w, double m);
    void mousePosChanged(int x, int y, bool isRGB, const std::vector<float>& values);
    void selectionChanged();
    void bgeControlPointsVisibilityChanged();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void drawForeground(QPainter* painter, const QRectF& rect) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void updateView();
    void updateLUT();
    float applyMTF(float v, float B, float W, float M);
    void clearCLAHE();

    enum ResizeHandle {
        HandleNone,
        HandleTopLeft,
        HandleTopRight,
        HandleBottomLeft,
        HandleBottomRight,
        HandleLeft,
        HandleRight,
        HandleTop,
        HandleBottom
    };
    ResizeHandle getResizeHandleAt(const QPointF& scenePos) const;
    void updateResizeCursor(ResizeHandle handle);

    QGraphicsScene* m_scene;
    ImageVariant m_currentImage;
    double m_zoomFactor;

    bool m_hasSelection = false;
    bool m_isSelecting = false;
    bool m_isResizing = false;
    QPoint m_selectionStart;
    QRect m_selectionRect;
    ResizeHandle m_activeHandle = HandleNone;

    // Stretching state
    DisplayMode m_displayMode;
    ChannelMode m_channelMode = RGB_ALL;
    int m_autoStretchLevel = 0;
    int m_localHistLevel = 0;
    double m_blackpoint;
    double m_whitepoint;
    double m_midpoint;
    std::vector<unsigned char> m_lut;

    // CLAHE cache
    mutable std::vector<float> m_claheGray;
    mutable std::vector<float> m_claheR;
    mutable std::vector<float> m_claheG;
    mutable std::vector<float> m_claheB;

    // Panning state
    bool m_isPanning;
    QPoint m_lastMousePos;
    bool m_isFrameSelected;
    bool m_fitOnNextResize = false;
    bool m_updatesSuspended = false;
    QImage m_cachedViewportImage;

    bool m_showStars = false;
    bool m_showConstellations = false;
    std::vector<Star> m_stars;

    // BGE control points editing
    bool m_bgeMode = false;
    bool m_showBgeControlPoints = false;
    bool m_bgeControlPointsManuallyToggled = false;
};

} // namespace blastro
