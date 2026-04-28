#include <filesystem>
#include <ostream>
#include <set>
#include <string>
#include <unordered_map>

#include "Config.hpp"
#include "Logger.hpp"
#include "util/util.hpp"

namespace {

    using Feature = std::string;
    using Vertex = std::string;
    using FeatureToVertices = std::unordered_map<Feature, std::set<Vertex>>;

    FeatureToVertices processing(const util::csvData& data) {
        FeatureToVertices result;

        size_t count = 0;
        const size_t total = data.size();
        const std::string label = "Processing : ";

        for (const auto& row : data) {
            Logger::progress(++count, total, label, true);

            if (row.size() < 3)
                throw std::runtime_error("Invalid row: " + util::join(row, ","));

            const Vertex& vert = row[1];
            const Feature& feat = row[2];

            result[feat].insert(vert);
        }

        return result;
    }

    void writeOutput(const FeatureToVertices& result, std::ostream& out) {
        std::vector<std::pair<Feature, std::set<Vertex>>> sorted(result.begin(), result.end());
        std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) { return std::tie(a.second, a.first) < std::tie(b.second, b.first); });

        for (const auto& [feat, verts] : sorted) {
            util::checkInterrupted();
            out << feat << "," << util::join(verts, ",") << "\n";
        }
    }

    bool shouldSkip(const Config& cfg) {
        return !std::filesystem::exists(cfg.toPath("step3", false));
    }

} // namespace

int main() {
    util::setupSignalHandler();

    Config cfg("config.txt");
    if (shouldSkip(cfg))
        return 0;

    const auto data = util::readCSV(cfg.toPath("step3", false));
    const auto result = processing(data);

    try {
        util::SafeOutput out(cfg.toPath("step4", false));
        writeOutput(result, out.stream());
        out.commit();
    } catch (const std::exception& e) {
        if (std::string(e.what()) == "Interrupted")
            return 0;
        throw;
    }

    return 0;
}
