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

#include "StackingDialog.h"
#include <QJsonObject>
#include "core/Preferences.h"
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
    
    setWindowTitle("Image Stacking");



    m_outputPattern = "{input}_stacked";

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    formLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    formLayout->setSpacing(8);

    // 1. Target Input ComboBox (Must be a batch)
    m_targetInputCombo = new QComboBox(this);
    m_targetInputCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    formLayout->addRow("Aligned Batch:", m_targetInputCombo);

    // 2. Output Name LineEdit
    m_outputName = new QLineEdit(this);
    formLayout->addRow("Output Name:", m_outputName);

    connect(m_targetInputCombo, &QComboBox::currentTextChanged, this, [this](const QString& text) {
        if (!text.isEmpty()) {
            QString name = m_outputPattern;
            name.replace("{input}", text);
            m_outputName->setText(name);
        }
    });

    connect(m_outputName, &QLineEdit::textEdited, this, [this](const QString& text) {
        m_outputPattern = text;
    });

    refreshWorkspaceElements();

    if (!m_targetInputCombo->currentText().isEmpty()) {
        QString name = m_outputPattern;
        name.replace("{input}", m_targetInputCombo->currentText());
        m_outputName->setText(name);
    }

    // 3. Stacking Method ComboBox
    m_methodCombo = new QComboBox(this);
    m_methodCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_methodCombo->addItem("Average (Mean)", "average");
    m_methodCombo->addItem("Median", "median");
    m_methodCombo->addItem("Maximum", "max");
    m_methodCombo->addItem("Minimum", "min");
    formLayout->addRow("Stacking Method:", m_methodCombo);

    // 4. Rejection Method ComboBox
    m_rejectionCombo = new QComboBox(this);
    m_rejectionCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_rejectionCombo->addItem("No Rejection", "none");
    m_rejectionCombo->addItem("Winsorized Sigma Clipping", "winsorized_sigma");
    m_rejectionCombo->addItem("Sigma Clipping", "sigma");
    m_rejectionCombo->addItem("Quantile Rejection", "quantile");
    m_rejectionCombo->setCurrentIndex(1); // Default to Winsorized Sigma Clipping
    formLayout->addRow("Rejection Method:", m_rejectionCombo);
    connect(m_rejectionCombo, &QComboBox::currentTextChanged, this, &StackingDialog::onRejectionChanged);

    // 5. Low / High Rejection range
    m_lowClipSpin = new QDoubleSpinBox(this);
    m_lowClipSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_lowClipSpin->setRange(0.0, 10.0);
    m_lowClipSpin->setSingleStep(0.05);
    m_lowClipSpin->setValue(0.03);
 
    m_slashLabel = new QLabel("/", this);
    m_slashLabel->setStyleSheet("QLabel { color: #aaaaaa; font-weight: bold; font-size: 12px; } QLabel:disabled { color: #4a4a4a; }");
 
    m_highClipSpin = new QDoubleSpinBox(this);
    m_highClipSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_highClipSpin->setRange(0.0, 10.0);
    m_highClipSpin->setSingleStep(0.05);
    m_highClipSpin->setValue(0.03);
 
    QHBoxLayout* clipLayout = new QHBoxLayout();
    clipLayout->setSpacing(6);
    clipLayout->addWidget(m_lowClipSpin);
    clipLayout->addWidget(m_slashLabel);
    clipLayout->addWidget(m_highClipSpin);

    m_clipLabel = new QLabel("Low / High Rejection:", this);
    formLayout->addRow(m_clipLabel, clipLayout);

    m_scaleAdditiveChk = new QCheckBox("Additive Normalization (Median to Reference)", this);
    m_scaleAdditiveChk->setChecked(true);
    formLayout->addRow("", m_scaleAdditiveChk);

    m_scaleMultiplicativeChk = new QCheckBox("Multiplicative Normalization (Sn to Reference)", this);
    m_scaleMultiplicativeChk->setChecked(true);
    formLayout->addRow("", m_scaleMultiplicativeChk);

    mainLayout->addLayout(formLayout);

    mainLayout->addStretch(1); // Content top-justifies; buttons pin to bottom

    // Initial trigger for label adjustments
    onRejectionChanged(m_rejectionCombo->currentText());

    // Buttons Box
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    QPushButton* prefsBtn = new QPushButton("Preferences...", this);
    connect(prefsBtn, &QPushButton::clicked, this, &StackingDialog::onPrefsClicked);
    btnLayout->addWidget(prefsBtn);

    btnLayout->addStretch(1);
    
    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &AlgorithmDialog::onClose);
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

    QComboBox* modeCombo = new QComboBox(&dlg);
    modeCombo->addItem("Full RAM Stacking", "ram");
    modeCombo->addItem("2D Chunked Stacking (Disk-Backed)", "chunked");
    int modeIdx = modeCombo->findData(QString::fromStdString(Preferences::instance().getStackingMode()));
    if (modeIdx >= 0) modeCombo->setCurrentIndex(modeIdx);
    form->addRow("Stacking Mode:", modeCombo);

    QSpinBox* patchSpin = new QSpinBox(&dlg);
    patchSpin->setRange(16, 4096);
    patchSpin->setSingleStep(16);
    patchSpin->setValue(m_patchSize);
    
    // Toggle the patch size spinbox's enabled state based on the selected stacking mode
    bool isChunked = (modeCombo->currentData().toString() == "chunked");
    patchSpin->setEnabled(isChunked);
    
    connect(modeCombo, &QComboBox::currentIndexChanged, this, [patchSpin, modeCombo]() {
        bool isChunked = (modeCombo->currentData().toString() == "chunked");
        patchSpin->setEnabled(isChunked);
    });
    
    form->addRow("Stack Patch Size:", patchSpin);

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
        m_weightMethod = weightCombo->currentData().toString().toStdString();
        m_patchSize = patchSpin->value();
        m_threads = threadSpin->value();
        Preferences::instance().setStackingMode(modeCombo->currentData().toString().toStdString());
        Preferences::instance().save();
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
    config["stacking_mode"] = Preferences::instance().getStackingMode();
    config["patch_size"] = std::to_string(m_patchSize);
    config["threads"] = std::to_string(m_threads);
    config["scale_additive"] = m_scaleAdditiveChk->isChecked() ? "true" : "false";
    config["scale_multiplicative"] = m_scaleMultiplicativeChk->isChecked() ? "true" : "false";
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

QJsonObject StackingDialog::serializeState() const {
    QJsonObject obj;
    obj["method"] = m_methodCombo->currentData().toString();
    obj["rejection"] = m_rejectionCombo->currentData().toString();
    obj["sigma_low"] = m_lowClipSpin->value();
    obj["sigma_high"] = m_highClipSpin->value();
    obj["weight_method"] = QString::fromStdString(m_weightMethod);
    obj["patch_size"] = m_patchSize;
    obj["threads"] = m_threads;
    obj["scale_additive"] = m_scaleAdditiveChk->isChecked();
    obj["scale_multiplicative"] = m_scaleMultiplicativeChk->isChecked();
    return obj;
}

void StackingDialog::restoreState(const QJsonObject& obj) {
    if (obj.contains("method")) {
        int idx = m_methodCombo->findData(obj["method"].toString());
        if (idx >= 0) m_methodCombo->setCurrentIndex(idx);
    }
    if (obj.contains("rejection")) {
        int idx = m_rejectionCombo->findData(obj["rejection"].toString());
        if (idx >= 0) m_rejectionCombo->setCurrentIndex(idx);
    }
    if (obj.contains("sigma_low"))
        m_lowClipSpin->setValue(obj["sigma_low"].toDouble());
    if (obj.contains("sigma_high"))
        m_highClipSpin->setValue(obj["sigma_high"].toDouble());
    if (obj.contains("weight_method"))
        m_weightMethod = obj["weight_method"].toString().toStdString();
    if (obj.contains("patch_size"))
        m_patchSize = obj["patch_size"].toInt();
    if (obj.contains("threads"))
        m_threads = obj["threads"].toInt();
    if (obj.contains("scale_additive"))
        m_scaleAdditiveChk->setChecked(obj["scale_additive"].toBool());
    if (obj.contains("scale_multiplicative"))
        m_scaleMultiplicativeChk->setChecked(obj["scale_multiplicative"].toBool());
}

} // namespace blastro
