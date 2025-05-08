#include "utils/logging/logger.hh"

namespace optkit::utils::logger
{
    std::shared_ptr<spdlog::logger> BaseLogger::core_logger;
    std::shared_ptr<spdlog::logger> BaseLogger::client_logger;

    std::shared_ptr<spdlog::logger> BaseLogger::get_core_logger() { return core_logger; }
    std::shared_ptr<spdlog::logger> BaseLogger::get_client_logger() { return client_logger; }

    void BaseLogger::init()
    {
        // Set global log level to trace
        spdlog::set_level(spdlog::level::trace);

        core_logger = spdlog::stdout_color_mt("CORE");
        client_logger = spdlog::stdout_color_mt("CLIENT");
    }
}