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
#include <QTabWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QThread>
#include <atomic>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QProgressBar>
#include <QTextEdit>
#include <QPushButton>
#include <QMdiArea>
#include <QLabel>
#include <QScrollArea>
#include <QToolButton>
#include <QFrame>
#include "core/WorkspaceRegistry.h"
#include "core/ImageBatch.h"
#include "StatsPlotWidget.h"
#include <QJsonObject>

namespace blastro {

class PreprocessingWizardDialog : public QDialog {
    Q_OBJECT
public:
    PreprocessingWizardDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~PreprocessingWizardDialog() override = default;

    // Project / session persistence (Control tab settings only)
    QJsonObject serializeControlState() const;
    void restoreControlState(const QJsonObject& obj);

    // Called when CWD changes (project open/save) to refresh the resolved process dir label
    void updateProcessDirLabel();

private slots:
    void onAddFiles();
    void onAddDirectories();
    void onRemoveSelected();
    void onClearAll();
    void onRunCalibrationRegister();
    void onResumeStacking();
    void updatePreview();
    void onMetricChanged(int index);
    void onRangeChanged(double minVal, double maxVal);
    void onSpinBoxChanged();
    void onCancel();
    void onStepStarted(int stepIndex);
    void onStepProgressUpdated(int stepIndex, int percent, double elapsed);
    void onStepFinished(int stepIndex, bool success, double elapsed);
    void onFilterSelectionChanged(int index);
    void recomputeColumnWidths();
    void onFileDoubleClicked(QTableWidgetItem* item);
    void onTreeItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onSelectionCellDoubleClicked(int row, int column);
    void onSelectionTableItemChanged(QTableWidgetItem* item);

private:
    void addFilesList(const QStringList& filepaths);
    void logMessage(const QString& msg);
    QMdiArea* findMdiArea() const;
    int getTabIndex(const QString& label) const;
    void updateStepsPlan(const std::string& stage = "calibrate_register");
    void saveCurrentSelectionStates();
    void openFileAsImage(const QString& filepath);
    void notifyBatchViewsUpdated();

    WorkspaceRegistry& m_workspace;

    QTabWidget* m_tabs;

    // Files Tab
    QTableWidget* m_filesTable;
    QPushButton* m_addFilesBtn;
    QPushButton* m_addDirBtn;
    QPushButton* m_removeBtn;
    QPushButton* m_clearBtn;

    // Control Tab — Output Settings section
    QLabel* m_processDirLabel;  // Read-only; shows {CWD}/{processFolderName}
    QLineEdit* m_outPrefixEdit;
    QCheckBox* m_keepIntermediateChk;
    QCheckBox* m_overwriteMastersChk;
    QCheckBox* m_openCalibStacksChk;
    QCheckBox* m_openLightMastersChk;

    // Control Tab — Grouping section
    QCheckBox* m_strictDarkChk;
    QDoubleSpinBox* m_expToleranceSpin;
    QCheckBox* m_debayerChk;
    QComboBox* m_bayerPatternCombo;
    QComboBox* m_debayerMethodCombo;

    // Control Tab — Bias / Dark Stacking section
    QComboBox* m_biasDarkStackMethodCombo;
    QComboBox* m_biasDarkRejectionCombo;
    QDoubleSpinBox* m_biasDarkSigmaLowSpin;
    QDoubleSpinBox* m_biasDarkSigmaHighSpin;
    QCheckBox* m_biasDarkScaleAdditiveChk;
    QCheckBox* m_biasDarkScaleMultiplicativeChk;

    // Control Tab — Flat Stacking section
    QComboBox* m_flatStackMethodCombo;
    QComboBox* m_flatRejectionCombo;
    QDoubleSpinBox* m_flatSigmaLowSpin;
    QDoubleSpinBox* m_flatSigmaHighSpin;
    QCheckBox* m_flatScaleAdditiveChk;
    QCheckBox* m_flatScaleMultiplicativeChk;

    // Control Tab — Registration (Star Finding) section
    QDoubleSpinBox* m_starMinSnrSpin;
    QDoubleSpinBox* m_starMinFwhmSpin;
    QComboBox* m_transformationModelCombo;
    QComboBox* m_starDetectionMethodCombo;
    QSpinBox* m_starMaxStarsSpin;
    QDoubleSpinBox* m_starMatchToleranceSpin;
    QDoubleSpinBox* m_starMaxEccentricitySpin;

    // Control Tab — Alignment section
    QComboBox* m_alignRefModeCombo;
    QDoubleSpinBox* m_drizzleScaleSpin;
    QCheckBox* m_alignMutuallyChk;
    QComboBox* m_interpolationMethodCombo;

    // Control Tab — Background Normalization section
    QCheckBox* m_runBgeChk;
    QSpinBox* m_bgeOrderSpin;
    QDoubleSpinBox* m_bgeSigmaCutSpin;
    QDoubleSpinBox* m_bgeSampleFracSpin;
    QComboBox* m_bgeMethodCombo;

    // Control Tab — Light Stacking section
    QComboBox* m_lightStackMethodCombo;
    QComboBox* m_lightRejectionCombo;
    QDoubleSpinBox* m_lightSigmaLowSpin;
    QDoubleSpinBox* m_lightSigmaHighSpin;
    QComboBox* m_lightWeightCombo;
    QCheckBox* m_lightScaleAdditiveChk;
    QCheckBox* m_lightScaleMultiplicativeChk;

    // Groups & Process Tabs
    QTreeWidget* m_previewTree;
    QTreeWidget* m_processTree;
    QTextEdit* m_processFormulaText;

    // Frame Selection Tab
    QWidget* m_selectTab;
    StatsPlotWidget* m_plotWidget;
    QComboBox* m_metricCombo;
    QDoubleSpinBox* m_minSpin;
    QDoubleSpinBox* m_maxSpin;
    QTableWidget* m_selectionTable;
    QPushButton* m_resumeBtn;
    QComboBox* m_filterSelectCombo;

    // Progress Tab
    QProgressBar* m_progressBar;
    QPushButton* m_cancelBtn;
    QPushButton* m_runBtn;
    QPushButton* m_alignStackBtn;
    QTableWidget* m_stepsTable;

    QThread* m_activeThread = nullptr;
    QObject* m_activeWorker = nullptr;

    // Staged State
    struct FileEntry {
        QString filepath;
        double exposure = 0.0;
        QString filter = "None";
        QString imageType = "Light";
        int binningX = 1;
        int binningY = 1;
        int width = 0;
        int height = 0;
        double gain = 0.0;
        QString objectName;
    };
    std::vector<FileEntry> m_stagedFiles;
    std::string m_registeredLightsName;
    std::vector<QString> m_activeFilters;
    std::string m_runningStage;
    int m_stage1StepCount = 0;

    std::pair<double, double> getAbsoluteRangeForBatch(ImageBatchPtr batch, const std::string& metric);
    std::map<std::string, std::pair<double, double>> m_filterRanges;
protected:
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
};

class PreprocessingWorker : public QObject {
    Q_OBJECT
public:
    PreprocessingWorker(WorkspaceRegistry& workspace, const std::map<std::string, std::string>& config);
    ~PreprocessingWorker() override = default;

public slots:
    void run();
    void cancel();

signals:
    void progressUpdated(int val);
    void finished(bool success, const QString& errorMsg);
    void stepStarted(int stepIndex);
    void stepProgressUpdated(int stepIndex, int percent, double elapsed);
    void stepFinished(int stepIndex, bool success, double elapsed);

private:
    WorkspaceRegistry& m_workspace;
    std::map<std::string, std::string> m_config;
    std::atomic<bool> m_cancelRequested;
};

} // namespace blastro
