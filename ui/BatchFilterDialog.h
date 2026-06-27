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
