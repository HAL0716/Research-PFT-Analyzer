#pragma once

#include <fstream>
#include <sstream>
#include <string>

namespace util {
    namespace fs = std::filesystem;

    inline std::ofstream createFile(const std::string& path) {
        fs::path fs_path(path);
        if (fs_path.has_parent_path())
            fs::create_directories(fs_path.parent_path());

        std::ofstream ofs(path);
        if (!ofs)
            throw std::runtime_error("failed to open: " + path);

        return ofs;
    }

} // namespace util
