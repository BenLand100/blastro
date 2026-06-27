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

namespace blastro {

HistogramWidget::HistogramWidget(QWidget* parent)
    : QWidget(parent),
      m_blackpoint(0.0),
      m_whitepoint(1.0),
      m_midpoint(0.5),
      m_zoom(1.0),
      m_scrollOffset(0.0),
      m_active(false),
      m_dragTarget(None),
      m_isScrolling(false) {
      
    setMouseTracking(true);
    setStyleSheet("background-color: transparent; border: none;");
}

void HistogramWidget::setActive(bool active) {
    m_active = active;
    update();
}

void HistogramWidget::setGhsMode(bool ghs) {
    m_ghsMode = ghs;
    update();
}

void HistogramWidget::setDrawCurve(bool draw) {
    m_drawCurve = draw;
    update();
}

void HistogramWidget::setGhsParams(double sp, double d) {
    m_spPoint = sp;
    m_stretchFactor = d;
    update();
}

void HistogramWidget::setGhsProtections(double shadowProtect, double highlightProtect) {
    m_shadowProtect = shadowProtect;
    m_highlightProtect = highlightProtect;
    update();
}

void HistogramWidget::setHistogram(const std::vector<int>& hist) {
    m_histogram = hist;
    update();
}

void HistogramWidget::setStretchParams(double b, double w, double m) {
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
    if (!m_active) return None;

    const double threshold = 10.0;
    double minDist = threshold;
    DragTarget target = None;

    if (m_ghsMode) {
        double xSP = valueToX(m_spPoint);
        double xS = valueToX(m_shadowProtect);
        double xH = valueToX(1.0 - m_highlightProtect); // Inverted representation

        double dxSP = std::abs(pos.x() - xSP);
        double dxS = std::abs(pos.x() - xS);
        double dxH = std::abs(pos.x() - xH);

        double h = height();
        // Disambiguate handles vertically:
        // SP handle is at the top (pos.y() < h / 3)
        // Shadow handle is in the middle (pos.y() >= h / 3 && pos.y() < 2 * h / 3)
        // Highlight handle is at the bottom (pos.y() >= 2 * h / 3)
        double distSP = dxSP + (pos.y() >= h / 3 ? 12.0 : 0.0);
        double distS = dxS + ((pos.y() < h / 3 || pos.y() >= 2 * h / 3) ? 12.0 : 0.0);
        double distH = dxH + (pos.y() < 2 * h / 3 ? 12.0 : 0.0);

        if (distSP < minDist) {
            minDist = distSP;
            target = SymmetryPoint;
        }
        if (distS < minDist) {
            minDist = distS;
            target = ShadowProtect;
        }
        if (distH < minDist) {
            minDist = distH;
            target = HighlightProtect;
        }
    } else {
        double xB = valueToX(m_blackpoint);
        double xW = valueToX(m_whitepoint);
        double xM = valueToX(m_blackpoint + m_midpoint * (m_whitepoint - m_blackpoint));

        double dxB = std::abs(pos.x() - xB);
        double dxW = std::abs(pos.x() - xW);
        double dxM = std::abs(pos.x() - xM);

        double h = height();
        double distB = dxB + (pos.y() < h / 2 ? 12.0 : 0.0);
        double distW = dxW + (pos.y() < h / 2 ? 12.0 : 0.0);
        double distM = dxM + (pos.y() >= h / 2 ? 12.0 : 0.0);

        if (distB < minDist) {
            minDist = distB;
            target = Black;
        }
        if (distW < minDist) {
            minDist = distW;
            target = White;
        }
        if (distM < minDist) {
            minDist = distM;
            target = Mid;
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

    // 1. Draw rounded background and border manually
    painter.setPen(QPen(QColor("#333333"), 1));
    painter.setBrush(QBrush(QColor("#1a1a1a")));
    painter.drawRoundedRect(QRectF(0.5, 0.5, w - 1.0, h - 1.0), 4.0, 4.0);

    // 2. Draw background grid
    painter.setPen(QPen(QColor("#262626"), 1, Qt::DashLine));
    for (int i = 1; i < 10; ++i) {
        double val = static_cast<double>(i) / 10.0;
        double x = valueToX(val);
        if (x >= 0 && x <= w) {
            painter.drawLine(x, 1, x, h - 1);
        }
    }

    // 3. Draw Histogram Curve
    if (!m_histogram.empty()) {
        int numBins = m_histogram.size();
        int nonZeroCount = 0;
        for (int val : m_histogram) {
            if (val > 0) nonZeroCount++;
        }

        std::vector<double> renderHist(numBins, 0.0);
        if (nonZeroCount > 0 && nonZeroCount < 500) {
            int windowSize = std::max(5, numBins / 256 + 1);
            if (windowSize % 2 == 0) windowSize++;
            int half = windowSize / 2;
            double currentSum = 0;
            for (int i = 0; i < half && i < numBins; ++i) {
                currentSum += m_histogram[i];
            }
            for (int i = 0; i < numBins; ++i) {
                int addIdx = i + half;
                int removeIdx = i - half - 1;
                if (addIdx < numBins) currentSum += m_histogram[addIdx];
                if (removeIdx >= 0) currentSum -= m_histogram[removeIdx];
                renderHist[i] = currentSum / windowSize;
            }
        } else {
            for (int i = 0; i < numBins; ++i) {
                renderHist[i] = m_histogram[i];
            }
        }

        double maxVal = *std::max_element(renderHist.begin(), renderHist.end());
        if (maxVal > 0.0) {
            double logMax = std::log1p(maxVal);
            QPainterPath path;
            bool started = false;
            
            for (int x = 0; x < w; ++x) {
                double val = xToValue(x);
                if (val >= 0.0 && val <= 1.0) {
                    int bin = std::max(0, std::min(numBins - 1, static_cast<int>(val * (numBins - 1))));
                    double logVal = std::log1p(renderHist[bin]);
                    double y = h - (logVal / logMax) * (h - 6);
                    
                    if (!started) {
                        path.moveTo(x, h - 1);
                        path.lineTo(x, y);
                        started = true;
                    } else {
                        path.lineTo(x, y);
                    }
                }
            }
            if (started) {
                double endX = valueToX(1.0);
                path.lineTo(std::min(static_cast<double>(w), endX), h - 1);
                path.closeSubpath();
            }

            QLinearGradient gradient(0, 0, 0, h);
            gradient.setColorAt(0, QColor(0, 122, 204, 80));
            gradient.setColorAt(1, QColor(0, 122, 204, 10));
            painter.fillPath(path, gradient);

            painter.setPen(QPen(QColor(0, 150, 255, 120), 1.0));
            QPainterPath outlinePath;
            started = false;
            for (int x = 0; x < w; ++x) {
                double val = xToValue(x);
                if (val >= 0.0 && val <= 1.0) {
                    int bin = std::max(0, std::min(numBins - 1, static_cast<int>(val * (numBins - 1))));
                    double logVal = std::log1p(renderHist[bin]);
                    double y = h - (logVal / logMax) * (h - 6);
                    if (!started) {
                        outlinePath.moveTo(x, y);
                        started = true;
                    } else {
                        outlinePath.lineTo(x, y);
                    }
                }
            }
            painter.drawPath(outlinePath);
        }
    }

    if (!m_active) {
        return;
    }

    // 4. Draw stretch curves & lines
    if (m_ghsMode) {
        // Draw Symmetry Point line
        double xSP = valueToX(m_spPoint);
        if (xSP >= 0 && xSP <= w) {
            painter.setPen(QPen(QColor("#e040fb"), 1.2)); // purple/magenta for SP
            painter.drawLine(xSP, 1, xSP, h - 1);
            painter.setBrush(QColor("#e040fb"));
            painter.drawEllipse(QPointF(xSP, 4), 3, 3);
        }

        // Draw Shadow Protection line
        double xS = valueToX(m_shadowProtect);
        if (xS >= 0 && xS <= w) {
            painter.setPen(QPen(QColor("#00e5ff"), 1.2)); // cyan/light blue
            painter.drawLine(xS, 1, xS, h - 1);
            painter.setBrush(QColor("#00e5ff"));
            painter.drawEllipse(QPointF(xS, h / 2.0), 3, 3);
        }

        // Draw Highlight Protection line (inverted mapping)
        double xH = valueToX(1.0 - m_highlightProtect);
        if (xH >= 0 && xH <= w) {
            painter.setPen(QPen(QColor("#ffab40"), 1.2)); // orange/yellow
            painter.drawLine(xH, 1, xH, h - 1);
            painter.setBrush(QColor("#ffab40"));
            painter.drawEllipse(QPointF(xH, h - 4), 3, 3);
        }

        // Draw overlay text
        painter.setPen(QColor(255, 255, 255, 140));
        painter.setFont(QFont("monospace", 7));
        QString txt = QString("GHS  SP:%1 D:%2 ProtS:%3 ProtH:%4")
                      .arg(m_spPoint, 0, 'f', 4)
                      .arg(m_stretchFactor, 0, 'f', 2)
                      .arg(m_shadowProtect, 0, 'f', 2)
                      .arg(m_highlightProtect, 0, 'f', 2);
        painter.drawText(8, h - 6, txt);
    } else {
        double xB = valueToX(m_blackpoint);
        double xW = valueToX(m_whitepoint);
        double xM = valueToX(m_blackpoint + m_midpoint * (m_whitepoint - m_blackpoint));

        if (xB > 0) {
            painter.fillRect(QRectF(0, 0, std::min(static_cast<double>(w), xB), h), QColor(0, 0, 0, 120));
        }
        if (xW < w) {
            double fillStart = std::max(0.0, xW);
            painter.fillRect(QRectF(fillStart, 0, w - fillStart, h), QColor(0, 0, 0, 120));
        }

        // Blackpoint line
        if (xB >= 0 && xB <= w) {
            painter.setPen(QPen(QColor("#ff4444"), 1.2));
            painter.drawLine(xB, 1, xB, h - 1);
            painter.setBrush(QColor("#ff4444"));
            painter.drawEllipse(QPointF(xB, h - 4), 3, 3);
        }

        // Whitepoint line
        if (xW >= 0 && xW <= w) {
            painter.setPen(QPen(QColor("#ffffff"), 1.2));
            painter.drawLine(xW, 1, xW, h - 1);
            painter.setBrush(QColor("#ffffff"));
            painter.drawEllipse(QPointF(xW, h - 4), 3, 3);
        }

        // Midpoint line
        if (xM >= 0 && xM <= w) {
            painter.setPen(QPen(QColor("#44ff44"), 1.2));
            painter.drawLine(xM, 1, xM, h - 1);
            painter.setBrush(QColor("#44ff44"));
            painter.drawEllipse(QPointF(xM, 4), 3, 3);
        }

        painter.setPen(QColor(255, 255, 255, 140));
        painter.setFont(QFont("monospace", 7));
        QString txt = QString("HT   B:%1 M:%2 W:%3")
                      .arg(m_blackpoint, 0, 'f', 4)
                      .arg(m_midpoint, 0, 'f', 4)
                      .arg(m_whitepoint, 0, 'f', 4);
        painter.drawText(8, h - 6, txt);
    }

    // 5. Draw active transfer function curve
    if (m_drawCurve) {
        QPainterPath curvePath;
        bool started = false;
        for (int px = 0; px < w; ++px) {
            double val = xToValue(px);
            double yVal = 0.0;
            if (m_ghsMode) {
                yVal = evaluateLocalGhs(val, m_spPoint, m_stretchFactor, m_shadowProtect, m_highlightProtect);
            } else {
                yVal = evaluateLocalHt(val, m_blackpoint, m_whitepoint, m_midpoint);
            }
            double py = h - 2.0 - yVal * (h - 4.0);
            if (!started) {
                curvePath.moveTo(px, py);
                started = true;
            } else {
                curvePath.lineTo(px, py);
            }
        }
        painter.setPen(QPen(QColor(m_ghsMode ? "#ffaa00" : "#44ff44"), 1.6));
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(curvePath);
    }
}

void HistogramWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (!m_active) {
            QWidget::mousePressEvent(event);
            return;
        }
        m_dragTarget = getCloseLine(event->pos());
        if (m_dragTarget != None) {
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
        m_dragTarget = None;
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
        update();
        event->accept();
        return;
    }

    if (!m_active) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    if (m_dragTarget == None) {
        DragTarget hoverTarget = getCloseLine(event->pos());
        if (hoverTarget != None) {
            setCursor(Qt::SplitHCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
        QWidget::mouseMoveEvent(event);
        return;
    }

    double val = std::clamp(xToValue(event->pos().x()), 0.0, 1.0);

    if (m_ghsMode) {
        if (m_dragTarget == SymmetryPoint) {
            m_spPoint = val;
            emit ghsParamsChanged(m_spPoint, m_stretchFactor);
        } else if (m_dragTarget == ShadowProtect) {
            m_shadowProtect = val;
            emit ghsProtectionsChanged(m_shadowProtect, m_highlightProtect);
        } else if (m_dragTarget == HighlightProtect) {
            m_highlightProtect = 1.0 - val; // Inverted mapping
            emit ghsProtectionsChanged(m_shadowProtect, m_highlightProtect);
        }
    } else {
        if (m_dragTarget == Black) {
            m_blackpoint = std::min(val, m_whitepoint - 0.001);
        } else if (m_dragTarget == White) {
            m_whitepoint = std::max(val, m_blackpoint + 0.001);
        } else if (m_dragTarget == Mid) {
            if (m_whitepoint > m_blackpoint) {
                double relativeVal = (val - m_blackpoint) / (m_whitepoint - m_blackpoint);
                m_midpoint = std::max(0.001, std::min(0.999, relativeVal));
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
    
    update();
    event->accept();
}

void HistogramWidget::snapToBlackToMid() {
    double actualMid = m_blackpoint + m_midpoint * (m_whitepoint - m_blackpoint);
    double diff = actualMid - m_blackpoint;
    if (diff > 1e-6) {
        m_zoom = 0.45 / diff;
        m_zoom = std::max(1.0, std::min(250.0, m_zoom));
        m_scrollOffset = m_blackpoint - 0.05 / m_zoom;
        m_scrollOffset = std::max(0.0, std::min(1.0 - 1.0 / m_zoom, m_scrollOffset));
    } else {
        m_zoom = 1.0;
        m_scrollOffset = 0.0;
    }
    update();
}

void HistogramWidget::resetZoom() {
    m_zoom = 1.0;
    m_scrollOffset = 0.0;
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
