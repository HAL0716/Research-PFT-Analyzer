#include <atomic>
#include <csignal>
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

    std::atomic<bool> g_interrupted = false;

    void onSigint(int) {
        g_interrupted = true;
    }

    struct OutputGuard {
        std::string path;
        bool success = false;

        ~OutputGuard() {
            if (!success && std::filesystem::exists(path))
                std::filesystem::remove(path);
        }
    };

    void processRows(const util::csvData& rows, std::ofstream& out, const Config& cfg) {
        Alphabet alphabet(cfg.Q);
        Analysis::Engine engine(cfg, alphabet);

        size_t cnt = 0;
        const size_t total = rows.size();
        const std::string label = "Processing N = " + std::to_string(cfg.N) + " : ";
        for (const auto& row : rows) {
            if (g_interrupted)
                throw std::runtime_error("Interrupted");

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
    std::signal(SIGINT, onSigint);

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

        const auto finalPath = cfg.toPath("step2-2");
        const auto tmpPath = finalPath + ".tmp";

        auto outFile = util::createFile(tmpPath);
        OutputGuard guard{tmpPath};

        try {
            processRows(rows, outFile, cfg);

            outFile.close();

            std::filesystem::rename(tmpPath, finalPath);

            guard.success = true;
        } catch (const std::runtime_error& e) {
            if (std::string(e.what()) == "Interrupted")
                return 0;
            throw;
        }
    }

    return 0;
}
