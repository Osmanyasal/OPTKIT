#pragma once
#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_INTEL

#include <unordered_map>
#include "utils/utils.hh"
#include "core/metrics/cpu/core_events.hh"
#include "core/metrics/cpu/intel/native_events.hh"


namespace optkit::core::metrics::cpu::intel
{
    /**
     * @brief Considered only zen1 to zen4. others are not guaranteed.
     *
     */
    class EventMapper final
    {
    public:
        static std::vector<uint64_t> get(cpu::CoreEvents event)
        {
            auto it = core_event_map.find(event);
            if (it != core_event_map.end())
            {
                return it->second;
            }
            OPTKIT_CORE_WARN("EventMapper: No event found for core event: {}", to_string(event));
            return {};
        }
        static std::vector<uint64_t> get(cpu::intel::NativeEvents event)
        {
            auto it = native_event_map.find(event);
            if (it != native_event_map.end())
            {
                return it->second;
            }
            OPTKIT_CORE_WARN("EventMapper: No event found for core event: {}", to_string(event));
            return {};
        }

    private:
        EventMapper() {}
        ~EventMapper() {}
        static const std::unordered_map<cpu::CoreEvents, std::vector<uint64_t>> core_event_map;     // coreEvent - even nums to monitor.
        static const std::unordered_map<cpu::intel::NativeEvents, std::vector<uint64_t>> native_event_map; // coreEvent - even nums to monitor.
    };
};
#endif