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

    void processRows(const util::csvData& rows, FeatureToVertices& result) {
        for (const auto& row : rows) {
            if (row.size() < 3)
                throw std::runtime_error("Invalid Row: " + util::join(row, ","));

            const Vertex& vert = row[1];
            const std::vector<Feature> feats(row.begin() + 2, row.end());

            result[util::join(feats, ":")].insert(vert);
        }
    }

    void writeOutput(const FeatureToVertices& result, std::ostream& out) {
        std::vector<std::pair<Feature, std::set<Vertex>>> sorted(result.begin(), result.end());
        std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) { return std::tie(a.second, a.first) < std::tie(b.second, b.first); });

        size_t cnt = 0;
        const size_t total = sorted.size();
        const std::string label = "Writing : ";

        for (const auto& [feat, verts] : sorted) {
            util::checkInterrupted();

            Logger::progress(++cnt, total, label, true);

            out << feat << "," << util::join(verts, ",") << "\n";
        }
    }

    bool shouldSkip(const Config& cfg) {
        return !std::filesystem::exists(cfg.toPath("step3"));
    }

} // namespace

int main() {
    util::setupSignalHandler();

    const Config baseConfig("config.txt");
    const size_t maxN = util::calcPower(baseConfig.Q, baseConfig.L);

    util::SafeOutput out(baseConfig.toPath("step4", false));

    FeatureToVertices res;

    for (size_t N = baseConfig.P; N <= maxN; ++N) {
        Logger::progress(N, maxN, "Processing N = " + std::to_string(N) + " : ", true);

        const auto cfg = baseConfig.withN(N);

        if (shouldSkip(cfg))
            continue;

        const auto row = util::readCSV(cfg.toPath("step3"));
        if (row.empty())
            continue;

        processRows(row, res);
    }

    try {
        writeOutput(res, out.stream());

        out.commit();
    } catch (const std::exception& e) {
        if (std::string(e.what()) == "Interrupted")
            return 0;
        throw;
    }

    return 0;
}
