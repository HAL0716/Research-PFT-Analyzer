#include <filesystem>
#include <string>
#include <vector>

#include "Alphabet.hpp"
#include "Analysis/Engine.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Transform.hpp"
#include "util/util.hpp"

namespace {

    void processRows(const util::csvData& rows, std::ostream& out, const Config& cfg) {
        Alphabet alphabet(cfg.Q);
        Analysis::Engine engine(cfg);

        size_t cnt = 0;
        const size_t total = rows.size();
        const std::string label = "Processing N = " + std::to_string(cfg.N) + " : ";

        for (const auto& row : rows) {
            util::checkInterrupted();

            Logger::progress(++cnt, total, label, true);

            engine.set(Transform::toProductSet(row, cfg));

            const auto res = engine.getResult();
            out << util::join(res, ",") << '\n';
        }
    }

    bool shouldSkip(const Config& cfg, bool update) {
        if (!std::filesystem::exists(cfg.toPath("step2-1")))
            return true;
        return std::filesystem::exists(cfg.toPath("step2-2")) && !update;
    }

} // namespace

int main() {
    util::setupSignalHandler();

    constexpr bool Update = false;

    const Config baseConfig("config.txt");
    const size_t maxN = util::calcPower(baseConfig.Q, baseConfig.L);

    for (size_t n = baseConfig.P; n <= maxN; ++n) {
        const Config cfg = baseConfig.withN(n);

        if (shouldSkip(cfg, Update))
            continue;

        const auto rows = util::readCSV(cfg.toPath("step1"));
        if (rows.empty())
            continue;

        util::SafeOutput out(cfg.toPath("step2-2"));

        try {
            processRows(rows, out.stream(), cfg);

            out.commit();
        } catch (const std::runtime_error& e) {
            if (std::string(e.what()) == "Interrupted")
                return 0;
            throw;
        }
    }

    return 0;
}
