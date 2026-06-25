#include "ImageView.h"
#include <QScrollBar>
#include <QMouseEvent>
#include <cmath>

namespace blastro {

ImageView::ImageView(QWidget* parent)
    : QGraphicsView(parent),
      m_scene(new QGraphicsScene(this)),
      m_pixmapItem(new QGraphicsPixmapItem()),
      m_zoomFactor(1.0),
      m_isPanning(false) {
      
    setScene(m_scene);
    m_scene->addItem(m_pixmapItem);
    
    // Set view properties
    setRenderHint(QPainter::Antialiasing, false); // Keep pixels sharp
    setRenderHint(QPainter::SmoothPixmapTransform, false);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    
    // UI Styling - Dark theme for astro imaging
    setStyleSheet("background-color: #1e1e1e; border: none;");
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void ImageView::setImage(const ImageVariant& image) {
    m_currentImage = image;
    updateView();
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
}

void ImageView::updateView() {
    QImage qimg = convertToQImage(m_currentImage);
    if (!qimg.isNull()) {
        m_pixmapItem->setPixmap(QPixmap::fromImage(qimg));
        m_scene->setSceneRect(m_pixmapItem->boundingRect());
    }
}

QImage ImageView::convertToQImage(const ImageVariant& image) {
    int width = 0;
    int height = 0;

    if (std::holds_alternative<GrayscaleImagePtr>(image)) {
        auto img = std::get<GrayscaleImagePtr>(image);
        if (!img) return QImage();
        width = img->width();
        height = img->height();
    } else if (std::holds_alternative<RGBImagePtr>(image)) {
        auto img = std::get<RGBImagePtr>(image);
        if (!img) return QImage();
        width = img->width();
        height = img->height();
    } else {
        return QImage();
    }

    QImage qimg(width, height, QImage::Format_RGB32);

    if (std::holds_alternative<GrayscaleImagePtr>(image)) {
        auto img = std::get<GrayscaleImagePtr>(image);
        auto buffer = img->buffer();
        for (int y = 0; y < height; ++y) {
            QRgb* line = reinterpret_cast<QRgb*>(qimg.scanLine(y));
            for (int x = 0; x < width; ++x) {
                float val = buffer->pixel(x, y);
                int grayVal = qBound(0, static_cast<int>(val * 255.0f), 255);
                line[x] = qRgb(grayVal, grayVal, grayVal);
            }
        }
    } else {
        auto img = std::get<RGBImagePtr>(image);
        auto bufR = img->r()->buffer();
        auto bufG = img->g()->buffer();
        auto bufB = img->b()->buffer();
        for (int y = 0; y < height; ++y) {
            QRgb* line = reinterpret_cast<QRgb*>(qimg.scanLine(y));
            for (int x = 0; x < width; ++x) {
                int rVal = qBound(0, static_cast<int>(bufR->pixel(x, y) * 255.0f), 255);
                int gVal = qBound(0, static_cast<int>(bufG->pixel(x, y) * 255.0f), 255);
                int bVal = qBound(0, static_cast<int>(bufB->pixel(x, y) * 255.0f), 255);
                line[x] = qRgb(rVal, gVal, bVal);
            }
        }
    }

    return qimg;
}

} // namespace blastro
