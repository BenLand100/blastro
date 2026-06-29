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

#include "PreprocessingWizardDialog.h"
#include "MainWindow.h"
#include "algorithms/PreprocessingPipeline.h"
#include "core/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QMainWindow>
#include <QMdiArea>
#include <QHeaderView>
#include <QLabel>
#include <QDirIterator>
#include <QFileInfo>
#include <QListView>
#include <QTreeView>
#include <QSplitter>

namespace blastro {

PreprocessingWizardDialog::PreprocessingWizardDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : QDialog(parent),
      m_workspace(workspace),
      m_tabs(new QTabWidget(this)),
      m_filesTable(new QTableWidget(this)),
      m_addFilesBtn(new QPushButton("Add Files", this)),
      m_addDirBtn(new QPushButton("Add Directories", this)),
      m_removeBtn(new QPushButton("Remove Selected", this)),
      m_clearBtn(new QPushButton("Clear All", this)),
      m_modeCombo(new QComboBox(this)),
      m_expToleranceSpin(new QDoubleSpinBox(this)),
      m_debayerChk(new QCheckBox("Debayer Raw Lights", this)),
      m_bayerPatternCombo(new QComboBox(this)),
      m_debayerMethodCombo(new QComboBox(this)),
      m_starMinSnrSpin(new QDoubleSpinBox(this)),
      m_starMinFwhmSpin(new QDoubleSpinBox(this)),
      m_previewTree(new QTreeWidget(this)),
      m_processTree(new QTreeWidget(this)),
      m_processFormulaText(new QTextEdit(this)),
      m_plotWidget(new StatsPlotWidget(this)),
      m_metricCombo(new QComboBox(this)),
      m_minSpin(new QDoubleSpinBox(this)),
      m_maxSpin(new QDoubleSpinBox(this)),
      m_selectionTable(new QTableWidget(this)),
      m_resumeBtn(new QPushButton("Execute Align && Stack", this)),
      m_outDirEdit(new QLineEdit(this)),
      m_outDirBrowseBtn(new QPushButton("Browse...", this)),
      m_keepIntermediateChk(new QCheckBox("Keep intermediate files", this)),
      m_drizzleScaleSpin(new QDoubleSpinBox(this)),
      m_alignRefModeCombo(new QComboBox(this)),
      m_logText(new QTextEdit(this)),
      m_progressBar(new QProgressBar(this)),
      m_cancelBtn(new QPushButton("Cancel", this)),
      m_runBtn(new QPushButton("Execute Calibrate & Register", this)),
      m_alignStackBtn(new QPushButton("Execute Align && Stack", this)),
      m_stepsTable(new QTableWidget(this)),
      m_overwriteMastersChk(new QCheckBox("Overwrite existing master frames", this)),
      m_filterSelectCombo(new QComboBox(this)),
      m_outPrefixEdit(new QLineEdit(this)) {

    setObjectName("PreprocessingWizardDialog");
    setWindowTitle("Preprocessing Wizard (PPW)");
    resize(850, 600);
    setStyleSheet(
        "QDialog#PreprocessingWizardDialog { background-color: #202020; color: #ffffff; font-family: 'Segoe UI', Arial, sans-serif; font-size: 11px; }"
        "QTabWidget::panel { border: 1px solid #3c3c3c; background-color: #202020; }"
        "QTabBar::tab { background-color: #2d2d2d; color: #aaaaaa; border: 1px solid #3c3c3c; border-bottom: none; padding: 6px 12px; border-top-left-radius: 4px; border-top-right-radius: 4px; }"
        "QTabBar::tab:selected { background-color: #202020; color: #ffffff; font-weight: bold; border-bottom: 1px solid #202020; }"
        "QTabBar::tab:hover:!selected { background-color: #353535; color: #ffffff; }"
        "QLabel { background-color: transparent; color: #aaaaaa; }"
        "QLabel:disabled { color: #555555; }"
        "QLineEdit { background-color: #2d2d2d; color: #ffffff; border: 1px solid #444444; padding: 4px 8px; border-radius: 4px; }"
        "QLineEdit:disabled { background-color: #1a1a1a; color: #555555; border-color: #2d2d2d; }"
        "QComboBox { background-color: #2d2d2d; color: #ffffff; border: 1px solid #444444; padding: 4px 8px; border-radius: 4px; }"
        "QComboBox:disabled { background-color: #1a1a1a; color: #555555; border-color: #2d2d2d; }"
        "QComboBox QAbstractItemView { background-color: #2a2a2a; color: #ffffff; selection-background-color: #007acc; }"
        "QDoubleSpinBox { background-color: #2d2d2d; color: #ffffff; border: 1px solid #444444; padding: 4px 8px; border-radius: 4px; }"
        "QDoubleSpinBox:disabled { background-color: #1a1a1a; color: #555555; border-color: #2d2d2d; }"
        "QSpinBox { background-color: #2d2d2d; color: #ffffff; border: 1px solid #444444; padding: 4px 8px; border-radius: 4px; }"
        "QSpinBox:disabled { background-color: #1a1a1a; color: #555555; border-color: #2d2d2d; }"
        "QCheckBox { background-color: transparent; color: #ffffff; }"
        "QCheckBox:disabled { color: #555555; }"
        "QGroupBox { font-weight: bold; border: 1px solid #444444; margin-top: 10px; padding: 8px; color: #ffffff; border-radius: 4px; }"
        "QGroupBox:disabled { color: #555555; border-color: #2d2d2d; }"
        "QTableWidget, QTreeWidget, QTextEdit { background-color: #1a1a1a; color: #eeeeee; gridline-color: #2d2d2d; border: 1px solid #3c3c3c; border-radius: 4px; }"
        "QHeaderView::section { background-color: #2d2d2d; color: #aaaaaa; padding: 4px; border: 1px solid #3c3c3c; font-weight: bold; }"
        "QTableWidget::item:selected, QTreeWidget::item:selected { background-color: #007acc; color: #ffffff; }"
        "QPushButton { background-color: #2d2d2d; color: #ffffff; border: 1px solid #444444; padding: 6px 12px; border-radius: 4px; font-weight: bold; }"
        "QPushButton:hover { background-color: #353535; }"
        "QPushButton:pressed { background-color: #007acc; }"
        "QPushButton:disabled { background-color: #1a1a1a; color: #555555; border-color: #2d2d2d; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(10);

    mainLayout->addWidget(m_tabs, 1);

    // 1. Files Tab
    QWidget* filesTab = new QWidget(this);
    QVBoxLayout* filesLayout = new QVBoxLayout(filesTab);
    filesLayout->setContentsMargins(10, 10, 10, 10);
    filesLayout->setSpacing(8);

    m_filesTable->setColumnCount(8);
    m_filesTable->setHorizontalHeaderLabels({"File Path", "Type", "Filter", "Exposure (s)", "Binning", "Width", "Height", "Gain"});
    m_filesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_filesTable->horizontalHeader()->setStretchLastSection(false);
    m_filesTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_filesTable->setSortingEnabled(true);
    filesLayout->addWidget(m_filesTable, 1);

    QHBoxLayout* fileBtns = new QHBoxLayout();
    fileBtns->addWidget(m_addFilesBtn);
    fileBtns->addWidget(m_addDirBtn);
    fileBtns->addWidget(m_removeBtn);
    fileBtns->addWidget(m_clearBtn);
    filesLayout->addLayout(fileBtns);
    m_tabs->addTab(filesTab, "Files");

    // 2. Control Tab
    QWidget* controlTab = new QWidget(this);
    QFormLayout* controlLayout = new QFormLayout(controlTab);
    controlLayout->setContentsMargins(15, 15, 15, 15);
    controlLayout->setSpacing(12);

    m_modeCombo->addItem("Dark Current Mode", "dark_current");
    m_modeCombo->addItem("Zero Dark Current Mode", "zero_dark_current");
    controlLayout->addRow("Preprocessing Mode:", m_modeCombo);

    m_expToleranceSpin->setRange(0.1, 10.0);
    m_expToleranceSpin->setValue(0.5);
    m_expToleranceSpin->setSingleStep(0.1);
    m_expToleranceSpin->setSuffix(" s");
    controlLayout->addRow("Exposure Matching Tolerance:", m_expToleranceSpin);

    controlLayout->addRow("", m_debayerChk);

    m_bayerPatternCombo->addItems({"RGGB", "BGGR", "GRBG", "GBRG"});
    controlLayout->addRow("OSC Bayer Pattern:", m_bayerPatternCombo);

    m_debayerMethodCombo->addItems({"bilinear", "vng"});
    controlLayout->addRow("Debayer Method:", m_debayerMethodCombo);

    m_starMinSnrSpin->setRange(1.0, 100.0);
    m_starMinSnrSpin->setValue(4.0);
    m_starMinSnrSpin->setSingleStep(0.5);
    controlLayout->addRow("Star Detection Min SNR:", m_starMinSnrSpin);

    m_starMinFwhmSpin->setRange(0.5, 10.0);
    m_starMinFwhmSpin->setValue(1.5);
    m_starMinFwhmSpin->setSingleStep(0.1);
    controlLayout->addRow("Star Detection Min FWHM:", m_starMinFwhmSpin);

    // Output Settings Group Box placed in Control Tab
    QGroupBox* outGroup = new QGroupBox("Output Settings", controlTab);
    QFormLayout* outGroupLayout = new QFormLayout(outGroup);
    outGroupLayout->setContentsMargins(10, 10, 10, 10);
    outGroupLayout->setSpacing(8);

    outGroupLayout->addRow("Output Prefix:", m_outPrefixEdit);

    QHBoxLayout* outDirLayout = new QHBoxLayout();
    outDirLayout->addWidget(m_outDirEdit, 1);
    outDirLayout->addWidget(m_outDirBrowseBtn);
    outGroupLayout->addRow("Output Directory:", outDirLayout);

    m_drizzleScaleSpin->setRange(1.0, 3.0);
    m_drizzleScaleSpin->setValue(1.0);
    m_drizzleScaleSpin->setSingleStep(0.5);
    outGroupLayout->addRow("Drizzle Alignment Scale:", m_drizzleScaleSpin);

    m_alignRefModeCombo->addItem("Find Centermost", "average_center");
    m_alignRefModeCombo->addItem("Use Reference", "registration");
    outGroupLayout->addRow("Alignment Reference Mode:", m_alignRefModeCombo);

    m_keepIntermediateChk->setChecked(false);
    outGroupLayout->addRow("", m_keepIntermediateChk);

    m_overwriteMastersChk->setChecked(false);
    outGroupLayout->addRow("", m_overwriteMastersChk);

    controlLayout->addRow(outGroup);

    m_tabs->addTab(controlTab, "Control");

    // 3. Groups Tab
    QWidget* groupsTab = new QWidget(this);
    QVBoxLayout* groupsLayout = new QVBoxLayout(groupsTab);
    m_previewTree->setColumnCount(6);
    m_previewTree->setHeaderLabels({"Grouping Structure", "Count", "Filter", "Exposure", "Gain", "Binning"});
    m_previewTree->header()->setSectionResizeMode(QHeaderView::Interactive);
    m_previewTree->header()->setStretchLastSection(false);
    m_previewTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_previewTree->setSortingEnabled(true);
    groupsLayout->addWidget(m_previewTree);
    m_tabs->addTab(groupsTab, "Groups");

    // 3b. Process Tab
    QWidget* processTab = new QWidget(this);
    QVBoxLayout* processLayout = new QVBoxLayout(processTab);
    m_processTree->setColumnCount(6);
    m_processTree->setHeaderLabels({"Process Flow", "Count", "Filter", "Exposure", "Gain", "Binning"});
    m_processTree->header()->setSectionResizeMode(QHeaderView::Interactive);
    m_processTree->header()->setStretchLastSection(false);
    m_processTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_processTree->setSortingEnabled(true);
    processLayout->addWidget(m_processTree, 1);

    m_processFormulaText->setReadOnly(true);
    m_processFormulaText->setFixedHeight(120);
    processLayout->addWidget(new QLabel("Staged Calibration Equations:", this));
    processLayout->addWidget(m_processFormulaText);
    m_tabs->addTab(processTab, "Process");

    // 4. Frame Selection Tab
    m_selectTab = new QWidget();
    QVBoxLayout* selectLayout = new QVBoxLayout(m_selectTab);
    selectLayout->setContentsMargins(10, 10, 10, 10);
    selectLayout->setSpacing(8);

    selectLayout->addWidget(m_plotWidget, 1);

    QHBoxLayout* selectCtrlLayout = new QHBoxLayout();
    selectCtrlLayout->addWidget(new QLabel("Light Group Filter:", this));
    selectCtrlLayout->addWidget(m_filterSelectCombo);

    m_metricCombo->addItem("Star Count", "starCount");
    m_metricCombo->addItem("FWHM", "fwhm");
    m_metricCombo->addItem("SNR", "snr");
    selectCtrlLayout->addWidget(new QLabel("Plot Metric:", this));
    selectCtrlLayout->addWidget(m_metricCombo);

    m_minSpin->setRange(-1000.0, 10000.0);
    m_maxSpin->setRange(-1000.0, 10000.0);
    selectCtrlLayout->addWidget(new QLabel("Min:", this));
    selectCtrlLayout->addWidget(m_minSpin);
    selectCtrlLayout->addWidget(new QLabel("Max:", this));
    selectCtrlLayout->addWidget(m_maxSpin);
    selectLayout->addLayout(selectCtrlLayout);

    m_selectionTable->setColumnCount(5);
    m_selectionTable->setHorizontalHeaderLabels({"Use", "Frame Name", "Star Count", "FWHM", "SNR"});
    m_selectionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    selectLayout->addWidget(m_selectionTable, 1);

    m_resumeBtn->setEnabled(false);
    selectLayout->addWidget(m_resumeBtn);

    // 6. Execute Tab
    QWidget* progressTab = new QWidget(this);
    QVBoxLayout* progressLayout = new QVBoxLayout(progressTab);

    QSplitter* splitter = new QSplitter(Qt::Vertical, progressTab);

    m_stepsTable->setColumnCount(5);
    m_stepsTable->setHorizontalHeaderLabels({"Planned Step", "Stage", "Status", "Progress", "Elapsed"});
    m_stepsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_stepsTable->horizontalHeader()->setStretchLastSection(true);
    m_stepsTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_stepsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    splitter->addWidget(m_stepsTable);

    m_logText->setReadOnly(true);
    splitter->addWidget(m_logText);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    progressLayout->addWidget(splitter, 1);

    progressLayout->addWidget(m_progressBar);

    QHBoxLayout* progressBtns = new QHBoxLayout();
    progressBtns->addWidget(m_runBtn);
    m_alignStackBtn->setEnabled(false);
    progressBtns->addWidget(m_alignStackBtn);
    m_cancelBtn->setEnabled(false);
    progressBtns->addWidget(m_cancelBtn);
    progressLayout->addLayout(progressBtns);
    m_tabs->addTab(progressTab, "Execute");

    // Connects
    connect(m_addFilesBtn, &QPushButton::clicked, this, &PreprocessingWizardDialog::onAddFiles);
    connect(m_addDirBtn, &QPushButton::clicked, this, &PreprocessingWizardDialog::onAddDirectories);
    connect(m_removeBtn, &QPushButton::clicked, this, &PreprocessingWizardDialog::onRemoveSelected);
    connect(m_clearBtn, &QPushButton::clicked, this, &PreprocessingWizardDialog::onClearAll);
    connect(m_runBtn, &QPushButton::clicked, this, &PreprocessingWizardDialog::onRunCalibrationRegister);
    connect(m_alignStackBtn, &QPushButton::clicked, this, &PreprocessingWizardDialog::onResumeStacking);
    connect(m_resumeBtn, &QPushButton::clicked, this, &PreprocessingWizardDialog::onResumeStacking);
    connect(m_cancelBtn, &QPushButton::clicked, this, &PreprocessingWizardDialog::onCancel);
    connect(m_outDirBrowseBtn, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Output Directory", m_outDirEdit->text());
        if (!dir.isEmpty()) {
            m_outDirEdit->setText(dir);
        }
    });

    connect(m_metricCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, &PreprocessingWizardDialog::onMetricChanged);
    connect(m_minSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &PreprocessingWizardDialog::onSpinBoxChanged);
    connect(m_maxSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &PreprocessingWizardDialog::onSpinBoxChanged);
    connect(m_outPrefixEdit, &QLineEdit::textChanged, this, &PreprocessingWizardDialog::updatePreview);
    connect(m_modeCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, &PreprocessingWizardDialog::updatePreview);
    connect(m_expToleranceSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &PreprocessingWizardDialog::updatePreview);
    connect(m_debayerChk, &QCheckBox::checkStateChanged, this, &PreprocessingWizardDialog::updatePreview);
    connect(m_plotWidget, &StatsPlotWidget::rangeChanged, this, [this](double minVal, double maxVal) {
        m_minSpin->blockSignals(true);
        m_maxSpin->blockSignals(true);
        m_minSpin->setValue(minVal);
        m_maxSpin->setValue(maxVal);
        m_minSpin->blockSignals(false);
        m_maxSpin->blockSignals(false);
        onRangeChanged(minVal, maxVal);
    });
    connect(m_filterSelectCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, &PreprocessingWizardDialog::onFilterSelectionChanged);

    // Default output directory to current path
    m_outDirEdit->setText(QDir::currentPath());
}

void PreprocessingWizardDialog::onAddFiles() {
    QStringList files = QFileDialog::getOpenFileNames(this, "Select FITS files to preprocess", "", "FITS Images (*.fits *.fit)");
    if (!files.isEmpty()) {
        addFilesList(files);
    }
}

void PreprocessingWizardDialog::onAddDirectories() {
    QFileDialog dialog(this, "Select Directories to Import recursively");
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);

    // Enable multiple selection in native-looking view lists
    QListView* listView = dialog.findChild<QListView*>("listView");
    if (listView) {
        listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
    QTreeView* treeView = dialog.findChild<QTreeView*>("treeView");
    if (treeView) {
        treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }

    if (dialog.exec() != QDialog::Accepted) return;

    QStringList dirs = dialog.selectedFiles();
    QStringList filepaths;
    for (const auto& dir : dirs) {
        QDirIterator it(dir, {"*.fits", "*.fit"}, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            filepaths << it.next();
        }
    }
    if (!filepaths.isEmpty()) {
        addFilesList(filepaths);
    }
}

void PreprocessingWizardDialog::addFilesList(const QStringList& filepaths) {
    for (const auto& path : filepaths) {
        FitsHeaderInfo info;
        if (FitsIO::readHeaderInfo(path.toStdString(), info)) {
            FileEntry entry;
            entry.filepath = path;
            entry.exposure = info.exposureTime;
            entry.filter = QString::fromStdString(info.filter);
            entry.imageType = QString::fromStdString(info.imageType);
            entry.binningX = info.binningX;
            entry.binningY = info.binningY;
            entry.width = info.width;
            entry.height = info.height;
            entry.gain = info.gain;
            entry.objectName = QString::fromStdString(info.objectName);
            m_stagedFiles.push_back(entry);
        }
    }

    // Find common base path for staged files
    QString commonBase;
    if (!m_stagedFiles.empty()) {
        commonBase = QFileInfo(m_stagedFiles[0].filepath).absolutePath() + "/";
        for (size_t i = 1; i < m_stagedFiles.size(); ++i) {
            QString path = QFileInfo(m_stagedFiles[i].filepath).absolutePath() + "/";
            int len = std::min(commonBase.length(), path.length());
            int lastSlash = -1;
            for (int j = 0; j < len; ++j) {
                if (commonBase[j] != path[j]) {
                    break;
                }
                if (commonBase[j] == '/' || commonBase[j] == '\\') {
                    lastSlash = j;
                }
            }
            if (lastSlash == -1) {
                commonBase = "";
                break;
            }
            commonBase = commonBase.left(lastSlash + 1);
        }
    }

    // Refresh Files table
    m_filesTable->setSortingEnabled(false);
    m_filesTable->setRowCount(0);
    for (int i = 0; i < static_cast<int>(m_stagedFiles.size()); ++i) {
        const auto& f = m_stagedFiles[i];
        m_filesTable->insertRow(i);

        QString relPath = f.filepath;
        if (!commonBase.isEmpty() && relPath.startsWith(commonBase)) {
            relPath = relPath.mid(commonBase.length());
        }

        m_filesTable->setItem(i, 0, new QTableWidgetItem(relPath));
        m_filesTable->item(i, 0)->setToolTip(f.filepath);

        m_filesTable->setItem(i, 1, new QTableWidgetItem(f.imageType));
        m_filesTable->setItem(i, 2, new QTableWidgetItem(f.filter));
        m_filesTable->setItem(i, 3, new QTableWidgetItem(QString::number(f.exposure)));
        m_filesTable->setItem(i, 4, new QTableWidgetItem(QString("%1x%2").arg(f.binningX).arg(f.binningY)));
        m_filesTable->setItem(i, 5, new QTableWidgetItem(QString::number(f.width)));
        m_filesTable->setItem(i, 6, new QTableWidgetItem(QString::number(f.height)));
        m_filesTable->setItem(i, 7, new QTableWidgetItem(QString::number(f.gain)));
    }
    m_filesTable->setSortingEnabled(true);
    m_filesTable->resizeColumnsToContents();

    updatePreview();
}

void PreprocessingWizardDialog::onRemoveSelected() {
    auto selectedRanges = m_filesTable->selectedRanges();
    std::vector<int> rowsToRemove;
    for (const auto& range : selectedRanges) {
        for (int r = range.topRow(); r <= range.bottomRow(); ++r) {
            rowsToRemove.push_back(r);
        }
    }
    std::sort(rowsToRemove.rbegin(), rowsToRemove.rend());
    for (int r : rowsToRemove) {
        m_stagedFiles.erase(m_stagedFiles.begin() + r);
        m_filesTable->removeRow(r);
    }
    updatePreview();
}

void PreprocessingWizardDialog::onClearAll() {
    m_stagedFiles.clear();
    m_filesTable->setRowCount(0);
    updatePreview();
}

void PreprocessingWizardDialog::updatePreview() {
    m_previewTree->setSortingEnabled(false);
    m_processTree->setSortingEnabled(false);
    m_previewTree->clear();
    m_processTree->clear();
    m_processFormulaText->clear();

    if (m_stagedFiles.empty()) {
        m_outPrefixEdit->clear();
        m_outDirEdit->setText(QDir::currentPath());
        return;
    }

    // Determine default prefix if not set
    if (m_outPrefixEdit->text().isEmpty() || m_outPrefixEdit->text() == "master") {
        std::map<QString, int> objectCounts;
        for (const auto& f : m_stagedFiles) {
            if (f.imageType == "Light" && !f.objectName.isEmpty()) {
                objectCounts[f.objectName]++;
            }
        }
        
        QString majorityObject = "";
        int maxCount = 0;
        for (const auto& pair : objectCounts) {
            if (pair.second > maxCount) {
                maxCount = pair.second;
                majorityObject = pair.first;
            }
        }
        
        QString prefix = majorityObject;
        
        if (prefix.isEmpty()) {
            QStringList basenames;
            for (const auto& f : m_stagedFiles) {
                if (f.imageType == "Light") {
                    basenames << QFileInfo(f.filepath).baseName();
                }
            }
            if (basenames.isEmpty()) {
                for (const auto& f : m_stagedFiles) {
                    basenames << QFileInfo(f.filepath).baseName();
                }
            }
            
            if (!basenames.isEmpty()) {
                QString first = basenames[0];
                int len = first.length();
                bool found = false;
                for (int subLen = len; subLen > 0; --subLen) {
                    for (int start = 0; start <= len - subLen; ++start) {
                        QString sub = first.mid(start, subLen);
                        bool allContain = true;
                        for (int i = 1; i < basenames.size(); ++i) {
                            if (!basenames[i].contains(sub)) {
                                allContain = false;
                                break;
                            }
                        }
                        if (allContain) {
                            prefix = sub;
                            found = true;
                            break;
                        }
                    }
                    if (found) break;
                }
            }
        }
        
        prefix = prefix.trimmed();
        while (prefix.endsWith('_') || prefix.endsWith('-') || prefix.endsWith(' ')) {
            prefix.chop(1);
        }
        
        if (prefix.isEmpty()) {
            prefix = "master";
        }
        
        m_outPrefixEdit->blockSignals(true);
        m_outPrefixEdit->setText(prefix);
        m_outPrefixEdit->blockSignals(false);
    }

    if (m_outDirEdit->text().isEmpty() || m_outDirEdit->text() == QDir::currentPath() || m_outDirEdit->text().endsWith("/master")) {
        m_outDirEdit->blockSignals(true);
        m_outDirEdit->setText(QDir::currentPath() + "/" + m_outPrefixEdit->text());
        m_outDirEdit->blockSignals(false);
    }

    // Find common base path
    QString commonBase = QFileInfo(m_stagedFiles[0].filepath).absolutePath() + "/";
    for (size_t i = 1; i < m_stagedFiles.size(); ++i) {
        QString path = QFileInfo(m_stagedFiles[i].filepath).absolutePath() + "/";
        int len = std::min(commonBase.length(), path.length());
        int lastSlash = -1;
        for (int j = 0; j < len; ++j) {
            if (commonBase[j] != path[j]) {
                break;
            }
            if (commonBase[j] == '/' || commonBase[j] == '\\') {
                lastSlash = j;
            }
        }
        if (lastSlash == -1) {
            commonBase = "";
            break;
        }
        commonBase = commonBase.left(lastSlash + 1);
    }

    std::vector<std::string> bias, dark, flat, light;
    for (const auto& f : m_stagedFiles) {
        if (f.imageType == "Bias") bias.push_back(f.filepath.toStdString());
        else if (f.imageType == "Dark") dark.push_back(f.filepath.toStdString());
        else if (f.imageType == "Flat") flat.push_back(f.filepath.toStdString());
        else if (f.imageType == "Light") light.push_back(f.filepath.toStdString());
    }

    std::vector<PreprocessingGroup> groups = PreprocessingPipeline::groupFiles(
        bias, dark, flat, light, m_expToleranceSpin->value()
    );

    // Lambda helper to find matching calibration groups
    auto findGroup = [&](const std::string& type, int binX, int binY, int w, int h, double gain, const std::string& filter, double exp = -1.0) -> const PreprocessingGroup* {
        for (const auto& g : groups) {
            if (g.type != type) continue;
            if (g.binningX != binX || g.binningY != binY || g.width != w || g.height != h) continue;
            if (static_cast<int>(g.gain) != static_cast<int>(gain)) continue;
            if (!filter.empty() && g.filter != filter) continue;
            if (exp >= 0.0 && std::abs(g.exposure - exp) > m_expToleranceSpin->value()) continue;
            return &g;
        }
        return nullptr;
    };

    // Lambda helper to format and add the Input Frames node
    auto addInputFramesNode = [&](QTreeWidgetItem* parent, const std::vector<std::string>& filepaths) {
        QTreeWidgetItem* framesNode = new QTreeWidgetItem(parent);
        framesNode->setText(0, "Input Frames");
        for (const auto& f : filepaths) {
            QTreeWidgetItem* child = new QTreeWidgetItem(framesNode);
            QString relPath = QString::fromStdString(f);
            if (!commonBase.isEmpty() && relPath.startsWith(commonBase)) {
                relPath = relPath.mid(commonBase.length());
            }
            child->setText(0, relPath);
        }
    };

    // 1. Populate Groups Tree (Flat preview of groups)
    for (const auto& g : groups) {
        QTreeWidgetItem* root = new QTreeWidgetItem(m_previewTree);
        QString symbol = "📄";
        if (g.type == "Light") symbol = "◯";
        else if (g.type == "Flat") symbol = "▣";
        else if (g.type == "Dark") symbol = "⧇";
        else if (g.type == "Bias") symbol = "⧆";

        root->setText(0, QString("%1 %2 Group").arg(symbol).arg(QString::fromStdString(g.type)));
        root->setText(1, QString::number(g.filepaths.size()));
        root->setText(2, (g.type == "Light" || g.type == "Flat") ? QString::fromStdString(g.filter) : "");
        root->setText(3, QString("%1s").arg(g.exposure));
        root->setText(4, QString::number(g.gain));
        root->setText(5, QString("%1x%2").arg(g.binningX).arg(g.binningY));

        for (const auto& f : g.filepaths) {
            QTreeWidgetItem* child = new QTreeWidgetItem(root);
            QString relPath = QString::fromStdString(f);
            if (!commonBase.isEmpty() && relPath.startsWith(commonBase)) {
                relPath = relPath.mid(commonBase.length());
            }
            child->setText(0, relPath);
        }
    }
    m_previewTree->collapseAll();

    // 2. Populate Process Tree (Dependency flow) and collect equations
    QStringList formulas;
    for (const auto& lg : groups) {
        if (lg.type != "Light") continue;

        QTreeWidgetItem* lightNode = new QTreeWidgetItem(m_processTree);
        lightNode->setText(0, "◯ Light Group");
        lightNode->setText(1, QString::number(lg.filepaths.size()));
        lightNode->setText(2, QString::fromStdString(lg.filter));
        lightNode->setText(3, QString("%1s").arg(lg.exposure));
        lightNode->setText(4, QString::number(lg.gain));
        lightNode->setText(5, QString("%1x%2").arg(lg.binningX).arg(lg.binningY));

        // Check matching Calibration masters
        const PreprocessingGroup* darkMatch = nullptr;
        if (m_modeCombo->currentData().toString() == "dark_current") {
            darkMatch = findGroup("Dark", lg.binningX, lg.binningY, lg.width, lg.height, lg.gain, "", lg.exposure);
        }
        const PreprocessingGroup* biasMatch = findGroup("Bias", lg.binningX, lg.binningY, lg.width, lg.height, lg.gain, "");

        QString equation;
        if (darkMatch) {
            equation = "Calibrated_Light = (Light - Master_Dark) / Master_Flat";
        } else if (biasMatch) {
            equation = "Calibrated_Light = (Light - Master_Bias) / Master_Flat";
        } else {
            equation = "Calibrated_Light = Light / Master_Flat";
        }
        
        formulas.append(QString("Light Group (%1, %2s, bin %3x%4, gain %5):\n  %6")
            .arg(QString::fromStdString(lg.filter))
            .arg(lg.exposure)
            .arg(lg.binningX).arg(lg.binningY)
            .arg(lg.gain)
            .arg(equation));

        // Input frames for Light group
        addInputFramesNode(lightNode, lg.filepaths);

        // Nested dark or bias
        if (darkMatch) {
            QTreeWidgetItem* darkNode = new QTreeWidgetItem(lightNode);
            darkNode->setText(0, "⧇ Master Dark");
            darkNode->setText(1, QString::number(darkMatch->filepaths.size()));
            darkNode->setText(2, "");
            darkNode->setText(3, QString("%1s").arg(darkMatch->exposure));
            darkNode->setText(4, QString::number(darkMatch->gain));
            darkNode->setText(5, QString("%1x%2").arg(darkMatch->binningX).arg(darkMatch->binningY));

            addInputFramesNode(darkNode, darkMatch->filepaths);
        } else if (biasMatch) {
            QTreeWidgetItem* biasNode = new QTreeWidgetItem(lightNode);
            biasNode->setText(0, "⧆ Master Bias");
            biasNode->setText(1, QString::number(biasMatch->filepaths.size()));
            biasNode->setText(2, "");
            biasNode->setText(3, QString("%1s").arg(biasMatch->exposure));
            biasNode->setText(4, QString::number(biasMatch->gain));
            biasNode->setText(5, QString("%1x%2").arg(biasMatch->binningX).arg(biasMatch->binningY));

            addInputFramesNode(biasNode, biasMatch->filepaths);
        }

        // Nested Master Flat
        const PreprocessingGroup* flatMatch = findGroup("Flat", lg.binningX, lg.binningY, lg.width, lg.height, lg.gain, lg.filter);
        if (flatMatch) {
            QTreeWidgetItem* flatNode = new QTreeWidgetItem(lightNode);
            flatNode->setText(0, "▣ Master Flat");
            flatNode->setText(1, QString::number(flatMatch->filepaths.size()));
            flatNode->setText(2, QString::fromStdString(flatMatch->filter));
            flatNode->setText(3, QString("%1s").arg(flatMatch->exposure));
            flatNode->setText(4, QString::number(flatMatch->gain));
            flatNode->setText(5, QString("%1x%2").arg(flatMatch->binningX).arg(flatMatch->binningY));

            const PreprocessingGroup* flatDarkMatch = nullptr;
            if (m_modeCombo->currentData().toString() == "dark_current") {
                flatDarkMatch = findGroup("Dark", flatMatch->binningX, flatMatch->binningY, flatMatch->width, flatMatch->height, flatMatch->gain, "", flatMatch->exposure);
            }
            const PreprocessingGroup* flatBiasMatch = findGroup("Bias", flatMatch->binningX, flatMatch->binningY, flatMatch->width, flatMatch->height, flatMatch->gain, "");

            QString flatEq;
            if (flatDarkMatch) {
                flatEq = "Calibrated_Flat = Flat - Master_Flat_Dark";
            } else if (flatBiasMatch) {
                flatEq = "Calibrated_Flat = Flat - Master_Bias";
            } else {
                flatEq = "Calibrated_Flat = Flat";
            }
            
            formulas.append(QString("Flat Group (%1, bin %2x%3, gain %4):\n  %5")
                .arg(QString::fromStdString(flatMatch->filter))
                .arg(flatMatch->binningX).arg(flatMatch->binningY)
                .arg(flatMatch->gain)
                .arg(flatEq));

            addInputFramesNode(flatNode, flatMatch->filepaths);

            // Flat dependencies
            if (flatDarkMatch) {
                QTreeWidgetItem* fdNode = new QTreeWidgetItem(flatNode);
                fdNode->setText(0, "⧇ Master Flat-Dark");
                fdNode->setText(1, QString::number(flatDarkMatch->filepaths.size()));
                fdNode->setText(2, "");
                fdNode->setText(3, QString("%1s").arg(flatDarkMatch->exposure));
                fdNode->setText(4, QString::number(flatDarkMatch->gain));
                fdNode->setText(5, QString("%1x%2").arg(flatDarkMatch->binningX).arg(flatDarkMatch->binningY));

                addInputFramesNode(fdNode, flatDarkMatch->filepaths);
            } else if (flatBiasMatch) {
                QTreeWidgetItem* fbNode = new QTreeWidgetItem(flatNode);
                fbNode->setText(0, "⧆ Master Bias");
                fbNode->setText(1, QString::number(flatBiasMatch->filepaths.size()));
                fbNode->setText(2, "");
                fbNode->setText(3, QString("%1s").arg(flatBiasMatch->exposure));
                fbNode->setText(4, QString::number(flatBiasMatch->gain));
                fbNode->setText(5, QString("%1x%2").arg(flatBiasMatch->binningX).arg(flatBiasMatch->binningY));

                addInputFramesNode(fbNode, flatBiasMatch->filepaths);
            }
        }
    }
    m_processFormulaText->setText(formulas.join("\n\n"));
    m_previewTree->collapseAll();
    m_processTree->collapseAll();
    m_previewTree->setSortingEnabled(true);
    m_processTree->setSortingEnabled(true);

    // Auto-fit starting sizes to contents, keep interactive, enable horizontal scrolling
    for (int i = 0; i < 6; ++i) {
        m_previewTree->resizeColumnToContents(i);
        m_processTree->resizeColumnToContents(i);
    }
    updateStepsPlan("calibrate_register");
}

void PreprocessingWizardDialog::logMessage(const QString& msg) {
    m_logText->append(msg);
}

void PreprocessingWizardDialog::onRunCalibrationRegister() {
    if (m_stagedFiles.empty()) {
        QMessageBox::warning(this, "PPW", "No files have been added to the pipeline.");
        return;
    }

    int oldSelIndex = getTabIndex("Frame Selection");
    if (oldSelIndex != -1) {
        m_tabs->removeTab(oldSelIndex);
    }

    m_tabs->setCurrentIndex(getTabIndex("Execute"));
    m_progressBar->setValue(0);
    m_logText->clear();
    logMessage("[PPW] Starting calibration and registration pipeline...");

    std::vector<std::string> bias, dark, flat, light;
    for (const auto& f : m_stagedFiles) {
        if (f.imageType == "Bias") bias.push_back(f.filepath.toStdString());
        else if (f.imageType == "Dark") dark.push_back(f.filepath.toStdString());
        else if (f.imageType == "Flat") flat.push_back(f.filepath.toStdString());
        else if (f.imageType == "Light") light.push_back(f.filepath.toStdString());
    }

    auto groups = PreprocessingPipeline::groupFiles(bias, dark, flat, light, m_expToleranceSpin->value());
    std::string firstFilter = "None";
    for (const auto& g : groups) {
        if (g.type == "Light") {
            firstFilter = g.filter;
            break;
        }
    }

    // Build configuration
    std::map<std::string, std::string> config;
    config["stage"] = "calibrate_register";
    config["mode"] = m_modeCombo->currentData().toString().toStdString();
    config["exp_tolerance"] = QString::number(m_expToleranceSpin->value()).toStdString();
    config["debayer"] = m_debayerChk->isChecked() ? "true" : "false";
    config["bayer_pattern"] = m_bayerPatternCombo->currentText().toStdString();
    config["debayer_method"] = m_debayerMethodCombo->currentText().toStdString();
    config["out_dir"] = m_outDirEdit->text().toStdString();
    config["keep_intermediate"] = m_keepIntermediateChk->isChecked() ? "true" : "false";
    config["star_min_snr"] = QString::number(m_starMinSnrSpin->value()).toStdString();
    config["star_min_fwhm"] = QString::number(m_starMinFwhmSpin->value()).toStdString();
    config["out_prefix"] = m_outPrefixEdit->text().toStdString();

    auto joinPaths = [](const std::vector<std::string>& paths) -> std::string {
        std::string res;
        for (size_t i = 0; i < paths.size(); ++i) {
            res += paths[i];
            if (i + 1 < paths.size()) res += ";";
        }
        return res;
    };

    config["bias_files"] = joinPaths(bias);
    config["dark_files"] = joinPaths(dark);
    config["flat_files"] = joinPaths(flat);
    config["light_files"] = joinPaths(light);
    config["overwrite_masters"] = m_overwriteMastersChk->isChecked() ? "true" : "false";

    m_runningStage = "calibrate_register";

    m_progressBar->setRange(0, 0); // Pulse

    updateStepsPlan("calibrate_register");

    // Disable other tabs and main window controls to keep Process Console responsive
    for (int i = 0; i < m_tabs->count(); ++i) {
        if (m_tabs->tabText(i) != "Execute") {
            m_tabs->setTabEnabled(i, false);
        }
    }
    m_runBtn->setEnabled(false);
    m_alignStackBtn->setEnabled(false);
    m_cancelBtn->setEnabled(true);
    m_cancelBtn->setStyleSheet("QPushButton { background-color: #c62828; color: #ffffff; font-weight: bold; border-color: #d32f2f; }"
                               "QPushButton:hover { background-color: #e53935; }"
                               "QPushButton:disabled { background-color: #1e1e1e; color: #555555; border-color: #2a2a2a; }");

    QMainWindow* mainWin = nullptr;
    QWidget* w = parentWidget();
    while (w) {
        if (auto* mw = qobject_cast<QMainWindow*>(w)) {
            mainWin = mw;
            break;
        }
        w = w->parentWidget();
    }
    if (auto* mw = qobject_cast<MainWindow*>(mainWin)) {
        mw->setProcessingState(true);
    }

    // Spin up thread
    m_activeThread = new QThread(this);
    PreprocessingWorker* worker = new PreprocessingWorker(m_workspace, config);
    m_activeWorker = worker;
    worker->moveToThread(m_activeThread);

    connect(worker, &PreprocessingWorker::progressUpdated, this, [this](int progressVal) {
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(progressVal);
    });

    connect(worker, &PreprocessingWorker::stepStarted, this, &PreprocessingWizardDialog::onStepStarted);
    connect(worker, &PreprocessingWorker::stepProgressUpdated, this, &PreprocessingWizardDialog::onStepProgressUpdated);
    connect(worker, &PreprocessingWorker::stepFinished, this, &PreprocessingWizardDialog::onStepFinished);

    connect(m_activeThread, &QThread::started, worker, &PreprocessingWorker::run);
    connect(worker, &PreprocessingWorker::finished, this, [this, worker, mainWin](bool success, const QString& errorMsg) {
        m_activeThread->quit();
        m_activeThread->wait();

        worker->deleteLater();
        m_activeThread->deleteLater();
        m_activeThread = nullptr;
        m_activeWorker = nullptr;

        // Restore UI state
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(success ? 100 : 0);
        m_runBtn->setEnabled(true);
        m_alignStackBtn->setEnabled(success);
        m_resumeBtn->setEnabled(success);
        m_cancelBtn->setEnabled(false);
        m_cancelBtn->setStyleSheet("");

        for (int i = 0; i < m_tabs->count(); ++i) {
            if (m_tabs->tabText(i) != "Frame Selection") {
                m_tabs->setTabEnabled(i, true);
            }
        }

        if (auto* mw = qobject_cast<MainWindow*>(mainWin)) {
            mw->setProcessingState(false);
        }

        if (success) {
            logMessage("[PPW] Calibration & Registration stage finished successfully.");

            int frameSelRow = m_stage1StepCount;
            if (frameSelRow >= 0 && frameSelRow < m_stepsTable->rowCount()) {
                m_stepsTable->setItem(frameSelRow, 2, new QTableWidgetItem("Running"));
                m_stepsTable->item(frameSelRow, 2)->setForeground(QColor("#569CD6"));
            }
            
            // Build the list of active filters
            m_activeFilters.clear();
            for (const auto& f : m_stagedFiles) {
                if (f.imageType == "Light" && std::find(m_activeFilters.begin(), m_activeFilters.end(), f.filter) == m_activeFilters.end()) {
                    m_activeFilters.push_back(f.filter);
                }
            }

            // Populate filter combobox
            m_filterSelectCombo->blockSignals(true);
            m_filterSelectCombo->clear();
            for (const auto& filter : m_activeFilters) {
                m_filterSelectCombo->addItem(filter, filter);
            }
            m_filterSelectCombo->blockSignals(false);

            // Open master calibration frames in workspace layout for inspection
            if (auto* mw = qobject_cast<MainWindow*>(mainWin)) {
                for (const auto& name : m_workspace.elementNames()) {
                    if (name.find("master_bias_") != std::string::npos || name.find("master_dark_") != std::string::npos || name.find("master_flat_") != std::string::npos) {
                        QString qName = QString::fromStdString(name);
                        mw->m_workspaceArea->removeElementView(qName);
                        mw->m_workspaceArea->addElementView(qName, m_workspace.getElement(name));
                    }
                }
            }


            int selIndex = getTabIndex("Frame Selection");
            if (selIndex == -1) {
                m_tabs->addTab(m_selectTab, "Frame Selection");
                selIndex = m_tabs->count() - 1;
            }
            m_tabs->setTabEnabled(selIndex, true);

            if (!m_activeFilters.empty()) {
                m_filterSelectCombo->setCurrentIndex(0);
                onFilterSelectionChanged(0);
            }

            m_resumeBtn->setEnabled(true);
            m_tabs->setCurrentIndex(selIndex); // Switch to Frame Selection Tab
            QMessageBox::information(this, "PPW", "Calibration and registration finished! Please review the registered light frames in the Selection tab and configure range limits if needed.");
        } else {
            // Mark pending/running steps as cancelled/failed
            for (int r = 0; r < m_stepsTable->rowCount(); ++r) {
                if (auto* item = m_stepsTable->item(r, 2)) {
                    QString status = item->text();
                    if (status == "Pending" || status == "Running") {
                        m_stepsTable->setItem(r, 2, new QTableWidgetItem(errorMsg.contains("cancelled") ? "Cancelled" : "Failed"));
                        m_stepsTable->item(r, 2)->setForeground(QColor("#F44336"));
                    }
                }
            }

            if (errorMsg.contains("cancelled")) {
                logMessage("[PPW] Pipeline cancelled by user.");
            } else {
                logMessage(QString("[PPW] ERROR: %1").arg(errorMsg));
                QMessageBox::critical(this, "PPW Error", QString("Pipeline execution failed: %1").arg(errorMsg));
            }
        }
    });

    m_activeThread->start();
}

void PreprocessingWizardDialog::onMetricChanged(int index) {
    if (!m_workspace.contains(m_registeredLightsName)) return;
    auto batch = std::get<ImageBatchPtr>(m_workspace.getElement(m_registeredLightsName));

    std::string metric = "starCount";
    if (index == 1) metric = "fwhm";
    else if (index == 2) metric = "snr";

    m_plotWidget->setBatch(batch, metric);

    std::string rangeKey = m_registeredLightsName + "_" + metric;
    if (m_filterRanges.find(rangeKey) == m_filterRanges.end()) {
        m_filterRanges[rangeKey] = { m_plotWidget->absoluteMin(), m_plotWidget->absoluteMax() };
    }
    auto range = m_filterRanges[rangeKey];

    m_minSpin->blockSignals(true);
    m_maxSpin->blockSignals(true);
    m_minSpin->setRange(m_plotWidget->absoluteMin(), m_plotWidget->absoluteMax());
    m_maxSpin->setRange(m_plotWidget->absoluteMin(), m_plotWidget->absoluteMax());
    m_minSpin->setValue(range.first);
    m_maxSpin->setValue(range.second);
    m_minSpin->blockSignals(false);
    m_maxSpin->blockSignals(false);

    m_plotWidget->setFilterRange(range.first, range.second);
}

void PreprocessingWizardDialog::onRangeChanged(double minVal, double maxVal) {
    m_plotWidget->setFilterRange(minVal, maxVal);

    if (!m_workspace.contains(m_registeredLightsName)) return;
    auto batch = std::get<ImageBatchPtr>(m_workspace.getElement(m_registeredLightsName));
    std::string metric = m_plotWidget->currentMetric();

    std::string rangeKey = m_registeredLightsName + "_" + metric;
    m_filterRanges[rangeKey] = {minVal, maxVal};

    for (int i = 0; i < batch->count(); ++i) {
        auto meta = batch->frameMetadata(i);
        bool match = true;
        std::vector<std::string> metrics = {"starCount", "fwhm", "snr"};
        for (const auto& m : metrics) {
            std::string key = m_registeredLightsName + "_" + m;
            if (m_filterRanges.find(key) == m_filterRanges.end()) {
                m_filterRanges[key] = getAbsoluteRangeForBatch(batch, m);
            }
            auto range = m_filterRanges[key];
            double val = 0.0;
            if (m == "starCount") val = meta.starCount;
            else if (m == "fwhm") val = meta.fwhm;
            else if (m == "snr") val = meta.snr;

            if (val < range.first || val > range.second) {
                match = false;
                break;
            }
        }
        if (m_selectionTable->item(i, 0)) {
            m_selectionTable->item(i, 0)->setCheckState(match ? Qt::Checked : Qt::Unchecked);
        }
    }
}

void PreprocessingWizardDialog::onResumeStacking() {
    m_runningStage = "align_stack";
    saveCurrentSelectionStates();

    int execIndex = getTabIndex("Execute");
    if (execIndex != -1) {
        m_tabs->setCurrentIndex(execIndex);
    }

    int frameSelIndex = getTabIndex("Frame Selection");
    if (frameSelIndex != -1) {
        m_tabs->removeTab(frameSelIndex);
    }

    // Mark User Frame Selection as Completed
    int frameSelRow = m_stage1StepCount;
    if (frameSelRow >= 0 && frameSelRow < m_stepsTable->rowCount()) {
        m_stepsTable->setItem(frameSelRow, 2, new QTableWidgetItem("Completed"));
        m_stepsTable->item(frameSelRow, 2)->setForeground(QColor("#4EC9B0"));
        m_stepsTable->setItem(frameSelRow, 3, new QTableWidgetItem("100%"));
        m_stepsTable->item(frameSelRow, 3)->setForeground(QColor("#4EC9B0"));
    }

    m_progressBar->setValue(0);
    logMessage("[PPW] Resuming pipeline: Alignment and final Stacking...");

    std::string prefix = m_outPrefixEdit->text().toStdString();
    if (!prefix.empty() && prefix.back() != '_') {
        prefix += "_";
    }

    std::vector<std::string> activeBatchNames;
    for (const auto& filter : m_activeFilters) {
        activeBatchNames.push_back(prefix + "preprocessed_lights_" + filter.toStdString());
    }
    
    std::string joinedNames;
    for (size_t i = 0; i < activeBatchNames.size(); ++i) {
        joinedNames += activeBatchNames[i];
        if (i + 1 < activeBatchNames.size()) joinedNames += ";";
    }

    std::map<std::string, std::string> config;
    config["stage"] = "align_stack";
    config["registered_light_batch_name"] = joinedNames;
    config["drizzle_scale"] = QString::number(m_drizzleScaleSpin->value()).toStdString();
    config["align_ref_mode"] = m_alignRefModeCombo->currentData().toString().toStdString();
    config["keep_intermediate"] = m_keepIntermediateChk->isChecked() ? "true" : "false";
    config["out_dir"] = m_outDirEdit->text().toStdString();
    config["star_min_snr"] = QString::number(m_starMinSnrSpin->value()).toStdString();
    config["star_min_fwhm"] = QString::number(m_starMinFwhmSpin->value()).toStdString();
    config["out_prefix"] = m_outPrefixEdit->text().toStdString();

    m_progressBar->setRange(0, 0);

    // Disable other tabs and main window controls to keep Process Console responsive
    for (int i = 0; i < m_tabs->count(); ++i) {
        if (m_tabs->tabText(i) != "Execute") {
            m_tabs->setTabEnabled(i, false);
        }
    }
    m_runBtn->setEnabled(false);
    m_alignStackBtn->setEnabled(false);
    m_resumeBtn->setEnabled(false);
    m_cancelBtn->setEnabled(true);
    m_cancelBtn->setStyleSheet("QPushButton { background-color: #c62828; color: #ffffff; font-weight: bold; border-color: #d32f2f; }"
                               "QPushButton:hover { background-color: #e53935; }"
                               "QPushButton:disabled { background-color: #1e1e1e; color: #555555; border-color: #2a2a2a; }");

    QMainWindow* mainWin = nullptr;
    QWidget* w = parentWidget();
    while (w) {
        if (auto* mw = qobject_cast<QMainWindow*>(w)) {
            mainWin = mw;
            break;
        }
        w = w->parentWidget();
    }
    if (auto* mw = qobject_cast<MainWindow*>(mainWin)) {
        mw->setProcessingState(true);
    }

    // Spin up thread
    m_activeThread = new QThread(this);
    PreprocessingWorker* worker = new PreprocessingWorker(m_workspace, config);
    m_activeWorker = worker;
    worker->moveToThread(m_activeThread);

    connect(worker, &PreprocessingWorker::progressUpdated, this, [this](int progressVal) {
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(progressVal);
    });

    connect(worker, &PreprocessingWorker::stepStarted, this, &PreprocessingWizardDialog::onStepStarted);
    connect(worker, &PreprocessingWorker::stepProgressUpdated, this, &PreprocessingWizardDialog::onStepProgressUpdated);
    connect(worker, &PreprocessingWorker::stepFinished, this, &PreprocessingWizardDialog::onStepFinished);

    connect(m_activeThread, &QThread::started, worker, &PreprocessingWorker::run);
    connect(worker, &PreprocessingWorker::finished, this, [this, worker, mainWin](bool success, const QString& errorMsg) {
        m_activeThread->quit();
        m_activeThread->wait();

        worker->deleteLater();
        m_activeThread->deleteLater();
        m_activeThread = nullptr;
        m_activeWorker = nullptr;

        // Restore UI state
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(success ? 100 : 0);
        m_runBtn->setEnabled(true);
        m_alignStackBtn->setEnabled(success);
        m_resumeBtn->setEnabled(success);
        m_cancelBtn->setEnabled(false);
        m_cancelBtn->setStyleSheet("");

        for (int i = 0; i < m_tabs->count(); ++i) {
            m_tabs->setTabEnabled(i, true);
        }

        if (auto* mw = qobject_cast<MainWindow*>(mainWin)) {
            mw->setProcessingState(false);
        }

        if (success) {
            logMessage("[PPW] Stacking stage finished successfully!");
            
            QWidget* p = parentWidget();
            MainWindow* mw = nullptr;
            while (p) {
                if (auto* candidate = qobject_cast<MainWindow*>(p)) {
                    mw = candidate;
                    break;
                }
                p = p->parentWidget();
            }

            if (mw) {
                std::string prefix = m_outPrefixEdit->text().toStdString();
                if (!prefix.empty() && prefix.back() != '_') {
                    prefix += "_";
                }
                for (const auto& filter : m_activeFilters) {
                    std::string finalMasterName = prefix + "preprocessed_lights_" + filter.toStdString() + "_stacked";
                    if (m_workspace.contains(finalMasterName)) {
                        QString qName = QString::fromStdString(finalMasterName);
                        mw->m_workspaceArea->removeElementView(qName);
                        mw->m_workspaceArea->addElementView(qName, m_workspace.getElement(finalMasterName));
                    }
                }
            }
            QMessageBox::information(this, "PPW", "Preprocessing complete! Stacked master image(s) have been loaded into the workspace.");
            close();
        } else {
            // Mark pending/running steps as cancelled/failed
            for (int r = 0; r < m_stepsTable->rowCount(); ++r) {
                if (auto* item = m_stepsTable->item(r, 2)) {
                    QString status = item->text();
                    if (status == "Pending" || status == "Running") {
                        m_stepsTable->setItem(r, 2, new QTableWidgetItem(errorMsg.contains("cancelled") ? "Cancelled" : "Failed"));
                        m_stepsTable->item(r, 2)->setForeground(QColor("#F44336"));
                    }
                }
            }

            if (errorMsg.contains("cancelled")) {
                logMessage("[PPW] Pipeline cancelled by user.");
            } else {
                logMessage(QString("[PPW] ERROR: %1").arg(errorMsg));
                QMessageBox::critical(this, "PPW Error", QString("Pipeline execution failed: %1").arg(errorMsg));
            }
        }
    });

    m_activeThread->start();
}

void PreprocessingWizardDialog::saveCurrentSelectionStates() {
    if (m_registeredLightsName.empty() || !m_workspace.contains(m_registeredLightsName)) return;
    auto batch = std::get<ImageBatchPtr>(m_workspace.getElement(m_registeredLightsName));
    for (int i = 0; i < batch->count(); ++i) {
        if (m_selectionTable->item(i, 0)) {
            bool checked = (m_selectionTable->item(i, 0)->checkState() == Qt::Checked);
            batch->setFrameSelected(i, checked);
        }
    }
}

void PreprocessingWizardDialog::onFilterSelectionChanged(int index) {
    if (index < 0 || index >= static_cast<int>(m_activeFilters.size())) return;
    
    // 1. Save current state
    saveCurrentSelectionStates();
    
    // 2. Load new filter
    QString filter = m_activeFilters[index];
    
    std::string prefix = m_outPrefixEdit->text().toStdString();
    if (!prefix.empty() && prefix.back() != '_') {
        prefix += "_";
    }
    m_registeredLightsName = (prefix + "preprocessed_lights_" + filter.toStdString());
    
    if (m_workspace.contains(m_registeredLightsName)) {
        auto batch = std::get<ImageBatchPtr>(m_workspace.getElement(m_registeredLightsName));
        
        onMetricChanged(m_metricCombo->currentIndex());
        
        // Populate selection table
        m_selectionTable->setRowCount(0);
        for (int i = 0; i < batch->count(); ++i) {
            m_selectionTable->insertRow(i);
            
            QTableWidgetItem* checkItem = new QTableWidgetItem();
            bool match = true;
            std::vector<std::string> metrics = {"starCount", "fwhm", "snr"};
            auto meta = batch->frameMetadata(i);
            for (const auto& m : metrics) {
                std::string key = m_registeredLightsName + "_" + m;
                if (m_filterRanges.find(key) == m_filterRanges.end()) {
                    m_filterRanges[key] = getAbsoluteRangeForBatch(batch, m);
                }
                auto range = m_filterRanges[key];
                double val = 0.0;
                if (m == "starCount") val = meta.starCount;
                else if (m == "fwhm") val = meta.fwhm;
                else if (m == "snr") val = meta.snr;

                if (val < range.first || val > range.second) {
                    match = false;
                    break;
                }
            }
            checkItem->setCheckState(match ? Qt::Checked : Qt::Unchecked);
            m_selectionTable->setItem(i, 0, checkItem);
            
            m_selectionTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(batch->frameName(i))));
            
            m_selectionTable->setItem(i, 2, new QTableWidgetItem(QString::number(meta.starCount)));
            m_selectionTable->setItem(i, 3, new QTableWidgetItem(QString::number(meta.fwhm, 'f', 2)));
            m_selectionTable->setItem(i, 4, new QTableWidgetItem(QString::number(meta.snr, 'f', 2)));
        }
    }
}

std::pair<double, double> PreprocessingWizardDialog::getAbsoluteRangeForBatch(ImageBatchPtr batch, const std::string& metric) {
    if (!batch || batch->count() == 0) return {0.0, 1.0};
    int count = batch->count();
    double minVal = std::numeric_limits<double>::max();
    double maxVal = -std::numeric_limits<double>::max();
    for (int i = 0; i < count; ++i) {
        auto meta = batch->frameMetadata(i);
        double v = 0.0;
        if (metric == "starCount") v = meta.starCount;
        else if (metric == "fwhm") v = meta.fwhm;
        else if (metric == "snr") v = meta.snr;

        if (v < minVal) minVal = v;
        if (v > maxVal) maxVal = v;
    }
    if (minVal == maxVal) {
        minVal -= 1.0;
        maxVal += 1.0;
    }
    return {minVal, maxVal};
}

void PreprocessingWizardDialog::onSpinBoxChanged() {
    onRangeChanged(m_minSpin->value(), m_maxSpin->value());
}

QMdiArea* PreprocessingWizardDialog::findMdiArea() const {
    QWidget* p = parentWidget();
    while (p) {
        if (auto* mw = qobject_cast<QMainWindow*>(p)) {
            return mw->findChild<QMdiArea*>();
        }
        p = p->parentWidget();
    }
    return nullptr;
}

void PreprocessingWizardDialog::onCancel() {
    if (m_activeWorker) {
        logMessage("[PPW] Cancellation requested by user...");
        QMetaObject::invokeMethod(m_activeWorker, "cancel");
        m_cancelBtn->setEnabled(false);
    }
}

// PreprocessingWorker Implementation
PreprocessingWorker::PreprocessingWorker(WorkspaceRegistry& workspace, const std::map<std::string, std::string>& config)
    : m_workspace(workspace), m_config(config), m_cancelRequested(false) {}

void PreprocessingWorker::run() {
    try {
        PreprocessingPipeline pipeline;
        
        pipeline.setCancelCallback([this]() {
            return m_cancelRequested.load();
        });

        pipeline.setStepCallback([this](int stepIndex, int percent, double elapsed, bool finished, bool success) {
            if (finished) {
                emit stepFinished(stepIndex, success, elapsed);
            } else if (percent == 0 && elapsed == 0.0) {
                emit stepStarted(stepIndex);
            } else {
                emit stepProgressUpdated(stepIndex, percent, elapsed);
            }
        });

        pipeline.execute(m_workspace, m_config, [this](int progress) {
            emit progressUpdated(progress);
        });
        emit finished(true, "");
    } catch (const std::exception& ex) {
        emit finished(false, QString::fromStdString(ex.what()));
    }
}

void PreprocessingWorker::cancel() {
    m_cancelRequested.store(true);
}

int PreprocessingWizardDialog::getTabIndex(const QString& label) const {
    for (int i = 0; i < m_tabs->count(); ++i) {
        if (m_tabs->tabText(i) == label) {
            return i;
        }
    }
    return -1;
}

static QString getStageFromStepName(const std::string& stepName) {
    if (stepName.rfind("Stack ", 0) == 0 &&
        (stepName.find("master_bias") != std::string::npos ||
         stepName.find("master_dark") != std::string::npos ||
         stepName.find("master_flat") != std::string::npos)) {
        return "Calibrate (Stack)";
    } else if (stepName.rfind("Calibrate Flats", 0) == 0 ||
               stepName.rfind("Calibrate Lights", 0) == 0 ||
               stepName.rfind("Debayer Lights", 0) == 0) {
        return "Calibrate";
    } else if (stepName.rfind("Register Lights", 0) == 0) {
        return "Register";
    } else if (stepName.rfind("User Frame Selection", 0) == 0) {
        return "Frame Selection";
    } else if (stepName.rfind("Register ", 0) == 0) {
        return "Align (Register)";
    } else if (stepName.rfind("Align ", 0) == 0) {
        return "Align";
    } else if (stepName.rfind("Stack ", 0) == 0) {
        return "Stack";
    }
    return "";
}

void PreprocessingWizardDialog::updateStepsPlan(const std::string& dummyStage) {
    Q_UNUSED(dummyStage);
    m_stepsTable->setRowCount(0);
    
    std::map<std::string, std::string> config;
    config["mode"] = m_modeCombo->currentData().toString().toStdString();
    config["exp_tolerance"] = QString::number(m_expToleranceSpin->value()).toStdString();
    config["debayer"] = m_debayerChk->isChecked() ? "true" : "false";
    
    std::vector<std::string> bias, dark, flat, light;
    for (const auto& f : m_stagedFiles) {
        if (f.imageType == "Bias") bias.push_back(f.filepath.toStdString());
        else if (f.imageType == "Dark") dark.push_back(f.filepath.toStdString());
        else if (f.imageType == "Flat") flat.push_back(f.filepath.toStdString());
        else if (f.imageType == "Light") light.push_back(f.filepath.toStdString());
    }
    
    auto joinPaths = [](const std::vector<std::string>& paths) -> std::string {
        std::string res;
        for (size_t i = 0; i < paths.size(); ++i) {
            res += paths[i];
            if (i + 1 < paths.size()) res += ";";
        }
        return res;
    };

    config["bias_files"] = joinPaths(bias);
    config["dark_files"] = joinPaths(dark);
    config["flat_files"] = joinPaths(flat);
    config["light_files"] = joinPaths(light);
    config["out_prefix"] = m_outPrefixEdit->text().toStdString();
    
    // Stage 1
    config["stage"] = "calibrate_register";
    std::vector<std::string> stage1Steps = PreprocessingPipeline::getPlannedSteps(config);
    m_stage1StepCount = stage1Steps.size();

    // Stage 2
    config["stage"] = "align_stack";
    config["drizzle_scale"] = QString::number(m_drizzleScaleSpin->value()).toStdString();
    config["align_ref_mode"] = m_alignRefModeCombo->currentData().toString().toStdString();
    std::string prefix = m_outPrefixEdit->text().toStdString();
    if (!prefix.empty() && prefix.back() != '_') {
        prefix += "_";
    }
    std::vector<QString> activeFilters;
    for (const auto& f : m_stagedFiles) {
        if (f.imageType == "Light" && std::find(activeFilters.begin(), activeFilters.end(), f.filter) == activeFilters.end()) {
            activeFilters.push_back(f.filter);
        }
    }
    std::string joinedNames;
    for (size_t i = 0; i < activeFilters.size(); ++i) {
        joinedNames += prefix + "preprocessed_lights_" + activeFilters[i].toStdString();
        if (i + 1 < activeFilters.size()) joinedNames += ";";
    }
    config["registered_light_batch_name"] = joinedNames;
    std::vector<std::string> stage2Steps = PreprocessingPipeline::getPlannedSteps(config);

    // Combine them
    std::vector<std::string> allSteps = stage1Steps;
    allSteps.push_back("User Frame Selection");
    allSteps.insert(allSteps.end(), stage2Steps.begin(), stage2Steps.end());

    m_stepsTable->setRowCount(allSteps.size());
    for (int i = 0; i < static_cast<int>(allSteps.size()); ++i) {
        QString stepName = QString::fromStdString(allSteps[i]);
        QString stageName = getStageFromStepName(allSteps[i]);

        m_stepsTable->setItem(i, 0, new QTableWidgetItem(stepName));
        m_stepsTable->setItem(i, 1, new QTableWidgetItem(stageName));
        m_stepsTable->setItem(i, 2, new QTableWidgetItem("Pending"));
        m_stepsTable->setItem(i, 3, new QTableWidgetItem("0%"));
        m_stepsTable->setItem(i, 4, new QTableWidgetItem("0.0s"));
        
        m_stepsTable->item(i, 0)->setForeground(QColor("#a9b7c6"));
        m_stepsTable->item(i, 1)->setForeground(QColor("#a9b7c6"));
        m_stepsTable->item(i, 2)->setForeground(QColor("#808080"));
        m_stepsTable->item(i, 3)->setForeground(QColor("#808080"));
        m_stepsTable->item(i, 4)->setForeground(QColor("#808080"));
    }
    m_stepsTable->resizeColumnsToContents();
}

void PreprocessingWizardDialog::onStepStarted(int stepIndex) {
    int row = (m_runningStage == "align_stack") ? m_stage1StepCount + 1 + stepIndex : stepIndex;
    if (row < 0 || row >= m_stepsTable->rowCount()) return;
    m_stepsTable->setItem(row, 2, new QTableWidgetItem("Running"));
    m_stepsTable->item(row, 2)->setForeground(QColor("#569CD6"));
    m_stepsTable->setItem(row, 3, new QTableWidgetItem("0%"));
    m_stepsTable->item(row, 3)->setForeground(QColor("#569CD6"));
    m_stepsTable->scrollToItem(m_stepsTable->item(row, 0));
}

void PreprocessingWizardDialog::onStepProgressUpdated(int stepIndex, int percent, double elapsed) {
    int row = (m_runningStage == "align_stack") ? m_stage1StepCount + 1 + stepIndex : stepIndex;
    if (row < 0 || row >= m_stepsTable->rowCount()) return;
    m_stepsTable->setItem(row, 3, new QTableWidgetItem(QString("%1%").arg(percent)));
    m_stepsTable->item(row, 3)->setForeground(QColor("#569CD6"));
    m_stepsTable->setItem(row, 4, new QTableWidgetItem(QString("%1s").arg(elapsed, 0, 'f', 1)));
    m_stepsTable->item(row, 4)->setForeground(QColor("#a9b7c6"));
}

void PreprocessingWizardDialog::onStepFinished(int stepIndex, bool success, double elapsed) {
    int row = (m_runningStage == "align_stack") ? m_stage1StepCount + 1 + stepIndex : stepIndex;
    if (row < 0 || row >= m_stepsTable->rowCount()) return;
    if (success) {
        m_stepsTable->setItem(row, 2, new QTableWidgetItem("Completed"));
        m_stepsTable->item(row, 2)->setForeground(QColor("#4EC9B0"));
        m_stepsTable->setItem(row, 3, new QTableWidgetItem("100%"));
        m_stepsTable->item(row, 3)->setForeground(QColor("#4EC9B0"));
    } else {
        m_stepsTable->setItem(row, 2, new QTableWidgetItem("Error"));
        m_stepsTable->item(row, 2)->setForeground(QColor("#F44336"));
        m_stepsTable->setItem(row, 3, new QTableWidgetItem("-"));
        m_stepsTable->item(row, 3)->setForeground(QColor("#F44336"));
    }
    m_stepsTable->setItem(row, 4, new QTableWidgetItem(QString("%1s").arg(elapsed, 0, 'f', 1)));
    m_stepsTable->item(row, 4)->setForeground(QColor("#a9b7c6"));
}

} // namespace blastro
