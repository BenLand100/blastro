#include "Preferences.h"
#include <QSettings>
#include <QDir>

namespace blastro {

Preferences& Preferences::instance() {
    static Preferences inst;
    return inst;
}

Preferences::Preferences() {
    // Default values
    m_pclModuleFolder = "/opt/PixInsight/bin";
    m_pclLibFolder = "/opt/PixInsight/lib";
    m_pclLibraryFolder = "/opt/PixInsight/library";
    m_pclLoadTensorflow = false;
    m_temporaryFolder = "/tmp";
    m_intermediateFolder = (QDir::currentPath() + "/process").toStdString();
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

void Preferences::load() {
    std::lock_guard<std::mutex> lock(m_mutex);
    QSettings settings("BLastro", "BLastro");
    m_pclModuleFolder = settings.value("Preferences/PclModuleFolder", QString::fromStdString(m_pclModuleFolder)).toString().toStdString();
    m_pclLibFolder = settings.value("Preferences/PclLibFolder", QString::fromStdString(m_pclLibFolder)).toString().toStdString();
    m_pclLibraryFolder = settings.value("Preferences/PclLibraryFolder", QString::fromStdString(m_pclLibraryFolder)).toString().toStdString();
    m_pclLoadTensorflow = settings.value("Preferences/PclLoadTensorflow", m_pclLoadTensorflow).toBool();
    m_temporaryFolder = settings.value("Preferences/TemporaryFolder", QString::fromStdString(m_temporaryFolder)).toString().toStdString();
    m_intermediateFolder = settings.value("Preferences/IntermediateFolder", QString::fromStdString(m_intermediateFolder)).toString().toStdString();
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
}

} // namespace blastro
