/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#pragma once
#include <QWidget>
#include <QPixmap>
#include <vector>

namespace blastro {

class HistogramBaseWidget : public QWidget {
    Q_OBJECT
public:
    explicit HistogramBaseWidget(QWidget* parent = nullptr);
    ~HistogramBaseWidget() override = default;

    void setHistograms(const std::vector<std::vector<int>>& histos);
    void setChannelsLinked(bool linked);
    void setActive(bool active);
    void setActiveChannel(int channel);
    void setSingleChannelColor(const QColor& color);
    void setSingleTraceColor(const QColor& color);

    void resetZoom();
    void snapToBlackToMid(double blackpoint, double midpoint, double whitepoint);

    void setDrawCurve(bool draw) { m_drawCurve = draw; m_cacheDirty = true; update(); }
    bool drawCurve() const { return m_drawCurve; }

    bool isActive() const { return m_active; }
    bool isWidgetChannelsLinked() const {
        return m_channelsLinked || m_activeChannel == 0 || m_activeChannel == 4 || m_activeChannel == 5;
    }

protected:
    double valueToX(double val) const;
    double xToValue(double x) const;

    void drawBaseBackground(QPainter& bgPainter, int w, int h, float lineWidth);

    // Event overrides for shared Zoom and Pan
    void wheelEvent(QWheelEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    // Shared state variables
    std::vector<std::vector<int>> m_histograms;
    bool m_channelsLinked = true;
    bool m_active = true;
    bool m_drawCurve = true;
    int m_activeChannel = 0; // 0=K, 1=R, 2=G, 3=B, 4=L, 5=S
    QColor m_singleChannelColor = QColor("#ffffff");
    QColor m_singleTraceColor = Qt::white;

    double m_zoom = 1.0;
    double m_scrollOffset = 0.0;
    bool m_isScrolling = false;
    QPoint m_lastMousePos;

    QPixmap m_cachedBackground;
    bool m_cacheDirty = true;
};

} // namespace blastro
