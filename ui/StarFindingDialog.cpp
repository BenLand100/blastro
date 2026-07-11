/*
 * BLastro - Astronomical Image Processing Software
 * Copyright (C) 2026 Benjamin Land
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "StarFindingDialog.h"
#include <QJsonObject>
#include "core/Preferences.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDialog>

namespace blastro {

StarFindingDialog::StarFindingDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : AlgorithmDialog(workspace, parent) {
    
    setWindowTitle("Star Finding");

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

    // 2. Star Detection Method
    m_methodCombo = new QComboBox(this);
    m_methodCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_methodCombo->addItem("Advanced Adaptive", "adaptive");
    m_methodCombo->addItem("Basic Centroid", "centroid");
    m_methodCombo->addItem("Standard Gaussian", "gaussian");
    formLayout->addRow("Detection Method:", m_methodCombo);

    // 3. SNR Threshold
    m_snrSpin = new QDoubleSpinBox(this);
    m_snrSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_snrSpin->setRange(1.0, 100.0);
    m_snrSpin->setSingleStep(0.5);
    m_snrSpin->setValue(4.0);
    formLayout->addRow("Detection SNR Min:", m_snrSpin);

    // 4. Min FWHM
    m_minFwhmSpin = new QDoubleSpinBox(this);
    m_minFwhmSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_minFwhmSpin->setRange(0.5, 20.0);
    m_minFwhmSpin->setSingleStep(0.1);
    m_minFwhmSpin->setValue(1.5);
    formLayout->addRow("Min Star FWHM:", m_minFwhmSpin);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch(1);

    // Buttons Box
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    QPushButton* prefsBtn = new QPushButton("Preferences...", this);
    connect(prefsBtn, &QPushButton::clicked, this, &StarFindingDialog::onPrefsClicked);
    btnLayout->addWidget(prefsBtn);

    btnLayout->addStretch(1);
    
    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &AlgorithmDialog::onClose);
    btnLayout->addWidget(closeBtn);

    QPushButton* runBtn = new QPushButton("Run", this);
    runBtn->setObjectName("primaryButton");
    connect(runBtn, &QPushButton::clicked, this, &StarFindingDialog::onRunClicked);
    btnLayout->addWidget(runBtn);

    mainLayout->addLayout(btnLayout);
}

void StarFindingDialog::onPrefsClicked() {
    QDialog dlg(this);
    dlg.setWindowTitle("Star Finding Preferences");
    dlg.resize(320, 200);
    dlg.setStyleSheet(styleSheet());

    QFormLayout* form = new QFormLayout(&dlg);
    form->setContentsMargins(15, 15, 15, 15);
    form->setSpacing(10);

    QSpinBox* maxStarsSpin = new QSpinBox(&dlg);
    maxStarsSpin->setRange(10, 50000);
    maxStarsSpin->setValue(m_maxStars);
    form->addRow("Max Stars to Detect:", maxStarsSpin);

    QSpinBox* maxRefinedSpin = new QSpinBox(&dlg);
    maxRefinedSpin->setRange(10, 5000);
    maxRefinedSpin->setValue(m_maxRefinedStars);
    form->addRow("Max Stars to Refine:", maxRefinedSpin);

    QDoubleSpinBox* maxEccSpin = new QDoubleSpinBox(&dlg);
    maxEccSpin->setRange(0.1, 1.0);
    maxEccSpin->setSingleStep(0.05);
    maxEccSpin->setValue(m_maxEccentricity);
    form->addRow("Max Star Eccentricity:", maxEccSpin);

    QSpinBox* threadSpin = new QSpinBox(&dlg);
    threadSpin->setRange(1, 64);
    threadSpin->setValue(m_threads > 0 ? m_threads : Preferences::instance().getThreadCount());
    form->addRow("Execution Threads:", threadSpin);

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
        m_maxRefinedStars = maxRefinedSpin->value();
        m_maxEccentricity = maxEccSpin->value();
        m_threads = threadSpin->value();
    }
}

void StarFindingDialog::onRunClicked() {
    if (m_targetInputCombo->currentText().isEmpty()) {
        QMessageBox::warning(this, "Configuration Error", "Please select a target image batch.");
        return;
    }
    emit algorithmExecuted(algorithmName(), getConfig());
}

std::map<std::string, std::string> StarFindingDialog::getConfig() const {
    std::map<std::string, std::string> config;
    config["input_name"] = m_targetInputCombo->currentText().toStdString();
    config["detection_method"] = m_methodCombo->currentData().toString().toStdString();
    config["snr_min"] = std::to_string(m_snrSpin->value());
    config["min_fwhm"] = std::to_string(m_minFwhmSpin->value());
    config["max_stars"] = std::to_string(m_maxStars);
    config["max_refined_stars"] = std::to_string(m_maxRefinedStars);
    config["max_eccentricity"] = std::to_string(m_maxEccentricity);
    config["threads"] = std::to_string(m_threads);
    return config;
}

void StarFindingDialog::refreshWorkspaceElements() {
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

QJsonObject StarFindingDialog::serializeState() const {
    QJsonObject obj;
    obj["detection_method"] = m_methodCombo->currentData().toString();
    obj["snr_min"] = m_snrSpin->value();
    obj["min_fwhm"] = m_minFwhmSpin->value();
    obj["max_stars"] = m_maxStars;
    obj["max_refined_stars"] = m_maxRefinedStars;
    obj["max_eccentricity"] = m_maxEccentricity;
    obj["threads"] = m_threads;
    return obj;
}

void StarFindingDialog::restoreState(const QJsonObject& obj) {
    if (obj.contains("detection_method")) {
        int idx = m_methodCombo->findData(obj["detection_method"].toString());
        if (idx >= 0) m_methodCombo->setCurrentIndex(idx);
    }
    if (obj.contains("snr_min"))
        m_snrSpin->setValue(obj["snr_min"].toDouble());
    if (obj.contains("min_fwhm"))
        m_minFwhmSpin->setValue(obj["min_fwhm"].toDouble());
    if (obj.contains("max_stars"))
        m_maxStars = obj["max_stars"].toInt();
    if (obj.contains("max_refined_stars"))
        m_maxRefinedStars = obj["max_refined_stars"].toInt();
    if (obj.contains("max_eccentricity"))
        m_maxEccentricity = obj["max_eccentricity"].toDouble();
    if (obj.contains("threads"))
        m_threads = obj["threads"].toInt();
}

} // namespace blastro
