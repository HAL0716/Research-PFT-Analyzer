#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

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

    inline std::ifstream openFile(const std::string& path) {
        std::ifstream ifs(path);
        if (!ifs)
            throw std::runtime_error("failed to open: " + path);
        return ifs;
    }

    inline auto readCSV(const std::string& path) {
        auto ifs = openFile(path);

        std::vector<std::vector<std::string>> data;
        std::string line;

        while (std::getline(ifs, line)) {
            std::vector<std::string> row;
            std::stringstream ss(line);
            std::string cell;

            while (std::getline(ss, cell, ',')) {
                row.push_back(cell);
            }

            data.push_back(std::move(row));
        }

        return data;
    }

} // namespace util
