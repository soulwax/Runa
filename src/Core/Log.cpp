


#include "../runapch.h"
#include "Log.h"

namespace Runa {

std::shared_ptr<spdlog::logger> Log::s_logger = nullptr;

void Log::init() {

    if (s_logger) {
        return;
    }


    try {
        auto existing = spdlog::get("runa2");
        if (existing) {
            spdlog::drop("runa2");
        }
    } catch (...) {

    }

    try {

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern("[%^%l%$] %v");


        std::filesystem::path logDir = "logs";
        if (!std::filesystem::exists(logDir)) {
            std::filesystem::create_directories(logDir);
        }

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/runa2.log", true);
        file_sink->set_level(spdlog::level::trace);
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");


        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        s_logger = std::make_shared<spdlog::logger>("runa2", sinks.begin(), sinks.end());
        s_logger->set_level(spdlog::level::trace);
        s_logger->flush_on(spdlog::level::warn);


        spdlog::register_logger(s_logger);
        spdlog::set_default_logger(s_logger);

    } catch (const spdlog::spdlog_ex& ex) {


        try {


            try {
                auto existing = spdlog::get("runa2");
                if (existing) {
                    s_logger = existing;
                    s_logger->warn("Failed to initialize file logging: {} (using existing logger)", ex.what());
                } else {

                    s_logger = spdlog::stdout_color_mt("runa2");
                    s_logger->set_pattern("[%^%l%$] %v");
                    s_logger->warn("Failed to initialize file logging: {}", ex.what());
                }
            } catch (const spdlog::spdlog_ex&) {

                s_logger = spdlog::stdout_color_mt("runa2");
                s_logger->set_pattern("[%^%l%$] %v");
                s_logger->warn("Failed to initialize file logging: {}", ex.what());
            }
        } catch (...) {



            try {
                auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                console_sink->set_pattern("[%^%l%$] %v");
                s_logger = std::make_shared<spdlog::logger>("runa2_fallback", console_sink);
                s_logger->set_level(spdlog::level::trace);

            } catch (...) {


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

        try {
            init();
        } catch (...) {


            if (!s_logger) {
                try {
                    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                    s_logger = std::make_shared<spdlog::logger>("runa2_emergency", console_sink);
                    s_logger->set_level(spdlog::level::trace);
                } catch (...) {


                    return nullptr;
                }
            }
        }
    }
    return s_logger;
}

}
