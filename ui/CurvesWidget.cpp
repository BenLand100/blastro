/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "CurvesWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <cmath>
#include <algorithm>
#include "core/MathUtils.h"
#include "core/Preferences.h"

namespace blastro {

CurvesWidget::CurvesWidget(QWidget* parent)
    : HistogramBaseWidget(parent) {
    // Default identity curve: input → output, LUT maps i/65535 → i/65535
    constexpr int lutSize = 65536;
    for (int i = 0; i < 6; ++i) {
        m_points[i] = { QPointF(0.0, 0.0), QPointF(1.0, 1.0) };
        m_luts[i].resize(lutSize);
        for (int j = 0; j < lutSize; ++j)
            m_luts[i][j] = static_cast<float>(j) / (lutSize - 1);
    }
}

void CurvesWidget::setCurvePoints(int channel, const std::vector<QPointF>& points) {
    if (channel >= 0 && channel < 6) {
        m_points[channel] = points;
        rebuildLut(channel);
        update();
    }
}

std::vector<QPointF> CurvesWidget::getCurvePoints(int channel) const {
    if (channel >= 0 && channel < 6) {
        return m_points[channel];
    }
    return {};
}

const std::vector<float>& CurvesWidget::getLut(int channel) const {
    static const std::vector<float> identity;
    if (channel >= 0 && channel < 6) return m_luts[channel];
    return identity;
}

void CurvesWidget::rebuildLut(int channel) {
    if (channel < 0 || channel >= 6) return;
    const auto& pts = m_points[channel];
    std::vector<double> px, py;
    px.reserve(pts.size());
    py.reserve(pts.size());
    for (const auto& pt : pts) { px.push_back(pt.x()); py.push_back(pt.y()); }
    m_luts[channel] = MathUtils::computeCurvesLUT(px, py);
}

double CurvesWidget::valueToY(double val) const {
    return (1.0 - val) * height();
}

double CurvesWidget::yToValue(double y) const {
    double h = height();
    if (h <= 0.0) return 0.0;
    return 1.0 - y / h;
}

int CurvesWidget::getClosestPointIndex(const QPointF& pos, double& minDist) const {
    if (!m_active || m_activeChannel < 0 || m_activeChannel >= 6) return -1;
    const auto& points = m_points[m_activeChannel];
    int bestIdx = -1;
    minDist = 1e9;
    for (size_t i = 0; i < points.size(); ++i) {
        double px = valueToX(points[i].x());
        double py = valueToY(points[i].y());
        double dx = pos.x() - px;
        double dy = pos.y() - py;
        double dist = std::sqrt(dx * dx + dy * dy);
        if (dist < minDist) {
            minDist = dist;
            bestIdx = i;
        }
    }
    return bestIdx;
}

void CurvesWidget::paintEvent(QPaintEvent* event) {
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

        // Render base background, vertical grid and histograms
        drawBaseBackground(bgPainter, w, h, lineWidth);

        // Draw horizontal grid lines (which do not zoom/pan)
        bgPainter.setPen(QPen(QColor("#262626"), 1, Qt::DashLine));
        for (int i = 1; i < 10; ++i) {
            double val = static_cast<double>(i) / 10.0;
            double y = valueToY(val);
            if (y >= 0 && y <= h) bgPainter.drawLine(1, y, w - 1, y);
        }

        // Draw zoomed/panned identity line
        bgPainter.setPen(QPen(QColor("#333333"), 1, Qt::SolidLine));
        bgPainter.drawLine(valueToX(0.0), valueToY(0.0), valueToX(1.0), valueToY(1.0));
        
        m_cacheDirty = false;
    }

    painter.drawPixmap(0, 0, m_cachedBackground);

    if (!m_active) {
        return;
    }

    auto isDefault = [](const std::vector<QPointF>& pts) {
        return pts.size() == 2 && pts[0] == QPointF(0.0, 0.0) && pts[1] == QPointF(1.0, 1.0);
    };

    for (int c = 0; c < 6; ++c) {
        const auto& points = m_points[c];
        bool channelIsDefault = isDefault(points);

        if (c == m_activeChannel || !channelIsDefault) {
            int act = c;

            QColor lineColor;
            if (act == 0) lineColor = QColor("#ffffff"); // K
            else if (act == 1) lineColor = QColor("#cc3333"); // R
            else if (act == 2) lineColor = QColor("#33aa33"); // G
            else if (act == 3) lineColor = QColor("#3366cc"); // B
            else if (act == 4) lineColor = QColor("#c4a000"); // L
            else if (act == 5) lineColor = QColor("#75507b"); // S
            else lineColor = m_singleChannelColor;

            if (points.size() < 2) continue;

            const auto& lut = m_luts[act];

            QPolygonF curvePolygon;
            for (int x = 0; x <= w; x += 2) {
                if (x > w) x = w;
                double valX = xToValue(x);
                
                float idxF = valX * (lut.size() - 1);
                int idx = static_cast<int>(idxF);
                float yVal = 0;
                if (idx < 0) yVal = lut.front();
                else if (idx >= lut.size() - 1) yVal = lut.back();
                else {
                    float t = idxF - idx;
                    yVal = (1.0f - t) * lut[idx] + t * lut[idx + 1];
                }

                double y = valueToY(yVal);
                curvePolygon << QPointF(x, y);
                if (x == w) break;
            }

            QPen curvePen(lineColor, 2.0);
            if (act != m_activeChannel) {
                curvePen.setColor(QColor(lineColor.red(), lineColor.green(), lineColor.blue(), 100)); // Dimmed
            }
            curvePen.setJoinStyle(Qt::RoundJoin);
            curvePen.setCapStyle(Qt::RoundCap);
            painter.setPen(curvePen);
            painter.setBrush(Qt::NoBrush);
            if (!curvePolygon.isEmpty()) {
                painter.drawPolyline(curvePolygon);
            }

            // Draw points for active channel only
            if (act == m_activeChannel) {
                painter.setPen(QPen(QColor(0, 0, 0), 1));
                painter.setBrush(lineColor);
                for (const auto& pt : points) {
                    painter.drawRect(QRectF(valueToX(pt.x()) - 3, valueToY(pt.y()) - 3, 6, 6));
                }
            }
        }
    }
}

void CurvesWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    m_cacheDirty = true;
}

void CurvesWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        HistogramBaseWidget::mousePressEvent(event);
        return;
    }

    if (!m_active || m_activeChannel < 0 || m_activeChannel >= 6) return;

    double dist;
    int closestIdx = getClosestPointIndex(event->position(), dist);

    if (event->button() == Qt::LeftButton) {
        if (closestIdx != -1 && dist < 15.0) {
            m_dragPointIndex = closestIdx;
            m_isDragging = true;
        } else {
            // Add point
            double x = std::clamp(xToValue(event->pos().x()), 0.0, 1.0);
            double y = std::clamp(yToValue(event->pos().y()), 0.0, 1.0);
            auto& points = m_points[m_activeChannel];
            points.push_back(QPointF(x, y));
            std::sort(points.begin(), points.end(), [](const QPointF& a, const QPointF& b) {
                return a.x() < b.x();
            });
            for (size_t i = 0; i < points.size(); ++i) {
                if (points[i].x() == x && points[i].y() == y) {
                    m_dragPointIndex = i;
                    break;
                }
            }
            m_isDragging = true;
            rebuildLut(m_activeChannel);
            emit curveChanged(m_activeChannel, points);
            update();
        }
    } else if (event->button() == Qt::RightButton) {
        if (closestIdx != -1 && dist < 15.0) {
            auto& points = m_points[m_activeChannel];
            if (closestIdx > 0 && closestIdx < points.size() - 1) { // Can't delete ends
                points.erase(points.begin() + closestIdx);
                rebuildLut(m_activeChannel);
                emit curveChanged(m_activeChannel, points);
                update();
            }
        }
    }
}

void CurvesWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton || m_isScrolling) {
        HistogramBaseWidget::mouseReleaseEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        m_dragPointIndex = -1;
    }
}

void CurvesWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_isScrolling) {
        HistogramBaseWidget::mouseMoveEvent(event);
        return;
    }

    if (!m_active || m_activeChannel < 0 || m_activeChannel >= 6) return;

    if (m_isDragging && m_dragPointIndex != -1) {
        auto& points = m_points[m_activeChannel];
        double x = std::clamp(xToValue(event->pos().x()), 0.0, 1.0);
        double y = std::clamp(yToValue(event->pos().y()), 0.0, 1.0);

        if (m_dragPointIndex == 0) x = 0.0;
        else if (m_dragPointIndex == points.size() - 1) x = 1.0;
        else {
            double prevX = points[m_dragPointIndex - 1].x() + 0.001;
            double nextX = points[m_dragPointIndex + 1].x() - 0.001;
            x = std::clamp(x, prevX, nextX);
        }

        points[m_dragPointIndex] = QPointF(x, y);

        rebuildLut(m_activeChannel);

        emit curveChanged(m_activeChannel, points);
        update();
    }
}

} // namespace blastro
