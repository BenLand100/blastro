/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "HistogramBaseWidget.h"
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <cmath>
#include <algorithm>
#include "core/Preferences.h"

namespace blastro {

HistogramBaseWidget::HistogramBaseWidget(QWidget* parent)
    : QWidget(parent),
      m_zoom(1.0),
      m_scrollOffset(0.0),
      m_active(false),
      m_isScrolling(false) {
    setMouseTracking(true);
    setStyleSheet("background-color: transparent; border: none;");
}

void HistogramBaseWidget::setHistograms(const std::vector<std::vector<int>>& histos) {
    m_histograms = histos;
    m_cacheDirty = true;
    update();
}

void HistogramBaseWidget::setChannelsLinked(bool linked) {
    m_channelsLinked = linked;
    update();
}

void HistogramBaseWidget::setActive(bool active) {
    m_active = active;
    update();
}

void HistogramBaseWidget::setActiveChannel(int channel) {
    m_activeChannel = static_cast<Channel>(channel);
    m_cacheDirty = true;
    update();
}

void HistogramBaseWidget::setSingleChannelColor(const QColor& color) {
    if (m_singleChannelColor != color) {
        m_singleChannelColor = color;
        m_cacheDirty = true;
        update();
    }
}

void HistogramBaseWidget::setSingleTraceColor(const QColor& color) {
    if (m_singleTraceColor != color) {
        m_singleTraceColor = color;
        m_cacheDirty = true;
        update();
    }
}

double HistogramBaseWidget::valueToX(double val) const {
    return (val - m_scrollOffset) * m_zoom * width();
}

double HistogramBaseWidget::xToValue(double x) const {
    double w = width();
    if (w <= 0.0) return 0.0;
    return m_scrollOffset + x / (m_zoom * w);
}

void HistogramBaseWidget::resetZoom() {
    m_zoom = 1.0;
    m_scrollOffset = 0.0;
    m_cacheDirty = true;
    update();
}

void HistogramBaseWidget::snapToBlackToMid(double blackpoint, double midpoint, double whitepoint) {
    double actualMid = blackpoint + midpoint * (whitepoint - blackpoint);
    double diff = actualMid - blackpoint;
    if (diff > 1e-6) {
        m_zoom = 0.45 / diff;
        m_zoom = std::max(1.0, std::min(250.0, m_zoom));
        m_scrollOffset = blackpoint - 0.05 / m_zoom;
        m_scrollOffset = std::max(0.0, std::min(1.0 - 1.0 / m_zoom, m_scrollOffset));
    } else {
        m_zoom = 1.0;
        m_scrollOffset = 0.0;
    }
    m_cacheDirty = true;
    update();
}

void HistogramBaseWidget::drawBaseBackground(QPainter& bgPainter, int w, int h, float lineWidth) {
    // 1. Rounded background and border
    bgPainter.setPen(QPen(QColor("#333333"), 1));
    bgPainter.setBrush(QBrush(QColor("#1a1a1a")));
    bgPainter.drawRoundedRect(QRectF(0.5, 0.5, w - 1.0, h - 1.0), 4.0, 4.0);

    // 2. Background vertical grid lines (Z-scaled / zoomed)
    bgPainter.setPen(QPen(QColor("#262626"), 1, Qt::DashLine));
    for (int i = 1; i < 10; ++i) {
        double val = static_cast<double>(i) / 10.0;
        double x = valueToX(val);
        if (x >= 0 && x <= w) {
            bgPainter.drawLine(x, 1, x, h - 1);
        }
    }

    // 3. Draw Histogram Curves
    if (!m_histograms.empty()) {
        for (size_t c = 0; c < m_histograms.size(); ++c) {
            const auto& hist = m_histograms[c];
            if (hist.empty()) continue;

            int numBins = hist.size();
            int nonZeroCount = 0;
            for (int val : hist) {
                if (val > 0) nonZeroCount++;
            }

            std::vector<double> renderHist(numBins, 0.0);
            for (int i = 0; i < numBins; ++i) {
                renderHist[i] = hist[i];
            }

            double maxVal = *std::max_element(renderHist.begin(), renderHist.end());
            if (maxVal > 0.0) {
                double logMax = std::max(1e-6, std::log(maxVal + 0.8) - std::log(0.8));

                std::vector<double> yValues(w, h);
                std::vector<bool> validX(w, false);

                for (int x = 0; x < w; ++x) {
                    double valCenter = xToValue(x);
                    if (valCenter >= 0.0 && valCenter <= 1.0) {
                        double valLeft = xToValue(x - 0.5);
                        double valRight = xToValue(x + 0.5);
                        int binStart = std::max(0, std::min(numBins - 1, static_cast<int>(valLeft * numBins)));
                        int binEnd = std::max(0, std::min(numBins - 1, static_cast<int>(valRight * numBins)));
                        if (binStart > binEnd) std::swap(binStart, binEnd);

                        double maxInBin = 0.0;
                        if (binStart == binEnd) {
                            maxInBin = renderHist[binStart];
                        } else {
                            for (int b = binStart; b <= binEnd; ++b) {
                                if (renderHist[b] > maxInBin) maxInBin = renderHist[b];
                            }
                        }

                        double logVal = std::max(0.0, std::log(maxInBin + 0.8) - std::log(0.8));
                        yValues[x] = (h - 1.0) - (logVal / logMax) * (h - 7.0);
                        validX[x] = true;
                    }
                }

                QColor outline;
                if (m_histograms.size() == 1 || c == 0) {
                    outline = m_singleTraceColor;
                    outline.setAlpha(m_histograms.size() == 1 ? 200 : 80);
                } else if (c == 1) { // R
                    outline = QColor("#cc3333");
                    outline.setAlpha(200);
                } else if (c == 2) { // G
                    outline = QColor("#33aa33");
                    outline.setAlpha(200);
                } else if (c == 3) { // B
                    outline = QColor("#3366cc");
                    outline.setAlpha(200);
                }

                QPen pen(outline, lineWidth);
                pen.setJoinStyle(Qt::RoundJoin);
                pen.setCapStyle(Qt::RoundCap);
                bgPainter.setPen(pen);
                QPolygonF polygon;
                for (int x = 0; x < w; ++x) {
                    if (validX[x]) {
                        polygon << QPointF(x, yValues[x]);
                    }
                }
                if (!polygon.isEmpty()) {
                    bgPainter.drawPolyline(polygon);
                }
            }
        }
    }
}

void HistogramBaseWidget::wheelEvent(QWheelEvent* event) {
    double oldZoom = m_zoom;
    double scrollDelta = event->angleDelta().y();

    if (scrollDelta > 0) {
        m_zoom *= 1.25;
    } else {
        m_zoom /= 1.25;
    }
    m_zoom = std::max(1.0, std::min(250.0, m_zoom));

    double mouseX = event->position().x();
    double mouseVal = m_scrollOffset + mouseX / (oldZoom * width());

    m_scrollOffset = mouseVal - mouseX / (m_zoom * width());
    m_scrollOffset = std::max(0.0, std::min(1.0 - 1.0 / m_zoom, m_scrollOffset));

    m_cacheDirty = true;
    update();
    event->accept();
}

void HistogramBaseWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    resetZoom();
    event->accept();
}

void HistogramBaseWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        m_isScrolling = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::SizeAllCursor);
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void HistogramBaseWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        m_isScrolling = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void HistogramBaseWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_isScrolling) {
        double dx = event->pos().x() - m_lastMousePos.x();
        double dv = dx / (m_zoom * width());
        m_scrollOffset -= dv;
        m_scrollOffset = std::max(0.0, std::min(1.0 - 1.0 / m_zoom, m_scrollOffset));
        m_lastMousePos = event->pos();
        m_cacheDirty = true;
        update();
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

} // namespace blastro
