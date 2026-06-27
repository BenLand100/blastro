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
#include <string>
#include <mutex>
#include <vector>

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

    int getThreadCount() const;
    void setThreadCount(int count);

    std::string getStackingMode() const;
    void setStackingMode(const std::string& mode);

    int getMaxRamUsage() const;
    void setMaxRamUsage(int gb);

    std::vector<std::string> getUpdateRepositories() const;
    void setUpdateRepositories(const std::vector<std::string>& repos);

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
    int m_threadCount;
    std::string m_stackingMode;
    int m_maxRamUsage;
    std::vector<std::string> m_updateRepositories;

    mutable std::mutex m_mutex;
};

} // namespace blastro
