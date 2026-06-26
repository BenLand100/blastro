#pragma once
#include <string>
#include <mutex>

namespace blastro {

class Preferences {
public:
    static Preferences& instance();

    // Getters and setters
    std::string getPclModuleFolder() const;
    void setPclModuleFolder(const std::string& path);

    std::string getPclLibFolder() const;
    void setPclLibFolder(const std::string& path);

    std::string getPclLibraryFolder() const;
    void setPclLibraryFolder(const std::string& path);

    bool getPclLoadTensorflow() const;
    void setPclLoadTensorflow(bool load);

    std::string getTemporaryFolder() const;
    void setTemporaryFolder(const std::string& path);

    std::string getIntermediateFolder() const;
    void setIntermediateFolder(const std::string& path);

    // Save and load
    void load();
    void save();

private:
    Preferences();
    ~Preferences() = default;

    std::string m_pclModuleFolder;
    std::string m_pclLibFolder;
    std::string m_pclLibraryFolder;
    bool m_pclLoadTensorflow;
    std::string m_temporaryFolder;
    std::string m_intermediateFolder;

    mutable std::mutex m_mutex;
};

} // namespace blastro
