#pragma once
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include "algorithms/StarFinder.h"
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
        Autostretch,
        LocalHist
    };

    enum ChannelMode {
        RGB_ALL,
        RED_ONLY,
        GREEN_ONLY,
        BLUE_ONLY
    };

    explicit ImageView(QWidget* parent = nullptr);
    ~ImageView() override = default;

    void setImage(const ImageVariant& image, bool preserveStretch = false);
    ImageVariant currentImage() const { return m_currentImage; }
    int autoStretchLevel() const { return m_autoStretchLevel; }
    
    // Channel mode
    ChannelMode channelMode() const { return m_channelMode; }
    void setChannelMode(ChannelMode mode);
    
    // Display modes and stretching
    DisplayMode displayMode() const { return m_displayMode; }
    void setDisplayMode(DisplayMode mode);
    void setAutoStretchLevel(int level);
    
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

    // Star/Constellation Visualization
    void setShowStars(bool show);
    void setShowConstellations(bool show);
    void setStars(const std::vector<Star>& stars);

signals:
    void stretchParamsChanged(double b, double w, double m);
    void mousePosChanged(int x, int y, bool isRGB, const std::vector<float>& values);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void drawForeground(QPainter* painter, const QRectF& rect) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateView();
    void updateLUT();
    float applyMTF(float v, float B, float W, float M);
    void clearCLAHE();

    QGraphicsScene* m_scene;
    ImageVariant m_currentImage;
    double m_zoomFactor;

    // Stretching state
    DisplayMode m_displayMode;
    ChannelMode m_channelMode = RGB_ALL;
    int m_autoStretchLevel = 0;
    double m_blackpoint;
    double m_whitepoint;
    double m_midpoint;
    std::vector<unsigned char> m_lut;

    // CLAHE cache
    mutable std::vector<float> m_claheGray;
    mutable std::vector<float> m_claheR;
    mutable std::vector<float> m_claheG;
    mutable std::vector<float> m_claheB;

    // Panning state
    bool m_isPanning;
    QPoint m_lastMousePos;
    bool m_isFrameSelected;
    bool m_fitOnNextResize = false;

    // Star/Constellation Overlay state
    bool m_showStars = false;
    bool m_showConstellations = false;
    std::vector<Star> m_stars;
};

} // namespace blastro
