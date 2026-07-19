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
#include "HistogramBaseWidget.h"
#include <QPointF>
#include <array>

namespace blastro {

class CurvesWidget : public HistogramBaseWidget {
    Q_OBJECT
public:
    explicit CurvesWidget(QWidget* parent = nullptr);
    ~CurvesWidget() override = default;

    void setCurvePoints(int channel, const std::vector<QPointF>& points);
    std::vector<QPointF> getCurvePoints(int channel) const;

    // Returns the cached 65536-entry LUT for the given channel (0=K,1=R,2=G,3=B,4=L,5=S).
    // Updated automatically whenever the curve for that channel changes.
    const std::vector<float>& getLut(int channel) const;

signals:
    void curveChanged(int channel, const std::vector<QPointF>& points);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    double valueToY(double val) const;
    double yToValue(double y) const;
    
    int getClosestPointIndex(const QPointF& pos, double& minDist) const;

    // Rebuilds the LUT cache for the given channel from m_points[channel].
    void rebuildLut(int channel);

    // Curve points for K, R, G, B, L, S
    std::array<std::vector<QPointF>, 6> m_points;

    // Per-channel cached LUTs, rebuilt whenever curve points change.
    std::array<std::vector<float>, 6> m_luts;

    int m_dragPointIndex = -1;
    bool m_isDragging = false;
};

} // namespace blastro
