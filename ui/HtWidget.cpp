/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "HtWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <algorithm>
#include "core/Preferences.h"

namespace blastro {

HtWidget::HtWidget(QWidget* parent)
    : HistogramBaseWidget(parent) {
    m_blackpoint.fill(0.0);
    m_whitepoint.fill(1.0);
    m_midpoint.fill(0.5);
    m_dragTarget = {DragTarget::None, 0};
}

void HtWidget::setStretchParams(const std::array<double, 6>& b, const std::array<double, 6>& w, const std::array<double, 6>& m) {
    m_blackpoint = b;
    m_whitepoint = w;
    m_midpoint = m;
    update();
}

bool HtWidget::isChannelActive(int c) const {
    if (m_isImageWindow) {
        if (isWidgetChannelsLinked()) {
            return c == 0;
        } else {
            if (m_activeChannel == 0) {
                return c == 1 || c == 2 || c == 3;
            } else {
                return c == m_activeChannel;
            }
        }
    } else {
        return c == m_activeChannel;
    }
}

HtWidget::DragTarget HtWidget::getCloseLine(const QPoint& pos) const {
    if (!m_active) return {DragTarget::None, 0};

    const double threshold = 10.0;
    double minDist = threshold;
    DragTarget target = {DragTarget::None, 0};
    double h = height();

    for (int c = 0; c < 6; ++c) {
        if (!isChannelActive(c)) continue;

        double xB = valueToX(m_blackpoint[c]);
        double xW = valueToX(m_whitepoint[c]);
        double xM = valueToX(m_blackpoint[c] + m_midpoint[c] * (m_whitepoint[c] - m_blackpoint[c]));

        double dxB = std::abs(pos.x() - xB);
        double dxW = std::abs(pos.x() - xW);
        double dxM = std::abs(pos.x() - xM);

        double distB = dxB + (pos.y() > h - 12 ? 0.0 : 12.0); // bottom
        double distW = dxW + (pos.y() < 12 ? 0.0 : 12.0); // top
        double distM = dxM + (std::abs(pos.y() - h/2.0) < 12.0 ? 0.0 : 12.0); // middle

        if (distB < minDist) { minDist = distB; target = {DragTarget::Black, c}; }
        if (distW < minDist) { minDist = distW; target = {DragTarget::White, c}; }
        if (distM < minDist) { minDist = distM; target = {DragTarget::Mid, c}; }
    }

    return target;
}

static double evaluateLocalHt(double val, double blackpoint, double whitepoint, double midpoint) {
    double x = (val - blackpoint) / (whitepoint - blackpoint + 1e-12);
    x = std::clamp(x, 0.0, 1.0);
    if (x <= 0.0) return 0.0;
    if (x >= 1.0) return 1.0;
    return (midpoint - 1.0) * x / ((2.0 * midpoint - 1.0) * x - midpoint);
}

void HtWidget::paintEvent(QPaintEvent* event) {
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

    auto isDefault = [](double b, double w, double m) {
        return std::abs(b - 0.0) < 1e-4 && std::abs(w - 1.0) < 1e-4 && std::abs(m - 0.5) < 1e-4;
    };

    int activeIdx = m_activeChannel;

    // Draw background/inactive curves first
    for (int c = 0; c < 6; ++c) {
        if (isChannelActive(c)) continue;
        if (isDefault(m_blackpoint[c], m_whitepoint[c], m_midpoint[c])) continue;

        QColor lineColor;
        if (c == 0) lineColor = QColor("#ffffff"); // K
        else if (c == 1) lineColor = QColor("#cc3333"); // R
        else if (c == 2) lineColor = QColor("#33aa33"); // G
        else if (c == 3) lineColor = QColor("#3366cc"); // B
        else if (c == 4) lineColor = QColor("#c4a000"); // L
        else if (c == 5) lineColor = QColor("#75507b"); // S

        lineColor.setAlpha(100); // Dimmed

        if (m_drawCurve) {
            QPolygonF curvePolygon;
            for (int px = 0; px <= w; px += 2) {
                if (px > w) px = w;
                double val = xToValue(px);
                double yVal = evaluateLocalHt(val, m_blackpoint[c], m_whitepoint[c], m_midpoint[c]);
                double py = h - 2.0 - yVal * (h - 4.0);
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
    }

    // Now draw active channels (foreground)
    for (int c = 0; c < 6; ++c) {
        if (!isChannelActive(c)) continue;

        QColor lineColor;
        if (c == 0) lineColor = QColor("#ffffff"); // K
        else if (c == 1) lineColor = QColor("#cc3333"); // R
        else if (c == 2) lineColor = QColor("#33aa33"); // G
        else if (c == 3) lineColor = QColor("#3366cc"); // B
        else if (c == 4) lineColor = QColor("#c4a000"); // L
        else if (c == 5) lineColor = QColor("#75507b"); // S

        double xB = valueToX(m_blackpoint[c]);
        double xW = valueToX(m_whitepoint[c]);
        double xM = valueToX(m_blackpoint[c] + m_midpoint[c] * (m_whitepoint[c] - m_blackpoint[c]));

        if (c == 0 || c == 4 || c == 5 || isWidgetChannelsLinked()) {
            if (xB > 0) {
                painter.fillRect(QRectF(0, 0, std::min(static_cast<double>(w), xB), h), QColor(0, 0, 0, 120));
            }
            if (xW < w) {
                double fillStart = std::max(0.0, xW);
                painter.fillRect(QRectF(fillStart, 0, w - fillStart, h), QColor(0, 0, 0, 120));
            }
        }

        // Blackpoint line
        if (xB >= 0 && xB <= w) {
             painter.setPen(QPen(lineColor, lineWidth));
             painter.drawLine(xB, 1, xB, h - 1);
             painter.setBrush(lineColor);
             painter.drawEllipse(QPointF(xB, h - 4), 3, 3); // bottom
        }

        // Whitepoint line
        if (xW >= 0 && xW <= w) {
             painter.setPen(QPen(lineColor, lineWidth));
             painter.drawLine(xW, 1, xW, h - 1);
             painter.setBrush(lineColor);
             painter.drawEllipse(QPointF(xW, 4), 3, 3); // top
        }

        // Midpoint line
        if (xM >= 0 && xM <= w) {
             painter.setPen(QPen(lineColor, lineWidth));
             painter.drawLine(xM, 1, xM, h - 1);
             painter.setBrush(lineColor);
             painter.drawEllipse(QPointF(xM, h / 2.0), 3, 3); // middle
        }

        if (m_drawCurve) {
            QPolygonF curvePolygon;
            for (int px = 0; px <= w; px += 2) {
                if (px > w) px = w;
                double val = xToValue(px);
                double yVal = evaluateLocalHt(val, m_blackpoint[c], m_whitepoint[c], m_midpoint[c]);
                double py = h - 2.0 - yVal * (h - 4.0);
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

    // Finally draw status text for the base active channel
    {
        int c = m_activeChannel;
        painter.setPen(QColor(255, 255, 255, 140));
        painter.setFont(QFont("monospace", 7));
        QString txt = QString("HT   B:%1 M:%2 W:%3")
                      .arg(m_blackpoint[c], 0, 'f', 4)
                      .arg(m_blackpoint[c] + m_midpoint[c] * (m_whitepoint[c] - m_blackpoint[c]), 0, 'f', 4)
                      .arg(m_whitepoint[c], 0, 'f', 4);
        painter.drawText(8, h - 6, txt);
    }
}

void HtWidget::mousePressEvent(QMouseEvent* event) {
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

void HtWidget::mouseReleaseEvent(QMouseEvent* event) {
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

void HtWidget::mouseMoveEvent(QMouseEvent* event) {
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

    if (m_dragTarget.type == DragTarget::Black) {
        m_blackpoint[c] = std::min(val, m_whitepoint[c] - 0.001);
    } else if (m_dragTarget.type == DragTarget::White) {
        m_whitepoint[c] = std::max(val, m_blackpoint[c] + 0.001);
    } else if (m_dragTarget.type == DragTarget::Mid) {
        if (m_whitepoint[c] > m_blackpoint[c]) {
            double relativeVal = (val - m_blackpoint[c]) / (m_whitepoint[c] - m_blackpoint[c]);
            m_midpoint[c] = std::max(0.001, std::min(0.999, relativeVal));
        }
    }

    emit stretchParamsChanged(m_blackpoint, m_whitepoint, m_midpoint);
    update();
    event->accept();
}

void HtWidget::snapToBlackToMid() {
    int activeIdx = m_activeChannel;
    double bp = m_blackpoint[activeIdx];
    double wp = m_whitepoint[activeIdx];
    double rel_mp = m_midpoint[activeIdx];
    HistogramBaseWidget::snapToBlackToMid(bp, rel_mp, wp);
}

} // namespace blastro
