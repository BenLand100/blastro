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
#include <vector>
#include <mutex>

namespace blastro {

class TempDirectory {
public:
    // Returns the path to a newly created unique temporary subdirectory inside the workspace-relative .blastro_temp/ folder.
    static std::string createTempDir(const std::string& prefix);
    
    // Cleans up all registered temporary directories and their contents.
    static void cleanup();

    // Splits off extension, adds suffix, and re-appends extension
    static std::string getIntermediateFileName(const std::string& originalPath, const std::string& suffix, int fallbackIndex);

private:
    static std::vector<std::string> s_tempDirs;
    static std::mutex s_mutex;
};

} // namespace blastro
