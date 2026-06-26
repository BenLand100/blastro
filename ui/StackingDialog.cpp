#include "StackingDialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDialog>
#include <QSpinBox>
#include <QLabel>
#include <QThread>

namespace blastro {

StackingDialog::StackingDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : AlgorithmDialog(workspace, parent) {
    
    setWindowTitle("Image Stacking Configuration");
    resize(380, 260);



    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    formLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    formLayout->setSpacing(8);

    // 1. Target Input ComboBox (Must be a batch)
    m_targetInputCombo = new QComboBox(this);
    refreshWorkspaceElements();
    formLayout->addRow("Aligned Batch:", m_targetInputCombo);

    // 2. Output Name LineEdit
    m_outputName = new QLineEdit(this);
    m_outputName->setText("stacked_master");
    formLayout->addRow("Output Name:", m_outputName);

    // 3. Stacking Method ComboBox
    m_methodCombo = new QComboBox(this);
    m_methodCombo->addItem("Average (Mean)", "average");
    m_methodCombo->addItem("Median", "median");
    m_methodCombo->addItem("Maximum", "max");
    m_methodCombo->addItem("Minimum", "min");
    formLayout->addRow("Stacking Method:", m_methodCombo);

    // 4. Rejection Method ComboBox
    m_rejectionCombo = new QComboBox(this);
    m_rejectionCombo->addItem("No Rejection", "none");
    m_rejectionCombo->addItem("Winsorized Sigma Clipping", "winsorized_sigma");
    m_rejectionCombo->addItem("Sigma Clipping", "sigma");
    m_rejectionCombo->addItem("Quantile Rejection", "quantile");
    formLayout->addRow("Rejection Method:", m_rejectionCombo);
    connect(m_rejectionCombo, &QComboBox::currentTextChanged, this, &StackingDialog::onRejectionChanged);

    // 5. Stacking Mode ComboBox
    m_modeCombo = new QComboBox(this);
    m_modeCombo->addItem("Full RAM Stacking", "ram");
    m_modeCombo->addItem("2D Chunked (Patch-based)", "chunked");
    m_modeCombo->setCurrentIndex(1); // default to chunked
    formLayout->addRow("Stacking Mode:", m_modeCombo);

    // 5. Low / High Rejection range
    m_lowClipSpin = new QDoubleSpinBox(this);
    m_lowClipSpin->setRange(0.0, 10.0);
    m_lowClipSpin->setSingleStep(0.05);
    m_lowClipSpin->setValue(0.03);

    m_slashLabel = new QLabel("/", this);
    m_slashLabel->setStyleSheet("QLabel { color: #aaaaaa; font-weight: bold; font-size: 12px; } QLabel:disabled { color: #4a4a4a; }");

    m_highClipSpin = new QDoubleSpinBox(this);
    m_highClipSpin->setRange(0.0, 10.0);
    m_highClipSpin->setSingleStep(0.05);
    m_highClipSpin->setValue(0.03);

    QHBoxLayout* clipLayout = new QHBoxLayout();
    clipLayout->setSpacing(6);
    clipLayout->addWidget(m_lowClipSpin);
    clipLayout->addWidget(m_slashLabel);
    clipLayout->addWidget(m_highClipSpin);
    clipLayout->addStretch(1);

    m_clipLabel = new QLabel("Low / High Rejection:", this);
    formLayout->addRow(m_clipLabel, clipLayout);

    mainLayout->addLayout(formLayout);

    // Initial trigger for label adjustments
    onRejectionChanged(m_rejectionCombo->currentText());

    // Buttons Box
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    QPushButton* prefsBtn = new QPushButton("Preferences...", this);
    connect(prefsBtn, &QPushButton::clicked, this, &StackingDialog::onPrefsClicked);
    btnLayout->addWidget(prefsBtn);

    btnLayout->addStretch(1);
    
    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);
    btnLayout->addWidget(closeBtn);

    QPushButton* runBtn = new QPushButton("Run", this);
    runBtn->setObjectName("primaryButton");
    connect(runBtn, &QPushButton::clicked, this, &StackingDialog::onRunClicked);
    btnLayout->addWidget(runBtn);

    mainLayout->addLayout(btnLayout);
}

void StackingDialog::onRejectionChanged(const QString& text) {
    if (text == "No Rejection") {
        if (m_clipLabel) m_clipLabel->setEnabled(false);
        if (m_lowClipSpin) m_lowClipSpin->setEnabled(false);
        if (m_highClipSpin) m_highClipSpin->setEnabled(false);
        if (m_slashLabel) m_slashLabel->setEnabled(false);
    } else if (text == "Quantile Rejection") {
        if (m_clipLabel) m_clipLabel->setEnabled(true);
        if (m_lowClipSpin) m_lowClipSpin->setEnabled(true);
        if (m_highClipSpin) m_highClipSpin->setEnabled(true);
        if (m_slashLabel) m_slashLabel->setEnabled(true);
        // Quantiles are in range [0, 0.5]
        m_lowClipSpin->setRange(0.0, 0.5);
        m_highClipSpin->setRange(0.0, 0.5);
        m_lowClipSpin->setSingleStep(0.01);
        m_highClipSpin->setSingleStep(0.01);
        m_lowClipSpin->setValue(0.03);
        m_highClipSpin->setValue(0.03);
    } else {
        if (m_clipLabel) m_clipLabel->setEnabled(true);
        if (m_lowClipSpin) m_lowClipSpin->setEnabled(true);
        if (m_highClipSpin) m_highClipSpin->setEnabled(true);
        if (m_slashLabel) m_slashLabel->setEnabled(true);
        // Sigma clipping and Winsorized Sigma clipping thresholds are standard deviations
        m_lowClipSpin->setRange(0.1, 10.0);
        m_highClipSpin->setRange(0.1, 10.0);
        m_lowClipSpin->setSingleStep(0.1);
        m_highClipSpin->setSingleStep(0.1);
        m_lowClipSpin->setValue(3.0);
        m_highClipSpin->setValue(3.0);
    }
}

void StackingDialog::onPrefsClicked() {
    QDialog dlg(this);
    dlg.setWindowTitle("Stacking Preferences");
    dlg.resize(300, 160);
    dlg.setStyleSheet(styleSheet());

    QFormLayout* form = new QFormLayout(&dlg);
    form->setContentsMargins(15, 15, 15, 15);
    form->setSpacing(10);

    QComboBox* weightCombo = new QComboBox(&dlg);
    weightCombo->addItem("None (Equal weights)", "none");
    weightCombo->addItem("SNR-based Weighting", "snr");
    weightCombo->addItem("FWHM-based Weighting", "fwhm");
    if (m_weightMethod == "snr") weightCombo->setCurrentIndex(1);
    else if (m_weightMethod == "fwhm") weightCombo->setCurrentIndex(2);
    form->addRow("Frame Weighting:", weightCombo);

    QSpinBox* patchSpin = new QSpinBox(&dlg);
    patchSpin->setRange(16, 4096);
    patchSpin->setSingleStep(16);
    patchSpin->setValue(m_patchSize);
    
    // Toggle the patch size spinbox's enabled state based on the selected stacking mode
    bool isChunked = (m_modeCombo->currentData().toString() == "chunked");
    patchSpin->setEnabled(isChunked);
    
    form->addRow("Stack Patch Size:", patchSpin);

    QSpinBox* threadSpin = new QSpinBox(&dlg);
    threadSpin->setRange(1, 64);
    threadSpin->setValue(m_threads > 0 ? m_threads : QThread::idealThreadCount());
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
        m_weightMethod = weightCombo->currentData().toString().toStdString();
        m_patchSize = patchSpin->value();
        m_threads = threadSpin->value();
    }
}

void StackingDialog::onRunClicked() {
    if (m_targetInputCombo->currentText().isEmpty()) {
        QMessageBox::warning(this, "Configuration Error", "Please select an aligned batch to stack.");
        return;
    }

    if (m_outputName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Configuration Error", "Please enter a valid output name.");
        return;
    }

    emit algorithmExecuted(algorithmName(), getConfig());
}

std::map<std::string, std::string> StackingDialog::getConfig() const {
    std::map<std::string, std::string> config;
    config["input_name"] = m_targetInputCombo->currentText().toStdString();
    config["output_name"] = m_outputName->text().trimmed().toStdString();
    config["method"] = m_methodCombo->currentData().toString().toStdString();
    config["rejection"] = m_rejectionCombo->currentData().toString().toStdString();
    
    std::string rej = m_rejectionCombo->currentData().toString().toStdString();
    if (rej == "quantile") {
        config["quantile_low"] = std::to_string(m_lowClipSpin->value());
        config["quantile_high"] = std::to_string(m_highClipSpin->value());
        config["sigma_low"] = "3.0";
        config["sigma_high"] = "3.0";
    } else {
        config["sigma_low"] = std::to_string(m_lowClipSpin->value());
        config["sigma_high"] = std::to_string(m_highClipSpin->value());
        config["quantile_low"] = "0.03";
        config["quantile_high"] = "0.03";
    }

    config["weight_method"] = m_weightMethod;
    config["stacking_mode"] = m_modeCombo->currentData().toString().toStdString();
    config["patch_size"] = std::to_string(m_patchSize);
    config["threads"] = std::to_string(m_threads);
    return config;
}

void StackingDialog::refreshWorkspaceElements() {
    QString currentText = m_targetInputCombo->currentText();
    m_targetInputCombo->clear();
    auto keys = m_workspace.elementNames();
    for (const auto& name : keys) {
        WorkspaceElement elem = m_workspace.getElement(name);
        if (std::holds_alternative<ImageBatchPtr>(elem)) {
            m_targetInputCombo->addItem(QString::fromStdString(name));
        }
    }
    int idx = m_targetInputCombo->findText(currentText);
    if (idx >= 0) {
        m_targetInputCombo->setCurrentIndex(idx);
    }
}

} // namespace blastro
