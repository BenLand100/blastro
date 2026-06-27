#pragma once
#include <QWidget>
#include <vector>

namespace blastro {

class HistogramWidget : public QWidget {
    Q_OBJECT
public:
    explicit HistogramWidget(QWidget* parent = nullptr);
    ~HistogramWidget() override = default;

    void setHistogram(const std::vector<int>& hist);
    void setStretchParams(double b, double w, double m);
    
    void setGhsMode(bool ghs);
    void setGhsParams(double sp, double d);
    void setGhsProtections(double shadowProtect, double highlightProtect);
    
    void setDrawCurve(bool draw);
    
    void snapToBlackToMid();
    void resetZoom();
    
    bool isActive() const { return m_active; }
    void setActive(bool active);

signals:
    void stretchParamsChanged(double b, double w, double m);
    void ghsParamsChanged(double sp, double d);
    void ghsProtectionsChanged(double shadowProtect, double highlightProtect);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    enum DragTarget {
        None,
        Black,
        Mid,
        White,
        SymmetryPoint,
        ShadowProtect,
        HighlightProtect
    };

    double valueToX(double val) const;
    double xToValue(double x) const;
    DragTarget getCloseLine(const QPoint& pos) const;

    std::vector<int> m_histogram;
    
    // Mode
    bool m_ghsMode = false;
    bool m_drawCurve = true;

    // HT parameters
    double m_blackpoint;
    double m_whitepoint;
    double m_midpoint; // Relative to [B, W]

    // GHS parameters
    double m_spPoint = 0.5;
    double m_stretchFactor = 3.0;
    double m_shadowProtect = 0.0;
    double m_highlightProtect = 1.0;

    // Zoom & Scroll
    double m_zoom;
    double m_scrollOffset;
    bool m_active;

    DragTarget m_dragTarget;
    bool m_isScrolling;
    QPoint m_lastMousePos;
};

} // namespace blastro
