#include <iostream>

#include "Config.hpp"
#include "Logger.hpp"
#include "util/util.hpp"

namespace {

    struct Record {
        std::vector<std::string> features;
        int num;
        int uniqueSize;
    };

    auto readCSV(const Config& cfg) {
        const auto csv1 = util::readCSV(cfg.toPath("step2-1"));
        const auto csv2 = util::readCSV(cfg.toPath("step2-2"));

        std::vector<Record> result;

        if (csv1.empty() || csv2.empty() || csv1.size() != csv2.size())
            return result;

        for (size_t i = 0; i < csv1.size(); ++i) {
            const auto& row1 = csv1[i];
            const auto& row2 = csv2[i];

            Record r;
            r.features = row2;
            r.num = std::stoi(row1[1]);
            r.uniqueSize = r.features.size();

            result.push_back(std::move(r));
        }

        return result;
    }

    void writeCSV(const std::vector<Record>& res, const Config& cfg) {
        const auto csvPath = cfg.toPath("step3");
        auto csv = util::createFile(csvPath);
        size_t cnt = 0, total = res.size();
        for (const auto& r : res) {
            Logger::progress(++cnt, total, "Writing Results: ", true);

            std::vector<std::string> resultRow = r.features;
            resultRow.push_back(std::to_string(r.num));
            resultRow.push_back(std::to_string(r.uniqueSize));

            csv << util::join(resultRow, ",") << std::endl;
        }
        std::cout << csvPath << " Saved." << std::endl;
    }

} // namespace

int main() {
    const Config base("config.txt");

    const size_t maxN = util::calcPower(base.Q, base.L);
    for (size_t N = 1; N <= maxN; ++N) {
        if (N < base.P)
            continue;

        const auto cfg = base.withN(N);
        auto data = readCSV(cfg);

        if (!data.empty())
            writeCSV(data, cfg);
    }

    return 0;
}
