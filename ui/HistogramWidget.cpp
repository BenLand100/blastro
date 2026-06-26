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
      
    setMinimumHeight(24);
    setMaximumHeight(32);
    setMouseTracking(true);
    // Remove background stylesheet border so we can draw it dynamically in paintEvent
    setStyleSheet("background-color: transparent; border: none;");
}

void HistogramWidget::setActive(bool active) {
    m_active = active;
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

    double xB = valueToX(m_blackpoint);
    double xW = valueToX(m_whitepoint);
    double xM = valueToX(m_blackpoint + m_midpoint * (m_whitepoint - m_blackpoint));

    double dxB = std::abs(pos.x() - xB);
    double dxW = std::abs(pos.x() - xW);
    double dxM = std::abs(pos.x() - xM);

    double h = height();
    // Add a penalty if the click is in the vertical half opposite to the handle's visual circle
    double distB = dxB + (pos.y() < h / 2 ? 12.0 : 0.0);
    double distW = dxW + (pos.y() < h / 2 ? 12.0 : 0.0);
    double distM = dxM + (pos.y() >= h / 2 ? 12.0 : 0.0);

    const double threshold = 10.0; // Grab threshold in pixels
    double minDist = threshold;
    DragTarget target = None;

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

    return target;
}

void HistogramWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int w = width();
    int h = height();

    // If inactive, paint nothing (transparent spacer to prevent layout shifting)
    if (!m_active) {
        return;
    }

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

    // 3. Draw Histogram Curve (Logarithmic scale) - Optimized for 16-bit (65536 bins)
    if (!m_histogram.empty()) {
        int numBins = m_histogram.size();
        
        // Detect sparsity (number of non-zero bins) to identify discrete-valued (e.g. 8-bit) images
        int nonZeroCount = 0;
        for (int val : m_histogram) {
            if (val > 0) nonZeroCount++;
        }

        // Apply a moving average filter to smooth sparse histograms (e.g. 8-bit discrete levels)
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
            
            // Loop per horizontal pixel column to keep rendering super fast
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

            // Fill with a sleek semi-transparent cyan/blue gradient
            QLinearGradient gradient(0, 0, 0, h);
            gradient.setColorAt(0, QColor(0, 122, 204, 120));
            gradient.setColorAt(1, QColor(0, 122, 204, 15));
            painter.fillPath(path, gradient);

            // Draw outline
            painter.setPen(QPen(QColor(0, 150, 255, 180), 1.0));
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

    // 4. Draw blackpoint, midpoint, and whitepoint lines
    double xB = valueToX(m_blackpoint);
    double xW = valueToX(m_whitepoint);
    double xM = valueToX(m_blackpoint + m_midpoint * (m_whitepoint - m_blackpoint));

    // Shadow out clipped regions
    if (xB > 0) {
        painter.fillRect(QRectF(0, 0, std::min(static_cast<double>(w), xB), h), QColor(0, 0, 0, 120));
    }
    if (xW < w) {
        double fillStart = std::max(0.0, xW);
        painter.fillRect(QRectF(fillStart, 0, w - fillStart, h), QColor(0, 0, 0, 120));
    }

    // Blackpoint line (Red)
    if (xB >= 0 && xB <= w) {
        painter.setPen(QPen(QColor("#ff4444"), 1.2));
        painter.drawLine(xB, 1, xB, h - 1);
        painter.setBrush(QColor("#ff4444"));
        painter.drawEllipse(QPointF(xB, h - 4), 3, 3);
    }

    // Whitepoint line (White)
    if (xW >= 0 && xW <= w) {
        painter.setPen(QPen(QColor("#ffffff"), 1.2));
        painter.drawLine(xW, 1, xW, h - 1);
        painter.setBrush(QColor("#ffffff"));
        painter.drawEllipse(QPointF(xW, h - 4), 3, 3);
    }

    // Midpoint line (Green)
    if (xM >= 0 && xM <= w) {
        painter.setPen(QPen(QColor("#44ff44"), 1.2));
        painter.drawLine(xM, 1, xM, h - 1);
        painter.setBrush(QColor("#44ff44"));
        painter.drawEllipse(QPointF(xM, 4), 3, 3);
    }

    // 5. Draw Overlay Text
    painter.setPen(QColor(255, 255, 255, 140));
    painter.setFont(QFont("monospace", 7));
    QString txt = QString("B:%1 M:%2 W:%3")
                  .arg(m_blackpoint, 0, 'f', 4)
                  .arg(m_midpoint, 0, 'f', 4)
                  .arg(m_whitepoint, 0, 'f', 4);
    if (m_zoom > 1.0) {
        txt += QString("  [Zoom: %1x]").arg(m_zoom, 0, 'f', 1);
    }
    painter.drawText(8, h - 6, txt);
}

void HistogramWidget::mousePressEvent(QMouseEvent* event) {
    if (!m_active) {
        QWidget::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
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
    if (!m_active) {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_dragTarget = None;
    } else if (event->button() == Qt::RightButton) {
        m_isScrolling = false;
    }
    setCursor(Qt::ArrowCursor);
    QWidget::mouseReleaseEvent(event);
}

void HistogramWidget::mouseMoveEvent(QMouseEvent* event) {
    if (!m_active) {
        QWidget::mouseMoveEvent(event);
        return;
    }

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

    double val = xToValue(event->pos().x());

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

    update();
    emit stretchParamsChanged(m_blackpoint, m_whitepoint, m_midpoint);
    event->accept();
}

void HistogramWidget::wheelEvent(QWheelEvent* event) {
    if (!m_active) {
        QWidget::wheelEvent(event);
        return;
    }

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

} // namespace blastro
