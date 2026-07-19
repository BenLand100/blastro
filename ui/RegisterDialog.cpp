/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "RegisterDialog.h"
#include "WorkspaceImageWindow.h"
#include <QJsonObject>
#include "core/Preferences.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDialog>
#include <QCheckBox>

namespace blastro {

RegisterDialog::RegisterDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : AlgorithmDialog(workspace, parent) {
    
    setWindowTitle("Star Registration");

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
    refreshWorkspaceElements();
    formLayout->addRow("Input Batch:", m_targetInputCombo);

    // 2. Reference Selection Strategy
    m_refStrategyCombo = new QComboBox(this);
    m_refStrategyCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_refStrategyCombo->addItem("Best SNR", "snr");
    m_refStrategyCombo->addItem("Lowest FWHM", "fwhm");
    m_refStrategyCombo->addItem("Manual Index", "manual");
    formLayout->addRow("Reference Selection:", m_refStrategyCombo);

    // 3. Manual Reference Frame Index
    m_refIdxSpin = new QSpinBox(this);
    m_refIdxSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_refIdxSpin->setRange(0, 9999);
    m_refIdxSpin->setValue(0);
    m_refIdxSpin->setEnabled(false); // Only enable when "manual" is selected
    formLayout->addRow("Manual Ref Index:", m_refIdxSpin);

    connect(m_refStrategyCombo, &QComboBox::currentIndexChanged, this, [this]() {
        m_refIdxSpin->setEnabled(m_refStrategyCombo->currentData().toString() == "manual");
    });

    // 4. Transformation Model dropdown
    m_modelCombo = new QComboBox(this);
    m_modelCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_modelCombo->addItem("Rigid Body (Translation + Rotation)", "rigid");
    m_modelCombo->addItem("Affine (Translation + Rotation + Scale + Shear)", "affine");
    formLayout->addRow("Transformation Model:", m_modelCombo);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch(1);

    // Buttons Box
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    QPushButton* prefsBtn = new QPushButton("Preferences...", this);
    connect(prefsBtn, &QPushButton::clicked, this, &RegisterDialog::onPrefsClicked);
    btnLayout->addWidget(prefsBtn);

    btnLayout->addStretch(1);
    
    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &AlgorithmDialog::onClose);
    btnLayout->addWidget(closeBtn);

    QPushButton* runBtn = new QPushButton("Run", this);
    runBtn->setObjectName("primaryButton");
    connect(runBtn, &QPushButton::clicked, this, &RegisterDialog::onRunClicked);
    btnLayout->addWidget(runBtn);

    mainLayout->addLayout(btnLayout);
}

void RegisterDialog::onPrefsClicked() {
    QDialog dlg(this);
    dlg.setWindowTitle("Registration Preferences");
    dlg.resize(320, 180);
    dlg.setStyleSheet(styleSheet());

    QFormLayout* form = new QFormLayout(&dlg);
    form->setContentsMargins(15, 15, 15, 15);
    form->setSpacing(10);

    QSpinBox* maxStarsSpin = new QSpinBox(&dlg);
    maxStarsSpin->setRange(10, 2000);
    maxStarsSpin->setValue(m_maxStars);
    form->addRow("Max Stars to Match:", maxStarsSpin);

    QDoubleSpinBox* matchTolSpin = new QDoubleSpinBox(&dlg);
    matchTolSpin->setRange(0.1, 10.0);
    matchTolSpin->setSingleStep(0.1);
    matchTolSpin->setValue(m_matchTol);
    form->addRow("Constellation Match Tol (px):", matchTolSpin);

    QSpinBox* threadSpin = new QSpinBox(&dlg);
    threadSpin->setRange(1, 64);
    threadSpin->setValue(m_threads > 0 ? m_threads : Preferences::instance().getThreadCount());
    form->addRow("Orchestration Threads:", threadSpin);

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
        m_maxStars = maxStarsSpin->value();
        m_matchTol = matchTolSpin->value();
        m_threads = threadSpin->value();
    }
}

void RegisterDialog::onRunClicked() {
    if (m_targetInputCombo->currentText().isEmpty()) {
        QMessageBox::warning(this, "Configuration Error", "Please select a target image batch to register.");
        return;
    }
    emit algorithmExecuted(algorithmName(), getConfig());
}

std::map<std::string, std::string> RegisterDialog::getConfig() const {
    std::map<std::string, std::string> config;
    config["input_name"] = m_targetInputCombo->currentText().toStdString();
    
    std::string refStrategy = m_refStrategyCombo->currentData().toString().toStdString();
    if (refStrategy == "manual") {
        config["ref_frame_index"] = std::to_string(m_refIdxSpin->value());
        config["reference_strategy"] = "snr"; // fallback
    } else {
        config["ref_frame_index"] = "";
        config["reference_strategy"] = refStrategy;
    }

    config["transformation_model"] = m_modelCombo->currentData().toString().toStdString();
    config["max_stars"] = std::to_string(m_maxStars);
    config["match_tolerance"] = std::to_string(m_matchTol);
    config["threads"] = std::to_string(m_threads);
    return config;
}

void RegisterDialog::refreshWorkspaceElements() {
    QString activeName;
    if (auto win = getActiveImageWindow()) {
        activeName = win->name();
    }
    QString currentText = m_targetInputCombo->currentText();
    m_targetInputCombo->clear();
    auto keys = m_workspace.elementNames();
    for (const auto& name : keys) {
        WorkspaceElement elem = m_workspace.getElement(name);
        if (std::holds_alternative<ImageBatchPtr>(elem)) {
            m_targetInputCombo->addItem(QString::fromStdString(name));
        }
    }
    int idx = -1;
    if (!activeName.isEmpty()) {
        idx = m_targetInputCombo->findText(activeName);
    }
    if (idx < 0 && !currentText.isEmpty()) {
        idx = m_targetInputCombo->findText(currentText);
    }
    if (idx >= 0) {
        m_targetInputCombo->setCurrentIndex(idx);
    } else if (m_targetInputCombo->count() > 0) {
        m_targetInputCombo->setCurrentIndex(0);
    }
}

QJsonObject RegisterDialog::serializeState() const {
    QJsonObject obj;
    obj["reference_strategy"] = m_refStrategyCombo->currentData().toString();
    obj["ref_frame_index"] = m_refIdxSpin->value();
    obj["transformation_model"] = m_modelCombo->currentData().toString();
    obj["max_stars"] = m_maxStars;
    obj["match_tol"] = m_matchTol;
    obj["threads"] = m_threads;
    return obj;
}

void RegisterDialog::restoreState(const QJsonObject& obj) {
    if (obj.contains("reference_strategy")) {
        int idx = m_refStrategyCombo->findData(obj["reference_strategy"].toString());
        if (idx >= 0) {
            m_refStrategyCombo->setCurrentIndex(idx);
            m_refIdxSpin->setEnabled(obj["reference_strategy"].toString() == "manual");
        }
    }
    if (obj.contains("ref_frame_index")) {
        m_refIdxSpin->setValue(obj["ref_frame_index"].toInt());
    }
    if (obj.contains("transformation_model")) {
        int idx = m_modelCombo->findData(obj["transformation_model"].toString());
        if (idx >= 0) m_modelCombo->setCurrentIndex(idx);
    }
    if (obj.contains("max_stars"))
        m_maxStars = obj["max_stars"].toInt();
    if (obj.contains("match_tol"))
        m_matchTol = obj["match_tol"].toDouble();
    if (obj.contains("threads"))
        m_threads = obj["threads"].toInt();
}

} // namespace blastro
