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

#include "PlatesolveDialog.h"
#include "WorkspaceImageWindow.h"
#include <QJsonObject>
#include "core/Preferences.h"
#include "core/GrayscaleImage.h"
#include "core/RGBImage.h"
#include "core/ImageBatch.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDialog>
#include <QCheckBox>

namespace blastro {

PlatesolveDialog::PlatesolveDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : AlgorithmDialog(workspace, parent) {
    
    setWindowTitle("Platesolving");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    formLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    formLayout->setSpacing(8);

    // 1. Target Input ComboBox (Single images or batches)
    m_targetInputCombo = new QComboBox(this);
    m_targetInputCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    refreshWorkspaceElements();
    formLayout->addRow("Input Element:", m_targetInputCombo);

    connect(m_targetInputCombo, &QComboBox::currentTextChanged, this, &PlatesolveDialog::populateHintsFromElement);

    // 2. Solver engine
    m_solverCombo = new QComboBox(this);
    m_solverCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_solverCombo->addItem("ASTAP", "astap");
    m_solverCombo->addItem("solve-field (Astrometry.net)", "solve-field");
    formLayout->addRow("Solver Engine:", m_solverCombo);

    // 3. Hints: RA / DEC
    m_blindSolveChk = new QCheckBox("Blind Solve", this);
    m_blindSolveChk->setChecked(true);
    formLayout->addRow("", m_blindSolveChk);

    m_raHintSpin = new QDoubleSpinBox(this);
    m_raHintSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_raHintSpin->setRange(0.0, 360.0);
    m_raHintSpin->setValue(0.0);
    m_raHintSpin->setSuffix(" deg");
    m_raHintSpin->setEnabled(false);
    formLayout->addRow("RA Hint:", m_raHintSpin);

    m_decHintSpin = new QDoubleSpinBox(this);
    m_decHintSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_decHintSpin->setRange(-90.0, 90.0);
    m_decHintSpin->setValue(0.0);
    m_decHintSpin->setSuffix(" deg");
    m_decHintSpin->setEnabled(false);
    formLayout->addRow("DEC Hint:", m_decHintSpin);

    connect(m_blindSolveChk, &QCheckBox::toggled, this, [this](bool checked) {
        m_raHintSpin->setDisabled(checked);
        m_decHintSpin->setDisabled(checked);
    });

    // 4. Hints: Focal Length / Pixel Size
    m_focalLengthSpin = new QDoubleSpinBox(this);
    m_focalLengthSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_focalLengthSpin->setRange(0.0, 100000.0);
    m_focalLengthSpin->setValue(0.0);
    m_focalLengthSpin->setSuffix(" mm (0 for auto)");
    formLayout->addRow("Focal Length:", m_focalLengthSpin);

    m_pixelSizeSpin = new QDoubleSpinBox(this);
    m_pixelSizeSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pixelSizeSpin->setRange(0.0, 100.0);
    m_pixelSizeSpin->setValue(0.0);
    m_pixelSizeSpin->setSuffix(" um (0 for auto)");
    formLayout->addRow("Pixel Size:", m_pixelSizeSpin);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch(1);

    // Buttons Box
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch(1);
    
    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &AlgorithmDialog::onClose);
    btnLayout->addWidget(closeBtn);

    QPushButton* runBtn = new QPushButton("Run", this);
    runBtn->setObjectName("primaryButton");
    connect(runBtn, &QPushButton::clicked, this, &PlatesolveDialog::onRunClicked);
    btnLayout->addWidget(runBtn);

    mainLayout->addLayout(btnLayout);

    // Populate hints from the initially selected element (if any)
    if (!m_targetInputCombo->currentText().isEmpty()) {
        populateHintsFromElement(m_targetInputCombo->currentText());
    }
}

void PlatesolveDialog::onRunClicked() {
    if (m_targetInputCombo->currentText().isEmpty()) {
        QMessageBox::warning(this, "Configuration Error", "Please select a target element to platesolve.");
        return;
    }

    emit algorithmExecuted(algorithmName(), getConfig());
}

std::map<std::string, std::string> PlatesolveDialog::getConfig() const {
    std::map<std::string, std::string> config;
    config["input_name"] = m_targetInputCombo->currentText().toStdString();
    config["solver"] = m_solverCombo->currentData().toString().toStdString();
    if (m_blindSolveChk->isChecked()) {
        config["ra_hint"] = "-1.0";
        config["dec_hint"] = "-99.0";
    } else {
        config["ra_hint"] = std::to_string(m_raHintSpin->value());
        config["dec_hint"] = std::to_string(m_decHintSpin->value());
    }
    config["focal_length"] = std::to_string(m_focalLengthSpin->value());
    config["pixel_size"] = std::to_string(m_pixelSizeSpin->value());
    return config;
}

void PlatesolveDialog::refreshWorkspaceElements() {
    QString activeName;
    if (auto win = getActiveImageWindow()) {
        activeName = win->name();
    }
    QString currentText = m_targetInputCombo->currentText();
    m_targetInputCombo->clear();
    auto keys = m_workspace.elementNames();
    for (const auto& name : keys) {
        m_targetInputCombo->addItem(QString::fromStdString(name));
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

void PlatesolveDialog::populateHintsFromElement(const QString& name) {
    if (name.isEmpty() || !m_workspace.contains(name.toStdString())) {
        return;
    }

    // Retrieve the metadata for the selected element.
    // For batches, use the first frame's baseMetadata.
    const ImageMetadata* meta = nullptr;
    WorkspaceElement elem = m_workspace.getElement(name.toStdString());
    ImageMetadata batchMeta;
    if (std::holds_alternative<GrayscaleImagePtr>(elem)) {
        meta = &std::get<GrayscaleImagePtr>(elem)->metadata();
    } else if (std::holds_alternative<RGBImagePtr>(elem)) {
        meta = &std::get<RGBImagePtr>(elem)->metadata();
    } else if (std::holds_alternative<ImageBatchPtr>(elem)) {
        auto batch = std::get<ImageBatchPtr>(elem);
        if (batch->count() > 0) {
            batchMeta = batch->frameMetadata(0).baseMetadata;
            meta = &batchMeta;
        }
    }

    if (!meta) return;

    // Helper: look up a numeric FITS keyword, return true if found and parseable
    auto getDouble = [&](const std::string& key, double& out) -> bool {
        auto it = meta->fitsKeywords.find(key);
        if (it != meta->fitsKeywords.end()) {
            try {
                out = std::stod(it->second);
                return true;
            } catch (...) {}
        }
        return false;
    };

    // Focal length: standard FITS keyword FOCALLEN (mm)
    double focalLen = 0.0;
    if (getDouble("FOCALLEN", focalLen) && focalLen > 0.0) {
        m_focalLengthSpin->setValue(focalLen);
    }

    // Pixel size: XPIXSZ preferred, fallback to YPIXSZ (micrometers)
    double pixSz = 0.0;
    if ((getDouble("XPIXSZ", pixSz) || getDouble("YPIXSZ", pixSz)) && pixSz > 0.0) {
        m_pixelSizeSpin->setValue(pixSz);
    }

    // RA/Dec: prefer already-solved WCS center; fallback to RA/DEC FITS keywords
    double ra = -1.0, dec = -99.0;
    if (meta->wcsSolved) {
        ra = meta->wcsRaCenter;
        dec = meta->wcsDecCenter;
    } else {
        getDouble("RA", ra);
        getDouble("DEC", dec);
    }
    if (ra >= 0.0 && dec >= -90.0) {
        m_raHintSpin->setValue(ra);
        m_decHintSpin->setValue(dec);
        m_blindSolveChk->setChecked(false);
    } else {
        m_raHintSpin->setValue(0.0);
        m_decHintSpin->setValue(0.0);
        m_blindSolveChk->setChecked(true);
    }
}

QJsonObject PlatesolveDialog::serializeState() const {
    QJsonObject obj;
    obj["solver"] = m_solverCombo->currentData().toString();
    obj["blind_solve"] = m_blindSolveChk->isChecked();
    obj["ra_hint"] = m_raHintSpin->value();
    obj["dec_hint"] = m_decHintSpin->value();
    obj["focal_length"] = m_focalLengthSpin->value();
    obj["pixel_size"] = m_pixelSizeSpin->value();
    return obj;
}

void PlatesolveDialog::restoreState(const QJsonObject& obj) {
    if (obj.contains("solver")) {
        int idx = m_solverCombo->findData(obj["solver"].toString());
        if (idx >= 0) m_solverCombo->setCurrentIndex(idx);
    }
    if (obj.contains("blind_solve")) {
        m_blindSolveChk->setChecked(obj["blind_solve"].toBool());
    }
    if (obj.contains("ra_hint"))
        m_raHintSpin->setValue(obj["ra_hint"].toDouble());
    if (obj.contains("dec_hint"))
        m_decHintSpin->setValue(obj["dec_hint"].toDouble());
    if (obj.contains("focal_length"))
        m_focalLengthSpin->setValue(obj["focal_length"].toDouble());
    if (obj.contains("pixel_size"))
        m_pixelSizeSpin->setValue(obj["pixel_size"].toDouble());
}

} // namespace blastro
