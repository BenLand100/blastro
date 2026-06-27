#include "TempDirectory.h"
#include "Preferences.h"
#include <QDir>
#include <QUuid>
#include <QFileInfo>
#include <QDebug>

namespace blastro {

std::vector<std::string> TempDirectory::s_tempDirs;
std::mutex TempDirectory::s_mutex;

std::string TempDirectory::createTempDir(const std::string& prefix) {
    std::lock_guard<std::mutex> lock(s_mutex);
    
    // Create base folder from the configured Intermediate Folder
    std::string baseFolder = Preferences::instance().getIntermediateFolder();
    QDir baseDir(QString::fromStdString(baseFolder));
    if (!baseDir.exists()) {
        baseDir.mkpath(".");
    }
    
    QString uniqueSub = QString::fromStdString(prefix) + "_" + QUuid::createUuid().toString(QUuid::WithoutBraces);
    
    if (baseDir.mkdir(uniqueSub)) {
        QString fullPath = baseDir.absoluteFilePath(uniqueSub);
        std::string pathStr = fullPath.toStdString();
        s_tempDirs.push_back(pathStr);
        qDebug() << "[TempDirectory] Created intermediate directory:" << fullPath;
        return pathStr;
    }
    
    return "";
}

void TempDirectory::cleanup() {
    std::lock_guard<std::mutex> lock(s_mutex);
    for (const auto& pathStr : s_tempDirs) {
        QDir dir(QString::fromStdString(pathStr));
        if (dir.exists()) {
            qDebug() << "[TempDirectory] Cleaning up intermediate directory:" << dir.absolutePath();
            dir.removeRecursively();
        }
    }
    s_tempDirs.clear();
}

std::string TempDirectory::getIntermediateFileName(const std::string& originalPath, const std::string& suffix, int fallbackIndex) {
    if (originalPath.empty()) {
        return "frame_" + std::to_string(fallbackIndex) + suffix + ".fits";
    }
    QFileInfo fileInfo(QString::fromStdString(originalPath));
    QString baseName = fileInfo.completeBaseName();
    QString ext = fileInfo.suffix();
    if (ext.isEmpty()) {
        ext = "fits";
    }
    return (baseName + "_" + QString::number(fallbackIndex) + QString::fromStdString(suffix) + "." + ext).toStdString();
}

} // namespace blastro
