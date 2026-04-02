#pragma once
#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_RISCV

#include <unordered_map>
#include "utils/utils.hh"
#include "core/metrics/performance/cpu/core_events.hh"
#include "core/metrics/performance/cpu/riscv/native_events.hh"

/**
 * @brief metrics are mapped based on the manual:
 *
 *
 */
namespace optkit::metrics::performance::cpu::riscv
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

        static std::vector<uint64_t> get(performance::riscv::NativeEvents event)
        {
            auto it = native_event_map.find(event);
            if (it != native_event_map.end())
            {
                return it->second;
            }
            OPTKIT_CORE_WARN("EventMapper: No event found for native event: {}", to_string(event));
            return {};
        }
        static const std::vector<std::string> &get_supported_core_events()
        {
            static const std::vector<std::string> supported_core_events = [] {
                std::vector<std::string> supported_events;
                supported_events.reserve(core_event_map.size());
                for (int event_index = static_cast<int>(performance::cpu::CoreEvents::BEGIN) + 1;
                     event_index < static_cast<int>(performance::cpu::CoreEvents::END);
                     ++event_index)
                {
                    const auto event = static_cast<performance::cpu::CoreEvents>(event_index);
                    if (core_event_map.find(event) != core_event_map.end())
                    {
                        supported_events.push_back(to_string(event));
                    }
                }
                return supported_events;
            }();
            return supported_core_events;
        }
        static std::vector<uint64_t> get(std::string event);

    private:
        EventMapper() {}
        ~EventMapper() {}
        static const std::unordered_map<performance::CoreEvents, std::vector<uint64_t>> core_event_map;          // coreEvent - even nums to monitor.
        static const std::unordered_map<performance::riscv::NativeEvents, std::vector<uint64_t>> native_event_map; // nativeEvent - even nums to monitor.
    };
};
#endif // OPTKIT_ENV_CPU_RISCV