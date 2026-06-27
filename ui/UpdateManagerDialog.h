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
    void onCheckClicked();
    void onDownloadClicked();
    void onItemSelectionChanged();
    void onFetchFinished(int exitCode);
    void onDownloadFinished(int exitCode);

private:
    void startFetchingNextRepo();
    void parseXriData(const QByteArray& data, const QString& repoUrl);
    bool platformMatches(const QString& os, const QString& arch) const;
    void startDownloadingNextPackage();
    bool extractAndInstall(const QString& archivePath, const QString& type);
    void updateUIState(bool working);

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
    int m_currentDownloadIdx;

    QProcess* m_fetchProcess;
    QProcess* m_downloadProcess;
    QByteArray m_fetchBuffer;
    QString m_currentFetchingRepoUrl;
};

} // namespace blastro
