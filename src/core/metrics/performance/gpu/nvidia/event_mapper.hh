#pragma once
#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_LIB_NVML

#include <unordered_map>
#include "utils/utils.hh"
#include "core/metrics/performance/gpu/core_events.hh"
#include "core/metrics/performance/gpu/nvidia/native_events.hh"

/**
 * @brief metrics are mapped based on the manual:
 *
 *
 */
namespace optkit::metrics::performance::gpu::nvidia
{
    /**
     * @brief Considered only Neoverse gpu. others are not guaranteed.
     *
     */
    class EventMapper final
    {
    public:
        static std::vector<uint64_t> get(performance::gpu::CoreEvents event)
        {
            auto it = core_event_map.find(event);
            if (it != core_event_map.end())
            {
                return it->second;
            }
            OPTKIT_CORE_WARN("EventMapper: No event found for core event: {}", to_string(event));
            return {};
        }
        static std::vector<uint64_t> get(performance::gpu::nvidia::NativeEvents event)
        {
            auto it = native_event_map.find(event);
            if (it != native_event_map.end())
            {
                return it->second;
            }
            OPTKIT_CORE_WARN("EventMapper: No event found for native event: {}", to_string(event));
            return {};
        }
        static std::vector<uint64_t> get(std::string event);

    private:
        EventMapper() {}
        ~EventMapper() {}
        static const std::unordered_map<performance::gpu::CoreEvents, std::vector<uint64_t>> core_event_map;          // coreEvent - even nums to monitor.
        static const std::unordered_map<performance::gpu::nvidia::NativeEvents, std::vector<uint64_t>> native_event_map; // nativeEvent - even nums to monitor.
    };
};
#endif // OPTKIT_ENV_LIB_NVML