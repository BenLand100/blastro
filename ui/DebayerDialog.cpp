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

#include "DebayerDialog.h"
#include "core/Preferences.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDialog>
#include <QSpinBox>
#include <QCheckBox>
#include <QThread>

namespace blastro {

DebayerDialog::DebayerDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : AlgorithmDialog(workspace, parent) {
    
    setWindowTitle("Debayer");
    resize(360, 220);



    m_outputPattern = "{input}_debayered";

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    formLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    formLayout->setSpacing(8);

    // 1. Target Input ComboBox
    m_targetInputCombo = new QComboBox(this);
    formLayout->addRow("Target Input:", m_targetInputCombo);

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

    // 3. Bayer Pattern ComboBox
    m_patternCombo = new QComboBox(this);
    m_patternCombo->addItem("RGGB", "RGGB");
    m_patternCombo->addItem("BGGR", "BGGR");
    m_patternCombo->addItem("GBRG", "GBRG");
    m_patternCombo->addItem("GRBG", "GRBG");
    formLayout->addRow("Bayer Pattern:", m_patternCombo);

    // 4. Debayer Method ComboBox
    m_methodCombo = new QComboBox(this);
    m_methodCombo->addItem("Bilinear (Full Resolution)", "bilinear");
    m_methodCombo->addItem("Superpixel (Half Resolution)", "superpixel");
    formLayout->addRow("Debayer Method:", m_methodCombo);

    mainLayout->addLayout(formLayout);

    // Buttons Box
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    QPushButton* prefsBtn = new QPushButton("Preferences...", this);
    connect(prefsBtn, &QPushButton::clicked, this, &DebayerDialog::onPrefsClicked);
    btnLayout->addWidget(prefsBtn);

    btnLayout->addStretch(1);
    
    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &AlgorithmDialog::onClose);
    btnLayout->addWidget(closeBtn);

    QPushButton* runBtn = new QPushButton("Run", this);
    runBtn->setObjectName("primaryButton");
    connect(runBtn, &QPushButton::clicked, this, &DebayerDialog::onRunClicked);
    btnLayout->addWidget(runBtn);

    mainLayout->addLayout(btnLayout);
}

void DebayerDialog::onPrefsClicked() {
    QDialog dlg(this);
    dlg.setWindowTitle("Debayer Preferences");
    dlg.resize(280, 140);
    dlg.setStyleSheet(styleSheet());

    QFormLayout* form = new QFormLayout(&dlg);
    form->setContentsMargins(15, 15, 15, 15);
    form->setSpacing(10);

    QCheckBox* equalizeChk = new QCheckBox(&dlg);
    equalizeChk->setChecked(m_greenEqualize);
    form->addRow("Equalize Green Channels:", equalizeChk);

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
        m_greenEqualize = equalizeChk->isChecked();
        m_threads = threadSpin->value();
    }
}

void DebayerDialog::onRunClicked() {
    if (m_targetInputCombo->currentText().isEmpty()) {
        QMessageBox::warning(this, "Configuration Error", "Please select a target image or batch to debayer.");
        return;
    }

    if (m_outputName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Configuration Error", "Please enter a valid output name.");
        return;
    }

    emit algorithmExecuted(algorithmName(), getConfig());
}

std::map<std::string, std::string> DebayerDialog::getConfig() const {
    std::map<std::string, std::string> config;
    config["input_name"] = m_targetInputCombo->currentText().toStdString();
    config["output_name"] = m_outputName->text().trimmed().toStdString();
    config["pattern"] = m_patternCombo->currentData().toString().toStdString();
    config["method"] = m_methodCombo->currentData().toString().toStdString();
    config["green_equalize"] = m_greenEqualize ? "true" : "false";
    config["threads"] = std::to_string(m_threads);
    return config;
}

void DebayerDialog::refreshWorkspaceElements() {
    QString currentText = m_targetInputCombo->currentText();
    m_targetInputCombo->clear();
    auto keys = m_workspace.elementNames();
    for (const auto& name : keys) {
        m_targetInputCombo->addItem(QString::fromStdString(name));
    }
    int idx = m_targetInputCombo->findText(currentText);
    if (idx >= 0) {
        m_targetInputCombo->setCurrentIndex(idx);
    }
}

} // namespace blastro
