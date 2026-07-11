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
#include <QWidget>
#include <QPixmap>
#include <vector>
#include <array>

namespace blastro {

class HistogramWidget : public QWidget {
    Q_OBJECT
public:
    explicit HistogramWidget(QWidget* parent = nullptr);
    ~HistogramWidget() override = default;

    void setHistograms(const std::vector<std::vector<int>>& histos);

    void setSingleTraceColor(const QColor& color) { m_singleTraceColor = color; update(); }

    void setStretchParams(const std::array<double, 3>& b, const std::array<double, 3>& w, const std::array<double, 3>& m);
    void setChannelsLinked(bool linked);
    
    void setGhsMode(bool ghs);
    void setGhsParams(const std::array<double, 3>& sp, const std::array<double, 3>& d);
    void setGhsProtections(const std::array<double, 3>& shadowProtect, const std::array<double, 3>& highlightProtect);
    
    void setDrawCurve(bool draw);
    
    void snapToBlackToMid();
    void resetZoom();
    
    bool isActive() const { return m_active; }
    void setActive(bool active);
    void setActiveChannel(int channel);
    void setSingleChannelColor(const QColor& color);

signals:
    void stretchParamsChanged(const std::array<double, 3>& b, const std::array<double, 3>& w, const std::array<double, 3>& m);
    void ghsParamsChanged(const std::array<double, 3>& sp, const std::array<double, 3>& d);
    void ghsProtectionsChanged(const std::array<double, 3>& shadowProtect, const std::array<double, 3>& highlightProtect);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    struct DragTarget {
        enum Type {
            None,
            Black,
            Mid,
            White,
            SymmetryPoint,
            ShadowProtect,
            HighlightProtect
        };
        Type type = None;
        int channel = 0; // 0=Gray/R, 1=G, 2=B
        
        bool operator==(Type t) const { return type == t; }
        bool operator!=(Type t) const { return type != t; }
    };

    double valueToX(double val) const;
    double xToValue(double x) const;
    DragTarget getCloseLine(const QPoint& pos) const;

    std::vector<std::vector<int>> m_histograms;
    bool m_channelsLinked = true;
    bool m_ghsMode = false;
    bool m_drawCurve = true;
    QColor m_singleTraceColor = Qt::white;
    
    // UI Interaction state
    std::array<double, 3> m_blackpoint = {0.0, 0.0, 0.0};
    std::array<double, 3> m_whitepoint = {1.0, 1.0, 1.0};
    std::array<double, 3> m_midpoint = {0.5, 0.5, 0.5};

    // GHS parameters
    std::array<double, 3> m_spPoint = {0.5, 0.5, 0.5};
    std::array<double, 3> m_stretchFactor = {3.0, 3.0, 3.0};
    std::array<double, 3> m_shadowProtect = {0.0, 0.0, 0.0};
    std::array<double, 3> m_highlightProtect = {1.0, 1.0, 1.0};

    // Zoom & Scroll
    double m_zoom;
    double m_scrollOffset;
    bool m_active = true;
    int m_activeChannel = -1;
    QColor m_singleChannelColor = QColor("#ffffff");

    DragTarget m_dragTarget;
    bool m_isScrolling;
    QPoint m_lastMousePos;
    
    QPixmap m_cachedBackground;
    bool m_cacheDirty = true;
};

} // namespace blastro
