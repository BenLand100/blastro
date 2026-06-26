#include "CalibrationDialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>

namespace blastro {

CalibrationDialog::CalibrationDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : AlgorithmDialog(workspace, parent) {
    
    setWindowTitle("Calibration Configuration");
    resize(360, 240);



    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    formLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    formLayout->setSpacing(8);

    // 1. Target Input ComboBox (Images or Batches)
    m_targetInputCombo = new QComboBox(this);
    
    // 2. Output Name LineEdit
    m_outputName = new QLineEdit(this);
    m_outputName->setText("calibrated_result");
    
    // Calibration comboboxes
    m_biasCombo = new QComboBox(this);
    m_darkCombo = new QComboBox(this);
    m_flatCombo = new QComboBox(this);
    
    refreshWorkspaceElements();
    
    formLayout->addRow("Target Input:", m_targetInputCombo);
    formLayout->addRow("Output Name:", m_outputName);
    formLayout->addRow("Bias Image (Optional):", m_biasCombo);
    formLayout->addRow("Dark Image (Optional):", m_darkCombo);
    formLayout->addRow("Flat Image (Optional):", m_flatCombo);

    mainLayout->addLayout(formLayout);

    // 6. Buttons Box
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch(1);
    
    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);
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

    int idx = m_targetInputCombo->findText(curTarget);
    if (idx >= 0) {
        m_targetInputCombo->setCurrentIndex(idx);
    }
}

} // namespace blastro
