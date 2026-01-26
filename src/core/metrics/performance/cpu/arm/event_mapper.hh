#pragma once
#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_ARM

#include <unordered_map>
#include "utils/utils.hh"
#include "core/metrics/performance/cpu/core_events.hh"
#include "core/metrics/performance/cpu/arm/native_events.hh"

/**
 * @brief metrics are mapped based on the manual:
 *
 *
 */
namespace optkit::metrics::performance::arm
{
    /**
     * @brief Considered only Neoverse cpus. others are not guaranteed.
     *
     */
    class EventMapper final
    {
    public:
        static std::vector<uint64_t> get(performance::CoreEvents event)
        {
            auto it = core_event_map.find(event);
            if (it != core_event_map.end())
            {
                return it->second;
            }
            OPTKIT_CORE_WARN("EventMapper: No event found for core event: {}", to_string(event));
            return {};
        }
        static std::vector<uint64_t> get(performance::arm::NativeEvents event)
        {
            auto it = native_event_map.find(event);
            if (it != native_event_map.end())
            {
                return it->second;
            }
            OPTKIT_CORE_WARN("EventMapper: No event found for core event: {}", to_string(event));
            return {};
        }
        static std::vector<uint64_t> get(std::string event);

    private:
        EventMapper() {}
        ~EventMapper() {}
        static const std::unordered_map<performance::CoreEvents, std::vector<uint64_t>> core_event_map;          // coreEvent - even nums to monitor.
        static const std::unordered_map<performance::arm::NativeEvents, std::vector<uint64_t>> native_event_map; // coreEvent - even nums to monitor.
    };
};
#endif // OPTKIT_ENV_CPU_ARM