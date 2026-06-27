#pragma once
#include "core/ImageBatch.h"
#include <QWidget>
#include <vector>
#include <string>

namespace blastro {

class StatsPlotWidget : public QWidget {
    Q_OBJECT
public:
    explicit StatsPlotWidget(QWidget* parent = nullptr);
    ~StatsPlotWidget() override = default;

    void setBatch(ImageBatchPtr batch, const std::string& metric);
    void setHighlightIndex(int index);
    void setFilterRange(double minVal, double maxVal);
    
    std::string currentMetric() const { return m_metric; }
    double minFilter() const { return m_minVal; }
    double maxFilter() const { return m_maxVal; }
    double absoluteMin() const { return m_rangeMin; }
    double absoluteMax() const { return m_rangeMax; }

signals:
    void pointClicked(int index);
    void rangeChanged(double minVal, double maxVal);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    double getValForFrame(int index) const;
    QPointF dataToPixel(double idx, double val) const;
    void pixelToData(const QPoint& pix, double& idx, double& val) const;
    void updateRanges();

    ImageBatchPtr m_batch;
    std::string m_metric;
    int m_highlightIndex;
    
    double m_minVal;
    double m_maxVal;
    double m_rangeMin;
    double m_rangeMax;

    // Interaction states
    enum DragMode {
        DragNone,
        DragMin,
        DragMax
    };
    DragMode m_dragMode;
};

} // namespace blastro
