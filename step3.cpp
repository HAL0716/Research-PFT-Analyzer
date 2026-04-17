#include <algorithm>
#include <ostream>
#include <span>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "Config.hpp"
#include "Logger.hpp"
#include "util/util.hpp"

namespace {

    struct Record {
        size_t id;
        std::string key;

        bool operator==(const Record& other) const {
            return std::tie(id, key) == std::tie(other.id, other.key);
        }

        bool operator<(const Record& other) const {
            return std::tie(id, key) < std::tie(other.id, other.key);
        }

        static Record make(size_t id, const util::csvRow& data1, const util::csvRow& data2) {
            size_t mid = data1.size() / 2;

            std::span<const std::string> v1(data1.data(), mid);
            std::span<const std::string> v2(data1.data() + mid, data1.size() - mid);

            return Record{id, util::join(v1, "-") + "," + util::join(v2, "-") + "," + util::join(data2, "-")};
        }

        struct Hash {
            size_t operator()(const Record& r) const {
                size_t h = std::hash<size_t>()(r.id);
                h ^= std::hash<std::string>()(r.key) + 0x9e3779b9 + (h << 6) + (h >> 2);
                return h;
            }
        };
    };

    using RecordSet = std::unordered_set<Record, Record::Hash>;

    void updateRecords(size_t id, const util::csvData& data1, const util::csvData& data2, RecordSet& records) {
        for (size_t i = 0; i < data1.size(); ++i) {
            util::checkInterrupted();

            records.insert(Record::make(id, data1[i], data2[i]));
        }
    }

    void writeOutput(const RecordSet& records, std::ostream& out) {
        std::vector<Record> sorted(records.begin(), records.end());
        std::sort(sorted.begin(), sorted.end());

        for (const auto& rec : sorted)
            out << rec.key << '\n';
    }

} // namespace

int main() {
    util::setupSignalHandler();

    const Config baseConfig("config.txt");
    const size_t maxN = util::calcPower(baseConfig.Q, baseConfig.L);

    RecordSet records;

    for (size_t N = baseConfig.P; N <= maxN; ++N) {
        Logger::progress(N, maxN, "Processing N = " + std::to_string(N) + ": ", true);

        const auto cfg = baseConfig.withN(N);

        const auto data1 = util::readCSV(cfg.toPath("step2-1"));
        const auto data2 = util::readCSV(cfg.toPath("step2-2"));
        if (data1.size() != data2.size())
            continue;

        try {
            updateRecords(N, data1, data2, records);
        } catch (const std::exception& e) {
            if (std::string(e.what()) == "Interrupted")
                return 0;
            throw;
        }
    }

    util::SafeOutput out(baseConfig.toPath("step3", false));
    writeOutput(records, out.stream());
    out.commit();

    return 0;
}
