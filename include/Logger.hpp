#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

struct Logger {
    static void update(const std::string& s) {
        write(s, false);
    }

    static void finish(const std::string& s) {
        write(s, true);
    }

    static void progress(std::size_t current, std::size_t total, const std::string& label = "", bool autoFinish = false) {
        const int percent = calcPercent(current, total);
        const int width = std::to_string(total).size();

        std::ostringstream msg;
        msg << label << makeBar(current, total) << " " << std::setw(width) << current << "/" << total << " (" << percent << " %)";

        write(msg.str(), autoFinish && current >= total);
    }

  private:
    static void write(const std::string& s, bool newline) {
        std::cout << "\r\033[K" << s;
        if (newline)
            std::cout << '\n';
        std::cout << std::flush;
    }

    static int calcPercent(std::size_t current, std::size_t total) {
        return (total == 0) ? 0 : static_cast<int>(100 * current / total);
    }

    static std::string makeBar(std::size_t current, std::size_t total) {
        const int width = 30;
        const int filled = (total == 0) ? 0 : (width * current / total);

        return "[" + std::string(filled, '#') + std::string(width - filled, ' ') + "]";
    }
};
