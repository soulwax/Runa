// File: src/Core/Log.cpp
// Logging wrapper for spdlog

#include "Log.h"
#include <filesystem>

namespace Runa {

std::shared_ptr<spdlog::logger> Log::s_logger = nullptr;

void Log::init() {
    // Prevent multiple initialization
    if (s_logger) {
        return;
    }

    // Check if logger already exists in spdlog registry and drop it if needed
    try {
        auto existing = spdlog::get("runa2");
        if (existing) {
            spdlog::drop("runa2");
        }
    } catch (...) {
        // Ignore errors when checking/dropping existing logger
    }

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
        // Wrap in try-catch to prevent exception-during-exception
        try {
            // Check if logger already exists before creating
            // spdlog::get() throws if logger doesn't exist, so we catch that
            try {
                auto existing = spdlog::get("runa2");
                if (existing) {
                    s_logger = existing;
                    s_logger->warn("Failed to initialize file logging: {} (using existing logger)", ex.what());
                }
            } catch (const spdlog::spdlog_ex&) {
                // Logger doesn't exist, create a new one
                s_logger = spdlog::stdout_color_mt("runa2");
                s_logger->set_pattern("[%^%l%$] %v");
                s_logger->warn("Failed to initialize file logging: {}", ex.what());
            }
        } catch (...) {
            // Last resort: create a basic logger without registration
            // This should never fail, but if it does, we'll have a nullptr logger
            // which will be handled gracefully by getLogger()
            try {
                auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                console_sink->set_pattern("[%^%l%$] %v");
                s_logger = std::make_shared<spdlog::logger>("runa2_fallback", console_sink);
                s_logger->set_level(spdlog::level::trace);
                // Don't register this one to avoid conflicts
            } catch (...) {
                // If even this fails, s_logger remains nullptr
                // getLogger() will handle this gracefully
            }
        }
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
        try {
            init();
        } catch (...) {
            // If initialization fails completely, create a minimal fallback
            // This prevents exception-during-exception scenarios
            if (!s_logger) {
                try {
                    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                    s_logger = std::make_shared<spdlog::logger>("runa2_emergency", console_sink);
                    s_logger->set_level(spdlog::level::trace);
                } catch (...) {
                    // If even the emergency logger fails, return nullptr
                    // Callers should check for nullptr, but in practice this should never happen
                }
            }
        }
    }
    return s_logger;
}

} // namespace Runa


