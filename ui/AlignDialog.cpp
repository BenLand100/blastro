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

#include "AlignDialog.h"
#include <QJsonObject>
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

AlignDialog::AlignDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : AlgorithmDialog(workspace, parent) {
    
    setWindowTitle("Image Alignment & Drizzle");



    m_outputPattern = "{input}_aligned";

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
    formLayout->addRow("Registered Batch:", m_targetInputCombo);

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

    // 3. Alignment Mode ComboBox
    m_alignMethodCombo = new QComboBox(this);
    m_alignMethodCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_alignMethodCombo->addItem("Interpolate", "interpolate");
    m_alignMethodCombo->addItem("Drizzle", "drizzle");
    formLayout->addRow("Alignment Mode:", m_alignMethodCombo);

    // 4. Interpolation Method ComboBox
    m_interpolationCombo = new QComboBox(this);
    m_interpolationCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_interpolationCombo->addItem("Bilinear (Fast, Softer)", "bilinear");
    m_interpolationCombo->addItem("Bicubic Spline", "bicubic");
    m_interpolationCombo->addItem("Lanczos-3 (High Quality, Sharper)", "lanczos3");
    m_interpolationCombo->addItem("Lanczos-4 (Highest Quality)", "lanczos4");
    m_interpolationCombo->setCurrentIndex(2); // Default to Lanczos-3
    formLayout->addRow("Interpolation Method:", m_interpolationCombo);

    // 5. Drizzle Scale ComboBox
    m_drizzleCombo = new QComboBox(this);
    m_drizzleCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_drizzleCombo->addItem("1.0x Drizzle", 1.0);
    m_drizzleCombo->addItem("1.5x Drizzle", 1.5);
    m_drizzleCombo->addItem("2.0x Drizzle", 2.0);
    m_drizzleCombo->addItem("3.0x Drizzle", 3.0);
    m_drizzleCombo->setCurrentIndex(2); // Default to 2.0x
    formLayout->addRow("Drizzle Scale:", m_drizzleCombo);

    // 6. Drizzle Drop Size SpinBox
    m_drizzleDropSizeSpin = new QDoubleSpinBox(this);
    m_drizzleDropSizeSpin->setRange(0.1, 1.0);
    m_drizzleDropSizeSpin->setValue(1.0);
    m_drizzleDropSizeSpin->setSingleStep(0.1);
    m_drizzleDropSizeSpin->setDecimals(2);
    formLayout->addRow("Drizzle Drop Size:", m_drizzleDropSizeSpin);

    // 7. Alignment Reference Mode ComboBox
    m_refModeCombo = new QComboBox(this);
    m_refModeCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_refModeCombo->addItem("Find Centermost", "average_center");
    m_refModeCombo->addItem("Use Reference", "registration");
    formLayout->addRow("Alignment Reference Mode:", m_refModeCombo);

    connect(m_alignMethodCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AlignDialog::onMethodChanged);
    onMethodChanged(0);


    mainLayout->addLayout(formLayout);

    mainLayout->addStretch(1); // Content top-justifies; buttons pin to bottom

    // Buttons Box
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    QPushButton* prefsBtn = new QPushButton("Preferences...", this);
    connect(prefsBtn, &QPushButton::clicked, this, &AlignDialog::onPrefsClicked);
    btnLayout->addWidget(prefsBtn);

    btnLayout->addStretch(1);
    
    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &AlgorithmDialog::onClose);
    btnLayout->addWidget(closeBtn);

    QPushButton* runBtn = new QPushButton("Run", this);
    runBtn->setObjectName("primaryButton");
    connect(runBtn, &QPushButton::clicked, this, &AlignDialog::onRunClicked);
    btnLayout->addWidget(runBtn);

    mainLayout->addLayout(btnLayout);
}

void AlignDialog::onPrefsClicked() {
    QDialog dlg(this);
    dlg.setWindowTitle("Alignment Preferences");
    dlg.resize(280, 160);
    dlg.setStyleSheet(styleSheet());

    QFormLayout* form = new QFormLayout(&dlg);
    form->setContentsMargins(15, 15, 15, 15);
    form->setSpacing(10);

    QSpinBox* threadSpin = new QSpinBox(&dlg);
    threadSpin->setRange(1, 64);
    threadSpin->setValue(m_threads > 0 ? m_threads : Preferences::instance().getThreadCount());
    form->addRow("Computation Threads:", threadSpin);

    QCheckBox* evictChk = new QCheckBox(&dlg);
    evictChk->setChecked(m_evictCache);
    form->addRow("Evict Memory Cache:", evictChk);

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
        m_evictCache = evictChk->isChecked();
    }
}

void AlignDialog::onRunClicked() {
    if (m_targetInputCombo->currentText().isEmpty()) {
        QMessageBox::warning(this, "Configuration Error", "Please select a registered batch to align.");
        return;
    }

    if (m_outputName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Configuration Error", "Please enter a valid output name.");
        return;
    }

    emit algorithmExecuted(algorithmName(), getConfig());
}

std::map<std::string, std::string> AlignDialog::getConfig() const {
    std::map<std::string, std::string> config;
    config["input_name"] = m_targetInputCombo->currentText().toStdString();
    config["output_name"] = m_outputName->text().trimmed().toStdString();
    config["reference_mode"] = m_refModeCombo->currentData().toString().toStdString();
    config["threads"] = std::to_string(m_threads);
    config["evict_cache"] = m_evictCache ? "true" : "false";

    std::string method = m_alignMethodCombo->currentData().toString().toStdString();
    if (method == "drizzle") {
        config["interpolation_method"] = "drizzle";
        config["drizzle_scale"] = std::to_string(m_drizzleCombo->currentData().toDouble());
        config["drop_shrink"] = std::to_string(m_drizzleDropSizeSpin->value());
    } else {
        config["interpolation_method"] = m_interpolationCombo->currentData().toString().toStdString();
        config["drizzle_scale"] = "1.0";
        config["drop_shrink"] = "1.0";
    }
    return config;
}

void AlignDialog::refreshWorkspaceElements() {
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

QJsonObject AlignDialog::serializeState() const {
    QJsonObject obj;
    obj["align_method"] = m_alignMethodCombo->currentData().toString();
    obj["drizzle_scale"] = m_drizzleCombo->currentData().toDouble();
    obj["reference_mode"] = m_refModeCombo->currentData().toString();
    obj["interpolation_method"] = m_interpolationCombo->currentData().toString();
    obj["drop_shrink"] = m_drizzleDropSizeSpin->value();

    obj["threads"] = m_threads;
    obj["evict_cache"] = m_evictCache;
    return obj;
}

void AlignDialog::restoreState(const QJsonObject& obj) {
    if (obj.contains("align_method")) {
        int idx = m_alignMethodCombo->findData(obj["align_method"].toString());
        if (idx >= 0) m_alignMethodCombo->setCurrentIndex(idx);
    }
    if (obj.contains("drizzle_scale")) {
        double scale = obj["drizzle_scale"].toDouble();
        for (int i = 0; i < m_drizzleCombo->count(); ++i) {
            if (qAbs(m_drizzleCombo->itemData(i).toDouble() - scale) < 0.01) {
                m_drizzleCombo->setCurrentIndex(i);
                break;
            }
        }
    }
    if (obj.contains("reference_mode")) {
        int idx = m_refModeCombo->findData(obj["reference_mode"].toString());
        if (idx >= 0) m_refModeCombo->setCurrentIndex(idx);
    }
    if (obj.contains("interpolation_method")) {
        int idx = m_interpolationCombo->findData(obj["interpolation_method"].toString());
        if (idx >= 0) m_interpolationCombo->setCurrentIndex(idx);
    }
    if (obj.contains("drop_shrink")) {
        m_drizzleDropSizeSpin->setValue(obj["drop_shrink"].toDouble());
    }

    if (obj.contains("threads"))
        m_threads = obj["threads"].toInt();
    if (obj.contains("evict_cache"))
        m_evictCache = obj["evict_cache"].toBool();

    onMethodChanged(m_alignMethodCombo->currentIndex());
}

void AlignDialog::onMethodChanged(int index) {
    Q_UNUSED(index);
    QString method = m_alignMethodCombo->currentData().toString();
    if (method == "drizzle") {
        m_interpolationCombo->setEnabled(false);
        m_drizzleCombo->setEnabled(true);
        m_drizzleDropSizeSpin->setEnabled(true);
    } else {
        m_interpolationCombo->setEnabled(true);
        m_drizzleCombo->setEnabled(false);
        m_drizzleDropSizeSpin->setEnabled(false);
    }
}

} // namespace blastro
