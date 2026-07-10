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

#include "RegisterDialog.h"
#include <QJsonObject>
#include "core/Preferences.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDialog>
#include <QCheckBox>
#include <QThread>

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

    // 2. Reference Frame Index
    m_refIdxSpin = new QSpinBox(this);
    m_refIdxSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_refIdxSpin->setRange(0, 9999);
    m_refIdxSpin->setValue(0); // 0 is first frame
    formLayout->addRow("Reference Frame Index:", m_refIdxSpin);

    // 3. Star Detection Method
    m_methodCombo = new QComboBox(this);
    m_methodCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_methodCombo->addItem("Advanced Adaptive", "adaptive");
    m_methodCombo->addItem("Basic Centroid", "centroid");
    m_methodCombo->addItem("Standard Gaussian", "gaussian");
    formLayout->addRow("Detection Method:", m_methodCombo);

    // Transformation Model dropdown
    m_modelCombo = new QComboBox(this);
    m_modelCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_modelCombo->addItem("Rigid Body (Translation + Rotation)", "rigid");
    m_modelCombo->addItem("Affine (Translation + Rotation + Scale + Shear)", "affine");
    formLayout->addRow("Transformation Model:", m_modelCombo);


    // 4. SNR Threshold
    m_snrSpin = new QDoubleSpinBox(this);
    m_snrSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_snrSpin->setRange(1.0, 100.0);
    m_snrSpin->setSingleStep(0.5);
    m_snrSpin->setValue(5.0);
    formLayout->addRow("Detection SNR Min:", m_snrSpin);

    // 5. Min FWHM
    m_minFwhmSpin = new QDoubleSpinBox(this);
    m_minFwhmSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_minFwhmSpin->setRange(0.5, 20.0);
    m_minFwhmSpin->setSingleStep(0.1);
    m_minFwhmSpin->setValue(2.0);
    formLayout->addRow("Min Star FWHM:", m_minFwhmSpin);

    mainLayout->addLayout(formLayout);

    mainLayout->addStretch(1); // Content top-justifies; buttons pin to bottom

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
    dlg.resize(320, 240);
    dlg.setStyleSheet(styleSheet());

    QFormLayout* form = new QFormLayout(&dlg);
    form->setContentsMargins(15, 15, 15, 15);
    form->setSpacing(10);

    QSpinBox* maxStarsSpin = new QSpinBox(&dlg);
    maxStarsSpin->setRange(10, 2000);
    maxStarsSpin->setValue(m_maxStars);
    form->addRow("Max Stars to Detect:", maxStarsSpin);

    QDoubleSpinBox* maxEccSpin = new QDoubleSpinBox(&dlg);
    maxEccSpin->setRange(0.1, 1.0);
    maxEccSpin->setSingleStep(0.05);
    maxEccSpin->setValue(m_maxEccentricity);
    form->addRow("Max Star Eccentricity:", maxEccSpin);

    QDoubleSpinBox* matchTolSpin = new QDoubleSpinBox(&dlg);
    matchTolSpin->setRange(0.1, 10.0);
    matchTolSpin->setSingleStep(0.1);
    matchTolSpin->setValue(m_matchTol);
    form->addRow("Constellation Match Tol (px):", matchTolSpin);

    QDoubleSpinBox* simplexTolSpin = new QDoubleSpinBox(&dlg);
    simplexTolSpin->setDecimals(6);
    simplexTolSpin->setRange(1e-6, 1e-2);
    simplexTolSpin->setSingleStep(1e-5);
    simplexTolSpin->setValue(m_simplexTol);
    form->addRow("Simplex Fit Tolerance:", simplexTolSpin);

    QSpinBox* simplexIterSpin = new QSpinBox(&dlg);
    simplexIterSpin->setRange(10, 2000);
    simplexIterSpin->setValue(m_simplexMaxIter);
    form->addRow("Simplex Max Iterations:", simplexIterSpin);

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
        m_maxEccentricity = maxEccSpin->value();
        m_matchTol = matchTolSpin->value();
        m_simplexTol = simplexTolSpin->value();
        m_simplexMaxIter = simplexIterSpin->value();
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
    config["ref_frame_index"] = std::to_string(m_refIdxSpin->value());
    config["detection_method"] = m_methodCombo->currentData().toString().toStdString();
    config["transformation_model"] = m_modelCombo->currentData().toString().toStdString();

    config["snr_min"] = std::to_string(m_snrSpin->value());
    config["min_fwhm"] = std::to_string(m_minFwhmSpin->value());
    config["max_stars"] = std::to_string(m_maxStars);
    config["max_eccentricity"] = std::to_string(m_maxEccentricity);
    config["match_tolerance"] = std::to_string(m_matchTol);
    config["simplex_tolerance"] = std::to_string(m_simplexTol);
    config["simplex_max_iterations"] = std::to_string(m_simplexMaxIter);
    config["threads"] = std::to_string(m_threads);
    return config;
}

void RegisterDialog::refreshWorkspaceElements() {
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

QJsonObject RegisterDialog::serializeState() const {
    QJsonObject obj;
    obj["detection_method"] = m_methodCombo->currentData().toString();
    obj["transformation_model"] = m_modelCombo->currentData().toString();

    obj["snr_min"] = m_snrSpin->value();
    obj["min_fwhm"] = m_minFwhmSpin->value();
    obj["ref_frame_index"] = m_refIdxSpin->value();
    obj["max_stars"] = m_maxStars;
    obj["max_eccentricity"] = m_maxEccentricity;
    obj["match_tol"] = m_matchTol;
    obj["simplex_tol"] = m_simplexTol;
    obj["simplex_max_iter"] = m_simplexMaxIter;
    obj["threads"] = m_threads;
    return obj;
}

void RegisterDialog::restoreState(const QJsonObject& obj) {
    if (obj.contains("detection_method")) {
        int idx = m_methodCombo->findData(obj["detection_method"].toString());
        if (idx >= 0) m_methodCombo->setCurrentIndex(idx);
    }
    if (obj.contains("transformation_model")) {
        int idx = m_modelCombo->findData(obj["transformation_model"].toString());
        if (idx >= 0) m_modelCombo->setCurrentIndex(idx);
    }

    if (obj.contains("snr_min"))
        m_snrSpin->setValue(obj["snr_min"].toDouble());
    if (obj.contains("min_fwhm"))
        m_minFwhmSpin->setValue(obj["min_fwhm"].toDouble());
    if (obj.contains("ref_frame_index"))
        m_refIdxSpin->setValue(obj["ref_frame_index"].toInt());
    if (obj.contains("max_stars"))
        m_maxStars = obj["max_stars"].toInt();
    if (obj.contains("max_eccentricity"))
        m_maxEccentricity = obj["max_eccentricity"].toDouble();
    if (obj.contains("match_tol"))
        m_matchTol = obj["match_tol"].toDouble();
    if (obj.contains("simplex_tol"))
        m_simplexTol = obj["simplex_tol"].toDouble();
    if (obj.contains("simplex_max_iter"))
        m_simplexMaxIter = obj["simplex_max_iter"].toInt();
    if (obj.contains("threads"))
        m_threads = obj["threads"].toInt();
}

} // namespace blastro
