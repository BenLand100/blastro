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
#include "WorkspaceImageWindow.h"
#include "BatchImageWidget.h"
#include <QEvent>
#include <QCloseEvent>
#include "algorithms/PreprocessingPipeline.h"
#include "core/Logger.h"
#include "core/Preferences.h"
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
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QResizeEvent>

namespace blastro {

class LeftElideDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;
protected:
    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override {
        QStyledItemDelegate::initStyleOption(option, index);
        option->textElideMode = Qt::ElideLeft;
    }
};

PreprocessingWizardDialog::PreprocessingWizardDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : QDialog(parent),
      m_workspace(workspace),
      m_tabs(new QTabWidget(this)),
      m_filesTable(new QTableWidget(this)),
      m_addFilesBtn(new QPushButton("Add Files", this)),
      m_addDirBtn(new QPushButton("Add Directories", this)),
      m_removeBtn(new QPushButton("Remove Selected", this)),
      m_clearBtn(new QPushButton("Clear All", this)),
      // Output Settings
      m_processDirLabel(new QLabel(this)),
      m_outPrefixEdit(new QLineEdit(this)),
      m_keepIntermediateChk(new QCheckBox("Keep intermediate files", this)),
      m_overwriteMastersChk(new QCheckBox("Overwrite existing master frames", this)),
      m_openCalibStacksChk(new QCheckBox("Open calibration stacks in workspace", this)),
      m_openLightMastersChk(new QCheckBox("Open light masters in workspace", this)),
      // Grouping
      m_strictDarkChk(new QCheckBox(this)),
      m_expToleranceSpin(new QDoubleSpinBox(this)),
      m_debayerChk(new QCheckBox("Debayer raw lights (OSC)", this)),
      m_bayerPatternCombo(new QComboBox(this)),
      m_debayerMethodCombo(new QComboBox(this)),
      // Bias/Dark Stacking
      m_biasDarkStackMethodCombo(new QComboBox(this)),
      m_biasDarkRejectionCombo(new QComboBox(this)),
      m_biasDarkSigmaLowSpin(new QDoubleSpinBox(this)),
      m_biasDarkSigmaHighSpin(new QDoubleSpinBox(this)),
      m_biasDarkScaleAdditiveChk(new QCheckBox(this)),
      m_biasDarkScaleMultiplicativeChk(new QCheckBox(this)),
      // Flat Stacking
      m_flatStackMethodCombo(new QComboBox(this)),
      m_flatRejectionCombo(new QComboBox(this)),
      m_flatSigmaLowSpin(new QDoubleSpinBox(this)),
      m_flatSigmaHighSpin(new QDoubleSpinBox(this)),
      m_flatScaleAdditiveChk(new QCheckBox(this)),
      m_flatScaleMultiplicativeChk(new QCheckBox(this)),
      // Registration
      m_starMinSnrSpin(new QDoubleSpinBox(this)),
      m_starMinFwhmSpin(new QDoubleSpinBox(this)),
      m_transformationModelCombo(new QComboBox(this)),
      m_starDetectionMethodCombo(new QComboBox(this)),
      m_starMaxStarsSpin(new QSpinBox(this)),
      m_starMaxRefinedSpin(new QSpinBox(this)),
      m_starMatchToleranceSpin(new QDoubleSpinBox(this)),
      m_starMaxEccentricitySpin(new QDoubleSpinBox(this)),
      // Alignment
      m_alignMethodCombo(new QComboBox(this)),
      m_alignRefModeCombo(new QComboBox(this)),
      m_drizzleScaleSpin(new QDoubleSpinBox(this)),
      m_drizzleDropSizeSpin(new QDoubleSpinBox(this)),
      m_alignMutuallyChk(new QCheckBox(this)),
      m_interpolationMethodCombo(new QComboBox(this)),
      // Light Stacking
      m_lightStackMethodCombo(new QComboBox(this)),
      m_lightRejectionCombo(new QComboBox(this)),
      m_lightSigmaLowSpin(new QDoubleSpinBox(this)),
      m_lightSigmaHighSpin(new QDoubleSpinBox(this)),
      m_runBgeChk(new QCheckBox(this)),
      m_bgeMethodCombo(new QComboBox(this)),
      m_bgeOrderSlider(new QSlider(Qt::Horizontal, this)),
      m_bgeOrderSpin(new QSpinBox(this)),
      m_bgeRbfSmoothingSlider(new QSlider(Qt::Horizontal, this)),
      m_bgeRbfSmoothingSpin(new QDoubleSpinBox(this)),
      m_bgeGridColsSpin(new QSpinBox(this)),
      m_bgeGridRowsSpin(new QSpinBox(this)),
      m_bgeAutoExcludeChk(new QCheckBox("Enable Rejection", this)),
      m_bgeMaxDeviationSpin(new QDoubleSpinBox(this)),
      m_bgeMaxStructureSpin(new QDoubleSpinBox(this)),
      m_lightScaleAdditiveChk(new QCheckBox(this)),
      m_lightScaleMultiplicativeChk(new QCheckBox(this)),
      // Other
      m_previewTree(new QTreeWidget(this)),
      m_processTree(new QTreeWidget(this)),
      m_processFormulaText(new QTextEdit(this)),
      m_plotWidget(new StatsPlotWidget(this)),
      m_metricCombo(new QComboBox(this)),
      m_minSpin(new QDoubleSpinBox(this)),
      m_maxSpin(new QDoubleSpinBox(this)),
      m_selectionTable(new QTableWidget(this)),
      m_resumeBtn(new QPushButton("Execute Align && Stack", this)),
      m_filterSelectCombo(new QComboBox(this)),
      m_progressBar(new QProgressBar(this)),
      m_cancelBtn(new QPushButton("Cancel", this)),
      m_runBtn(new QPushButton("Execute Calibrate && Register", this)),
      m_alignStackBtn(new QPushButton("Execute Align && Stack", this)),
      m_stepsTable(new QTableWidget(this)) {

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
    m_filesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    filesLayout->addWidget(m_filesTable, 1);

    QHBoxLayout* fileBtns = new QHBoxLayout();
    fileBtns->addWidget(m_addFilesBtn);
    fileBtns->addWidget(m_addDirBtn);
    fileBtns->addWidget(m_removeBtn);
    fileBtns->addWidget(m_clearBtn);
    filesLayout->addLayout(fileBtns);
    m_tabs->addTab(filesTab, "Files");

    // 2. Control Tab — collapsible-section design inside a QScrollArea
    QWidget* controlTab = new QWidget(this);
    QVBoxLayout* controlTabOuterLayout = new QVBoxLayout(controlTab);
    controlTabOuterLayout->setContentsMargins(0, 0, 0, 0);
    controlTabOuterLayout->setSpacing(0);

    QScrollArea* controlScroll = new QScrollArea(controlTab);
    controlScroll->setWidgetResizable(true);
    controlScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    controlScroll->setFrameShape(QFrame::NoFrame);

    QWidget* controlScrollContent = new QWidget();
    QVBoxLayout* controlLayout = new QVBoxLayout(controlScrollContent);
    controlLayout->setContentsMargins(12, 10, 12, 10);
    controlLayout->setSpacing(6);

    // Helper lambda to build a collapsible section
    // Returns {headerBtn, contentWidget, contentLayout}
    auto makeSectionHeader = [&](const QString& title, bool expanded) -> std::tuple<QPushButton*, QWidget*, QFormLayout*> {
        QPushButton* hdr = new QPushButton(
            (expanded ? "▼  " : "▶  ") + title, controlScrollContent);
        hdr->setCheckable(true);
        hdr->setChecked(expanded);
        hdr->setStyleSheet("QPushButton { text-align: left; }");
        hdr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QWidget* body = new QWidget(controlScrollContent);
        body->setVisible(expanded);
        QFormLayout* form = new QFormLayout(body);
        form->setContentsMargins(14, 10, 14, 10);
        form->setSpacing(9);
        form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
        form->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);

        QObject::connect(hdr, &QPushButton::toggled, body, [hdr, body](bool checked) {
            body->setVisible(checked);
            hdr->setText((checked ? "▼  " : "▶  ") + hdr->text().mid(3));
        });

        return {hdr, body, form};
    };

    // Helper lambda to build a stacking sub-form (method + rejection + sigma)
    // Returns the combo pointers via out-params
    auto addStackingRows = [&](QFormLayout* form,
                               QComboBox* methodCombo,
                               QComboBox* rejCombo,
                               QDoubleSpinBox* sigLowSpin,
                               QDoubleSpinBox* sigHighSpin,
                               const QString& defaultMethod,
                               const QString& defaultRej,
                               double sigLow, double sigHigh)
    {
        methodCombo->addItem("Average (Mean)", "average");
        methodCombo->addItem("Median", "median");
        if (defaultMethod == "average") methodCombo->setCurrentIndex(0);
        else if (defaultMethod == "median") methodCombo->setCurrentIndex(1);
        form->addRow("Stacking Method:", methodCombo);

        rejCombo->addItem("Sigma Clipping", "sigmaclip");
        rejCombo->addItem("Winsorized Sigma Clipping", "winsorized");
        rejCombo->addItem("No Rejection", "none");
        if (defaultRej == "sigmaclip") rejCombo->setCurrentIndex(0);
        else if (defaultRej == "winsorized") rejCombo->setCurrentIndex(1);
        else rejCombo->setCurrentIndex(2);
        form->addRow("Rejection:", rejCombo);

        sigLowSpin->setRange(0.5, 10.0);
        sigLowSpin->setSingleStep(0.5);
        sigLowSpin->setValue(sigLow);
        sigLowSpin->setDecimals(1);

        sigHighSpin->setRange(0.5, 10.0);
        sigHighSpin->setSingleStep(0.5);
        sigHighSpin->setValue(sigHigh);
        sigHighSpin->setDecimals(1);

        QWidget* sigRow = new QWidget();
        QHBoxLayout* sigLayout = new QHBoxLayout(sigRow);
        sigLayout->setContentsMargins(0, 0, 0, 0);
        sigLayout->setSpacing(6);
        sigLayout->addWidget(sigLowSpin);
        sigLayout->addWidget(new QLabel("  /  "));
        sigLayout->addWidget(sigHighSpin);
        sigLayout->addStretch(1);
        form->addRow("σ Low / High:", sigRow);

        // Toggle sigma row enabled state based on rejection
        auto updateSigmaEnabled = [sigLowSpin, sigHighSpin, rejCombo]() {
            bool enabled = (rejCombo->currentData().toString() != "none");
            sigLowSpin->setEnabled(enabled);
            sigHighSpin->setEnabled(enabled);
        };
        updateSigmaEnabled();
        QObject::connect(rejCombo, qOverload<int>(&QComboBox::currentIndexChanged), [updateSigmaEnabled](int) {
            updateSigmaEnabled();
        });
    };

    // ── Section 1: Output Settings (expanded by default) ──────────────────────
    {
        auto [hdr, body, form] = makeSectionHeader("Output Settings", true);
        controlLayout->addWidget(hdr);
        controlLayout->addWidget(body);

        form->addRow("Output Prefix:", m_outPrefixEdit);

        m_processDirLabel->setStyleSheet("color: #aaa; font-size: 10px;");
        m_processDirLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        updateProcessDirLabel();
        form->addRow("Process Folder:", m_processDirLabel);

        m_keepIntermediateChk->setChecked(false);
        form->addRow("", m_keepIntermediateChk);

        m_overwriteMastersChk->setChecked(false);
        form->addRow("", m_overwriteMastersChk);

        m_openCalibStacksChk->setChecked(true);
        form->addRow("", m_openCalibStacksChk);

        m_openLightMastersChk->setChecked(true);
        form->addRow("", m_openLightMastersChk);
    }

    controlLayout->addSpacing(4);

    // ── Section 2: Grouping (expanded by default) ─────────────────────────────
    {
        auto [hdr, body, form] = makeSectionHeader("Grouping", true);
        controlLayout->addWidget(hdr);
        controlLayout->addWidget(body);

        m_strictDarkChk->setChecked(false);
        form->addRow("Strict Dark Exposure Matching:", m_strictDarkChk);

        m_expToleranceSpin->setRange(0.1, 60.0);
        m_expToleranceSpin->setValue(0.5);
        m_expToleranceSpin->setSingleStep(0.1);
        m_expToleranceSpin->setSuffix(" s");
        m_expToleranceSpin->setDecimals(1);
        form->addRow("Exposure Tolerance:", m_expToleranceSpin);

        form->addRow("", m_debayerChk);

        m_bayerPatternCombo->addItems({"RGGB", "BGGR", "GRBG", "GBRG"});
        form->addRow("Bayer Pattern:", m_bayerPatternCombo);

        m_debayerMethodCombo->addItems({"bilinear", "vng"});
        form->addRow("Debayer Method:", m_debayerMethodCombo);
    }

    controlLayout->addSpacing(4);

    // ── Section 3: Bias / Dark Stacking Settings (collapsed) ─────────────────
    {
        auto [hdr, body, form] = makeSectionHeader("Bias / Dark Stacking Settings", false);
        controlLayout->addWidget(hdr);
        controlLayout->addWidget(body);

        addStackingRows(form,
            m_biasDarkStackMethodCombo, m_biasDarkRejectionCombo,
            m_biasDarkSigmaLowSpin,     m_biasDarkSigmaHighSpin,
            "average", "sigmaclip", 3.0, 3.0);

        m_biasDarkScaleAdditiveChk->setChecked(false);
        form->addRow("Additive Normalization:", m_biasDarkScaleAdditiveChk);
        m_biasDarkScaleMultiplicativeChk->setChecked(false);
        form->addRow("Multiplicative Normalization:", m_biasDarkScaleMultiplicativeChk);
    }

    controlLayout->addSpacing(4);

    // ── Section 4: Flat Stacking Settings (collapsed) ────────────────────────
    {
        auto [hdr, body, form] = makeSectionHeader("Flat Stacking Settings", false);
        controlLayout->addWidget(hdr);
        controlLayout->addWidget(body);

        addStackingRows(form,
            m_flatStackMethodCombo, m_flatRejectionCombo,
            m_flatSigmaLowSpin,     m_flatSigmaHighSpin,
            "average", "sigmaclip", 3.0, 3.0);

        m_flatScaleAdditiveChk->setChecked(false);
        form->addRow("Additive Normalization:", m_flatScaleAdditiveChk);
        m_flatScaleMultiplicativeChk->setChecked(true);
        form->addRow("Multiplicative Normalization:", m_flatScaleMultiplicativeChk);
    }

    controlLayout->addSpacing(4);

    // ── Section 5: Registration Settings / Star Finding (collapsed) ──────────
    {
        auto [hdr, body, form] = makeSectionHeader("Registration Settings", false);
        controlLayout->addWidget(hdr);
        controlLayout->addWidget(body);

        m_starDetectionMethodCombo->addItem("Advanced Adaptive", "adaptive");
        m_starDetectionMethodCombo->addItem("Basic Centroid", "centroid");
        m_starDetectionMethodCombo->addItem("Standard Gaussian", "gaussian");
        form->addRow("Detection Method:", m_starDetectionMethodCombo);

        m_starMinSnrSpin->setRange(1.0, 100.0);
        m_starMinSnrSpin->setValue(4.0);
        m_starMinSnrSpin->setSingleStep(0.5);
        m_starMinSnrSpin->setDecimals(1);
        form->addRow("Minimum Star SNR:", m_starMinSnrSpin);

        m_starMinFwhmSpin->setRange(0.5, 20.0);
        m_starMinFwhmSpin->setValue(1.5);
        m_starMinFwhmSpin->setSingleStep(0.5);
        m_starMinFwhmSpin->setDecimals(1);
        m_starMinFwhmSpin->setSuffix(" px");
        form->addRow("Minimum Star FWHM:", m_starMinFwhmSpin);

        m_starMaxStarsSpin->setRange(10, 50000);
        m_starMaxStarsSpin->setValue(10000);
        form->addRow("Max Stars to Detect:", m_starMaxStarsSpin);

        m_starMaxRefinedSpin->setRange(10, 5000);
        m_starMaxRefinedSpin->setValue(250);
        form->addRow("Max Stars to Refine:", m_starMaxRefinedSpin);

        m_starMaxEccentricitySpin->setRange(0.1, 1.0);
        m_starMaxEccentricitySpin->setValue(0.9);
        m_starMaxEccentricitySpin->setSingleStep(0.05);
        form->addRow("Max Star Eccentricity:", m_starMaxEccentricitySpin);

        m_starMatchToleranceSpin->setRange(0.1, 10.0);
        m_starMatchToleranceSpin->setValue(1.5);
        m_starMatchToleranceSpin->setSingleStep(0.1);
        m_starMatchToleranceSpin->setSuffix(" px");
        form->addRow("Constellation Match Tol:", m_starMatchToleranceSpin);

        m_transformationModelCombo->addItem("Rigid Body (Translation + Rotation)", "rigid");
        m_transformationModelCombo->addItem("Affine (Translation + Rotation + Scale + Shear)", "affine");
        form->addRow("Transformation Model:", m_transformationModelCombo);
    }

    controlLayout->addSpacing(4);

    // ── Section 6: Alignment Settings (collapsed) ────────────────────────────
    {
        auto [hdr, body, form] = makeSectionHeader("Alignment Settings", false);
        controlLayout->addWidget(hdr);
        controlLayout->addWidget(body);

        m_alignMethodCombo->addItem("Interpolate", "interpolate");
        m_alignMethodCombo->addItem("Drizzle", "drizzle");
        form->addRow("Alignment Mode:", m_alignMethodCombo);

        m_interpolationMethodCombo->addItem("Bilinear (Fast, Softer)", "bilinear");
        m_interpolationMethodCombo->addItem("Bicubic Spline", "bicubic");
        m_interpolationMethodCombo->addItem("Lanczos-3 (High Quality, Sharper)", "lanczos3");
        m_interpolationMethodCombo->addItem("Lanczos-4 (Highest Quality)", "lanczos4");
        m_interpolationMethodCombo->setCurrentIndex(2); // Default to Lanczos-3
        form->addRow("Interpolation Method:", m_interpolationMethodCombo);

        m_drizzleScaleSpin->setRange(1.0, 3.0);
        m_drizzleScaleSpin->setValue(1.5);
        m_drizzleScaleSpin->setSingleStep(0.5);
        m_drizzleScaleSpin->setDecimals(1);
        m_drizzleScaleSpin->setSuffix("×");
        form->addRow("Drizzle Scale:", m_drizzleScaleSpin);

        m_drizzleDropSizeSpin->setRange(0.1, 1.0);
        m_drizzleDropSizeSpin->setValue(1.0);
        m_drizzleDropSizeSpin->setSingleStep(0.1);
        m_drizzleDropSizeSpin->setDecimals(2);
        form->addRow("Drizzle Drop Size:", m_drizzleDropSizeSpin);

        m_alignRefModeCombo->addItem("Find Centermost Frame", "average_center");
        m_alignRefModeCombo->addItem("Use Registration Reference", "registration");
        form->addRow("Reference Mode:", m_alignRefModeCombo);

        m_alignMutuallyChk->setChecked(true);
        form->addRow("Mutually Align Stacks:", m_alignMutuallyChk);

        connect(m_alignMethodCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PreprocessingWizardDialog::onAlignMethodChanged);
        onAlignMethodChanged(0);
    }

    controlLayout->addSpacing(4);

    // ── Section 6.5: Background Normalization Settings (collapsed) ───────────
    {
        auto [hdr, body, form] = makeSectionHeader("Background Normalization", false);
        controlLayout->addWidget(hdr);
        controlLayout->addWidget(body);

        m_runBgeChk->setChecked(true);
        form->addRow("Run Background Subtraction:", m_runBgeChk);

        m_bgeMethodCombo->addItem("2D Polynomial Surface", "Polynomial");
        m_bgeMethodCombo->addItem("Radial Basis Function (RBF)", "RBF");
        form->addRow("Model Method:", m_bgeMethodCombo);

        // Polynomial Order
        QHBoxLayout* orderLayout = new QHBoxLayout();
        m_bgeOrderSlider->setRange(1, 5);
        m_bgeOrderSlider->setValue(3);
        m_bgeOrderSpin->setRange(1, 5);
        m_bgeOrderSpin->setValue(3);
        orderLayout->addWidget(m_bgeOrderSlider, 1);
        orderLayout->addWidget(m_bgeOrderSpin);
        form->addRow("Polynomial Order:", orderLayout);

        connect(m_bgeOrderSlider, &QSlider::valueChanged, m_bgeOrderSpin, &QSpinBox::setValue);
        connect(m_bgeOrderSpin, qOverload<int>(&QSpinBox::valueChanged), m_bgeOrderSlider, &QSlider::setValue);

        // RBF Smoothing
        QHBoxLayout* rbfSmoothingLayout = new QHBoxLayout();
        m_bgeRbfSmoothingSlider->setRange(0, 100);
        m_bgeRbfSmoothingSlider->setValue(50);
        m_bgeRbfSmoothingSpin->setRange(0.0, 1000.0);
        m_bgeRbfSmoothingSpin->setSingleStep(0.01);
        m_bgeRbfSmoothingSpin->setValue(0.5);
        rbfSmoothingLayout->addWidget(m_bgeRbfSmoothingSlider, 1);
        rbfSmoothingLayout->addWidget(m_bgeRbfSmoothingSpin);
        form->addRow("RBF Smoothing:", rbfSmoothingLayout);

        connect(m_bgeRbfSmoothingSlider, &QSlider::valueChanged, this, [this](int val) {
            m_bgeRbfSmoothingSpin->blockSignals(true);
            m_bgeRbfSmoothingSpin->setValue(val / 100.0);
            m_bgeRbfSmoothingSpin->blockSignals(false);
        });
        connect(m_bgeRbfSmoothingSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double val) {
            m_bgeRbfSmoothingSlider->blockSignals(true);
            if (val >= 0.0 && val <= 1.0) {
                m_bgeRbfSmoothingSlider->setValue(static_cast<int>(val * 100.0));
            } else {
                m_bgeRbfSmoothingSlider->setValue(0);
            }
            m_bgeRbfSmoothingSlider->blockSignals(false);
        });

        // Dynamic show/hide
        connect(m_bgeMethodCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this, form, orderLayout, rbfSmoothingLayout](int index) {
            form->setRowVisible(orderLayout, index == 0);
            form->setRowVisible(rbfSmoothingLayout, index == 1);
        });
        form->setRowVisible(rbfSmoothingLayout, false);

        // Grid Dimensions
        QHBoxLayout* gridLayout = new QHBoxLayout();
        gridLayout->addWidget(new QLabel("Cols:", this));
        m_bgeGridColsSpin->setRange(3, 30);
        m_bgeGridColsSpin->setValue(5);
        m_bgeGridColsSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        gridLayout->addWidget(m_bgeGridColsSpin);

        gridLayout->addWidget(new QLabel("Rows:", this));
        m_bgeGridRowsSpin->setRange(3, 30);
        m_bgeGridRowsSpin->setValue(5);
        m_bgeGridRowsSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        gridLayout->addWidget(m_bgeGridRowsSpin);
        form->addRow("Grid Dimensions:", gridLayout);

        // Bad Point Rejection
        QHBoxLayout* rejectLayout = new QHBoxLayout();
        m_bgeAutoExcludeChk->setChecked(true);
        rejectLayout->addWidget(m_bgeAutoExcludeChk);

        rejectLayout->addWidget(new QLabel("Max Dev:", this));
        m_bgeMaxDeviationSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_bgeMaxDeviationSpin->setRange(0.5, 10.0);
        m_bgeMaxDeviationSpin->setSingleStep(0.5);
        m_bgeMaxDeviationSpin->setValue(3.0);
        rejectLayout->addWidget(m_bgeMaxDeviationSpin);

        rejectLayout->addWidget(new QLabel("Max Struct:", this));
        m_bgeMaxStructureSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_bgeMaxStructureSpin->setRange(0.1, 10.0);
        m_bgeMaxStructureSpin->setSingleStep(0.1);
        m_bgeMaxStructureSpin->setValue(1.5);
        rejectLayout->addWidget(m_bgeMaxStructureSpin);
        form->addRow("Bad Point Rejection:", rejectLayout);

        connect(m_bgeAutoExcludeChk, &QCheckBox::toggled, this, [this](bool checked) {
            m_bgeMaxDeviationSpin->setEnabled(checked);
            m_bgeMaxStructureSpin->setEnabled(checked);
        });

        connect(m_runBgeChk, &QCheckBox::toggled, this, [this, form, orderLayout, rbfSmoothingLayout](bool checked) {
            m_bgeMethodCombo->setEnabled(checked);
            if (checked) {
                form->setRowVisible(orderLayout, m_bgeMethodCombo->currentIndex() == 0);
                form->setRowVisible(rbfSmoothingLayout, m_bgeMethodCombo->currentIndex() == 1);
            } else {
                form->setRowVisible(orderLayout, false);
                form->setRowVisible(rbfSmoothingLayout, false);
            }
            m_bgeGridColsSpin->setEnabled(checked);
            m_bgeGridRowsSpin->setEnabled(checked);
            m_bgeAutoExcludeChk->setEnabled(checked);
            m_bgeMaxDeviationSpin->setEnabled(checked && m_bgeAutoExcludeChk->isChecked());
            m_bgeMaxStructureSpin->setEnabled(checked && m_bgeAutoExcludeChk->isChecked());
        });

        // Trigger initial state
        m_bgeMethodCombo->currentIndexChanged(m_bgeMethodCombo->currentIndex());
    }

    controlLayout->addSpacing(4);

    // ── Section 7: Light Stacking Settings (expanded by default) ─────────────
    {
        auto [hdr, body, form] = makeSectionHeader("Light Stacking Settings", true);
        controlLayout->addWidget(hdr);
        controlLayout->addWidget(body);

        addStackingRows(form,
            m_lightStackMethodCombo, m_lightRejectionCombo,
            m_lightSigmaLowSpin,     m_lightSigmaHighSpin,
            "average", "winsorized", 3.0, 3.0);



        m_lightScaleAdditiveChk->setChecked(true);
        form->addRow("Additive Normalization:", m_lightScaleAdditiveChk);

        m_lightScaleMultiplicativeChk->setChecked(true);
        form->addRow("Multiplicative Normalization:", m_lightScaleMultiplicativeChk);
    }

    controlLayout->addStretch(1);
    controlScroll->setWidget(controlScrollContent);
    controlTabOuterLayout->addWidget(controlScroll);
    m_tabs->addTab(controlTab, "Control");



    // 3. Groups Tab
    QWidget* groupsTab = new QWidget(this);
    QVBoxLayout* groupsLayout = new QVBoxLayout(groupsTab);
    m_previewTree->setColumnCount(6);
    m_previewTree->setItemDelegateForColumn(0, new LeftElideDelegate(this));
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
    m_processTree->setItemDelegateForColumn(0, new LeftElideDelegate(this));
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
    m_selectionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_selectionTable->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(m_selectionTable, &QTableWidget::cellDoubleClicked, this, &PreprocessingWizardDialog::onSelectionCellDoubleClicked);
    connect(m_selectionTable, &QTableWidget::itemChanged, this, &PreprocessingWizardDialog::onSelectionTableItemChanged);
    selectLayout->addWidget(m_selectionTable, 1);

    m_resumeBtn->setEnabled(false);
    selectLayout->addWidget(m_resumeBtn);

    // 6. Execute Tab
    QWidget* progressTab = new QWidget(this);
    QVBoxLayout* progressLayout = new QVBoxLayout(progressTab);

    m_stepsTable->setColumnCount(5);
    m_stepsTable->setHorizontalHeaderLabels({"Planned Step", "Stage", "Status", "Progress", "Elapsed"});
    m_stepsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_stepsTable->horizontalHeader()->setStretchLastSection(true);
    m_stepsTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_stepsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    progressLayout->addWidget(m_stepsTable, 1);

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

    connect(m_metricCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, &PreprocessingWizardDialog::onMetricChanged);
    connect(m_minSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &PreprocessingWizardDialog::onSpinBoxChanged);
    connect(m_maxSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &PreprocessingWizardDialog::onSpinBoxChanged);
    connect(m_outPrefixEdit, &QLineEdit::textChanged, this, &PreprocessingWizardDialog::updatePreview);
    connect(m_strictDarkChk, &QCheckBox::checkStateChanged, this, &PreprocessingWizardDialog::updatePreview);
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
    connect(m_previewTree, &QTreeWidget::itemExpanded, this, &PreprocessingWizardDialog::recomputeColumnWidths);
    connect(m_previewTree, &QTreeWidget::itemCollapsed, this, &PreprocessingWizardDialog::recomputeColumnWidths);
    connect(m_processTree, &QTreeWidget::itemExpanded, this, &PreprocessingWizardDialog::recomputeColumnWidths);
    connect(m_processTree, &QTreeWidget::itemCollapsed, this, &PreprocessingWizardDialog::recomputeColumnWidths);
    connect(m_filesTable, &QTableWidget::itemDoubleClicked, this, &PreprocessingWizardDialog::onFileDoubleClicked);
    connect(m_previewTree, &QTreeWidget::itemDoubleClicked, this, &PreprocessingWizardDialog::onTreeItemDoubleClicked);
    connect(m_processTree, &QTreeWidget::itemDoubleClicked, this, &PreprocessingWizardDialog::onTreeItemDoubleClicked);
    // (Process folder is derived from CWD + preferences, no default needed here)
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
        m_filesTable->item(i, 0)->setData(Qt::UserRole, f.filepath);

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
        // outDir is now derived from CWD + process folder name from preferences
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

    if (m_outPrefixEdit->text().isEmpty()) {
        // (out_dir now derived from CWD+prefs; nothing to auto-update here)
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
            child->setData(0, Qt::UserRole, QString::fromStdString(f));
            child->setToolTip(0, QString::fromStdString(f));
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
            child->setData(0, Qt::UserRole, QString::fromStdString(f));
            child->setToolTip(0, QString::fromStdString(f));
        }
    }
    m_previewTree->collapseAll();

    // 2. Populate Process Tree (Dependency flow) and collect equations
    QStringList formulas;

    auto findBestDark = [&](const PreprocessingGroup& rawGroup) -> const PreprocessingGroup* {
        const PreprocessingGroup* bestDark = nullptr;
        double bestDarkExp = -1.0;
        bool strict = m_strictDarkChk->isChecked();
        double tol = m_expToleranceSpin->value();
        for (const auto& g : groups) {
            if (g.type != "Dark") continue;
            if (g.binningX != rawGroup.binningX || g.binningY != rawGroup.binningY || g.width != rawGroup.width || g.height != rawGroup.height) continue;
            if (static_cast<int>(g.gain) != static_cast<int>(rawGroup.gain)) continue;
            
            bool match = false;
            if (strict) {
                match = (std::abs(g.exposure - rawGroup.exposure) <= tol);
            } else {
                match = (g.exposure <= rawGroup.exposure + tol);
            }
            
            if (match) {
                if (g.exposure > bestDarkExp) {
                    bestDarkExp = g.exposure;
                    bestDark = &g;
                }
            }
        }
        return bestDark;
    };

    auto findBias = [&](const PreprocessingGroup& rawGroup) -> const PreprocessingGroup* {
        for (const auto& g : groups) {
            if (g.type != "Bias") continue;
            if (g.binningX != rawGroup.binningX || g.binningY != rawGroup.binningY || g.width != rawGroup.width || g.height != rawGroup.height) continue;
            if (static_cast<int>(g.gain) != static_cast<int>(rawGroup.gain)) continue;
            return &g;
        }
        return nullptr;
    };

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
        const PreprocessingGroup* matchedDark = findBestDark(lg);
        const PreprocessingGroup* matchedBias = findBias(lg);
        const PreprocessingGroup* darkMatch = nullptr;
        const PreprocessingGroup* biasMatch = nullptr;
        if (matchedDark) {
            darkMatch = matchedDark;
        } else if (matchedBias) {
            biasMatch = matchedBias;
        }

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
        if (!flatMatch) {
            for (const auto& g : groups) {
                if (g.type != "Flat") continue;
                if (g.binningX != lg.binningX || g.binningY != lg.binningY || g.width != lg.width || g.height != lg.height) continue;
                if (g.filter != lg.filter) continue;
                flatMatch = &g;
                break;
            }
        }
        if (flatMatch) {
            QTreeWidgetItem* flatNode = new QTreeWidgetItem(lightNode);
            flatNode->setText(0, "▣ Master Flat");
            flatNode->setText(1, QString::number(flatMatch->filepaths.size()));
            flatNode->setText(2, QString::fromStdString(flatMatch->filter));
            flatNode->setText(3, QString("%1s").arg(flatMatch->exposure));
            flatNode->setText(4, QString::number(flatMatch->gain));
            flatNode->setText(5, QString("%1x%2").arg(flatMatch->binningX).arg(flatMatch->binningY));

            const PreprocessingGroup* matchedFlatDark = findBestDark(*flatMatch);
            const PreprocessingGroup* matchedFlatBias = findBias(*flatMatch);
            const PreprocessingGroup* flatDarkMatch = nullptr;
            const PreprocessingGroup* flatBiasMatch = nullptr;
            if (matchedFlatDark) {
                flatDarkMatch = matchedFlatDark;
            } else if (matchedFlatBias) {
                flatBiasMatch = matchedFlatBias;
            }

            QString flatEq;
            if (flatDarkMatch) {
                flatEq = "Calibrated_Flat = Flat - Master_Dark";
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
                fdNode->setText(0, "⧇ Master Dark");
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

    recomputeColumnWidths();
    updateStepsPlan("calibrate_register");
}

void PreprocessingWizardDialog::recomputeColumnWidths() {
    int maxW = this->width() / 2;
    if (maxW < 100) maxW = 400;
    
    for (int i = 0; i < 6; ++i) {
        m_previewTree->resizeColumnToContents(i);
        m_processTree->resizeColumnToContents(i);
    }
    
    if (m_previewTree->columnWidth(0) > maxW) {
        m_previewTree->setColumnWidth(0, maxW);
    }
    if (m_processTree->columnWidth(0) > maxW) {
        m_processTree->setColumnWidth(0, maxW);
    }
}

void PreprocessingWizardDialog::resizeEvent(QResizeEvent* event) {
    QDialog::resizeEvent(event);
    recomputeColumnWidths();
}

void PreprocessingWizardDialog::logMessage(const QString& msg) {
    Logger::info("Preprocessing", msg);
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
    config["strict_dark"] = m_strictDarkChk->isChecked() ? "true" : "false";
    config["exp_tolerance"] = QString::number(m_expToleranceSpin->value()).toStdString();
    config["debayer"] = m_debayerChk->isChecked() ? "true" : "false";
    config["bayer_pattern"] = m_bayerPatternCombo->currentText().toStdString();
    config["debayer_method"] = m_debayerMethodCombo->currentText().toStdString();
    config["out_dir"] = QDir::current().filePath(
        QString::fromStdString(Preferences::instance().getProcessFolderName())).toStdString();
    config["keep_intermediate"] = m_keepIntermediateChk->isChecked() ? "true" : "false";
    config["star_min_snr"] = QString::number(m_starMinSnrSpin->value()).toStdString();
    config["star_min_fwhm"] = QString::number(m_starMinFwhmSpin->value()).toStdString();
    config["star_detection_method"] = m_starDetectionMethodCombo->currentData().toString().toStdString();
    config["star_max_stars"] = std::to_string(m_starMaxStarsSpin->value());
    config["star_max_refined_stars"] = std::to_string(m_starMaxRefinedSpin->value());
    config["star_max_eccentricity"] = std::to_string(m_starMaxEccentricitySpin->value());
    config["match_tolerance"] = std::to_string(m_starMatchToleranceSpin->value());
    config["transformation_model"] = m_transformationModelCombo->currentData().toString().toStdString();
    config["out_prefix"] = m_outPrefixEdit->text().toStdString();
    // Bias/Dark stacking settings
    config["bias_dark_stack_method"] = m_biasDarkStackMethodCombo->currentData().toString().toStdString();
    config["bias_dark_rejection"]    = m_biasDarkRejectionCombo->currentData().toString().toStdString();
    config["bias_dark_sigma_low"]    = QString::number(m_biasDarkSigmaLowSpin->value()).toStdString();
    config["bias_dark_sigma_high"]   = QString::number(m_biasDarkSigmaHighSpin->value()).toStdString();
    config["bias_dark_scale_additive"]       = m_biasDarkScaleAdditiveChk->isChecked() ? "true" : "false";
    config["bias_dark_scale_multiplicative"] = m_biasDarkScaleMultiplicativeChk->isChecked() ? "true" : "false";
    // Flat stacking settings
    config["flat_stack_method"]  = m_flatStackMethodCombo->currentData().toString().toStdString();
    config["flat_rejection"]     = m_flatRejectionCombo->currentData().toString().toStdString();
    config["flat_sigma_low"]     = QString::number(m_flatSigmaLowSpin->value()).toStdString();
    config["flat_sigma_high"]    = QString::number(m_flatSigmaHighSpin->value()).toStdString();
    config["flat_scale_additive"]            = m_flatScaleAdditiveChk->isChecked() ? "true" : "false";
    config["flat_scale_multiplicative"]      = m_flatScaleMultiplicativeChk->isChecked() ? "true" : "false";

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
            if (m_openCalibStacksChk->isChecked()) {
                if (auto* mw = qobject_cast<MainWindow*>(mainWin)) {
                    for (const auto& name : m_workspace.elementNames()) {
                        if (name.find("master_bias_") != std::string::npos || name.find("master_dark_") != std::string::npos || name.find("master_flat_") != std::string::npos) {
                            QString qName = QString::fromStdString(name);
                            mw->m_workspaceArea->removeElementView(qName);
                            mw->m_workspaceArea->addElementView(qName, m_workspace.getElement(name));
                        }
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

    m_selectionTable->blockSignals(true);
    for (int i = 0; i < batch->count(); ++i) {
        auto meta = batch->frameMetadata(i);
        bool match = true;
        if (!meta.registered) {
            match = false;
        } else {
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
        }
        
        batch->setFrameSelected(i, match);
        
        if (m_selectionTable->item(i, 0)) {
            QTableWidgetItem* checkItem = m_selectionTable->item(i, 0);
            if (!meta.registered) {
                checkItem->setCheckState(Qt::Unchecked);
                checkItem->setFlags(Qt::ItemIsSelectable);
            } else {
                checkItem->setCheckState(match ? Qt::Checked : Qt::Unchecked);
                checkItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
            }
        }
    }
    m_selectionTable->blockSignals(false);

    notifyBatchViewsUpdated();
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
    std::string alignMethod = m_alignMethodCombo->currentData().toString().toStdString();
    if (alignMethod == "drizzle") {
        config["interpolation_method"] = "drizzle";
        config["drizzle_scale"] = QString::number(m_drizzleScaleSpin->value()).toStdString();
        config["drop_shrink"] = QString::number(m_drizzleDropSizeSpin->value()).toStdString();
    } else {
        config["interpolation_method"] = m_interpolationMethodCombo->currentData().toString().toStdString();
        config["drizzle_scale"] = "1.0";
        config["drop_shrink"] = "1.0";
    }
    config["align_ref_mode"] = m_alignRefModeCombo->currentData().toString().toStdString();
    config["align_mutually"] = m_alignMutuallyChk->isChecked() ? "true" : "false";
    config["transformation_model"] = m_transformationModelCombo->currentData().toString().toStdString();
    config["keep_intermediate"] = m_keepIntermediateChk->isChecked() ? "true" : "false";
    config["out_dir"] = QDir::current().filePath(
        QString::fromStdString(Preferences::instance().getProcessFolderName())).toStdString();
    config["star_min_snr"] = QString::number(m_starMinSnrSpin->value()).toStdString();
    config["star_min_fwhm"] = QString::number(m_starMinFwhmSpin->value()).toStdString();
    config["star_detection_method"] = m_starDetectionMethodCombo->currentData().toString().toStdString();
    config["star_max_stars"] = std::to_string(m_starMaxStarsSpin->value());
    config["star_max_refined_stars"] = std::to_string(m_starMaxRefinedSpin->value());
    config["star_max_eccentricity"] = std::to_string(m_starMaxEccentricitySpin->value());
    config["match_tolerance"] = std::to_string(m_starMatchToleranceSpin->value());
    config["out_prefix"] = m_outPrefixEdit->text().toStdString();
    
    // Background Normalization
    config["run_bge"] = m_runBgeChk->isChecked() ? "true" : "false";
    config["bge_model_method"] = m_bgeMethodCombo->currentData().toString().toStdString();
    config["bge_poly_order"] = std::to_string(m_bgeOrderSpin->value());
    config["bge_grid_cols"] = std::to_string(m_bgeGridColsSpin->value());
    config["bge_grid_rows"] = std::to_string(m_bgeGridRowsSpin->value());
    config["bge_rbf_smoothing"] = QString::number(m_bgeRbfSmoothingSpin->value()).toStdString();
    config["bge_auto_exclude"] = m_bgeAutoExcludeChk->isChecked() ? "true" : "false";
    config["bge_max_deviation"] = QString::number(m_bgeMaxDeviationSpin->value()).toStdString();
    config["bge_max_structure"] = QString::number(m_bgeMaxStructureSpin->value()).toStdString();

    // Light stacking settings
    config["light_stack_method"] = m_lightStackMethodCombo->currentData().toString().toStdString();
    config["light_rejection"]    = m_lightRejectionCombo->currentData().toString().toStdString();
    config["light_sigma_low"]    = QString::number(m_lightSigmaLowSpin->value()).toStdString();
    config["light_sigma_high"]   = QString::number(m_lightSigmaHighSpin->value()).toStdString();
    config["scale_additive"]     = m_lightScaleAdditiveChk->isChecked() ? "true" : "false";
    config["scale_multiplicative"] = m_lightScaleMultiplicativeChk->isChecked() ? "true" : "false";

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
                    std::string baseName = prefix + "preprocessed_lights_" + filter.toStdString() + "_stacked";
                    std::string finalMasterName = baseName;
                    if (m_workspace.contains(baseName)) {
                        int suffix = 1;
                        std::string candidate = baseName + "_" + std::to_string(suffix);
                        while (m_workspace.contains(candidate)) {
                            finalMasterName = candidate;
                            suffix++;
                            candidate = baseName + "_" + std::to_string(suffix);
                        }
                    }
                    if (m_openLightMastersChk->isChecked() && m_workspace.contains(finalMasterName)) {
                        QString qName = QString::fromStdString(finalMasterName);
                        mw->m_workspaceArea->removeElementView(qName);
                        mw->m_workspaceArea->addElementView(qName, m_workspace.getElement(finalMasterName));
                    }
                }
            }
            QMessageBox::information(this, "PPW", "Preprocessing complete! Stacked master image(s) have been loaded into the workspace.");
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
        m_selectionTable->blockSignals(true);
        for (int i = 0; i < batch->count(); ++i) {
            m_selectionTable->insertRow(i);
            
            QTableWidgetItem* checkItem = new QTableWidgetItem();
            bool match = true;
            std::vector<std::string> metrics = {"starCount", "fwhm", "snr"};
            auto meta = batch->frameMetadata(i);
            if (!meta.registered) {
                match = false;
            } else {
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
            }
            if (!meta.registered) {
                checkItem->setCheckState(Qt::Unchecked);
                checkItem->setFlags(Qt::ItemIsSelectable);
            } else {
                checkItem->setCheckState(match ? Qt::Checked : Qt::Unchecked);
                checkItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
            }
            m_selectionTable->setItem(i, 0, checkItem);
            
            QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(batch->frameName(i)));
            nameItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            m_selectionTable->setItem(i, 1, nameItem);
            
            QTableWidgetItem* starCountItem = new QTableWidgetItem(QString::number(meta.starCount));
            starCountItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            m_selectionTable->setItem(i, 2, starCountItem);
            
            QTableWidgetItem* fwhmItem = new QTableWidgetItem(QString::number(meta.fwhm, 'f', 2));
            fwhmItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            m_selectionTable->setItem(i, 3, fwhmItem);
            
            QTableWidgetItem* snrItem = new QTableWidgetItem(QString::number(meta.snr, 'f', 2));
            snrItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            m_selectionTable->setItem(i, 4, snrItem);
        }
        m_selectionTable->blockSignals(false);
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
        if (auto* worker = qobject_cast<PreprocessingWorker*>(m_activeWorker)) {
            worker->cancel();
        }
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

        pipeline.setStepCallback([this](int stepIndex, int percent, double elapsed, bool finished, bool success, bool cached) {
            if (finished) {
                emit stepFinished(stepIndex, success, elapsed, cached);
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
    } else if (stepName.rfind("Star Finding", 0) == 0 ||
               stepName.rfind("Register Lights", 0) == 0) {
        return "Register";
    } else if (stepName.rfind("User Frame Selection", 0) == 0) {
        return "Frame Selection";
    } else if (stepName.rfind("Finalize Registration", 0) == 0) {
        return "Align (Register)";
    } else if (stepName.rfind("Background Normalization", 0) == 0) {
        return "Align (Normalize)";
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
    config["strict_dark"] = m_strictDarkChk->isChecked() ? "true" : "false";
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

    config["stage"] = "align_stack";
    std::string alignMethod = m_alignMethodCombo->currentData().toString().toStdString();
    if (alignMethod == "drizzle") {
        config["interpolation_method"] = "drizzle";
        config["drizzle_scale"] = QString::number(m_drizzleScaleSpin->value()).toStdString();
        config["drop_shrink"] = QString::number(m_drizzleDropSizeSpin->value()).toStdString();
    } else {
        config["interpolation_method"] = m_interpolationMethodCombo->currentData().toString().toStdString();
        config["drizzle_scale"] = "1.0";
        config["drop_shrink"] = "1.0";
    }
    config["align_ref_mode"] = m_alignRefModeCombo->currentData().toString().toStdString();
    config["align_mutually"] = m_alignMutuallyChk->isChecked() ? "true" : "false";
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

void PreprocessingWizardDialog::onStepFinished(int stepIndex, bool success, double elapsed, bool cached) {
    int row = (m_runningStage == "align_stack") ? m_stage1StepCount + 1 + stepIndex : stepIndex;
    if (row < 0 || row >= m_stepsTable->rowCount()) return;
    if (success) {
        if (cached) {
            m_stepsTable->setItem(row, 2, new QTableWidgetItem("Cached"));
            m_stepsTable->item(row, 2)->setForeground(QColor("#9CDCFE"));
            m_stepsTable->setItem(row, 3, new QTableWidgetItem("100%"));
            m_stepsTable->item(row, 3)->setForeground(QColor("#9CDCFE"));
            m_stepsTable->setItem(row, 4, new QTableWidgetItem("0.0s (Cached)"));
            m_stepsTable->item(row, 4)->setForeground(QColor("#808080"));
        } else {
            m_stepsTable->setItem(row, 2, new QTableWidgetItem("Completed"));
            m_stepsTable->item(row, 2)->setForeground(QColor("#4EC9B0"));
            m_stepsTable->setItem(row, 3, new QTableWidgetItem("100%"));
            m_stepsTable->item(row, 3)->setForeground(QColor("#4EC9B0"));
            m_stepsTable->setItem(row, 4, new QTableWidgetItem(QString("%1s").arg(elapsed, 0, 'f', 1)));
            m_stepsTable->item(row, 4)->setForeground(QColor("#a9b7c6"));
        }
    } else {
        m_stepsTable->setItem(row, 2, new QTableWidgetItem("Error"));
        m_stepsTable->item(row, 2)->setForeground(QColor("#F44336"));
        m_stepsTable->setItem(row, 3, new QTableWidgetItem("-"));
        m_stepsTable->item(row, 3)->setForeground(QColor("#F44336"));
        m_stepsTable->setItem(row, 4, new QTableWidgetItem(QString("%1s").arg(elapsed, 0, 'f', 1)));
        m_stepsTable->item(row, 4)->setForeground(QColor("#a9b7c6"));
    }
}

void PreprocessingWizardDialog::onFileDoubleClicked(QTableWidgetItem* item) {
    if (!item) return;
    int row = item->row();
    QTableWidgetItem* pathItem = m_filesTable->item(row, 0);
    if (pathItem) {
        QString filepath = pathItem->data(Qt::UserRole).toString();
        if (filepath.isEmpty()) {
            filepath = pathItem->toolTip();
        }
        openFileAsImage(filepath);
    }
}

void PreprocessingWizardDialog::onTreeItemDoubleClicked(QTreeWidgetItem* item, int column) {
    if (!item) return;
    QString filepath = item->data(0, Qt::UserRole).toString();
    if (filepath.isEmpty()) {
        filepath = item->toolTip(0);
    }
    openFileAsImage(filepath);
}

void PreprocessingWizardDialog::openFileAsImage(const QString& filepath) {
    if (filepath.isEmpty()) return;
    QFileInfo info(filepath);
    if (!info.exists() || !info.isFile()) {
        logMessage(QString("[PPW] Cannot open file: %1 (does not exist or is not a file)").arg(filepath));
        return;
    }
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
        mw->loadImageDirectly(filepath, info.fileName());
    }
}

void PreprocessingWizardDialog::onSelectionCellDoubleClicked(int row, int column) {
    Q_UNUSED(column);
    if (!m_workspace.contains(m_registeredLightsName)) return;
    auto batch = std::get<ImageBatchPtr>(m_workspace.getElement(m_registeredLightsName));
    if (row < 0 || row >= batch->count()) return;

    saveCurrentSelectionStates();

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
        QString batchName = QString::fromStdString(m_registeredLightsName);
        
        QMdiSubWindow* subWin = mw->m_workspaceArea->addElementView(batchName, batch);
        if (subWin) {
            subWin->setFocus();
            if (auto* win = qobject_cast<WorkspaceImageWindow*>(subWin->widget())) {
                if (auto* bw = qobject_cast<BatchImageWidget*>(win->viewportWidget())) {
                    bw->setCurrentIndex(row);
                }
            }
        }
    }
}

void PreprocessingWizardDialog::onSelectionTableItemChanged(QTableWidgetItem* item) {
    if (item->column() != 0) return;
    if (m_registeredLightsName.empty() || !m_workspace.contains(m_registeredLightsName)) return;
    auto batch = std::get<ImageBatchPtr>(m_workspace.getElement(m_registeredLightsName));
    int row = item->row();
    if (row < 0 || row >= batch->count()) return;
    
    bool checked = (item->checkState() == Qt::Checked);
    batch->setFrameSelected(row, checked);
    
    notifyBatchViewsUpdated();
}

void PreprocessingWizardDialog::notifyBatchViewsUpdated() {
    if (m_registeredLightsName.empty() || !m_workspace.contains(m_registeredLightsName)) return;
    auto batch = std::get<ImageBatchPtr>(m_workspace.getElement(m_registeredLightsName));
    QMainWindow* mainWin = nullptr;
    QWidget* w = parentWidget();
    while (w) {
        if (auto* mw = qobject_cast<QMainWindow*>(w)) {
            mainWin = mw;
            break;
        }
        w = w->parentWidget();
    }
    if (mainWin) {
        for (auto* bw : mainWin->findChildren<BatchImageWidget*>()) {
            if (bw->batch() == batch) {
                bw->notifyBatchUpdated();
            }
        }
    }
}

void PreprocessingWizardDialog::changeEvent(QEvent* event) {
    if (event->type() == QEvent::ActivationChange) {
        if (isActiveWindow()) {
            if (!m_registeredLightsName.empty() && m_workspace.contains(m_registeredLightsName)) {
                auto batch = std::get<ImageBatchPtr>(m_workspace.getElement(m_registeredLightsName));
                m_selectionTable->blockSignals(true);
                for (int i = 0; i < batch->count(); ++i) {
                    if (m_selectionTable->item(i, 0)) {
                        bool selected = batch->isFrameSelected(i);
                        m_selectionTable->item(i, 0)->setCheckState(selected ? Qt::Checked : Qt::Unchecked);
                    }
                }
                m_selectionTable->blockSignals(false);
                m_plotWidget->update();
            }
        }
    }
    QDialog::changeEvent(event);
}

void PreprocessingWizardDialog::updateProcessDirLabel() {
    QString folderName = QString::fromStdString(Preferences::instance().getProcessFolderName());
    QString resolved = QDir::current().filePath(folderName);
    m_processDirLabel->setText(resolved);
    m_processDirLabel->setToolTip(resolved);
}

QJsonObject PreprocessingWizardDialog::serializeControlState() const {
    QJsonObject obj;
    obj["out_prefix"] = m_outPrefixEdit->text();
    obj["keep_intermediate"] = m_keepIntermediateChk->isChecked();
    obj["overwrite_masters"] = m_overwriteMastersChk->isChecked();
    obj["open_calib_stacks"] = m_openCalibStacksChk->isChecked();
    obj["open_light_masters"] = m_openLightMastersChk->isChecked();
    // Grouping
    obj["strict_dark"] = m_strictDarkChk->isChecked();
    obj["exp_tolerance"] = m_expToleranceSpin->value();
    obj["debayer"] = m_debayerChk->isChecked();
    obj["bayer_pattern"] = m_bayerPatternCombo->currentText();
    obj["debayer_method"] = m_debayerMethodCombo->currentText();
    // Bias/Dark Stacking
    obj["bias_dark_stack_method"] = m_biasDarkStackMethodCombo->currentData().toString();
    obj["bias_dark_rejection"] = m_biasDarkRejectionCombo->currentData().toString();
    obj["bias_dark_sigma_low"] = m_biasDarkSigmaLowSpin->value();
    obj["bias_dark_sigma_high"] = m_biasDarkSigmaHighSpin->value();
    obj["bias_dark_scale_additive"] = m_biasDarkScaleAdditiveChk->isChecked();
    obj["bias_dark_scale_multiplicative"] = m_biasDarkScaleMultiplicativeChk->isChecked();
    // Flat Stacking
    obj["flat_stack_method"] = m_flatStackMethodCombo->currentData().toString();
    obj["flat_rejection"] = m_flatRejectionCombo->currentData().toString();
    obj["flat_sigma_low"] = m_flatSigmaLowSpin->value();
    obj["flat_sigma_high"] = m_flatSigmaHighSpin->value();
    obj["flat_scale_additive"] = m_flatScaleAdditiveChk->isChecked();
    obj["flat_scale_multiplicative"] = m_flatScaleMultiplicativeChk->isChecked();
    // Registration
    obj["star_min_snr"] = m_starMinSnrSpin->value();
    obj["star_min_fwhm"] = m_starMinFwhmSpin->value();
    obj["star_detection_method"] = m_starDetectionMethodCombo->currentData().toString();
    obj["star_max_stars"] = m_starMaxStarsSpin->value();
    obj["star_max_refined_stars"] = m_starMaxRefinedSpin->value();
    obj["star_max_eccentricity"] = m_starMaxEccentricitySpin->value();
    obj["match_tolerance"] = m_starMatchToleranceSpin->value();
    obj["transformation_model"] = m_transformationModelCombo->currentData().toString();
    // Alignment
    obj["align_method"] = m_alignMethodCombo->currentData().toString();
    obj["align_ref_mode"] = m_alignRefModeCombo->currentData().toString();
    obj["drizzle_scale"] = m_drizzleScaleSpin->value();
    obj["drizzle_drop_size"] = m_drizzleDropSizeSpin->value();
    obj["align_mutually"] = m_alignMutuallyChk->isChecked();
    obj["interpolation_method"] = m_interpolationMethodCombo->currentData().toString();
    // Light Stacking
    obj["light_stack_method"] = m_lightStackMethodCombo->currentData().toString();
    obj["light_rejection"] = m_lightRejectionCombo->currentData().toString();
    obj["light_sigma_low"] = m_lightSigmaLowSpin->value();
    obj["light_sigma_high"] = m_lightSigmaHighSpin->value();
    // Background Normalization
    obj["run_bge"] = m_runBgeChk->isChecked();
    obj["bge_model_method"] = m_bgeMethodCombo->currentData().toString();
    obj["bge_poly_order"] = m_bgeOrderSpin->value();
    obj["bge_grid_cols"] = m_bgeGridColsSpin->value();
    obj["bge_grid_rows"] = m_bgeGridRowsSpin->value();
    obj["bge_rbf_smoothing"] = m_bgeRbfSmoothingSpin->value();
    obj["bge_auto_exclude"] = m_bgeAutoExcludeChk->isChecked();
    obj["bge_max_deviation"] = m_bgeMaxDeviationSpin->value();
    obj["bge_max_structure"] = m_bgeMaxStructureSpin->value();
    // Scaling Normalizations
    obj["scale_additive"] = m_lightScaleAdditiveChk->isChecked();
    obj["scale_multiplicative"] = m_lightScaleMultiplicativeChk->isChecked();
    return obj;
}

void PreprocessingWizardDialog::restoreControlState(const QJsonObject& obj) {
    auto setCombo = [](QComboBox* combo, const QString& data) {
        int idx = combo->findData(data);
        if (idx >= 0) combo->setCurrentIndex(idx);
    };
    if (obj.contains("out_prefix")) m_outPrefixEdit->setText(obj["out_prefix"].toString());
    if (obj.contains("keep_intermediate")) m_keepIntermediateChk->setChecked(obj["keep_intermediate"].toBool());
    if (obj.contains("overwrite_masters")) m_overwriteMastersChk->setChecked(obj["overwrite_masters"].toBool());
    if (obj.contains("open_calib_stacks")) m_openCalibStacksChk->setChecked(obj["open_calib_stacks"].toBool());
    if (obj.contains("open_light_masters")) m_openLightMastersChk->setChecked(obj["open_light_masters"].toBool());
    if (obj.contains("strict_dark")) m_strictDarkChk->setChecked(obj["strict_dark"].toBool());
    if (obj.contains("exp_tolerance")) m_expToleranceSpin->setValue(obj["exp_tolerance"].toDouble());
    if (obj.contains("debayer")) m_debayerChk->setChecked(obj["debayer"].toBool());
    if (obj.contains("bayer_pattern")) {
        int idx = m_bayerPatternCombo->findText(obj["bayer_pattern"].toString());
        if (idx >= 0) m_bayerPatternCombo->setCurrentIndex(idx);
    }
    if (obj.contains("debayer_method")) {
        int idx = m_debayerMethodCombo->findText(obj["debayer_method"].toString());
        if (idx >= 0) m_debayerMethodCombo->setCurrentIndex(idx);
    }
    if (obj.contains("bias_dark_stack_method")) setCombo(m_biasDarkStackMethodCombo, obj["bias_dark_stack_method"].toString());
    if (obj.contains("bias_dark_rejection")) setCombo(m_biasDarkRejectionCombo, obj["bias_dark_rejection"].toString());
    if (obj.contains("bias_dark_sigma_low")) m_biasDarkSigmaLowSpin->setValue(obj["bias_dark_sigma_low"].toDouble());
    if (obj.contains("bias_dark_sigma_high")) m_biasDarkSigmaHighSpin->setValue(obj["bias_dark_sigma_high"].toDouble());
    if (obj.contains("bias_dark_scale_additive")) m_biasDarkScaleAdditiveChk->setChecked(obj["bias_dark_scale_additive"].toBool());
    if (obj.contains("bias_dark_scale_multiplicative")) m_biasDarkScaleMultiplicativeChk->setChecked(obj["bias_dark_scale_multiplicative"].toBool());
    if (obj.contains("flat_stack_method")) setCombo(m_flatStackMethodCombo, obj["flat_stack_method"].toString());
    if (obj.contains("flat_rejection")) setCombo(m_flatRejectionCombo, obj["flat_rejection"].toString());
    if (obj.contains("flat_sigma_low")) m_flatSigmaLowSpin->setValue(obj["flat_sigma_low"].toDouble());
    if (obj.contains("flat_sigma_high")) m_flatSigmaHighSpin->setValue(obj["flat_sigma_high"].toDouble());
    if (obj.contains("flat_scale_additive")) m_flatScaleAdditiveChk->setChecked(obj["flat_scale_additive"].toBool());
    if (obj.contains("flat_scale_multiplicative")) m_flatScaleMultiplicativeChk->setChecked(obj["flat_scale_multiplicative"].toBool());
    if (obj.contains("star_min_snr")) m_starMinSnrSpin->setValue(obj["star_min_snr"].toDouble());
    if (obj.contains("star_min_fwhm")) m_starMinFwhmSpin->setValue(obj["star_min_fwhm"].toDouble());
    if (obj.contains("star_detection_method")) setCombo(m_starDetectionMethodCombo, obj["star_detection_method"].toString());
    if (obj.contains("star_max_stars")) m_starMaxStarsSpin->setValue(obj["star_max_stars"].toInt());
    if (obj.contains("star_max_refined_stars")) m_starMaxRefinedSpin->setValue(obj["star_max_refined_stars"].toInt());
    if (obj.contains("star_max_eccentricity")) m_starMaxEccentricitySpin->setValue(obj["star_max_eccentricity"].toDouble());
    if (obj.contains("match_tolerance")) m_starMatchToleranceSpin->setValue(obj["match_tolerance"].toDouble());
    if (obj.contains("transformation_model")) setCombo(m_transformationModelCombo, obj["transformation_model"].toString());
    if (obj.contains("align_method")) setCombo(m_alignMethodCombo, obj["align_method"].toString());
    if (obj.contains("align_ref_mode")) setCombo(m_alignRefModeCombo, obj["align_ref_mode"].toString());
    if (obj.contains("drizzle_scale")) m_drizzleScaleSpin->setValue(obj["drizzle_scale"].toDouble());
    if (obj.contains("drizzle_drop_size")) m_drizzleDropSizeSpin->setValue(obj["drizzle_drop_size"].toDouble());
    if (obj.contains("align_mutually")) m_alignMutuallyChk->setChecked(obj["align_mutually"].toBool());
    if (obj.contains("interpolation_method")) setCombo(m_interpolationMethodCombo, obj["interpolation_method"].toString());
    onAlignMethodChanged(m_alignMethodCombo->currentIndex());
    if (obj.contains("light_stack_method")) setCombo(m_lightStackMethodCombo, obj["light_stack_method"].toString());
    if (obj.contains("light_rejection")) setCombo(m_lightRejectionCombo, obj["light_rejection"].toString());
    if (obj.contains("light_sigma_low")) m_lightSigmaLowSpin->setValue(obj["light_sigma_low"].toDouble());
    if (obj.contains("light_sigma_high")) m_lightSigmaHighSpin->setValue(obj["light_sigma_high"].toDouble());
    if (obj.contains("run_bge")) m_runBgeChk->setChecked(obj["run_bge"].toBool());
    if (obj.contains("bge_model_method")) setCombo(m_bgeMethodCombo, obj["bge_model_method"].toString());
    if (obj.contains("bge_poly_order")) m_bgeOrderSpin->setValue(obj["bge_poly_order"].toInt());
    if (obj.contains("bge_grid_cols")) m_bgeGridColsSpin->setValue(obj["bge_grid_cols"].toInt());
    if (obj.contains("bge_grid_rows")) m_bgeGridRowsSpin->setValue(obj["bge_grid_rows"].toInt());
    if (obj.contains("bge_rbf_smoothing")) m_bgeRbfSmoothingSpin->setValue(obj["bge_rbf_smoothing"].toDouble());
    if (obj.contains("bge_auto_exclude")) m_bgeAutoExcludeChk->setChecked(obj["bge_auto_exclude"].toBool());
    if (obj.contains("bge_max_deviation")) m_bgeMaxDeviationSpin->setValue(obj["bge_max_deviation"].toDouble());
    if (obj.contains("bge_max_structure")) m_bgeMaxStructureSpin->setValue(obj["bge_max_structure"].toDouble());
    if (obj.contains("scale_additive")) m_lightScaleAdditiveChk->setChecked(obj["scale_additive"].toBool());
    if (obj.contains("scale_multiplicative")) m_lightScaleMultiplicativeChk->setChecked(obj["scale_multiplicative"].toBool());
}

void PreprocessingWizardDialog::onAlignMethodChanged(int index) {
    Q_UNUSED(index);
    QString method = m_alignMethodCombo->currentData().toString();
    if (method == "drizzle") {
        m_interpolationMethodCombo->setEnabled(false);
        m_drizzleScaleSpin->setEnabled(true);
        m_drizzleDropSizeSpin->setEnabled(true);
    } else {
        m_interpolationMethodCombo->setEnabled(true);
        m_drizzleScaleSpin->setEnabled(false);
        m_drizzleDropSizeSpin->setEnabled(false);
    }
}

} // namespace blastro
