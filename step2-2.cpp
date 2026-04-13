#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "Alphabet.hpp"
#include "Analysis/Engine.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Transform.hpp"
#include "util/util.hpp"

namespace {

    void processRows(const util::csvData& rows, std::ofstream& out, const Config& cfg) {
        Alphabet alphabet(cfg.Q);
        Analysis::Engine engine(cfg, alphabet);

        size_t cnt = 0, total = rows.size();
        std::string label = "Processing N = " + std::to_string(cfg.N) + " : ";
        for (const auto& row : rows) {
            Logger::progress(++cnt, total, label, true);

            engine.set(Transform::toProductSet(row, cfg));

            const auto res = engine.getResult();
            out << util::join(res, ",") << '\n';
        }
    }

    bool shouldSkip(const Config& cfg, bool update) {
        return std::filesystem::exists(cfg.toPath("step2-2")) && !update;
    }

} // namespace

int main() {
    constexpr bool kUpdate = false;

    const Config baseConfig("config.txt");
    const size_t maxN = util::calcPower(baseConfig.Q, baseConfig.L);

    for (size_t n = baseConfig.P; n <= maxN; ++n) {
        const Config cfg = baseConfig.withN(n);

        if (shouldSkip(cfg, kUpdate))
            continue;

        const auto rows = util::readCSV(cfg.toPath("step1"));
        if (rows.empty())
            continue;

        auto outFile = util::createFile(cfg.toPath("step2-2"));

        processRows(rows, outFile, cfg);
    }

    return 0;
}
