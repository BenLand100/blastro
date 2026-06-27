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

#include "StatsPlotWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>
#include <cmath>
#include <algorithm>

namespace blastro {

StatsPlotWidget::StatsPlotWidget(QWidget* parent)
    : QWidget(parent),
      m_highlightIndex(-1),
      m_minVal(0.0),
      m_maxVal(0.0),
      m_rangeMin(0.0),
      m_rangeMax(1.0),
      m_dragMode(DragNone) {
    setMouseTracking(true);
    setMinimumHeight(200);
}

void StatsPlotWidget::setBatch(ImageBatchPtr batch, const std::string& metric) {
    m_batch = batch;
    m_metric = metric;
    updateRanges();
    update();
}

void StatsPlotWidget::setHighlightIndex(int index) {
    if (m_highlightIndex != index) {
        m_highlightIndex = index;
        update();
    }
}

void StatsPlotWidget::setFilterRange(double minVal, double maxVal) {
    m_minVal = minVal;
    m_maxVal = maxVal;
    update();
}

void StatsPlotWidget::updateRanges() {
    if (!m_batch || m_batch->count() == 0) {
        m_rangeMin = 0.0;
        m_rangeMax = 1.0;
        m_minVal = 0.0;
        m_maxVal = 1.0;
        return;
    }

    int count = m_batch->count();
    double minVal = getValForFrame(0);
    double maxVal = minVal;

    for (int i = 1; i < count; ++i) {
        double v = getValForFrame(i);
        if (v < minVal) minVal = v;
        if (v > maxVal) maxVal = v;
    }

    // Add a 10% margin so points aren't right on the edge
    double diff = maxVal - minVal;
    if (diff < 1e-6) {
        m_rangeMin = minVal - 1.0;
        m_rangeMax = maxVal + 1.0;
    } else {
        m_rangeMin = minVal - 0.1 * diff;
        m_rangeMax = maxVal + 0.1 * diff;
    }

    // Default filters to cover full range
    m_minVal = minVal;
    m_maxVal = maxVal;
}

double StatsPlotWidget::getValForFrame(int index) const {
    if (!m_batch || index < 0 || index >= m_batch->count()) {
        return 0.0;
    }
    FrameMetadata meta = m_batch->frameMetadata(index);
    if (m_metric == "starCount") return meta.starCount;
    if (m_metric == "fwhm") return meta.fwhm;
    if (m_metric == "snr") return meta.snr;
    if (m_metric == "dx") return meta.dx;
    if (m_metric == "dy") return meta.dy;
    if (m_metric == "theta") return meta.theta * 180.0 / M_PI; // Degrees
    return 0.0;
}

QPointF StatsPlotWidget::dataToPixel(double idx, double val) const {
    double leftMargin = 50.0;
    double rightMargin = 20.0;
    double topMargin = 20.0;
    double bottomMargin = 30.0;

    double plotW = width() - leftMargin - rightMargin;
    double plotH = height() - topMargin - bottomMargin;

    double x = leftMargin;
    if (m_batch && m_batch->count() > 1) {
        x += (idx / (m_batch->count() - 1)) * plotW;
    } else {
        x += plotW / 2.0;
    }

    double y = topMargin + plotH;
    double rangeDiff = m_rangeMax - m_rangeMin;
    if (rangeDiff > 1e-6) {
        y -= ((val - m_rangeMin) / rangeDiff) * plotH;
    } else {
        y -= plotH / 2.0;
    }

    return QPointF(x, y);
}

void StatsPlotWidget::pixelToData(const QPoint& pix, double& idx, double& val) const {
    double leftMargin = 50.0;
    double rightMargin = 20.0;
    double topMargin = 20.0;
    double bottomMargin = 30.0;

    double plotW = width() - leftMargin - rightMargin;
    double plotH = height() - topMargin - bottomMargin;

    double rx = pix.x() - leftMargin;
    if (m_batch && m_batch->count() > 1) {
        idx = (rx / plotW) * (m_batch->count() - 1);
        idx = std::max(0.0, std::min(static_cast<double>(m_batch->count() - 1), idx));
    } else {
        idx = 0.0;
    }

    double ry = topMargin + plotH - pix.y();
    double rangeDiff = m_rangeMax - m_rangeMin;
    val = m_rangeMin + (ry / plotH) * rangeDiff;
    val = std::max(m_rangeMin, std::min(m_rangeMax, val));
}

void StatsPlotWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int w = width();
    int h = height();

    double leftMargin = 50.0;
    double rightMargin = 20.0;
    double topMargin = 20.0;
    double bottomMargin = 30.0;

    double plotW = w - leftMargin - rightMargin;
    double plotH = h - topMargin - bottomMargin;

    // Draw background
    painter.fillRect(rect(), QColor("#1e1e1e"));

    // Draw border
    painter.setPen(QPen(QColor("#333333"), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(leftMargin, topMargin, plotW, plotH);

    // Draw background grid lines (horizontal)
    painter.setPen(QPen(QColor("#2c2c2c"), 1, Qt::DashLine));
    int gridCount = 4;
    for (int i = 1; i <= gridCount; ++i) {
        double val = m_rangeMin + (i * (m_rangeMax - m_rangeMin) / (gridCount + 1));
        QPointF p = dataToPixel(0, val);
        painter.drawLine(leftMargin, p.y(), leftMargin + plotW, p.y());
        
        // Draw tick label on left
        painter.setPen(QColor("#888888"));
        painter.setFont(QFont("monospace", 8));
        painter.drawText(QRectF(5, p.y() - 6, leftMargin - 10, 12), Qt::AlignRight | Qt::AlignVCenter, QString::number(val, 'g', 4));
        painter.setPen(QPen(QColor("#2c2c2c"), 1, Qt::DashLine));
    }

    if (!m_batch || m_batch->count() == 0) {
        return;
    }

    int count = m_batch->count();

    // Draw frame index labels on X-axis (draw up to 10 labels to prevent crowding)
    painter.setPen(QColor("#888888"));
    int labelStep = std::max(1, count / 8);
    for (int i = 0; i < count; i += labelStep) {
        QPointF p = dataToPixel(i, m_rangeMin);
        QRectF labelRect(p.x() - 20, p.y() + 5, 40, 15);
        painter.drawText(labelRect, Qt::AlignCenter, QString::number(i + 1));
    }

    // Draw horizontal filter threshold lines
    // Min threshold line (orange)
    QPointF pMin = dataToPixel(0, m_minVal);
    painter.setPen(QPen(QColor("#ffaa00"), 1.5, Qt::SolidLine));
    painter.drawLine(leftMargin, pMin.y(), leftMargin + plotW, pMin.y());
    painter.drawText(leftMargin + 5, pMin.y() - 13, "Min");

    // Max threshold line (orange)
    QPointF pMax = dataToPixel(0, m_maxVal);
    painter.setPen(QPen(QColor("#ffaa00"), 1.5, Qt::SolidLine));
    painter.drawLine(leftMargin, pMax.y(), leftMargin + plotW, pMax.y());
    painter.drawText(leftMargin + 5, pMax.y() + 2, "Max");

    // Draw line connecting the dots
    QPainterPath linePath;
    for (int i = 0; i < count; ++i) {
        double val = getValForFrame(i);
        QPointF p = dataToPixel(i, val);
        if (i == 0) {
            linePath.moveTo(p);
        } else {
            linePath.lineTo(p);
        }
    }
    painter.setPen(QPen(QColor(0, 122, 204, 80), 1.0)); // semi-transparent cyan line
    painter.drawPath(linePath);

    // Draw scatter points
    for (int i = 0; i < count; ++i) {
        double val = getValForFrame(i);
        QPointF p = dataToPixel(i, val);
        bool selected = m_batch->isFrameSelected(i);

        // Dot color
        QColor dotColor = selected ? QColor("#4caf50") : QColor("#f44336"); // Green vs Red
        painter.setBrush(dotColor);
        painter.setPen(QPen(dotColor.darker(150), 1.0));

        if (i == m_highlightIndex) {
            // Draw highlight border for the current frame
            painter.setBrush(dotColor);
            painter.setPen(QPen(Qt::white, 2.0));
            painter.drawEllipse(p, 6.0, 6.0);
        } else {
            painter.drawEllipse(p, 4.0, 4.0);
        }
    }
}

void StatsPlotWidget::mousePressEvent(QMouseEvent* event) {
    if (!m_batch || m_batch->count() == 0) return;

    QPoint pos = event->pos();
    QPointF pMin = dataToPixel(0, m_minVal);
    QPointF pMax = dataToPixel(0, m_maxVal);

    // Check if clicking near threshold lines (tolerance 6 pixels)
    if (std::abs(pos.y() - pMin.y()) < 6) {
        m_dragMode = DragMin;
        setCursor(Qt::SplitVCursor);
        event->accept();
        return;
    }
    if (std::abs(pos.y() - pMax.y()) < 6) {
        m_dragMode = DragMax;
        setCursor(Qt::SplitVCursor);
        event->accept();
        return;
    }

    // Check if clicking near any data point (tolerance 8 pixels)
    int count = m_batch->count();
    for (int i = 0; i < count; ++i) {
        double val = getValForFrame(i);
        QPointF p = dataToPixel(i, val);
        double dist = std::hypot(pos.x() - p.x(), pos.y() - p.y());
        if (dist < 8.0) {
            emit pointClicked(i);
            event->accept();
            return;
        }
    }
}

void StatsPlotWidget::mouseReleaseEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    m_dragMode = DragNone;
    setCursor(Qt::ArrowCursor);
}

void StatsPlotWidget::mouseMoveEvent(QMouseEvent* event) {
    QPoint pos = event->pos();
    QPointF pMin = dataToPixel(0, m_minVal);
    QPointF pMax = dataToPixel(0, m_maxVal);

    if (m_dragMode != DragNone) {
        double dummyIdx;
        double clickedVal;
        pixelToData(pos, dummyIdx, clickedVal);

        if (m_dragMode == DragMin) {
            m_minVal = clickedVal;
            // Bound so min <= max
            if (m_minVal > m_maxVal) m_minVal = m_maxVal;
        } else if (m_dragMode == DragMax) {
            m_maxVal = clickedVal;
            // Bound so max >= min
            if (m_maxVal < m_minVal) m_maxVal = m_minVal;
        }
        
        update();
        emit rangeChanged(m_minVal, m_maxVal);
        event->accept();
        return;
    }

    // Set cursor to vertical split if hovering over threshold lines
    if (std::abs(pos.y() - pMin.y()) < 6 || std::abs(pos.y() - pMax.y()) < 6) {
        setCursor(Qt::SplitVCursor);
    } else {
        // Set cursor to pointing hand if hovering over any data point
        if (m_batch) {
            int count = m_batch->count();
            for (int i = 0; i < count; ++i) {
                double val = getValForFrame(i);
                QPointF p = dataToPixel(i, val);
                if (std::hypot(pos.x() - p.x(), pos.y() - p.y()) < 8.0) {
                    setCursor(Qt::PointingHandCursor);
                    return;
                }
            }
        }
        setCursor(Qt::ArrowCursor);
    }
}

} // namespace blastro
