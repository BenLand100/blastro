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
    
    bool isActive() const { return m_active; }
    void setActive(bool active);

signals:
    void stretchParamsChanged(double b, double w, double m);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    enum DragTarget {
        None,
        Black,
        Mid,
        White
    };

    double valueToX(double val) const;
    double xToValue(double x) const;
    DragTarget getCloseLine(const QPoint& pos) const;

    std::vector<int> m_histogram;
    double m_blackpoint;
    double m_whitepoint;
    double m_midpoint; // Relative to [B, W]

    // Zoom & Scroll
    double m_zoom;
    double m_scrollOffset;
    bool m_active;

    DragTarget m_dragTarget;
    bool m_isScrolling;
    QPoint m_lastMousePos;
};

} // namespace blastro
