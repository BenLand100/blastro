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
