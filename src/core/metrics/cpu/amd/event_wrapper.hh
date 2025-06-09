#pragma once

#include <unordered_map>
#include "utils/utils.hh"
#include "core/metrics/cpu/core_events.hh"
#include "core/metrics/cpu/amd/core_events.hh"

namespace optkit::core::metrics::cpu::amd
{
    class AMDEventWrapper final
    {
    public:
        static uint64_t get(cpu::CoreEvents event)
        {
            auto it = core_event_map.find(event);
            if (it != core_event_map.end())
            {
                return it->second;
            }
            return 0x0;
        }

        static uint64_t get(cpu::amd::CoreEvents event)
        {
            auto it = amd_event_map.find(event);
            if (it != amd_event_map.end())
            {
                return it->second;
            }
            return 0x0;
        }

    private:
        AMDEventWrapper() {}
        ~AMDEventWrapper() {}
        static const std::unordered_map<cpu::CoreEvents, uint64_t> core_event_map;   // coreEvent - even nums to monitor.
        static const std::unordered_map<cpu::amd::CoreEvents, uint64_t> amd_event_map;   // coreEvent - even nums to monitor.
    };
};