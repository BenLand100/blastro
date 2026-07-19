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

#include "BackgroundExtractionDialog.h"
#include <QJsonObject>
#include "algorithms/BackgroundExtractor.h"
#include "core/MathUtils.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/Preferences.h"
#include "WorkspaceArea.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QMainWindow>
#include <QGroupBox>

#include <QLabel>

namespace blastro {

static float getPixelValue(const ImageVariant& imgVar, int x, int y, int channel = 0) {
    if (std::holds_alternative<GrayscaleImagePtr>(imgVar)) {
        auto img = std::get<GrayscaleImagePtr>(imgVar);
        if (img && x >= 0 && x < img->width() && y >= 0 && y < img->height()) {
            return img->buffer()->pixel(x, y);
        }
    } else if (std::holds_alternative<RGBImagePtr>(imgVar)) {
        auto img = std::get<RGBImagePtr>(imgVar);
        if (img && x >= 0 && x < img->width() && y >= 0 && y < img->height()) {
            if (channel == 0) return img->r()->buffer()->pixel(x, y);
            if (channel == 1) return img->g()->buffer()->pixel(x, y);
            if (channel == 2) return img->b()->buffer()->pixel(x, y);
        }
    }
    return 0.0f;
}

static void getLocalPatchStats(const ImageVariant& imgVar, int cx, int cy, int size, double& median, double& sn) {
    std::vector<float> vals;
    vals.reserve(size * size);
    int w = 0, h = 0;
    if (std::holds_alternative<GrayscaleImagePtr>(imgVar)) {
        auto img = std::get<GrayscaleImagePtr>(imgVar);
        if (img) { w = img->width(); h = img->height(); }
    } else if (std::holds_alternative<RGBImagePtr>(imgVar)) {
        auto img = std::get<RGBImagePtr>(imgVar);
        if (img) { w = img->width(); h = img->height(); }
    }

    int half = size / 2;
    for (int dy = -half; dy <= half; ++dy) {
        for (int dx = -half; dx <= half; ++dx) {
            int px = cx + dx;
            int py = cy + dy;
            if (px >= 0 && px < w && py >= 0 && py < h) {
                float val = 0.0f;
                if (std::holds_alternative<GrayscaleImagePtr>(imgVar)) {
                    val = getPixelValue(imgVar, px, py);
                } else {
                    val = (getPixelValue(imgVar, px, py, 0) + getPixelValue(imgVar, px, py, 1) + getPixelValue(imgVar, px, py, 2)) / 3.0f;
                }
                vals.push_back(val);
            }
        }
    }

    if (vals.empty()) {
        median = 0.0;
        sn = 0.0;
        return;
    }

    median = MathUtils::computeMedian(vals);
    sn = MathUtils::computeRousseeuwSn(vals);
}

static void getGlobalStats(const ImageVariant& imgVar, double& median, double& sn) {
    int w = 0, h = 0;
    if (std::holds_alternative<GrayscaleImagePtr>(imgVar)) {
        auto img = std::get<GrayscaleImagePtr>(imgVar);
        if (img) { w = img->width(); h = img->height(); }
    } else if (std::holds_alternative<RGBImagePtr>(imgVar)) {
        auto img = std::get<RGBImagePtr>(imgVar);
        if (img) { w = img->width(); h = img->height(); }
    }

    std::vector<float> vals;
    for (int y = 0; y < h; y += 4) {
        for (int x = 0; x < w; x += 4) {
            float val = 0.0f;
            if (std::holds_alternative<GrayscaleImagePtr>(imgVar)) {
                val = getPixelValue(imgVar, x, y);
            } else {
                val = (getPixelValue(imgVar, x, y, 0) + getPixelValue(imgVar, x, y, 1) + getPixelValue(imgVar, x, y, 2)) / 3.0f;
            }
            vals.push_back(val);
        }
    }

    if (vals.empty()) {
        median = 0.0;
        sn = 0.0;
        return;
    }

    median = MathUtils::computeMedian(vals);
    sn = MathUtils::computeRousseeuwSn(vals);
}

BackgroundExtractionDialog::BackgroundExtractionDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : AlgorithmDialog(workspace, parent) {
    
    setWindowTitle("Background Gradient Extraction");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    // 1. Extraction Settings Group Box
    QGroupBox* settingsGroup = new QGroupBox("Extraction Settings", this);
    QFormLayout* settingsForm = new QFormLayout(settingsGroup);
    settingsForm->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    settingsForm->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    settingsForm->setSpacing(10);

    // Extraction Method Choice
    m_methodCombo = new QComboBox(this);
    m_methodCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_methodCombo->addItem("Polynomial Surface Fit");
    m_methodCombo->addItem("Radial Basis Function (RBF)");
    settingsForm->addRow("Method:", m_methodCombo);

    // Polynomial Order
    QHBoxLayout* orderLayout = new QHBoxLayout();
    m_orderSlider = new QSlider(Qt::Horizontal, this);
    m_orderSlider->setRange(1, 5);
    m_orderSlider->setValue(3);
    m_orderSpin = new QSpinBox(this);
    m_orderSpin->setRange(1, 5);
    m_orderSpin->setValue(3);
    orderLayout->addWidget(m_orderSlider, 1);
    orderLayout->addWidget(m_orderSpin);
    settingsForm->addRow("Polynomial Order:", orderLayout);

    connect(m_orderSlider, &QSlider::valueChanged, m_orderSpin, &QSpinBox::setValue);
    connect(m_orderSpin, qOverload<int>(&QSpinBox::valueChanged), m_orderSlider, &QSlider::setValue);

    // RBF Smoothing
    QHBoxLayout* rbfSmoothingLayout = new QHBoxLayout();
    m_rbfSmoothingSlider = new QSlider(Qt::Horizontal, this);
    m_rbfSmoothingSlider->setRange(0, 100);
    m_rbfSmoothingSlider->setValue(50);
    m_rbfSmoothingSpin = new QDoubleSpinBox(this);
    m_rbfSmoothingSpin->setRange(0.0, 1000.0);
    m_rbfSmoothingSpin->setSingleStep(0.01);
    m_rbfSmoothingSpin->setValue(0.5);
    m_rbfSmoothingSpin->setToolTip("Smoothing regularizer (lambda). 0 = exact interpolation.");
    rbfSmoothingLayout->addWidget(m_rbfSmoothingSlider, 1);
    rbfSmoothingLayout->addWidget(m_rbfSmoothingSpin);
    settingsForm->addRow("RBF Smoothing:", rbfSmoothingLayout);

    connect(m_rbfSmoothingSlider, &QSlider::valueChanged, this, [this](int val) {
        m_rbfSmoothingSpin->blockSignals(true);
        m_rbfSmoothingSpin->setValue(val / 100.0);
        m_rbfSmoothingSpin->blockSignals(false);
    });
    connect(m_rbfSmoothingSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        m_rbfSmoothingSlider->blockSignals(true);
        if (val >= 0.0 && val <= 1.0) {
            m_rbfSmoothingSlider->setValue(static_cast<int>(val * 100.0));
        } else {
            m_rbfSmoothingSlider->setValue(0);
        }
        m_rbfSmoothingSlider->blockSignals(false);
    });

    // Normalize Checkbox
    QHBoxLayout* togglesLayout = new QHBoxLayout();
    m_normalizeChk = new QCheckBox("Normalize (Preserve Median)", this);
    m_normalizeChk->setChecked(true);
    togglesLayout->addWidget(m_normalizeChk);
    settingsForm->addRow("", togglesLayout);

    mainLayout->addWidget(settingsGroup);

    // Dynamic row switching visibility
    connect(m_methodCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this, settingsForm, orderLayout, rbfSmoothingLayout](int index) {
        settingsForm->setRowVisible(orderLayout, index == 0);
        settingsForm->setRowVisible(rbfSmoothingLayout, index == 1);
        adjustSize();
    });

    // Initial state setup
    settingsForm->setRowVisible(rbfSmoothingLayout, false);

    // 2. Grid Generation Group Box
    QGroupBox* gridGroup = new QGroupBox("Grid Generation", this);
    QFormLayout* gridForm = new QFormLayout(gridGroup);
    gridForm->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    gridForm->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    gridForm->setSpacing(10);

    // Grid Spacing Density
    QHBoxLayout* gridLayout = new QHBoxLayout();
    gridLayout->addWidget(new QLabel("Cols:", this));
    m_gridColsSpin = new QSpinBox(this);
    m_gridColsSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_gridColsSpin->setRange(3, 30);
    m_gridColsSpin->setValue(5);
    gridLayout->addWidget(m_gridColsSpin);

    gridLayout->addWidget(new QLabel("Rows:", this));
    m_gridRowsSpin = new QSpinBox(this);
    m_gridRowsSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_gridRowsSpin->setRange(3, 30);
    m_gridRowsSpin->setValue(5);
    gridLayout->addWidget(m_gridRowsSpin);
    gridForm->addRow("Grid Dimensions:", gridLayout);

    // Bad point rejection controls
    QHBoxLayout* rejectLayout = new QHBoxLayout();
    m_autoExcludeChk = new QCheckBox("Enable Rejection", this);
    m_autoExcludeChk->setChecked(true);
    rejectLayout->addWidget(m_autoExcludeChk);

    rejectLayout->addWidget(new QLabel("Max Dev:", this));
    m_maxDeviationSpin = new QDoubleSpinBox(this);
    m_maxDeviationSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_maxDeviationSpin->setRange(0.5, 10.0);
    m_maxDeviationSpin->setSingleStep(0.5);
    m_maxDeviationSpin->setValue(3.0);
    rejectLayout->addWidget(m_maxDeviationSpin);

    rejectLayout->addWidget(new QLabel("Max Struct:", this));
    m_maxStructureSpin = new QDoubleSpinBox(this);
    m_maxStructureSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_maxStructureSpin->setRange(0.1, 10.0);
    m_maxStructureSpin->setSingleStep(0.1);
    m_maxStructureSpin->setValue(1.5);
    rejectLayout->addWidget(m_maxStructureSpin);
    gridForm->addRow("Bad Point Rejection:", rejectLayout);

    connect(m_autoExcludeChk, &QCheckBox::toggled, this, [this](bool checked) {
        m_maxDeviationSpin->setEnabled(checked);
        m_maxStructureSpin->setEnabled(checked);
    });

    mainLayout->addWidget(gridGroup);

    mainLayout->addStretch(1); // Content top-justifies; buttons pin to bottom

    // Control Box row
    QHBoxLayout* ctrlLayout = new QHBoxLayout();
    QPushButton* generateBtn = new QPushButton("Generate Grid", this);
    connect(generateBtn, &QPushButton::clicked, this, &BackgroundExtractionDialog::onGenerateGridClicked);
    ctrlLayout->addWidget(generateBtn);

    QPushButton* clearBtn = new QPushButton("Clear Points", this);
    connect(clearBtn, &QPushButton::clicked, this, &BackgroundExtractionDialog::onClearPointsClicked);
    ctrlLayout->addWidget(clearBtn);

    ctrlLayout->addStretch(1);

    m_previewChk = new QCheckBox("Show Preview", this);
    m_previewChk->setChecked(false);
    ctrlLayout->addWidget(m_previewChk);

    m_updatePreviewBtn = new QPushButton("Update Preview", this);
    m_updatePreviewBtn->setEnabled(false);
    ctrlLayout->addWidget(m_updatePreviewBtn);

    mainLayout->addLayout(ctrlLayout);

    // Buttons Box
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    QPushButton* prefsBtn = new QPushButton("Preferences...", this);
    connect(prefsBtn, &QPushButton::clicked, this, &BackgroundExtractionDialog::onPrefsClicked);
    btnLayout->addWidget(prefsBtn);

    btnLayout->addStretch(1);
    
    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &AlgorithmDialog::onClose);
    btnLayout->addWidget(closeBtn);

    QPushButton* applyBtn = new QPushButton("Apply", this);
    applyBtn->setObjectName("primaryButton");
    connect(applyBtn, &QPushButton::clicked, this, &BackgroundExtractionDialog::onApplyClicked);
    btnLayout->addWidget(applyBtn);

    mainLayout->addLayout(btnLayout);

    connect(m_previewChk, &QCheckBox::toggled, this, [this](bool checked) {
        m_updatePreviewBtn->setEnabled(checked);
        if (checked) {
            updatePreview();
        } else {
            if (auto win = getActiveImageWindow()) {
                win->restoreOriginalImage();
            }
        }
    });
    connect(m_updatePreviewBtn, &QPushButton::clicked, this, &BackgroundExtractionDialog::updatePreview);

    if (auto* mdi = findWorkspaceArea()) {
        connect(mdi, &QMdiArea::subWindowActivated, this, &BackgroundExtractionDialog::onSubWindowActivated);
    }
    updateBgeModes();
}


void BackgroundExtractionDialog::onApplyClicked() {
    auto win = getActiveImageWindow();
    if (!win) {
        QMessageBox::warning(this, "Apply Error", "No active image found to apply background extraction.");
        close();
        return;
    }

    if (m_previewChk->isChecked()) {
        win->restoreOriginalImage();
        m_previewChk->blockSignals(true);
        m_previewChk->setChecked(false);
        m_previewChk->blockSignals(false);
        m_updatePreviewBtn->setEnabled(false);
    }

    emit algorithmExecuted(algorithmName(), getConfig());
}

void BackgroundExtractionDialog::onPrefsClicked() {
    QDialog dlg(this);
    dlg.setWindowTitle("Extraction Preferences");
    dlg.resize(300, 160);
    dlg.setStyleSheet(styleSheet());

    QFormLayout* form = new QFormLayout(&dlg);
    form->setContentsMargins(15, 15, 15, 15);
    form->setSpacing(10);

    QDoubleSpinBox* deltaSpin = new QDoubleSpinBox(&dlg);
    deltaSpin->setRange(0.1, 100.0);
    deltaSpin->setSingleStep(0.5);
    deltaSpin->setValue(m_huberDelta);
    form->addRow("Huber Loss Delta:", deltaSpin);

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
        m_huberDelta = deltaSpin->value();
        m_threads = threadSpin->value();
    }
}

std::map<std::string, std::string> BackgroundExtractionDialog::getConfig() const {
    std::map<std::string, std::string> config;
    config["method"] = m_methodCombo->currentIndex() == 1 ? "RBF" : "Polynomial";
    config["order"] = std::to_string(m_orderSpin->value());
    config["rbf_smoothing"] = std::to_string(m_rbfSmoothingSpin->value());
    config["normalize"] = m_normalizeChk->isChecked() ? "true" : "false";
    config["grid_size"] = std::to_string(std::max(m_gridColsSpin->value(), m_gridRowsSpin->value()));
    config["auto_exclude"] = m_autoExcludeChk->isChecked() ? "true" : "false";
    config["max_deviation"] = std::to_string(m_maxDeviationSpin->value());
    config["max_structure"] = std::to_string(m_maxStructureSpin->value());
    config["huber_delta"] = std::to_string(m_huberDelta);
    config["threads"] = std::to_string(m_threads);

    if (auto* win = getActiveImageWindow()) {
        std::string activeName = win->name().toStdString();
        config["input_name"] = activeName;
        config["output_name"] = activeName;
    }
    return config;
}

BackgroundExtractionDialog::~BackgroundExtractionDialog() {
    disableAllBgeModes();
}

void BackgroundExtractionDialog::showEvent(QShowEvent* event) {
    AlgorithmDialog::showEvent(event);
    updateBgeModes();
}

void BackgroundExtractionDialog::hideEvent(QHideEvent* event) {
    disableAllBgeModes();
    AlgorithmDialog::hideEvent(event);
}

void BackgroundExtractionDialog::onGenerateGridClicked() {
    auto* win = getActiveImageWindow();
    if (!win || !win->imageView()) return;
    
    QSize imgSz = win->imageView()->currentImageSize();
    if (imgSz.isEmpty()) return;
    
    // Clear previous control points
    win->imageView()->setBgeControlPoints({});
    
    int cols = m_gridColsSpin->value();
    int rows = m_gridRowsSpin->value();
    
    double maxDeviation = 9999.0;
    double maxStructure = 9999.0;
    if (m_autoExcludeChk->isChecked()) {
        maxDeviation = m_maxDeviationSpin->value();
        maxStructure = m_maxStructureSpin->value();
    }
    
    ImageVariant currentImg = win->currentImage();
    GrayscaleImagePtr grayImg;
    if (std::holds_alternative<GrayscaleImagePtr>(currentImg)) {
        grayImg = std::get<GrayscaleImagePtr>(currentImg);
    } else if (std::holds_alternative<RGBImagePtr>(currentImg)) {
        grayImg = std::get<RGBImagePtr>(currentImg)->g();
    }
    
    std::vector<std::pair<double, double>> pts = BackgroundExtractor::generateGridPoints(grayImg, cols, rows, maxDeviation, maxStructure);

    win->imageView()->setBgeControlPoints(pts);
    win->imageView()->setShowBgeControlPoints(true, true);
}

void BackgroundExtractionDialog::onClearPointsClicked() {
    auto* win = getActiveImageWindow();
    if (!win || !win->imageView()) return;
    win->imageView()->setBgeControlPoints({});
}

void BackgroundExtractionDialog::onSubWindowActivated(QMdiSubWindow*) {
    updateBgeModes();
}

void BackgroundExtractionDialog::updateBgeModes() {
    auto* mdi = findWorkspaceArea();
    if (!mdi) return;
    
    auto* activeWin = getActiveImageWindow();
    bool wasPreviewActive = m_previewChk && m_previewChk->isChecked();
    
    for (auto* sub : mdi->subWindowList()) {
        if (auto* win = qobject_cast<WorkspaceImageWindow*>(sub->widget())) {
            if (win->imageView()) {
                win->imageView()->setBgeMode(win == activeWin);
            }
            if (wasPreviewActive && win != activeWin) {
                win->restoreOriginalImage();
            }
        }
    }

    m_previewChk->blockSignals(true);
    m_previewChk->setChecked(false);
    m_previewChk->blockSignals(false);
    m_updatePreviewBtn->setEnabled(false);
}

void BackgroundExtractionDialog::disableAllBgeModes() {
    auto* mdi = findWorkspaceArea();
    if (!mdi) return;
    
    bool wasPreviewActive = m_previewChk && m_previewChk->isChecked();
    
    for (auto* sub : mdi->subWindowList()) {
        if (auto* win = qobject_cast<WorkspaceImageWindow*>(sub->widget())) {
            if (win->imageView()) {
                win->imageView()->setBgeMode(false);
            }
            if (wasPreviewActive) {
                win->restoreOriginalImage();
            }
        }
    }
    
    if (m_previewChk) {
        m_previewChk->blockSignals(true);
        m_previewChk->setChecked(false);
        m_previewChk->blockSignals(false);
        m_updatePreviewBtn->setEnabled(false);
    }
}

QJsonObject BackgroundExtractionDialog::serializeState() const {
    QJsonObject obj;
    obj["method_index"] = m_methodCombo->currentIndex();
    obj["order"] = m_orderSpin->value();
    obj["rbf_smoothing"] = m_rbfSmoothingSpin->value();
    obj["normalize"] = m_normalizeChk->isChecked();
    obj["max_deviation"] = m_maxDeviationSpin->value();
    obj["max_structure"] = m_maxStructureSpin->value();
    obj["huber_delta"] = m_huberDelta;
    obj["threads"] = m_threads;
    return obj;
}

void BackgroundExtractionDialog::restoreState(const QJsonObject& obj) {
    if (obj.contains("method_index"))
        m_methodCombo->setCurrentIndex(obj["method_index"].toInt());
    if (obj.contains("order"))
        m_orderSpin->setValue(obj["order"].toInt());
    if (obj.contains("rbf_smoothing"))
        m_rbfSmoothingSpin->setValue(obj["rbf_smoothing"].toDouble());
    if (obj.contains("normalize"))
        m_normalizeChk->setChecked(obj["normalize"].toBool());
    if (obj.contains("max_deviation"))
        m_maxDeviationSpin->setValue(obj["max_deviation"].toDouble());
    if (obj.contains("max_structure"))
        m_maxStructureSpin->setValue(obj["max_structure"].toDouble());
    if (obj.contains("huber_delta"))
        m_huberDelta = obj["huber_delta"].toDouble();
    if (obj.contains("threads"))
        m_threads = obj["threads"].toInt();
}

void BackgroundExtractionDialog::updatePreview() {
    auto* win = getActiveImageWindow();
    if (!win) return;

    ImageVariant baseImg = win->originalImage();
    if (baseImg.index() == 0 && std::get<0>(baseImg) == nullptr) return;

    std::string method = m_methodCombo->currentIndex() == 1 ? "RBF" : "Polynomial";
    int order = m_orderSpin->value();
    double rbfSmoothing = m_rbfSmoothingSpin->value();
    bool normalize = m_normalizeChk->isChecked();
    int gridSize = std::max(m_gridColsSpin->value(), m_gridRowsSpin->value());
    double maxDeviation = m_maxDeviationSpin->value();
    double maxStructure = m_maxStructureSpin->value();

    std::vector<std::pair<double, double>> ctrlPts = win->imageView() ? win->imageView()->getBgeControlPoints() : std::vector<std::pair<double, double>>();

    ImageVariant previewResult;
    try {
        if (std::holds_alternative<GrayscaleImagePtr>(baseImg)) {
            auto gray = std::get<GrayscaleImagePtr>(baseImg);
            previewResult = BackgroundExtractor::extractGrayscale(
                gray, order, gridSize, m_huberDelta, normalize,
                nullptr, 0, 100, method, rbfSmoothing, ctrlPts, maxDeviation, maxStructure, "Preview");
        } else if (std::holds_alternative<RGBImagePtr>(baseImg)) {
            auto rgb = std::get<RGBImagePtr>(baseImg);
            previewResult = BackgroundExtractor::extractRGB(
                rgb, order, gridSize, m_huberDelta, normalize,
                nullptr, method, rbfSmoothing, ctrlPts, maxDeviation, maxStructure, "Preview");
        }
        win->setPreviewImage(previewResult);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Preview Error", QString("Error during preview generation:\n%1").arg(e.what()));
    }
}

bool BackgroundExtractionDialog::hasActivePreview() const {
    return m_previewChk && m_previewChk->isChecked();
}

void BackgroundExtractionDialog::clearPreview() {
    if (m_previewChk) {
        m_previewChk->blockSignals(true);
        m_previewChk->setChecked(false);
        m_previewChk->blockSignals(false);
    }
    if (m_updatePreviewBtn) {
        m_updatePreviewBtn->setEnabled(false);
    }
    if (auto win = getActiveImageWindow()) {
        win->restoreOriginalImage();
    }
}

QMdiSubWindow* BackgroundExtractionDialog::getTargetWindow() const {
    auto win = getActiveImageWindow();
    if (!win) return nullptr;
    QWidget* p = win->parentWidget();
    while (p) {
        if (auto sub = qobject_cast<QMdiSubWindow*>(p)) {
            return sub;
        }
        p = p->parentWidget();
    }
    return nullptr;
}

} // namespace blastro
