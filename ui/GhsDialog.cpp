#include "GhsDialog.h"
#include "core/Preferences.h"
#include "algorithms/GhsAlgorithm.h"
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

GhsDialog::GhsDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : AlgorithmDialog(workspace, parent),
      m_previewTimer(new QTimer(this)) {
    
    setWindowTitle("Generalized Hyperbolic Stretch (GHS)");
    resize(400, 260);

    m_previewTimer->setSingleShot(true);
    connect(m_previewTimer, &QTimer::timeout, this, &GhsDialog::updatePreview);



    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    formLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    formLayout->setSpacing(10);

    // 1. Stretch Factor D (0.0 to 15.0)
    QHBoxLayout* dLayout = new QHBoxLayout();
    m_dSlider = new QSlider(Qt::Horizontal, this);
    m_dSlider->setRange(0, 150); // 0.0 to 15.0 in tenths
    m_dSlider->setValue(30);
    m_dSpin = new QDoubleSpinBox(this);
    m_dSpin->setRange(0.0, 15.0);
    m_dSpin->setSingleStep(0.1);
    m_dSpin->setValue(3.0);
    dLayout->addWidget(m_dSlider, 1);
    dLayout->addWidget(m_dSpin);
    formLayout->addRow("Stretch Factor (D):", dLayout);

    connect(m_dSlider, &QSlider::valueChanged, this, [this](int val) {
        m_dSpin->blockSignals(true);
        m_dSpin->setValue(val / 10.0);
        m_dSpin->blockSignals(false);
        onParameterChanged();
    });
    connect(m_dSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        m_dSlider->blockSignals(true);
        m_dSlider->setValue(static_cast<int>(val * 10.0));
        m_dSlider->blockSignals(false);
        onParameterChanged();
    });

    // 2. Symmetry Point SP (0.00 to 1.00)
    QHBoxLayout* spLayout = new QHBoxLayout();
    m_spSlider = new QSlider(Qt::Horizontal, this);
    m_spSlider->setRange(0, 1000); // 0.000 to 1.000 in thousandths
    m_spSlider->setValue(100);
    m_spSpin = new QDoubleSpinBox(this);
    m_spSpin->setRange(0.0, 1.0);
    m_spSpin->setSingleStep(0.01);
    m_spSpin->setDecimals(3);
    m_spSpin->setValue(0.1);
    spLayout->addWidget(m_spSlider, 1);
    spLayout->addWidget(m_spSpin);
    formLayout->addRow("Symmetry Point (SP):", spLayout);

    connect(m_spSlider, &QSlider::valueChanged, this, [this](int val) {
        m_spSpin->blockSignals(true);
        m_spSpin->setValue(val / 1000.0);
        m_spSpin->blockSignals(false);
        onParameterChanged();
    });
    connect(m_spSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        m_spSlider->blockSignals(true);
        m_spSlider->setValue(static_cast<int>(val * 1000.0));
        m_spSlider->blockSignals(false);
        onParameterChanged();
    });

    // 3. Local Stretch Intensity b (0.0 to 10.0)
    QHBoxLayout* bLayout = new QHBoxLayout();
    m_bSlider = new QSlider(Qt::Horizontal, this);
    m_bSlider->setRange(0, 100); // 0.0 to 10.0 in tenths
    m_bSlider->setValue(0);
    m_bSpin = new QDoubleSpinBox(this);
    m_bSpin->setRange(0.0, 10.0);
    m_bSpin->setSingleStep(0.1);
    m_bSpin->setValue(0.0);
    bLayout->addWidget(m_bSlider, 1);
    bLayout->addWidget(m_bSpin);
    formLayout->addRow("Local Contrast (b):", bLayout);

    connect(m_bSlider, &QSlider::valueChanged, this, [this](int val) {
        m_bSpin->blockSignals(true);
        m_bSpin->setValue(val / 10.0);
        m_bSpin->blockSignals(false);
        onParameterChanged();
    });
    connect(m_bSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        m_bSlider->blockSignals(true);
        m_bSlider->setValue(static_cast<int>(val * 10.0));
        m_bSlider->blockSignals(false);
        onParameterChanged();
    });

    // 4. Preview Toggle
    m_previewChk = new QCheckBox("Live Preview", this);
    m_previewChk->setChecked(false);
    formLayout->addRow("", m_previewChk);
    connect(m_previewChk, &QCheckBox::stateChanged, this, &GhsDialog::onParameterChanged);

    mainLayout->addLayout(formLayout);

    // Buttons Box
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    QPushButton* prefsBtn = new QPushButton("Preferences...", this);
    connect(prefsBtn, &QPushButton::clicked, this, &GhsDialog::onPrefsClicked);
    btnLayout->addWidget(prefsBtn);

    btnLayout->addStretch(1);
    
    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &AlgorithmDialog::onClose);
    btnLayout->addWidget(closeBtn);

    QPushButton* applyBtn = new QPushButton("Apply", this);
    applyBtn->setObjectName("primaryButton");
    connect(applyBtn, &QPushButton::clicked, this, &GhsDialog::onApplyClicked);
    btnLayout->addWidget(applyBtn);

    mainLayout->addLayout(btnLayout);
}

GhsDialog::~GhsDialog() {
    m_previewTimer->stop();
    if (auto win = getActiveImageWindow()) {
        win->restoreOriginalImage();
    }
}

WorkspaceImageWindow* GhsDialog::getActiveImageWindow() const {
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

void GhsDialog::onParameterChanged() {
    if (m_previewChk->isChecked()) {
        m_previewTimer->start(150); // Debounce
    } else {
        m_previewTimer->stop();
        if (auto win = getActiveImageWindow()) {
            win->restoreOriginalImage();
        }
    }
}

void GhsDialog::updatePreview() {
    auto win = getActiveImageWindow();
    if (!win) return;

    ImageVariant baseImg = win->originalImage();
    if (baseImg.index() == 0 && std::get<0>(baseImg) == nullptr) return;

    double D = m_dSpin->value();
    double SP = m_spSpin->value();
    double b = m_bSpin->value();

    ImageVariant previewResult;

    if (std::holds_alternative<GrayscaleImagePtr>(baseImg)) {
        auto gray = std::get<GrayscaleImagePtr>(baseImg);
        auto cloned = cloneGrayscale(gray);
        previewResult = GhsAlgorithm::stretchGrayscale(cloned, 0.0, 1.0, SP, D, static_cast<int>(b));
    } else if (std::holds_alternative<RGBImagePtr>(baseImg)) {
        auto rgb = std::get<RGBImagePtr>(baseImg);
        auto cloned = cloneRGB(rgb);
        previewResult = GhsAlgorithm::stretchRGB(cloned, 0.0, 1.0, SP, D, static_cast<int>(b), m_colorPreserving);
    }

    win->setPreviewImage(previewResult);
}

void GhsDialog::onApplyClicked() {
    auto win = getActiveImageWindow();
    if (!win) {
        QMessageBox::warning(this, "Apply Error", "No active image found to apply Generalized Hyperbolic Stretch.");
        close();
        return;
    }

    double D = m_dSpin->value();
    double SP = m_spSpin->value();
    double b = m_bSpin->value();

    ImageVariant baseImg = win->originalImage();
    ImageVariant finalResult;

    if (std::holds_alternative<GrayscaleImagePtr>(baseImg)) {
        auto gray = std::get<GrayscaleImagePtr>(baseImg);
        auto cloned = cloneGrayscale(gray);
        finalResult = GhsAlgorithm::stretchGrayscale(cloned, 0.0, 1.0, SP, D, static_cast<int>(b));
    } else if (std::holds_alternative<RGBImagePtr>(baseImg)) {
        auto rgb = std::get<RGBImagePtr>(baseImg);
        auto cloned = cloneRGB(rgb);
        finalResult = GhsAlgorithm::stretchRGB(cloned, 0.0, 1.0, SP, D, static_cast<int>(b), m_colorPreserving);
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

void GhsDialog::closeEvent(QCloseEvent* event) {
    m_previewTimer->stop();
    if (auto win = getActiveImageWindow()) {
        win->restoreOriginalImage();
    }
    QWidget::closeEvent(event);
}

void GhsDialog::onPrefsClicked() {
    QDialog dlg(this);
    dlg.setWindowTitle("GHS Preferences");
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

    QDoubleSpinBox* shadowSpin = new QDoubleSpinBox(&dlg);
    shadowSpin->setRange(0.0, 1.0);
    shadowSpin->setSingleStep(0.05);
    shadowSpin->setValue(m_shadowProtect);
    form->addRow("Shadow Protection:", shadowSpin);

    QDoubleSpinBox* highlightSpin = new QDoubleSpinBox(&dlg);
    highlightSpin->setRange(0.0, 1.0);
    highlightSpin->setSingleStep(0.05);
    highlightSpin->setValue(m_highlightProtect);
    form->addRow("Highlight Protection:", highlightSpin);

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
        m_shadowProtect = shadowSpin->value();
        m_highlightProtect = highlightSpin->value();
        m_threads = threadSpin->value();
        onParameterChanged(); // Refresh preview
    }
}

std::map<std::string, std::string> GhsDialog::getConfig() const {
    std::map<std::string, std::string> config;
    config["stretch_factor"] = std::to_string(m_dSpin->value());
    config["symmetry_point"] = std::to_string(m_spSpin->value());
    config["local_stretch"] = std::to_string(m_bSpin->value());
    config["color_preserving"] = m_colorPreserving ? "true" : "false";
    config["shadow_protect"] = std::to_string(m_shadowProtect);
    config["highlight_protect"] = std::to_string(m_highlightProtect);
    config["threads"] = std::to_string(m_threads);
    return config;
}

} // namespace blastro
