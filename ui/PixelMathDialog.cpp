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

#include "PixelMathDialog.h"
#include <QJsonObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

namespace blastro {

PixelMathDialog::PixelMathDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : AlgorithmDialog(workspace, parent),
      m_exprR(new QLineEdit(this)),
      m_exprG(new QLineEdit(this)),
      m_exprB(new QLineEdit(this)),
      m_exprK(new QLineEdit(this)),
      m_useSingleExpr(new QCheckBox("Use a single expression for RGB channels", this)),
      m_rgbMode(new QRadioButton("RGB Color Space", this)),
      m_grayMode(new QRadioButton("Grayscale Space", this)),
      m_outputName(new QLineEdit("PixelMath_Output", this)),
      m_targetImageCombo(new QComboBox(this)),
      m_createNewImage(new QRadioButton("Create new image:", this)),
      m_replaceTargetImage(new QRadioButton("Replace target image:", this)) {

    setWindowTitle("Pixel Math");
    resize(500, 400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    // 1. Image References Info Box
    QGroupBox* infoBox = new QGroupBox("Available Images (use as variables)", this);
    QVBoxLayout* infoLayout = new QVBoxLayout(infoBox);
    
    m_infoLabel = new QLabel(this);
    m_infoLabel->setStyleSheet("font-family: monospace; color: #88ff88;");
    infoLayout->addWidget(m_infoLabel);
    mainLayout->addWidget(infoBox);

    // 2. Color Space / Expression type
    QGroupBox* modeBox = new QGroupBox("Output Color Space", this);
    QHBoxLayout* modeLayout = new QHBoxLayout(modeBox);
    m_rgbMode->setChecked(true);
    m_grayMode->setChecked(false);
    modeLayout->addWidget(m_rgbMode);
    modeLayout->addWidget(m_grayMode);
    mainLayout->addWidget(modeBox);

    // 3. Expressions input
    QGroupBox* exprBox = new QGroupBox("RGB/K Expressions", this);
    QFormLayout* formLayout = new QFormLayout(exprBox);
    
    m_exprR->setPlaceholderText("Expression for Red channel");
    m_exprG->setPlaceholderText("Expression for Green channel");
    m_exprB->setPlaceholderText("Expression for Blue channel");
    m_exprK->setPlaceholderText("Expression for Grayscale (K) channel");
    m_exprK->setEnabled(false); // Default is RGB mode
    
    formLayout->addRow("R / Gray:", m_exprR);
    formLayout->addRow("G:", m_exprG);
    formLayout->addRow("B:", m_exprB);
    formLayout->addRow("K (Gray):", m_exprK);
    formLayout->addRow("", m_useSingleExpr);
    
    mainLayout->addWidget(exprBox);

    // 4. Destination
    QGroupBox* destBox = new QGroupBox("Destination", this);
    QVBoxLayout* destLayout = new QVBoxLayout(destBox);
    
    QHBoxLayout* createLayout = new QHBoxLayout();
    m_createNewImage->setChecked(true);
    createLayout->addWidget(m_createNewImage);
    createLayout->addWidget(m_outputName, 1);
    destLayout->addLayout(createLayout);

    QHBoxLayout* replaceLayout = new QHBoxLayout();
    m_replaceTargetImage->setChecked(false);
    replaceLayout->addWidget(m_replaceTargetImage);
    
    refreshWorkspaceElements();
    m_targetImageCombo->setEnabled(false);
    replaceLayout->addWidget(m_targetImageCombo, 1);
    destLayout->addLayout(replaceLayout);
    
    mainLayout->addWidget(destBox);

    // 5. Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* runBtn = new QPushButton("Run", this);
    runBtn->setObjectName("primaryButton");
    QPushButton* closeBtn = new QPushButton("Close", this);
    
    btnLayout->addStretch();
    btnLayout->addWidget(runBtn);
    btnLayout->addWidget(closeBtn);
    mainLayout->addLayout(btnLayout);

    // Connections
    connect(m_useSingleExpr, &QCheckBox::toggled, this, &PixelMathDialog::onUseSingleExpressionChanged);
    connect(m_rgbMode, &QRadioButton::toggled, this, [this](bool checked) {
        m_exprR->setEnabled(checked);
        m_exprG->setEnabled(checked && !m_useSingleExpr->isChecked());
        m_exprB->setEnabled(checked && !m_useSingleExpr->isChecked());
        m_useSingleExpr->setEnabled(checked);
        m_exprK->setEnabled(!checked);
    });
    connect(m_grayMode, &QRadioButton::toggled, this, [this](bool checked) {
        m_exprK->setEnabled(checked);
        m_exprR->setEnabled(!checked);
        m_exprG->setEnabled(!checked);
        m_exprB->setEnabled(!checked);
        m_useSingleExpr->setEnabled(!checked);
    });
    connect(m_createNewImage, &QRadioButton::toggled, this, [this](bool checked) {
        m_outputName->setEnabled(checked);
        m_targetImageCombo->setEnabled(!checked);
    });
    connect(m_replaceTargetImage, &QRadioButton::toggled, this, [this](bool checked) {
        m_outputName->setEnabled(!checked);
        m_targetImageCombo->setEnabled(checked);
    });

    connect(runBtn, &QPushButton::clicked, this, &PixelMathDialog::onRunClicked);
    connect(closeBtn, &QPushButton::clicked, this, &AlgorithmDialog::onClose);
}

void PixelMathDialog::onUseSingleExpressionChanged(bool checked) {
    if (m_rgbMode->isChecked()) {
        m_exprG->setEnabled(!checked);
        m_exprB->setEnabled(!checked);
    }
}

std::map<std::string, std::string> PixelMathDialog::getConfig() const {
    std::map<std::string, std::string> config;

    bool isRGB = m_rgbMode->isChecked();
    config["color_space"] = isRGB ? "RGB" : "Grayscale";

    if (isRGB) {
        if (m_useSingleExpr->isChecked()) {
            std::string expr = m_exprR->text().toStdString();
            config["expr_r"] = expr;
            config["expr_g"] = expr;
            config["expr_b"] = expr;
        } else {
            config["expr_r"] = m_exprR->text().toStdString();
            config["expr_g"] = m_exprG->text().toStdString();
            config["expr_b"] = m_exprB->text().toStdString();
        }
    } else {
        config["expr_k"] = m_exprK->text().toStdString();
    }

    if (m_createNewImage->isChecked()) {
        config["output_name"] = m_outputName->text().toStdString();
    } else {
        config["output_name"] = m_targetImageCombo->currentText().toStdString();
    }

    return config;
}

void PixelMathDialog::onRunClicked() {
    // Validate inputs
    if (m_rgbMode->isChecked()) {
        if (m_exprR->text().isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "R / Gray expression cannot be empty in RGB mode");
            return;
        }
        if (!m_useSingleExpr->isChecked() && (m_exprG->text().isEmpty() || m_exprB->text().isEmpty())) {
            QMessageBox::warning(this, "Validation Error", "All channel expressions must be filled, or check 'Use single expression'");
            return;
        }
    } else {
        if (m_exprK->text().isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Grayscale expression (K) cannot be empty");
            return;
        }
    }

    if (m_createNewImage->isChecked() && m_outputName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Output image name cannot be empty");
        return;
    }

    if (m_replaceTargetImage->isChecked() && m_targetImageCombo->currentText().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "No target image selected for replacement");
        return;
    }

    emit algorithmExecuted("PixelMath", getConfig());
}

void PixelMathDialog::refreshWorkspaceElements() {
    QString currentText = m_targetImageCombo->currentText();
    m_targetImageCombo->clear();
    auto keys = m_workspace.elementNames();
    for (const auto& name : keys) {
        m_targetImageCombo->addItem(QString::fromStdString(name));
    }
    int idx = m_targetImageCombo->findText(currentText);
    if (idx >= 0) {
        m_targetImageCombo->setCurrentIndex(idx);
    }

    if (m_infoLabel) {
        QString infoText = "Active Workspace Variables:\n";
        if (keys.empty()) {
            infoText += "  (No images open. Math will output using default 800x600 dimensions)";
        } else {
            for (const auto& name : keys) {
                infoText += QString("  - %1\n").arg(QString::fromStdString(name));
            }
            infoText += "\nNote: For RGB images, you can also use suffix _r, _g, _b (e.g. Image1_r)";
        }
        m_infoLabel->setText(infoText);
    }
}

QJsonObject PixelMathDialog::serializeState() const {
    QJsonObject obj;
    obj["expr_r"] = m_exprR->text();
    obj["expr_g"] = m_exprG->text();
    obj["expr_b"] = m_exprB->text();
    obj["expr_k"] = m_exprK->text();
    obj["use_single_expr"] = m_useSingleExpr->isChecked();
    obj["output_mode"] = m_rgbMode->isChecked() ? QString("rgb") : QString("grayscale");
    obj["create_new"] = m_createNewImage->isChecked();
    obj["output_name"] = m_outputName->text();
    return obj;
}

void PixelMathDialog::restoreState(const QJsonObject& obj) {
    if (obj.contains("expr_r")) m_exprR->setText(obj["expr_r"].toString());
    if (obj.contains("expr_g")) m_exprG->setText(obj["expr_g"].toString());
    if (obj.contains("expr_b")) m_exprB->setText(obj["expr_b"].toString());
    if (obj.contains("expr_k")) m_exprK->setText(obj["expr_k"].toString());
    if (obj.contains("use_single_expr"))
        m_useSingleExpr->setChecked(obj["use_single_expr"].toBool());
    if (obj.contains("output_mode")) {
        bool isRgb = obj["output_mode"].toString() == "rgb";
        m_rgbMode->setChecked(isRgb);
        m_grayMode->setChecked(!isRgb);
    }
    if (obj.contains("create_new")) {
        bool createNew = obj["create_new"].toBool();
        m_createNewImage->setChecked(createNew);
        m_replaceTargetImage->setChecked(!createNew);
    }
    if (obj.contains("output_name"))
        m_outputName->setText(obj["output_name"].toString());
}

} // namespace blastro
