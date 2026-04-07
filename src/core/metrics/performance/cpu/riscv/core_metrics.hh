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
                "INST_RETIRED",
                "LLC-load-misses",
                "LLC-store-misses",
            };
            return names;
        }

        static const MetricBuilder<uint64_t> &get_metric(const std::string &metric_name)
        {
            if (metric_name == "INST_RETIRED")
                return instructions();
            if (metric_name == "LLC-load-misses")
                return llc_load_misses();
            if (metric_name == "LLC-store-misses")
                return llc_store_misses();
            if (metric_name == "all_metrics")
                return all_metrics();

            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        static const MetricBuilder<uint64_t> &instructions()
        {
            static const MetricBuilder<uint64_t> metric = [] {
                const std::string event_name = to_string(CoreEvents::INST_RETIRED);
                return MetricBuilder<uint64_t>{}
                    .add(event_name, riscv::EventMapper::get(CoreEvents::INST_RETIRED))
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

        static const MetricBuilder<uint64_t> &all_metrics()
        {
            static const MetricBuilder<uint64_t> metric = [] {
                MetricBuilder<uint64_t> mb;
                mb.add(instructions());
                mb.add(llc_load_misses());
                mb.add(llc_store_misses());
                return mb;
            }();
            return metric;
        }
    };
}

#endif // OPTKIT_ENV_CPU_RISCV