/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
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
    : QWidget(parent) {
    setMouseTracking(true);
    setStyleSheet("background-color: transparent; border: none;");
    // Default identity curve: input → output, LUT maps i/65535 → i/65535
    constexpr int lutSize = 65536;
    for (int i = 0; i < 6; ++i) {
        m_points[i] = { QPointF(0.0, 0.0), QPointF(1.0, 1.0) };
        m_luts[i].resize(lutSize);
        for (int j = 0; j < lutSize; ++j)
            m_luts[i][j] = static_cast<float>(j) / (lutSize - 1);
    }
}

void CurvesWidget::setHistograms(const std::vector<std::vector<int>>& hists) {
    m_histograms = hists;
    m_cacheDirty = true;
    update();
}

void CurvesWidget::setChannelsLinked(bool linked) {
    m_channelsLinked = linked;
    update();
}

void CurvesWidget::setActive(bool active) {
    m_active = active;
    update();
}

void CurvesWidget::setActiveChannel(int channel) {
    m_activeChannel = channel;
    m_cacheDirty = true;
    update();
}

void CurvesWidget::setSingleChannelColor(const QColor& color) {
    if (m_singleChannelColor != color) {
        m_singleChannelColor = color;
        m_cacheDirty = true;
        update();
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

double CurvesWidget::valueToX(double val) const {
    return val * width();
}

double CurvesWidget::xToValue(double x) const {
    double w = width();
    if (w <= 0.0) return 0.0;
    return x / w;
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

        bgPainter.setPen(QPen(QColor("#333333"), 1));
        bgPainter.setBrush(QBrush(QColor("#1a1a1a")));
        bgPainter.drawRoundedRect(QRectF(0.5, 0.5, w - 1.0, h - 1.0), 4.0, 4.0);

        bgPainter.setPen(QPen(QColor("#262626"), 1, Qt::DashLine));
        for (int i = 1; i < 10; ++i) {
            double val = static_cast<double>(i) / 10.0;
            double x = valueToX(val);
            double y = valueToY(val);
            if (x >= 0 && x <= w) bgPainter.drawLine(x, 1, x, h - 1);
            if (y >= 0 && y <= h) bgPainter.drawLine(1, y, w - 1, y);
        }

        // Draw identity line
        bgPainter.setPen(QPen(QColor("#333333"), 1, Qt::SolidLine));
        bgPainter.drawLine(0, h, w, 0);

        if (!m_histograms.empty()) {
            for (size_t c = 0; c < m_histograms.size(); ++c) {
                const auto& hist = m_histograms[c];
                if (hist.empty()) continue;
                
                int numBins = hist.size();
                std::vector<double> renderHist(numBins, 0.0);
                for (int i = 0; i < numBins; ++i) {
                    renderHist[i] = hist[i];
                }

                double maxVal = *std::max_element(renderHist.begin(), renderHist.end());
                if (maxVal > 0.0) {
                    double logMax = std::max(1e-6, std::log(maxVal + 0.8) - std::log(0.8));
                    
                    std::vector<double> yValues(w, h);
                    std::vector<bool> validX(w, false);
                    int minBinWindow = std::max(5, static_cast<int>(numBins / (w * 2.0))); 

                    for (int x = 0; x < w; ++x) {
                        double valCenter = xToValue(x);
                        if (valCenter >= 0.0 && valCenter <= 1.0) {
                            double valLeft = xToValue(x - 0.5);
                            double valRight = xToValue(x + 0.5);
                            int binStart = std::max(0, std::min(numBins - 1, static_cast<int>(valLeft * numBins)));
                            int binEnd = std::max(0, std::min(numBins - 1, static_cast<int>(valRight * numBins)));
                            if (binStart > binEnd) std::swap(binStart, binEnd);
                            
                            int centerBin = (binStart + binEnd) / 2;
                            if (binEnd - binStart < minBinWindow) {
                                binStart = std::max(0, centerBin - minBinWindow / 2);
                                binEnd = std::min(numBins - 1, centerBin + minBinWindow / 2);
                            }
                            
                            double maxInBin = 0.0;
                            for (int b = binStart; b <= binEnd; ++b) {
                                if (renderHist[b] > maxInBin) maxInBin = renderHist[b];
                            }
                            
                            double logVal = std::max(0.0, std::log(maxInBin + 0.8) - std::log(0.8));
                            yValues[x] = (h - 1.0) - (logVal / logMax) * (h - 7.0);
                            validX[x] = true;
                        }
                    }

                    QColor outline;
                    if (m_histograms.size() == 1 || c == 0) {
                        outline = m_singleChannelColor; outline.setAlpha(m_histograms.size() == 1 ? 200 : 80);
                    } else if (c == 1) { // R
                        QColor base("#cc3333");
                        outline = base; outline.setAlpha(200);
                    } else if (c == 2) { // G
                        QColor base("#33aa33");
                        outline = base; outline.setAlpha(200);
                    } else if (c == 3) { // B
                        QColor base("#3366cc");
                        outline = base; outline.setAlpha(200);
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
        if (m_channelsLinked && c != m_activeChannel) continue;

        const auto& points = m_points[c];
        bool channelIsDefault = isDefault(points);

        // Draw if it is the active channel, or if it has a non-default stretch
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

            // Use the cached LUT — no spline recomputation here
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
            if (m_channelsLinked) {
                for (int c = 0; c < 6; ++c) {
                    if (c != m_activeChannel) m_points[c] = points;
                }
                for (int c = 0; c < 6; ++c) rebuildLut(c);
            } else {
                rebuildLut(m_activeChannel);
            }
            emit curveChanged(m_activeChannel, points);
            update();
        }
    } else if (event->button() == Qt::RightButton) {
        if (closestIdx != -1 && dist < 15.0) {
            auto& points = m_points[m_activeChannel];
            if (closestIdx > 0 && closestIdx < points.size() - 1) { // Can't delete ends
                points.erase(points.begin() + closestIdx);
                if (m_channelsLinked) {
                    for (int c = 0; c < 6; ++c) {
                        if (c != m_activeChannel) m_points[c] = points;
                    }
                    for (int c = 0; c < 6; ++c) rebuildLut(c);
                } else {
                    rebuildLut(m_activeChannel);
                }
                emit curveChanged(m_activeChannel, points);
                update();
            }
        }
    }
}

void CurvesWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        m_dragPointIndex = -1;
    }
}

void CurvesWidget::mouseMoveEvent(QMouseEvent* event) {
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

        if (m_channelsLinked) {
            for (int c = 0; c < 6; ++c) {
                if (c != m_activeChannel) m_points[c] = points;
            }
            for (int c = 0; c < 6; ++c) rebuildLut(c);
        } else {
            rebuildLut(m_activeChannel);
        }

        emit curveChanged(m_activeChannel, points);
        update();
    }
}

} // namespace blastro
