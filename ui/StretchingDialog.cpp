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

    auto getActiveBP = [this]() { return m_blackpoint[static_cast<int>(m_activeChannel)]; };
    auto getActiveWP = [this]() { return m_whitepoint[static_cast<int>(m_activeChannel)]; };
    auto getActiveRelMP = [this]() { return m_midpoint[static_cast<int>(m_activeChannel)]; };
    auto updateActiveBP = [this](double val) { m_blackpoint[static_cast<int>(m_activeChannel)] = val; };
    auto updateActiveWP = [this](double val) { m_whitepoint[static_cast<int>(m_activeChannel)] = val; };
    auto updateActiveRelMP = [this](double val) { m_midpoint[static_cast<int>(m_activeChannel)] = val; };
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

    auto setParam = [this](std::array<double, 6>& arr, double val) {
        arr[static_cast<int>(m_activeChannel)] = val;
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
        setParam(m_spPoint, val / 1000.0);
        enforceGhsConstraints(ConstraintSource::SymmetryPoint);
        onParameterChanged();
    });
    connect(m_spSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, setParam](double val) {
        m_spSlider->blockSignals(true);
        m_spSlider->setValue(static_cast<int>(val * 1000.0));
        m_spSlider->blockSignals(false);
        setParam(m_spPoint, val);
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
        setParam(m_stretchFactor, val / 10.0);
        onParameterChanged();
    });
    connect(m_dSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, setParam](double val) {
        m_dSlider->blockSignals(true);
        m_dSlider->setValue(static_cast<int>(val * 10.0));
        m_dSlider->blockSignals(false);
        setParam(m_stretchFactor, val);
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
        setParam(m_shadowProtect, val / 100.0);
        enforceGhsConstraints(ConstraintSource::ShadowProtect);
        onParameterChanged();
    });
    connect(m_shadowSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, setParam](double val) {
        m_shadowSlider->blockSignals(true);
        m_shadowSlider->setValue(static_cast<int>(val * 100.0));
        m_shadowSlider->blockSignals(false);
        setParam(m_shadowProtect, val);
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
        setParam(m_highlightProtect, val / 100.0);
        enforceGhsConstraints(ConstraintSource::HighlightProtect);
        onParameterChanged();
    });
    connect(m_highlightSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, setParam](double val) {
        m_highlightSlider->blockSignals(true);
        m_highlightSlider->setValue(static_cast<int>(val * 100.0));
        m_highlightSlider->blockSignals(false);
        setParam(m_highlightProtect, val);
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
        
        QString highlightColor = "#ffffff";
        QString textColor = "white";
        if (i == 0) { highlightColor = "#ffffff"; textColor = "black"; } // K (White)
        else if (i == 1) highlightColor = "#cc3333"; // R (Red)
        else if (i == 2) highlightColor = "#33aa33"; // G (Green)
        else if (i == 3) highlightColor = "#3366cc"; // B (Blue)
        else if (i == 4) highlightColor = "#c4a000"; // L (Yellow/Gold)
        else if (i == 5) highlightColor = "#75507b"; // S (Purple)

        QString buttonStyle = QString(
            "QPushButton { background-color: #333; border: 1px solid #444; border-radius: 4px; margin: 0px 2px; color: #ccc; font-weight: bold; padding: 4px 0px; }"
            "QPushButton:hover { background-color: #444; }"
            "QPushButton:checked { background-color: %1; color: %2; border-color: %1; }"
        ).arg(highlightColor, textColor);

        btn->setStyleSheet(buttonStyle);
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
    m_blackpoint = b_arr;
    m_whitepoint = w_arr;
    m_midpoint = m_arr;
    syncUiFromValues();
    onParameterChanged();
}

void StretchingDialog::onGhsParamsChanged(const std::array<double, 6>& sp_arr, const std::array<double, 6>& d_arr) {
    m_spPoint = sp_arr;
    m_stretchFactor = d_arr;
    enforceGhsConstraints(ConstraintSource::SymmetryPoint);
    syncUiFromValues();
    onParameterChanged();
}

void StretchingDialog::onGhsProtectionsChanged(const std::array<double, 6>& shadowProtect_arr, const std::array<double, 6>& highlightProtect_arr) {
    m_shadowProtect = shadowProtect_arr;
    m_highlightProtect = highlightProtect_arr;
    enforceGhsConstraints(ConstraintSource::ShadowProtect);
    enforceGhsConstraints(ConstraintSource::HighlightProtect);
    syncUiFromValues();
    onParameterChanged();
}

void StretchingDialog::onCopyLiveStretch() {
    auto win = getActiveImageWindow();
    if (win && win->imageView() && m_htWidget && m_tabWidget) {
        m_mode = StretchMode::HT;
        m_tabWidget->setCurrentIndex(0);

        m_blackpoint.fill(0.0);
        m_whitepoint.fill(1.0);
        m_midpoint.fill(0.5);

        auto bp = win->imageView()->blackpoints();
        auto wp = win->imageView()->whitepoints();
        auto mp = win->imageView()->midpoints();

        if (win->imageView()->channelsLinked()) {
            m_blackpoint[0] = bp[0];
            m_whitepoint[0] = wp[0];
            m_midpoint[0] = mp[0];
            if (m_channelGroup && m_channelGroup->button(static_cast<int>(ActiveChannel::K))) {
                m_channelGroup->button(static_cast<int>(ActiveChannel::K))->setChecked(true);
            }
            onChannelChanged(static_cast<int>(ActiveChannel::K));
        } else {
            for (int i = 0; i < 3; ++i) {
                m_blackpoint[i + 1] = bp[i];
                m_whitepoint[i + 1] = wp[i];
                m_midpoint[i + 1] = mp[i];
            }
            if (m_channelGroup && m_channelGroup->button(static_cast<int>(ActiveChannel::G))) {
                m_channelGroup->button(static_cast<int>(ActiveChannel::G))->setChecked(true);
            }
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
    int idx = static_cast<int>(m_activeChannel);
    if (m_mode == StretchMode::HT) {
        m_blackpoint[idx] = 0.0;
        m_whitepoint[idx] = 1.0;
        m_midpoint[idx] = 0.5;
    } else if (m_mode == StretchMode::GHS) {
        m_spPoint[idx] = 0.5;
        m_stretchFactor[idx] = 0.0;
        m_shadowProtect[idx] = 0.0;
        m_highlightProtect[idx] = 1.0;
    } else if (m_mode == StretchMode::Curves) {
        std::vector<QPointF> defaultCurve = { QPointF(0.0, 0.0), QPointF(1.0, 1.0) };
        m_curvePoints[idx] = defaultCurve;
        if (m_curvesWidget) {
            m_curvesWidget->setCurvePoints(idx, defaultCurve);
        }
    }
    
    syncUiFromValues();
    onParameterChanged();
}

void StretchingDialog::onParameterChanged() {
    if (!m_htWidget || !m_ghsWidget) return;

    int currentStage = 0;
    if (m_activeChannel == ActiveChannel::R || m_activeChannel == ActiveChannel::G || m_activeChannel == ActiveChannel::B) currentStage = 1;
    else if (m_activeChannel == ActiveChannel::L) currentStage = 2;
    else if (m_activeChannel == ActiveChannel::S) currentStage = 3;

    for (int i = currentStage + 1; i < 4; ++i) {
        m_stageHistograms[i].clear();
    }

    if (m_htWidget) {
        m_htWidget->setStretchParams(m_blackpoint, m_whitepoint, m_midpoint);
    }
    if (m_ghsWidget) {
        m_ghsWidget->setGhsParams(m_spPoint, m_stretchFactor);
        m_ghsWidget->setGhsProtections(m_shadowProtect, m_highlightProtect);
    }

    if (m_previewChk && m_previewChk->isChecked()) {
        m_copyStretchesBtn->setEnabled(false);
        m_previewTimer->start(150); // Debounce
    } else {
        if (m_previewChk) m_copyStretchesBtn->setEnabled(true);
        if (m_previewTimer) m_previewTimer->stop();
        if (auto win = getActiveImageWindow()) {
            if (win->hasPreviewActive()) {
                m_ignoreImageUpdates = true;
                win->restoreOriginalImage();
                m_ignoreImageUpdates = false;
            }
        }
    }
}

static std::vector<std::vector<int>> computeBufferHistogram(std::shared_ptr<ImageBuffer> buf, int bins = 65536) {
    std::vector<std::vector<int>> hists(1, std::vector<int>(bins, 0));
    if (!buf) return hists;
    int totalPixels = buf->width() * buf->height();
    int sampleStep = std::max(1, totalPixels / 50000);
    const float* data = buf->data();

    #pragma omp parallel
    {
        std::vector<int> localHist(bins, 0);
        #pragma omp for nowait
        for (int i = 0; i < totalPixels; i += sampleStep) {
            float val = data[i];
            if (!std::isnan(val)) {
                int bin = std::max(0, std::min(bins - 1, static_cast<int>(val * (bins - 1))));
                localHist[bin]++;
            }
        }
        #pragma omp critical
        {
            for (int b = 0; b < bins; ++b) {
                hists[0][b] += localHist[b];
            }
        }
    }
    return hists;
}

static std::vector<std::vector<int>> computeVariantHistogram(const ImageVariant& baseImg, int bins = 65536) {
    std::vector<std::vector<int>> hists;
    if (std::holds_alternative<GrayscaleImagePtr>(baseImg)) {
        auto img = std::get<GrayscaleImagePtr>(baseImg);
        if (!img || !img->buffer()) return hists;
        hists.push_back(std::vector<int>(bins, 0));
        int totalPixels = img->width() * img->height();
        int sampleStep = std::max(1, totalPixels / 50000);
        const float* data = img->buffer()->data();

        #pragma omp parallel
        {
            std::vector<int> localHist(bins, 0);
            #pragma omp for nowait
            for (int i = 0; i < totalPixels; i += sampleStep) {
                float val = data[i];
                if (!std::isnan(val)) {
                    int bin = std::max(0, std::min(bins - 1, static_cast<int>(val * (bins - 1))));
                    localHist[bin]++;
                }
            }
            #pragma omp critical
            {
                for (int b = 0; b < bins; ++b) {
                    hists[0][b] += localHist[b];
                }
            }
        }
    } else if (std::holds_alternative<RGBImagePtr>(baseImg)) {
        auto img = std::get<RGBImagePtr>(baseImg);
        if (!img || !img->r() || !img->g() || !img->b()) return hists;
        hists.assign(4, std::vector<int>(bins, 0)); // L, R, G, B
        int totalPixels = img->width() * img->height();
        int sampleStep = std::max(1, totalPixels / 50000);
        const float* rData = img->r()->buffer()->data();
        const float* gData = img->g()->buffer()->data();
        const float* bData = img->b()->buffer()->data();

        #pragma omp parallel
        {
            std::vector<std::vector<int>> localHists(4, std::vector<int>(bins, 0));
            #pragma omp for nowait
            for (int i = 0; i < totalPixels; i += sampleStep) {
                float r = rData[i];
                float g = gData[i];
                float b = bData[i];
                if (!std::isnan(r) && !std::isnan(g) && !std::isnan(b)) {
                    int binR = std::max(0, std::min(bins - 1, static_cast<int>(r * (bins - 1))));
                    int binG = std::max(0, std::min(bins - 1, static_cast<int>(g * (bins - 1))));
                    int binB = std::max(0, std::min(bins - 1, static_cast<int>(b * (bins - 1))));
                    int binL = std::max(0, std::min(bins - 1, static_cast<int>(((r + g + b) / 3.0f) * (bins - 1))));
                    localHists[0][binL]++;
                    localHists[1][binR]++;
                    localHists[2][binG]++;
                    localHists[3][binB]++;
                }
            }
            #pragma omp critical
            {
                for (int ch = 0; ch < 4; ++ch) {
                    for (int b = 0; b < bins; ++b) {
                        hists[ch][b] += localHists[ch][b];
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

    int threads = m_threads > 0 ? m_threads : Preferences::instance().getThreadCount();
    if (threads > 0) {
        omp_set_num_threads(threads);
    }

    ImageVariant baseImg = win->originalImage();
    bool isRGB = std::holds_alternative<RGBImagePtr>(baseImg);
    if (m_htWidget) m_htWidget->setChannelsLinked(!isRGB);
    if (m_ghsWidget) m_ghsWidget->setChannelsLinked(!isRGB);
    if (m_curvesWidget) m_curvesWidget->setChannelsLinked(!isRGB);

    if (m_channelGroup) {
        if (isRGB) {
            for (int i = 0; i <= 5; ++i) {
                if (auto btn = m_channelGroup->button(i)) {
                    btn->show();
                    if (i == 0) btn->setText("K");
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

    int currentStage = 0;
    if (m_activeChannel == ActiveChannel::R || m_activeChannel == ActiveChannel::G || m_activeChannel == ActiveChannel::B) currentStage = 1;
    else if (m_activeChannel == ActiveChannel::L) currentStage = 2;
    else if (m_activeChannel == ActiveChannel::S) currentStage = 3;

    if (!m_stageHistograms[currentStage].empty()) {
        m_cachedHistogramStage = currentStage;
        m_cachedHistogram = m_stageHistograms[currentStage];
        QColor traceColor = Qt::white;
        if (m_activeChannel == ActiveChannel::L) {
            traceColor = QColor("#c4a000");
        } else if (m_activeChannel == ActiveChannel::S) {
            traceColor = QColor("#75507b");
        }
        if (m_htWidget) { m_htWidget->setSingleTraceColor(traceColor); m_htWidget->setHistograms(m_cachedHistogram); }
        if (m_ghsWidget) { m_ghsWidget->setSingleTraceColor(traceColor); m_ghsWidget->setHistograms(m_cachedHistogram); }
        if (m_curvesWidget) { m_curvesWidget->setSingleTraceColor(traceColor); m_curvesWidget->setHistograms(m_cachedHistogram); }
        return;
    }

    m_cachedHistogramStage = currentStage;

    ImageVariant stretchedImg = applyPartialStretch(baseImg, m_activeChannel);

    if (m_activeChannel == ActiveChannel::L || m_activeChannel == ActiveChannel::S) {
        QColor traceColor = (m_activeChannel == ActiveChannel::L) ? QColor("#c4a000") : QColor("#75507b");
        
        std::vector<std::vector<int>> hists(1, std::vector<int>(65536, 0));
        if (std::holds_alternative<RGBImagePtr>(stretchedImg)) {
            auto rgb = std::get<RGBImagePtr>(stretchedImg);
            int totalPixels = rgb->width() * rgb->height();
            int sampleStep = std::max(1, totalPixels / 50000);
            const float* rData = rgb->r()->buffer()->data();
            const float* gData = rgb->g()->buffer()->data();
            const float* bData = rgb->b()->buffer()->data();
            
            #pragma omp parallel
            {
                std::vector<int> localHist(65536, 0);
                #pragma omp for nowait
                for (int i = 0; i < totalPixels; i += sampleStep) {
                    float h, s, l;
                    StretchingAlgorithm::rgbToHsl(rData[i], gData[i], bData[i], h, s, l);
                    float val = (m_activeChannel == ActiveChannel::L) ? l : s;
                    if (!std::isnan(val)) {
                        int bin = std::max(0, std::min(65535, static_cast<int>(val * 65535.0f)));
                        localHist[bin]++;
                    }
                }
                #pragma omp critical
                {
                    for (int b = 0; b < 65536; ++b) {
                        hists[0][b] += localHist[b];
                    }
                }
            }
        }
        m_cachedHistogram = hists;
        if (m_htWidget) { m_htWidget->setSingleTraceColor(traceColor); m_htWidget->setHistograms(m_cachedHistogram); }
        if (m_ghsWidget) { m_ghsWidget->setSingleTraceColor(traceColor); m_ghsWidget->setHistograms(m_cachedHistogram); }
        if (m_curvesWidget) { m_curvesWidget->setSingleTraceColor(traceColor); m_curvesWidget->setHistograms(m_cachedHistogram); }
    } else {
        QColor traceColor = Qt::white;
        m_cachedHistogram = computeVariantHistogram(stretchedImg, 65536);
        if (m_htWidget) { m_htWidget->setSingleTraceColor(traceColor); m_htWidget->setHistograms(m_cachedHistogram); }
        if (m_ghsWidget) { m_ghsWidget->setSingleTraceColor(traceColor); m_ghsWidget->setHistograms(m_cachedHistogram); }
        if (m_curvesWidget) { m_curvesWidget->setSingleTraceColor(traceColor); m_curvesWidget->setHistograms(m_cachedHistogram); }
    }
    
    m_stageHistograms[currentStage] = m_cachedHistogram;
}

void StretchingDialog::syncUiFromValues() {
    int idx = static_cast<int>(m_activeChannel);

    double b = m_blackpoint[idx];
    double w = m_whitepoint[idx];
    double m_rel = m_midpoint[idx];
    double m_abs = b + m_rel * (w - b);
    double sp = m_spPoint[idx];
    double d = m_stretchFactor[idx];
    double shadow = m_shadowProtect[idx];
    double high = m_highlightProtect[idx];

    // 1. HT block signals & set
    if (m_bSpin) { m_bSpin->blockSignals(true); m_bSpin->setValue(b); m_bSpin->blockSignals(false); }
    if (m_bSlider) { m_bSlider->blockSignals(true); m_bSlider->setValue(static_cast<int>(b * 1000.0)); m_bSlider->blockSignals(false); }

    if (m_wSpin) { m_wSpin->blockSignals(true); m_wSpin->setValue(w); m_wSpin->blockSignals(false); }
    if (m_wSlider) { m_wSlider->blockSignals(true); m_wSlider->setValue(static_cast<int>(w * 1000.0)); m_wSlider->blockSignals(false); }

    if (m_mSpin) { m_mSpin->blockSignals(true); m_mSpin->setValue(m_abs); m_mSpin->blockSignals(false); }
    if (m_mSlider) { m_mSlider->blockSignals(true); m_mSlider->setValue(static_cast<int>(m_abs * 1000.0)); m_mSlider->blockSignals(false); }

    // 2. GHS block signals & set
    if (m_spSpin) { m_spSpin->blockSignals(true); m_spSpin->setValue(sp); m_spSpin->blockSignals(false); }
    if (m_spSlider) { m_spSlider->blockSignals(true); m_spSlider->setValue(static_cast<int>(sp * 1000.0)); m_spSlider->blockSignals(false); }

    if (m_dSpin) { m_dSpin->blockSignals(true); m_dSpin->setValue(d); m_dSpin->blockSignals(false); }
    if (m_dSlider) { m_dSlider->blockSignals(true); m_dSlider->setValue(static_cast<int>(d * 10.0)); m_dSlider->blockSignals(false); }

    if (m_shadowSpin) { m_shadowSpin->blockSignals(true); m_shadowSpin->setValue(shadow); m_shadowSpin->blockSignals(false); }
    if (m_shadowSlider) { m_shadowSlider->blockSignals(true); m_shadowSlider->setValue(static_cast<int>(shadow * 100.0)); m_shadowSlider->blockSignals(false); }

    if (m_highlightSpin) { m_highlightSpin->blockSignals(true); m_highlightSpin->setValue(high); m_highlightSpin->blockSignals(false); }
    if (m_highlightSlider) { m_highlightSlider->blockSignals(true); m_highlightSlider->setValue(static_cast<int>(high * 100.0)); m_highlightSlider->blockSignals(false); }

    if (m_htWidget) {
        m_htWidget->blockSignals(true);
        m_htWidget->setStretchParams(m_blackpoint, m_whitepoint, m_midpoint);
        m_htWidget->blockSignals(false);
    }
    if (m_ghsWidget) {
        m_ghsWidget->blockSignals(true);
        m_ghsWidget->setGhsParams(m_spPoint, m_stretchFactor);
        m_ghsWidget->setGhsProtections(m_shadowProtect, m_highlightProtect);
        m_ghsWidget->blockSignals(false);
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

    int idx = static_cast<int>(m_activeChannel);
    auto res = doEnforce(m_spPoint[idx], m_shadowProtect[idx], m_highlightProtect[idx]);
    bool spC = std::get<0>(res), shC = std::get<1>(res), hiC = std::get<2>(res);

    if (spC || shC || hiC) {
        double activeSp = m_spPoint[idx];
        double activeSh = m_shadowProtect[idx];
        double activeHi = m_highlightProtect[idx];

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
    
    if (std::holds_alternative<GrayscaleImagePtr>(baseImg)) {
        auto gray = std::get<GrayscaleImagePtr>(baseImg);
        auto cloned = cloneGrayscale(gray);

        if (m_mode == StretchMode::Curves) {
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

        constexpr int chK = static_cast<int>(ActiveChannel::K);
        constexpr int chR = static_cast<int>(ActiveChannel::R);
        constexpr int chG = static_cast<int>(ActiveChannel::G);
        constexpr int chB = static_cast<int>(ActiveChannel::B);
        constexpr int chL = static_cast<int>(ActiveChannel::L);
        constexpr int chS = static_cast<int>(ActiveChannel::S);

        if (m_mode == StretchMode::Curves) {
            auto isDefaultCurve = [this](int channel) {
                const auto& pts = m_curvesWidget->getCurvePoints(channel);
                return pts.size() == 2 && pts[0] == QPointF(0,0) && pts[1] == QPointF(1,1);
            };

            if (!isDefaultCurve(chK)) {
                auto lut = m_curvesWidget->getLut(chK);
                std::array<std::vector<float>, 3> luts = {lut, lut, lut};
                cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, false);
            }

            if (!isDefaultCurve(chR) || !isDefaultCurve(chG) || !isDefaultCurve(chB)) {
                std::array<std::vector<float>, 3> luts = {
                    m_curvesWidget->getLut(chR),
                    m_curvesWidget->getLut(chG),
                    m_curvesWidget->getLut(chB)
                };
                cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, false);
            }

            if (!isDefaultCurve(chL)) {
                cloned = StretchingAlgorithm::stretchCurvesHSL(cloned, m_curvesWidget->getLut(chL), false);
            }

            if (!isDefaultCurve(chS)) {
                cloned = StretchingAlgorithm::stretchCurvesHSL(cloned, m_curvesWidget->getLut(chS), true);
            }

        } else if (m_mode == StretchMode::GHS) {
            auto isDefaultGHS = [](double sp, double sh, double hl, double sf) {
                return std::abs(sp - 0.5) < 1e-4 && std::abs(sh - 0.0) < 1e-4 && std::abs(hl - 1.0) < 1e-4 && sf < 1e-4;
            };

            auto applyGhsLUT = [&](RGBImagePtr img, int ch, bool colorPreserving) -> RGBImagePtr {
                auto lut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[ch], m_stretchFactor[ch], m_shadowProtect[ch], m_highlightProtect[ch], 1);
                std::array<std::vector<float>, 3> luts = {lut, lut, lut};
                return StretchingAlgorithm::stretchCurvesRGB(img, luts, colorPreserving);
            };

            if (isPreview) {
                if (!isDefaultGHS(m_spPoint[chK], m_shadowProtect[chK], m_highlightProtect[chK], m_stretchFactor[chK])) {
                    cloned = applyGhsLUT(cloned, chK, false);
                }
                if (!isDefaultGHS(m_spPoint[chR], m_shadowProtect[chR], m_highlightProtect[chR], m_stretchFactor[chR]) ||
                    !isDefaultGHS(m_spPoint[chG], m_shadowProtect[chG], m_highlightProtect[chG], m_stretchFactor[chG]) ||
                    !isDefaultGHS(m_spPoint[chB], m_shadowProtect[chB], m_highlightProtect[chB], m_stretchFactor[chB])) {
                    std::array<std::vector<float>, 3> luts = {
                        StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[chR], m_stretchFactor[chR], m_shadowProtect[chR], m_highlightProtect[chR], 1),
                        StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[chG], m_stretchFactor[chG], m_shadowProtect[chG], m_highlightProtect[chG], 1),
                        StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[chB], m_stretchFactor[chB], m_shadowProtect[chB], m_highlightProtect[chB], 1)
                    };
                    cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, false);
                }
                if (!isDefaultGHS(m_spPoint[chL], m_shadowProtect[chL], m_highlightProtect[chL], m_stretchFactor[chL])) {
                    cloned = applyGhsLUT(cloned, chL, true);
                }
                if (!isDefaultGHS(m_spPoint[chS], m_shadowProtect[chS], m_highlightProtect[chS], m_stretchFactor[chS])) {
                    auto lut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[chS], m_stretchFactor[chS], m_shadowProtect[chS], m_highlightProtect[chS], 1);
                    cloned = StretchingAlgorithm::stretchCurvesHSL(cloned, lut, true);
                }
            } else {
                std::array<double, 3> low = {0.0, 0.0, 0.0};
                std::array<double, 3> high = {1.0, 1.0, 1.0};

                if (!isDefaultGHS(m_spPoint[chK], m_shadowProtect[chK], m_highlightProtect[chK], m_stretchFactor[chK])) {
                    std::array<double, 3> sp = {m_spPoint[chK], m_spPoint[chK], m_spPoint[chK]};
                    std::array<double, 3> sf = {m_stretchFactor[chK], m_stretchFactor[chK], m_stretchFactor[chK]};
                    std::array<double, 3> sh = {m_shadowProtect[chK], m_shadowProtect[chK], m_shadowProtect[chK]};
                    std::array<double, 3> hl = {m_highlightProtect[chK], m_highlightProtect[chK], m_highlightProtect[chK]};
                    cloned = StretchingAlgorithm::stretchGhsRGB(cloned, low, high, sp, sf, sh, hl, 1, false);
                }
                if (!isDefaultGHS(m_spPoint[chR], m_shadowProtect[chR], m_highlightProtect[chR], m_stretchFactor[chR]) ||
                    !isDefaultGHS(m_spPoint[chG], m_shadowProtect[chG], m_highlightProtect[chG], m_stretchFactor[chG]) ||
                    !isDefaultGHS(m_spPoint[chB], m_shadowProtect[chB], m_highlightProtect[chB], m_stretchFactor[chB])) {
                    std::array<double, 3> sp = {m_spPoint[chR], m_spPoint[chG], m_spPoint[chB]};
                    std::array<double, 3> sf = {m_stretchFactor[chR], m_stretchFactor[chG], m_stretchFactor[chB]};
                    std::array<double, 3> sh = {m_shadowProtect[chR], m_shadowProtect[chG], m_shadowProtect[chB]};
                    std::array<double, 3> hl = {m_highlightProtect[chR], m_highlightProtect[chG], m_highlightProtect[chB]};
                    cloned = StretchingAlgorithm::stretchGhsRGB(cloned, low, high, sp, sf, sh, hl, 1, false);
                }
                if (!isDefaultGHS(m_spPoint[chL], m_shadowProtect[chL], m_highlightProtect[chL], m_stretchFactor[chL])) {
                    std::array<double, 3> sp = {m_spPoint[chL], m_spPoint[chL], m_spPoint[chL]};
                    std::array<double, 3> sf = {m_stretchFactor[chL], m_stretchFactor[chL], m_stretchFactor[chL]};
                    std::array<double, 3> sh = {m_shadowProtect[chL], m_shadowProtect[chL], m_shadowProtect[chL]};
                    std::array<double, 3> hl = {m_highlightProtect[chL], m_highlightProtect[chL], m_highlightProtect[chL]};
                    cloned = StretchingAlgorithm::stretchGhsRGB(cloned, low, high, sp, sf, sh, hl, 1, true);
                }
                if (!isDefaultGHS(m_spPoint[chS], m_shadowProtect[chS], m_highlightProtect[chS], m_stretchFactor[chS])) {
                    cloned = StretchingAlgorithm::stretchGhsHSL(cloned, 0.0, 1.0, m_spPoint[chS], m_stretchFactor[chS], m_shadowProtect[chS], m_highlightProtect[chS], 1, true);
                }
            }

        } else { // HT
            auto isDefaultHT = [](double b, double w, double m) {
                return std::abs(b - 0.0) < 1e-4 && std::abs(w - 1.0) < 1e-4 && std::abs(m - 0.5) < 1e-4;
            };

            if (isPreview) {
                if (!isDefaultHT(m_blackpoint[chK], m_whitepoint[chK], m_midpoint[chK])) {
                    auto lut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chK], m_whitepoint[chK], m_midpoint[chK]);
                    std::array<std::vector<float>, 3> luts = {lut, lut, lut};
                    cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, false);
                }
                if (!isDefaultHT(m_blackpoint[chR], m_whitepoint[chR], m_midpoint[chR]) ||
                    !isDefaultHT(m_blackpoint[chG], m_whitepoint[chG], m_midpoint[chG]) ||
                    !isDefaultHT(m_blackpoint[chB], m_whitepoint[chB], m_midpoint[chB])) {
                    std::array<std::vector<float>, 3> luts = {
                        StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chR], m_whitepoint[chR], m_midpoint[chR]),
                        StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chG], m_whitepoint[chG], m_midpoint[chG]),
                        StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chB], m_whitepoint[chB], m_midpoint[chB])
                    };
                    cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, false);
                }
                if (!isDefaultHT(m_blackpoint[chL], m_whitepoint[chL], m_midpoint[chL])) {
                    auto lut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chL], m_whitepoint[chL], m_midpoint[chL]);
                    std::array<std::vector<float>, 3> luts = {lut, lut, lut};
                    cloned = StretchingAlgorithm::stretchCurvesRGB(cloned, luts, true);
                }
                if (!isDefaultHT(m_blackpoint[chS], m_whitepoint[chS], m_midpoint[chS])) {
                    auto lut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chS], m_whitepoint[chS], m_midpoint[chS]);
                    cloned = StretchingAlgorithm::stretchCurvesHSL(cloned, lut, true);
                }
            } else {
                if (!isDefaultHT(m_blackpoint[chK], m_whitepoint[chK], m_midpoint[chK])) {
                    std::array<double, 3> bp = {m_blackpoint[chK], m_blackpoint[chK], m_blackpoint[chK]};
                    std::array<double, 3> wp = {m_whitepoint[chK], m_whitepoint[chK], m_whitepoint[chK]};
                    std::array<double, 3> mp = {m_midpoint[chK], m_midpoint[chK], m_midpoint[chK]};
                    cloned = StretchingAlgorithm::stretchHistogramRGB(cloned, bp, wp, mp, false);
                }
                if (!isDefaultHT(m_blackpoint[chR], m_whitepoint[chR], m_midpoint[chR]) ||
                    !isDefaultHT(m_blackpoint[chG], m_whitepoint[chG], m_midpoint[chG]) ||
                    !isDefaultHT(m_blackpoint[chB], m_whitepoint[chB], m_midpoint[chB])) {
                    std::array<double, 3> bp = {m_blackpoint[chR], m_blackpoint[chG], m_blackpoint[chB]};
                    std::array<double, 3> wp = {m_whitepoint[chR], m_whitepoint[chG], m_whitepoint[chB]};
                    std::array<double, 3> mp = {m_midpoint[chR], m_midpoint[chG], m_midpoint[chB]};
                    cloned = StretchingAlgorithm::stretchHistogramRGB(cloned, bp, wp, mp, false);
                }
                if (!isDefaultHT(m_blackpoint[chL], m_whitepoint[chL], m_midpoint[chL])) {
                    std::array<double, 3> bp = {m_blackpoint[chL], m_blackpoint[chL], m_blackpoint[chL]};
                    std::array<double, 3> wp = {m_whitepoint[chL], m_whitepoint[chL], m_whitepoint[chL]};
                    std::array<double, 3> mp = {m_midpoint[chL], m_midpoint[chL], m_midpoint[chL]};
                    cloned = StretchingAlgorithm::stretchHistogramRGB(cloned, bp, wp, mp, true);
                }
                if (!isDefaultHT(m_blackpoint[chS], m_whitepoint[chS], m_midpoint[chS])) {
                    cloned = StretchingAlgorithm::stretchHistogramHSL(cloned, m_blackpoint[chS], m_whitepoint[chS], m_midpoint[chS], true);
                }
            }
        }
        return cloned;
    }
    return baseImg;
}

ImageVariant StretchingDialog::applyPartialStretch(const ImageVariant& baseImg, ActiveChannel activeChannel) {
    if (baseImg.index() == 0 && std::get<0>(baseImg) == nullptr) return baseImg;
    
    if (std::holds_alternative<GrayscaleImagePtr>(baseImg)) {
        return baseImg;
    }
    
    auto rgb = std::get<RGBImagePtr>(baseImg);
    RGBImagePtr cloned = nullptr;
    
    auto getClone = [&]() -> RGBImagePtr {
        if (!cloned) cloned = cloneRGB(rgb);
        return cloned;
    };
    
    constexpr int chK = static_cast<int>(ActiveChannel::K);
    constexpr int chR = static_cast<int>(ActiveChannel::R);
    constexpr int chG = static_cast<int>(ActiveChannel::G);
    constexpr int chB = static_cast<int>(ActiveChannel::B);
    constexpr int chL = static_cast<int>(ActiveChannel::L);
    constexpr int chS = static_cast<int>(ActiveChannel::S);

    bool needRGBCombined = (activeChannel != ActiveChannel::K);
    bool needRgbChannels = (activeChannel == ActiveChannel::L || activeChannel == ActiveChannel::S);
    bool needL           = (activeChannel == ActiveChannel::S);
    
    if (m_mode == StretchMode::HT) {
        auto isDefaultHT = [](double b, double w, double m) {
            return std::abs(b - 0.0) < 1e-4 && std::abs(w - 1.0) < 1e-4 && std::abs(m - 0.5) < 1e-4;
        };
        
        if (needRGBCombined) {
            if (!isDefaultHT(m_blackpoint[chK], m_whitepoint[chK], m_midpoint[chK])) {
                auto lut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chK], m_whitepoint[chK], m_midpoint[chK]);
                std::array<std::vector<float>, 3> luts = {lut, lut, lut};
                cloned = StretchingAlgorithm::stretchCurvesRGB(getClone(), luts, false);
            }
        }
        
        if (needRgbChannels) {
            if (!isDefaultHT(m_blackpoint[chR], m_whitepoint[chR], m_midpoint[chR]) ||
                !isDefaultHT(m_blackpoint[chG], m_whitepoint[chG], m_midpoint[chG]) ||
                !isDefaultHT(m_blackpoint[chB], m_whitepoint[chB], m_midpoint[chB])) {
                std::array<std::vector<float>, 3> luts = {
                    StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chR], m_whitepoint[chR], m_midpoint[chR]),
                    StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chG], m_whitepoint[chG], m_midpoint[chG]),
                    StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chB], m_whitepoint[chB], m_midpoint[chB])
                };
                cloned = StretchingAlgorithm::stretchCurvesRGB(getClone(), luts, false);
            }
        }
        
        if (needL) {
            if (!isDefaultHT(m_blackpoint[chL], m_whitepoint[chL], m_midpoint[chL])) {
                auto lut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chL], m_whitepoint[chL], m_midpoint[chL]);
                std::array<std::vector<float>, 3> luts = {lut, lut, lut};
                cloned = StretchingAlgorithm::stretchCurvesRGB(getClone(), luts, true);
            }
        }
    } else if (m_mode == StretchMode::GHS) {
        auto isDefaultGHS = [](double sp, double sh, double hl, double sf) {
            return std::abs(sp - 0.5) < 1e-4 && std::abs(sh - 0.0) < 1e-4 && std::abs(hl - 1.0) < 1e-4 && sf < 1e-4;
        };
        
        auto applyGhsLUT = [&](RGBImagePtr img, int ch, bool colorPreserving) -> RGBImagePtr {
            auto lut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[ch], m_stretchFactor[ch], m_shadowProtect[ch], m_highlightProtect[ch], 1);
            std::array<std::vector<float>, 3> luts = {lut, lut, lut};
            return StretchingAlgorithm::stretchCurvesRGB(img, luts, colorPreserving);
        };
        
        if (needRGBCombined) {
            if (!isDefaultGHS(m_spPoint[chK], m_shadowProtect[chK], m_highlightProtect[chK], m_stretchFactor[chK])) {
                cloned = applyGhsLUT(getClone(), chK, false);
            }
        }
        
        if (needRgbChannels) {
            if (!isDefaultGHS(m_spPoint[chR], m_shadowProtect[chR], m_highlightProtect[chR], m_stretchFactor[chR]) ||
                !isDefaultGHS(m_spPoint[chG], m_shadowProtect[chG], m_highlightProtect[chG], m_stretchFactor[chG]) ||
                !isDefaultGHS(m_spPoint[chB], m_shadowProtect[chB], m_highlightProtect[chB], m_stretchFactor[chB])) {
                std::array<std::vector<float>, 3> luts = {
                    StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[chR], m_stretchFactor[chR], m_shadowProtect[chR], m_highlightProtect[chR], 1),
                    StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[chG], m_stretchFactor[chG], m_shadowProtect[chG], m_highlightProtect[chG], 1),
                    StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[chB], m_stretchFactor[chB], m_shadowProtect[chB], m_highlightProtect[chB], 1)
                };
                cloned = StretchingAlgorithm::stretchCurvesRGB(getClone(), luts, false);
            }
        }
        
        if (needL) {
            if (!isDefaultGHS(m_spPoint[chL], m_shadowProtect[chL], m_highlightProtect[chL], m_stretchFactor[chL])) {
                cloned = applyGhsLUT(getClone(), chL, true);
            }
        }
    } else if (m_mode == StretchMode::Curves) {
        auto isDefaultCurve = [this](int channel) {
            const auto& pts = m_curvesWidget->getCurvePoints(channel);
            return pts.size() == 2 && pts[0] == QPointF(0,0) && pts[1] == QPointF(1,1);
        };
        
        if (needRGBCombined) {
            if (!isDefaultCurve(chK)) {
                auto lut = m_curvesWidget->getLut(chK);
                std::array<std::vector<float>, 3> luts = {lut, lut, lut};
                cloned = StretchingAlgorithm::stretchCurvesRGB(getClone(), luts, false);
            }
        }
        
        if (needRgbChannels) {
            if (!isDefaultCurve(chR) || !isDefaultCurve(chG) || !isDefaultCurve(chB)) {
                std::array<std::vector<float>, 3> luts = {
                    m_curvesWidget->getLut(chR),
                    m_curvesWidget->getLut(chG),
                    m_curvesWidget->getLut(chB)
                };
                cloned = StretchingAlgorithm::stretchCurvesRGB(getClone(), luts, false);
            }
        }
        
        if (needL) {
            if (!isDefaultCurve(chL)) {
                cloned = StretchingAlgorithm::stretchCurvesHSL(getClone(), m_curvesWidget->getLut(chL), false);
            }
        }
    }
    if (cloned) {
        return cloned;
    }
    return baseImg;
}

void StretchingDialog::updatePreview() {
    auto win = getActiveImageWindow();
    if (!win || !win->imageView()) return;

    int threads = m_threads > 0 ? m_threads : Preferences::instance().getThreadCount();
    if (threads > 0) {
        omp_set_num_threads(threads);
    }

    ImageVariant baseImg = win->originalImage();
    if (baseImg.index() == 0 && std::get<0>(baseImg) == nullptr) return;
    bool isRGB = std::holds_alternative<RGBImagePtr>(baseImg);

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

    auto isDefaultCurveWidget = [this](int ch) {
        const auto& pts = m_curvesWidget->getCurvePoints(ch);
        return pts.size() == 2 && pts[0] == QPointF(0,0) && pts[1] == QPointF(1,1);
    };
    auto isDefaultGHS = [](double sp, double sh, double hl, double sf) {
        return std::abs(sp - 0.5) < 1e-4 && std::abs(sh - 0.0) < 1e-4 && std::abs(hl - 1.0) < 1e-4 && sf < 1e-4;
    };
    auto isDefaultHT = [](double b, double w, double m) {
        return std::abs(b - 0.0) < 1e-4 && std::abs(w - 1.0) < 1e-4 && std::abs(m - 0.5) < 1e-4;
    };

    auto composeLuts = [&](const std::vector<float>& lut1, const std::vector<float>& lut2) -> std::vector<float> {
        if (lut1.empty() && lut2.empty()) return {};
        if (lut1.empty()) return lut2;
        if (lut2.empty()) return lut1;
        int bins = 65536;
        std::vector<float> res(bins);
        for (int i = 0; i < bins; ++i) {
            float x = static_cast<float>(i) / (bins - 1);
            float y = evalLutLinear(x, lut1);
            res[i] = evalLutLinear(y, lut2);
        }
        return res;
    };

    constexpr int chK = static_cast<int>(ActiveChannel::K);
    constexpr int chR = static_cast<int>(ActiveChannel::R);
    constexpr int chG = static_cast<int>(ActiveChannel::G);
    constexpr int chB = static_cast<int>(ActiveChannel::B);
    constexpr int chL = static_cast<int>(ActiveChannel::L);
    constexpr int chS = static_cast<int>(ActiveChannel::S);

    // 1. Get base RGB/K LUTs
    std::vector<float> kLut, rLut, gLut, bLut;
    if (isRGB) {
        if (m_mode == StretchMode::Curves) {
            if (!isDefaultCurveWidget(chK)) kLut = m_curvesWidget->getLut(chK);
            if (!isDefaultCurveWidget(chR)) rLut = m_curvesWidget->getLut(chR);
            if (!isDefaultCurveWidget(chG)) gLut = m_curvesWidget->getLut(chG);
            if (!isDefaultCurveWidget(chB)) bLut = m_curvesWidget->getLut(chB);
        } else if (m_mode == StretchMode::GHS) {
            if (!isDefaultGHS(m_spPoint[chK], m_shadowProtect[chK], m_highlightProtect[chK], m_stretchFactor[chK]))
                kLut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[chK], m_stretchFactor[chK], m_shadowProtect[chK], m_highlightProtect[chK], 1);
            if (!isDefaultGHS(m_spPoint[chR], m_shadowProtect[chR], m_highlightProtect[chR], m_stretchFactor[chR]))
                rLut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[chR], m_stretchFactor[chR], m_shadowProtect[chR], m_highlightProtect[chR], 1);
            if (!isDefaultGHS(m_spPoint[chG], m_shadowProtect[chG], m_highlightProtect[chG], m_stretchFactor[chG]))
                gLut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[chG], m_stretchFactor[chG], m_shadowProtect[chG], m_highlightProtect[chG], 1);
            if (!isDefaultGHS(m_spPoint[chB], m_shadowProtect[chB], m_highlightProtect[chB], m_stretchFactor[chB]))
                bLut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[chB], m_stretchFactor[chB], m_shadowProtect[chB], m_highlightProtect[chB], 1);
        } else { // HT
            if (!isDefaultHT(m_blackpoint[chK], m_whitepoint[chK], m_midpoint[chK]))
                kLut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chK], m_whitepoint[chK], m_midpoint[chK]);
            if (!isDefaultHT(m_blackpoint[chR], m_whitepoint[chR], m_midpoint[chR]))
                rLut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chR], m_whitepoint[chR], m_midpoint[chR]);
            if (!isDefaultHT(m_blackpoint[chG], m_whitepoint[chG], m_midpoint[chG]))
                gLut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chG], m_whitepoint[chG], m_midpoint[chG]);
            if (!isDefaultHT(m_blackpoint[chB], m_whitepoint[chB], m_midpoint[chB]))
                bLut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chB], m_whitepoint[chB], m_midpoint[chB]);
        }
    } else {
        if (m_mode == StretchMode::Curves) {
            kLut = m_curvesWidget->getLut(chK);
        } else if (m_mode == StretchMode::GHS) {
            if (!isDefaultGHS(m_spPoint[chK], m_shadowProtect[chK], m_highlightProtect[chK], m_stretchFactor[chK]))
                kLut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[chK], m_stretchFactor[chK], m_shadowProtect[chK], m_highlightProtect[chK], 1);
        } else {
            if (!isDefaultHT(m_blackpoint[chK], m_whitepoint[chK], m_midpoint[chK]))
                kLut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chK], m_whitepoint[chK], m_midpoint[chK]);
        }
    }

    std::vector<float> baseLutR = composeLuts(kLut, rLut);
    std::vector<float> baseLutG = composeLuts(kLut, gLut);
    std::vector<float> baseLutB = composeLuts(kLut, bLut);

    // 2. Get L stretch LUT (applied in HSL luminance space via liveLumLut)
    std::vector<float> lLut;
    if (isRGB) {
        if (m_mode == StretchMode::Curves) {
            if (!isDefaultCurveWidget(chL))
                lLut = m_curvesWidget->getLut(chL);
        } else if (m_mode == StretchMode::GHS) {
            if (!isDefaultGHS(m_spPoint[chL], m_shadowProtect[chL], m_highlightProtect[chL], m_stretchFactor[chL]))
                lLut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[chL], m_stretchFactor[chL], m_shadowProtect[chL], m_highlightProtect[chL], 1);
        } else {
            if (!isDefaultHT(m_blackpoint[chL], m_whitepoint[chL], m_midpoint[chL]))
                lLut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chL], m_whitepoint[chL], m_midpoint[chL]);
        }
    }

    // 3. Get S stretch LUT (applied in HSL saturation space via liveSatLut)
    std::vector<float> sLut;
    if (isRGB) {
        if (m_mode == StretchMode::Curves) {
            if (!isDefaultCurveWidget(chS))
                sLut = m_curvesWidget->getLut(chS);
        } else if (m_mode == StretchMode::GHS) {
            if (!isDefaultGHS(m_spPoint[chS], m_shadowProtect[chS], m_highlightProtect[chS], m_stretchFactor[chS]))
                sLut = StretchingAlgorithm::buildGhsLUT(0.0, 1.0, m_spPoint[chS], m_stretchFactor[chS], m_shadowProtect[chS], m_highlightProtect[chS], 1);
        } else {
            if (!isDefaultHT(m_blackpoint[chS], m_whitepoint[chS], m_midpoint[chS]))
                sLut = StretchingAlgorithm::buildHistogramLUT(m_blackpoint[chS], m_whitepoint[chS], m_midpoint[chS]);
        }
    }

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
    
    auto arrToJson = [](const std::array<double, 6>& arr) {
        QJsonArray jArr;
        for (int i = 0; i < 6; ++i) jArr.append(arr[i]);
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

    auto jsonToArr = [](const QJsonObject& o, const QString& key, std::array<double, 6>& out, double def) {
        if (o.contains(key)) {
            if (o[key].isArray()) {
                QJsonArray arr = o[key].toArray();
                // backward-compatible: handle both size 3 and size 6
                for (int i = 0; i < 6; ++i) {
                    if (i < arr.size()) {
                        out[i] = arr[i].toDouble(def);
                    } else if (i >= 1 && i <= 3) {
                        // fallback: R, G, B copy from RGB Combined (arr[0]) if array size was 3
                        out[i] = arr[0].toDouble(def);
                    } else {
                        out[i] = def;
                    }
                }
            } else {
                double val = o[key].toDouble(def);
                out.fill(val);
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
    m_cachedHistogramStage = -1;
    for (int i = 0; i < 4; ++i) m_stageHistograms[i].clear();
    m_cachedHistogram.clear();
    refreshHistogramAndCache();
}

void StretchingDialog::onTargetImageUpdated() {
    if (m_ignoreImageUpdates) return;
    m_cachedHistogramStage = -1;
    for (int i = 0; i < 4; ++i) m_stageHistograms[i].clear();
    m_cachedHistogram.clear();
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
