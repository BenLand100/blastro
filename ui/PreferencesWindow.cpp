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
#include <QTabWidget>

namespace blastro {

PreferencesWindow::PreferencesWindow(QWidget* parent)
    : QWidget(parent) {
    
    setWindowTitle("BLastro Preferences");
    resize(520, 520);

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
        "QTabWidget::pane { border: 1px solid #555555; background-color: #202020; top: -1px; }"
        "QTabBar::tab { background-color: #333333; color: #aaaaaa; padding: 6px 12px; border: 1px solid #555555; border-bottom: none; border-top-left-radius: 4px; border-top-right-radius: 4px; font-size: 11px; font-weight: bold; margin-right: 2px; }"
        "QTabBar::tab:selected { background-color: #202020; color: #ffffff; border-bottom: 1px solid #202020; }"
        "QTabBar::tab:hover { background-color: #444444; color: #ffffff; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);

    QTabWidget* tabWidget = new QTabWidget(this);

    // Tab 1: General Settings
    QWidget* generalTab = new QWidget(this);
    QVBoxLayout* generalLayout = new QVBoxLayout(generalTab);
    generalLayout->setContentsMargins(10, 10, 10, 10);
    generalLayout->setSpacing(10);

    // Group 1: PCL Settings
    QGroupBox* pclGroup = new QGroupBox("PCL Settings", this);
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

    m_tensorflowUrlEdit = new QLineEdit(this);
    pclForm->addRow("TensorFlow Download URL:", m_tensorflowUrlEdit);
    generalLayout->addWidget(pclGroup);

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
    generalLayout->addWidget(pathsGroup);

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
    generalLayout->addWidget(sysGroup);

    tabWidget->addTab(generalTab, "General Settings");

    // Tab 2: PCL Repositories
    QWidget* reposTab = new QWidget(this);
    QVBoxLayout* reposLayout = new QVBoxLayout(reposTab);
    reposLayout->setContentsMargins(10, 10, 10, 10);
    reposLayout->setSpacing(10);

    QGroupBox* reposGroup = new QGroupBox("PCL Repositories List", this);
    QVBoxLayout* reposGroupLayout = new QVBoxLayout(reposGroup);
    reposGroupLayout->setSpacing(8);

    m_reposListWidget = new QListWidget(this);
    m_reposListWidget->setStyleSheet("background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; border-radius: 3px; font-size: 11px;");
    reposGroupLayout->addWidget(m_reposListWidget, 1);

    QHBoxLayout* addLayout = new QHBoxLayout();
    m_newRepoEdit = new QLineEdit(this);
    m_newRepoEdit->setPlaceholderText("Enter repository URL (e.g., https://example.com/)...");
    QPushButton* btnAddRepo = new QPushButton("Add", this);
    connect(btnAddRepo, &QPushButton::clicked, this, &PreferencesWindow::onAddRepo);
    QPushButton* btnRemoveRepo = new QPushButton("Remove", this);
    connect(btnRemoveRepo, &QPushButton::clicked, this, &PreferencesWindow::onRemoveRepo);

    addLayout->addWidget(m_newRepoEdit, 1);
    addLayout->addWidget(btnAddRepo);
    addLayout->addWidget(btnRemoveRepo);
    reposGroupLayout->addLayout(addLayout);

    reposLayout->addWidget(reposGroup, 1);
    tabWidget->addTab(reposTab, "PCL Repositories");

    mainLayout->addWidget(tabWidget, 1);

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
    m_tensorflowUrlEdit->setText(QString::fromStdString(prefs.getPclTensorflowDownloadUrl()));
    m_tempEdit->setText(QString::fromStdString(prefs.getTemporaryFolder()));
    m_intermediateEdit->setText(QString::fromStdString(prefs.getIntermediateFolder()));
    m_threadSpin->setValue(prefs.getThreadCount());
    m_ramSpin->setValue(prefs.getMaxRamUsage());

    m_reposListWidget->clear();
    for (const auto& repo : prefs.getUpdateRepositories()) {
        m_reposListWidget->addItem(QString::fromStdString(repo));
    }
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
    prefs.setPclTensorflowDownloadUrl(m_tensorflowUrlEdit->text().trimmed().toStdString());
    prefs.setTemporaryFolder(m_tempEdit->text().trimmed().toStdString());
    prefs.setIntermediateFolder(m_intermediateEdit->text().trimmed().toStdString());
    prefs.setThreadCount(m_threadSpin->value());
    prefs.setMaxRamUsage(m_ramSpin->value());

    std::vector<std::string> repos;
    for (int i = 0; i < m_reposListWidget->count(); ++i) {
        repos.push_back(m_reposListWidget->item(i)->text().trimmed().toStdString());
    }
    prefs.setUpdateRepositories(repos);

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

void PreferencesWindow::onAddRepo() {
    QString url = m_newRepoEdit->text().trimmed();
    if (!url.isEmpty()) {
        if (!url.endsWith('/')) {
            url.append('/');
        }
        m_reposListWidget->addItem(url);
        m_newRepoEdit->clear();
    }
}

void PreferencesWindow::onRemoveRepo() {
    QListWidgetItem* item = m_reposListWidget->currentItem();
    if (item) {
        delete m_reposListWidget->takeItem(m_reposListWidget->row(item));
    }
}

} // namespace blastro

