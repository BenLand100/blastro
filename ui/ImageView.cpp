#include "ImageView.h"
#include <QScrollBar>
#include <QMouseEvent>
#include <QKeyEvent>
#include <cmath>
#include <algorithm>
#include <vector>

namespace blastro {

ImageView::ImageView(QWidget* parent)
    : QGraphicsView(parent),
      m_scene(new QGraphicsScene(this)),
      m_zoomFactor(1.0),
      m_displayMode(Normal),
      m_blackpoint(0.0),
      m_whitepoint(1.0),
      m_midpoint(0.5),
      m_isPanning(false),
      m_isFrameSelected(true) {
      
    setScene(m_scene);
    viewport()->setMouseTracking(true);
    
    // Set view properties
    setRenderHint(QPainter::Antialiasing, false); // Keep pixels sharp
    setRenderHint(QPainter::SmoothPixmapTransform, false);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    
    // UI Styling - Dark theme for astro imaging
    setStyleSheet("background-color: #1e1e1e; border: none;");
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Precompute initial LUT
    updateLUT();
}

void ImageView::setImage(const ImageVariant& image, bool preserveStretch) {
    m_currentImage = image;
    
    // Set scene rect to the image dimensions
    int w = 0, h = 0;
    if (std::holds_alternative<GrayscaleImagePtr>(m_currentImage)) {
        auto img = std::get<GrayscaleImagePtr>(m_currentImage);
        if (img) { w = img->width(); h = img->height(); }
    } else if (std::holds_alternative<RGBImagePtr>(m_currentImage)) {
        auto img = std::get<RGBImagePtr>(m_currentImage);
        if (img) { w = img->width(); h = img->height(); }
    }
    m_scene->setSceneRect(0, 0, w, h);
    
    // Auto-fit to the MDI window on load
    if (viewport()->width() > 100 && viewport()->height() > 100) {
        fitToWindow();
    } else {
        m_fitOnNextResize = true;
    }
    
    if (preserveStretch) {
        updateView();
    } else {
        if (m_displayMode == Autostretch) {
            runAutostretch();
        } else {
            updateView();
        }
    }
}

void ImageView::setDisplayMode(DisplayMode mode) {
    if (mode == Autostretch) {
        runAutostretch();
    } else {
        m_displayMode = mode;
        updateLUT(); // Precompute LUT for the new mode
        updateView();
    }
}

void ImageView::setStretchParams(double b, double w, double m) {
    m_blackpoint = b;
    m_whitepoint = w;
    m_midpoint = m;
    
    if (m_displayMode == Normal) {
        m_displayMode = Stretch;
    }
    
    updateLUT(); // Precompute LUT with new parameters
    updateView();
    emit stretchParamsChanged(b, w, m);
}

void ImageView::updateLUT() {
    m_lut.resize(65536);
    float B = static_cast<float>(m_blackpoint);
    float W = static_cast<float>(m_whitepoint);
    float M = static_cast<float>(m_midpoint);
    
    for (int i = 0; i < 65536; ++i) {
        float v = static_cast<float>(i) / 65535.0f;
        float stretched = applyMTF(v, B, W, M);
        m_lut[i] = static_cast<unsigned char>(qBound(0.0f, stretched * 255.0f, 255.0f));
    }
}

void ImageView::runAutostretch() {
    std::vector<float> samples;
    int width = 0;
    int height = 0;

    if (std::holds_alternative<GrayscaleImagePtr>(m_currentImage)) {
        auto img = std::get<GrayscaleImagePtr>(m_currentImage);
        if (!img) return;
        width = img->width();
        height = img->height();
    } else if (std::holds_alternative<RGBImagePtr>(m_currentImage)) {
        auto img = std::get<RGBImagePtr>(m_currentImage);
        if (!img) return;
        width = img->width();
        height = img->height();
    } else {
        return;
    }

    // Sample pixels uniformly (up to 20,000 samples for fast statistics)
    int totalPixels = width * height;
    int sampleStep = std::max(1, totalPixels / 20000);
    samples.reserve(totalPixels / sampleStep);

    auto getPixelLuminance = [&](int x, int y) {
        if (std::holds_alternative<GrayscaleImagePtr>(m_currentImage)) {
            return std::get<GrayscaleImagePtr>(m_currentImage)->buffer()->pixel(x, y);
        } else {
            auto rgb = std::get<RGBImagePtr>(m_currentImage);
            float r = rgb->r()->buffer()->pixel(x, y);
            float g = rgb->g()->buffer()->pixel(x, y);
            float b = rgb->b()->buffer()->pixel(x, y);
            return (r + g + b) / 3.0f;
        }
    };

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int linearIdx = y * width + x;
            if (linearIdx % sampleStep == 0) {
                samples.push_back(getPixelLuminance(x, y));
            }
        }
    }

    if (samples.empty()) return;

    // Sort to find quantiles
    std::sort(samples.begin(), samples.end());

    // Guess Blackpoint (2% quantile)
    int bpIdx = static_cast<int>(samples.size() * 0.02);
    double bp = samples[bpIdx];

    // Guess Whitepoint (98% quantile)
    int wpIdx = static_cast<int>(samples.size() * 0.98);
    double wp = samples[wpIdx];

    // Ensure bp < wp
    if (bp >= wp) {
        bp = 0.0;
        wp = 1.0;
    }

    // Find Median
    int medIdx = static_cast<int>(samples.size() * 0.5);
    double originalMedian = samples[medIdx];

    // Normalize median to the clipped [bp, wp] range
    double med = (originalMedian - bp) / (wp - bp);
    med = std::max(0.001, std::min(0.999, med));

    // Target background median is 0.25 (standard for astrophotography stretch)
    double T = 0.25;
    
    // Solve for midpoint: M = (med * (T - 1)) / (2 * med * T - T - med)
    double mp = (med * (T - 1.0)) / (2.0 * med * T - T - med);
    
    // Clamp midpoint to a reasonable range
    mp = std::max(0.001, std::min(0.999, mp));

    m_blackpoint = bp;
    m_whitepoint = wp;
    m_midpoint = mp;
    m_displayMode = Autostretch;

    updateLUT(); // Precompute LUT for autostretch parameters
    updateView();
    emit stretchParamsChanged(m_blackpoint, m_whitepoint, m_midpoint);
}

std::vector<int> ImageView::getHistogram(int bins) const {
    std::vector<int> hist(bins, 0);
    int width = 0;
    int height = 0;

    if (std::holds_alternative<GrayscaleImagePtr>(m_currentImage)) {
        auto img = std::get<GrayscaleImagePtr>(m_currentImage);
        if (!img) return hist;
        width = img->width();
        height = img->height();
    } else if (std::holds_alternative<RGBImagePtr>(m_currentImage)) {
        auto img = std::get<RGBImagePtr>(m_currentImage);
        if (!img) return hist;
        width = img->width();
        height = img->height();
    } else {
        return hist;
    }

    int totalPixels = width * height;
    int sampleStep = std::max(1, totalPixels / 50000);

    auto getPixelLuminance = [&](int x, int y) {
        if (std::holds_alternative<GrayscaleImagePtr>(m_currentImage)) {
            return std::get<GrayscaleImagePtr>(m_currentImage)->buffer()->pixel(x, y);
        } else {
            auto rgb = std::get<RGBImagePtr>(m_currentImage);
            float r = rgb->r()->buffer()->pixel(x, y);
            float g = rgb->g()->buffer()->pixel(x, y);
            float b = rgb->b()->buffer()->pixel(x, y);
            return (r + g + b) / 3.0f;
        }
    };

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int linearIdx = y * width + x;
            if (linearIdx % sampleStep == 0) {
                float val = getPixelLuminance(x, y);
                int bin = std::max(0, std::min(bins - 1, static_cast<int>(val * (bins - 1))));
                hist[bin]++;
            }
        }
    }

    return hist;
}

void ImageView::zoomIn() {
    m_zoomFactor *= 1.2;
    scale(1.2, 1.2);
}

void ImageView::zoomOut() {
    m_zoomFactor /= 1.2;
    scale(1.0 / 1.2, 1.0 / 1.2);
}

void ImageView::resetZoom() {
    resetTransform();
    m_zoomFactor = 1.0;
}

void ImageView::fitToWindow() {
    int imgW = 0, imgH = 0;
    if (std::holds_alternative<GrayscaleImagePtr>(m_currentImage)) {
        auto img = std::get<GrayscaleImagePtr>(m_currentImage);
        if (img) { imgW = img->width(); imgH = img->height(); }
    } else if (std::holds_alternative<RGBImagePtr>(m_currentImage)) {
        auto img = std::get<RGBImagePtr>(m_currentImage);
        if (img) { imgW = img->width(); imgH = img->height(); }
    }
    
    if (imgW <= 0 || imgH <= 0) return;
    
    int vpW = viewport()->width();
    int vpH = viewport()->height();
    if (vpW <= 100 || vpH <= 100) return;
    
    resetTransform();
    double scaleX = static_cast<double>(vpW) / imgW;
    double scaleY = static_cast<double>(vpH) / imgH;
    double factor = std::min(scaleX, scaleY);
    
    scale(factor, factor);
    m_zoomFactor = factor;
}

void ImageView::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    if (m_fitOnNextResize) {
        fitToWindow();
        m_fitOnNextResize = false;
    }
}

void ImageView::wheelEvent(QWheelEvent* event) {
    if (event->angleDelta().y() > 0) {
        zoomIn();
    } else {
        zoomOut();
    }
    event->accept();
}

void ImageView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton || event->button() == Qt::MiddleButton) {
        m_isPanning = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void ImageView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton || event->button() == Qt::MiddleButton) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void ImageView::mouseMoveEvent(QMouseEvent* event) {
    if (m_isPanning) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();
        
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        event->accept();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }

    // Capture coordinates and pixel values
    int imgW = 0, imgH = 0;
    const float* bufGray = nullptr;
    const float* bufR = nullptr;
    const float* bufG = nullptr;
    const float* bufB = nullptr;
    bool isRGB = false;

    if (std::holds_alternative<GrayscaleImagePtr>(m_currentImage)) {
        auto img = std::get<GrayscaleImagePtr>(m_currentImage);
        if (img && img->buffer()) {
            imgW = img->width();
            imgH = img->height();
            bufGray = img->buffer()->data();
        }
    } else if (std::holds_alternative<RGBImagePtr>(m_currentImage)) {
        auto img = std::get<RGBImagePtr>(m_currentImage);
        if (img && img->r() && img->g() && img->b()) {
            imgW = img->width();
            imgH = img->height();
            bufR = img->r()->buffer()->data();
            bufG = img->g()->buffer()->data();
            bufB = img->b()->buffer()->data();
            isRGB = true;
        }
    }

    if (imgW > 0 && imgH > 0) {
        QPointF scenePt = mapToScene(event->pos());
        int sx = static_cast<int>(std::floor(scenePt.x()));
        int sy = static_cast<int>(std::floor(scenePt.y()));

        if (sx >= 0 && sx < imgW && sy >= 0 && sy < imgH) {
            int idx = sy * imgW + sx;
            std::vector<float> values;
            if (isRGB) {
                values = {bufR[idx], bufG[idx], bufB[idx]};
            } else {
                values = {bufGray[idx]};
            }
            emit mousePosChanged(sx, sy, isRGB, values);
        } else {
            emit mousePosChanged(-1, -1, false, {});
        }
    } else {
        emit mousePosChanged(-1, -1, false, {});
    }
}

void ImageView::updateView() {
    viewport()->update();
}

float ImageView::applyMTF(float v, float B, float W, float M) {
    float y = (v - B) / (W - B);
    y = std::max(0.0f, std::min(1.0f, y));
    
    float denom = (2.0f * M - 1.0f) * y - M;
    if (std::abs(denom) < 1e-6f) {
        denom = (denom >= 0.0f) ? 1e-6f : -1e-6f;
    }
    float z = ((M - 1.0f) * y) / denom;
    return std::max(0.0f, std::min(1.0f, z));
}

void ImageView::drawBackground(QPainter* painter, const QRectF& rect) {
    Q_UNUSED(rect);
    
    int vpW = viewport()->width();
    int vpH = viewport()->height();
    if (vpW <= 0 || vpH <= 0) return;

    // 1. Get the image dimensions and buffer pointers
    int imgW = 0, imgH = 0;
    const float* bufGray = nullptr;
    const float* bufR = nullptr;
    const float* bufG = nullptr;
    const float* bufB = nullptr;
    bool isRGB = false;

    if (std::holds_alternative<GrayscaleImagePtr>(m_currentImage)) {
        auto img = std::get<GrayscaleImagePtr>(m_currentImage);
        if (img && img->buffer()) {
            imgW = img->width();
            imgH = img->height();
            bufGray = img->buffer()->data();
        }
    } else if (std::holds_alternative<RGBImagePtr>(m_currentImage)) {
        auto img = std::get<RGBImagePtr>(m_currentImage);
        if (img && img->r() && img->g() && img->b()) {
            imgW = img->width();
            imgH = img->height();
            bufR = img->r()->buffer()->data();
            bufG = img->g()->buffer()->data();
            bufB = img->b()->buffer()->data();
            isRGB = true;
        }
    }

    if (imgW <= 0 || imgH <= 0) {
        // No image loaded, draw background color
        painter->fillRect(rect, QColor("#1e1e1e"));
        return;
    }

    // 2. Create viewport-sized QImage
    QImage vpImg(vpW, vpH, QImage::Format_RGB32);

    // 3. Precompute transform coefficients from viewport (vx, vy) to scene (sx, sy)
    QPointF p0 = mapToScene(0, 0);
    QPointF p1 = mapToScene(1, 0);
    QPointF p2 = mapToScene(0, 1);
    double dx = p1.x() - p0.x();
    double dy = p2.y() - p0.y();

    // Background color: #1e1e1e (30, 30, 30)
    QRgb bgRgb = qRgb(30, 30, 30);

    // 4. Render loop
    double sy = p0.y();
    for (int vy = 0; vy < vpH; ++vy) {
        QRgb* line = reinterpret_cast<QRgb*>(vpImg.scanLine(vy));
        double sx = p0.x();
        for (int vx = 0; vx < vpW; ++vx) {
            int isx = static_cast<int>(sx);
            int isy = static_cast<int>(sy);

            if (isx >= 0 && isx < imgW && isy >= 0 && isy < imgH) {
                int idx = isy * imgW + isx;
                if (isRGB) {
                    float r = bufR[idx];
                    float g = bufG[idx];
                    float b = bufB[idx];

                    if (m_displayMode != Normal) {
                        int idxR = qBound(0, static_cast<int>(r * 65535.0f), 65535);
                        int idxG = qBound(0, static_cast<int>(g * 65535.0f), 65535);
                        int idxB = qBound(0, static_cast<int>(b * 65535.0f), 65535);
                        line[vx] = qRgb(m_lut[idxR], m_lut[idxG], m_lut[idxB]);
                    } else {
                        int rVal = qBound(0, static_cast<int>(r * 255.0f), 255);
                        int gVal = qBound(0, static_cast<int>(g * 255.0f), 255);
                        int bVal = qBound(0, static_cast<int>(b * 255.0f), 255);
                        line[vx] = qRgb(rVal, gVal, bVal);
                    }
                } else {
                    float val = bufGray[idx];

                    if (m_displayMode != Normal) {
                        int idxLut = qBound(0, static_cast<int>(val * 65535.0f), 65535);
                        line[vx] = qRgb(m_lut[idxLut], m_lut[idxLut], m_lut[idxLut]);
                    } else {
                        int grayVal = qBound(0, static_cast<int>(val * 255.0f), 255);
                        line[vx] = qRgb(grayVal, grayVal, grayVal);
                    }
                }
            } else {
                line[vx] = bgRgb;
            }
            sx += dx;
        }
        sy += dy;
    }

    // 5. Draw the viewport image directly on the viewport (identity transform)
    painter->save();
    painter->setTransform(QTransform()); // Reset transform to viewport coordinates
    painter->drawImage(0, 0, vpImg);

    // If the frame is not selected, draw a red X overlay
    if (!m_isFrameSelected) {
        QPen pen(QColor(220, 50, 50, 180), 4); // Semi-transparent red, width 4
        painter->setPen(pen);
        painter->drawLine(10, 10, vpW - 10, vpH - 10);
        painter->drawLine(10, vpH - 10, vpW - 10, 10);
    }

    painter->restore();
}

void ImageView::setFrameSelectedStatus(bool selected) {
    if (m_isFrameSelected != selected) {
        m_isFrameSelected = selected;
        viewport()->update();
    }
}

void ImageView::setShowStars(bool show) {
    if (m_showStars != show) {
        m_showStars = show;
        viewport()->update();
    }
}

void ImageView::setShowConstellations(bool show) {
    if (m_showConstellations != show) {
        m_showConstellations = show;
        viewport()->update();
    }
}

void ImageView::setStars(const std::vector<Star>& stars) {
    m_stars = stars;
    viewport()->update();
}

void ImageView::drawForeground(QPainter* painter, const QRectF& rect) {
    Q_UNUSED(rect);
    
    // Draw star circles
    if (m_showStars && !m_stars.empty()) {
        painter->save();
        QPen pen(Qt::green);
        pen.setWidth(1);
        pen.setCosmetic(true); // Keep thickness independent of zoom!
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);

        for (const auto& star : m_stars) {
            painter->drawEllipse(QPointF(star.x, star.y), 6.0, 6.0);
            painter->drawLine(QPointF(star.x - 2, star.y), QPointF(star.x + 2, star.y));
            painter->drawLine(QPointF(star.x, star.y - 2), QPointF(star.x, star.y + 2));
        }
        painter->restore();
    }

    // Draw constellation lines
    if (m_showConstellations && !m_stars.empty()) {
        painter->save();
        QPen pen(QColor(0, 191, 255, 120)); // Semi-transparent DeepSkyBlue
        pen.setWidth(1);
        pen.setCosmetic(true);
        painter->setPen(pen);

        int numStars = m_stars.size();
        int kNeighbors = 3;
        for (int i = 0; i < numStars; ++i) {
            std::vector<std::pair<double, int>> dists;
            for (int j = 0; j < numStars; ++j) {
                if (i == j) continue;
                double dx = m_stars[i].x - m_stars[j].x;
                double dy = m_stars[i].y - m_stars[j].y;
                dists.push_back({dx*dx + dy*dy, j});
            }
            std::sort(dists.begin(), dists.end());
            int limit = std::min(kNeighbors, (int)dists.size());
            for (int k = 0; k < limit; ++k) {
                int neighborIdx = dists[k].second;
                if (i < neighborIdx) {
                    painter->drawLine(QPointF(m_stars[i].x, m_stars[i].y), 
                                     QPointF(m_stars[neighborIdx].x, m_stars[neighborIdx].y));
                }
            }
        }
        painter->restore();
    }
}

void ImageView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right || event->key() == Qt::Key_Space) {
        // Ignore these key events so they bubble up to the parent widget (BatchImageWidget)
        event->ignore();
        return;
    }
    QGraphicsView::keyPressEvent(event);
}

} // namespace blastro
