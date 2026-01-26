#pragma once
#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_AMD

#include <unordered_map>
#include "utils/utils.hh"
#include "core/metrics/performance/cpu/core_events.hh"
#include "core/metrics/performance/cpu/amd/native_events.hh"

/**
 * @brief metrics are mapped based on the manual:
 *      https://www.amd.com/content/dam/amd/en/documents/epyc-technical-docs/programmer-references/58550-0.01.pdf (Performance Monitor Counters for AMD Family 1Ah Model 00h- 0Fh Processors)
 *
 */
namespace optkit::metrics::performance::amd
{
    /**
     * @brief Considered only zen1 to zen4. others are not guaranteed.
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
        static std::vector<uint64_t> get(performance::amd::NativeEvents event)
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
        static const std::unordered_map<performance::amd::NativeEvents, std::vector<uint64_t>> native_event_map; // coreEvent - even nums to monitor.
    };

};
#endif // OPTKIT_ENV_CPU_AMD