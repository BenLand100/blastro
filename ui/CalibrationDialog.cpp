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

    // Apply high-quality dark theme styling matching StackingDialog
    setStyleSheet(
        "QDialog { background-color: #202020; color: #ffffff; }"
        "QLabel { color: #aaaaaa; font-size: 11px; }"
        "QLineEdit { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; padding: 3px 6px; border-radius: 3px; font-size: 11px; }"
        "QComboBox { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; padding: 3px 6px; border-radius: 3px; font-size: 11px; }"
        "QComboBox QAbstractItemView { background-color: #2a2a2a; color: #ffffff; selection-background-color: #007acc; }"
        "QPushButton { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; padding: 5px 14px; border-radius: 3px; font-size: 11px; font-weight: bold; }"
        "QPushButton:hover { background-color: #4a4a4a; }"
        "QPushButton:pressed { background-color: #007acc; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    formLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    formLayout->setSpacing(8);

    // 1. Target Input ComboBox (Images or Batches)
    m_targetInputCombo = new QComboBox(this);
    auto keys = workspace.elementNames();
    for (const auto& name : keys) {
        m_targetInputCombo->addItem(QString::fromStdString(name));
    }
    formLayout->addRow("Target Input:", m_targetInputCombo);

    // 2. Output Name LineEdit
    m_outputName = new QLineEdit(this);
    m_outputName->setText("calibrated_result");
    formLayout->addRow("Output Name:", m_outputName);

    // Helper lambda to populate calibration frame combos (only images, with <None> option)
    auto populateCalibrationCombo = [&](QComboBox* combo) {
        combo->addItem("<None>", "");
        for (const auto& name : keys) {
            WorkspaceElement elem = workspace.getElement(name);
            if (std::holds_alternative<GrayscaleImagePtr>(elem) || std::holds_alternative<RGBImagePtr>(elem)) {
                combo->addItem(QString::fromStdString(name), QString::fromStdString(name));
            }
        }
    };

    // 3. Bias Image ComboBox
    m_biasCombo = new QComboBox(this);
    populateCalibrationCombo(m_biasCombo);
    formLayout->addRow("Bias Image (Optional):", m_biasCombo);

    // 4. Dark Image ComboBox
    m_darkCombo = new QComboBox(this);
    populateCalibrationCombo(m_darkCombo);
    formLayout->addRow("Dark Image (Optional):", m_darkCombo);

    // 5. Flat Image ComboBox
    m_flatCombo = new QComboBox(this);
    populateCalibrationCombo(m_flatCombo);
    formLayout->addRow("Flat Image (Optional):", m_flatCombo);

    mainLayout->addLayout(formLayout);

    // 6. Buttons Box
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch(1);
    
    QPushButton* cancelBtn = new QPushButton("Cancel", this);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(cancelBtn);

    QPushButton* runBtn = new QPushButton("Run", this);
    runBtn->setStyleSheet("QPushButton { background-color: #007acc; border-color: #007acc; } QPushButton:hover { background-color: #008be5; }");
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
    accept();
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

} // namespace blastro
