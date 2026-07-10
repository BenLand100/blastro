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

#include "ImageView.h"
#include <QScrollBar>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QJsonObject>
#include <QTimer>
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

void ImageView::setImage(const ImageVariant& image, bool preserveStretch, bool preserveZoom) {
    clearCLAHE();
    m_currentImage = image;
    m_hasSelection = false;
    m_selectionRect = QRect();
    emit selectionChanged();
    
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
    
    // Auto-fit to the MDI window on load, unless preserving zoom
    if (!preserveZoom) {
        if (!fitToWindow()) {
            m_fitOnNextResize = true;
        }
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
        if (m_displayMode == Autostretch) {
            m_autoStretchLevel = (m_autoStretchLevel + 1) % 3;
        } else {
            m_autoStretchLevel = 0; // Start with the lowest intensity
        }
        runAutostretch();
    } else if (mode == LocalHist) {
        if (m_displayMode == LocalHist) {
            m_localHistLevel = (m_localHistLevel + 1) % 3;
            clearCLAHE(); // Recompute cache for new clipLimitVal
        } else {
            m_localHistLevel = 0; // Start with lowest intensity
        }
        m_displayMode = LocalHist;
        updateView();
    } else {
        m_displayMode = mode;
        updateLUT(); // Precompute LUT for the new mode
        updateView();
    }
}

void ImageView::setAutoStretchLevel(int level) {
    m_autoStretchLevel = std::max(0, std::min(2, level));
    m_displayMode = Autostretch;
    runAutostretch();
}

void ImageView::setLocalHistLevel(int level) {
    int newLvl = std::max(0, std::min(2, level));
    if (m_localHistLevel != newLvl || m_displayMode != LocalHist) {
        m_localHistLevel = newLvl;
        clearCLAHE();
        m_displayMode = LocalHist;
        updateView();
    }
}

void ImageView::setChannelMode(ChannelMode mode) {
    if (m_channelMode != mode) {
        m_channelMode = mode;
        updateView();
    }
}

void ImageView::clearCLAHE() {
    m_claheGray.clear();
    m_claheR.clear();
    m_claheG.clear();
    m_claheB.clear();
}

void ImageView::setUpdatesSuspended(bool suspended) {
    if (m_updatesSuspended != suspended) {
        m_updatesSuspended = suspended;
        if (!m_updatesSuspended) {
            clearCLAHE();
        }
        updateView();
    }
}

static void runFastCLAHE(const float* src, float* dst, int width, int height, int gridX = 8, int gridY = 8, float clipLimitVal = 40.0f) {
    if (width <= 0 || height <= 0 || !src || !dst) return;
    int numBins = 256;
    int tileSizeX = width / gridX;
    int tileSizeY = height / gridY;
    if (tileSizeX <= 0 || tileSizeY <= 0) return;

    // 1. Compute histograms for each tile in parallel
    std::vector<std::vector<int>> histograms(gridX * gridY, std::vector<int>(numBins, 0));
    
    #pragma omp parallel for collapse(2)
    for (int ty = 0; ty < gridY; ++ty) {
        for (int tx = 0; tx < gridX; ++tx) {
            int tileIdx = ty * gridX + tx;
            int xStart = tx * tileSizeX;
            int yStart = ty * tileSizeY;
            int xEnd = (tx == gridX - 1) ? width : xStart + tileSizeX;
            int yEnd = (ty == gridY - 1) ? height : yStart + tileSizeY;
            int tilePixels = (xEnd - xStart) * (yEnd - yStart);

            auto& hist = histograms[tileIdx];
            for (int y = yStart; y < yEnd; ++y) {
                for (int x = xStart; x < xEnd; ++x) {
                    float val = src[y * width + x];
                    int bin = std::max(0, std::min(numBins - 1, static_cast<int>(val * (numBins - 1))));
                    hist[bin]++;
                }
            }

            // Clip histogram
            int clipLimit = static_cast<int>(clipLimitVal * tilePixels / numBins);
            if (clipLimit < 1) clipLimit = 1;
            int excess = 0;
            for (int b = 0; b < numBins; ++b) {
                if (hist[b] > clipLimit) {
                    excess += hist[b] - clipLimit;
                    hist[b] = clipLimit;
                }
            }
            int binIncr = excess / numBins;
            int remainder = excess % numBins;
            for (int b = 0; b < numBins; ++b) {
                hist[b] += binIncr;
            }
            for (int b = 0; b < remainder; ++b) {
                hist[b]++;
            }

            // Convert to CDF
            float sum = 0.0f;
            for (int b = 0; b < numBins; ++b) {
                sum += hist[b];
                hist[b] = static_cast<int>(sum);
            }
        }
    }

    // Precompute tx0, tx1, tx_weight for all x
    std::vector<int> tx0_arr(width);
    std::vector<int> tx1_arr(width);
    std::vector<float> tx_weight_arr(width);
    for (int x = 0; x < width; ++x) {
        float fx = static_cast<float>(x) / tileSizeX - 0.5f;
        int tx = std::max(0, std::min(gridX - 1, static_cast<int>(std::floor(fx))));
        tx0_arr[x] = tx;
        tx1_arr[x] = std::min(gridX - 1, tx + 1);
        tx_weight_arr[x] = fx - tx;
    }

    // Precompute tile sizes
    std::vector<int> tilePixelsList(gridX * gridY);
    for (int ty = 0; ty < gridY; ++ty) {
        for (int tx = 0; tx < gridX; ++tx) {
            int xStart = tx * tileSizeX;
            int yStart = ty * tileSizeY;
            int xEnd = (tx == gridX - 1) ? width : xStart + tileSizeX;
            int yEnd = (ty == gridY - 1) ? height : yStart + tileSizeY;
            tilePixelsList[ty * gridX + tx] = (xEnd - xStart) * (yEnd - yStart);
        }
    }

    // 2. Interpolate for each pixel in parallel (row-wise)
    #pragma omp parallel for schedule(static)
    for (int y = 0; y < height; ++y) {
        float fy = static_cast<float>(y) / tileSizeY - 0.5f;
        int ty = std::max(0, std::min(gridY - 1, static_cast<int>(std::floor(fy))));
        float ty_weight = fy - ty;
        int ty0 = ty;
        int ty1 = std::min(gridY - 1, ty + 1);

        for (int x = 0; x < width; ++x) {
            float tx_weight = tx_weight_arr[x];
            int tx0 = tx0_arr[x];
            int tx1 = tx1_arr[x];

            float val = src[y * width + x];
            int bin = std::max(0, std::min(numBins - 1, static_cast<int>(val * (numBins - 1))));

            float cdf00 = histograms[ty0 * gridX + tx0][bin];
            float cdf10 = histograms[ty0 * gridX + tx1][bin];
            float cdf01 = histograms[ty1 * gridX + tx0][bin];
            float cdf11 = histograms[ty1 * gridX + tx1][bin];

            // Bilinear interpolation
            float cdf = (1.0f - ty_weight) * ((1.0f - tx_weight) * cdf00 + tx_weight * cdf10) +
                        ty_weight * ((1.0f - tx_weight) * cdf01 + tx_weight * cdf11);

            int numPix00 = tilePixelsList[ty0 * gridX + tx0];
            float outVal = cdf / numPix00;
            dst[y * width + x] = std::max(0.0f, std::min(1.0f, outVal));
        }
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
                float val = getPixelLuminance(x, y);
                if (!std::isnan(val)) {
                    samples.push_back(val);
                }
            }
        }
    }

    if (samples.empty()) return;

    // Sort to find quantiles
    std::sort(samples.begin(), samples.end());

    // Guess Blackpoint (0.01% quantile)
    int bpIdx = static_cast<int>(samples.size() * 0.0001);
    double bp = samples[bpIdx];

    // Whitepoint is kept at 1.0 to ensure starless and star-heavy images have comparable stretches
    double wp = 1.0;

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

    // Target background median based on intensity level (0 = lowest, 1 = medium, 2 = highest)
    double T = 0.25;
    if (m_autoStretchLevel == 0) {
        T = 0.0625;
    } else if (m_autoStretchLevel == 1) {
        T = 0.125;
    }
    
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
    if (m_updatesSuspended) {
        return hist;
    }
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
                if (!std::isnan(val)) {
                    int bin = std::max(0, std::min(bins - 1, static_cast<int>(val * (bins - 1))));
                    hist[bin]++;
                }
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

bool ImageView::fitToWindow() {
    int imgW = 0, imgH = 0;
    if (std::holds_alternative<GrayscaleImagePtr>(m_currentImage)) {
        auto img = std::get<GrayscaleImagePtr>(m_currentImage);
        if (img) { imgW = img->width(); imgH = img->height(); }
    } else if (std::holds_alternative<RGBImagePtr>(m_currentImage)) {
        auto img = std::get<RGBImagePtr>(m_currentImage);
        if (img) { imgW = img->width(); imgH = img->height(); }
    }
    
    if (imgW <= 0 || imgH <= 0) return false;
    
    int vpW = viewport()->width();
    int vpH = viewport()->height();
    if (vpW <= 100 || vpH <= 100) return false;
    
    resetTransform();
    double scaleX = static_cast<double>(vpW) / imgW;
    double scaleY = static_cast<double>(vpH) / imgH;
    double factor = std::min(scaleX, scaleY);
    
    scale(factor, factor);
    m_zoomFactor = factor;
    return true;
}

QJsonObject ImageView::serializeViewState() const {
    QJsonObject obj;
    // Display mode
    obj["display_mode"] = static_cast<int>(m_displayMode);
    obj["channel_mode"] = static_cast<int>(m_channelMode);
    obj["blackpoint"] = m_blackpoint;
    obj["whitepoint"] = m_whitepoint;
    obj["midpoint"] = m_midpoint;
    obj["auto_stretch_level"] = m_autoStretchLevel;
    obj["local_hist_level"] = m_localHistLevel;
    // Zoom transform (QTransform 6 components)
    QTransform t = transform();
    obj["zoom_m11"] = t.m11();
    obj["zoom_m12"] = t.m12();
    obj["zoom_m21"] = t.m21();
    obj["zoom_m22"] = t.m22();
    obj["zoom_dx"]  = t.dx();
    obj["zoom_dy"]  = t.dy();
    // Scroll positions
    obj["scroll_h"] = horizontalScrollBar()->value();
    obj["scroll_v"] = verticalScrollBar()->value();
    return obj;
}

void ImageView::restoreViewState(const QJsonObject& obj) {
    if (obj.contains("display_mode"))
        setDisplayMode(static_cast<DisplayMode>(obj["display_mode"].toInt()));
    if (obj.contains("channel_mode"))
        setChannelMode(static_cast<ChannelMode>(obj["channel_mode"].toInt()));
    if (obj.contains("blackpoint") && obj.contains("whitepoint") && obj.contains("midpoint"))
        setStretchParams(obj["blackpoint"].toDouble(), obj["whitepoint"].toDouble(), obj["midpoint"].toDouble());
    if (obj.contains("auto_stretch_level"))
        setAutoStretchLevel(obj["auto_stretch_level"].toInt());
    if (obj.contains("local_hist_level"))
        setLocalHistLevel(obj["local_hist_level"].toInt());
    // Restore zoom transform
    if (obj.contains("zoom_m11")) {
        QTransform t(
            obj["zoom_m11"].toDouble(), obj["zoom_m12"].toDouble(),
            obj["zoom_m21"].toDouble(), obj["zoom_m22"].toDouble(),
            obj["zoom_dx"].toDouble(),  obj["zoom_dy"].toDouble());
        setTransform(t);
        m_zoomFactor = t.m11(); // approximate isotropic scale
    }
    // Restore scroll positions after the event loop settles
    if (obj.contains("scroll_h") || obj.contains("scroll_v")) {
        int sh = obj["scroll_h"].toInt();
        int sv = obj["scroll_v"].toInt();
        QTimer::singleShot(0, this, [this, sh, sv]() {
            horizontalScrollBar()->setValue(sh);
            verticalScrollBar()->setValue(sv);
        });
    }
}

void ImageView::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    if (m_fitOnNextResize) {
        if (fitToWindow()) {
            m_fitOnNextResize = false;
        }
    }
}

void ImageView::showEvent(QShowEvent* event) {
    QGraphicsView::showEvent(event);
    if (m_fitOnNextResize) {
        if (fitToWindow()) {
            m_fitOnNextResize = false;
        }
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
    if (m_bgeMode && (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) && (event->modifiers() & Qt::ControlModifier)) {
        QPointF scenePos = mapToScene(event->pos());
        QSize imgSz = currentImageSize();
        if (scenePos.x() >= 0 && scenePos.x() < imgSz.width() && scenePos.y() >= 0 && scenePos.y() < imgSz.height()) {
            auto pts = getBgeControlPoints();
            if (event->button() == Qt::LeftButton) {
                pts.push_back({scenePos.x(), scenePos.y()});
                setBgeControlPoints(pts);
                setShowBgeControlPoints(true, true);
                viewport()->update();
                event->accept();
                return;
            } else if (event->button() == Qt::RightButton) {
                double threshold = 12.0;
                if (transform().m11() > 0.0) {
                    threshold = 12.0 / transform().m11();
                }
                auto nearestIt = pts.end();
                double minDist = threshold;
                for (auto it = pts.begin(); it != pts.end(); ++it) {
                    double dx = it->first - scenePos.x();
                    double dy = it->second - scenePos.y();
                    double dist = std::sqrt(dx*dx + dy*dy);
                    if (dist < minDist) {
                        minDist = dist;
                        nearestIt = it;
                    }
                }
                if (nearestIt != pts.end()) {
                    pts.erase(nearestIt);
                    setBgeControlPoints(pts);
                    setShowBgeControlPoints(true, true);
                    viewport()->update();
                }
                event->accept();
                return;
            }
        }
    }

    if (event->button() == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier)) {
        QPointF scenePos = mapToScene(event->pos());
        ResizeHandle handle = getResizeHandleAt(scenePos);
        if (handle != HandleNone) {
            m_isResizing = true;
            m_activeHandle = handle;
            event->accept();
        } else {
            m_isSelecting = true;
            QSize imgSz = currentImageSize();
            int sx = std::clamp(static_cast<int>(scenePos.x()), 0, imgSz.width());
            int sy = std::clamp(static_cast<int>(scenePos.y()), 0, imgSz.height());
            m_selectionStart = QPoint(sx, sy);
            m_selectionRect = QRect(m_selectionStart, QSize(0, 0));
            m_hasSelection = true;
            emit selectionChanged();
            viewport()->update();
            event->accept();
        }
        return;
    }

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
    if (m_isSelecting) {
        m_isSelecting = false;
        if (m_selectionRect.width() < 4 || m_selectionRect.height() < 4) {
            m_hasSelection = false;
            m_selectionRect = QRect();
        }
        emit selectionChanged();
        viewport()->update();
        event->accept();
        return;
    }
    if (m_isResizing) {
        m_isResizing = false;
        m_activeHandle = HandleNone;
        emit selectionChanged();
        viewport()->update();
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton || event->button() == Qt::MiddleButton) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void ImageView::mouseMoveEvent(QMouseEvent* event) {
    if (m_updatesSuspended) {
        emit mousePosChanged(-1, -1, false, {});
        QGraphicsView::mouseMoveEvent(event);
        return;
    }

    QPointF scenePos = mapToScene(event->pos());

    if (m_isSelecting) {
        QSize imgSz = currentImageSize();
        int cx = std::clamp(static_cast<int>(scenePos.x()), 0, imgSz.width());
        int cy = std::clamp(static_cast<int>(scenePos.y()), 0, imgSz.height());
        m_selectionRect = QRect(m_selectionStart, QPoint(cx, cy)).normalized();
        viewport()->update();
        event->accept();
        return;
    }

    if (m_isResizing) {
        QSize imgSz = currentImageSize();
        int cx = std::clamp(static_cast<int>(scenePos.x()), 0, imgSz.width());
        int cy = std::clamp(static_cast<int>(scenePos.y()), 0, imgSz.height());

        int left = m_selectionRect.left();
        int right = m_selectionRect.right();
        int top = m_selectionRect.top();
        int bottom = m_selectionRect.bottom();

        switch (m_activeHandle) {
            case HandleTopLeft:
                left = cx;
                top = cy;
                break;
            case HandleTopRight:
                right = cx;
                top = cy;
                break;
            case HandleBottomLeft:
                left = cx;
                bottom = cy;
                break;
            case HandleBottomRight:
                right = cx;
                bottom = cy;
                break;
            case HandleLeft:
                left = cx;
                break;
            case HandleRight:
                right = cx;
                break;
            case HandleTop:
                top = cy;
                break;
            case HandleBottom:
                bottom = cy;
                break;
            default:
                break;
        }

        m_selectionRect = QRect(QPoint(left, top), QPoint(right, bottom)).normalized();
        viewport()->update();
        event->accept();
        return;
    }

    if (m_isPanning) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();
        
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        event->accept();
    } else {
        if (event->modifiers() & Qt::ShiftModifier) {
            ResizeHandle handle = getResizeHandleAt(scenePos);
            updateResizeCursor(handle);
        } else {
            setCursor(Qt::ArrowCursor);
        }
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

    if (m_updatesSuspended) {
        if (!m_cachedViewportImage.isNull()) {
            painter->save();
            painter->setTransform(QTransform()); // Reset transform to viewport coordinates
            
            if (m_cachedViewportImage.size() != QSize(vpW, vpH)) {
                painter->drawImage(0, 0, m_cachedViewportImage.scaled(vpW, vpH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            } else {
                painter->drawImage(0, 0, m_cachedViewportImage);
            }
            
            // If the frame is not selected, draw a red X overlay
            if (!m_isFrameSelected) {
                QPen pen(QColor(220, 50, 50, 180), 4);
                painter->setPen(pen);
                painter->drawLine(10, 10, vpW - 10, vpH - 10);
                painter->drawLine(10, vpH - 10, vpW - 10, 10);
            }
            
            painter->restore();
            return;
        } else {
            painter->fillRect(viewport()->rect(), QColor("#1e1e1e"));
            return;
        }
    }

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
            if (m_displayMode == LocalHist) {
                if (m_claheGray.empty()) {
                    m_claheGray.resize(imgW * imgH);
                    std::vector<float> stretched(imgW * imgH);
                    float B = 0.0f, W = 1.0f, M = 0.5f;
                    int totalPixels = imgW * imgH;
                    int sampleStep = std::max(1, totalPixels / 20000);
                    std::vector<float> samples;
                    samples.reserve(totalPixels / sampleStep);
                    const float* rawData = img->buffer()->data();
                    for (int i = 0; i < totalPixels; i += sampleStep) {
                        float val = rawData[i];
                        if (!std::isnan(val)) {
                            samples.push_back(val);
                        }
                    }
                    if (!samples.empty()) {
                        std::sort(samples.begin(), samples.end());
                        int bpIdx = static_cast<int>(samples.size() * 0.0001);
                        B = samples[bpIdx];
                        W = 1.0f;
                        if (B >= W) { B = 0.0f; W = 1.0f; }
                        int medIdx = static_cast<int>(samples.size() * 0.5);
                        float originalMedian = samples[medIdx];
                        float med = (originalMedian - B) / (W - B);
                        med = std::max(0.001f, std::min(0.999f, med));
                        float T = 0.25f;
                        M = (med * (T - 1.0f)) / (2.0f * med * T - T - med);
                        M = std::max(0.001f, std::min(0.999f, M));
                    }
                    #pragma omp parallel for
                    for (int i = 0; i < totalPixels; ++i) {
                        stretched[i] = applyMTF(rawData[i], B, W, M);
                    }
                    float clip = 1.5f;
                    if (m_localHistLevel == 1) clip = 3.0f;
                    else if (m_localHistLevel == 2) clip = 5.0f;
                    runFastCLAHE(stretched.data(), m_claheGray.data(), imgW, imgH, 8, 8, clip);
                }
                bufGray = m_claheGray.data();
            } else {
                bufGray = img->buffer()->data();
            }
        }
    } else if (std::holds_alternative<RGBImagePtr>(m_currentImage)) {
        auto img = std::get<RGBImagePtr>(m_currentImage);
        if (img && img->r() && img->g() && img->b()) {
            imgW = img->width();
            imgH = img->height();
            
            const float* origR = img->r()->buffer()->data();
            const float* origG = img->g()->buffer()->data();
            const float* origB = img->b()->buffer()->data();

            if (m_displayMode == LocalHist) {
                if (m_claheR.empty()) {
                    m_claheR.resize(imgW * imgH);
                    m_claheG.resize(imgW * imgH);
                    m_claheB.resize(imgW * imgH);
                    
                    std::vector<float> stretched(imgW * imgH);
                    int totalPixels = imgW * imgH;
                    int sampleStep = std::max(1, totalPixels / 20000);
                    std::vector<float> samples;
                    samples.reserve(totalPixels / sampleStep);
                    for (int i = 0; i < totalPixels; i += sampleStep) {
                        float val = (origR[i] + origG[i] + origB[i]) / 3.0f;
                        if (!std::isnan(val)) {
                            samples.push_back(val);
                        }
                    }
                    float B = 0.0f, W = 1.0f, M = 0.5f;
                    if (!samples.empty()) {
                        std::sort(samples.begin(), samples.end());
                        int bpIdx = static_cast<int>(samples.size() * 0.0001);
                        B = samples[bpIdx];
                        W = 1.0f;
                        if (B >= W) { B = 0.0f; W = 1.0f; }
                        int medIdx = static_cast<int>(samples.size() * 0.5);
                        float originalMedian = samples[medIdx];
                        float med = (originalMedian - B) / (W - B);
                        med = std::max(0.001f, std::min(0.999f, med));
                        float T = 0.25f;
                        M = (med * (T - 1.0f)) / (2.0f * med * T - T - med);
                        M = std::max(0.001f, std::min(0.999f, M));
                    }
                    
                    float clip = 1.5f;
                    if (m_localHistLevel == 1) clip = 3.0f;
                    else if (m_localHistLevel == 2) clip = 5.0f;
                    
                    #pragma omp parallel for
                    for (int i = 0; i < totalPixels; ++i) {
                        stretched[i] = applyMTF(origR[i], B, W, M);
                    }
                    runFastCLAHE(stretched.data(), m_claheR.data(), imgW, imgH, 8, 8, clip);
                    
                    #pragma omp parallel for
                    for (int i = 0; i < totalPixels; ++i) {
                        stretched[i] = applyMTF(origG[i], B, W, M);
                    }
                    runFastCLAHE(stretched.data(), m_claheG.data(), imgW, imgH, 8, 8, clip);
                    
                    #pragma omp parallel for
                    for (int i = 0; i < totalPixels; ++i) {
                        stretched[i] = applyMTF(origB[i], B, W, M);
                    }
                    runFastCLAHE(stretched.data(), m_claheB.data(), imgW, imgH, 8, 8, clip);
                }
                bufR = m_claheR.data();
                bufG = m_claheG.data();
                bufB = m_claheB.data();
            } else {
                bufR = origR;
                bufG = origG;
                bufB = origB;
            }

            // Channel selection redirection
            if (m_channelMode == RED_ONLY) {
                bufGray = bufR;
                isRGB = false;
            } else if (m_channelMode == GREEN_ONLY) {
                bufGray = bufG;
                isRGB = false;
            } else if (m_channelMode == BLUE_ONLY) {
                bufGray = bufB;
                isRGB = false;
            } else {
                isRGB = true;
            }
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

                    if (m_displayMode == LocalHist) {
                        int rVal = qBound(0, static_cast<int>(r * 255.0f), 255);
                        int gVal = qBound(0, static_cast<int>(g * 255.0f), 255);
                        int bVal = qBound(0, static_cast<int>(b * 255.0f), 255);
                        line[vx] = qRgb(rVal, gVal, bVal);
                    } else if (m_displayMode != Normal) {
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

                    if (m_displayMode == LocalHist) {
                        int grayVal = qBound(0, static_cast<int>(val * 255.0f), 255);
                        line[vx] = qRgb(grayVal, grayVal, grayVal);
                    } else if (m_displayMode != Normal) {
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

    m_cachedViewportImage = vpImg;

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

void ImageView::setBgeMode(bool enabled) {
    if (m_bgeMode != enabled) {
        m_bgeMode = enabled;
        if (!enabled) {
            m_showBgeControlPoints = false;
        }
        viewport()->update();
    }
}

void ImageView::setShowBgeControlPoints(bool show, bool manual) {
    if (m_showBgeControlPoints != show) {
        m_showBgeControlPoints = show;
        if (manual) {
            m_bgeControlPointsManuallyToggled = true;
        }
        viewport()->update();
        emit bgeControlPointsVisibilityChanged();
    }
}

std::vector<std::pair<double, double>> ImageView::getBgeControlPoints() const {
    if (std::holds_alternative<GrayscaleImagePtr>(m_currentImage)) {
        if (auto img = std::get<GrayscaleImagePtr>(m_currentImage)) {
            if (img->buffer()) return img->buffer()->bgeControlPoints();
        }
    } else if (std::holds_alternative<RGBImagePtr>(m_currentImage)) {
        if (auto img = std::get<RGBImagePtr>(m_currentImage)) {
            if (img->r() && img->r()->buffer()) return img->r()->buffer()->bgeControlPoints();
        }
    }
    return {};
}

void ImageView::setBgeControlPoints(const std::vector<std::pair<double, double>>& pts) {
    if (std::holds_alternative<GrayscaleImagePtr>(m_currentImage)) {
        if (auto img = std::get<GrayscaleImagePtr>(m_currentImage)) {
            if (img->buffer()) img->buffer()->setBgeControlPoints(pts);
        }
    } else if (std::holds_alternative<RGBImagePtr>(m_currentImage)) {
        if (auto img = std::get<RGBImagePtr>(m_currentImage)) {
            if (img->r() && img->r()->buffer()) img->r()->buffer()->setBgeControlPoints(pts);
            if (img->g() && img->g()->buffer()) img->g()->buffer()->setBgeControlPoints(pts);
            if (img->b() && img->b()->buffer()) img->b()->buffer()->setBgeControlPoints(pts);
        }
    }
    if (pts.empty()) {
        m_showBgeControlPoints = false;
        m_bgeControlPointsManuallyToggled = false;
        emit bgeControlPointsVisibilityChanged();
    }
    viewport()->update();
}

void ImageView::drawForeground(QPainter* painter, const QRectF& rect) {
    Q_UNUSED(rect);

    // Draw selection overlay
    if (m_hasSelection && !m_selectionRect.isEmpty()) {
        painter->save();
        
        // 1. Draw dashed border (cosmetic pen so it doesn't zoom)
        QPen pen(Qt::blue);
        pen.setWidth(1);
        pen.setCosmetic(true);
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(m_selectionRect);

        QPen pen2(Qt::white);
        pen2.setWidth(1);
        pen2.setCosmetic(true);
        pen2.setStyle(Qt::DotLine);
        painter->setPen(pen2);
        painter->drawRect(m_selectionRect);

        // 2. Draw dark stencil overlay outside selection
        QSize sz = currentImageSize();
        QRectF imageRect(0, 0, sz.width(), sz.height());
        QPainterPath path;
        path.addRect(imageRect);
        path.addRect(m_selectionRect);
        path.setFillRule(Qt::OddEvenFill);
        painter->setBrush(QColor(0, 0, 0, 100)); // ~40% opacity black
        painter->setPen(Qt::NoPen);
        painter->drawPath(path);

        painter->restore();
    }
    
    // Draw BGE Control Points
    auto bgePts = getBgeControlPoints();
    if (m_showBgeControlPoints && !bgePts.empty()) {
        painter->save();
        QPen pen(QColor("#00ffff"));
        pen.setWidth(1);
        pen.setCosmetic(true);
        painter->setPen(pen);
        painter->setBrush(QColor(0, 255, 255, 40));
        double size = 8.0;
        if (transform().m11() > 0.0) {
            size = 8.0 / transform().m11();
        }
        for (const auto& pt : bgePts) {
            QRectF sq(pt.first - size/2, pt.second - size/2, size, size);
            painter->drawRect(sq);
            painter->drawLine(QPointF(pt.first - size/4, pt.second), QPointF(pt.first + size/4, pt.second));
            painter->drawLine(QPointF(pt.first, pt.second - size/4), QPointF(pt.first, pt.second + size/4));
        }
        painter->restore();
    }

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
    if (event->key() == Qt::Key_Escape) {
        if (m_hasSelection) {
            clearSelection();
            event->accept();
            return;
        }
    }
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right || event->key() == Qt::Key_Space) {
        // Ignore these key events so they bubble up to the parent widget (BatchImageWidget)
        event->ignore();
        return;
    }
    QGraphicsView::keyPressEvent(event);
}

void ImageView::clearSelection() {
    if (m_hasSelection) {
        m_hasSelection = false;
        m_selectionRect = QRect();
        emit selectionChanged();
        viewport()->update();
    }
}

QSize ImageView::currentImageSize() const {
    if (std::holds_alternative<GrayscaleImagePtr>(m_currentImage)) {
        auto img = std::get<GrayscaleImagePtr>(m_currentImage);
        if (img) return QSize(img->width(), img->height());
    } else if (std::holds_alternative<RGBImagePtr>(m_currentImage)) {
        auto img = std::get<RGBImagePtr>(m_currentImage);
        if (img) return QSize(img->width(), img->height());
    }
    return QSize(0, 0);
}

ImageView::ResizeHandle ImageView::getResizeHandleAt(const QPointF& scenePos) const {
    if (!m_hasSelection || m_selectionRect.isEmpty()) return HandleNone;

    double tol = 8.0 / transform().m11(); // 8 screen pixels tolerance

    double x = scenePos.x();
    double y = scenePos.y();

    double left = m_selectionRect.left();
    double right = m_selectionRect.right();
    double top = m_selectionRect.top();
    double bottom = m_selectionRect.bottom();

    // Check corners first
    bool nearLeft = std::abs(x - left) <= tol;
    bool nearRight = std::abs(x - right) <= tol;
    bool nearTop = std::abs(y - top) <= tol;
    bool nearBottom = std::abs(y - bottom) <= tol;

    if (nearLeft && nearTop) return HandleTopLeft;
    if (nearRight && nearTop) return HandleTopRight;
    if (nearLeft && nearBottom) return HandleBottomLeft;
    if (nearRight && nearBottom) return HandleBottomRight;

    // Check edges next
    bool withinX = x >= (left - tol) && x <= (right + tol);
    bool withinY = y >= (top - tol) && y <= (bottom + tol);

    if (nearLeft && withinY) return HandleLeft;
    if (nearRight && withinY) return HandleRight;
    if (nearTop && withinX) return HandleTop;
    if (nearBottom && withinX) return HandleBottom;

    return HandleNone;
}

void ImageView::updateResizeCursor(ResizeHandle handle) {
    switch (handle) {
        case HandleTopLeft:
        case HandleBottomRight:
            setCursor(Qt::SizeFDiagCursor);
            break;
        case HandleTopRight:
        case HandleBottomLeft:
            setCursor(Qt::SizeBDiagCursor);
            break;
        case HandleLeft:
        case HandleRight:
            setCursor(Qt::SizeHorCursor);
            break;
        case HandleTop:
        case HandleBottom:
            setCursor(Qt::SizeVerCursor);
            break;
        default:
            setCursor(Qt::ArrowCursor);
            break;
    }
}

} // namespace blastro
