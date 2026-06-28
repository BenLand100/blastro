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
#include "PreferencesWindow.h"
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
#include <QSettings>
#include <QCoreApplication>

#include <QRegularExpression>
#include <QSet>
#include <QUrl>

namespace blastro {

// Helper to query ZIP or tar.gz file list
static QStringList getArchiveFiles(const QString& archivePath) {
    QStringList files;
    QProcess listProc;
    if (archivePath.endsWith(".zip", Qt::CaseInsensitive)) {
        listProc.start("unzip", QStringList() << "-Z" << "-1" << archivePath);
    } else if (archivePath.endsWith(".tar.gz", Qt::CaseInsensitive) || archivePath.endsWith(".tgz", Qt::CaseInsensitive)) {
        listProc.start("tar", QStringList() << "-tf" << archivePath);
    } else {
        QFileInfo fi(archivePath);
        files << fi.fileName();
        return files;
    }
    
    if (listProc.waitForFinished(10000)) {
        if (listProc.exitCode() == 0) {
            QString out = QString::fromUtf8(listProc.readAllStandardOutput());
            QStringList lines = out.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
            for (QString line : lines) {
                line = line.trimmed();
                if (!line.isEmpty() && !line.endsWith("/")) {
                    files << line;
                }
            }
        }
    }
    return files;
}

// QSettings storage helpers
static void saveInstalledPackage(const UpdatePackage& pkg, const QStringList& files) {
    QSettings settings("BLastro", "BLastro");
    QString group = QString("InstalledPackages/%1/").arg(pkg.fileName);
    settings.setValue(group + "title", pkg.title);
    settings.setValue(group + "description", pkg.description);
    settings.setValue(group + "fileName", pkg.fileName);
    settings.setValue(group + "downloadUrl", pkg.downloadUrl);
    settings.setValue(group + "sha1", pkg.sha1);
    settings.setValue(group + "type", pkg.type);
    settings.setValue(group + "version", pkg.version);
    settings.setValue(group + "repoUrl", pkg.repoUrl);
    settings.setValue(group + "files", files);
}

static std::vector<UpdatePackage> getInstalledPackages() {
    std::vector<UpdatePackage> list;
    QSettings settings("BLastro", "BLastro");
    settings.beginGroup("InstalledPackages");
    QStringList keys = settings.childGroups();
    for (const auto& key : keys) {
        settings.beginGroup(key);
        UpdatePackage pkg;
        pkg.title = settings.value("title").toString();
        pkg.description = settings.value("description").toString();
        pkg.fileName = settings.value("fileName").toString();
        pkg.downloadUrl = settings.value("downloadUrl").toString();
        pkg.sha1 = settings.value("sha1").toString();
        pkg.type = settings.value("type").toString();
        pkg.version = settings.value("version").toString();
        pkg.repoUrl = settings.value("repoUrl").toString();
        list.push_back(pkg);
        settings.endGroup();
    }
    settings.endGroup();
    return list;
}

static QStringList getInstalledPackageFiles(const QString& fileName) {
    QSettings settings("BLastro", "BLastro");
    return settings.value(QString("InstalledPackages/%1/files").arg(fileName)).toStringList();
}

static void removeInstalledPackage(const QString& fileName) {
    QSettings settings("BLastro", "BLastro");
    
    // Delete files first (checking refcounts)
    QStringList files = getInstalledPackageFiles(fileName);
    Preferences& prefs = Preferences::instance();
    QString moduleFolder = QString::fromStdString(prefs.getPclModuleFolder());
    QString targetDir = QFileInfo(moduleFolder).absolutePath(); // plugins root
    
    // Check directory write permissions
    QFileInfo dirInfo(targetDir);
    if (targetDir.isEmpty() || !dirInfo.isWritable()) {
        QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        targetDir = QDir(configDir).filePath("plugins");
    }

    // Get all other installed packages' files to compute refcounts dynamically
    std::vector<UpdatePackage> allInstalled = getInstalledPackages();
    QSet<QString> otherFiles;
    for (const auto& otherPkg : allInstalled) {
        if (otherPkg.fileName != fileName) {
            QStringList of = getInstalledPackageFiles(otherPkg.fileName);
            for (const auto& f : of) {
                otherFiles.insert(f);
            }
        }
    }

    for (const auto& file : files) {
        if (otherFiles.contains(file)) {
            qInfo() << "[Uninstall] File" << file << "is shared by another package, skipping physical deletion.";
            continue; // Do not delete this file as it's still owned by another package!
        }
        QString fullPath = QDir(targetDir).filePath(file);
        QFile::remove(fullPath);
        // Also remove parent directory if empty
        QDir().rmdir(QFileInfo(fullPath).absolutePath());
    }

    // Remove from settings
    settings.remove(QString("InstalledPackages/%1").arg(fileName));
}

#include <QDateTime>

static void saveAvailablePackagesCache(const std::vector<UpdatePackage>& packages) {
    QSettings settings("BLastro", "BLastro");
    settings.remove("AvailablePackagesCache");
    settings.beginWriteArray("AvailablePackagesCache/packages");
    for (size_t i = 0; i < packages.size(); ++i) {
        settings.setArrayIndex(i);
        const auto& pkg = packages[i];
        settings.setValue("title", pkg.title);
        settings.setValue("description", pkg.description);
        settings.setValue("fileName", pkg.fileName);
        settings.setValue("downloadUrl", pkg.downloadUrl);
        settings.setValue("sha1", pkg.sha1);
        settings.setValue("type", pkg.type);
        settings.setValue("version", pkg.version);
        settings.setValue("repoUrl", pkg.repoUrl);
    }
    settings.endArray();
    settings.setValue("AvailablePackagesCache/LastCheckTimestamp", QDateTime::currentDateTime().toString(Qt::ISODate));
}

static std::vector<UpdatePackage> loadAvailablePackagesCache(bool& ok) {
    std::vector<UpdatePackage> packages;
    QSettings settings("BLastro", "BLastro");
    if (!settings.contains("AvailablePackagesCache/LastCheckTimestamp")) {
        ok = false;
        return packages;
    }
    
    int size = settings.beginReadArray("AvailablePackagesCache/packages");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        UpdatePackage pkg;
        pkg.title = settings.value("title").toString();
        pkg.description = settings.value("description").toString();
        pkg.fileName = settings.value("fileName").toString();
        pkg.downloadUrl = settings.value("downloadUrl").toString();
        pkg.sha1 = settings.value("sha1").toString();
        pkg.type = settings.value("type").toString();
        pkg.version = settings.value("version").toString();
        pkg.repoUrl = settings.value("repoUrl").toString();
        packages.push_back(pkg);
    }
    settings.endArray();
    ok = true;
    return packages;
}

UpdateManagerDialog::UpdateManagerDialog(QWidget* parent)
    : QDialog(parent), m_currentRepoIdx(0), m_currentDownloadIdx(0),
      m_fetchProcess(nullptr), m_downloadProcess(nullptr) {

    setWindowTitle("PCL Repo Packages");
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

    QLabel* noteLabel = new QLabel("Repositories can be configured in <a href=\"preferences\" style=\"color: #007acc; text-decoration: none;\">Preferences</a>.", this);
    noteLabel->setTextFormat(Qt::RichText);
    noteLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    connect(noteLabel, &QLabel::linkActivated, this, [this](const QString&) {
        QDialog prefDlg(this);
        prefDlg.setWindowTitle("Preferences");
        prefDlg.resize(600, 500);
        QVBoxLayout* layout = new QVBoxLayout(&prefDlg);
        layout->setContentsMargins(0, 0, 0, 0);
        PreferencesWindow* prefWin = new PreferencesWindow(&prefDlg);
        prefWin->setAttribute(Qt::WA_DeleteOnClose);
        layout->addWidget(prefWin);
        connect(prefWin, &QObject::destroyed, &prefDlg, &QDialog::accept);
        prefDlg.exec();
        onCheckClicked(true);
    });
    mainLayout->addWidget(noteLabel);

    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setHeaderLabels(QStringList() << "Package" << "Version" << "Type" << "Repository Source");
    m_treeWidget->header()->setSectionResizeMode(0, QHeaderView::Interactive);
    m_treeWidget->header()->setSectionResizeMode(1, QHeaderView::Interactive);
    m_treeWidget->header()->setSectionResizeMode(2, QHeaderView::Interactive);
    m_treeWidget->header()->setSectionResizeMode(3, QHeaderView::Interactive);
    m_treeWidget->setColumnWidth(0, 320);
    m_treeWidget->setColumnWidth(1, 80);
    m_treeWidget->setColumnWidth(2, 100);
    m_treeWidget->setColumnWidth(3, 180);
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
    connect(m_checkBtn, &QPushButton::clicked, this, [this]() { onCheckClicked(true); });
    btnLayout->addWidget(m_checkBtn);

    btnLayout->addStretch(1);

    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnLayout->addWidget(closeBtn);

    m_downloadBtn = new QPushButton("Apply Changes", this);
    m_downloadBtn->setObjectName("primaryButton");
    m_downloadBtn->setEnabled(false);
    connect(m_downloadBtn, &QPushButton::clicked, this, &UpdateManagerDialog::onApplyChangesClicked);
    btnLayout->addWidget(m_downloadBtn);

    mainLayout->addLayout(btnLayout);

    // Initial check on load
    onCheckClicked(false);
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
    m_downloadBtn->setEnabled(!working);
}

void UpdateManagerDialog::onCheckClicked(bool force) {
    m_treeWidget->clear();
    m_availablePackages.clear();
    m_repoQueue.clear();

    m_descBrowser->clear();
    m_downloadBtn->setEnabled(false);

    if (!force) {
        // Try to load from cache if less than 1 day old
        QSettings settings("BLastro", "BLastro");
        QString tsStr = settings.value("AvailablePackagesCache/LastCheckTimestamp").toString();
        if (!tsStr.isEmpty()) {
            QDateTime lastCheck = QDateTime::fromString(tsStr, Qt::ISODate);
            if (lastCheck.isValid() && lastCheck.secsTo(QDateTime::currentDateTime()) < 86400 && lastCheck.secsTo(QDateTime::currentDateTime()) >= 0) {
                bool ok = false;
                m_availablePackages = loadAvailablePackagesCache(ok);
                if (ok) {
                    qInfo() << "[Update Cache] Loaded available packages list from cache (timestamp:" << tsStr << ")";
                    populateTreeWidget();
                    return;
                }
            }
        }
    }

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

void UpdateManagerDialog::populateTreeWidget() {
    m_treeWidget->clear();

    std::vector<UpdatePackage> installedPkgs = getInstalledPackages();
    QMap<QString, UpdatePackage> installedMap;
    for (const auto& pkg : installedPkgs) {
        installedMap[pkg.fileName] = pkg;
    }

    // Group active packages by repo URL
    QMap<QString, std::vector<size_t>> repoToPackageIndices;
    for (size_t i = 0; i < m_availablePackages.size(); ++i) {
        repoToPackageIndices[m_availablePackages[i].repoUrl].push_back(i);
    }

    // Keep track of which installed fileNames are found in the repos
    QSet<QString> foundFileNames;

    // Populate tree by repo
    for (auto it = repoToPackageIndices.begin(); it != repoToPackageIndices.end(); ++it) {
        QString repoUrl = it.key();
        const auto& indices = it.value();

        QTreeWidgetItem* repoGroupItem = new QTreeWidgetItem(m_treeWidget);
        repoGroupItem->setText(0, QString("Repository: %1").arg(repoUrl));
        repoGroupItem->setFlags(repoGroupItem->flags() & ~Qt::ItemIsUserCheckable);
        
        QFont font = repoGroupItem->font(0);
        font.setBold(true);
        repoGroupItem->setFont(0, font);
        repoGroupItem->setExpanded(true);

        for (size_t idx : indices) {
            const auto& pkg = m_availablePackages[idx];
            foundFileNames.insert(pkg.fileName);

            QTreeWidgetItem* item = new QTreeWidgetItem(repoGroupItem);
            item->setText(0, pkg.title.isEmpty() ? pkg.fileName : pkg.title);
            item->setText(1, pkg.version);
            item->setText(2, pkg.type);
            item->setText(3, QUrl(pkg.repoUrl).host());

            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            bool isInstalled = installedMap.contains(pkg.fileName);
            item->setCheckState(0, isInstalled ? Qt::Checked : Qt::Unchecked);
            item->setData(0, Qt::UserRole, (int)idx);
            item->setData(0, Qt::UserRole + 1, "repo");
        }
    }

    // Find and populate orphaned/missing packages
    QTreeWidgetItem* orphanedGroupItem = nullptr;
    for (const auto& pkg : installedPkgs) {
        if (!foundFileNames.contains(pkg.fileName)) {
            if (!orphanedGroupItem) {
                orphanedGroupItem = new QTreeWidgetItem(m_treeWidget);
                orphanedGroupItem->setText(0, "Orphaned / Missing Packages (Installed but missing from repos)");
                orphanedGroupItem->setFlags(orphanedGroupItem->flags() & ~Qt::ItemIsUserCheckable);
                
                QFont font = orphanedGroupItem->font(0);
                font.setBold(true);
                orphanedGroupItem->setFont(0, font);
                orphanedGroupItem->setExpanded(true);
            }

            QTreeWidgetItem* item = new QTreeWidgetItem(orphanedGroupItem);
            item->setText(0, (pkg.title.isEmpty() ? pkg.fileName : pkg.title) + " (Missing)");
            item->setText(1, pkg.version);
            item->setText(2, pkg.type);
            item->setText(3, pkg.repoUrl);

            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(0, Qt::Checked); // Missing packages are installed
            item->setForeground(0, QBrush(Qt::red));
            
            item->setData(0, Qt::UserRole + 1, "orphaned");
            item->setData(0, Qt::UserRole + 2, pkg.title);
            item->setData(0, Qt::UserRole + 3, pkg.description);
            item->setData(0, Qt::UserRole + 4, pkg.fileName);
            item->setData(0, Qt::UserRole + 5, pkg.downloadUrl);
            item->setData(0, Qt::UserRole + 6, pkg.sha1);
            item->setData(0, Qt::UserRole + 7, pkg.type);
            item->setData(0, Qt::UserRole + 8, pkg.version);
            item->setData(0, Qt::UserRole + 9, pkg.repoUrl);
        }
    }

    // Select first leaf item
    QTreeWidgetItemIterator itemIt(m_treeWidget);
    while (*itemIt) {
        if ((*itemIt)->childCount() == 0 && (*itemIt)->parent()) {
            (*itemIt)->setSelected(true);
            break;
        }
        ++itemIt;
    }

    m_downloadBtn->setEnabled(true);
}

void UpdateManagerDialog::startFetchingNextRepo() {
    if (m_currentRepoIdx >= (int)m_repoQueue.size()) {
        // Queue finished
        m_statusLabel->setText(QString("Check complete. Found %1 packages.").arg(m_availablePackages.size()));
        updateUIState(false);
        
        // Save fetched packages to cache
        saveAvailablePackagesCache(m_availablePackages);

        populateTreeWidget();
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

    QTreeWidgetItem* item = selected.first();
    QString itemType = item->data(0, Qt::UserRole + 1).toString();
    if (itemType == "repo") {
        int idx = item->data(0, Qt::UserRole).toInt();
        if (idx >= 0 && idx < (int)m_availablePackages.size()) {
            m_descBrowser->setHtml(m_availablePackages[idx].description);
        }
    } else if (itemType == "orphaned") {
        m_descBrowser->setHtml(item->data(0, Qt::UserRole + 3).toString());
    } else {
        m_descBrowser->clear();
    }
}

void UpdateManagerDialog::onApplyChangesClicked() {
    m_packagesToDownload.clear();
    m_packagesToUninstall.clear();

    std::vector<UpdatePackage> installedPkgs = getInstalledPackages();
    QMap<QString, UpdatePackage> installedMap;
    for (const auto& pkg : installedPkgs) {
        installedMap[pkg.fileName] = pkg;
    }

    // Traverse the tree widget to find checks/unchecks
    QTreeWidgetItemIterator it(m_treeWidget);
    while (*it) {
        QTreeWidgetItem* item = *it;
        QString itemType = item->data(0, Qt::UserRole + 1).toString();

        if (itemType == "repo") {
            int idx = item->data(0, Qt::UserRole).toInt();
            const auto& pkg = m_availablePackages[idx];
            bool currentlyInstalled = installedMap.contains(pkg.fileName);
            bool checked = (item->checkState(0) == Qt::Checked);

            if (checked && !currentlyInstalled) {
                m_packagesToDownload.push_back(pkg);
            } else if (!checked && currentlyInstalled) {
                m_packagesToUninstall.push_back(pkg);
            }
        } else if (itemType == "orphaned") {
            bool checked = (item->checkState(0) == Qt::Checked);
            if (!checked) {
                // Orphaned package uninstalled
                UpdatePackage pkg;
                pkg.title = item->data(0, Qt::UserRole + 2).toString();
                pkg.description = item->data(0, Qt::UserRole + 3).toString();
                pkg.fileName = item->data(0, Qt::UserRole + 4).toString();
                pkg.downloadUrl = item->data(0, Qt::UserRole + 5).toString();
                pkg.sha1 = item->data(0, Qt::UserRole + 6).toString();
                pkg.type = item->data(0, Qt::UserRole + 7).toString();
                pkg.version = item->data(0, Qt::UserRole + 8).toString();
                pkg.repoUrl = item->data(0, Qt::UserRole + 9).toString();
                m_packagesToUninstall.push_back(pkg);
            }
        }
        ++it;
    }

    if (m_packagesToDownload.empty() && m_packagesToUninstall.empty()) {
        QMessageBox::information(this, "No Changes", "No changes were made to package check states.");
        return;
    }

    // Confirm uninstallation
    if (!m_packagesToUninstall.empty()) {
        QStringList names;
        for (const auto& pkg : m_packagesToUninstall) {
            names << QString("- %1 (%2)").arg(pkg.title.isEmpty() ? pkg.fileName : pkg.title).arg(pkg.version);
        }
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Changes",
            QString("You are about to uninstall the following package(s):\n\n%1\n\nAre you sure you want to proceed?")
            .arg(names.join("\n")),
            QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) {
            return;
        }
    }

    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0); // Pulse behavior
    updateUIState(true);

    m_currentUninstallIdx = 0;
    startUninstallingNextPackage();
}

void UpdateManagerDialog::startUninstallingNextPackage() {
    if (m_currentUninstallIdx >= (int)m_packagesToUninstall.size()) {
        // Uninstallation complete, proceed to downloads/installations
        m_currentDownloadIdx = 0;
        startDownloadingNextPackage();
        return;
    }

    const auto& pkg = m_packagesToUninstall[m_currentUninstallIdx];
    m_statusLabel->setText(QString("Uninstalling %1...").arg(pkg.title));
    QCoreApplication::processEvents();

    removeInstalledPackage(pkg.fileName);

    m_currentUninstallIdx++;
    startUninstallingNextPackage();
}

void UpdateManagerDialog::startDownloadingNextPackage() {
    if (m_currentDownloadIdx >= (int)m_packagesToDownload.size()) {
        m_progressBar->setVisible(false);
        m_statusLabel->setText("All package modifications applied successfully.");
        updateUIState(false);
        QMessageBox::information(this, "Changes Applied", "Packages updated successfully! Please restart BLastro to reload modules.");
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
        // Collision checks
        QStringList archiveFiles = getArchiveFiles(tempFilePath);
        std::vector<UpdatePackage> installedPkgs = getInstalledPackages();

        bool stopQueue = false;
        
        for (const auto& file : archiveFiles) {
            for (const auto& oldPkg : installedPkgs) {
                if (oldPkg.fileName == pkg.fileName) continue; // skip self
                
                QStringList oldFiles = getInstalledPackageFiles(oldPkg.fileName);
                if (oldFiles.contains(file)) {
                    // Collision found! Prompt user
                    QMessageBox msgBox(this);
                    msgBox.setWindowTitle("File Collision Detected");
                    msgBox.setText(QString("The package '%1' wants to install the file '%2'\nwhich is already owned by package '%3'.")
                                   .arg(pkg.title).arg(file).arg(oldPkg.title));
                    msgBox.setInformativeText("Would you like to uninstall the other package first, share file ownership, or cancel?");
                    
                    QPushButton* uninstallBtn = msgBox.addButton("Uninstall Other", QMessageBox::ActionRole);
                    QPushButton* shareBtn = msgBox.addButton("Share Ownership", QMessageBox::ActionRole);
                    QPushButton* cancelBtn = msgBox.addButton(QMessageBox::Cancel);
                    msgBox.setDefaultButton(shareBtn);
                    msgBox.exec();

                    if (msgBox.clickedButton() == uninstallBtn) {
                        qInfo() << "[Collision] Uninstalling" << oldPkg.fileName << "to resolve collision.";
                        removeInstalledPackage(oldPkg.fileName);
                        // Refresh list
                        installedPkgs = getInstalledPackages();
                        break;
                    } else if (msgBox.clickedButton() == cancelBtn) {
                        qInfo() << "[Collision] Installation cancelled.";
                        stopQueue = true;
                        break;
                    } else {
                        qInfo() << "[Collision] Share ownership chosen.";
                        break;
                    }
                }
            }
            if (stopQueue) break;
        }

        if (stopQueue) {
            QFile::remove(tempFilePath);
            m_progressBar->setVisible(false);
            m_statusLabel->setText("Operation cancelled.");
            updateUIState(false);
            onCheckClicked();
            return;
        }

        bool installed = extractAndInstall(tempFilePath, pkg.type);
        if (installed) {
            saveInstalledPackage(pkg, archiveFiles);
        } else {
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
