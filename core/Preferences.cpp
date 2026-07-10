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

#include "Preferences.h"
#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <thread>

namespace blastro {

Preferences& Preferences::instance() {
    static Preferences inst;
    return inst;
}

Preferences::Preferences() {
    // Default values pointing to AppConfigLocation (~/.config/BLastro on Linux)
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty()) {
        configDir = QDir::homePath() + "/.config/BLastro";
    }
    m_pclModuleFolder = QDir(configDir).filePath("plugins/bin").toStdString();
    m_pclLibFolder = QDir(configDir).filePath("plugins/lib").toStdString();
    m_pclLibraryFolder = QDir(configDir).filePath("plugins/library").toStdString();
    m_pclPreloadLibDir = false;
    m_pclTensorflowDownloadUrl = "https://download.starnetastro.com/pixinsight/legacy/tensorflow-runtime/pixinsight_tensorflow_runtime_linux_TF_x64.zip";
    m_temporaryFolder = "/tmp";
    m_processFolderName = "process";
    
    // Default thread count to hardware concurrency
    unsigned int cores = std::thread::hardware_concurrency();
    m_threadCount = (cores > 0) ? static_cast<int>(cores) : 4;
    m_stackingMode = "ram";
    m_maxRamUsage = 16;
    m_updateRepositories = { "https://pixinsight.deepsnrastro.com/" };
    m_astapBinaryPath = "/usr/bin/astap";
    m_solveFieldBinaryPath = "/usr/bin/solve-field";
    
    load();
}

std::string Preferences::getPclModuleFolder() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_pclModuleFolder;
}

void Preferences::setPclModuleFolder(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pclModuleFolder = path;
}

std::string Preferences::getPclLibFolder() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_pclLibFolder;
}

void Preferences::setPclLibFolder(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pclLibFolder = path;
}

std::string Preferences::getPclLibraryFolder() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_pclLibraryFolder;
}

void Preferences::setPclLibraryFolder(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pclLibraryFolder = path;
}

bool Preferences::getPclPreloadLibDir() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_pclPreloadLibDir;
}

void Preferences::setPclPreloadLibDir(bool preload) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pclPreloadLibDir = preload;
}

std::string Preferences::getPclTensorflowDownloadUrl() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_pclTensorflowDownloadUrl;
}

void Preferences::setPclTensorflowDownloadUrl(const std::string& url) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pclTensorflowDownloadUrl = url;
}

std::string Preferences::getTemporaryFolder() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_temporaryFolder;
}

void Preferences::setTemporaryFolder(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_temporaryFolder = path;
}

std::string Preferences::getProcessFolderName() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_processFolderName;
}

void Preferences::setProcessFolderName(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_processFolderName = name;
}

int Preferences::getThreadCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_threadCount;
}

void Preferences::setThreadCount(int count) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_threadCount = count;
}

std::string Preferences::getStackingMode() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_stackingMode;
}

void Preferences::setStackingMode(const std::string& mode) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_stackingMode = mode;
}

int Preferences::getMaxRamUsage() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_maxRamUsage;
}

void Preferences::setMaxRamUsage(int gb) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_maxRamUsage = gb;
}

std::vector<std::string> Preferences::getUpdateRepositories() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_updateRepositories;
}

void Preferences::setUpdateRepositories(const std::vector<std::string>& repos) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_updateRepositories = repos;
}

std::string Preferences::getAstapBinaryPath() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_astapBinaryPath;
}

void Preferences::setAstapBinaryPath(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_astapBinaryPath = path;
}

std::string Preferences::getSolveFieldBinaryPath() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_solveFieldBinaryPath;
}

void Preferences::setSolveFieldBinaryPath(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_solveFieldBinaryPath = path;
}

void Preferences::load() {
    std::lock_guard<std::mutex> lock(m_mutex);
    QSettings settings("BLastro", "BLastro");
    m_pclModuleFolder = settings.value("Preferences/PclModuleFolder", QString::fromStdString(m_pclModuleFolder)).toString().toStdString();
    m_pclLibFolder = settings.value("Preferences/PclLibFolder", QString::fromStdString(m_pclLibFolder)).toString().toStdString();
    m_pclLibraryFolder = settings.value("Preferences/PclLibraryFolder", QString::fromStdString(m_pclLibraryFolder)).toString().toStdString();
    m_pclPreloadLibDir = settings.value("Preferences/PclPreloadLibDir", m_pclPreloadLibDir).toBool();
    m_pclTensorflowDownloadUrl = settings.value("Preferences/PclTensorflowDownloadUrl", QString::fromStdString(m_pclTensorflowDownloadUrl)).toString().toStdString();
    m_temporaryFolder = settings.value("Preferences/TemporaryFolder", QString::fromStdString(m_temporaryFolder)).toString().toStdString();
    m_processFolderName = settings.value("Preferences/ProcessFolderName", QString::fromStdString(m_processFolderName)).toString().toStdString();
    m_threadCount = settings.value("Preferences/ThreadCount", m_threadCount).toInt();
    m_stackingMode = settings.value("Preferences/StackingMode", QString::fromStdString(m_stackingMode)).toString().toStdString();
    m_maxRamUsage = settings.value("Preferences/MaxRamUsage", m_maxRamUsage).toInt();
    m_astapBinaryPath = settings.value("Preferences/AstapBinaryPath", QString::fromStdString(m_astapBinaryPath)).toString().toStdString();
    m_solveFieldBinaryPath = settings.value("Preferences/SolveFieldBinaryPath", QString::fromStdString(m_solveFieldBinaryPath)).toString().toStdString();

    QStringList defaultRepos;
    for (const auto& r : m_updateRepositories) {
        defaultRepos.append(QString::fromStdString(r));
    }
    QStringList reposList = settings.value("Preferences/UpdateRepositories", defaultRepos).toStringList();
    m_updateRepositories.clear();
    for (const auto& r : reposList) {
        m_updateRepositories.push_back(r.toStdString());
    }
}

void Preferences::save() {
    std::lock_guard<std::mutex> lock(m_mutex);
    QSettings settings("BLastro", "BLastro");
    settings.setValue("Preferences/PclModuleFolder", QString::fromStdString(m_pclModuleFolder));
    settings.setValue("Preferences/PclLibFolder", QString::fromStdString(m_pclLibFolder));
    settings.setValue("Preferences/PclLibraryFolder", QString::fromStdString(m_pclLibraryFolder));
    settings.setValue("Preferences/PclPreloadLibDir", m_pclPreloadLibDir);
    settings.setValue("Preferences/PclTensorflowDownloadUrl", QString::fromStdString(m_pclTensorflowDownloadUrl));
    settings.setValue("Preferences/TemporaryFolder", QString::fromStdString(m_temporaryFolder));
    settings.setValue("Preferences/ProcessFolderName", QString::fromStdString(m_processFolderName));
    settings.setValue("Preferences/ThreadCount", m_threadCount);
    settings.setValue("Preferences/StackingMode", QString::fromStdString(m_stackingMode));
    settings.setValue("Preferences/MaxRamUsage", m_maxRamUsage);
    settings.setValue("Preferences/AstapBinaryPath", QString::fromStdString(m_astapBinaryPath));
    settings.setValue("Preferences/SolveFieldBinaryPath", QString::fromStdString(m_solveFieldBinaryPath));

    QStringList reposList;
    for (const auto& r : m_updateRepositories) {
        reposList.append(QString::fromStdString(r));
    }
    settings.setValue("Preferences/UpdateRepositories", reposList);
}

} // namespace blastro
