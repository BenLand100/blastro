#include "AlignDialog.h"
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
    
    setWindowTitle("Image Alignment & Drizzle Configuration");
    resize(360, 200);



    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    formLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    formLayout->setSpacing(8);

    // 1. Target Input ComboBox (Must be a batch)
    m_targetInputCombo = new QComboBox(this);
    refreshWorkspaceElements();
    formLayout->addRow("Registered Batch:", m_targetInputCombo);

    // 2. Output Name LineEdit
    m_outputName = new QLineEdit(this);
    m_outputName->setText("aligned_batch");
    formLayout->addRow("Output Name:", m_outputName);

    // 3. Drizzle Scale ComboBox
    m_drizzleCombo = new QComboBox(this);
    m_drizzleCombo->addItem("1.0x (No Drizzle)", 1.0);
    m_drizzleCombo->addItem("1.5x Drizzle", 1.5);
    m_drizzleCombo->addItem("2.0x Drizzle", 2.0);
    m_drizzleCombo->addItem("3.0x Drizzle", 3.0);
    formLayout->addRow("Drizzle Scale:", m_drizzleCombo);

    mainLayout->addLayout(formLayout);

    // Buttons Box
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    QPushButton* prefsBtn = new QPushButton("Preferences...", this);
    connect(prefsBtn, &QPushButton::clicked, this, &AlignDialog::onPrefsClicked);
    btnLayout->addWidget(prefsBtn);

    btnLayout->addStretch(1);
    
    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);
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

    QSpinBox* chunkSpin = new QSpinBox(&dlg);
    chunkSpin->setRange(16, 4096);
    chunkSpin->setSingleStep(16);
    chunkSpin->setValue(m_stripHeight);
    form->addRow("Warp Chunk Height (Rows):", chunkSpin);

    QSpinBox* threadSpin = new QSpinBox(&dlg);
    threadSpin->setRange(1, 64);
    threadSpin->setValue(m_threads > 0 ? m_threads : QThread::idealThreadCount());
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
        m_stripHeight = chunkSpin->value();
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
    config["drizzle_scale"] = std::to_string(m_drizzleCombo->currentData().toDouble());
    config["strip_height"] = std::to_string(m_stripHeight);
    config["threads"] = std::to_string(m_threads);
    config["evict_cache"] = m_evictCache ? "true" : "false";
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

} // namespace blastro
