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
