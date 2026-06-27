#include "PreferencesWindow.h"
#include "core/Preferences.h"
#include <QVBoxLayout>
#include <QMdiSubWindow>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QMainWindow>
#include <QStatusBar>

namespace blastro {

PreferencesWindow::PreferencesWindow(QWidget* parent)
    : QWidget(parent) {
    
    setWindowTitle("BLastro Preferences");
    resize(500, 480);

    setStyleSheet(
        "QWidget { background-color: #202020; color: #ffffff; }"
        "QLabel { background-color: transparent; color: #aaaaaa; font-size: 11px; }"
        "QLineEdit { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; padding: 3px 6px; border-radius: 3px; font-size: 11px; }"
        "QSpinBox { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; padding: 3px 6px; border-radius: 3px; font-size: 11px; }"
        "QCheckBox { background-color: transparent; color: #ffffff; font-size: 11px; }"
        "QPushButton { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; padding: 4px 10px; border-radius: 3px; font-size: 11px; font-weight: bold; }"
        "QPushButton:hover { background-color: #4a4a4a; }"
        "QPushButton:pressed { background-color: #007acc; }"
        "QPushButton#primaryButton { background-color: #007acc; border-color: #007acc; color: #ffffff; }"
        "QPushButton#primaryButton:hover { background-color: #008be5; }"
        "QPushButton#primaryButton:pressed { background-color: #0060a0; }"
        "QGroupBox { font-weight: bold; border: 1px solid #555555; margin-top: 10px; padding: 12px; color: #ffffff; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);

    // Group 1: PCL Settings
    QGroupBox* pclGroup = new QGroupBox("PixInsight PCL Settings", this);
    QFormLayout* pclForm = new QFormLayout(pclGroup);
    pclForm->setSpacing(8);

    m_moduleEdit = new QLineEdit(this);
    QPushButton* btnBrowseModule = new QPushButton("Browse...", this);
    connect(btnBrowseModule, &QPushButton::clicked, this, &PreferencesWindow::onBrowseModule);
    QHBoxLayout* layoutModule = new QHBoxLayout();
    layoutModule->addWidget(m_moduleEdit, 1);
    layoutModule->addWidget(btnBrowseModule);
    pclForm->addRow("PCL Module Folder (bin):", layoutModule);

    m_libEdit = new QLineEdit(this);
    QPushButton* btnBrowseLib = new QPushButton("Browse...", this);
    connect(btnBrowseLib, &QPushButton::clicked, this, &PreferencesWindow::onBrowseLib);
    QHBoxLayout* layoutLib = new QHBoxLayout();
    layoutLib->addWidget(m_libEdit, 1);
    layoutLib->addWidget(btnBrowseLib);
    pclForm->addRow("PCL Lib Folder (shared libs):", layoutLib);

    m_libraryEdit = new QLineEdit(this);
    QPushButton* btnBrowseLibrary = new QPushButton("Browse...", this);
    connect(btnBrowseLibrary, &QPushButton::clicked, this, &PreferencesWindow::onBrowseLibrary);
    QHBoxLayout* layoutLibrary = new QHBoxLayout();
    layoutLibrary->addWidget(m_libraryEdit, 1);
    layoutLibrary->addWidget(btnBrowseLibrary);
    pclForm->addRow("PCL Library Folder (AI models):", layoutLibrary);

    m_tensorflowChk = new QCheckBox("Load TensorFlow dynamically on startup", this);
    pclForm->addRow("", m_tensorflowChk);

    mainLayout->addWidget(pclGroup);

    // Group 2: Path Configurations
    QGroupBox* pathsGroup = new QGroupBox("Path Configurations", this);
    QFormLayout* pathsForm = new QFormLayout(pathsGroup);
    pathsForm->setSpacing(8);

    m_tempEdit = new QLineEdit(this);
    QPushButton* btnBrowseTemp = new QPushButton("Browse...", this);
    connect(btnBrowseTemp, &QPushButton::clicked, this, &PreferencesWindow::onBrowseTemp);
    QHBoxLayout* layoutTemp = new QHBoxLayout();
    layoutTemp->addWidget(m_tempEdit, 1);
    layoutTemp->addWidget(btnBrowseTemp);
    pathsForm->addRow("Temporary Folder:", layoutTemp);

    m_intermediateEdit = new QLineEdit(this);
    QPushButton* btnBrowseIntermediate = new QPushButton("Browse...", this);
    connect(btnBrowseIntermediate, &QPushButton::clicked, this, &PreferencesWindow::onBrowseIntermediate);
    QHBoxLayout* layoutIntermediate = new QHBoxLayout();
    layoutIntermediate->addWidget(m_intermediateEdit, 1);
    layoutIntermediate->addWidget(btnBrowseIntermediate);
    pathsForm->addRow("Intermediate Folder (Batches):", layoutIntermediate);

    mainLayout->addWidget(pathsGroup);

    // Group 3: System Resource Settings
    QGroupBox* sysGroup = new QGroupBox("System Resource Settings", this);
    QFormLayout* sysForm = new QFormLayout(sysGroup);
    sysForm->setSpacing(8);

    m_threadSpin = new QSpinBox(this);
    m_threadSpin->setRange(1, 64);
    sysForm->addRow("Maximum CPU Cores / Threads:", m_threadSpin);

    m_ramSpin = new QSpinBox(this);
    m_ramSpin->setRange(1, 512);
    m_ramSpin->setSuffix(" GB");
    sysForm->addRow("Maximum RAM Limit:", m_ramSpin);

    mainLayout->addWidget(sysGroup);

    // Bottom buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch(1);

    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, [this]() {
        QObject* current = this;
        while (current) {
            if (auto sub = qobject_cast<QMdiSubWindow*>(current)) {
                sub->close();
                return;
            }
            current = current->parent();
        }
        close();
    });
    btnLayout->addWidget(closeBtn);

    QPushButton* saveBtn = new QPushButton("Save", this);
    saveBtn->setObjectName("primaryButton");
    connect(saveBtn, &QPushButton::clicked, this, &PreferencesWindow::onSaveClicked);
    btnLayout->addWidget(saveBtn);

    mainLayout->addLayout(btnLayout);

    // Load current settings into fields
    Preferences& prefs = Preferences::instance();
    m_moduleEdit->setText(QString::fromStdString(prefs.getPclModuleFolder()));
    m_libEdit->setText(QString::fromStdString(prefs.getPclLibFolder()));
    m_libraryEdit->setText(QString::fromStdString(prefs.getPclLibraryFolder()));
    m_tensorflowChk->setChecked(prefs.getPclLoadTensorflow());
    m_tempEdit->setText(QString::fromStdString(prefs.getTemporaryFolder()));
    m_intermediateEdit->setText(QString::fromStdString(prefs.getIntermediateFolder()));
    m_threadSpin->setValue(prefs.getThreadCount());
    m_ramSpin->setValue(prefs.getMaxRamUsage());
}

void PreferencesWindow::onBrowseModule() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select PCL Module Folder (bin)", m_moduleEdit->text());
    if (!dir.isEmpty()) {
        m_moduleEdit->setText(dir);
    }
}

void PreferencesWindow::onBrowseLib() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select PCL Lib Folder (lib)", m_libEdit->text());
    if (!dir.isEmpty()) {
        m_libEdit->setText(dir);
    }
}

void PreferencesWindow::onBrowseLibrary() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select PCL Library Folder (library)", m_libraryEdit->text());
    if (!dir.isEmpty()) {
        m_libraryEdit->setText(dir);
    }
}

void PreferencesWindow::onBrowseTemp() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Temporary Folder", m_tempEdit->text());
    if (!dir.isEmpty()) {
        m_tempEdit->setText(dir);
    }
}

void PreferencesWindow::onBrowseIntermediate() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Intermediate Folder", m_intermediateEdit->text());
    if (!dir.isEmpty()) {
        m_intermediateEdit->setText(dir);
    }
}

void PreferencesWindow::onSaveClicked() {
    Preferences& prefs = Preferences::instance();
    prefs.setPclModuleFolder(m_moduleEdit->text().trimmed().toStdString());
    prefs.setPclLibFolder(m_libEdit->text().trimmed().toStdString());
    prefs.setPclLibraryFolder(m_libraryEdit->text().trimmed().toStdString());
    prefs.setPclLoadTensorflow(m_tensorflowChk->isChecked());
    prefs.setTemporaryFolder(m_tempEdit->text().trimmed().toStdString());
    prefs.setIntermediateFolder(m_intermediateEdit->text().trimmed().toStdString());
    prefs.setThreadCount(m_threadSpin->value());
    prefs.setMaxRamUsage(m_ramSpin->value());
    prefs.save();

    // Find main window status bar to show a message
    QWidget* p = parentWidget();
    while (p) {
        if (auto mw = qobject_cast<QMainWindow*>(p)) {
            if (auto sb = mw->statusBar()) {
                sb->showMessage("Preferences saved successfully", 3000);
            }
            break;
        }
        p = p->parentWidget();
    }

    QMessageBox::information(this, "Preferences Saved", "BLastro preferences have been saved successfully.");
    
    QObject* current = this;
    while (current) {
        if (auto sub = qobject_cast<QMdiSubWindow*>(current)) {
            sub->close();
            return;
        }
        current = current->parent();
    }
    close();
}

} // namespace blastro
