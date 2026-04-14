#pragma once

#include <atomic>
#include <csignal>
#include <stdexcept>

namespace util {

    namespace detail {
        inline std::atomic<bool> interrupted = false;

        inline void handler(int) {
            interrupted.store(true, std::memory_order_relaxed);
        }
    } // namespace detail

    inline void setupSignalHandler() {
        std::signal(SIGINT, detail::handler);
    }

    inline bool isInterrupted() {
        return detail::interrupted.load(std::memory_order_relaxed);
    }

    inline void checkInterrupted() {
        if (isInterrupted())
            throw std::runtime_error("Interrupted");
    }

} // namespace util
