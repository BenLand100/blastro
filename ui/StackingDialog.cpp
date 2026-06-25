#include "StackingDialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDoubleValidator>

namespace blastro {

StackingDialog::StackingDialog(WorkspaceRegistry& workspace, QWidget* parent)
    : AlgorithmDialog(workspace, parent) {
    
    setWindowTitle("Stacking Configuration");
    resize(360, 320);

    // Apply high-quality dark theme styling
    setStyleSheet(
        "QDialog { background-color: #202020; color: #ffffff; }"
        "QLabel { color: #aaaaaa; font-size: 11px; }"
        "QLineEdit { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; padding: 3px 6px; border-radius: 3px; font-size: 11px; }"
        "QLineEdit:disabled { background-color: #282828; color: #666666; border-color: #444444; }"
        "QComboBox { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; padding: 3px 6px; border-radius: 3px; font-size: 11px; }"
        "QComboBox QAbstractItemView { background-color: #2a2a2a; color: #ffffff; selection-background-color: #007acc; }"
        "QGroupBox { border: 1px solid #444444; border-radius: 4px; margin-top: 12px; padding-top: 12px; color: #ffffff; font-weight: bold; font-size: 11px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; }"
        "QPushButton { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; padding: 5px 14px; border-radius: 3px; font-size: 11px; font-weight: bold; }"
        "QPushButton:hover { background-color: #4a4a4a; }"
        "QPushButton:pressed { background-color: #007acc; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    formLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    formLayout->setSpacing(8);

    // 1. Target Batch ComboBox
    m_targetBatchCombo = new QComboBox(this);
    auto keys = workspace.elementNames();
    for (const auto& name : keys) {
        WorkspaceElement elem = workspace.getElement(name);
        if (std::holds_alternative<ImageBatchPtr>(elem)) {
            m_targetBatchCombo->addItem(QString::fromStdString(name));
        }
    }
    formLayout->addRow("Target Batch:", m_targetBatchCombo);

    // 2. Output Name LineEdit
    m_outputName = new QLineEdit(this);
    m_outputName->setText("stacked_image");
    formLayout->addRow("Output Name:", m_outputName);

    // 3. Stacking Method ComboBox
    m_methodCombo = new QComboBox(this);
    m_methodCombo->addItem("Average", "average");
    m_methodCombo->addItem("Median", "median");
    m_methodCombo->addItem("Maximum", "max");
    m_methodCombo->addItem("Minimum", "min");
    formLayout->addRow("Stacking Method:", m_methodCombo);

    // 4. Rejection Method ComboBox
    m_rejectionCombo = new QComboBox(this);
    m_rejectionCombo->addItem("None", "none");
    m_rejectionCombo->addItem("Quantile", "quantile");
    m_rejectionCombo->addItem("Sigma Clipping", "sigma");
    m_rejectionCombo->addItem("Winsorized Sigma Clipping", "winsorized_sigma");
    formLayout->addRow("Rejection Method:", m_rejectionCombo);

    mainLayout->addLayout(formLayout);

    // 5. Parameters GroupBox (disabled/enabled dynamically)
    m_paramGroup = new QGroupBox("Rejection Parameters", this);
    QFormLayout* paramLayout = new QFormLayout(m_paramGroup);
    paramLayout->setSpacing(6);

    QDoubleValidator* doubleValidator = new QDoubleValidator(this);
    doubleValidator->setBottom(0.0);

    QDoubleValidator* fractionValidator = new QDoubleValidator(0.0, 0.5, 4, this);

    m_sigmaLowLabel = new QLabel("Sigma Low:", this);
    m_sigmaLowEdit = new QLineEdit(this);
    m_sigmaLowEdit->setText("3.0");
    m_sigmaLowEdit->setValidator(doubleValidator);
    paramLayout->addRow(m_sigmaLowLabel, m_sigmaLowEdit);

    m_sigmaHighLabel = new QLabel("Sigma High:", this);
    m_sigmaHighEdit = new QLineEdit(this);
    m_sigmaHighEdit->setText("3.0");
    m_sigmaHighEdit->setValidator(doubleValidator);
    paramLayout->addRow(m_sigmaHighLabel, m_sigmaHighEdit);

    m_quantileLowLabel = new QLabel("Quantile Low:", this);
    m_quantileLowEdit = new QLineEdit(this);
    m_quantileLowEdit->setText("0.0");
    m_quantileLowEdit->setValidator(fractionValidator);
    paramLayout->addRow(m_quantileLowLabel, m_quantileLowEdit);

    m_quantileHighLabel = new QLabel("Quantile High:", this);
    m_quantileHighEdit = new QLineEdit(this);
    m_quantileHighEdit->setText("0.0");
    m_quantileHighEdit->setValidator(fractionValidator);
    paramLayout->addRow(m_quantileHighLabel, m_quantileHighEdit);

    mainLayout->addWidget(m_paramGroup);

    // Connect rejection combo box change signal to dynamic layout slots
    connect(m_rejectionCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, &StackingDialog::onRejectionChanged);
    onRejectionChanged(0); // Trigger initial state

    // 6. Buttons Box
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch(1);
    
    QPushButton* cancelBtn = new QPushButton("Cancel", this);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(cancelBtn);

    QPushButton* runBtn = new QPushButton("Run", this);
    runBtn->setStyleSheet("QPushButton { background-color: #007acc; border-color: #007acc; } QPushButton:hover { background-color: #008be5; }");
    connect(runBtn, &QPushButton::clicked, this, &StackingDialog::onRunClicked);
    btnLayout->addWidget(runBtn);

    mainLayout->addLayout(btnLayout);
}

void StackingDialog::onRejectionChanged(int index) {
    Q_UNUSED(index);
    QString rej = m_rejectionCombo->currentData().toString();
    
    if (rej == "none") {
        m_paramGroup->setEnabled(false);
    } else if (rej == "quantile") {
        m_paramGroup->setEnabled(true);
        
        m_sigmaLowLabel->setEnabled(false);
        m_sigmaLowEdit->setEnabled(false);
        m_sigmaHighLabel->setEnabled(false);
        m_sigmaHighEdit->setEnabled(false);
        
        m_quantileLowLabel->setEnabled(true);
        m_quantileLowEdit->setEnabled(true);
        m_quantileHighLabel->setEnabled(true);
        m_quantileHighEdit->setEnabled(true);
    } else {
        // sigma or winsorized_sigma
        m_paramGroup->setEnabled(true);
        
        m_sigmaLowLabel->setEnabled(true);
        m_sigmaLowEdit->setEnabled(true);
        m_sigmaHighLabel->setEnabled(true);
        m_sigmaHighEdit->setEnabled(true);
        
        m_quantileLowLabel->setEnabled(false);
        m_quantileLowEdit->setEnabled(false);
        m_quantileHighLabel->setEnabled(false);
        m_quantileHighEdit->setEnabled(false);
    }
}

void StackingDialog::onRunClicked() {
    if (m_targetBatchCombo->currentText().isEmpty()) {
        QMessageBox::warning(this, "Configuration Error", "Please select a target batch to stack.");
        return;
    }

    if (m_outputName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Configuration Error", "Please enter a valid output name.");
        return;
    }

    emit algorithmExecuted(algorithmName(), getConfig());
    accept();
}

std::map<std::string, std::string> StackingDialog::getConfig() const {
    std::map<std::string, std::string> config;
    config["input_name"] = m_targetBatchCombo->currentText().toStdString();
    config["output_name"] = m_outputName->text().trimmed().toStdString();
    config["method"] = m_methodCombo->currentData().toString().toStdString();
    config["rejection"] = m_rejectionCombo->currentData().toString().toStdString();
    
    config["sigma_low"] = m_sigmaLowEdit->text().toStdString();
    config["sigma_high"] = m_sigmaHighEdit->text().toStdString();
    config["quantile_low"] = m_quantileLowEdit->text().toStdString();
    config["quantile_high"] = m_quantileHighEdit->text().toStdString();
    
    return config;
}

} // namespace blastro
