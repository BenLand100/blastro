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
#include <QDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include "core/ImageBatch.h"
#include "StatsPlotWidget.h"

namespace blastro {

class BatchFilterDialog : public QDialog {
    Q_OBJECT
public:
    BatchFilterDialog(ImageBatchPtr batch, int currentFrameIdx, QWidget* parent = nullptr);
    ~BatchFilterDialog() override = default;

signals:
    void selectionChanged();

private slots:
    void onMetricChanged(int index);
    void onRangeChanged(double minVal, double maxVal);
    void onSpinBoxChanged();
    void selectAll();
    void deselectAll();
    void invertSelection();

private:
    void applyFilter();
    void updateLabels();
    void updateSpinBoxRanges();

    ImageBatchPtr m_batch;
    int m_currentFrameIdx;

    StatsPlotWidget* m_plotWidget;
    QComboBox* m_metricCombo;
    QDoubleSpinBox* m_minSpin;
    QDoubleSpinBox* m_maxSpin;
    QLabel* m_summaryLabel;
};

} // namespace blastro
