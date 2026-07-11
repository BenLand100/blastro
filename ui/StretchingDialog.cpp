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
#include <QJsonObject>
#include "HistogramWidget.h"
#include "core/Preferences.h"
#include "algorithms/StretchingAlgorithm.h"
#include "WorkspaceArea.h"
#include <QVBoxLayout>
#include <QFormLayout>
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
      m_histogramWidget(nullptr),
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

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(10);

    // 1. Large Histogram Widget at the top
    m_histogramWidget = new HistogramWidget(this);
    m_histogramWidget->setMinimumHeight(150);
    m_histogramWidget->setActive(true);
    mainLayout->addWidget(m_histogramWidget, 1); // Histogram absorbs vertical resize space

    connect(m_histogramWidget, &HistogramWidget::stretchParamsChanged, this, &StretchingDialog::onHtParamsChanged);
    connect(m_histogramWidget, &HistogramWidget::ghsParamsChanged, this, &StretchingDialog::onGhsParamsChanged);
    connect(m_histogramWidget, &HistogramWidget::ghsProtectionsChanged, this, &StretchingDialog::onGhsProtectionsChanged);

    // 2. Tab Widget for HT vs GHS mode
    m_tabWidget = new QTabWidget(this);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &StretchingDialog::onTabChanged);

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

    connect(m_bSlider, &QSlider::valueChanged, this, [this](int val) {
        m_bSpin->blockSignals(true);
        m_bSpin->setValue(val / 1000.0);
        m_bSpin->blockSignals(false);
        m_blackpoint = val / 1000.0;
        onParameterChanged();
    });
    connect(m_bSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        m_bSlider->blockSignals(true);
        m_bSlider->setValue(static_cast<int>(val * 1000.0));
        m_bSlider->blockSignals(false);
        m_blackpoint = val;
        onParameterChanged();
    });

    // Midpoint Slider/SpinBox
    QHBoxLayout* mLayout = new QHBoxLayout();
    m_mSlider = new QSlider(Qt::Horizontal, this);
    m_mSlider->setRange(1, 999);
    m_mSlider->setValue(500);
    m_mSpin = new QDoubleSpinBox(this);
    m_mSpin->setFixedWidth(80);
    m_mSpin->setRange(0.0001, 0.9999);
    m_mSpin->setSingleStep(0.01);
    m_mSpin->setDecimals(4);
    m_mSpin->setValue(0.5);
    mLayout->addWidget(m_mSlider, 1);
    mLayout->addWidget(m_mSpin);
    htForm->addRow("Mid Point (Gamma):", mLayout);

    connect(m_mSlider, &QSlider::valueChanged, this, [this](int val) {
        m_mSpin->blockSignals(true);
        m_mSpin->setValue(val / 1000.0);
        m_mSpin->blockSignals(false);
        m_midpoint = val / 1000.0;
        onParameterChanged();
    });
    connect(m_mSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        m_mSlider->blockSignals(true);
        m_mSlider->setValue(static_cast<int>(val * 1000.0));
        m_mSlider->blockSignals(false);
        m_midpoint = val;
        onParameterChanged();
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

    connect(m_wSlider, &QSlider::valueChanged, this, [this](int val) {
        m_wSpin->blockSignals(true);
        m_wSpin->setValue(val / 1000.0);
        m_wSpin->blockSignals(false);
        m_whitepoint = val / 1000.0;
        onParameterChanged();
    });
    connect(m_wSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        m_wSlider->blockSignals(true);
        m_wSlider->setValue(static_cast<int>(val * 1000.0));
        m_wSlider->blockSignals(false);
        m_whitepoint = val;
        onParameterChanged();
    });

    m_tabWidget->addTab(htTab, "Histogram Transform");

    // --- Tab 1: Generalized Hyperbolic (GHS) ---
    QWidget* ghsTab = new QWidget(this);
    QFormLayout* ghsForm = new QFormLayout(ghsTab);
    ghsForm->setSpacing(8);

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

    connect(m_spSlider, &QSlider::valueChanged, this, [this](int val) {
        m_spSpin->blockSignals(true);
        m_spSpin->setValue(val / 1000.0);
        m_spSpin->blockSignals(false);
        m_spPoint = val / 1000.0;
        enforceGhsConstraints(ConstraintSource::SymmetryPoint);
        onParameterChanged();
    });
    connect(m_spSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        m_spSlider->blockSignals(true);
        m_spSlider->setValue(static_cast<int>(val * 1000.0));
        m_spSlider->blockSignals(false);
        m_spPoint = val;
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

    connect(m_dSlider, &QSlider::valueChanged, this, [this](int val) {
        m_dSpin->blockSignals(true);
        m_dSpin->setValue(val / 10.0);
        m_dSpin->blockSignals(false);
        m_stretchFactor = val / 10.0;
        onParameterChanged();
    });
    connect(m_dSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        m_dSlider->blockSignals(true);
        m_dSlider->setValue(static_cast<int>(val * 10.0));
        m_dSlider->blockSignals(false);
        m_stretchFactor = val;
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

    connect(m_shadowSlider, &QSlider::valueChanged, this, [this](int val) {
        m_shadowSpin->blockSignals(true);
        m_shadowSpin->setValue(val / 100.0);
        m_shadowSpin->blockSignals(false);
        m_shadowProtect = val / 100.0;
        enforceGhsConstraints(ConstraintSource::ShadowProtect);
        onParameterChanged();
    });
    connect(m_shadowSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        m_shadowSlider->blockSignals(true);
        m_shadowSlider->setValue(static_cast<int>(val * 100.0));
        m_shadowSlider->blockSignals(false);
        m_shadowProtect = val;
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

    connect(m_highlightSlider, &QSlider::valueChanged, this, [this](int val) {
        m_highlightSpin->blockSignals(true);
        m_highlightSpin->setValue(val / 100.0);
        m_highlightSpin->blockSignals(false);
        m_highlightProtect = val / 100.0;
        enforceGhsConstraints(ConstraintSource::HighlightProtect);
        onParameterChanged();
    });
    connect(m_highlightSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        m_highlightSlider->blockSignals(true);
        m_highlightSlider->setValue(static_cast<int>(val * 100.0));
        m_highlightSlider->blockSignals(false);
        m_highlightProtect = val;
        enforceGhsConstraints(ConstraintSource::HighlightProtect);
        onParameterChanged();
    });

    m_tabWidget->addTab(ghsTab, "Generalized Hyperbolic (GHS)");

    mainLayout->addWidget(m_tabWidget, 0); // Controls tab stays compact

    // 3. General control box
    QHBoxLayout* ctrlLayout = new QHBoxLayout();
    m_copyStretchesBtn = new QPushButton("Copy Live Stretch", this);
    connect(m_copyStretchesBtn, &QPushButton::clicked, this, &StretchingDialog::onCopyLiveStretch);
    ctrlLayout->addWidget(m_copyStretchesBtn);

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

    // Load active image histogram into widget
    auto activeWin = getActiveImageWindow();
    if (activeWin && activeWin->imageView()) {
        m_histogramWidget->setHistogram(activeWin->imageView()->getHistogram(65536));
    }

    syncUiFromValues();
}

StretchingDialog::~StretchingDialog() {
    m_previewTimer->stop();
    if (auto win = getActiveImageWindow()) {
        win->restoreOriginalImage();
    }
}

void StretchingDialog::closeEvent(QCloseEvent* event) {
    m_previewTimer->stop();
    if (auto win = getActiveImageWindow()) {
        win->restoreOriginalImage();
    }
    QWidget::closeEvent(event);
}

WorkspaceImageWindow* StretchingDialog::getActiveImageWindow() const {
    QWidget* p = parentWidget();
    while (p) {
        if (auto mw = qobject_cast<QMainWindow*>(p)) {
            if (auto wa = mw->findChild<WorkspaceArea*>()) {
                QString activeName = wa->getActiveImageName();
                if (!activeName.isEmpty()) {
                    return wa->getImageWindow(activeName);
                }
            }
        }
        p = p->parentWidget();
    }
    return nullptr;
}

void StretchingDialog::onTabChanged(int index) {
    m_isGhsMode = (index == 1);
    m_histogramWidget->setGhsMode(m_isGhsMode);
    onParameterChanged();
}

void StretchingDialog::onHtParamsChanged(double b, double w, double m) {
    m_blackpoint = b;
    m_whitepoint = w;
    m_midpoint = m;

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

    if (m_mSpin) {
        m_mSpin->blockSignals(true);
        m_mSpin->setValue(m);
        m_mSpin->blockSignals(false);
    }
    if (m_mSlider) {
        m_mSlider->blockSignals(true);
        m_mSlider->setValue(static_cast<int>(m * 1000.0));
        m_mSlider->blockSignals(false);
    }

    onParameterChanged();
}

void StretchingDialog::onGhsParamsChanged(double sp, double d) {
    m_spPoint = sp;
    m_stretchFactor = d;

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

void StretchingDialog::onGhsProtectionsChanged(double shadowProtect, double highlightProtect) {
    m_shadowProtect = shadowProtect;
    m_highlightProtect = highlightProtect;

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
    if (win && win->imageView() && m_histogramWidget && m_tabWidget) {
        m_blackpoint = win->imageView()->blackpoint();
        m_whitepoint = win->imageView()->whitepoint();
        m_midpoint = win->imageView()->midpoint();
        
        m_tabWidget->setCurrentIndex(0); // HT Tab
        m_isGhsMode = false;
        m_histogramWidget->setGhsMode(false);

        syncUiFromValues();
        onParameterChanged();
    }
}


void StretchingDialog::onParameterChanged() {
    if (!m_histogramWidget) return;

    // Keep histogram widget synced
    if (m_isGhsMode) {
        m_histogramWidget->setGhsParams(m_spPoint, m_stretchFactor);
        m_histogramWidget->setGhsProtections(m_shadowProtect, m_highlightProtect);
    } else {
        m_histogramWidget->setStretchParams(m_blackpoint, m_whitepoint, m_midpoint);
    }

    if (m_previewChk && m_previewChk->isChecked()) {
        m_previewTimer->start(150); // Debounce
    } else {
        if (m_previewTimer) m_previewTimer->stop();
        if (auto win = getActiveImageWindow()) {
            win->restoreOriginalImage();
        }
    }
}

void StretchingDialog::syncUiFromValues() {
    // 1. HT block signals & set
    m_bSpin->blockSignals(true);
    m_bSpin->setValue(m_blackpoint);
    m_bSpin->blockSignals(false);
    m_bSlider->blockSignals(true);
    m_bSlider->setValue(static_cast<int>(m_blackpoint * 1000.0));
    m_bSlider->blockSignals(false);

    m_wSpin->blockSignals(true);
    m_wSpin->setValue(m_whitepoint);
    m_wSpin->blockSignals(false);
    m_wSlider->blockSignals(true);
    m_wSlider->setValue(static_cast<int>(m_whitepoint * 1000.0));
    m_wSlider->blockSignals(false);

    m_mSpin->blockSignals(true);
    m_mSpin->setValue(m_midpoint);
    m_mSpin->blockSignals(false);
    m_mSlider->blockSignals(true);
    m_mSlider->setValue(static_cast<int>(m_midpoint * 1000.0));
    m_mSlider->blockSignals(false);

    // 2. GHS block signals & set
    m_spSpin->blockSignals(true);
    m_spSpin->setValue(m_spPoint);
    m_spSpin->blockSignals(false);
    m_spSlider->blockSignals(true);
    m_spSlider->setValue(static_cast<int>(m_spPoint * 1000.0));
    m_spSlider->blockSignals(false);

    m_dSpin->blockSignals(true);
    m_dSpin->setValue(m_stretchFactor);
    m_dSpin->blockSignals(false);
    m_dSlider->blockSignals(true);
    m_dSlider->setValue(static_cast<int>(m_stretchFactor * 10.0));
    m_dSlider->blockSignals(false);

    m_shadowSpin->blockSignals(true);
    m_shadowSpin->setValue(m_shadowProtect);
    m_shadowSpin->blockSignals(false);
    m_shadowSlider->blockSignals(true);
    m_shadowSlider->setValue(static_cast<int>(m_shadowProtect * 100.0));
    m_shadowSlider->blockSignals(false);

    m_highlightSpin->blockSignals(true);
    m_highlightSpin->setValue(m_highlightProtect);
    m_highlightSpin->blockSignals(false);
    m_highlightSlider->blockSignals(true);
    m_highlightSlider->setValue(static_cast<int>(m_highlightProtect * 100.0));
    m_highlightSlider->blockSignals(false);

    // Set widget params
    if (m_histogramWidget) {
        m_histogramWidget->setStretchParams(m_blackpoint, m_whitepoint, m_midpoint);
        m_histogramWidget->setGhsParams(m_spPoint, m_stretchFactor);
        m_histogramWidget->setGhsProtections(m_shadowProtect, m_highlightProtect);
    }
}

void StretchingDialog::enforceGhsConstraints(ConstraintSource source) {
    bool spChanged = false;
    bool shadowChanged = false;
    bool highlightChanged = false;

    if (source == ConstraintSource::SymmetryPoint) {
        if (m_shadowProtect > m_spPoint) {
            m_shadowProtect = m_spPoint;
            shadowChanged = true;
        }
        if (m_highlightProtect < m_spPoint) {
            m_highlightProtect = m_spPoint;
            highlightChanged = true;
        }
    } else if (source == ConstraintSource::ShadowProtect) {
        if (m_shadowProtect > m_spPoint) {
            m_spPoint = m_shadowProtect;
            spChanged = true;
            if (m_highlightProtect < m_spPoint) {
                m_highlightProtect = m_spPoint;
                highlightChanged = true;
            }
        }
    } else if (source == ConstraintSource::HighlightProtect) {
        if (m_highlightProtect < m_spPoint) {
            m_spPoint = m_highlightProtect;
            spChanged = true;
            if (m_shadowProtect > m_spPoint) {
                m_shadowProtect = m_spPoint;
                shadowChanged = true;
            }
        }
    }

    if (spChanged) {
        if (m_spSpin) {
            m_spSpin->blockSignals(true);
            m_spSpin->setValue(m_spPoint);
            m_spSpin->blockSignals(false);
        }
        if (m_spSlider) {
            m_spSlider->blockSignals(true);
            m_spSlider->setValue(static_cast<int>(m_spPoint * 1000.0));
            m_spSlider->blockSignals(false);
        }
    }
    if (shadowChanged) {
        if (m_shadowSpin) {
            m_shadowSpin->blockSignals(true);
            m_shadowSpin->setValue(m_shadowProtect);
            m_shadowSpin->blockSignals(false);
        }
        if (m_shadowSlider) {
            m_shadowSlider->blockSignals(true);
            m_shadowSlider->setValue(static_cast<int>(m_shadowProtect * 100.0));
            m_shadowSlider->blockSignals(false);
        }
    }
    if (highlightChanged) {
        if (m_highlightSpin) {
            m_highlightSpin->blockSignals(true);
            m_highlightSpin->setValue(m_highlightProtect);
            m_highlightSpin->blockSignals(false);
        }
        if (m_highlightSlider) {
            m_highlightSlider->blockSignals(true);
            m_highlightSlider->setValue(static_cast<int>(m_highlightProtect * 100.0));
            m_highlightSlider->blockSignals(false);
        }
    }
}

void StretchingDialog::updatePreview() {
    auto win = getActiveImageWindow();
    if (!win) return;

    ImageVariant baseImg = win->originalImage();
    if (baseImg.index() == 0 && std::get<0>(baseImg) == nullptr) return;

    ImageVariant previewResult;

    if (m_isGhsMode) {
        if (std::holds_alternative<GrayscaleImagePtr>(baseImg)) {
            auto gray = std::get<GrayscaleImagePtr>(baseImg);
            auto cloned = cloneGrayscale(gray);
            previewResult = StretchingAlgorithm::stretchGhsGrayscale(cloned, 0.0, 1.0, m_spPoint, m_stretchFactor, m_shadowProtect, m_highlightProtect, 1);
        } else if (std::holds_alternative<RGBImagePtr>(baseImg)) {
            auto rgb = std::get<RGBImagePtr>(baseImg);
            auto cloned = cloneRGB(rgb);
            previewResult = StretchingAlgorithm::stretchGhsRGB(cloned, 0.0, 1.0, m_spPoint, m_stretchFactor, m_shadowProtect, m_highlightProtect, 1, m_colorPreserving);
        }
    } else {
        if (std::holds_alternative<GrayscaleImagePtr>(baseImg)) {
            auto gray = std::get<GrayscaleImagePtr>(baseImg);
            auto cloned = cloneGrayscale(gray);
            previewResult = StretchingAlgorithm::stretchHistogramGrayscale(cloned, m_blackpoint, m_whitepoint, m_midpoint);
        } else if (std::holds_alternative<RGBImagePtr>(baseImg)) {
            auto rgb = std::get<RGBImagePtr>(baseImg);
            auto cloned = cloneRGB(rgb);
            previewResult = StretchingAlgorithm::stretchHistogramRGB(cloned, m_blackpoint, m_whitepoint, m_midpoint, m_colorPreserving);
        }
    }

    win->setPreviewImage(previewResult);
}

void StretchingDialog::onApplyClicked() {
    auto win = getActiveImageWindow();
    if (!win) {
        QMessageBox::warning(this, "Apply Error", "No active image found to apply stretching.");
        close();
        return;
    }

    ImageVariant baseImg = win->originalImage();
    ImageVariant finalResult;

    if (m_isGhsMode) {
        if (std::holds_alternative<GrayscaleImagePtr>(baseImg)) {
            auto gray = std::get<GrayscaleImagePtr>(baseImg);
            auto cloned = cloneGrayscale(gray);
            finalResult = StretchingAlgorithm::stretchGhsGrayscale(cloned, 0.0, 1.0, m_spPoint, m_stretchFactor, m_shadowProtect, m_highlightProtect, 1);
        } else if (std::holds_alternative<RGBImagePtr>(baseImg)) {
            auto rgb = std::get<RGBImagePtr>(baseImg);
            auto cloned = cloneRGB(rgb);
            finalResult = StretchingAlgorithm::stretchGhsRGB(cloned, 0.0, 1.0, m_spPoint, m_stretchFactor, m_shadowProtect, m_highlightProtect, 1, m_colorPreserving);
        }
    } else {
        if (std::holds_alternative<GrayscaleImagePtr>(baseImg)) {
            auto gray = std::get<GrayscaleImagePtr>(baseImg);
            auto cloned = cloneGrayscale(gray);
            finalResult = StretchingAlgorithm::stretchHistogramGrayscale(cloned, m_blackpoint, m_whitepoint, m_midpoint);
        } else if (std::holds_alternative<RGBImagePtr>(baseImg)) {
            auto rgb = std::get<RGBImagePtr>(baseImg);
            auto cloned = cloneRGB(rgb);
            finalResult = StretchingAlgorithm::stretchHistogramRGB(cloned, m_blackpoint, m_whitepoint, m_midpoint, m_colorPreserving);
        }
    }

    if (m_previewChk->isChecked()) {
        win->commitPreviewImage(finalResult);
    } else {
        win->setPreviewImage(finalResult);
        win->commitPreviewImage(finalResult);
    }

    m_previewChk->blockSignals(true);
    m_previewChk->setChecked(false);
    m_previewChk->blockSignals(false);
}

void StretchingDialog::onPrefsClicked() {
    QDialog dlg(this);
    dlg.setWindowTitle("Stretching Preferences");
    dlg.resize(320, 200);
    dlg.setStyleSheet(styleSheet());

    QFormLayout* form = new QFormLayout(&dlg);
    form->setContentsMargins(15, 15, 15, 15);
    form->setSpacing(10);

    QComboBox* modeCombo = new QComboBox(&dlg);
    modeCombo->addItem("Color-Preserving (Luminance Stretch)", true);
    modeCombo->addItem("Independent RGB Channels Stretch", false);
    modeCombo->setCurrentIndex(m_colorPreserving ? 0 : 1);
    form->addRow("Stretch Mode:", modeCombo);

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
        m_colorPreserving = modeCombo->currentData().toBool();
        m_threads = threadSpin->value();
        onParameterChanged();
    }
}

std::map<std::string, std::string> StretchingDialog::getConfig() const {
    std::map<std::string, std::string> config;
    config["mode"] = m_isGhsMode ? "ghs" : "histogram";
    config["color_preserving"] = m_colorPreserving ? "true" : "false";
    config["threads"] = std::to_string(m_threads);

    if (m_isGhsMode) {
        config["symmetry_point"] = std::to_string(m_spPoint);
        config["stretch_factor"] = std::to_string(m_stretchFactor);
        config["shadow_protect"] = std::to_string(m_shadowProtect);
        config["highlight_protect"] = std::to_string(m_highlightProtect);
        config["form"] = "1";
    } else {
        config["blackpoint"] = std::to_string(m_blackpoint);
        config["whitepoint"] = std::to_string(m_whitepoint);
        config["midpoint"] = std::to_string(m_midpoint);
    }
    return config;
}

QJsonObject StretchingDialog::serializeState() const {
    QJsonObject obj;
    obj["is_ghs_mode"] = m_isGhsMode;
    // HT params
    obj["blackpoint"] = m_blackpoint;
    obj["whitepoint"] = m_whitepoint;
    obj["midpoint"] = m_midpoint;
    // GHS params
    obj["sp_point"] = m_spPoint;
    obj["stretch_factor"] = m_stretchFactor;
    obj["shadow_protect"] = m_shadowProtect;
    obj["highlight_protect"] = m_highlightProtect;
    // Prefs
    obj["color_preserving"] = m_colorPreserving;
    obj["threads"] = m_threads;
    return obj;
}

void StretchingDialog::restoreState(const QJsonObject& obj) {
    if (obj.contains("blackpoint")) {
        m_blackpoint = obj["blackpoint"].toDouble();
        m_whitepoint = obj["whitepoint"].toDouble(0.0);
        // update HT spinboxes
        m_bSpin->blockSignals(true); m_bSpin->setValue(m_blackpoint); m_bSpin->blockSignals(false);
        m_wSpin->blockSignals(true); m_wSpin->setValue(m_whitepoint); m_wSpin->blockSignals(false);
        if (obj.contains("midpoint")) {
            m_midpoint = obj["midpoint"].toDouble(0.5);
            m_mSpin->blockSignals(true); m_mSpin->setValue(m_midpoint); m_mSpin->blockSignals(false);
        }
    }
    if (obj.contains("sp_point")) {
        m_spPoint = obj["sp_point"].toDouble();
        m_spSpin->blockSignals(true); m_spSpin->setValue(m_spPoint); m_spSpin->blockSignals(false);
    }
    if (obj.contains("stretch_factor")) {
        m_stretchFactor = obj["stretch_factor"].toDouble();
        m_dSpin->blockSignals(true); m_dSpin->setValue(m_stretchFactor); m_dSpin->blockSignals(false);
    }
    if (obj.contains("shadow_protect")) {
        m_shadowProtect = obj["shadow_protect"].toDouble();
        m_shadowSpin->blockSignals(true); m_shadowSpin->setValue(m_shadowProtect); m_shadowSpin->blockSignals(false);
    }
    if (obj.contains("highlight_protect")) {
        m_highlightProtect = obj["highlight_protect"].toDouble();
        m_highlightSpin->blockSignals(true); m_highlightSpin->setValue(m_highlightProtect); m_highlightSpin->blockSignals(false);
    }
    if (obj.contains("color_preserving"))
        m_colorPreserving = obj["color_preserving"].toBool();
    if (obj.contains("threads"))
        m_threads = obj["threads"].toInt();
    if (obj.contains("is_ghs_mode")) {
        m_isGhsMode = obj["is_ghs_mode"].toBool();
        m_tabWidget->setCurrentIndex(m_isGhsMode ? 1 : 0);
    }
}

} // namespace blastro
