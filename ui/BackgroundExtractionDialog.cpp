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
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/Preferences.h"
#include "WorkspaceArea.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QMainWindow>

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

static void getLocalPatchStats(const ImageVariant& imgVar, int cx, int cy, int size, double& median, double& stddev) {
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
    double sum = 0.0;
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
                sum += val;
            }
        }
    }

    if (vals.empty()) {
        median = 0.0;
        stddev = 0.0;
        return;
    }

    std::sort(vals.begin(), vals.end());
    median = vals[vals.size() / 2];

    double mean = sum / vals.size();
    double sumSqDiff = 0.0;
    for (float val : vals) {
        double diff = val - mean;
        sumSqDiff += diff * diff;
    }
    stddev = std::sqrt(sumSqDiff / vals.size());
}

static void getGlobalStats(const ImageVariant& imgVar, double& median, double& stddev) {
    int w = 0, h = 0;
    if (std::holds_alternative<GrayscaleImagePtr>(imgVar)) {
        auto img = std::get<GrayscaleImagePtr>(imgVar);
        if (img) { w = img->width(); h = img->height(); }
    } else if (std::holds_alternative<RGBImagePtr>(imgVar)) {
        auto img = std::get<RGBImagePtr>(imgVar);
        if (img) { w = img->width(); h = img->height(); }
    }

    std::vector<float> vals;
    double sum = 0.0;
    for (int y = 0; y < h; y += 4) {
        for (int x = 0; x < w; x += 4) {
            float val = 0.0f;
            if (std::holds_alternative<GrayscaleImagePtr>(imgVar)) {
                val = getPixelValue(imgVar, x, y);
            } else {
                val = (getPixelValue(imgVar, x, y, 0) + getPixelValue(imgVar, x, y, 1) + getPixelValue(imgVar, x, y, 2)) / 3.0f;
            }
            vals.push_back(val);
            sum += val;
        }
    }

    if (vals.empty()) {
        median = 0.0;
        stddev = 0.0;
        return;
    }

    std::sort(vals.begin(), vals.end());
    median = vals[vals.size() / 2];

    double mean = sum / vals.size();
    double sumSqDiff = 0.0;
    for (float val : vals) {
        double diff = val - mean;
        sumSqDiff += diff * diff;
    }
    stddev = std::sqrt(sumSqDiff / vals.size());
}

BackgroundExtractionDialog::BackgroundExtractionDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : AlgorithmDialog(workspace, parent) {
    
    setWindowTitle("Background Gradient Extraction");
    resize(380, 300);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    formLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    formLayout->setSpacing(10);

    // Extraction Method Choice
    m_methodCombo = new QComboBox(this);
    m_methodCombo->addItem("Polynomial Surface Fit");
    m_methodCombo->addItem("Radial Basis Function (RBF)");
    formLayout->addRow("Method:", m_methodCombo);

    // Polynomial parameters widget
    m_polyParamsWidget = new QWidget(this);
    QFormLayout* polyForm = new QFormLayout(m_polyParamsWidget);
    polyForm->setContentsMargins(0, 0, 0, 0);
    polyForm->setSpacing(10);

    QHBoxLayout* orderLayout = new QHBoxLayout();
    m_orderSlider = new QSlider(Qt::Horizontal, this);
    m_orderSlider->setRange(1, 5);
    m_orderSlider->setValue(3);
    m_orderSpin = new QSpinBox(this);
    m_orderSpin->setRange(1, 5);
    m_orderSpin->setValue(3);
    orderLayout->addWidget(m_orderSlider, 1);
    orderLayout->addWidget(m_orderSpin);
    polyForm->addRow("Polynomial Order:", orderLayout);
    formLayout->addRow(m_polyParamsWidget);

    connect(m_orderSlider, &QSlider::valueChanged, m_orderSpin, &QSpinBox::setValue);
    connect(m_orderSpin, qOverload<int>(&QSpinBox::valueChanged), m_orderSlider, &QSlider::setValue);

    // RBF parameters widget
    m_rbfParamsWidget = new QWidget(this);
    QFormLayout* rbfForm = new QFormLayout(m_rbfParamsWidget);
    rbfForm->setContentsMargins(0, 0, 0, 0);
    rbfForm->setSpacing(10);

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
    rbfForm->addRow("RBF Smoothing:", rbfSmoothingLayout);
    formLayout->addRow(m_rbfParamsWidget);
    m_rbfParamsWidget->setVisible(false);

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

    connect(m_methodCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
        m_polyParamsWidget->setVisible(index == 0);
        m_rbfParamsWidget->setVisible(index == 1);
        adjustSize();
    });

    // 2. Sigma Cut (1.0 to 10.0)
    QHBoxLayout* sigmaLayout = new QHBoxLayout();
    m_sigmaSlider = new QSlider(Qt::Horizontal, this);
    m_sigmaSlider->setRange(10, 100); // 1.0 to 10.0 in tenths
    m_sigmaSlider->setValue(30);
    m_sigmaSpin = new QDoubleSpinBox(this);
    m_sigmaSpin->setRange(1.0, 10.0);
    m_sigmaSpin->setSingleStep(0.1);
    m_sigmaSpin->setValue(3.0);
    sigmaLayout->addWidget(m_sigmaSlider, 1);
    sigmaLayout->addWidget(m_sigmaSpin);
    formLayout->addRow("Sigma Rejection Cut:", sigmaLayout);

    connect(m_sigmaSlider, &QSlider::valueChanged, this, [this](int val) {
        m_sigmaSpin->blockSignals(true);
        m_sigmaSpin->setValue(val / 10.0);
        m_sigmaSpin->blockSignals(false);
    });
    connect(m_sigmaSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        m_sigmaSlider->blockSignals(true);
        m_sigmaSlider->setValue(static_cast<int>(val * 10.0));
        m_sigmaSlider->blockSignals(false);
    });

    // Grid Spacing Density
    QHBoxLayout* gridLayout = new QHBoxLayout();
    gridLayout->addWidget(new QLabel("Cols:", this));
    m_gridColsSpin = new QSpinBox(this);
    m_gridColsSpin->setRange(3, 30);
    m_gridColsSpin->setValue(5);
    gridLayout->addWidget(m_gridColsSpin);

    gridLayout->addWidget(new QLabel("Rows:", this));
    m_gridRowsSpin = new QSpinBox(this);
    m_gridRowsSpin->setRange(3, 30);
    m_gridRowsSpin->setValue(5);
    gridLayout->addWidget(m_gridRowsSpin);
    formLayout->addRow("Grid Dimensions:", gridLayout);

    // Bad point rejection controls
    QHBoxLayout* rejectLayout = new QHBoxLayout();
    m_autoExcludeChk = new QCheckBox("Auto-Exclude Stars", this);
    m_autoExcludeChk->setChecked(true);
    rejectLayout->addWidget(m_autoExcludeChk);

    rejectLayout->addWidget(new QLabel("Thresh:", this));
    m_excludeThresholdSpin = new QDoubleSpinBox(this);
    m_excludeThresholdSpin->setRange(0.5, 10.0);
    m_excludeThresholdSpin->setSingleStep(0.1);
    m_excludeThresholdSpin->setValue(1.5);
    rejectLayout->addWidget(m_excludeThresholdSpin);
    formLayout->addRow("Bad Point Rejection:", rejectLayout);

    connect(m_autoExcludeChk, &QCheckBox::toggled, m_excludeThresholdSpin, &QDoubleSpinBox::setEnabled);

    // 3. Channel Equalization Toggle
    QHBoxLayout* togglesLayout = new QHBoxLayout();
    m_equalizeChk = new QCheckBox("Equalize Channels", this);
    m_equalizeChk->setChecked(true);
    togglesLayout->addWidget(m_equalizeChk);
    
    formLayout->addRow("", togglesLayout);

    mainLayout->addLayout(formLayout);

    // Buttons Box
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    QPushButton* prefsBtn = new QPushButton("Preferences...", this);
    connect(prefsBtn, &QPushButton::clicked, this, &BackgroundExtractionDialog::onPrefsClicked);
    btnLayout->addWidget(prefsBtn);

    QPushButton* generateBtn = new QPushButton("Generate Grid", this);
    connect(generateBtn, &QPushButton::clicked, this, &BackgroundExtractionDialog::onGenerateGridClicked);
    btnLayout->addWidget(generateBtn);

    QPushButton* clearBtn = new QPushButton("Clear Points", this);
    connect(clearBtn, &QPushButton::clicked, this, &BackgroundExtractionDialog::onClearPointsClicked);
    btnLayout->addWidget(clearBtn);

    btnLayout->addStretch(1);
    
    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &AlgorithmDialog::onClose);
    btnLayout->addWidget(closeBtn);

    QPushButton* applyBtn = new QPushButton("Apply", this);
    applyBtn->setObjectName("primaryButton");
    connect(applyBtn, &QPushButton::clicked, this, &BackgroundExtractionDialog::onApplyClicked);
    btnLayout->addWidget(applyBtn);

    mainLayout->addLayout(btnLayout);

    if (auto* mdi = findMdiArea()) {
        connect(mdi, &QMdiArea::subWindowActivated, this, &BackgroundExtractionDialog::onSubWindowActivated);
    }
    updateBgeModes();
}

WorkspaceImageWindow* BackgroundExtractionDialog::getActiveImageWindow() const {
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

void BackgroundExtractionDialog::onApplyClicked() {
    auto win = getActiveImageWindow();
    if (!win) {
        QMessageBox::warning(this, "Apply Error", "No active image found to apply background extraction.");
        close();
        return;
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

    QDoubleSpinBox* fracSpin = new QDoubleSpinBox(&dlg);
    fracSpin->setRange(0.0001, 0.5);
    fracSpin->setSingleStep(0.005);
    fracSpin->setDecimals(4);
    fracSpin->setValue(m_sampleFrac);
    form->addRow("Sample Fraction (0.01 = 1%):", fracSpin);

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
        m_sampleFrac = fracSpin->value();
        m_huberDelta = deltaSpin->value();
        m_threads = threadSpin->value();
    }
}

std::map<std::string, std::string> BackgroundExtractionDialog::getConfig() const {
    std::map<std::string, std::string> config;
    config["method"] = m_methodCombo->currentIndex() == 1 ? "RBF" : "Polynomial";
    config["order"] = std::to_string(m_orderSpin->value());
    config["rbf_smoothing"] = std::to_string(m_rbfSmoothingSpin->value());
    config["sigma_cut"] = std::to_string(m_sigmaSpin->value());
    config["equalize"] = m_equalizeChk->isChecked() ? "true" : "false";
    config["sample_frac"] = std::to_string(m_sampleFrac);
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
    
    double marginX = imgSz.width() * 0.08;
    double marginY = imgSz.height() * 0.08;
    
    double stepX = (imgSz.width() - 2 * marginX) / std::max(1, cols - 1);
    double stepY = (imgSz.height() - 2 * marginY) / std::max(1, rows - 1);
    
    // Compute global stats if bad point auto-exclusion is enabled
    bool runExclude = m_autoExcludeChk->isChecked();
    double globalMedian = 0.0, globalStddev = 0.0;
    if (runExclude) {
        getGlobalStats(win->currentImage(), globalMedian, globalStddev);
    }
    
    std::vector<std::pair<double, double>> pts;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            double x = marginX + c * stepX;
            double y = marginY + r * stepY;
            
            // Check for bad point exclusion
            if (runExclude) {
                double localMedian = 0.0, localStddev = 0.0;
                getLocalPatchStats(win->currentImage(), static_cast<int>(std::round(x)), static_cast<int>(std::round(y)), 15, localMedian, localStddev);
                
                // Exclude if local median exceeds threshold of global noise floor,
                // or if local standard deviation is too high
                double thresholdVal = globalMedian + m_excludeThresholdSpin->value() * globalStddev;
                if (localMedian > thresholdVal) {
                    continue; // Exclude
                }
                if (localStddev > 1.5 * globalStddev) {
                    continue; // Exclude
                }
            }
            
            pts.push_back({x, y});
        }
    }
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

QMdiArea* BackgroundExtractionDialog::findMdiArea() const {
    QWidget* p = parentWidget();
    while (p) {
        if (auto* mw = qobject_cast<QMainWindow*>(p)) {
            return mw->findChild<QMdiArea*>();
        }
        p = p->parentWidget();
    }
    return nullptr;
}

void BackgroundExtractionDialog::updateBgeModes() {
    auto* mdi = findMdiArea();
    if (!mdi) return;
    
    auto* activeWin = getActiveImageWindow();
    
    for (auto* sub : mdi->subWindowList()) {
        if (auto* win = qobject_cast<WorkspaceImageWindow*>(sub->widget())) {
            if (win->imageView()) {
                win->imageView()->setBgeMode(win == activeWin);
            }
        }
    }
}

void BackgroundExtractionDialog::disableAllBgeModes() {
    auto* mdi = findMdiArea();
    if (!mdi) return;
    
    for (auto* sub : mdi->subWindowList()) {
        if (auto* win = qobject_cast<WorkspaceImageWindow*>(sub->widget())) {
            if (win->imageView()) {
                win->imageView()->setBgeMode(false);
            }
        }
    }
}

QJsonObject BackgroundExtractionDialog::serializeState() const {
    QJsonObject obj;
    obj["method_index"] = m_methodCombo->currentIndex();
    obj["order"] = m_orderSpin->value();
    obj["rbf_smoothing"] = m_rbfSmoothingSpin->value();
    obj["sigma_cut"] = m_sigmaSpin->value();
    obj["equalize"] = m_equalizeChk->isChecked();
    obj["sample_frac"] = m_sampleFrac;
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
    if (obj.contains("sigma_cut"))
        m_sigmaSpin->setValue(obj["sigma_cut"].toDouble());
    if (obj.contains("equalize"))
        m_equalizeChk->setChecked(obj["equalize"].toBool());
    if (obj.contains("sample_frac"))
        m_sampleFrac = obj["sample_frac"].toDouble();
    if (obj.contains("huber_delta"))
        m_huberDelta = obj["huber_delta"].toDouble();
    if (obj.contains("threads"))
        m_threads = obj["threads"].toInt();
}

} // namespace blastro
