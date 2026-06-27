#include "Preferences.h"
#include <QSettings>
#include <QDir>
#include <thread>

namespace blastro {

Preferences& Preferences::instance() {
    static Preferences inst;
    return inst;
}

Preferences::Preferences() {
    // Default values
    m_pclModuleFolder = (QDir::currentPath() + "/plugins/bin").toStdString();
    m_pclLibFolder = (QDir::currentPath() + "/plugins/lib").toStdString();
    m_pclLibraryFolder = (QDir::currentPath() + "/plugins/library").toStdString();
    m_pclLoadTensorflow = false;
    m_temporaryFolder = "/tmp";
    m_intermediateFolder = (QDir::currentPath() + "/process").toStdString();
    
    // Default thread count to hardware concurrency
    unsigned int cores = std::thread::hardware_concurrency();
    m_threadCount = (cores > 0) ? static_cast<int>(cores) : 4;
    m_stackingMode = "ram";
    m_maxRamUsage = 16;
    m_updateRepositories = { "https://pixinsight.deepsnrastro.com/" };
    
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

bool Preferences::getPclLoadTensorflow() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_pclLoadTensorflow;
}

void Preferences::setPclLoadTensorflow(bool load) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pclLoadTensorflow = load;
}

std::string Preferences::getTemporaryFolder() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_temporaryFolder;
}

void Preferences::setTemporaryFolder(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_temporaryFolder = path;
}

std::string Preferences::getIntermediateFolder() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_intermediateFolder;
}

void Preferences::setIntermediateFolder(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_intermediateFolder = path;
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

void Preferences::load() {
    std::lock_guard<std::mutex> lock(m_mutex);
    QSettings settings("BLastro", "BLastro");
    m_pclModuleFolder = settings.value("Preferences/PclModuleFolder", QString::fromStdString(m_pclModuleFolder)).toString().toStdString();
    m_pclLibFolder = settings.value("Preferences/PclLibFolder", QString::fromStdString(m_pclLibFolder)).toString().toStdString();
    m_pclLibraryFolder = settings.value("Preferences/PclLibraryFolder", QString::fromStdString(m_pclLibraryFolder)).toString().toStdString();
    m_pclLoadTensorflow = settings.value("Preferences/PclLoadTensorflow", m_pclLoadTensorflow).toBool();
    m_temporaryFolder = settings.value("Preferences/TemporaryFolder", QString::fromStdString(m_temporaryFolder)).toString().toStdString();
    m_intermediateFolder = settings.value("Preferences/IntermediateFolder", QString::fromStdString(m_intermediateFolder)).toString().toStdString();
    m_threadCount = settings.value("Preferences/ThreadCount", m_threadCount).toInt();
    m_stackingMode = settings.value("Preferences/StackingMode", QString::fromStdString(m_stackingMode)).toString().toStdString();
    m_maxRamUsage = settings.value("Preferences/MaxRamUsage", m_maxRamUsage).toInt();

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
    settings.setValue("Preferences/PclLoadTensorflow", m_pclLoadTensorflow);
    settings.setValue("Preferences/TemporaryFolder", QString::fromStdString(m_temporaryFolder));
    settings.setValue("Preferences/IntermediateFolder", QString::fromStdString(m_intermediateFolder));
    settings.setValue("Preferences/ThreadCount", m_threadCount);
    settings.setValue("Preferences/StackingMode", QString::fromStdString(m_stackingMode));
    settings.setValue("Preferences/MaxRamUsage", m_maxRamUsage);

    QStringList reposList;
    for (const auto& r : m_updateRepositories) {
        reposList.append(QString::fromStdString(r));
    }
    settings.setValue("Preferences/UpdateRepositories", reposList);
}

} // namespace blastro
