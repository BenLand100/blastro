/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "GhsWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <algorithm>
#include "core/MathUtils.h"
#include "core/Preferences.h"

namespace blastro {

GhsWidget::GhsWidget(QWidget* parent)
    : HistogramBaseWidget(parent) {
    m_spPoint.fill(0.5);
    m_stretchFactor.fill(0.0);
    m_shadowProtect.fill(0.0);
    m_highlightProtect.fill(1.0);
    m_dragTarget = {DragTarget::None, 0};
}

void GhsWidget::setGhsParams(const std::array<double, 6>& sp, const std::array<double, 6>& d) {
    m_spPoint = sp;
    m_stretchFactor = d;
    update();
}

void GhsWidget::setGhsProtections(const std::array<double, 6>& shadowProtect, const std::array<double, 6>& highlightProtect) {
    m_shadowProtect = shadowProtect;
    m_highlightProtect = highlightProtect;
    update();
}

GhsWidget::DragTarget GhsWidget::getCloseLine(const QPoint& pos) const {
    if (!m_active) return {DragTarget::None, 0};

    const double threshold = 10.0;
    double minDist = threshold;
    DragTarget target = {DragTarget::None, 0};
    double h = height();

    int activeIdx = m_activeChannel;

    for (int c = activeIdx; c <= activeIdx; ++c) {
        double xSP = valueToX(m_spPoint[c]);
        double xS = valueToX(m_shadowProtect[c]);
        double xH = valueToX(m_highlightProtect[c]);

        double dxSP = std::abs(pos.x() - xSP);
        double dxS = std::abs(pos.x() - xS);
        double dxH = std::abs(pos.x() - xH);

        double distSP = dxSP + (std::abs(pos.y() - h/2.0) < 12.0 ? 0.0 : 12.0); // middle
        double distS = dxS + (pos.y() > h - 12 ? 0.0 : 12.0); // bottom
        double distH = dxH + (pos.y() < 12 ? 0.0 : 12.0); // top

        if (distSP < minDist) { minDist = distSP; target = {DragTarget::SymmetryPoint, c}; }
        if (distS < minDist) { minDist = distS; target = {DragTarget::ShadowProtect, c}; }
        if (distH < minDist) { minDist = distH; target = {DragTarget::HighlightProtect, c}; }
    }

    return target;
}

static double localGhsFunction(double arg, double D, int form) {
    if (form > 0) {
        double denom = 1.0 + form * D * arg;
        if (denom <= 0.0) return 1.0;
        return 1.0 - std::pow(denom, -1.0 / form);
    } else if (form == 0) {
        return 1.0 - std::exp(-D * arg);
    } else if (form == -1) {
        double val = 1.0 + D * arg;
        if (val <= 0.0) return 0.0;
        return std::log(val);
    } else {
        double val = 1.0 - form * D * arg;
        if (val <= 0.0) return 1.0 / (D * (form + 1) + 1e-6);
        double expTerm = (form + 1.0) / form;
        return (1.0 - std::pow(val, expTerm)) / (D * (form + 1) + 1e-6);
    }
}

void GhsWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int w = width();
    int h = height();
    float lineWidth = Preferences::instance().getHistogramLineWidth();

    if (m_cacheDirty || m_cachedBackground.size() != size()) {
        m_cachedBackground = QPixmap(w, h);
        m_cachedBackground.fill(Qt::transparent);

        QPainter bgPainter(&m_cachedBackground);
        bgPainter.setRenderHint(QPainter::Antialiasing, true);

        // Call base class helper to render rounded background, grid and histograms
        drawBaseBackground(bgPainter, w, h, lineWidth);

        m_cacheDirty = false;
    }

    painter.drawPixmap(0, 0, m_cachedBackground);

    if (!m_active) {
        return;
    }

    auto isDefault = [this](int c) {
        return std::abs(m_spPoint[c] - 0.5) < 1e-4 && 
               std::abs(m_shadowProtect[c] - 0.0) < 1e-4 && 
               std::abs(m_highlightProtect[c] - 1.0) < 1e-4 && 
               m_stretchFactor[c] < 1e-5;
    };

    int activeIdx = m_activeChannel;

    // Draw background/inactive curves first
    for (int c = 0; c < 6; ++c) {
        if (c == activeIdx) continue;
        if (isDefault(c)) continue;

        QColor lineColor;
        if (c == 0) lineColor = QColor("#ffffff"); // K
        else if (c == 1) lineColor = QColor("#cc3333"); // R
        else if (c == 2) lineColor = QColor("#33aa33"); // G
        else if (c == 3) lineColor = QColor("#3366cc"); // B
        else if (c == 4) lineColor = QColor("#c4a000"); // L
        else if (c == 5) lineColor = QColor("#75507b"); // S

        lineColor.setAlpha(100); // Dimmed

        QPolygonF curvePolygon;
        double D = 0.0, a = 0.0, denom = 1.0, sp = m_spPoint[c];
        bool doGhsMath = false;
        if (m_stretchFactor[c] >= 1e-5) {
            D = std::exp(m_stretchFactor[c]) - 1.0;
            a = -localGhsFunction(sp, D, 1);
            double b = localGhsFunction(1.0 - sp, D, 1);
            denom = b - a;
            if (std::abs(denom) < 1e-8) denom = 1e-8;
            doGhsMath = true;
        }

        for (int px = 0; px <= w; px += 2) {
            if (px > w) px = w;
            double val = xToValue(px);
            double stretched = std::clamp(val, 0.0, 1.0);
            if (doGhsMath) {
                double x = val - sp;
                double result = (x >= 0.0) ? localGhsFunction(x, D, 1) : -localGhsFunction(-x, D, 1);
                stretched = std::clamp((result - a) / denom, 0.0, 1.0);
            }
            stretched = blastro::MathUtils::applyGhsProtection(val, sp, m_shadowProtect[c], m_highlightProtect[c], stretched);
            double py = h - 2.0 - stretched * (h - 4.0);
            curvePolygon << QPointF(px, py);
            if (px == w) break;
        }

        QPen curvePen(lineColor, 1.5);
        curvePen.setJoinStyle(Qt::RoundJoin);
        curvePen.setCapStyle(Qt::RoundCap);
        painter.setPen(curvePen);
        painter.setBrush(Qt::NoBrush);
        if (!curvePolygon.isEmpty()) {
            painter.drawPolyline(curvePolygon);
        }
    }

    // Now draw active channel (foreground)
    {
        int c = activeIdx;
        QColor lineColor;
        if (c == 0) lineColor = QColor("#ffffff"); // K
        else if (c == 1) lineColor = QColor("#cc3333"); // R
        else if (c == 2) lineColor = QColor("#33aa33"); // G
        else if (c == 3) lineColor = QColor("#3366cc"); // B
        else if (c == 4) lineColor = QColor("#c4a000"); // L
        else if (c == 5) lineColor = QColor("#75507b"); // S

        // Draw Symmetry Point line
        double xSP = valueToX(m_spPoint[c]);
        if (xSP >= 0 && xSP <= w) {
            painter.setPen(QPen(lineColor, lineWidth));
            painter.drawLine(xSP, 1, xSP, h - 1);
            painter.setBrush(lineColor);
            painter.drawEllipse(QPointF(xSP, h / 2.0), 3, 3); // middle
        }

        // Draw Shadow Protection line
        double xS = valueToX(m_shadowProtect[c]);
        if (xS >= 0 && xS <= w) {
            painter.setPen(QPen(lineColor, lineWidth, Qt::DashLine));
            painter.drawLine(xS, 1, xS, h - 1);
            painter.setBrush(lineColor);
            painter.drawEllipse(QPointF(xS, h - 4), 3, 3); // bottom
        }

        // Draw Highlight Protection line
        double xH = valueToX(m_highlightProtect[c]);
        if (xH >= 0 && xH <= w) {
            painter.setPen(QPen(lineColor, lineWidth, Qt::DotLine));
            painter.drawLine(xH, 1, xH, h - 1);
            painter.setBrush(lineColor);
            painter.drawEllipse(QPointF(xH, 4), 3, 3); // top
        }

        // Draw transfer function curve
        QPolygonF curvePolygon;
        double D = 0.0, a = 0.0, denom = 1.0, sp = m_spPoint[c];
        bool doGhsMath = false;
        if (m_stretchFactor[c] >= 1e-5) {
            D = std::exp(m_stretchFactor[c]) - 1.0;
            a = -localGhsFunction(sp, D, 1);
            double b = localGhsFunction(1.0 - sp, D, 1);
            denom = b - a;
            if (std::abs(denom) < 1e-8) denom = 1e-8;
            doGhsMath = true;
        }

        for (int px = 0; px <= w; px += 2) {
            if (px > w) px = w;
            double val = xToValue(px);
            double stretched = std::clamp(val, 0.0, 1.0);
            if (doGhsMath) {
                double x = val - sp;
                double result = (x >= 0.0) ? localGhsFunction(x, D, 1) : -localGhsFunction(-x, D, 1);
                stretched = std::clamp((result - a) / denom, 0.0, 1.0);
            }
            stretched = blastro::MathUtils::applyGhsProtection(val, sp, m_shadowProtect[c], m_highlightProtect[c], stretched);
            double py = h - 2.0 - stretched * (h - 4.0);
            curvePolygon << QPointF(px, py);
            if (px == w) break;
        }

        QPen curvePen(lineColor, 2.0);
        curvePen.setJoinStyle(Qt::RoundJoin);
        curvePen.setCapStyle(Qt::RoundCap);
        painter.setPen(curvePen);
        painter.setBrush(Qt::NoBrush);
        if (!curvePolygon.isEmpty()) {
            painter.drawPolyline(curvePolygon);
        }
    }
}

void GhsWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        HistogramBaseWidget::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        if (!m_active) {
            QWidget::mousePressEvent(event);
            return;
        }
        m_dragTarget = getCloseLine(event->pos());
        if (m_dragTarget.type != DragTarget::None) {
            event->accept();
            return;
        }
    }
    QWidget::mousePressEvent(event);
}

void GhsWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton || m_isScrolling) {
        HistogramBaseWidget::mouseReleaseEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_dragTarget = {DragTarget::None, 0};
    }
    setCursor(Qt::ArrowCursor);
    QWidget::mouseReleaseEvent(event);
}

void GhsWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_isScrolling) {
        HistogramBaseWidget::mouseMoveEvent(event);
        return;
    }

    if (!m_active) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    if (m_dragTarget.type == DragTarget::None) {
        DragTarget hoverTarget = getCloseLine(event->pos());
        if (hoverTarget.type != DragTarget::None) {
            setCursor(Qt::SplitHCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
        QWidget::mouseMoveEvent(event);
        return;
    }

    double val = std::clamp(xToValue(event->pos().x()), 0.0, 1.0);
    int c = m_dragTarget.channel;

    if (m_dragTarget.type == DragTarget::SymmetryPoint) {
        m_spPoint[c] = val;
        if (m_shadowProtect[c] > m_spPoint[c]) m_shadowProtect[c] = m_spPoint[c];
        if (m_highlightProtect[c] < m_spPoint[c]) m_highlightProtect[c] = m_spPoint[c];
        if (isWidgetChannelsLinked() && c == 0) {
            m_spPoint.fill(m_spPoint[c]);
            m_shadowProtect.fill(m_shadowProtect[c]);
            m_highlightProtect.fill(m_highlightProtect[c]);
        }
        emit ghsParamsChanged(m_spPoint, m_stretchFactor);
        emit ghsProtectionsChanged(m_shadowProtect, m_highlightProtect);
    } else if (m_dragTarget.type == DragTarget::ShadowProtect) {
        m_shadowProtect[c] = val;
        if (m_shadowProtect[c] > m_spPoint[c]) {
            m_spPoint[c] = m_shadowProtect[c];
            if (m_highlightProtect[c] < m_spPoint[c]) m_highlightProtect[c] = m_spPoint[c];
        }
        if (isWidgetChannelsLinked() && c == 0) {
            m_spPoint.fill(m_spPoint[c]);
            m_shadowProtect.fill(m_shadowProtect[c]);
            m_highlightProtect.fill(m_highlightProtect[c]);
        }
        emit ghsProtectionsChanged(m_shadowProtect, m_highlightProtect);
        emit ghsParamsChanged(m_spPoint, m_stretchFactor);
    } else if (m_dragTarget.type == DragTarget::HighlightProtect) {
        m_highlightProtect[c] = val;
        if (m_highlightProtect[c] < m_spPoint[c]) {
            m_spPoint[c] = m_highlightProtect[c];
            if (m_shadowProtect[c] > m_spPoint[c]) m_shadowProtect[c] = m_spPoint[c];
        }
        if (isWidgetChannelsLinked() && c == 0) {
            m_spPoint.fill(m_spPoint[c]);
            m_shadowProtect.fill(m_shadowProtect[c]);
            m_highlightProtect.fill(m_highlightProtect[c]);
        }
        emit ghsProtectionsChanged(m_shadowProtect, m_highlightProtect);
        emit ghsParamsChanged(m_spPoint, m_stretchFactor);
    }

    update();
    event->accept();
}

} // namespace blastro
