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
    enum DisplayMode {
        Normal,
        Stretch,
        Autostretch
    };

    explicit ImageView(QWidget* parent = nullptr);
    ~ImageView() override = default;

    void setImage(const ImageVariant& image);
    ImageVariant currentImage() const { return m_currentImage; }
    
    // Display modes and stretching
    DisplayMode displayMode() const { return m_displayMode; }
    void setDisplayMode(DisplayMode mode);
    
    double blackpoint() const { return m_blackpoint; }
    double whitepoint() const { return m_whitepoint; }
    double midpoint() const { return m_midpoint; }
    void setStretchParams(double b, double w, double m);
    void runAutostretch();
    
    std::vector<int> getHistogram(int bins = 256) const;

    void zoomIn();
    void zoomOut();
    void resetZoom();
    void fitToWindow();

    void setFrameSelectedStatus(bool selected);

signals:
    void stretchParamsChanged(double b, double w, double m);
    void mousePosChanged(int x, int y, bool isRGB, const std::vector<float>& values);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateView();
    void updateLUT();
    float applyMTF(float v, float B, float W, float M);

    QGraphicsScene* m_scene;
    ImageVariant m_currentImage;
    double m_zoomFactor;

    // Stretching state
    DisplayMode m_displayMode;
    double m_blackpoint;
    double m_whitepoint;
    double m_midpoint;
    std::vector<unsigned char> m_lut;

    // Panning state
    bool m_isPanning;
    QPoint m_lastMousePos;
    bool m_isFrameSelected;
    bool m_fitOnNextResize = false;
};

} // namespace blastro
