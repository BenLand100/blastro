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

#pragma once
#include <QDialog>
#include <QTreeWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QTextBrowser>
#include <QProcess>
#include <QXmlStreamReader>
#include <vector>
#include <map>

namespace blastro {

struct UpdatePackage {
    QString title;
    QString description;
    QString fileName;
    QString downloadUrl;
    QString sha1;
    QString type;
    QString version;
    QString repoUrl;
};

struct UpdateMetadata {
    QString title;
    QString description;
};

class UpdateManagerDialog : public QDialog {
    Q_OBJECT
public:
    explicit UpdateManagerDialog(QWidget* parent = nullptr);
    ~UpdateManagerDialog() override;

private slots:
    void onCheckClicked(bool force = true);
    void onApplyChangesClicked();
    void onItemSelectionChanged();
    void onFetchFinished(int exitCode);
    void onDownloadFinished(int exitCode);

private:
    void startFetchingNextRepo();
    void parseXriData(const QByteArray& data, const QString& repoUrl);
    bool platformMatches(const QString& os, const QString& arch) const;
    void startDownloadingNextPackage();
    void startUninstallingNextPackage();
    bool extractAndInstall(const QString& archivePath, const QString& type);
    void updateUIState(bool working);
    void populateTreeWidget();

    QTreeWidget* m_treeWidget;
    QTextBrowser* m_descBrowser;
    QPushButton* m_checkBtn;
    QPushButton* m_downloadBtn;
    QProgressBar* m_progressBar;
    QLabel* m_statusLabel;

    std::vector<QString> m_repoQueue;
    int m_currentRepoIdx;
    std::vector<UpdatePackage> m_availablePackages;
    std::vector<UpdatePackage> m_packagesToDownload;
    std::vector<UpdatePackage> m_packagesToUninstall;
    int m_currentDownloadIdx;
    int m_currentUninstallIdx;

    QProcess* m_fetchProcess;
    QProcess* m_downloadProcess;
    QByteArray m_fetchBuffer;
    QString m_currentFetchingRepoUrl;
};

} // namespace blastro
