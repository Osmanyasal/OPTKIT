#pragma once
 
#include <string>
#include <memory>

#include "utils/logging/spdlog.hh"
#include "utils/logging/stdout_color_sinks.hh"
#include "utils/logging/logger_config.hh"
namespace optkit::utils::logger
{
    class BaseLogger final
    {
    public:
        static void init();
        static std::shared_ptr<spdlog::logger> get_core_logger();
        static std::shared_ptr<spdlog::logger> get_client_logger();

    private:
        static std::shared_ptr<spdlog::logger> core_logger;
        static std::shared_ptr<spdlog::logger> client_logger;

        BaseLogger() {}
        ~BaseLogger() {}
    };
} 

// ENGINE CORE LOGGERS
#define OPTKIT_CORE_DEBUG(...)                                                                                                         \
    spdlog::set_pattern("[%n][%^%l%$][%Y-%m-%d %H:%M:%S.%f][" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "] [%v]"); \
    optkit::utils::logger::BaseLogger::get_client_logger()->debug(__VA_ARGS__)
#define OPTKIT_CORE_TRACE(...)                                                                                                         \
    spdlog::set_pattern("[%n][%^%l%$][%Y-%m-%d %H:%M:%S.%f][" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "] [%v]"); \
    optkit::utils::logger::BaseLogger::get_core_logger()->trace(__VA_ARGS__)
#define OPTKIT_CORE_INFO(...)                                                                                                          \
    spdlog::set_pattern("[%n][%^%l%$][%Y-%m-%d %H:%M:%S.%f][" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "] [%v]"); \
    optkit::utils::logger::BaseLogger::get_core_logger()->info(__VA_ARGS__)
#define OPTKIT_CORE_WARN(...)                                                                                                          \
    spdlog::set_pattern("[%n][%^%l%$][%Y-%m-%d %H:%M:%S.%f][" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "] [%v]"); \
    optkit::utils::logger::BaseLogger::get_core_logger()->warn(__VA_ARGS__)
#define OPTKIT_CORE_ERROR(...)                                                                                                         \
    spdlog::set_pattern("[%n][%^%l%$][%Y-%m-%d %H:%M:%S.%f][" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "] [%v]"); \
    optkit::utils::logger::BaseLogger::get_core_logger()->error(__VA_ARGS__)

// CLIENT LOGGERS
#define OPTKIT_DEBUG(...)                                                                                                              \
    spdlog::set_pattern("[%n][%^%l%$][%Y-%m-%d %H:%M:%S.%f][" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "] [%v]"); \
    optkit::utils::logger::BaseLogger::get_client_logger()->debug(__VA_ARGS__);                                                  
#define OPTKIT_TRACE(...)                                                                                                              \
    spdlog::set_pattern("[%n][%^%l%$][%Y-%m-%d %H:%M:%S.%f][" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "] [%v]"); \
    optkit::utils::logger::BaseLogger::get_client_logger()->trace(__VA_ARGS__);                                                  
#define OPTKIT_INFO(...)                                                                                                               \
    spdlog::set_pattern("[%n][%^%l%$][%Y-%m-%d %H:%M:%S.%f][" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "] [%v]"); \
    optkit::utils::logger::BaseLogger::get_client_logger()->info(__VA_ARGS__);                                                   
#define OPTKIT_WARN(...)                                                                                                               \
    spdlog::set_pattern("[%n][%^%l%$][%Y-%m-%d %H:%M:%S.%f][" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "] [%v]"); \
    optkit::utils::logger::BaseLogger::get_client_logger()->warn(__VA_ARGS__);                                                   
#define OPTKIT_ERROR(...)                                                                                                              \
    spdlog::set_pattern("[%n][%^%l%$][%Y-%m-%d %H:%M:%S.%f][" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "] [%v]"); \
    optkit::utils::logger::BaseLogger::get_client_logger()->error(__VA_ARGS__);                                                  

#if defined(CONF_LOG_DISABLE_TRACE) && CONF_LOG_DISABLE_TRACE
#undef OPTKIT_CORE_TRACE
#undef OPTKIT_TRACE
#define OPTKIT_CORE_TRACE(...)
#define OPTKIT_TRACE(...)
#endif

#if defined(CONF_LOG_DISABLE_DEBUG) && CONF_LOG_DISABLE_DEBUG
#undef OPTKIT_CORE_DEBUG
#undef OPTKIT_DEBUG
#define OPTKIT_CORE_DEBUG(...)
#define OPTKIT_DEBUG(...)
#endif

#if defined(CONF_LOG_DISABLE_INFO) && CONF_LOG_DISABLE_INFO
#undef OPTKIT_CORE_INFO
#undef OPTKIT_INFO
#define OPTKIT_CORE_INFO(...)
#define OPTKIT_INFO(...)
#endif

#if defined(CONF_LOG_DISABLE_WARN) && CONF_LOG_DISABLE_WARN
#undef OPTKIT_CORE_WARN
#undef OPTKIT_WARN
#define OPTKIT_CORE_WARN(...)
#define OPTKIT_WARN(...)
#endif

#if defined(CONF_LOG_DISABLE_ERROR) && CONF_LOG_DISABLE_ERROR
#undef OPTKIT_CORE_ERROR
#undef OPTKIT_ERROR
#define OPTKIT_CORE_ERROR(...)
#define OPTKIT_ERROR(...)
#endif
