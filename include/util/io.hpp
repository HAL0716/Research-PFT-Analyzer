#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
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

            if (!line.empty() && line.back() == ',') {
                row.emplace_back("");
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

    class SafeOutput {
      public:
        explicit SafeOutput(const std::string& finalPath) : finalPath_(finalPath), tmpPath_(finalPath + ".tmp"), ofs_(createFile(tmpPath_)) {
        }

        std::ofstream& stream() {
            return ofs_;
        }

        void commit() {
            ofs_.close();

            if (fs::file_size(tmpPath_) == 0) {
                fs::remove(tmpPath_);
                committed_ = true;
                return;
            }

            if (fs::exists(finalPath_))
                fs::remove(finalPath_);

            fs::rename(tmpPath_, finalPath_);
            committed_ = true;
        }

        ~SafeOutput() {
            if (!committed_) {
                std::error_code ec;
                fs::remove(tmpPath_, ec);
            }
        }

      private:
        std::string finalPath_;
        std::string tmpPath_;
        std::ofstream ofs_;
        bool committed_ = false;
    };

    class Encoder {
      public:
        std::pair<size_t, bool> getOrCreateId(std::string_view s) {
            auto [it, inserted] = str2id.try_emplace(std::string(s), next_id);
            if (inserted)
                ++next_id;
            return {it->second, inserted};
        }

      private:
        std::unordered_map<std::string, size_t> str2id;
        size_t next_id = 0;
    };

    class Decoder {
      public:
        void define(size_t id, std::string_view s) {
            if (id != id2str.size())
                throw std::runtime_error("Invalid ID order");
            id2str.emplace_back(s);
        }

        std::string_view get(size_t id) const {
            if (id >= id2str.size())
                throw std::runtime_error("Unknown ID");
            return id2str[id];
        }

      private:
        std::vector<std::string> id2str;
    };

} // namespace util
