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

#include "StretchingDialog.h"
#include "WorkspaceImageWindow.h"
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QComboBox>
#include <QStackedWidget>
#include "../core/MathUtils.h"
#include <omp.h>
#include <QJsonObject>
#include <QJsonArray>
#include "HtWidget.h"
#include "GhsWidget.h"
#include "core/Preferences.h"
#include "algorithms/StretchingAlgorithm.h"
#include "WorkspaceArea.h"
#include "CurvesWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QMainWindow>
#include <QThread>

namespace blastro {

static GrayscaleImagePtr cloneGrayscale(GrayscaleImagePtr src) {
    if (!src) return nullptr;
    int w = src->width();
    int h = src->height();
    auto dstBuf = std::make_shared<ImageBuffer>(w, h);
    std::copy(src->buffer()->data(), src->buffer()->data() + w * h, dstBuf->data());
    return std::make_shared<GrayscaleImage>(dstBuf);
}

static RGBImagePtr cloneRGB(RGBImagePtr src) {
    if (!src) return nullptr;
    auto r = cloneGrayscale(src->r());
    auto g = cloneGrayscale(src->g());
    auto b = cloneGrayscale(src->b());
    return std::make_shared<RGBImage>(r, g, b);
}

StretchingDialog::StretchingDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : AlgorithmDialog(workspace, parent),
      m_tabWidget(nullptr),
      m_htWidget(nullptr),
      m_ghsWidget(nullptr),
      m_bSlider(nullptr),
      m_bSpin(nullptr),
      m_wSlider(nullptr),
      m_wSpin(nullptr),
      m_mSlider(nullptr),
      m_mSpin(nullptr),
      m_spSlider(nullptr),
      m_spSpin(nullptr),
      m_dSlider(nullptr),
      m_dSpin(nullptr),
      m_shadowSlider(nullptr),
      m_shadowSpin(nullptr),
      m_highlightSlider(nullptr),
      m_highlightSpin(nullptr),
      m_previewChk(nullptr),
      m_copyStretchesBtn(nullptr),
      m_previewTimer(new QTimer(this)) {
    
    setWindowTitle("Stretching Transformation");

    m_previewTimer->setSingleShot(true);
    connect(m_previewTimer, &QTimer::timeout, this, &StretchingDialog::updatePreview);

    if (auto mw = qobject_cast<QMainWindow*>(parent)) {
        if (auto wa = mw->findChild<WorkspaceArea*>()) {
            connect(wa, &QMdiArea::subWindowActivated, this, &StretchingDialog::onTargetImageChanged);
        }
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(10);

    // 1. Large Histogram Widget at the top
    m_histogramContainer = new QStackedWidget(this);
    mainLayout->addWidget(m_histogramContainer, 1); // Container absorbs vertical resize space

    m_htWidget = new HtWidget(this);
    m_htWidget->setMinimumHeight(150);
    m_htWidget->setActive(true);
    m_histogramContainer->addWidget(m_htWidget);

    m_ghsWidget = new GhsWidget(this);
    m_ghsWidget->setMinimumHeight(150);
    m_ghsWidget->setActive(false);
    m_histogramContainer->addWidget(m_ghsWidget);

    m_curvesWidget = new CurvesWidget(this);
    m_curvesWidget->setMinimumHeight(150);
    m_curvesWidget->setActive(false);
    m_histogramContainer->addWidget(m_curvesWidget);
    
    connect(m_curvesWidget, &CurvesWidget::curveChanged, this, &StretchingDialog::onCurveChanged);

    connect(m_htWidget, &HtWidget::stretchParamsChanged, this, &StretchingDialog::onHtParamsChanged);
    connect(m_ghsWidget, &GhsWidget::ghsParamsChanged, this, &StretchingDialog::onGhsParamsChanged);
    connect(m_ghsWidget, &GhsWidget::ghsProtectionsChanged, this, &StretchingDialog::onGhsProtectionsChanged);

    // 2. Tab Widget for HT vs GHS mode
    m_tabWidget = new QTabWidget(this);

    // --- Tab 0: Histogram Transformation ---
    QWidget* htTab = new QWidget(this);
    QFormLayout* htForm = new QFormLayout(htTab);
    htForm->setSpacing(8);

    // Blackpoint Slider/SpinBox
    QHBoxLayout* bLayout = new QHBoxLayout();
    m_bSlider = new QSlider(Qt::Horizontal, this);
    m_bSlider->setRange(0, 1000);
    m_bSlider->setValue(0);
    m_bSpin = new QDoubleSpinBox(this);
    m_bSpin->setFixedWidth(80);
    m_bSpin->setRange(0.0, 1.0);
    m_bSpin->setSingleStep(0.001);
    m_bSpin->setDecimals(4);
    m_bSpin->setValue(0.0);
    bLayout->addWidget(m_bSlider, 1);
    bLayout->addWidget(m_bSpin);
    htForm->addRow("Black Point:", bLayout);

    auto getActiveBP = [this]() {
        if (m_activeChannel == ActiveChannel::L) return m_lBlackpoint;
        if (m_activeChannel == ActiveChannel::S) return m_sBlackpoint;
        int idx = static_cast<int>(m_activeChannel) - 1;
        if (idx >= 0 && idx < 3) return m_blackpoint[idx];
        return m_blackpoint[0];
    };
    auto getActiveWP = [this]() {
        if (m_activeChannel == ActiveChannel::L) return m_lWhitepoint;
        if (m_activeChannel == ActiveChannel::S) return m_sWhitepoint;
        int idx = static_cast<int>(m_activeChannel) - 1;
        if (idx >= 0 && idx < 3) return m_whitepoint[idx];
        return m_whitepoint[0];
    };
    auto getActiveRelMP = [this]() {
        if (m_activeChannel == ActiveChannel::L) return m_lMidpoint;
        if (m_activeChannel == ActiveChannel::S) return m_sMidpoint;
        int idx = static_cast<int>(m_activeChannel) - 1;
        if (idx >= 0 && idx < 3) return m_midpoint[idx];
        return m_midpoint[0];
    };
    auto updateActiveBP = [this](double val) {
        if (m_activeChannel == ActiveChannel::L) m_lBlackpoint = val;
        else if (m_activeChannel == ActiveChannel::S) m_sBlackpoint = val;
        else if (m_activeChannel == ActiveChannel::K) m_blackpoint.fill(val);
        else {
            int idx = static_cast<int>(m_activeChannel) - 1;
            m_blackpoint[idx] = val;
        }
    };
    auto updateActiveWP = [this](double val) {
        if (m_activeChannel == ActiveChannel::L) m_lWhitepoint = val;
        else if (m_activeChannel == ActiveChannel::S) m_sWhitepoint = val;
        else if (m_activeChannel == ActiveChannel::K) m_whitepoint.fill(val);
        else {
            int idx = static_cast<int>(m_activeChannel) - 1;
            m_whitepoint[idx] = val;
        }
    };
    auto updateActiveRelMP = [this](double val) {
        if (m_activeChannel == ActiveChannel::L) m_lMidpoint = val;
        else if (m_activeChannel == ActiveChannel::S) m_sMidpoint = val;
        else if (m_activeChannel == ActiveChannel::K) m_midpoint.fill(val);
        else {
            int idx = static_cast<int>(m_activeChannel) - 1;
            m_midpoint[idx] = val;
        }
    };
    auto syncHtSliders = [this, getActiveBP, getActiveWP, getActiveRelMP]() {
        double bp = getActiveBP();
        double wp = getActiveWP();
        double rel_mp = getActiveRelMP();
        double abs_mp = bp + rel_mp * (wp - bp);

        m_bSpin->blockSignals(true);
        m_bSpin->setValue(bp);
        m_bSpin->blockSignals(false);

        m_bSlider->blockSignals(true);
        m_bSlider->setValue(static_cast<int>(bp * 1000.0));
        m_bSlider->blockSignals(false);

        m_wSpin->blockSignals(true);
        m_wSpin->setValue(wp);
        m_wSpin->blockSignals(false);

        m_wSlider->blockSignals(true);
        m_wSlider->setValue(static_cast<int>(wp * 1000.0));
        m_wSlider->blockSignals(false);

        m_mSpin->blockSignals(true);
        m_mSpin->setValue(abs_mp);
        m_mSpin->blockSignals(false);

        m_mSlider->blockSignals(true);
        m_mSlider->setValue(static_cast<int>(abs_mp * 1000.0));
        m_mSlider->blockSignals(false);
    };

    auto onBpChanged = [this, getActiveWP, updateActiveBP, syncHtSliders](double val) {
        double wp = getActiveWP();
        double clamped = std::clamp(val, 0.0, wp - 0.001);
        updateActiveBP(clamped);
        syncHtSliders();
        onParameterChanged();
    };
    connect(m_bSlider, &QSlider::valueChanged, this, [onBpChanged](int val) {
        onBpChanged(val / 1000.0);
    });
    connect(m_bSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [onBpChanged](double val) {
        onBpChanged(val);
    });

    // Midpoint Slider/SpinBox
    QHBoxLayout* mLayout = new QHBoxLayout();
    m_mSlider = new QSlider(Qt::Horizontal, this);
    m_mSlider->setRange(0, 1000);
    m_mSlider->setValue(500);
    m_mSpin = new QDoubleSpinBox(this);
    m_mSpin->setFixedWidth(80);
    m_mSpin->setRange(0.0, 1.0);
    m_mSpin->setSingleStep(0.01);
    m_mSpin->setDecimals(4);
    m_mSpin->setValue(0.5);
    mLayout->addWidget(m_mSlider, 1);
    mLayout->addWidget(m_mSpin);
    htForm->addRow("Mid Point (Gamma):", mLayout);

    auto onMpChanged = [this, getActiveBP, getActiveWP, updateActiveRelMP, syncHtSliders](double val) {
        double bp = getActiveBP();
        double wp = getActiveWP();
        double clamped = std::clamp(val, bp + 0.0001, wp - 0.0001);
        double rel_mp = 0.5;
        if (wp > bp) {
            rel_mp = (clamped - bp) / (wp - bp);
        }
        updateActiveRelMP(std::clamp(rel_mp, 0.0001, 0.9999));
        syncHtSliders();
        onParameterChanged();
    };
    connect(m_mSlider, &QSlider::valueChanged, this, [onMpChanged](int val) {
        onMpChanged(val / 1000.0);
    });
    connect(m_mSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [onMpChanged](double val) {
        onMpChanged(val);
    });

    // Whitepoint Slider/SpinBox
    QHBoxLayout* wLayout = new QHBoxLayout();
    m_wSlider = new QSlider(Qt::Horizontal, this);
    m_wSlider->setRange(0, 1000);
    m_wSlider->setValue(1000);
    m_wSpin = new QDoubleSpinBox(this);
    m_wSpin->setFixedWidth(80);
    m_wSpin->setRange(0.0, 1.0);
    m_wSpin->setSingleStep(0.001);
    m_wSpin->setDecimals(4);
    m_wSpin->setValue(1.0);
    wLayout->addWidget(m_wSlider, 1);
    wLayout->addWidget(m_wSpin);
    htForm->addRow("White Point:", wLayout);

    auto onWpChanged = [this, getActiveBP, updateActiveWP, syncHtSliders](double val) {
        double bp = getActiveBP();
        double clamped = std::clamp(val, bp + 0.001, 1.0);
        updateActiveWP(clamped);
        syncHtSliders();
        onParameterChanged();
    };
    connect(m_wSlider, &QSlider::valueChanged, this, [onWpChanged](int val) {
        onWpChanged(val / 1000.0);
    });
    connect(m_wSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [onWpChanged](double val) {
        onWpChanged(val);
    });

    m_tabWidget->addTab(htTab, "Histogram Transform");

    // --- Tab 1: Generalized Hyperbolic (GHS) ---
    QWidget* ghsTab = new QWidget(this);
    QFormLayout* ghsForm = new QFormLayout(ghsTab);
    ghsForm->setSpacing(8);

    auto setParam = [this](std::array<double, 3>& rgb, double& l, double& s, double val) {
        if (m_activeChannel == ActiveChannel::L) l = val;
        else if (m_activeChannel == ActiveChannel::S) s = val;
        else if (m_activeChannel == ActiveChannel::R) rgb[0] = val;
        else if (m_activeChannel == ActiveChannel::G) rgb[1] = val;
        else if (m_activeChannel == ActiveChannel::B) rgb[2] = val;
        else rgb.fill(val);
    };

    // Symmetry Point SP Slider/SpinBox
    QHBoxLayout* spLayout = new QHBoxLayout();
    m_spSlider = new QSlider(Qt::Horizontal, this);
    m_spSlider->setRange(0, 1000);
    m_spSlider->setValue(500);
    m_spSpin = new QDoubleSpinBox(this);
    m_spSpin->setFixedWidth(80);
    m_spSpin->setRange(0.0, 1.0);
    m_spSpin->setSingleStep(0.001);
    m_spSpin->setDecimals(4);
    m_spSpin->setValue(0.5);
    spLayout->addWidget(m_spSlider, 1);
    spLayout->addWidget(m_spSpin);
    ghsForm->addRow("Symmetry Point (SP):", spLayout);

    connect(m_spSlider, &QSlider::valueChanged, this, [this, setParam](int val) {
        m_spSpin->blockSignals(true);
        m_spSpin->setValue(val / 1000.0);
        m_spSpin->blockSignals(false);
        setParam(m_spPoint, m_lSpPoint, m_sSpPoint, val / 1000.0);
        enforceGhsConstraints(ConstraintSource::SymmetryPoint);
        onParameterChanged();
    });
    connect(m_spSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, setParam](double val) {
        m_spSlider->blockSignals(true);
        m_spSlider->setValue(static_cast<int>(val * 1000.0));
        m_spSlider->blockSignals(false);
        setParam(m_spPoint, m_lSpPoint, m_sSpPoint, val);
        enforceGhsConstraints(ConstraintSource::SymmetryPoint);
        onParameterChanged();
    });

    // Stretch Factor D Slider/SpinBox
    QHBoxLayout* dLayout = new QHBoxLayout();
    m_dSlider = new QSlider(Qt::Horizontal, this);
    m_dSlider->setRange(0, 150);
    m_dSlider->setValue(0);
    m_dSpin = new QDoubleSpinBox(this);
    m_dSpin->setFixedWidth(80);
    m_dSpin->setRange(0.0, 15.0);
    m_dSpin->setSingleStep(0.1);
    m_dSpin->setDecimals(2);
    m_dSpin->setValue(0.0);
    dLayout->addWidget(m_dSlider, 1);
    dLayout->addWidget(m_dSpin);
    ghsForm->addRow("Stretch Factor (D):", dLayout);

    connect(m_dSlider, &QSlider::valueChanged, this, [this, setParam](int val) {
        m_dSpin->blockSignals(true);
        m_dSpin->setValue(val / 10.0);
        m_dSpin->blockSignals(false);
        setParam(m_stretchFactor, m_lStretchFactor, m_sStretchFactor, val / 10.0);
        onParameterChanged();
    });
    connect(m_dSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, setParam](double val) {
        m_dSlider->blockSignals(true);
        m_dSlider->setValue(static_cast<int>(val * 10.0));
        m_dSlider->blockSignals(false);
        setParam(m_stretchFactor, m_lStretchFactor, m_sStretchFactor, val);
        onParameterChanged();
    });

    // Shadow Protection Slider/SpinBox
    QHBoxLayout* shadowLayout = new QHBoxLayout();
    m_shadowSlider = new QSlider(Qt::Horizontal, this);
    m_shadowSlider->setRange(0, 100);
    m_shadowSlider->setValue(0);
    m_shadowSpin = new QDoubleSpinBox(this);
    m_shadowSpin->setFixedWidth(80);
    m_shadowSpin->setRange(0.0, 1.0);
    m_shadowSpin->setSingleStep(0.05);
    m_shadowSpin->setDecimals(2);
    m_shadowSpin->setValue(0.0);
    shadowLayout->addWidget(m_shadowSlider, 1);
    shadowLayout->addWidget(m_shadowSpin);
    ghsForm->addRow("Shadow Protection:", shadowLayout);

    connect(m_shadowSlider, &QSlider::valueChanged, this, [this, setParam](int val) {
        m_shadowSpin->blockSignals(true);
        m_shadowSpin->setValue(val / 100.0);
        m_shadowSpin->blockSignals(false);
        setParam(m_shadowProtect, m_lShadowProtect, m_sShadowProtect, val / 100.0);
        enforceGhsConstraints(ConstraintSource::ShadowProtect);
        onParameterChanged();
    });
    connect(m_shadowSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, setParam](double val) {
        m_shadowSlider->blockSignals(true);
        m_shadowSlider->setValue(static_cast<int>(val * 100.0));
        m_shadowSlider->blockSignals(false);
        setParam(m_shadowProtect, m_lShadowProtect, m_sShadowProtect, val);
        enforceGhsConstraints(ConstraintSource::ShadowProtect);
        onParameterChanged();
    });

    // Highlight Protection Slider/SpinBox
    QHBoxLayout* highlightLayout = new QHBoxLayout();
    m_highlightSlider = new QSlider(Qt::Horizontal, this);
    m_highlightSlider->setRange(0, 100);
    m_highlightSlider->setValue(100);
    m_highlightSpin = new QDoubleSpinBox(this);
    m_highlightSpin->setFixedWidth(80);
    m_highlightSpin->setRange(0.0, 1.0);
    m_highlightSpin->setSingleStep(0.05);
    m_highlightSpin->setDecimals(2);
    m_highlightSpin->setValue(1.0);
    highlightLayout->addWidget(m_highlightSlider, 1);
    highlightLayout->addWidget(m_highlightSpin);
    ghsForm->addRow("Highlight Protection:", highlightLayout);

    connect(m_highlightSlider, &QSlider::valueChanged, this, [this, setParam](int val) {
        m_highlightSpin->blockSignals(true);
        m_highlightSpin->setValue(val / 100.0);
        m_highlightSpin->blockSignals(false);
        setParam(m_highlightProtect, m_lHighlightProtect, m_sHighlightProtect, val / 100.0);
        enforceGhsConstraints(ConstraintSource::HighlightProtect);
        onParameterChanged();
    });
    connect(m_highlightSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, setParam](double val) {
        m_highlightSlider->blockSignals(true);
        m_highlightSlider->setValue(static_cast<int>(val * 100.0));
        m_highlightSlider->blockSignals(false);
        setParam(m_highlightProtect, m_lHighlightProtect, m_sHighlightProtect, val);
        enforceGhsConstraints(ConstraintSource::HighlightProtect);
        onParameterChanged();
    });

    m_tabWidget->addTab(ghsTab, "Generalized Hyperbolic (GHS)");

    // --- Tab 2: Curves ---
    QWidget* curvesTab = new QWidget(this);
    QVBoxLayout* curvesLayout = new QVBoxLayout(curvesTab);
    QLabel* curvesLbl = new QLabel("Curves tool. Edit directly on the histogram view above.", this);
    curvesLbl->setAlignment(Qt::AlignCenter);
    curvesLayout->addWidget(curvesLbl);
    m_tabWidget->addTab(curvesTab, "Curves");

    mainLayout->addWidget(m_tabWidget, 0); // Controls tab stays compact

    // 3. General control box
    QHBoxLayout* ctrlLayout = new QHBoxLayout();
    ctrlLayout->setSpacing(0);
    m_copyStretchesBtn = new QPushButton("Copy Live Stretch", this);
    connect(m_copyStretchesBtn, &QPushButton::clicked, this, &StretchingDialog::onCopyLiveStretch);
    ctrlLayout->addWidget(m_copyStretchesBtn);
    
    ctrlLayout->addStretch(1);
    
    m_channelGroup = new QButtonGroup(this);
    m_channelGroup->setExclusive(true);
    QStringList chNames = {"K", "R", "G", "B", "L", "S"};
    for (int i = 0; i < chNames.size(); ++i) {
        QPushButton* btn = new QPushButton(chNames[i], this);
        btn->setCheckable(true);
        btn->setFixedWidth(35);
        
        QString highlightColor = "#007acc";
        if (i == 0) highlightColor = "#666666"; // RGB/K (Neutral Gray)
        else if (i == 1) highlightColor = "#cc3333"; // R (Red)
        else if (i == 2) highlightColor = "#33aa33"; // G (Green)
        else if (i == 3) highlightColor = "#3366cc"; // B (Blue)
        else if (i == 4) highlightColor = "#c4a000"; // L (Yellow/Gold)
        else if (i == 5) highlightColor = "#75507b"; // S (Purple)

        QString borderRadiusStr = "";
        QString borderRightStr = "";
        QString marginStr = "margin: 0px;";
        
        if (i == 0) { // RGB
            borderRadiusStr = "border-top-left-radius: 4px; border-bottom-left-radius: 4px;";
            borderRightStr = "border-right: none;";
        } else if (i == 1 || i == 2) { // R, G
            borderRadiusStr = "border-radius: 0px;";
            borderRightStr = "border-right: none;";
        } else if (i == 3) { // B
            borderRadiusStr = "border-top-right-radius: 4px; border-bottom-right-radius: 4px;";
        } else if (i == 4 || i == 5) { // L, S
            borderRadiusStr = "border-radius: 4px;";
            marginStr = "margin: 0px; margin-left: 6px;";
        }

        QString segmentedStyle = QString(
            "QPushButton { background-color: #333; border: 1px solid #444; %2 %3 %4 color: #ccc; font-weight: bold; padding: 4px 0px; }"
            "QPushButton:hover { background-color: #444; }"
            "QPushButton:checked { background-color: %1; color: white; border-color: %1; }"
        ).arg(highlightColor, borderRadiusStr, borderRightStr, marginStr);

        btn->setStyleSheet(segmentedStyle);
        if (i == 0) btn->setChecked(true);
        m_channelGroup->addButton(btn, i);
        ctrlLayout->addWidget(btn);
    }
    connect(m_channelGroup, &QButtonGroup::idClicked, this, &StretchingDialog::onChannelChanged);

    QPushButton* resetBtn = new QPushButton("✖", this);
    resetBtn->setToolTip("Reset Current Stretch");
    resetBtn->setStyleSheet(
        "QPushButton { background: transparent; border: none; color: #ff4444; font-weight: bold; font-size: 16px; margin-left: 5px; padding-bottom: 2px; }"
        "QPushButton:hover { color: #ff0000; }"
    );
    connect(resetBtn, &QPushButton::clicked, this, &StretchingDialog::onResetStretchClicked);
    ctrlLayout->addWidget(resetBtn);

    ctrlLayout->addStretch(1);

    m_previewChk = new QCheckBox("Live Preview", this);
    m_previewChk->setChecked(false);
    connect(m_previewChk, &QCheckBox::stateChanged, this, &StretchingDialog::onParameterChanged);
    ctrlLayout->addWidget(m_previewChk);

    mainLayout->addLayout(ctrlLayout);

    // 4. Dialog Action Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* prefsBtn = new QPushButton("Preferences...", this);
    connect(prefsBtn, &QPushButton::clicked, this, &StretchingDialog::onPrefsClicked);
    btnLayout->addWidget(prefsBtn);

    btnLayout->addStretch(1);

    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &AlgorithmDialog::onClose);
    btnLayout->addWidget(closeBtn);

    QPushButton* applyBtn = new QPushButton("Apply", this);
    applyBtn->setObjectName("primaryButton");
    connect(applyBtn, &QPushButton::clicked, this, &StretchingDialog::onApplyClicked);
    btnLayout->addWidget(applyBtn);

    mainLayout->addLayout(btnLayout);

    connect(m_tabWidget, &QTabWidget::currentChanged, this, &StretchingDialog::onTabChanged);

    // Load active image histogram into widget
    refreshHistogramAndCache();

    syncUiFromValues();
}

StretchingDialog::~StretchingDialog() {
    clearPreview();
}

void StretchingDialog::closeEvent(QCloseEvent* event) {
    clearPreview();
    QWidget::closeEvent(event);
}


void StretchingDialog::onTabChanged(int index) {
    if (index == 0) m_mode = StretchMode::HT;
    else if (index == 1) m_mode = StretchMode::GHS;
    else m_mode = StretchMode::Curves;

    if (m_htWidget) m_htWidget->setActive(m_mode == StretchMode::HT);
    if (m_ghsWidget) m_ghsWidget->setActive(m_mode == StretchMode::GHS);
    if (m_curvesWidget) m_curvesWidget->setActive(m_mode == StretchMode::Curves);

    if (m_histogramContainer) {
        if (m_mode == StretchMode::Curves) {
            m_histogramContainer->setCurrentWidget(m_curvesWidget);
        } else if (m_mode == StretchMode::GHS) {
            m_histogramContainer->setCurrentWidget(m_ghsWidget);
        } else {
            m_histogramContainer->setCurrentWidget(m_htWidget);
        }
    }
    
    syncUiFromValues();
    // Turn off live preview on tab switch — tab navigation should always be instant.
    if (m_previewChk && m_previewChk->isChecked()) {
        m_previewChk->blockSignals(true);
        m_previewChk->setChecked(false);
        m_previewChk->blockSignals(false);
        clearPreview();
    }
}

void StretchingDialog::onChannelChanged(int id) {
    m_activeChannel = static_cast<ActiveChannel>(id);
    if (id == 0) {
        m_channelsLinked = true;
    } else if (id == 1 || id == 2 || id == 3) {
        m_channelsLinked = false;
    }

    if (m_htWidget) m_htWidget->setActiveChannel(id);
    if (m_ghsWidget) m_ghsWidget->setActiveChannel(id);
    if (m_curvesWidget) m_curvesWidget->setActiveChannel(id);
    
    QColor color = QColor("#ffffff");
    if (m_activeChannel == ActiveChannel::L) color = QColor("#c4a000");
    else if (m_activeChannel == ActiveChannel::S) color = QColor("#75507b");
    if (m_htWidget) m_htWidget->setSingleChannelColor(color);
    if (m_ghsWidget) m_ghsWidget->setSingleChannelColor(color);
    if (m_curvesWidget) m_curvesWidget->setSingleTraceColor(color);
    
    refreshHistogramAndCache(); // Fetch/update correct histogram for the channel
    syncUiFromValues();
}

void StretchingDialog::onHtParamsChanged(const std::array<double, 6>& b_arr, const std::array<double, 6>& w_arr, const std::array<double, 6>& m_arr) {
    if (m_activeChannel == ActiveChannel::L) {
        m_lBlackpoint = b_arr[4];
        m_lWhitepoint = w_arr[4];
        m_lMidpoint = m_arr[4];
    } else if (m_activeChannel == ActiveChannel::S) {
        m_sBlackpoint = b_arr[5];
        m_sWhitepoint = w_arr[5];
        m_sMidpoint = m_arr[5];
    } else {
        if (m_activeChannel == ActiveChannel::K) {
            m_blackpoint.fill(b_arr[0]);
            m_whitepoint.fill(w_arr[0]);
            m_midpoint.fill(m_arr[0]);
        } else {
            m_blackpoint[0] = b_arr[1];
            m_blackpoint[1] = b_arr[2];
            m_blackpoint[2] = b_arr[3];
            m_whitepoint[0] = w_arr[1];
            m_whitepoint[1] = w_arr[2];
            m_whitepoint[2] = w_arr[3];
            m_midpoint[0] = m_arr[1];
            m_midpoint[1] = m_arr[2];
            m_midpoint[2] = m_arr[3];
        }
    }

    double b = 0.0, w = 1.0, m = 0.5;
    if (m_activeChannel == ActiveChannel::L) { b = m_lBlackpoint; w = m_lWhitepoint; m = m_lMidpoint; }
    else if (m_activeChannel == ActiveChannel::S) { b = m_sBlackpoint; w = m_sWhitepoint; m = m_sMidpoint; }
    else if (m_activeChannel == ActiveChannel::K) { b = m_blackpoint[0]; w = m_whitepoint[0]; m = m_midpoint[0]; }
    else if (m_activeChannel == ActiveChannel::R) { b = m_blackpoint[0]; w = m_whitepoint[0]; m = m_midpoint[0]; }
    else if (m_activeChannel == ActiveChannel::G) { b = m_blackpoint[1]; w = m_whitepoint[1]; m = m_midpoint[1]; }
    else if (m_activeChannel == ActiveChannel::B) { b = m_blackpoint[2]; w = m_whitepoint[2]; m = m_midpoint[2]; }

    if (m_bSpin) {
        m_bSpin->blockSignals(true);
        m_bSpin->setValue(b);
        m_bSpin->blockSignals(false);
    }
    if (m_bSlider) {
        m_bSlider->blockSignals(true);
        m_bSlider->setValue(static_cast<int>(b * 1000.0));
        m_bSlider->blockSignals(false);
    }

    if (m_wSpin) {
        m_wSpin->blockSignals(true);
        m_wSpin->setValue(w);
        m_wSpin->blockSignals(false);
    }
    if (m_wSlider) {
        m_wSlider->blockSignals(true);
        m_wSlider->setValue(static_cast<int>(w * 1000.0));
        m_wSlider->blockSignals(false);
    }

    double abs_mp = b + m * (w - b);
    if (m_mSpin) {
        m_mSpin->blockSignals(true);
        m_mSpin->setValue(abs_mp);
        m_mSpin->blockSignals(false);
    }
    if (m_mSlider) {
        m_mSlider->blockSignals(true);
        m_mSlider->setValue(static_cast<int>(abs_mp * 1000.0));
        m_mSlider->blockSignals(false);
    }

    onParameterChanged();
}

void StretchingDialog::onGhsParamsChanged(const std::array<double, 6>& sp_arr, const std::array<double, 6>& d_arr) {
    if (m_activeChannel == ActiveChannel::L) {
        m_lSpPoint = sp_arr[4];
        m_lStretchFactor = d_arr[4];
    } else if (m_activeChannel == ActiveChannel::S) {
        m_sSpPoint = sp_arr[5];
        m_sStretchFactor = d_arr[5];
    } else {
        if (m_activeChannel == ActiveChannel::K) {
            m_spPoint.fill(sp_arr[0]);
            m_stretchFactor.fill(d_arr[0]);
        } else {
            m_spPoint[0] = sp_arr[1]; m_stretchFactor[0] = d_arr[1];
            m_spPoint[1] = sp_arr[2]; m_stretchFactor[1] = d_arr[2];
            m_spPoint[2] = sp_arr[3]; m_stretchFactor[2] = d_arr[3];
        }
    }

    double sp = 0.5, d = 0.0;
    if (m_activeChannel == ActiveChannel::L) { sp = m_lSpPoint; d = m_lStretchFactor; }
    else if (m_activeChannel == ActiveChannel::S) { sp = m_sSpPoint; d = m_sStretchFactor; }
    else if (m_activeChannel == ActiveChannel::K) { sp = m_spPoint[0]; d = m_stretchFactor[0]; }
    else if (m_activeChannel == ActiveChannel::R) { sp = m_spPoint[0]; d = m_stretchFactor[0]; }
    else if (m_activeChannel == ActiveChannel::G) { sp = m_spPoint[1]; d = m_stretchFactor[1]; }
    else if (m_activeChannel == ActiveChannel::B) { sp = m_spPoint[2]; d = m_stretchFactor[2]; }

    if (m_spSpin) {
        m_spSpin->blockSignals(true);
        m_spSpin->setValue(sp);
        m_spSpin->blockSignals(false);
    }
    if (m_spSlider) {
        m_spSlider->blockSignals(true);
        m_spSlider->setValue(static_cast<int>(sp * 1000.0));
        m_spSlider->blockSignals(false);
    }

    if (m_dSpin) {
        m_dSpin->blockSignals(true);
        m_dSpin->setValue(d);
        m_dSpin->blockSignals(false);
    }
    if (m_dSlider) {
        m_dSlider->blockSignals(true);
        m_dSlider->setValue(static_cast<int>(d * 10.0));
        m_dSlider->blockSignals(false);
    }

    enforceGhsConstraints(ConstraintSource::SymmetryPoint);
    onParameterChanged();
}

void StretchingDialog::onGhsProtectionsChanged(const std::array<double, 6>& shadowProtect_arr, const std::array<double, 6>& highlightProtect_arr) {
    if (m_activeChannel == ActiveChannel::L) {
        m_lShadowProtect = shadowProtect_arr[4];
        m_lHighlightProtect = highlightProtect_arr[4];
    } else if (m_activeChannel == ActiveChannel::S) {
        m_sShadowProtect = shadowProtect_arr[5];
        m_sHighlightProtect = highlightProtect_arr[5];
    } else {
        if (m_activeChannel == ActiveChannel::K) {
            m_shadowProtect.fill(shadowProtect_arr[0]);
            m_highlightProtect.fill(highlightProtect_arr[0]);
        } else {
            m_shadowProtect[0] = shadowProtect_arr[1]; m_highlightProtect[0] = highlightProtect_arr[1];
            m_shadowProtect[1] = shadowProtect_arr[2]; m_highlightProtect[1] = highlightProtect_arr[2];
            m_shadowProtect[2] = shadowProtect_arr[3]; m_highlightProtect[2] = highlightProtect_arr[3];
        }
    }

    double shadowProtect = 0.0, highlightProtect = 1.0;
    if (m_activeChannel == ActiveChannel::L) { shadowProtect = m_lShadowProtect; highlightProtect = m_lHighlightProtect; }
    else if (m_activeChannel == ActiveChannel::S) { shadowProtect = m_sShadowProtect; highlightProtect = m_sHighlightProtect; }
    else if (m_activeChannel == ActiveChannel::K) { shadowProtect = m_shadowProtect[0]; highlightProtect = m_highlightProtect[0]; }
    else if (m_activeChannel == ActiveChannel::R) { shadowProtect = m_shadowProtect[0]; highlightProtect = m_highlightProtect[0]; }
    else if (m_activeChannel == ActiveChannel::G) { shadowProtect = m_shadowProtect[1]; highlightProtect = m_highlightProtect[1]; }
    else if (m_activeChannel == ActiveChannel::B) { shadowProtect = m_shadowProtect[2]; highlightProtect = m_highlightProtect[2]; }

    if (m_shadowSpin) {
        m_shadowSpin->blockSignals(true);
        m_shadowSpin->setValue(shadowProtect);
        m_shadowSpin->blockSignals(false);
    }
    if (m_shadowSlider) {
        m_shadowSlider->blockSignals(true);
        m_shadowSlider->setValue(static_cast<int>(shadowProtect * 100.0));
        m_shadowSlider->blockSignals(false);
    }

    if (m_highlightSpin) {
        m_highlightSpin->blockSignals(true);
        m_highlightSpin->setValue(highlightProtect);
        m_highlightSpin->blockSignals(false);
    }
    if (m_highlightSlider) {
        m_highlightSlider->blockSignals(true);
        m_highlightSlider->setValue(static_cast<int>(highlightProtect * 100.0));
        m_highlightSlider->blockSignals(false);
    }

    enforceGhsConstraints(ConstraintSource::ShadowProtect);
    enforceGhsConstraints(ConstraintSource::HighlightProtect);
    onParameterChanged();
}

void StretchingDialog::onCopyLiveStretch() {
    auto win = getActiveImageWindow();
    if (win && win->imageView() && m_htWidget && m_tabWidget) {
        m_mode = StretchMode::HT;
        m_tabWidget->setCurrentIndex(0);
        m_blackpoint = win->imageView()->blackpoints();
        m_whitepoint = win->imageView()->whitepoints();
        m_midpoint = win->imageView()->midpoints();

        if (win->imageView()->channelsLinked()) {
            m_channelGroup->button(static_cast<int>(ActiveChannel::K))->setChecked(true);
            onChannelChanged(static_cast<int>(ActiveChannel::K));
        } else {
            m_channelGroup->button(static_cast<int>(ActiveChannel::G))->setChecked(true);
            onChannelChanged(static_cast<int>(ActiveChannel::G));
        }
        
        m_tabWidget->setCurrentIndex(0); // HT Tab
        m_mode = StretchMode::HT;
        if (m_htWidget) m_htWidget->setActive(true);
        if (m_ghsWidget) m_ghsWidget->setActive(false);
        if (m_curvesWidget) m_curvesWidget->setActive(false);

        syncUiFromValues();
        onParameterChanged();
    }
}


void StretchingDialog::onResetStretchClicked() {
    if (m_mode == StretchMode::HT) {
        if (m_activeChannel == ActiveChannel::L) {
            m_lBlackpoint = 0.0; m_lWhitepoint = 1.0; m_lMidpoint = 0.5;
        } else if (m_activeChannel == ActiveChannel::S) {
            m_sBlackpoint = 0.0; m_sWhitepoint = 1.0; m_sMidpoint = 0.5;
        } else if (m_activeChannel == ActiveChannel::K) {
            m_blackpoint.fill(0.0); m_whitepoint.fill(1.0); m_midpoint.fill(0.5);
        } else {
            int idx = static_cast<int>(m_activeChannel) - 1;
            if (idx >= 0 && idx < 3) {
                m_blackpoint[idx] = 0.0; m_whitepoint[idx] = 1.0; m_midpoint[idx] = 0.5;
            }
        }
    } else if (m_mode == StretchMode::GHS) {
        if (m_activeChannel == ActiveChannel::L) {
            m_lSpPoint = 0.5; m_lStretchFactor = 0.0; m_lShadowProtect = 0.0; m_lHighlightProtect = 1.0;
        } else if (m_activeChannel == ActiveChannel::S) {
            m_sSpPoint = 0.5; m_sStretchFactor = 0.0; m_sShadowProtect = 0.0; m_sHighlightProtect = 1.0;
        } else if (m_activeChannel == ActiveChannel::K) {
            m_spPoint.fill(0.5); m_stretchFactor.fill(0.0); m_shadowProtect.fill(0.0); m_highlightProtect.fill(1.0);
        } else {
            int idx = static_cast<int>(m_activeChannel) - 1;
            if (idx >= 0 && idx < 3) {
                m_spPoint[idx] = 0.5; m_stretchFactor[idx] = 0.0; m_shadowProtect[idx] = 0.0; m_highlightProtect[idx] = 1.0;
            }
        }
    } else if (m_mode == StretchMode::Curves) {
        int channelIdx = static_cast<int>(m_activeChannel);
        std::vector<QPointF> defaultCurve = { QPointF(0.0, 0.0), QPointF(1.0, 1.0) };
        m_curvePoints[channelIdx] = defaultCurve;
        if (m_curvesWidget) {
            m_curvesWidget->setCurvePoints(channelIdx, defaultCurve);
        }
    }
    
    syncUiFromValues();
    onParameterChanged();
}

void StretchingDialog::onParameterChanged() {
    if (!m_htWidget || !m_ghsWidget) return;

    std::array<double, 6> bp_arr = {
        m_channelsLinked ? m_blackpoint[0] : 0.0,
        m_channelsLinked ? 0.0 : m_blackpoint[0],
        m_channelsLinked ? 0.0 : m_blackpoint[1],
        m_channelsLinked ? 0.0 : m_blackpoint[2],
        m_lBlackpoint,
        m_sBlackpoint
    };
    std::array<double, 6> wp_arr = {
        m_channelsLinked ? m_whitepoint[0] : 1.0,
        m_channelsLinked ? 1.0 : m_whitepoint[0],
        m_channelsLinked ? 1.0 : m_whitepoint[1],
        m_channelsLinked ? 1.0 : m_whitepoint[2],
        m_lWhitepoint,
        m_sWhitepoint
    };
    std::array<double, 6> mp_arr = {
        m_channelsLinked ? m_midpoint[0] : 0.5,
        m_channelsLinked ? 0.5 : m_midpoint[0],
        m_channelsLinked ? 0.5 : m_midpoint[1],
        m_channelsLinked ? 0.5 : m_midpoint[2],
        m_lMidpoint,
        m_sMidpoint
    };

    std::array<double, 6> sp_arr = {
        m_channelsLinked ? m_spPoint[0] : 0.5,
        m_channelsLinked ? 0.5 : m_spPoint[0],
        m_channelsLinked ? 0.5 : m_spPoint[1],
        m_channelsLinked ? 0.5 : m_spPoint[2],
        m_lSpPoint,
        m_sSpPoint
    };
    std::array<double, 6> dp_arr = {
        m_channelsLinked ? m_stretchFactor[0] : 0.0,
        m_channelsLinked ? 0.0 : m_stretchFactor[0],
        m_channelsLinked ? 0.0 : m_stretchFactor[1],
        m_channelsLinked ? 0.0 : m_stretchFactor[2],
        m_lStretchFactor,
        m_sStretchFactor
    };
    std::array<double, 6> sPp_arr = {
        m_channelsLinked ? m_shadowProtect[0] : 0.0,
        m_channelsLinked ? 0.0 : m_shadowProtect[0],
        m_channelsLinked ? 0.0 : m_shadowProtect[1],
        m_channelsLinked ? 0.0 : m_shadowProtect[2],
        m_lShadowProtect,
        m_sShadowProtect
    };
    std::array<double, 6> hPp_arr = {
        m_channelsLinked ? m_highlightProtect[0] : 1.0,
        m_channelsLinked ? 1.0 : m_highlightProtect[0],
        m_channelsLinked ? 1.0 : m_highlightProtect[1],
        m_channelsLinked ? 1.0 : m_highlightProtect[2],
        m_lHighlightProtect,
        m_sHighlightProtect
    };

    if (m_htWidget) {
        m_htWidget->setStretchParams(bp_arr, wp_arr, mp_arr);
        m_htWidget->setChannelsLinked(m_channelsLinked);
    }
    if (m_ghsWidget) {
        m_ghsWidget->setGhsParams(sp_arr, dp_arr);
        m_ghsWidget->setGhsProtections(sPp_arr, hPp_arr);
        m_ghsWidget->setChannelsLinked(m_channelsLinked);
    }
    if (m_curvesWidget) {
        m_curvesWidget->setChannelsLinked(m_channelsLinked);
    }

    if (m_previewChk && m_previewChk->isChecked()) {
        m_copyStretchesBtn->setEnabled(false);
        m_previewTimer->start(150); // Debounce
    } else {
        if (m_previewChk) m_copyStretchesBtn->setEnabled(true);
        if (m_previewTimer) m_previewTimer->stop();
        if (auto win = getActiveImageWindow()) {
            win->restoreOriginalImage();
        }
    }
}

static std::vector<std::vector<int>> computeBufferHistogram(std::shared_ptr<ImageBuffer> buf, int bins = 65536) {
    std::vector<std::vector<int>> hists(1, std::vector<int>(bins, 0));
    if (!buf) return hists;
    int totalPixels = buf->width() * buf->height();
    int sampleStep = std::max(1, totalPixels / 50000);
    const float* data = buf->data();
    for (int i = 0; i < totalPixels; i += sampleStep) {
        float val = data[i];
        if (!std::isnan(val)) {
            int bin = std::max(0, std::min(bins - 1, static_cast<int>(val * (bins - 1))));
            hists[0][bin]++;
        }
    }
    return hists;
}

static std::vector<std::vector<int>> computeVariantHistogram(const ImageVariant& baseImg, int bins = 65536) {
    std::vector<std::vector<int>> hists;
    int width = 0, height = 0;
    if (std::holds_alternative<GrayscaleImagePtr>(baseImg)) {
        auto img = std::get<GrayscaleImagePtr>(baseImg);
        if (!img) return hists;
        width = img->width();
        height = img->height();
        hists.push_back(std::vector<int>(bins, 0));
    } else if (std::holds_alternative<RGBImagePtr>(baseImg)) {
        auto img = std::get<RGBImagePtr>(baseImg);
        if (!img) return hists;
        width = img->width();
        height = img->height();
        hists.assign(4, std::vector<int>(bins, 0)); // L, R, G, B
    } else {
        return hists;
    }

    int totalPixels = width * height;
    int sampleStep = std::max(1, totalPixels / 50000);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int linearIdx = y * width + x;
            if (linearIdx % sampleStep == 0) {
                if (std::holds_alternative<GrayscaleImagePtr>(baseImg)) {
                    float val = std::get<GrayscaleImagePtr>(baseImg)->buffer()->pixel(x, y);
                    if (!std::isnan(val)) {
                        int bin = std::max(0, std::min(bins - 1, static_cast<int>(val * (bins - 1))));
                        hists[0][bin]++;
                    }
                } else {
                    auto rgb = std::get<RGBImagePtr>(baseImg);
                    float r = rgb->r()->buffer()->pixel(x, y);
                    float g = rgb->g()->buffer()->pixel(x, y);
                    float b = rgb->b()->buffer()->pixel(x, y);
                    
                    if (!std::isnan(r) && !std::isnan(g) && !std::isnan(b)) {
                        int binR = std::max(0, std::min(bins - 1, static_cast<int>(r * (bins - 1))));
                        int binG = std::max(0, std::min(bins - 1, static_cast<int>(g * (bins - 1))));
                        int binB = std::max(0, std::min(bins - 1, static_cast<int>(b * (bins - 1))));
                        int binL = std::max(0, std::min(bins - 1, static_cast<int>(((r+g+b)/3.0f) * (bins - 1))));
                        hists[0][binL]++;
                        hists[1][binR]++;
                        hists[2][binG]++;
                        hists[3][binB]++;
                    }
                }
            }
        }
    }

    return hists;
}

void StretchingDialog::refreshHistogramAndCache() {
    auto win = getActiveImageWindow();
    if (!win || !win->imageView()) return;

#ifdef _OPENMP
    int threads = m_threads > 0 ? m_threads : Preferences::instance().getThreadCount();
    if (threads > 0) {
        omp_set_num_threads(threads);
    }
#endif

    ImageVariant baseImg = win->originalImage();
    void* currentPtr = nullptr;
    if (std::holds_alternative<RGBImagePtr>(baseImg)) {
        currentPtr = static_cast<void*>(std::get<RGBImagePtr>(baseImg).get());
    } else if (std::holds_alternative<GrayscaleImagePtr>(baseImg)) {
        currentPtr = static_cast<void*>(std::get<GrayscaleImagePtr>(baseImg).get());
    }

    if (!currentPtr) return;

    bool isRGB = std::holds_alternative<RGBImagePtr>(baseImg);
    if (m_channelGroup) {
        if (isRGB) {
            for (int i = 0; i <= 5; ++i) {
                if (auto btn = m_channelGroup->button(i)) {
                    btn->show();
                    if (i == 0) btn->setText("RGB");
                }
            }
        } else {
            for (int i = 1; i <= 5; ++i) {
                if (auto btn = m_channelGroup->button(i)) btn->hide();
            }
            if (auto btn = m_channelGroup->button(0)) {
                btn->setText("K");
                if (m_activeChannel != ActiveChannel::K) {
                    m_channelGroup->button(static_cast<int>(ActiveChannel::K))->setChecked(true);
                    m_activeChannel = ActiveChannel::K;
                }
            }
        }
    }

    if (m_cachedBaseImgPtr != currentPtr) {
        m_cachedBaseImgPtr = currentPtr;
        m_cachedHBuf = nullptr;
        m_cachedSBuf = nullptr;
        m_cachedLBuf = nullptr;
        m_cachedHistogram.clear();
        m_cachedHistogramChannel = -1;

        if (std::holds_alternative<RGBImagePtr>(baseImg)) {
            auto rgb = std::get<RGBImagePtr>(baseImg);
            int w = rgb->width();
            int h = rgb->height();
            int numPixels = w * h;
            m_cachedHBuf = std::make_shared<ImageBuffer>(w, h);
            m_cachedSBuf = std::make_shared<ImageBuffer>(w, h);
            m_cachedLBuf = std::make_shared<ImageBuffer>(w, h);
            
            const float* rData = rgb->r()->buffer()->data();
            const float* gData = rgb->g()->buffer()->data();
            const float* bData = rgb->b()->buffer()->data();
            float* hData = m_cachedHBuf->data();
            float* sData = m_cachedSBuf->data();
            float* lData = m_cachedLBuf->data();

            #pragma omp parallel for
            for (int i = 0; i < numPixels; ++i) {
                StretchingAlgorithm::rgbToHsl(rData[i], gData[i], bData[i], hData[i], sData[i], lData[i]);
            }
        }
    }

    int activeChInt = static_cast<int>(m_activeChannel);
    if (m_cachedHistogramChannel == activeChInt && !m_cachedHistogram.empty()) {
        QColor traceColor = Qt::white;
        if (m_activeChannel == ActiveChannel::L) {
            traceColor = QColor("#c4a000");
        } else if (m_activeChannel == ActiveChannel::S) {
            traceColor = QColor("#75507b");
        }
        if (m_htWidget) m_htWidget->setSingleTraceColor(traceColor);
        if (m_ghsWidget) m_ghsWidget->setSingleTraceColor(traceColor);
        if (m_curvesWidget) m_curvesWidget->setSingleTraceColor(traceColor);

        if (m_htWidget) m_htWidget->setHistograms(m_cachedHistogram);
        if (m_ghsWidget) m_ghsWidget->setHistograms(m_cachedHistogram);
        if (m_curvesWidget) m_curvesWidget->setHistograms(m_cachedHistogram);
        return;
    }

    m_cachedHistogramChannel = activeChInt;

    if (m_activeChannel == ActiveChannel::L && m_cachedLBuf) {
        QColor traceColor = QColor("#c4a000"); // Yellow
        m_cachedHistogram = computeBufferHistogram(m_cachedLBuf, 65536);
        if (m_htWidget) { m_htWidget->setSingleTraceColor(traceColor); m_htWidget->setHistograms(m_cachedHistogram); }
        if (m_ghsWidget) { m_ghsWidget->setSingleTraceColor(traceColor); m_ghsWidget->setHistograms(m_cachedHistogram); }
        if (m_curvesWidget) { m_curvesWidget->setSingleTraceColor(traceColor); m_curvesWidget->setHistograms(m_cachedHistogram); }
    } else if (m_activeChannel == ActiveChannel::S && m_cachedSBuf) {
        QColor traceColor = QColor("#75507b"); // Purple
        m_cachedHistogram = computeBufferHistogram(m_cachedSBuf, 65536);
        if (m_htWidget) { m_htWidget->setSingleTraceColor(traceColor); m_htWidget->setHistograms(m_cachedHistogram); }
        if (m_ghsWidget) { m_ghsWidget->setSingleTraceColor(traceColor); m_ghsWidget->setHistograms(m_cachedHistogram); }
        if (m_curvesWidget) { m_curvesWidget->setSingleTraceColor(traceColor); m_curvesWidget->setHistograms(m_cachedHistogram); }
    } else {
        QColor traceColor = Qt::white;
        m_cachedHistogram = computeVariantHistogram(baseImg, 65536);
        if (m_htWidget) { m_htWidget->setSingleTraceColor(traceColor); m_htWidget->setHistograms(m_cachedHistogram); }
        if (m_ghsWidget) { m_ghsWidget->setSingleTraceColor(traceColor); m_ghsWidget->setHistograms(m_cachedHistogram); }
        if (m_curvesWidget) { m_curvesWidget->setSingleTraceColor(traceColor); m_curvesWidget->setHistograms(m_cachedHistogram); }
    }
}

void StretchingDialog::syncUiFromValues() {
    auto getParam = [this](const std::array<double, 3>& rgb, double l, double s) {
        if (m_activeChannel == ActiveChannel::L) return l;
        if (m_activeChannel == ActiveChannel::S) return s;
        if (m_activeChannel == ActiveChannel::R) return rgb[0];
        if (m_activeChannel == ActiveChannel::G) return rgb[1];
        if (m_activeChannel == ActiveChannel::B) return rgb[2];
        return rgb[0]; // RGB linked
    };

    double b = getParam(m_blackpoint, m_lBlackpoint, m_sBlackpoint);
    double w = getParam(m_whitepoint, m_lWhitepoint, m_sWhitepoint);
    double m_rel = getParam(m_midpoint, m_lMidpoint, m_sMidpoint);
    double m_abs = b + m_rel * (w - b);
    double sp = getParam(m_spPoint, m_lSpPoint, m_sSpPoint);
    double d = getParam(m_stretchFactor, m_lStretchFactor, m_sStretchFactor);
    double shadow = getParam(m_shadowProtect, m_lShadowProtect, m_sShadowProtect);
    double high = getParam(m_highlightProtect, m_lHighlightProtect, m_sHighlightProtect);

    // 1. HT block signals & set
    m_bSpin->blockSignals(true); m_bSpin->setValue(b); m_bSpin->blockSignals(false);
    m_bSlider->blockSignals(true); m_bSlider->setValue(static_cast<int>(b * 1000.0)); m_bSlider->blockSignals(false);

    m_wSpin->blockSignals(true); m_wSpin->setValue(w); m_wSpin->blockSignals(false);
    m_wSlider->blockSignals(true); m_wSlider->setValue(static_cast<int>(w * 1000.0)); m_wSlider->blockSignals(false);

    m_mSpin->blockSignals(true); m_mSpin->setValue(m_abs); m_mSpin->blockSignals(false);
    m_mSlider->blockSignals(true); m_mSlider->setValue(static_cast<int>(m_abs * 1000.0)); m_mSlider->blockSignals(false);

    // 2. GHS block signals & set
    m_spSpin->blockSignals(true); m_spSpin->setValue(sp); m_spSpin->blockSignals(false);
    m_spSlider->blockSignals(true); m_spSlider->setValue(static_cast<int>(sp * 1000.0)); m_spSlider->blockSignals(false);

    m_dSpin->blockSignals(true); m_dSpin->setValue(d); m_dSpin->blockSignals(false);
    m_dSlider->blockSignals(true); m_dSlider->setValue(static_cast<int>(d * 10.0)); m_dSlider->blockSignals(false);

    m_shadowSpin->blockSignals(true); m_shadowSpin->setValue(shadow); m_shadowSpin->blockSignals(false);
    m_shadowSlider->blockSignals(true); m_shadowSlider->setValue(static_cast<int>(shadow * 100.0)); m_shadowSlider->blockSignals(false);

    m_highlightSpin->blockSignals(true); m_highlightSpin->setValue(high); m_highlightSpin->blockSignals(false);
    m_highlightSlider->blockSignals(true); m_highlightSlider->setValue(static_cast<int>(high * 100.0)); m_highlightSlider->blockSignals(false);

    // Set widget params
    {
        std::array<double, 6> bp_arr = {
            m_channelsLinked ? m_blackpoint[0] : 0.0,
            m_channelsLinked ? 0.0 : m_blackpoint[0],
            m_channelsLinked ? 0.0 : m_blackpoint[1],
            m_channelsLinked ? 0.0 : m_blackpoint[2],
            m_lBlackpoint,
            m_sBlackpoint
        };
        std::array<double, 6> wp_arr = {
            m_channelsLinked ? m_whitepoint[0] : 1.0,
            m_channelsLinked ? 1.0 : m_whitepoint[0],
            m_channelsLinked ? 1.0 : m_whitepoint[1],
            m_channelsLinked ? 1.0 : m_whitepoint[2],
            m_lWhitepoint,
            m_sWhitepoint
        };
        std::array<double, 6> mp_arr = {
            m_channelsLinked ? m_midpoint[0] : 0.5,
            m_channelsLinked ? 0.5 : m_midpoint[0],
            m_channelsLinked ? 0.5 : m_midpoint[1],
            m_channelsLinked ? 0.5 : m_midpoint[2],
            m_lMidpoint,
            m_sMidpoint
        };

        std::array<double, 6> sp_arr = {
            m_channelsLinked ? m_spPoint[0] : 0.5,
            m_channelsLinked ? 0.5 : m_spPoint[0],
            m_channelsLinked ? 0.5 : m_spPoint[1],
            m_channelsLinked ? 0.5 : m_spPoint[2],
            m_lSpPoint,
            m_sSpPoint
        };
        std::array<double, 6> dp_arr = {
            m_channelsLinked ? m_stretchFactor[0] : 0.0,
            m_channelsLinked ? 0.0 : m_stretchFactor[0],
            m_channelsLinked ? 0.0 : m_stretchFactor[1],
            m_channelsLinked ? 0.0 : m_stretchFactor[2],
            m_lStretchFactor,
            m_sStretchFactor
        };
        std::array<double, 6> sPp_arr = {
            m_channelsLinked ? m_shadowProtect[0] : 0.0,
            m_channelsLinked ? 0.0 : m_shadowProtect[0],
            m_channelsLinked ? 0.0 : m_shadowProtect[1],
            m_channelsLinked ? 0.0 : m_shadowProtect[2],
            m_lShadowProtect,
            m_sShadowProtect
        };
        std::array<double, 6> hPp_arr = {
            m_channelsLinked ? m_highlightProtect[0] : 1.0,
            m_channelsLinked ? 1.0 : m_highlightProtect[0],
            m_channelsLinked ? 1.0 : m_highlightProtect[1],
            m_channelsLinked ? 1.0 : m_highlightProtect[2],
            m_lHighlightProtect,
            m_sHighlightProtect
        };

        if (m_htWidget) {
            m_htWidget->blockSignals(true);
            m_htWidget->setStretchParams(bp_arr, wp_arr, mp_arr);
            m_htWidget->blockSignals(false);
        }
        if (m_ghsWidget) {
            m_ghsWidget->blockSignals(true);
            m_ghsWidget->setGhsParams(sp_arr, dp_arr);
            m_ghsWidget->setGhsProtections(sPp_arr, hPp_arr);
            m_ghsWidget->blockSignals(false);
        }
    }
}

void StretchingDialog::enforceGhsConstraints(ConstraintSource source) {
    auto doEnforce = [&](double& sp_val, double& sh_val, double& hi_val) {
        bool spChanged = false;
        bool shadowChanged = false;
        bool highlightChanged = false;

        if (source == ConstraintSource::SymmetryPoint) {
            if (sh_val > sp_val) { sh_val = sp_val; shadowChanged = true; }
            if (hi_val < sp_val) { hi_val = sp_val; highlightChanged = true; }
        } else if (source == ConstraintSource::ShadowProtect) {
            if (sh_val > sp_val) {
                sp_val = sh_val; spChanged = true;
                if (hi_val < sp_val) { hi_val = sp_val; highlightChanged = true; }
            }
        } else if (source == ConstraintSource::HighlightProtect) {
            if (hi_val < sp_val) {
                sp_val = hi_val; spChanged = true;
                if (sh_val > sp_val) { sh_val = sp_val; shadowChanged = true; }
            }
        }
        return std::make_tuple(spChanged, shadowChanged, highlightChanged);
    };

    bool spC = false, shC = false, hiC = false;
    if (m_activeChannel == ActiveChannel::L) {
        auto res = doEnforce(m_lSpPoint, m_lShadowProtect, m_lHighlightProtect);
        spC = std::get<0>(res); shC = std::get<1>(res); hiC = std::get<2>(res);
    } else if (m_activeChannel == ActiveChannel::K) {
        for (int i=0; i<3; ++i) {
            auto res = doEnforce(m_spPoint[i], m_shadowProtect[i], m_highlightProtect[i]);
            spC |= std::get<0>(res); shC |= std::get<1>(res); hiC |= std::get<2>(res);
        }
    } else if (m_activeChannel == ActiveChannel::S) {
        auto res = doEnforce(m_sSpPoint, m_sShadowProtect, m_sHighlightProtect);
        spC = std::get<0>(res); shC = std::get<1>(res); hiC = std::get<2>(res);
    } else {
        int i = static_cast<int>(m_activeChannel) - 1;
        auto res = doEnforce(m_spPoint[i], m_shadowProtect[i], m_highlightProtect[i]);
        spC = std::get<0>(res); shC = std::get<1>(res); hiC = std::get<2>(res);
    }

    if (spC || shC || hiC) {
        auto getParam = [this](const std::array<double, 3>& rgb, double l, double s) {
            if (m_activeChannel == ActiveChannel::L) return l;
            if (m_activeChannel == ActiveChannel::S) return s;
            if (m_activeChannel == ActiveChannel::R) return rgb[0];
            if (m_activeChannel == ActiveChannel::G) return rgb[1];
            if (m_activeChannel == ActiveChannel::B) return rgb[2];
            return rgb[0];
        };
        double activeSp = getParam(m_spPoint, m_lSpPoint, m_sSpPoint);
        double activeSh = getParam(m_shadowProtect, m_lShadowProtect, m_sShadowProtect);
        double activeHi = getParam(m_highlightProtect, m_lHighlightProtect, m_sHighlightProtect);

        if (spC) {
            if (m_spSpin) { m_spSpin->blockSignals(true); m_spSpin->setValue(activeSp); m_spSpin->blockSignals(false); }
            if (m_spSlider) { m_spSlider->blockSignals(true); m_spSlider->setValue(static_cast<int>(activeSp * 1000.0)); m_spSlider->blockSignals(false); }
        }
        if (shC) {
            if (m_shadowSpin) { m_shadowSpin->blockSignals(true); m_shadowSpin->setValue(activeSh); m_shadowSpin->blockSignals(false); }
            if (m_shadowSlider) { m_shadowSlider->blockSignals(true); m_shadowSlider->setValue(static_cast<int>(activeSh * 100.0)); m_shadowSlider->blockSignals(false); }
        }
        if (hiC) {
            if (m_highlightSpin) { m_highlightSpin->blockSignals(true); m_highlightSpin->setValue(activeHi); m_highlightSpin->blockSignals(false); }
            if (m_highlightSlider) { m_highlightSlider->blockSignals(true); m_highlightSlider->setValue(static_cast<int>(activeHi * 100.0)); m_highlightSlider->blockSignals(false); }
        }
    }
}


ImageVariant StretchingDialog::applyCurrentStretch(const ImageVariant& baseImg, bool isPreview) {
    if (baseImg.index() == 0 && std::get<0>(baseImg) == nullptr) return baseImg;
    
    bool channelsLinked = m_channelsLinked;

    if (std::holds_alternative<GrayscaleImagePtr>(baseImg)) {
        auto gray = std::get<GrayscaleImagePtr>(baseImg);
        auto cloned = cloneGrayscale(gray);

        if (m_mode == StretchMode::Curves) {
            // Always use the widget's cached LUT — it is the single source of truth,
            // kept up to date by onCurveChanged. No reason to recompute for apply.
            return StretchingAlgorithm::stretchCurvesGrayscale(cloned, m_curvesWidget->getLut(0));
        } else if (m_mode == StretchMode::GHS) {
            if (isPreview) {
                auto lut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[0], m_stretchFactor[0], m_shadowProtect[0], m_highlightProtect[0], 1);
                return StretchingAlgorithm::stretchCurvesGrayscale(cloned, lut);
            } else {
                return StretchingAlgorithm::stretchGhsGrayscale(cloned, 0.0, 1.0, m_spPoint[0], m_stretchFactor[0], m_shadowProtect[0], m_highlightProtect[0], 1);
            }
        } else {
            if (isPreview) {
                auto lut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[0], m_whitepoint[0], m_midpoint[0]);
                return StretchingAlgorithm::stretchCurvesGrayscale(cloned, lut);
            } else {
                return StretchingAlgorithm::stretchHistogramGrayscale(cloned, m_blackpoint[0], m_whitepoint[0], m_midpoint[0]);
            }
        }
    } else if (std::holds_alternative<RGBImagePtr>(baseImg)) {
        auto rgb = std::get<RGBImagePtr>(baseImg);
        auto cloned = cloneRGB(rgb);

        if (m_mode == StretchMode::Curves) {
            // Always use the widget's cached LUT and widget's own points for the default check.
            // The widget initialises all 6 channels to {(0,0),(1,1)} so isDefaultCurve is
            // always reliable here, unlike m_curvePoints which starts as empty vectors.
            auto isDefaultCurve = [this](int channel) {
                const auto& pts = m_curvesWidget->getCurvePoints(channel);
                return pts.size() == 2 && pts[0] == QPointF(0,0) && pts[1] == QPointF(1,1);
            };

            // 1. R, G, B stretches
            if (channelsLinked) {
                if (!isDefaultCurve(0)) {
                    auto lut = m_curvesWidget->getLut(0);
                    std::array<std::vector<float>, 3> luts = {lut, lut, lut};
                    cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, false);
                }
            } else {
                if (!isDefaultCurve(1) || !isDefaultCurve(2) || !isDefaultCurve(3)) {
                    std::array<std::vector<float>, 3> luts = {
                        m_curvesWidget->getLut(1),
                        m_curvesWidget->getLut(2),
                        m_curvesWidget->getLut(3)
                    };
                    cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, false);
                }
            }

            // 2. L stretch — proper HSL luminance, matching the preview liveLumLut path
            if (!isDefaultCurve(4)) {
                cloned = StretchingAlgorithm::stretchCurvesHSL(cloned, m_curvesWidget->getLut(4), /*stretchSaturation=*/false);
            }

            // 3. S stretch (saturation in HSL)
            if (!isDefaultCurve(5)) {
                cloned = StretchingAlgorithm::stretchCurvesHSL(cloned, m_curvesWidget->getLut(5), true);
            }

        } else if (m_mode == StretchMode::GHS) {
            auto isDefaultGHS = [](double sp, double sh, double hl, double sf) {
                return std::abs(sp - 0.5) < 1e-4 && std::abs(sh - 0.0) < 1e-4 && std::abs(hl - 1.0) < 1e-4 && sf < 1e-4;
            };

            if (isPreview) {
                // Preview: use per-channel LUTs for fast pixel evaluation
                auto applyGhsLUT = [&](RGBImagePtr img, int ch, bool colorPreserving) -> RGBImagePtr {
                    auto lut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[ch], m_stretchFactor[ch], m_shadowProtect[ch], m_highlightProtect[ch], 1);
                    std::array<std::vector<float>, 3> luts = {lut, lut, lut};
                    return StretchingAlgorithm::stretchCurvesRGB(img, luts, colorPreserving);
                };

                if (channelsLinked) {
                    if (!isDefaultGHS(m_spPoint[0], m_shadowProtect[0], m_highlightProtect[0], m_stretchFactor[0])) {
                        cloned = applyGhsLUT(cloned, 0, false);
                    }
                } else {
                    bool rgbNeedsStretch = !isDefaultGHS(m_spPoint[0], m_shadowProtect[0], m_highlightProtect[0], m_stretchFactor[0]) ||
                                           !isDefaultGHS(m_spPoint[1], m_shadowProtect[1], m_highlightProtect[1], m_stretchFactor[1]) ||
                                           !isDefaultGHS(m_spPoint[2], m_shadowProtect[2], m_highlightProtect[2], m_stretchFactor[2]);
                    if (rgbNeedsStretch) {
                        std::array<std::vector<float>, 3> luts;
                        for (int i = 0; i < 3; ++i) {
                            luts[i] = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[i], m_stretchFactor[i], m_shadowProtect[i], m_highlightProtect[i], 1);
                        }
                        cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, false);
                    }
                }

                if (!isDefaultGHS(m_lSpPoint, m_lShadowProtect, m_lHighlightProtect, m_lStretchFactor)) {
                    auto lut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_lSpPoint, m_lStretchFactor, m_lShadowProtect, m_lHighlightProtect, 1);
                    std::array<std::vector<float>, 3> luts = {lut, lut, lut};
                    cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, true);
                }

                if (!isDefaultGHS(m_sSpPoint, m_sShadowProtect, m_sHighlightProtect, m_sStretchFactor)) {
                    auto lut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_sSpPoint, m_sStretchFactor, m_sShadowProtect, m_sHighlightProtect, 1);
                    cloned = StretchingAlgorithm::stretchCurvesHSL(cloned, lut, true);
                }
            } else {
                // Apply: exact analytical math
                if (channelsLinked) {
                    if (!isDefaultGHS(m_spPoint[0], m_shadowProtect[0], m_highlightProtect[0], m_stretchFactor[0])) {
                        std::array<double, 3> low = {0.0, 0.0, 0.0};
                        std::array<double, 3> high = {1.0, 1.0, 1.0};
                        cloned = StretchingAlgorithm::stretchGhsRGB(cloned, low, high, m_spPoint, m_stretchFactor, m_shadowProtect, m_highlightProtect, 1, false);
                    }
                } else {
                    bool rgbNeedsStretch = !isDefaultGHS(m_spPoint[0], m_shadowProtect[0], m_highlightProtect[0], m_stretchFactor[0]) ||
                                           !isDefaultGHS(m_spPoint[1], m_shadowProtect[1], m_highlightProtect[1], m_stretchFactor[1]) ||
                                           !isDefaultGHS(m_spPoint[2], m_shadowProtect[2], m_highlightProtect[2], m_stretchFactor[2]);
                    if (rgbNeedsStretch) {
                        std::array<double, 3> low = {0.0, 0.0, 0.0};
                        std::array<double, 3> high = {1.0, 1.0, 1.0};
                        cloned = StretchingAlgorithm::stretchGhsRGB(cloned, low, high, m_spPoint, m_stretchFactor, m_shadowProtect, m_highlightProtect, 1, false);
                    }
                }

                if (!isDefaultGHS(m_lSpPoint, m_lShadowProtect, m_lHighlightProtect, m_lStretchFactor)) {
                    std::array<double, 3> low = {0.0, 0.0, 0.0};
                    std::array<double, 3> high = {1.0, 1.0, 1.0};
                    std::array<double, 3> sp = {m_lSpPoint, m_lSpPoint, m_lSpPoint};
                    std::array<double, 3> sf = {m_lStretchFactor, m_lStretchFactor, m_lStretchFactor};
                    std::array<double, 3> sh = {m_lShadowProtect, m_lShadowProtect, m_lShadowProtect};
                    std::array<double, 3> hl = {m_lHighlightProtect, m_lHighlightProtect, m_lHighlightProtect};
                    cloned = StretchingAlgorithm::stretchGhsRGB(cloned, low, high, sp, sf, sh, hl, 1, true);
                }

                if (!isDefaultGHS(m_sSpPoint, m_sShadowProtect, m_sHighlightProtect, m_sStretchFactor)) {
                    cloned = StretchingAlgorithm::stretchGhsHSL(cloned, 0.0, 1.0, m_sSpPoint, m_sStretchFactor, m_sShadowProtect, m_sHighlightProtect, 1, true);
                }
            }

        } else { // HT
            auto isDefaultHT = [](double b, double w, double m) {
                return std::abs(b - 0.0) < 1e-4 && std::abs(w - 1.0) < 1e-4 && std::abs(m - 0.5) < 1e-4;
            };

            if (isPreview) {
                // Preview: build per-channel LUTs and run through stretchCurvesRGB
                if (channelsLinked) {
                    if (!isDefaultHT(m_blackpoint[0], m_whitepoint[0], m_midpoint[0])) {
                        auto lut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[0], m_whitepoint[0], m_midpoint[0]);
                        std::array<std::vector<float>, 3> luts = {lut, lut, lut};
                        cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, false);
                    }
                } else {
                    bool rgbNeedsStretch = !isDefaultHT(m_blackpoint[0], m_whitepoint[0], m_midpoint[0]) ||
                                           !isDefaultHT(m_blackpoint[1], m_whitepoint[1], m_midpoint[1]) ||
                                           !isDefaultHT(m_blackpoint[2], m_whitepoint[2], m_midpoint[2]);
                    if (rgbNeedsStretch) {
                        std::array<std::vector<float>, 3> luts;
                        for (int i = 0; i < 3; ++i) {
                            luts[i] = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[i], m_whitepoint[i], m_midpoint[i]);
                        }
                        cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, false);
                    }
                }

                if (!isDefaultHT(m_lBlackpoint, m_lWhitepoint, m_lMidpoint)) {
                    auto lut = StretchingAlgorithm::buildHistogramLUT(m_lBlackpoint, m_lWhitepoint, m_lMidpoint);
                    std::array<std::vector<float>, 3> luts = {lut, lut, lut};
                    cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, true);
                }

                if (!isDefaultHT(m_sBlackpoint, m_sWhitepoint, m_sMidpoint)) {
                    auto lut = StretchingAlgorithm::buildHistogramLUT(m_sBlackpoint, m_sWhitepoint, m_sMidpoint);
                    cloned = StretchingAlgorithm::stretchCurvesHSL(cloned, lut, true);
                }
            } else {
                // Apply: exact analytical math
                if (channelsLinked) {
                    if (!isDefaultHT(m_blackpoint[0], m_whitepoint[0], m_midpoint[0])) {
                        cloned = StretchingAlgorithm::stretchHistogramRGB(cloned, m_blackpoint, m_whitepoint, m_midpoint, false);
                    }
                } else {
                    bool rgbNeedsStretch = !isDefaultHT(m_blackpoint[0], m_whitepoint[0], m_midpoint[0]) ||
                                           !isDefaultHT(m_blackpoint[1], m_whitepoint[1], m_midpoint[1]) ||
                                           !isDefaultHT(m_blackpoint[2], m_whitepoint[2], m_midpoint[2]);
                    if (rgbNeedsStretch) {
                        cloned = StretchingAlgorithm::stretchHistogramRGB(cloned, m_blackpoint, m_whitepoint, m_midpoint, false);
                    }
                }

                if (!isDefaultHT(m_lBlackpoint, m_lWhitepoint, m_lMidpoint)) {
                    std::array<double, 3> bp = {m_lBlackpoint, m_lBlackpoint, m_lBlackpoint};
                    std::array<double, 3> wp = {m_lWhitepoint, m_lWhitepoint, m_lWhitepoint};
                    std::array<double, 3> mp = {m_lMidpoint, m_lMidpoint, m_lMidpoint};
                    cloned = StretchingAlgorithm::stretchHistogramRGB(cloned, bp, wp, mp, true);
                }

                if (!isDefaultHT(m_sBlackpoint, m_sWhitepoint, m_sMidpoint)) {
                    cloned = StretchingAlgorithm::stretchHistogramHSL(cloned, m_sBlackpoint, m_sWhitepoint, m_sMidpoint, true);
                }
            }
        }
        return cloned;
    }
    return baseImg;
}

void StretchingDialog::updatePreview() {
    auto win = getActiveImageWindow();
    if (!win || !win->imageView()) return;

#ifdef _OPENMP
    int threads = m_threads > 0 ? m_threads : Preferences::instance().getThreadCount();
    if (threads > 0) {
        omp_set_num_threads(threads);
    }
#endif

    ImageVariant baseImg = win->originalImage();
    if (baseImg.index() == 0 && std::get<0>(baseImg) == nullptr) return;


    bool isRGB = std::holds_alternative<RGBImagePtr>(baseImg);
    bool channelsLinked = m_channelsLinked;


    // 1. Get base RGB/K LUTs
    std::vector<float> baseLutR;
    std::vector<float> baseLutG;
    std::vector<float> baseLutB;

    if (isRGB) {
        if (channelsLinked) {
            std::vector<float> lut;
            if (m_mode == StretchMode::Curves) {
                lut = m_curvesWidget->getLut(0);
            } else if (m_mode == StretchMode::GHS) {
                lut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[0], m_stretchFactor[0], m_shadowProtect[0], m_highlightProtect[0], 1);
            } else {
                lut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[0], m_whitepoint[0], m_midpoint[0]);
            }
            baseLutR = baseLutG = baseLutB = lut;
        } else {
            if (m_mode == StretchMode::Curves) {
                baseLutR = m_curvesWidget->getLut(1);
                baseLutG = m_curvesWidget->getLut(2);
                baseLutB = m_curvesWidget->getLut(3);
            } else if (m_mode == StretchMode::GHS) {
                baseLutR = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[0], m_stretchFactor[0], m_shadowProtect[0], m_highlightProtect[0], 1);
                baseLutG = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[1], m_stretchFactor[1], m_shadowProtect[1], m_highlightProtect[1], 1);
                baseLutB = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[2], m_stretchFactor[2], m_shadowProtect[2], m_highlightProtect[2], 1);
            } else {
                baseLutR = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[0], m_whitepoint[0], m_midpoint[0]);
                baseLutG = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[1], m_whitepoint[1], m_midpoint[1]);
                baseLutB = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[2], m_whitepoint[2], m_midpoint[2]);
            }
        }
    } else {
        std::vector<float> lut;
        if (m_mode == StretchMode::Curves) {
            lut = m_curvesWidget->getLut(0);
        } else if (m_mode == StretchMode::GHS) {
            lut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[0], m_stretchFactor[0], m_shadowProtect[0], m_highlightProtect[0], 1);
        } else {
            lut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[0], m_whitepoint[0], m_midpoint[0]);
        }
        baseLutR = baseLutG = baseLutB = lut;
    }

    // isDefaultCurve helper for Curves mode \u2014 checks widget points (always initialised to {(0,0),(1,1)})
    auto isDefaultCurveWidget = [this](int ch) {
        const auto& pts = m_curvesWidget->getCurvePoints(ch);
        return pts.size() == 2 && pts[0] == QPointF(0,0) && pts[1] == QPointF(1,1);
    };

    // 2. Get L stretch LUT (applied in HSL luminance space via liveLumLut)
    std::vector<float> lLut;
    if (isRGB) {
        if (m_mode == StretchMode::Curves) {
            if (!isDefaultCurveWidget(4))
                lLut = m_curvesWidget->getLut(4);
        } else if (m_mode == StretchMode::GHS) {
            lLut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_lSpPoint, m_lStretchFactor, m_lShadowProtect, m_lHighlightProtect, 1);
        } else {
            lLut = StretchingAlgorithm::buildHistogramLUT(m_lBlackpoint, m_lWhitepoint, m_lMidpoint);
        }
    }

    // 3. Get S stretch LUT (applied in HSL saturation space via liveSatLut)
    std::vector<float> sLut;
    if (isRGB) {
        if (m_mode == StretchMode::Curves) {
            if (!isDefaultCurveWidget(5))
                sLut = m_curvesWidget->getLut(5);
        } else if (m_mode == StretchMode::GHS) {
            sLut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_sSpPoint, m_sStretchFactor, m_sShadowProtect, m_sHighlightProtect, 1);
        } else {
            sLut = StretchingAlgorithm::buildHistogramLUT(m_sBlackpoint, m_sWhitepoint, m_sMidpoint);
        }
    }


    // Linear-interpolated LUT evaluation matching StretchingAlgorithm::evaluateLUT exactly
    auto evalLutLinear = [](float val, const std::vector<float>& lut) -> float {
        if (lut.empty()) return val;
        int lutSize = static_cast<int>(lut.size());
        float idxF = val * (lutSize - 1);
        int idx = static_cast<int>(idxF);
        if (idx < 0) return lut.front();
        if (idx >= lutSize - 1) return lut.back();
        float t = idxF - idx;
        return (1.0f - t) * lut[idx] + t * lut[idx + 1];
    };

    auto makeUCharLut = [&](const std::vector<float>& lut) -> std::vector<unsigned char> {
        std::vector<unsigned char> out(65536);
        for (int i = 0; i < 65536; ++i) {
            float val = static_cast<float>(i) / 65535.0f;
            float result = evalLutLinear(val, lut);
            out[i] = static_cast<unsigned char>(qBound(0.0f, result * 255.0f, 255.0f));
        }
        return out;
    };

    // 4. Compose per-channel RGB LUTs (base R/G/B stretches only)
    std::vector<std::vector<unsigned char>> liveLut(3, std::vector<unsigned char>(65536));
    for (int i = 0; i < 65536; ++i) {
        float val = static_cast<float>(i) / 65535.0f;
        auto evalLut = [&](const std::vector<float>& lut) -> unsigned char {
            float result = evalLutLinear(val, lut);
            return static_cast<unsigned char>(qBound(0.0f, result * 255.0f, 255.0f));
        };
        liveLut[0][i] = evalLut(baseLutR);
        liveLut[1][i] = evalLut(baseLutG);
        liveLut[2][i] = evalLut(baseLutB);
    }

    // 5. Build liveLumLut (L stretch in HSL luminance space) and liveSatLut (S in HSL saturation space)
    std::vector<unsigned char> liveLumLut;
    if (isRGB && !lLut.empty()) {
        liveLumLut = makeUCharLut(lLut);
    }

    std::vector<unsigned char> liveSatLut;
    if (isRGB && !sLut.empty()) {
        liveSatLut = makeUCharLut(sLut);
    }

    win->setLivePreview(liveLut, liveSatLut, liveLumLut);
}

void StretchingDialog::onApplyClicked() {
    auto win = getActiveImageWindow();
    if (!win) {
        QMessageBox::warning(this, "Apply Error", "No active image found to apply stretching.");
        close();
        return;
    }

    ImageVariant baseImg = win->originalImage();
    if (baseImg.index() == 0 && std::get<0>(baseImg) == nullptr) return;

    // Clear live preview first so the display mode is restored
    clearPreview();

    // Save undo state
    win->saveUndoState();

    // Compute final result analytically
    ImageVariant finalResult = applyCurrentStretch(baseImg, /*isPreview=*/false);
    if (finalResult.index() == 0 && std::get<0>(finalResult) == nullptr) return;

    // Mutate in workspace
    WorkspaceElement elem;
    if (std::holds_alternative<GrayscaleImagePtr>(finalResult)) {
        elem = std::get<GrayscaleImagePtr>(finalResult);
    } else if (std::holds_alternative<RGBImagePtr>(finalResult)) {
        elem = std::get<RGBImagePtr>(finalResult);
    }
    win->setElement(elem, /*preserveZoom=*/true);
}

void StretchingDialog::onPrefsClicked() {
    QDialog dlg(this);
    dlg.setWindowTitle("Stretching Preferences");
    dlg.resize(320, 150);
    dlg.setStyleSheet(styleSheet());

    QFormLayout* form = new QFormLayout(&dlg);
    form->setContentsMargins(15, 15, 15, 15);
    form->setSpacing(10);

    QSpinBox* threadSpin = new QSpinBox(&dlg);
    threadSpin->setRange(1, 64);
    threadSpin->setValue(m_threads > 0 ? m_threads : Preferences::instance().getThreadCount());
    form->addRow("Computation Threads:", threadSpin);

    QHBoxLayout* btns = new QHBoxLayout();
    btns->addStretch(1);
    QPushButton* cancel = new QPushButton("Cancel", &dlg);
    connect(cancel, &QPushButton::clicked, &dlg, &QDialog::reject);
    btns->addWidget(cancel);
    QPushButton* ok = new QPushButton("OK", &dlg);
    ok->setObjectName("primaryButton");
    connect(ok, &QPushButton::clicked, &dlg, &QDialog::accept);
    btns->addWidget(ok);
    form->addRow(btns);

    if (dlg.exec() == QDialog::Accepted) {
        m_threads = threadSpin->value();
        onParameterChanged();
    }
}

std::map<std::string, std::string> StretchingDialog::getConfig() const {
    std::map<std::string, std::string> config;
    if (m_mode == StretchMode::HT) config["mode"] = "histogram";
    else if (m_mode == StretchMode::GHS) config["mode"] = "ghs";
    else config["mode"] = "curves";
    
    config["threads"] = std::to_string(m_threads);

    if (m_mode == StretchMode::GHS) {
        config["symmetry_point"] = std::to_string(m_spPoint[0]);
        config["stretch_factor"] = std::to_string(m_stretchFactor[0]);
        config["shadow_protect"] = std::to_string(m_shadowProtect[0]);
        config["highlight_protect"] = std::to_string(m_highlightProtect[0]);
        config["form"] = "1";
    } else if (m_mode == StretchMode::HT) {
        config["blackpoint"] = std::to_string(m_blackpoint[0]);
        config["whitepoint"] = std::to_string(m_whitepoint[0]);
        config["midpoint"] = std::to_string(m_midpoint[0]);
    }
    return config;
}

QJsonObject StretchingDialog::serializeState() const {
    QJsonObject obj;
    obj["mode"] = static_cast<int>(m_mode);
    
    auto arrToJson = [](const std::array<double, 3>& arr) {
        QJsonArray jArr;
        jArr.append(arr[0]);
        jArr.append(arr[1]);
        jArr.append(arr[2]);
        return jArr;
    };
    
    // HT params
    obj["blackpoint"] = arrToJson(m_blackpoint);
    obj["whitepoint"] = arrToJson(m_whitepoint);
    obj["midpoint"] = arrToJson(m_midpoint);
    // GHS params
    obj["sp_point"] = arrToJson(m_spPoint);
    obj["stretch_factor"] = arrToJson(m_stretchFactor);
    obj["shadow_protect"] = arrToJson(m_shadowProtect);
    obj["highlight_protect"] = arrToJson(m_highlightProtect);
    
    // Curves params
    QJsonArray curvesArr;
    for (int i = 0; i < 6; ++i) {
        QJsonArray ptsArr;
        for (const auto& pt : m_curvePoints[i]) {
            QJsonObject ptObj;
            ptObj["x"] = pt.x();
            ptObj["y"] = pt.y();
            ptsArr.append(ptObj);
        }
        curvesArr.append(ptsArr);
    }
    obj["curves"] = curvesArr;

    // Prefs
    obj["threads"] = m_threads;
    return obj;
}

void StretchingDialog::restoreState(const QJsonObject& obj) {
    if (m_previewChk) {
        m_previewChk->blockSignals(true);
        m_previewChk->setChecked(false);
        m_previewChk->blockSignals(false);
    }

    auto jsonToArr = [](const QJsonObject& o, const QString& key, std::array<double, 3>& out, double def) {
        if (o.contains(key)) {
            if (o[key].isArray()) {
                QJsonArray arr = o[key].toArray();
                out[0] = arr[0].toDouble(def);
                out[1] = arr[1].toDouble(def);
                out[2] = arr[2].toDouble(def);
            } else {
                double val = o[key].toDouble(def);
                out = {val, val, val};
            }
        }
    };

    jsonToArr(obj, "blackpoint", m_blackpoint, 0.0);
    jsonToArr(obj, "whitepoint", m_whitepoint, 1.0);
    jsonToArr(obj, "midpoint", m_midpoint, 0.5);
    jsonToArr(obj, "sp_point", m_spPoint, 0.5);
    jsonToArr(obj, "stretch_factor", m_stretchFactor, 0.0);
    jsonToArr(obj, "shadow_protect", m_shadowProtect, 0.0);
    jsonToArr(obj, "highlight_protect", m_highlightProtect, 1.0);

    if (obj.contains("threads")) {
        m_threads = obj["threads"].toInt(-1);
    }
    if (obj.contains("mode")) {
        m_mode = static_cast<StretchMode>(obj["mode"].toInt(0));
    } else if (obj.contains("is_ghs_mode")) {
        m_mode = obj["is_ghs_mode"].toBool(false) ? StretchMode::GHS : StretchMode::HT;
    }
    
    if (obj.contains("curves")) {
        QJsonArray curvesArr = obj["curves"].toArray();
        for (int i = 0; i < 6 && i < curvesArr.size(); ++i) {
            QJsonArray ptsArr = curvesArr[i].toArray();
            std::vector<QPointF> pts;
            for (int j = 0; j < ptsArr.size(); ++j) {
                QJsonObject ptObj = ptsArr[j].toObject();
                pts.push_back(QPointF(ptObj["x"].toDouble(), ptObj["y"].toDouble()));
            }
            if (pts.size() >= 2) m_curvePoints[i] = pts;
        }
    }

    m_tabWidget->blockSignals(true);
    m_tabWidget->setCurrentIndex(static_cast<int>(m_mode));
    m_tabWidget->blockSignals(false);

    if (m_htWidget) m_htWidget->setActive(m_mode == StretchMode::HT);
    if (m_ghsWidget) m_ghsWidget->setActive(m_mode == StretchMode::GHS);
    if (m_curvesWidget) m_curvesWidget->setActive(m_mode == StretchMode::Curves);

    if (m_histogramContainer) {
        if (m_mode == StretchMode::Curves) {
            m_histogramContainer->setCurrentWidget(m_curvesWidget);
        } else if (m_mode == StretchMode::GHS) {
            m_histogramContainer->setCurrentWidget(m_ghsWidget);
        } else {
            m_histogramContainer->setCurrentWidget(m_htWidget);
        }
    }

    syncUiFromValues();
    onParameterChanged();
}

void StretchingDialog::onTargetImageChanged(QMdiSubWindow* sub) {
    if (!sub) {
        if (m_currentTrackedSub) {
            if (auto oldWin = qobject_cast<WorkspaceImageWindow*>(m_currentTrackedSub->widget())) {
                disconnect(oldWin, &WorkspaceImageWindow::imageUpdated, this, &StretchingDialog::onTargetImageUpdated);
            }
            m_currentTrackedSub = nullptr;
            refreshHistogramAndCache();
        }
        return;
    }

    auto newWin = qobject_cast<WorkspaceImageWindow*>(sub->widget());
    if (!newWin) return; // Ignore non-image subwindows like StretchingDialog itself

    // Preview windows cannot become a target unless we are actively using Live Preview
    if (newWin->hasPreviewActive() && (!m_previewChk || !m_previewChk->isChecked())) {
        return; 
    }

    if (m_currentTrackedSub == sub) return; // Already tracking

    if (m_currentTrackedSub) {
        if (auto oldWin = qobject_cast<WorkspaceImageWindow*>(m_currentTrackedSub->widget())) {
            disconnect(oldWin, &WorkspaceImageWindow::imageUpdated, this, &StretchingDialog::onTargetImageUpdated);
            if (m_previewChk && m_previewChk->isChecked()) {
                oldWin->restoreOriginalImage();
            }
        }
    }
    
    m_currentTrackedSub = sub;
    if (m_currentTrackedSub) {
        if (auto win = qobject_cast<WorkspaceImageWindow*>(m_currentTrackedSub->widget())) {
            connect(win, &WorkspaceImageWindow::imageUpdated, this, &StretchingDialog::onTargetImageUpdated);
            
            bool isGray = std::holds_alternative<GrayscaleImagePtr>(win->currentImage());
            m_channelGroup->button(static_cast<int>(ActiveChannel::R))->setEnabled(!isGray);
            m_channelGroup->button(static_cast<int>(ActiveChannel::G))->setEnabled(!isGray);
            m_channelGroup->button(static_cast<int>(ActiveChannel::B))->setEnabled(!isGray);
            m_channelGroup->button(static_cast<int>(ActiveChannel::L))->setEnabled(!isGray);
            m_channelGroup->button(static_cast<int>(ActiveChannel::S))->setEnabled(!isGray);

            if (isGray && m_activeChannel != ActiveChannel::K) {
                m_channelGroup->button(static_cast<int>(ActiveChannel::K))->setChecked(true);
                onChannelChanged(static_cast<int>(ActiveChannel::K));
            }
        }
    }
    refreshHistogramAndCache();
}

void StretchingDialog::onTargetImageUpdated() {
    refreshHistogramAndCache();
}

} // namespace blastro

void blastro::StretchingDialog::onCurveChanged(int channel, const std::vector<QPointF>& points) {
    if (channel >= 0 && channel < 6) {
        m_curvePoints[channel] = points;
        onParameterChanged();
    }
}

bool blastro::StretchingDialog::hasActivePreview() const {
    return m_previewChk && m_previewChk->isChecked();
}

void blastro::StretchingDialog::clearPreview() {
    WorkspaceImageWindow* win = nullptr;
    if (m_currentTrackedSub) {
        win = qobject_cast<WorkspaceImageWindow*>(m_currentTrackedSub->widget());
    } else {
        win = getActiveImageWindow();
    }

    if (m_previewChk) {
        m_previewChk->blockSignals(true);
        m_previewChk->setChecked(false);
        m_previewChk->blockSignals(false);
    }
    if (m_previewTimer) {
        m_previewTimer->stop();
    }

    if (win) {
        win->restoreOriginalImage();
    }
}

QMdiSubWindow* blastro::StretchingDialog::getTargetWindow() const {
    return m_currentTrackedSub;
}
