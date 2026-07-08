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

#include "BatchFilterDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

namespace blastro {

BatchFilterDialog::BatchFilterDialog(ImageBatchPtr batch, int currentFrameIdx, QWidget* parent)
    : QDialog(parent),
      m_batch(batch),
      m_currentFrameIdx(currentFrameIdx),
      m_plotWidget(new StatsPlotWidget(this)),
      m_metricCombo(new QComboBox(this)),
      m_minSpin(new QDoubleSpinBox(this)),
      m_maxSpin(new QDoubleSpinBox(this)),
      m_summaryLabel(new QLabel(this)) {

    setWindowTitle("Batch Statistics & Filtering");
    resize(700, 450);
    setStyleSheet("background-color: #2b2b2b; color: #ffffff; font-family: monospace; font-size: 11px;");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    // 1. Plot widget
    m_plotWidget->setHighlightIndex(m_currentFrameIdx);
    mainLayout->addWidget(m_plotWidget, 1); // Expand to fill

    // 2. Control Layout
    QWidget* controlWidget = new QWidget(this);
    controlWidget->setStyleSheet("background-color: #242424; border: 1px solid #3c3c3c; border-radius: 4px;");
    QHBoxLayout* controlLayout = new QHBoxLayout(controlWidget);
    controlLayout->setContentsMargins(10, 8, 10, 8);
    controlLayout->setSpacing(15);

    // Metric Selector
    m_metricCombo->addItem("Star Count", "starCount");
    m_metricCombo->addItem("FWHM", "fwhm");
    m_metricCombo->addItem("SNR", "snr");
    m_metricCombo->addItem("dx Shift", "dx");
    m_metricCombo->addItem("dy Shift", "dy");
    m_metricCombo->addItem("Rotation (degrees)", "theta");
    m_metricCombo->setStyleSheet(
        "QComboBox { background-color: #3a3a3a; color: #fff; border: 1px solid #555; padding: 3px 8px; border-radius: 3px; min-width: 150px; }"
        "QComboBox QAbstractItemView { background-color: #3a3a3a; color: #fff; selection-background-color: #007acc; }"
    );
    controlLayout->addWidget(new QLabel("Plot Metric:", this));
    controlLayout->addWidget(m_metricCombo);

    // Threshold SpinBoxes
    m_minSpin->setDecimals(3);
    m_minSpin->setSingleStep(0.1);
    m_minSpin->setStyleSheet("QDoubleSpinBox { background-color: #3a3a3a; color: #fff; border: 1px solid #555; padding: 2px 5px; border-radius: 3px; }");
    
    m_maxSpin->setDecimals(3);
    m_maxSpin->setSingleStep(0.1);
    m_maxSpin->setStyleSheet("QDoubleSpinBox { background-color: #3a3a3a; color: #fff; border: 1px solid #555; padding: 2px 5px; border-radius: 3px; }");

    controlLayout->addWidget(new QLabel("Min:", this));
    controlLayout->addWidget(m_minSpin);
    controlLayout->addWidget(new QLabel("Max:", this));
    controlLayout->addWidget(m_maxSpin);

    mainLayout->addWidget(controlWidget);

    // 3. Bottom controls
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(10);

    m_summaryLabel->setStyleSheet("font-weight: bold; font-size: 11px; color: #ccc;");
    bottomLayout->addWidget(m_summaryLabel, 1); // Stretches to fill left

    QString btnStyle = 
        "QPushButton { background-color: #3a3a3a; color: #fff; border: 1px solid #555; padding: 5px 12px; border-radius: 3px; font-weight: bold; }"
        "QPushButton:hover { background-color: #4a4a4a; }"
        "QPushButton:pressed { background-color: #007acc; }";

    QPushButton* allBtn = new QPushButton("Select All", this);
    QPushButton* noneBtn = new QPushButton("Deselect All", this);
    QPushButton* invertBtn = new QPushButton("Invert", this);
    QPushButton* closeBtn = new QPushButton("Close", this);

    allBtn->setStyleSheet(btnStyle);
    noneBtn->setStyleSheet(btnStyle);
    invertBtn->setStyleSheet(btnStyle);
    closeBtn->setStyleSheet("QPushButton { background-color: #007acc; color: #fff; border: 1px solid #005a9e; padding: 5px 15px; border-radius: 3px; font-weight: bold; }"
                           "QPushButton:hover { background-color: #008be5; }"
                           "QPushButton:pressed { background-color: #005a9e; }");

    bottomLayout->addWidget(allBtn);
    bottomLayout->addWidget(noneBtn);
    bottomLayout->addWidget(invertBtn);
    bottomLayout->addWidget(closeBtn);

    mainLayout->addLayout(bottomLayout);

    // Wire up events
    connect(m_metricCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, &BatchFilterDialog::onMetricChanged);
    connect(m_plotWidget, &StatsPlotWidget::rangeChanged, this, &BatchFilterDialog::onRangeChanged);
    connect(m_plotWidget, &StatsPlotWidget::pointClicked, this, [this](int index) {
        if (m_batch && index >= 0 && index < m_batch->count()) {
            FrameMetadata meta = m_batch->frameMetadata(index);
            if (!meta.registered) return;
            bool currentSelected = m_batch->isFrameSelected(index);
            m_batch->setFrameSelected(index, !currentSelected);
            m_plotWidget->update();
            updateLabels();
            emit selectionChanged();
        }
    });

    connect(m_minSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &BatchFilterDialog::onSpinBoxChanged);
    connect(m_maxSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &BatchFilterDialog::onSpinBoxChanged);

    connect(allBtn, &QPushButton::clicked, this, &BatchFilterDialog::selectAll);
    connect(noneBtn, &QPushButton::clicked, this, &BatchFilterDialog::deselectAll);
    connect(invertBtn, &QPushButton::clicked, this, &BatchFilterDialog::invertSelection);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    // Initial load
    resetFilters();
    onMetricChanged(0);
}

void BatchFilterDialog::onMetricChanged(int index) {
    Q_UNUSED(index);
    std::string metric = m_metricCombo->currentData().toString().toStdString();
    m_plotWidget->setBatch(m_batch, metric);

    updateSpinBoxRanges();

    if (m_filterRanges.find(metric) == m_filterRanges.end()) {
        m_filterRanges[metric] = { m_plotWidget->absoluteMin(), m_plotWidget->absoluteMax() };
    }
    auto range = m_filterRanges[metric];

    // Sync spinbox values to plot's initial filters
    m_minSpin->blockSignals(true);
    m_maxSpin->blockSignals(true);
    m_minSpin->setValue(range.first);
    m_maxSpin->setValue(range.second);
    m_minSpin->blockSignals(false);
    m_maxSpin->blockSignals(false);

    m_plotWidget->setFilterRange(range.first, range.second);
    updateLabels();
}

void BatchFilterDialog::updateSpinBoxRanges() {
    double absMin = m_plotWidget->absoluteMin();
    double absMax = m_plotWidget->absoluteMax();

    // Adjust step and decimals based on scale of the metric
    double diff = absMax - absMin;
    int decimals = 2;
    double step = 1.0;

    if (diff < 0.1) {
        decimals = 4;
        step = 0.0001;
    } else if (diff < 1.0) {
        decimals = 3;
        step = 0.001;
    } else if (diff < 10.0) {
        decimals = 2;
        step = 0.01;
    } else if (diff < 100.0) {
        decimals = 2;
        step = 0.1;
    }

    m_minSpin->blockSignals(true);
    m_maxSpin->blockSignals(true);
    
    m_minSpin->setDecimals(decimals);
    m_minSpin->setRange(absMin, absMax);
    m_minSpin->setSingleStep(step);

    m_maxSpin->setDecimals(decimals);
    m_maxSpin->setRange(absMin, absMax);
    m_maxSpin->setSingleStep(step);

    m_minSpin->blockSignals(false);
    m_maxSpin->blockSignals(false);
}

void BatchFilterDialog::onRangeChanged(double minVal, double maxVal) {
    m_minSpin->blockSignals(true);
    m_maxSpin->blockSignals(true);
    m_minSpin->setValue(minVal);
    m_maxSpin->setValue(maxVal);
    m_minSpin->blockSignals(false);
    m_maxSpin->blockSignals(false);

    applyFilter();
}

void BatchFilterDialog::onSpinBoxChanged() {
    double minVal = m_minSpin->value();
    double maxVal = m_maxSpin->value();

    if (minVal > maxVal) {
        minVal = maxVal;
        m_minSpin->blockSignals(true);
        m_minSpin->setValue(minVal);
        m_minSpin->blockSignals(false);
    }

    m_plotWidget->setFilterRange(minVal, maxVal);
    applyFilter();
}

void BatchFilterDialog::applyFilter() {
    if (!m_batch) return;

    double minVal = m_minSpin->value();
    double maxVal = m_maxSpin->value();

    std::string metric = m_metricCombo->currentData().toString().toStdString();
    m_filterRanges[metric] = {minVal, maxVal};

    int count = m_batch->count();

    for (int i = 0; i < count; ++i) {
        FrameMetadata meta = m_batch->frameMetadata(i);
        bool match = true;
        if (!meta.registered) {
            match = false;
        } else {
            for (const auto& pair : m_filterRanges) {
                std::string m = pair.first;
                double minR = pair.second.first;
                double maxR = pair.second.second;
                double v = 0.0;
                if (m == "starCount") v = meta.starCount;
                else if (m == "fwhm") v = meta.fwhm;
                else if (m == "snr") v = meta.snr;
                else if (m == "dx") v = meta.dx;
                else if (m == "dy") v = meta.dy;
                else if (m == "theta") v = meta.theta * 180.0 / M_PI;

                if (v < minR || v > maxR) {
                    match = false;
                    break;
                }
            }
        }
        m_batch->setFrameSelected(i, match);
    }

    m_plotWidget->update();
    updateLabels();
    emit selectionChanged();
}

void BatchFilterDialog::selectAll() {
    if (!m_batch) return;
    resetFilters();
    int count = m_batch->count();
    for (int i = 0; i < count; ++i) {
        FrameMetadata meta = m_batch->frameMetadata(i);
        m_batch->setFrameSelected(i, meta.registered);
    }
    std::string metric = m_metricCombo->currentData().toString().toStdString();
    auto range = m_filterRanges[metric];
    m_minSpin->blockSignals(true);
    m_maxSpin->blockSignals(true);
    m_minSpin->setValue(range.first);
    m_maxSpin->setValue(range.second);
    m_minSpin->blockSignals(false);
    m_maxSpin->blockSignals(false);
    m_plotWidget->setFilterRange(range.first, range.second);
    m_plotWidget->update();
    updateLabels();
    emit selectionChanged();
}

void BatchFilterDialog::deselectAll() {
    if (!m_batch) return;
    resetFilters();
    int count = m_batch->count();
    for (int i = 0; i < count; ++i) {
        m_batch->setFrameSelected(i, false);
    }
    std::string metric = m_metricCombo->currentData().toString().toStdString();
    auto range = m_filterRanges[metric];
    m_minSpin->blockSignals(true);
    m_maxSpin->blockSignals(true);
    m_minSpin->setValue(range.first);
    m_maxSpin->setValue(range.second);
    m_minSpin->blockSignals(false);
    m_maxSpin->blockSignals(false);
    m_plotWidget->setFilterRange(range.first, range.second);
    m_plotWidget->update();
    updateLabels();
    emit selectionChanged();
}

void BatchFilterDialog::invertSelection() {
    if (!m_batch) return;
    int count = m_batch->count();
    for (int i = 0; i < count; ++i) {
        FrameMetadata meta = m_batch->frameMetadata(i);
        if (!meta.registered) {
            m_batch->setFrameSelected(i, false);
        } else {
            bool current = m_batch->isFrameSelected(i);
            m_batch->setFrameSelected(i, !current);
        }
    }
    m_plotWidget->update();
    updateLabels();
    emit selectionChanged();
}

void BatchFilterDialog::updateLabels() {
    if (!m_batch) return;
    int count = m_batch->count();
    int selected = 0;
    for (int i = 0; i < count; ++i) {
        if (m_batch->isFrameSelected(i)) selected++;
    }
    m_summaryLabel->setText(QString("Batch Filter: %1 / %2 frames selected").arg(selected).arg(count));
}

void BatchFilterDialog::resetFilters() {
    m_filterRanges.clear();
    std::vector<std::string> metrics = {"starCount", "fwhm", "snr", "dx", "dy", "theta"};
    for (const auto& m : metrics) {
        m_filterRanges[m] = getAbsoluteRange(m);
    }
}

std::pair<double, double> BatchFilterDialog::getAbsoluteRange(const std::string& metric) {
    if (!m_batch || m_batch->count() == 0) return {0.0, 1.0};
    int count = m_batch->count();
    double minVal = std::numeric_limits<double>::max();
    double maxVal = -std::numeric_limits<double>::max();
    for (int i = 0; i < count; ++i) {
        FrameMetadata meta = m_batch->frameMetadata(i);
        double v = 0.0;
        if (metric == "starCount") v = meta.starCount;
        else if (metric == "fwhm") v = meta.fwhm;
        else if (metric == "snr") v = meta.snr;
        else if (metric == "dx") v = meta.dx;
        else if (metric == "dy") v = meta.dy;
        else if (metric == "theta") v = meta.theta * 180.0 / M_PI;

        if (v < minVal) minVal = v;
        if (v > maxVal) maxVal = v;
    }
    if (minVal == maxVal) {
        minVal -= 1.0;
        maxVal += 1.0;
    }
    return {minVal, maxVal};
}

} // namespace blastro
