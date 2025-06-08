#pragma once

#include <unordered_map>
#include "utils/utils.hh"
#include "core/metrics/cpu/core_events.hh"
#include "core/metrics/cpu/intel/core_events.hh"

namespace optkit::core::metrics::cpu::intel
{
    class IntelEventWrapper final
    {
    public:
        static std::vector<uint64_t> get(cpu::CoreEvents event)
        {
            auto it = core_event_map.find(event);
            if (it != core_event_map.end())
            {
                return it->second;
            }
            return std::vector<uint64_t>{};
        }

        static std::vector<uint64_t> get(cpu::intel::CoreEvents event)
        {
            auto it = intel_event_map.find(event);
            if (it != intel_event_map.end())
            {
                return it->second;
            }
            return std::vector<uint64_t>{};
        }


    private:
        IntelEventWrapper() {}
        ~IntelEventWrapper() {}
        static const std::unordered_map<cpu::CoreEvents, std::vector<uint64_t>> core_event_map;   // coreEvent - even nums to monitor.
        static const std::unordered_map<cpu::intel::CoreEvents, std::vector<uint64_t>> intel_event_map;   // coreEvent - even nums to monitor.
    };
};