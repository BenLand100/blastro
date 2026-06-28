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
#include "core/WorkspaceRegistry.h"
#include "core/ImageBatch.h"
#include "StatsPlotWidget.h"

namespace blastro {

class PreprocessingWizardDialog : public QDialog {
    Q_OBJECT
public:
    PreprocessingWizardDialog(WorkspaceRegistry& workspace, QWidget* parent = nullptr);
    ~PreprocessingWizardDialog() override = default;

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

private:
    void addFilesList(const QStringList& filepaths);
    void logMessage(const QString& msg);
    QMdiArea* findMdiArea() const;
    int getTabIndex(const QString& label) const;
    void updateStepsPlan(const std::string& stage = "calibrate_register");
    void saveCurrentSelectionStates();

    WorkspaceRegistry& m_workspace;

    QTabWidget* m_tabs;

    // Files Tab
    QTableWidget* m_filesTable;
    QPushButton* m_addFilesBtn;
    QPushButton* m_addDirBtn;
    QPushButton* m_removeBtn;
    QPushButton* m_clearBtn;

    // Control Tab
    QComboBox* m_modeCombo;
    QDoubleSpinBox* m_expToleranceSpin;
    QCheckBox* m_debayerChk;
    QComboBox* m_bayerPatternCombo;
    QComboBox* m_debayerMethodCombo;
    QDoubleSpinBox* m_starMinSnrSpin;
    QDoubleSpinBox* m_starMinFwhmSpin;

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

    // Output Tab
    QLineEdit* m_outDirEdit;
    QPushButton* m_outDirBrowseBtn;
    QCheckBox* m_keepIntermediateChk;
    QDoubleSpinBox* m_drizzleScaleSpin;
    QCheckBox* m_overwriteMastersChk;
    QLineEdit* m_outPrefixEdit;

    // Progress Tab
    QTextEdit* m_logText;
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
