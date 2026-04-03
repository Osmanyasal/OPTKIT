#pragma once

#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_RISCV

#include "core/metrics/performance/cpu/core_metrics.hh"
#include "core/metrics/performance/cpu/riscv/event_mapper.hh"
#include "core/metrics/performance/cpu/riscv/native_events.hh"

#include <limits>
#include <vector>

namespace optkit::metrics::performance::cpu
{
    class RISCVMetricsImpl
    {
    };

    template <>
    class CoreMetrics<RISCVMetricsImpl>
    {
    public:
        static const std::vector<std::string> &get_all_metrics()
        {
            static const std::vector<std::string> names = {
                "instructions",
                "LLC-load-misses",
                "LLC-store-misses",
                "l3_mpki",
            };
            return names;
        }

        static const MetricBuilder<uint64_t> &get_metric(const std::string &metric_name)
        {
            if (metric_name == "instructions")
                return instructions();
            if (metric_name == "LLC-load-misses")
                return llc_load_misses();
            if (metric_name == "LLC-store-misses")
                return llc_store_misses();
            if (metric_name == "l3_mpki")
                return l3_mpki();
            if (metric_name == "all_metrics")
                return all_metrics();

            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        static const MetricBuilder<uint64_t> &instructions()
        {
            static const MetricBuilder<uint64_t> metric = [] {
                const std::string event_name = riscv::to_string(riscv::NativeEvents::INSTRUCTIONS);
                return MetricBuilder<uint64_t>{}
                    .add(event_name, riscv::EventMapper::get(riscv::NativeEvents::INSTRUCTIONS))
                    .build(event_name,
                           [event_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               return static_cast<double>(get_event_count(counts, event_name));
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &llc_load_misses()
        {
            static const MetricBuilder<uint64_t> metric = [] {
                const std::string event_name = riscv::to_string(riscv::NativeEvents::LLC_LOAD_MISSES);
                return MetricBuilder<uint64_t>{}
                    .add(event_name, riscv::EventMapper::get(riscv::NativeEvents::LLC_LOAD_MISSES))
                    .build(event_name,
                           [event_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               return static_cast<double>(get_event_count(counts, event_name));
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &llc_store_misses()
        {
            static const MetricBuilder<uint64_t> metric = [] {
                const std::string event_name = riscv::to_string(riscv::NativeEvents::LLC_STORE_MISSES);
                return MetricBuilder<uint64_t>{}
                    .add(event_name, riscv::EventMapper::get(riscv::NativeEvents::LLC_STORE_MISSES))
                    .build(event_name,
                           [event_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               return static_cast<double>(get_event_count(counts, event_name));
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &l3_mpki()
        {
            static const MetricBuilder<uint64_t> metric = [] {
                const std::string l3_misses_name = to_string(CoreEvents::L3_MISSES);
                const std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder<uint64_t>{}
                    .add(l3_misses_name, riscv::EventMapper::get(CoreEvents::L3_MISSES))
                    .add(inst_retired_name, riscv::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("l3_mpki",
                           [l3_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               const uint64_t l3_misses = get_event_count(counts, l3_misses_name);
                               const uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                               if (inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();

                               return 1000.0 * static_cast<double>(l3_misses) / static_cast<double>(inst_retired);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &all_metrics()
        {
            static const MetricBuilder<uint64_t> metric = [] {
                MetricBuilder<uint64_t> mb;
                mb.add(instructions());
                mb.add(llc_load_misses());
                mb.add(llc_store_misses());
                mb.add(l3_mpki());
                return mb;
            }();
            return metric;
        }
    };
}

#endif // OPTKIT_ENV_CPU_RISCV