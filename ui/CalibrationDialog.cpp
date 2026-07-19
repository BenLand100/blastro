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

#include "CalibrationDialog.h"
#include "WorkspaceImageWindow.h"
#include <QJsonObject>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>

namespace blastro {

CalibrationDialog::CalibrationDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : AlgorithmDialog(workspace, parent) {
    
    setWindowTitle("Image Calibration");



    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    m_outputPattern = "{input}_calibrated";

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    formLayout->setSpacing(8);

    // 1. Target Input ComboBox (Images or Batches)
    m_targetInputCombo = new QComboBox(this);
    m_targetInputCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    // 2. Output Name LineEdit
    m_outputName = new QLineEdit(this);
    m_outputName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    // Calibration comboboxes
    m_biasCombo = new QComboBox(this);
    m_biasCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_darkCombo = new QComboBox(this);
    m_darkCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_flatCombo = new QComboBox(this);
    m_flatCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
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

    formLayout->addRow("Target Input:", m_targetInputCombo);
    formLayout->addRow("Output Name:", m_outputName);
    formLayout->addRow("Bias Image (Optional):", m_biasCombo);
    formLayout->addRow("Dark Image (Optional):", m_darkCombo);
    formLayout->addRow("Flat Image (Optional):", m_flatCombo);

    mainLayout->addLayout(formLayout);

    mainLayout->addStretch(1); // Content top-justifies; buttons pin to bottom

    // 6. Buttons Box
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch(1);
    
    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &AlgorithmDialog::onClose);
    btnLayout->addWidget(closeBtn);

    QPushButton* runBtn = new QPushButton("Run", this);
    runBtn->setObjectName("primaryButton");
    connect(runBtn, &QPushButton::clicked, this, &CalibrationDialog::onRunClicked);
    btnLayout->addWidget(runBtn);

    mainLayout->addLayout(btnLayout);
}

void CalibrationDialog::onRunClicked() {
    if (m_targetInputCombo->currentText().isEmpty()) {
        QMessageBox::warning(this, "Configuration Error", "Please select a target image or batch to calibrate.");
        return;
    }

    if (m_outputName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Configuration Error", "Please enter a valid output name.");
        return;
    }

    emit algorithmExecuted(algorithmName(), getConfig());
}

std::map<std::string, std::string> CalibrationDialog::getConfig() const {
    std::map<std::string, std::string> config;
    config["input_name"] = m_targetInputCombo->currentText().toStdString();
    config["output_name"] = m_outputName->text().trimmed().toStdString();
    config["bias_name"] = m_biasCombo->currentData().toString().toStdString();
    config["dark_name"] = m_darkCombo->currentData().toString().toStdString();
    config["flat_name"] = m_flatCombo->currentData().toString().toStdString();
    return config;
}

void CalibrationDialog::refreshWorkspaceElements() {
    QString activeName;
    if (auto win = getActiveImageWindow()) {
        activeName = win->name();
    }
    QString curTarget = m_targetInputCombo->currentText();
    QString curBias = m_biasCombo->currentData().toString();
    QString curDark = m_darkCombo->currentData().toString();
    QString curFlat = m_flatCombo->currentData().toString();

    m_targetInputCombo->clear();
    m_biasCombo->clear();
    m_darkCombo->clear();
    m_flatCombo->clear();

    auto keys = m_workspace.elementNames();

    // Populate target
    for (const auto& name : keys) {
        m_targetInputCombo->addItem(QString::fromStdString(name));
    }

    // Populate calibration frames
    auto populateCalibrationCombo = [&](QComboBox* combo, const QString& currentValue) {
        combo->addItem("<None>", "");
        for (const auto& name : keys) {
            WorkspaceElement elem = m_workspace.getElement(name);
            if (std::holds_alternative<GrayscaleImagePtr>(elem) || std::holds_alternative<RGBImagePtr>(elem)) {
                combo->addItem(QString::fromStdString(name), QString::fromStdString(name));
            }
        }
        int idx = combo->findData(currentValue);
        if (idx >= 0) {
            combo->setCurrentIndex(idx);
        } else {
            combo->setCurrentIndex(0);
        }
    };

    populateCalibrationCombo(m_biasCombo, curBias);
    populateCalibrationCombo(m_darkCombo, curDark);
    populateCalibrationCombo(m_flatCombo, curFlat);

    int idx = -1;
    if (!activeName.isEmpty()) {
        idx = m_targetInputCombo->findText(activeName);
    }
    if (idx < 0 && !curTarget.isEmpty()) {
        idx = m_targetInputCombo->findText(curTarget);
    }
    if (idx >= 0) {
        m_targetInputCombo->setCurrentIndex(idx);
    } else if (m_targetInputCombo->count() > 0) {
        m_targetInputCombo->setCurrentIndex(0);
    }
}

QJsonObject CalibrationDialog::serializeState() const {
    QJsonObject obj;
    obj["bias_name"] = m_biasCombo->currentData().toString();
    obj["dark_name"] = m_darkCombo->currentData().toString();
    obj["flat_name"] = m_flatCombo->currentData().toString();
    return obj;
}

void CalibrationDialog::restoreState(const QJsonObject& obj) {
    // Restore calibration master selections if the named elements exist in workspace
    if (obj.contains("bias_name")) {
        int idx = m_biasCombo->findData(obj["bias_name"].toString());
        if (idx >= 0) m_biasCombo->setCurrentIndex(idx);
    }
    if (obj.contains("dark_name")) {
        int idx = m_darkCombo->findData(obj["dark_name"].toString());
        if (idx >= 0) m_darkCombo->setCurrentIndex(idx);
    }
    if (obj.contains("flat_name")) {
        int idx = m_flatCombo->findData(obj["flat_name"].toString());
        if (idx >= 0) m_flatCombo->setCurrentIndex(idx);
    }
}

} // namespace blastro
