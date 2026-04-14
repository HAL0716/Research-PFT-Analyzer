#include <filesystem>
#include <string>
#include <vector>

#include "Alphabet.hpp"
#include "Config.hpp"
#include "Graph.hpp"
#include "Logger.hpp"
#include "Transform.hpp"
#include "util/util.hpp"

namespace {

    void processRows(const util::csvData& rows, std::ostream& out, const Config& cfg) {
        auto format = [](const Graph::Verts& verts, const Config& cfg) -> std::string {
            std::vector<size_t> res(cfg.L / cfg.T + 1, 0);
            for (const auto& s : verts) {
                size_t pos = s.find('+');
                if (pos == std::string::npos)
                    pos = s.size();
                res[pos / cfg.T]++;
            }
            return util::join(res, ",");
        };

        Alphabet alphabet(cfg.Q);
        Graph graph(cfg, alphabet);

        size_t cnt = 0;
        const size_t total = rows.size();
        const std::string label = "Processing N = " + std::to_string(cfg.N) + " : ";

        for (const auto& row : rows) {
            util::checkInterrupted();

            Logger::progress(++cnt, total, label, true);

            graph.set(Transform::toWords(row, cfg));
            const auto before = graph.getV();
            graph.minimize();
            const auto after = graph.getV();

            out << format(before, cfg) << ',' << format(after, cfg) << '\n';
        }
    }

    bool shouldSkip(const Config& cfg, bool update) {
        return std::filesystem::exists(cfg.toPath("step2-1")) && !update;
    }

} // namespace

int main() {
    util::setupSignalHandler();

    constexpr bool UPDATE = false;

    const Config baseConfig("config.txt");
    const size_t maxN = util::calcPower(baseConfig.Q, baseConfig.L);

    for (size_t N = baseConfig.P; N <= maxN; ++N) {
        const auto cfg = baseConfig.withN(N);

        if (shouldSkip(cfg, UPDATE))
            continue;

        const auto rows = util::readCSV(cfg.toPath("step1"));
        if (rows.empty())
            continue;

        util::SafeOutput out(cfg.toPath("step2-1"));

        try {
            processRows(rows, out.stream(), cfg);

            out.commit();
        } catch (const std::exception& e) {
            if (std::string(e.what()) == "Interrupted")
                return 0;
            throw;
        }
    }

    return 0;
}
