#include "BackgroundExtractionDialog.h"
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

namespace blastro {

BackgroundExtractionDialog::BackgroundExtractionDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : AlgorithmDialog(workspace, parent) {
    
    setWindowTitle("Background Gradient Extraction");
    resize(360, 180);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    formLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    formLayout->setSpacing(10);

    // 1. Polynomial Order (1 to 5)
    QHBoxLayout* orderLayout = new QHBoxLayout();
    m_orderSlider = new QSlider(Qt::Horizontal, this);
    m_orderSlider->setRange(1, 5);
    m_orderSlider->setValue(3);
    m_orderSpin = new QSpinBox(this);
    m_orderSpin->setRange(1, 5);
    m_orderSpin->setValue(3);
    orderLayout->addWidget(m_orderSlider, 1);
    orderLayout->addWidget(m_orderSpin);
    formLayout->addRow("Polynomial Order:", orderLayout);

    connect(m_orderSlider, &QSlider::valueChanged, m_orderSpin, &QSpinBox::setValue);
    connect(m_orderSpin, qOverload<int>(&QSpinBox::valueChanged), m_orderSlider, &QSlider::setValue);

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

    btnLayout->addStretch(1);
    
    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &AlgorithmDialog::onClose);
    btnLayout->addWidget(closeBtn);

    QPushButton* applyBtn = new QPushButton("Apply", this);
    applyBtn->setObjectName("primaryButton");
    connect(applyBtn, &QPushButton::clicked, this, &BackgroundExtractionDialog::onApplyClicked);
    btnLayout->addWidget(applyBtn);

    mainLayout->addLayout(btnLayout);
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
    config["order"] = std::to_string(m_orderSpin->value());
    config["sigma_cut"] = std::to_string(m_sigmaSpin->value());
    config["equalize"] = m_equalizeChk->isChecked() ? "true" : "false";
    config["sample_frac"] = std::to_string(m_sampleFrac);
    config["huber_delta"] = std::to_string(m_huberDelta);
    config["threads"] = std::to_string(m_threads);
    return config;
}

} // namespace blastro
