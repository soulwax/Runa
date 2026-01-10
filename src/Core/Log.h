// File: src/Core/Log.h

#ifndef RUNA_CORE_LOG_H
#define RUNA_CORE_LOG_H

#include "../runapch.h"
#include "../RunaAPI.h"

namespace Runa {

class RUNA_API Log {
private:
    static std::shared_ptr<spdlog::logger> s_logger;

public:

    static void init();


    static void shutdown();


    static std::shared_ptr<spdlog::logger> getLogger();
};

}


#define LOG_TRACE(...)    Runa::Log::getLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...)    Runa::Log::getLogger()->debug(__VA_ARGS__)
#define LOG_INFO(...)     Runa::Log::getLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     Runa::Log::getLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    Runa::Log::getLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) Runa::Log::getLogger()->critical(__VA_ARGS__)

#endif
