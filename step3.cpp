#include <algorithm>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "Config.hpp"
#include "Logger.hpp"
#include "util/util.hpp"

namespace {

    void processRows(const util::csvData& rows1, const util::csvData& rows2, std::ostream& out, const Config& cfg) {
        util::Decoder decoder1, decoder2;

        auto resolve = [&](const auto& key, const auto& val, util::Decoder& decoder) -> std::string {
            const size_t id = std::stoul(key);
            if (!val.empty())
                decoder.define(id, val);
            return std::string(decoder.get(id));
        };

        auto toRecord = [&](const auto& row1, const auto& row2) -> std::string {
            if (row1.size() != 4)
                throw std::runtime_error("Invalid Row1: " + util::join(row1, ","));
            if (row2.size() % 2 != 0)
                throw std::runtime_error("Invalid Row2: " + util::join(row2, ","));

            std::vector<std::string> res;
            res.push_back(resolve(row1[0], row1[1], decoder1));
            res.push_back(resolve(row1[2], row1[3], decoder1));
            for (size_t i = 0; i < row2.size(); i += 2)
                res.push_back(resolve(row2[i], row2[i + 1], decoder2));

            return util::join(res, ",");
        };

        std::unordered_set<std::string> records;

        for (size_t i = 0; i < rows1.size(); ++i) {
            util::checkInterrupted();

            Logger::progress(i + 1, rows1.size(), "Processing N = " + std::to_string(cfg.N) + " : ", true);

            records.emplace(toRecord(rows1[i], rows2[i]));
        }

        std::vector<std::string> sorted(records.begin(), records.end());
        std::sort(sorted.begin(), sorted.end());

        size_t cnt = 0;
        const size_t total = sorted.size();
        const std::string label = "Writing N = " + std::to_string(cfg.N) + " : ";

        for (const auto& rec : sorted) {
            util::checkInterrupted();

            Logger::progress(++cnt, total, label, true);

            out << rec << '\n';
        }
    }

    bool shouldSkip(const Config& cfg) {
        return std::filesystem::exists(cfg.toPath("step3")) && !cfg.UPDATE;
    }

} // namespace

int main() {
    util::setupSignalHandler();

    const Config baseConfig("config.txt");
    const size_t maxN = util::calcPower(baseConfig.Q, baseConfig.L);

    for (size_t N = baseConfig.P; N <= maxN; ++N) {
        const auto cfg = baseConfig.withN(N);

        if (shouldSkip(cfg))
            continue;

        const auto rows1 = util::readCSV(cfg.toPath("step2-1"));
        const auto rows2 = util::readCSV(cfg.toPath("step2-2"));
        if (rows1.size() != rows2.size())
            continue;

        util::SafeOutput out(cfg.toPath("step3"));

        try {
            processRows(rows1, rows2, out.stream(), cfg);

            out.commit();
        } catch (const std::exception& e) {
            if (std::string(e.what()) == "Interrupted")
                return 0;
            throw;
        }
    }

    return 0;
}
