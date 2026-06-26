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
