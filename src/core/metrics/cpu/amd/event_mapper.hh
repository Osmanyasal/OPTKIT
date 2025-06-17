#pragma once

#include <unordered_map>
#include "utils/utils.hh"
#include "core/metrics/cpu/core_events.hh"
#include "core/metrics/cpu/amd/core_events.hh"

/**
 * @brief metrics are mapped based on the manual:
 *      https://www.amd.com/content/dam/amd/en/documents/epyc-technical-docs/programmer-references/58550-0.01.pdf (Performance Monitor Counters for AMD Family 1Ah Model 00h- 0Fh Processors)
 *
 */
namespace optkit::core::metrics::cpu::amd
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
            return {};
        }

        static std::vector<uint64_t> get(cpu::amd::CoreEvents event)
        {
            auto it = amd_event_map.find(event);
            if (it != amd_event_map.end())
            {
                return it->second;
            }
            return {};
        }

    private:
        EventMapper() {}
        ~EventMapper() {}
        static const std::unordered_map<cpu::CoreEvents, std::vector<uint64_t>> core_event_map;     // coreEvent - even nums to monitor.
        static const std::unordered_map<cpu::amd::CoreEvents, std::vector<uint64_t>> amd_event_map; // coreEvent - even nums to monitor.
    };
};