// File: src/Core/Log.cpp
// Logging wrapper for spdlog

#include "Log.h"
#include <filesystem>

namespace Runa {

static std::shared_ptr<spdlog::logger> s_logger = nullptr;

void Log::init() {
    try {
        // Create console sink with colors
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern("[%^%l%$] %v");

        // Create file sink
        std::filesystem::path logDir = "logs";
        if (!std::filesystem::exists(logDir)) {
            std::filesystem::create_directories(logDir);
        }
        
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/runa2.log", true);
        file_sink->set_level(spdlog::level::trace);
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

        // Create logger with both sinks
        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        s_logger = std::make_shared<spdlog::logger>("runa2", sinks.begin(), sinks.end());
        s_logger->set_level(spdlog::level::trace);
        s_logger->flush_on(spdlog::level::warn);

        // Register as default logger
        spdlog::register_logger(s_logger);
        spdlog::set_default_logger(s_logger);

    } catch (const spdlog::spdlog_ex& ex) {
        // Fallback to basic console logger if file logging fails
        s_logger = spdlog::stdout_color_mt("runa2");
        s_logger->set_pattern("[%^%l%$] %v");
        s_logger->warn("Failed to initialize file logging: {}", ex.what());
    }
}

void Log::shutdown() {
    if (s_logger) {
        s_logger->flush();
        spdlog::drop_all();
        s_logger = nullptr;
    }
}

std::shared_ptr<spdlog::logger> Log::getLogger() {
    if (!s_logger) {
        // Auto-initialize if not already initialized
        init();
    }
    return s_logger;
}

} // namespace Runa


