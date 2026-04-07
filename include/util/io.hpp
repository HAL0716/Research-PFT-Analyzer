#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "util/string.hpp"

namespace util {
    namespace fs = std::filesystem;

    using csvRow = std::vector<std::string>;
    using csvData = std::vector<csvRow>;

    inline std::ofstream createFile(const std::string& path) {
        fs::path fs_path(path);
        if (fs_path.has_parent_path())
            fs::create_directories(fs_path.parent_path());

        std::ofstream ofs(path);
        if (!ofs)
            throw std::runtime_error("failed to open: " + path);

        return ofs;
    }

    enum class FileErrorPolicy {
        THROW,
        RETURN_EMPTY
    };

    inline std::ifstream openFile(const std::string& path, FileErrorPolicy policy = FileErrorPolicy::THROW) {
        std::ifstream ifs(path);
        if (!ifs && policy == FileErrorPolicy::THROW)
            throw std::runtime_error("failed to open: " + path);
        return ifs;
    }

    inline auto readCSV(const std::string& path, FileErrorPolicy policy = FileErrorPolicy::RETURN_EMPTY) {
        auto ifs = openFile(path, policy);
        csvData data;

        if (!ifs && policy == FileErrorPolicy::RETURN_EMPTY)
            return data;

        std::string line;

        while (std::getline(ifs, line)) {
            csvData::value_type row;
            std::stringstream ss(line);
            std::string cell;

            while (std::getline(ss, cell, ',')) {
                row.push_back(cell);
            }

            data.push_back(std::move(row));
        }

        return data;
    }

    inline void writeCSV(const std::string& path, const csvData& data) {
        auto ofs = createFile(path);

        for (const auto& row : data)
            ofs << util::join(row, ",") << "\n";

        std::cout << path << " saved." << std::endl;
    }

} // namespace util
