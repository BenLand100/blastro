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

#include "HistogramWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <cmath>
#include <algorithm>
#include "core/MathUtils.h"
#include "core/Preferences.h"

namespace blastro {

HistogramWidget::HistogramWidget(QWidget* parent)
    : QWidget(parent),
      m_zoom(1.0),
      m_scrollOffset(0.0),
      m_active(false),
      m_isScrolling(false) {
      
    m_blackpoint.fill(0.0);
    m_whitepoint.fill(1.0);
    m_midpoint.fill(0.5);
    
    m_spPoint.fill(0.5);
    m_stretchFactor.fill(3.0);
    m_shadowProtect.fill(0.0);
    m_highlightProtect.fill(1.0);
    
    setMouseTracking(true);
    setStyleSheet("background-color: transparent; border: none;");
}

void HistogramWidget::setActive(bool active) {
    m_active = active;
    update();
}

void HistogramWidget::setActiveChannel(int channel) {
    m_activeChannel = channel;
    m_cacheDirty = true;
    update();
}

void HistogramWidget::setSingleChannelColor(const QColor& color) {
    if (m_singleChannelColor != color) {
        m_singleChannelColor = color;
        m_cacheDirty = true;
        update();
    }
}

void HistogramWidget::setGhsMode(bool ghs) {
    m_ghsMode = ghs;
    update();
}

void HistogramWidget::setChannelsLinked(bool linked) {
    m_channelsLinked = linked;
    update();
}

void HistogramWidget::setDrawCurve(bool draw) {
    m_drawCurve = draw;
    update();
}

void HistogramWidget::setGhsParams(const std::array<double, 3>& sp, const std::array<double, 3>& d) {
    m_spPoint = sp;
    m_stretchFactor = d;
    update();
}

void HistogramWidget::setGhsProtections(const std::array<double, 3>& shadowProtect, const std::array<double, 3>& highlightProtect) {
    m_shadowProtect = shadowProtect;
    m_highlightProtect = highlightProtect;
    update();
}

void HistogramWidget::setHistograms(const std::vector<std::vector<int>>& hists) {
    m_histograms = hists;
    m_cacheDirty = true;
    update();
}

void HistogramWidget::setStretchParams(const std::array<double, 3>& b, const std::array<double, 3>& w, const std::array<double, 3>& m) {
    m_blackpoint = b;
    m_whitepoint = w;
    m_midpoint = m;
    update();
}

double HistogramWidget::valueToX(double val) const {
    return (val - m_scrollOffset) * m_zoom * width();
}

double HistogramWidget::xToValue(double x) const {
    double w = width();
    if (w <= 0.0) return 0.0;
    return m_scrollOffset + x / (m_zoom * w);
}

HistogramWidget::DragTarget HistogramWidget::getCloseLine(const QPoint& pos) const {
    if (!m_active) return {DragTarget::None, 0};

    const double threshold = 10.0;
    double minDist = threshold;
    DragTarget target = {DragTarget::None, 0};
    double h = height();

    int logicalActive = m_activeChannel;
    if (logicalActive > 2 || m_channelsLinked) {
        logicalActive = 0;
    }

    for (int c = logicalActive; c <= logicalActive; ++c) {
        if (m_ghsMode) {
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
        } else {
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

static double evaluateLocalGhs(double val, double symmetryPoint, double stretchFactor, double shadowProtect, double highlightProtect) {
    double spread = 1.0;
    if (stretchFactor < 1e-5) {
        double stretched = std::clamp((val - 0.0) / spread, 0.0, 1.0);
        if (shadowProtect > 0.0 && val < symmetryPoint) {
            double w_s = shadowProtect * std::exp(-val / (symmetryPoint + 1e-5));
            stretched = (1.0 - w_s) * stretched + w_s * val;
        }
        double effectiveHighlightProtect = 1.0 - highlightProtect;
        if (effectiveHighlightProtect > 0.0 && val > symmetryPoint) {
            double w_h = effectiveHighlightProtect * std::exp(-(1.0 - val) / (1.0 - symmetryPoint + 1e-5));
            stretched = (1.0 - w_h) * stretched + w_h * val;
        }
        return stretched;
    }

    double D = std::exp(stretchFactor) - 1.0;
    double x = (val - symmetryPoint) / spread;
    double bound = symmetryPoint / spread;
    double a = -localGhsFunction(bound, D, 1);
    double b = localGhsFunction(1.0 - bound, D, 1);
    double denom = b - a;
    if (std::abs(denom) < 1e-8) denom = 1e-8;
    double result = (x >= 0.0) ? localGhsFunction(x, D, 1) : -localGhsFunction(-x, D, 1);
    double stretched = (result - a) / denom;
    stretched = std::clamp(stretched, 0.0, 1.0);

    if (shadowProtect > 0.0 && val < symmetryPoint) {
        double w_s = shadowProtect * std::exp(-val / (symmetryPoint + 1e-5));
        stretched = (1.0 - w_s) * stretched + w_s * val;
    }
    double effectiveHighlightProtect = 1.0 - highlightProtect;
    if (effectiveHighlightProtect > 0.0 && val > symmetryPoint) {
        double w_h = effectiveHighlightProtect * std::exp(-(1.0 - val) / (1.0 - symmetryPoint + 1e-5));
        stretched = (1.0 - w_h) * stretched + w_h * val;
    }
    return stretched;
}

static double evaluateLocalHt(double val, double blackpoint, double whitepoint, double midpoint) {
    double x = (val - blackpoint) / (whitepoint - blackpoint + 1e-12);
    x = std::clamp(x, 0.0, 1.0);
    if (x <= 0.0) return 0.0;
    if (x >= 1.0) return 1.0;
    return (midpoint - 1.0) * x / ((2.0 * midpoint - 1.0) * x - midpoint);
}

void HistogramWidget::paintEvent(QPaintEvent* event) {
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

        // 1. Draw rounded background and border manually
        bgPainter.setPen(QPen(QColor("#333333"), 1));
        bgPainter.setBrush(QBrush(QColor("#1a1a1a")));
        bgPainter.drawRoundedRect(QRectF(0.5, 0.5, w - 1.0, h - 1.0), 4.0, 4.0);

        // 2. Draw background grid
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
                if (nonZeroCount > 0 && nonZeroCount < 500) {
                    int windowSize = std::max(5, numBins / 256 + 1);
                    if (windowSize % 2 == 0) windowSize++;
                    int half = windowSize / 2;
                    double currentSum = 0;
                    for (int i = 0; i < half && i < numBins; ++i) {
                        currentSum += hist[i];
                    }
                    for (int i = 0; i < numBins; ++i) {
                        int addIdx = i + half;
                        int removeIdx = i - half - 1;
                        if (addIdx < numBins) currentSum += hist[addIdx];
                        if (removeIdx >= 0) currentSum -= hist[removeIdx];
                        renderHist[i] = currentSum / windowSize;
                    }
                } else {
                    for (int i = 0; i < numBins; ++i) {
                        renderHist[i] = hist[i];
                    }
                }

                double maxVal = *std::max_element(renderHist.begin(), renderHist.end());
                if (maxVal > 0.0) {
                    double logMax = std::max(1e-6, std::log(maxVal + 0.8) - std::log(0.8));
                    
                    std::vector<double> yValues(w, h);
                    std::vector<bool> validX(w, false);
                    int minBinWindow = std::max(5, static_cast<int>(numBins / (w * 2.0))); // Dynamic binning based on zoom

                    for (int x = 0; x < w; ++x) {
                        double valCenter = xToValue(x);
                        if (valCenter >= 0.0 && valCenter <= 1.0) {
                            double valLeft = xToValue(x - 0.5);
                            double valRight = xToValue(x + 0.5);
                            int binStart = std::max(0, std::min(numBins - 1, static_cast<int>(valLeft * numBins)));
                            int binEnd = std::max(0, std::min(numBins - 1, static_cast<int>(valRight * numBins)));
                            if (binStart > binEnd) std::swap(binStart, binEnd);
                            
                            // Enforce minimum window size for dynamic smoothing
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
                        outline = m_singleTraceColor; outline.setAlpha(m_histograms.size() == 1 ? 200 : 80);
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

    // 4. Draw stretch curves & lines
    for (int c = 0; c < 3; ++c) {
        if (m_channelsLinked && c != 0) continue; // If linked, just draw index 0

        int logicalActive = m_activeChannel;
        if (logicalActive > 2) logicalActive = 0; // L and S map to 0

        bool isDefaultHT = (std::abs(m_blackpoint[c] - 0.0) < 1e-4 && std::abs(m_whitepoint[c] - 1.0) < 1e-4 && std::abs(m_midpoint[c] - 0.5) < 1e-4);
        bool isDefaultGHS = (std::abs(m_spPoint[c] - 0.5) < 1e-4 && std::abs(m_shadowProtect[c] - 0.0) < 1e-4 && std::abs(m_highlightProtect[c] - 1.0) < 1e-4 && m_stretchFactor[c] < 1e-4);

        bool isDefault = m_ghsMode ? isDefaultGHS : isDefaultHT;

        if (c != logicalActive && isDefault && !m_channelsLinked) {
            continue; // Skip rendering default channels that are not active
        }

        QColor lineColor;
        if (m_channelsLinked) {
            lineColor = m_singleChannelColor;
        } else {
            if (c == 0) lineColor = QColor("#cc3333"); // R
            else if (c == 1) lineColor = QColor("#33aa33"); // G
            else if (c == 2) lineColor = QColor("#3366cc"); // B
        }

        if (c != logicalActive && !m_channelsLinked) {
            lineColor.setAlpha(100); // Dimmed
        }

        if (m_ghsMode) {
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
            
            if (m_channelsLinked) {
                painter.setPen(QColor(255, 255, 255, 140));
                painter.setFont(QFont("monospace", 7));
                QString txt = QString("GHS  SP:%1 D:%2 ProtS:%3 ProtH:%4")
                              .arg(m_spPoint[0], 0, 'f', 4)
                              .arg(m_stretchFactor[0], 0, 'f', 2)
                              .arg(m_shadowProtect[0], 0, 'f', 2)
                              .arg(m_highlightProtect[0], 0, 'f', 2);
                painter.drawText(8, h - 6, txt);
            }
        } else {
            double xB = valueToX(m_blackpoint[c]);
            double xW = valueToX(m_whitepoint[c]);
            double xM = valueToX(m_blackpoint[c] + m_midpoint[c] * (m_whitepoint[c] - m_blackpoint[c]));

            if (m_channelsLinked) {
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

            if (m_channelsLinked) {
                painter.setPen(QColor(255, 255, 255, 140));
                painter.setFont(QFont("monospace", 7));
                QString txt = QString("HT   B:%1 M:%2 W:%3")
                              .arg(m_blackpoint[0], 0, 'f', 4)
                              .arg(m_midpoint[0], 0, 'f', 4)
                              .arg(m_whitepoint[0], 0, 'f', 4);
                painter.drawText(8, h - 6, txt);
            }
        }

        // 5. Draw active transfer function curve
        if (m_drawCurve) {
            QPolygonF curvePolygon;
            
            // Pre-compute constants for GHS to prevent lag during dragging
            double D = 0.0, a = 0.0, denom = 1.0, sp = m_spPoint[c];
            bool doGhsMath = false;
            if (m_ghsMode && m_stretchFactor[c] >= 1e-5) {
                D = std::exp(m_stretchFactor[c]) - 1.0;
                a = -localGhsFunction(sp, D, 1);
                double b = localGhsFunction(1.0 - sp, D, 1);
                denom = b - a;
                if (std::abs(denom) < 1e-8) denom = 1e-8;
                doGhsMath = true;
            }

            for (int px = 0; px <= w; px += 2) {
                if (px > w) px = w; // Ensure last pixel is drawn
                double val = xToValue(px);
                double yVal = 0.0;
                if (m_ghsMode) {
                    double stretched = std::clamp(val, 0.0, 1.0);
                    if (doGhsMath) {
                        double x = val - sp;
                        double result = (x >= 0.0) ? localGhsFunction(x, D, 1) : -localGhsFunction(-x, D, 1);
                        stretched = std::clamp((result - a) / denom, 0.0, 1.0);
                    }
                    
                    stretched = blastro::MathUtils::applyGhsProtection(val, sp, m_shadowProtect[c], m_highlightProtect[c], stretched);
                    yVal = stretched;
                } else {
                    yVal = evaluateLocalHt(val, m_blackpoint[c], m_whitepoint[c], m_midpoint[c]);
                }
                double py = h - 2.0 - yVal * (h - 4.0);
                curvePolygon << QPointF(px, py);
                if (px == w) break;
            }
            QPen curvePen(lineColor, 2.0); // Bolder line
            curvePen.setJoinStyle(Qt::RoundJoin);
            curvePen.setCapStyle(Qt::RoundCap);
            painter.setPen(curvePen);
            painter.setBrush(Qt::NoBrush);
            if (!curvePolygon.isEmpty()) {
                painter.drawPolyline(curvePolygon);
            }
        }
    }
}

void HistogramWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    m_cacheDirty = true;
}

void HistogramWidget::mousePressEvent(QMouseEvent* event) {
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
    } else if (event->button() == Qt::RightButton) {
        m_isScrolling = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::SizeAllCursor);
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void HistogramWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragTarget = {DragTarget::None, 0};
    } else if (event->button() == Qt::RightButton) {
        m_isScrolling = false;
    }
    setCursor(Qt::ArrowCursor);
    QWidget::mouseReleaseEvent(event);
}

void HistogramWidget::mouseMoveEvent(QMouseEvent* event) {
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

    if (m_ghsMode) {
        if (m_dragTarget.type == DragTarget::SymmetryPoint) {
            m_spPoint[c] = val;
            if (m_shadowProtect[c] > m_spPoint[c]) m_shadowProtect[c] = m_spPoint[c];
            if (m_highlightProtect[c] < m_spPoint[c]) m_highlightProtect[c] = m_spPoint[c];
            if (m_channelsLinked) {
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
            if (m_channelsLinked) {
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
            if (m_channelsLinked) {
                m_spPoint.fill(m_spPoint[c]);
                m_shadowProtect.fill(m_shadowProtect[c]);
                m_highlightProtect.fill(m_highlightProtect[c]);
            }
            emit ghsProtectionsChanged(m_shadowProtect, m_highlightProtect);
            emit ghsParamsChanged(m_spPoint, m_stretchFactor);
        }
    } else {
        if (m_dragTarget.type == DragTarget::Black) {
            m_blackpoint[c] = std::min(val, m_whitepoint[c] - 0.001);
            if (m_channelsLinked) m_blackpoint.fill(m_blackpoint[c]);
        } else if (m_dragTarget.type == DragTarget::White) {
            m_whitepoint[c] = std::max(val, m_blackpoint[c] + 0.001);
            if (m_channelsLinked) m_whitepoint.fill(m_whitepoint[c]);
        } else if (m_dragTarget.type == DragTarget::Mid) {
            if (m_whitepoint[c] > m_blackpoint[c]) {
                double relativeVal = (val - m_blackpoint[c]) / (m_whitepoint[c] - m_blackpoint[c]);
                m_midpoint[c] = std::max(0.001, std::min(0.999, relativeVal));
                if (m_channelsLinked) m_midpoint.fill(m_midpoint[c]);
            }
        }
        emit stretchParamsChanged(m_blackpoint, m_whitepoint, m_midpoint);
    }

    update();
    event->accept();
}

void HistogramWidget::wheelEvent(QWheelEvent* event) {
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

void HistogramWidget::snapToBlackToMid() {
    double actualMid = m_blackpoint[0] + m_midpoint[0] * (m_whitepoint[0] - m_blackpoint[0]);
    double diff = actualMid - m_blackpoint[0];
    if (diff > 1e-6) {
        m_zoom = 0.45 / diff;
        m_zoom = std::max(1.0, std::min(250.0, m_zoom));
        m_scrollOffset = m_blackpoint[0] - 0.05 / m_zoom;
        m_scrollOffset = std::max(0.0, std::min(1.0 - 1.0 / m_zoom, m_scrollOffset));
    } else {
        m_zoom = 1.0;
        m_scrollOffset = 0.0;
    }
    m_cacheDirty = true;
    update();
}

void HistogramWidget::resetZoom() {
    m_zoom = 1.0;
    m_scrollOffset = 0.0;
    m_cacheDirty = true;
    update();
}

void HistogramWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    // Toggle between snapped zoom and default [0, 1] view
    if (m_zoom > 1.05 || m_scrollOffset > 0.01) {
        resetZoom();
    } else {
        snapToBlackToMid();
    }
    event->accept();
}

} // namespace blastro
