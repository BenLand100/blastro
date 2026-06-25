#pragma once
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>

namespace blastro {

class ImageView : public QGraphicsView {
    Q_OBJECT
public:
    explicit ImageView(QWidget* parent = nullptr);
    ~ImageView() override = default;

    void setImage(const ImageVariant& image);
    ImageVariant currentImage() const { return m_currentImage; }
    void zoomIn();
    void zoomOut();
    void resetZoom();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QImage convertToQImage(const ImageVariant& image);
    void updateView();

    QGraphicsScene* m_scene;
    QGraphicsPixmapItem* m_pixmapItem;
    ImageVariant m_currentImage;
    double m_zoomFactor;

    // Panning state
    bool m_isPanning;
    QPoint m_lastMousePos;
};

} // namespace blastro
