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

#include "UpdateManagerDialog.h"
#include "core/Preferences.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTimer>
#include <QPointer>

namespace blastro {

UpdateManagerDialog::UpdateManagerDialog(QWidget* parent)
    : QDialog(parent), m_currentRepoIdx(0), m_currentDownloadIdx(0),
      m_fetchProcess(nullptr), m_downloadProcess(nullptr) {

    setWindowTitle("Download from Repo");
    resize(640, 500);

    setStyleSheet(
        "QDialog { background-color: #202020; color: #ffffff; }"
        "QLabel { background-color: transparent; color: #aaaaaa; font-size: 11px; }"
        "QTreeWidget { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; border-radius: 3px; font-size: 11px; }"
        "QTreeWidget::item:hover { background-color: #4a4a4a; }"
        "QTreeWidget::item:selected { background-color: #007acc; color: #ffffff; }"
        "QTextBrowser { background-color: #282828; color: #cccccc; border: 1px solid #555555; border-radius: 3px; font-size: 11px; padding: 5px; }"
        "QPushButton { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555555; padding: 5px 12px; border-radius: 3px; font-size: 11px; font-weight: bold; }"
        "QPushButton:hover { background-color: #4a4a4a; }"
        "QPushButton:pressed { background-color: #007acc; }"
        "QPushButton#primaryButton { background-color: #007acc; border-color: #007acc; color: #ffffff; }"
        "QPushButton#primaryButton:hover { background-color: #008be5; }"
        "QPushButton#primaryButton:pressed { background-color: #0060a0; }"
        "QProgressBar { border: 1px solid #555555; border-radius: 3px; background-color: #282828; text-align: center; color: #ffffff; font-size: 10px; height: 16px; }"
        "QProgressBar::chunk { background-color: #007acc; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    QLabel* titleLabel = new QLabel("Available Packages from Configured Repositories", this);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 12px; color: #ffffff;");
    mainLayout->addWidget(titleLabel);

    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setHeaderLabels(QStringList() << "Download" << "Version" << "Type" << "Repository Source");
    m_treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_treeWidget->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_treeWidget->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    connect(m_treeWidget, &QTreeWidget::itemSelectionChanged, this, &UpdateManagerDialog::onItemSelectionChanged);
    mainLayout->addWidget(m_treeWidget, 2);

    QLabel* descLabel = new QLabel("Package Description", this);
    descLabel->setStyleSheet("font-weight: bold; color: #ffffff;");
    mainLayout->addWidget(descLabel);

    m_descBrowser = new QTextBrowser(this);
    m_descBrowser->setOpenExternalLinks(true);
    m_descBrowser->setFixedHeight(100);
    mainLayout->addWidget(m_descBrowser, 1);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    mainLayout->addWidget(m_progressBar);

    m_statusLabel = new QLabel("Ready.", this);
    mainLayout->addWidget(m_statusLabel);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    m_checkBtn = new QPushButton("Check for Downloads", this);
    connect(m_checkBtn, &QPushButton::clicked, this, &UpdateManagerDialog::onCheckClicked);
    btnLayout->addWidget(m_checkBtn);

    btnLayout->addStretch(1);

    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnLayout->addWidget(closeBtn);

    m_downloadBtn = new QPushButton("Download", this);
    m_downloadBtn->setObjectName("primaryButton");
    m_downloadBtn->setEnabled(false);
    connect(m_downloadBtn, &QPushButton::clicked, this, &UpdateManagerDialog::onDownloadClicked);
    btnLayout->addWidget(m_downloadBtn);

    mainLayout->addLayout(btnLayout);

    // Initial check on load
    onCheckClicked();
}

UpdateManagerDialog::~UpdateManagerDialog() {
    if (m_fetchProcess) {
        m_fetchProcess->kill();
        m_fetchProcess->waitForFinished();
    }
    if (m_downloadProcess) {
        m_downloadProcess->kill();
        m_downloadProcess->waitForFinished();
    }
}

void UpdateManagerDialog::updateUIState(bool working) {
    m_checkBtn->setEnabled(!working);
    m_downloadBtn->setEnabled(!working && !m_availablePackages.empty());
}

void UpdateManagerDialog::onCheckClicked() {
    m_treeWidget->clear();
    m_availablePackages.clear();
    m_repoQueue.clear();

    m_descBrowser->clear();
    m_downloadBtn->setEnabled(false);

    Preferences& prefs = Preferences::instance();
    std::vector<std::string> repos = prefs.getUpdateRepositories();
    for (const auto& r : repos) {
        m_repoQueue.push_back(QString::fromStdString(r));
    }

    if (m_repoQueue.empty()) {
        m_statusLabel->setText("No repositories configured.");
        return;
    }

    m_currentRepoIdx = 0;
    updateUIState(true);
    startFetchingNextRepo();
}

void UpdateManagerDialog::startFetchingNextRepo() {
    if (m_currentRepoIdx >= (int)m_repoQueue.size()) {
        // Queue finished
        m_statusLabel->setText(QString("Check for downloads complete. Found %1 packages.").arg(m_availablePackages.size()));
        updateUIState(false);
        
        // Populate TreeWidget
        for (size_t i = 0; i < m_availablePackages.size(); ++i) {
            const auto& pkg = m_availablePackages[i];
            QTreeWidgetItem* item = new QTreeWidgetItem(m_treeWidget);
            item->setText(0, pkg.title.isEmpty() ? pkg.fileName : pkg.title);
            item->setText(1, pkg.version);
            item->setText(2, pkg.type);
            
            // Clean display repo source
            QUrl url(pkg.repoUrl);
            item->setText(3, url.host());
            
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(0, Qt::Unchecked);
            item->setData(0, Qt::UserRole, (int)i);
        }
        
        if (m_treeWidget->topLevelItemCount() > 0) {
            m_treeWidget->topLevelItem(0)->setSelected(true);
        }
        return;
    }

    m_currentFetchingRepoUrl = m_repoQueue[m_currentRepoIdx];
    QString xriUrl = m_currentFetchingRepoUrl;
    if (!xriUrl.endsWith("/")) {
        xriUrl.append("/");
    }
    xriUrl.append("updates.xri");

    m_statusLabel->setText(QString("Querying %1...").arg(xriUrl));

    if (m_fetchProcess) {
        m_fetchProcess->deleteLater();
    }
    m_fetchProcess = new QProcess(this);
    m_fetchBuffer.clear();

    connect(m_fetchProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this](int exitCode, QProcess::ExitStatus) {
        onFetchFinished(exitCode);
    });

    // Run curl to retrieve updates.xri.
    // Set typical useragent, 3s connect timeout, 4s max-time.
    m_fetchProcess->start("curl", QStringList() 
        << "-L" 
        << "-s" 
        << "--connect-timeout" << "3"
        << "--max-time" << "4"
        << "-A" << "BLastro/1.0" 
        << xriUrl
    );

    // Safeguard timer: kill the process after 5 seconds if it hasn't exited
    QPointer<QProcess> procPtr(m_fetchProcess);
    QTimer::singleShot(5000, this, [procPtr]() {
        if (procPtr && procPtr->state() == QProcess::Running) {
            procPtr->kill();
        }
    });
}

void UpdateManagerDialog::onFetchFinished(int exitCode) {
    if (exitCode == 0) {
        QByteArray data = m_fetchProcess->readAllStandardOutput();
        if (!data.isEmpty()) {
            parseXriData(data, m_currentFetchingRepoUrl);
        }
    } else {
        // Log query warning silently or in label
        m_statusLabel->setText(QString("Skipped unreachable repository: %1").arg(m_currentFetchingRepoUrl));
    }

    m_currentRepoIdx++;
    startFetchingNextRepo();
}

bool UpdateManagerDialog::platformMatches(const QString& os, const QString& arch) const {
    QString targetOS = os.toLower().trimmed();
    QString targetArch = arch.toLower().trimmed();

    bool osOk = (targetOS == "all" || targetOS == "linux");
    bool archOk = (targetArch == "all" || targetArch == "noarch" || targetArch == "x86_64" || targetArch == "x64");

    return osOk && archOk;
}

void UpdateManagerDialog::parseXriData(const QByteArray& data, const QString& repoUrl) {
    QXmlStreamReader xml(data);
    std::map<QString, UpdateMetadata> metadataMap;
    QString currentOS = "all";
    QString currentArch = "all";

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        
        if (token == QXmlStreamReader::StartElement) {
            QString name = xml.name().toString();
            
            if (name == "metadata") {
                QString metaId = xml.attributes().value("id").toString();
                UpdateMetadata meta;
                
                // Read title and description safely, preserving HTML markup
                while (!xml.atEnd() && !xml.hasError()) {
                    QXmlStreamReader::TokenType t = xml.readNext();
                    if (t == QXmlStreamReader::EndElement && xml.name().toString() == "metadata") {
                        break;
                    }
                    
                    if (t == QXmlStreamReader::StartElement) {
                        QString tagName = xml.name().toString();
                        if (tagName == "title") {
                            meta.title = xml.readElementText(QXmlStreamReader::IncludeChildElements).trimmed();
                        } else if (tagName == "description") {
                            meta.description = "";
                            int depth = 1;
                            while (depth > 0 && !xml.atEnd() && !xml.hasError()) {
                                QXmlStreamReader::TokenType tt = xml.readNext();
                                if (tt == QXmlStreamReader::StartElement) {
                                    depth++;
                                    meta.description += "<" + xml.name().toString();
                                    QXmlStreamAttributes attrs = xml.attributes();
                                    for (const auto& attr : attrs) {
                                        meta.description += " " + attr.name().toString() + "=\"" + attr.value().toString() + "\"";
                                    }
                                    meta.description += ">";
                                } else if (tt == QXmlStreamReader::EndElement) {
                                    depth--;
                                    if (depth > 0) {
                                        meta.description += "</" + xml.name().toString() + ">";
                                    }
                                } else if (tt == QXmlStreamReader::Characters) {
                                    meta.description += xml.text().toString();
                                }
                            }
                            meta.description = meta.description.trimmed();
                        } else {
                            xml.skipCurrentElement();
                        }
                    }
                }
                
                if (!metaId.isEmpty()) {
                    metadataMap[metaId] = meta;
                }
            } else if (name == "platform") {
                currentOS = xml.attributes().value("os").toString();
                currentArch = xml.attributes().value("arch").toString();
                
                // If the platform doesn't match our host platform, skip the entire tag
                if (!platformMatches(currentOS, currentArch)) {
                    xml.skipCurrentElement();
                }
            } else if (name == "package") {
                QXmlStreamAttributes attrs = xml.attributes();
                QString fileName = attrs.value("fileName").toString();
                QString serverURL = attrs.value("serverURL").toString();
                QString sha1 = attrs.value("sha1").toString();
                QString type = attrs.value("type").toString();
                QString version = attrs.value("version").toString();
                QString metaId = attrs.value("metadata").toString();

                if (!fileName.isEmpty()) {
                    UpdatePackage pkg;
                    pkg.fileName = fileName;
                    pkg.sha1 = sha1;
                    pkg.type = type.isEmpty() ? "module" : type;
                    pkg.version = version;
                    pkg.repoUrl = repoUrl;

                    // Resolve base download server URL
                    QString downloadBase = serverURL.isEmpty() ? repoUrl : serverURL;
                    if (!downloadBase.endsWith("/")) {
                        downloadBase.append("/");
                    }
                    pkg.downloadUrl = downloadBase + fileName;

                    // Resolve Metadata info
                    if (!metaId.isEmpty() && metadataMap.find(metaId) != metadataMap.end()) {
                        pkg.title = metadataMap[metaId].title;
                        pkg.description = metadataMap[metaId].description;
                    } else {
                        pkg.title = fileName;
                        pkg.description = "No package description available.";
                    }

                    m_availablePackages.push_back(pkg);
                }
            }
        }
    }
}

void UpdateManagerDialog::onItemSelectionChanged() {
    QList<QTreeWidgetItem*> selected = m_treeWidget->selectedItems();
    if (selected.isEmpty()) {
        m_descBrowser->clear();
        return;
    }

    int idx = selected.first()->data(0, Qt::UserRole).toInt();
    if (idx >= 0 && idx < (int)m_availablePackages.size()) {
        m_descBrowser->setHtml(m_availablePackages[idx].description);
    }
}

void UpdateManagerDialog::onDownloadClicked() {
    m_packagesToDownload.clear();
    for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = m_treeWidget->topLevelItem(i);
        if (item->checkState(0) == Qt::Checked) {
            int idx = item->data(0, Qt::UserRole).toInt();
            m_packagesToDownload.push_back(m_availablePackages[idx]);
        }
    }

    if (m_packagesToDownload.empty()) {
        QMessageBox::information(this, "No Selection", "Please check at least one package to download.");
        return;
    }

    m_currentDownloadIdx = 0;
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0); // Pulse behavior
    updateUIState(true);
    startDownloadingNextPackage();
}

void UpdateManagerDialog::startDownloadingNextPackage() {
    if (m_currentDownloadIdx >= (int)m_packagesToDownload.size()) {
        m_progressBar->setVisible(false);
        m_statusLabel->setText("All selected downloads completed.");
        updateUIState(false);
        QMessageBox::information(this, "Downloads Completed", "Packages downloaded and installed successfully! Please restart BLastro to load new modules.");
        onCheckClicked();
        return;
    }

    const auto& pkg = m_packagesToDownload[m_currentDownloadIdx];
    m_statusLabel->setText(QString("Downloading %1 (%2 of %3)...")
        .arg(pkg.title)
        .arg(m_currentDownloadIdx + 1)
        .arg(m_packagesToDownload.size()));

    QString tempDir = QString::fromStdString(Preferences::instance().getTemporaryFolder());
    QDir().mkpath(tempDir);
    QString tempFilePath = QDir(tempDir).filePath(pkg.fileName);

    if (m_downloadProcess) {
        m_downloadProcess->deleteLater();
    }
    m_downloadProcess = new QProcess(this);

    connect(m_downloadProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, tempFilePath](int exitCode, QProcess::ExitStatus) {
        onDownloadFinished(exitCode);
    });

    m_downloadProcess->start("curl", QStringList()
        << "-L"
        << "-o" << tempFilePath
        << "--connect-timeout" << "5"
        << "--max-time" << "120"
        << "-A" << "BLastro/1.0"
        << pkg.downloadUrl
    );
}

void UpdateManagerDialog::onDownloadFinished(int exitCode) {
    const auto& pkg = m_packagesToDownload[m_currentDownloadIdx];
    QString tempDir = QString::fromStdString(Preferences::instance().getTemporaryFolder());
    QString tempFilePath = QDir(tempDir).filePath(pkg.fileName);

    if (exitCode == 0) {
        bool installed = extractAndInstall(tempFilePath, pkg.type);
        if (!installed) {
            QMessageBox::warning(this, "Download Failed", QString("Failed to extract or copy files from %1").arg(pkg.fileName));
        }
    } else {
        QMessageBox::critical(this, "Download Error", QString("Failed to download package: %1").arg(pkg.title));
    }

    // Clean up temporary downloaded file
    QFile::remove(tempFilePath);

    m_currentDownloadIdx++;
    startDownloadingNextPackage();
}

bool UpdateManagerDialog::extractAndInstall(const QString& archivePath, const QString& type) {
    Q_UNUSED(type);
    Preferences& prefs = Preferences::instance();
    QString moduleFolder = QString::fromStdString(prefs.getPclModuleFolder());
    QString targetDir = QFileInfo(moduleFolder).absolutePath(); // Parent directory of bin, i.e. plugins root

    // Check directory write permissions
    QFileInfo dirInfo(targetDir);
    if (targetDir.isEmpty() || !dirInfo.isWritable()) {
        // Fall back to a local plugins folder under the configuration dotfolder
        QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        targetDir = QDir(configDir).filePath("plugins");
        QDir().mkpath(targetDir);
        
        // Update preference paths to local configuration dotfolder paths
        prefs.setPclModuleFolder(QDir(targetDir).filePath("bin").toStdString());
        prefs.setPclLibFolder(QDir(targetDir).filePath("lib").toStdString());
        prefs.setPclLibraryFolder(QDir(targetDir).filePath("library").toStdString());
        prefs.save();
    }

    // Extract archive
    QProcess extractProc;
    QStringList args;
    
    if (archivePath.endsWith(".zip", Qt::CaseInsensitive)) {
        args << "-o" << archivePath << "-d" << targetDir;
        extractProc.start("unzip", args);
    } else if (archivePath.endsWith(".tar.gz", Qt::CaseInsensitive) || archivePath.endsWith(".tgz", Qt::CaseInsensitive)) {
        args << "-xzf" << archivePath << "-C" << targetDir;
        extractProc.start("tar", args);
    } else {
        // Direct file copy
        QFileInfo fi(archivePath);
        QString destPath = QDir(targetDir).filePath(fi.fileName());
        QFile::remove(destPath);
        return QFile::copy(archivePath, destPath);
    }

    extractProc.waitForFinished(30000);
    return (extractProc.exitCode() == 0);
}

} // namespace blastro
