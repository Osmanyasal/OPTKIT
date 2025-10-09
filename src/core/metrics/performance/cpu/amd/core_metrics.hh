#pragma once
#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_AMD

#include "core/metrics/performance/cpu/core_metrics.hh"
#include "core/metrics/performance/cpu/amd/event_mapper.hh"
#include "core/metrics/performance/cpu/amd/native_events.hh"
#include "utils/metric_builder.hh"

#if OPTKIT_ENV_CPU_MICROARCH_ZEN
#define SUPERSCALAR_WIDE 5
#elif OPTKIT_ENV_CPU_MICROARCH_ZEN2
#define SUPERSCALAR_WIDE 5
#elif OPTKIT_ENV_CPU_MICROARCH_ZEN3
#define SUPERSCALAR_WIDE 5
#elif OPTKIT_ENV_CPU_MICROARCH_ZEN4
#define SUPERSCALAR_WIDE 6
#elif OPTKIT_ENV_CPU_MICROARCH_ZEN5
#define SUPERSCALAR_WIDE 8
#else
#define SUPERSCALAR_WIDE 8
#endif

/**
 * @brief AMD CoreEvent implementation for Zen+ architecture.
 *
 * This implementation is based on performance events available on AMD Zen+ CPUs.
 * Event compatibility with other AMD architectures (e.g., Zen3, Zen4, Zen5) is not guaranteed.
 *
 * Note: AMD's official Top-Down analysis is documented primarily for Zen4 and Zen5 processors.
 * For detailed information on supported Performance Monitor Counters (PMCs), refer to the
 * AMD Family 1Ah Model 00h–0Fh documentation:
 * https://www.amd.com/content/dam/amd/en/documents/epyc-technical-docs/programmer-references/58550-0.01.pdf
 *
 * Perf imlementation:
 *
 * below is zen4, likewise can change to other architectures.
 * https://github.com/torvalds/linux/blob/master/tools/perf/pmu-events/arch/x86/amdzen4/pipeline.json
 * https://github.com/torvalds/linux/blob/master/tools/perf/pmu-events/arch/x86/amdzen4/other.json
 * https://github.com/torvalds/linux/blob/master/tools/perf/pmu-events/arch/x86/amdzen4/core.json
 */

// Warn: to use template initialisation for a certain type, they must be in the same namespace. so do NOT change it.
namespace optkit::metrics::performance
{
    /**
     * @class AMDMetricsImpl
     * @brief Interface for retrieving AMD CPU performance metrics. Each metric method returns the set of events required for its computation.
     *
     */
    class AMDMetricsImpl
    {
    };

    template <>
    class CoreMetrics<AMDMetricsImpl>
    {
    public:
        // Native Metric implementations (not included in CoreMetrics)
        static const MetricBuilder<uint64_t> &l2_hit_ratio()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string l2_misses_name = to_string(CoreEvents::L2_MISSES);
                std::string l2_hits_name = to_string(CoreEvents::L2_HITS);
                return MetricBuilder<uint64_t>{}
                    .add(l2_misses_name, amd::EventMapper::get(CoreEvents::L2_MISSES))
                    .add(l2_hits_name, amd::EventMapper::get(CoreEvents::L2_HITS))
                    .build("l2_hit_ratio__%",
                           [l2_hits_name, l2_misses_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t l2_hits = get_event_count(counts, l2_hits_name);
                               uint64_t l2_misses = get_event_count(counts, l2_misses_name);
                               uint64_t total_l2 = l2_hits + l2_misses;
                               // Avoid div by zero
                               if (total_l2 == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * (static_cast<double>(l2_hits) / static_cast<double>(total_l2));
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &l3_hit_ratio()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string l3_cache_accesses_name = to_string(amd::NativeEvents::L3_CACHE_ACCESSES);
                std::string l3_misses_name = to_string(CoreEvents::L3_MISSES);
                return MetricBuilder<uint64_t>{}
                    .add(l3_cache_accesses_name, amd::EventMapper::get(amd::NativeEvents::L3_CACHE_ACCESSES))
                    .add(l3_misses_name, amd::EventMapper::get(CoreEvents::L3_MISSES))
                    .build("l3_hit_ratio_%",
                           [l3_misses_name, l3_cache_accesses_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t l3_misses = get_event_count(counts, l3_misses_name);
                               uint64_t l3_accesses = get_event_count(counts, l3_cache_accesses_name);

                               // Avoid div by zero
                               if (l3_accesses == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * (static_cast<double>(l3_accesses - l3_misses) / static_cast<double>(l3_accesses));
                           });
            }();
            return metric;
        }

    public:
        // CoreMetrics Implementation

        // CPU Utilization
        static const MetricBuilder<uint64_t> cpu_max_capacity_based_utilization()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string unhalted_core_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                return MetricBuilder<uint64_t>{}
                    .add(unhalted_core_cycles_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("cpu_max_capacity_based_utilization__%",
                           [unhalted_core_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               static const double max_cycles = OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS * frequency::QueryCPUFrequency::get_cpuinfo_max_freq() * 1000; // KHz to Hz
                               uint64_t unhalted_core_cycles = get_event_count(counts, unhalted_core_cycles_name);
                               double duration_sec = get_event_count(counts, "duration_microsec") / 1.0e6;

                               // Avoid div by zero
                               if (duration_sec == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100.0 * static_cast<double>(unhalted_core_cycles) / (max_cycles * duration_sec);
                           });
            }();
            return metric;
        } ///< 100 * (UNHALTED_CLK_CYCLES / (OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS * max_freq_khz * 1000  * duration_sec)))

        // Cache miss per kilo instruction (MPKI)
        static const MetricBuilder<uint64_t> &l1_mpki()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string l1_misses_name = to_string(CoreEvents::L1_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                return MetricBuilder<uint64_t>{}
                    .add(l1_misses_name, amd::EventMapper::get(CoreEvents::L1_MISSES))
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("l1_mpki",
                           [l1_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t l1_misses = get_event_count(counts, l1_misses_name);
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                               // Avoid div by zero
                               if (inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 1000.0 * static_cast<double>(l1_misses) / static_cast<double>(inst_retired);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &l2_mpki()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string l2_misses_name = to_string(CoreEvents::L2_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder<uint64_t>{}
                    .add(l2_misses_name, amd::EventMapper::get(CoreEvents::L2_MISSES))
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("l2_mpki",
                           [l2_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t l2_misses = get_event_count(counts, l2_misses_name);
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                               // Avoid div by zero
                               if (inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 1000.0 * static_cast<double>(l2_misses) / static_cast<double>(inst_retired);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &l3_mpki()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string l3_misses_name = to_string(CoreEvents::L3_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder<uint64_t>{}
                    .add(l3_misses_name, amd::EventMapper::get(CoreEvents::L3_MISSES))
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("l3_mpki",
                           [l3_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t l3_misses = get_event_count(counts, l3_misses_name);
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                               // Avoid div by zero
                               if (inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 1000.0 * static_cast<double>(l3_misses) / static_cast<double>(inst_retired);
                           });
            }();
            return metric;
        }

        // Branch
        static const MetricBuilder<uint64_t> &branch_mispr_ratio()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string branch_inst_retired_name = to_string(CoreEvents::BRANCH_INST_RETIRED);
                std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);

                return MetricBuilder<uint64_t>{}
                    .add(branch_inst_retired_name, amd::EventMapper::get(CoreEvents::BRANCH_INST_RETIRED))
                    .add(branch_misp_retired_name, amd::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                    .build("branch_mispr_ratio",
                           [branch_misp_retired_name, branch_inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t branch_misp = get_event_count(counts, branch_misp_retired_name);
                               uint64_t branch_inst = get_event_count(counts, branch_inst_retired_name);

                               // Avoid div by zero
                               if (branch_inst == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(branch_misp) / static_cast<double>(branch_inst);
                           });
            }();
            return metric;
        }

        // ITLB MPKI metrics
        static const MetricBuilder<uint64_t> &itlb_mpki()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string itlb_misses_name = to_string(CoreEvents::ITLB_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder<uint64_t>{}
                    .add(itlb_misses_name, amd::EventMapper::get(CoreEvents::ITLB_MISSES))
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("itlb_mpki",
                           [itlb_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t itlb_misses = get_event_count(counts, itlb_misses_name);
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                               if (inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 1000.0 * static_cast<double>(itlb_misses) / static_cast<double>(inst_retired);
                           });
            }();
            return metric;
        }

        // DTLB MPKI metrics
        static const MetricBuilder<uint64_t> &dtlb_mpki()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dtlb_misses_name = to_string(CoreEvents::DTLB_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder<uint64_t>{}
                    .add(dtlb_misses_name, amd::EventMapper::get(CoreEvents::DTLB_MISSES))
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("dtlb_mpki",
                           [dtlb_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t itlb_misses = get_event_count(counts, dtlb_misses_name);
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                               if (inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 1000.0 * static_cast<double>(itlb_misses) / static_cast<double>(inst_retired);
                           });
            }();
            return metric;
        }

        // TLB MPKI metrics
        static const MetricBuilder<uint64_t> &tlb_mpki()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dtlb_misses_name = to_string(CoreEvents::DTLB_MISSES);
                std::string itlb_misses_name = to_string(CoreEvents::ITLB_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder<uint64_t>{}
                    .add(itlb_misses_name, amd::EventMapper::get(CoreEvents::DTLB_MISSES))
                    .add(dtlb_misses_name, amd::EventMapper::get(CoreEvents::ITLB_MISSES))
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("tlb_mpki",
                           [itlb_misses_name, dtlb_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t itlb_misses = get_event_count(counts, itlb_misses_name);
                               uint64_t dtlb_misses = get_event_count(counts, dtlb_misses_name);
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                               if (inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 1000.0 * (static_cast<double>(dtlb_misses) + static_cast<double>(itlb_misses)) / static_cast<double>(inst_retired);
                           });
            }();
            return metric;
        }

        // Latency and parallelism metrics
        static const MetricBuilder<uint64_t> &load_miss_latency()
        {

            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        static const MetricBuilder<uint64_t> &ipc()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string unhalted_core_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);

                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(unhalted_core_cycles_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("ipc", [inst_retired_name, unhalted_core_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                           uint64_t inst_retired = get_event_count(counts,inst_retired_name);
                           uint64_t unhalted_core_cycles = get_event_count(counts,unhalted_core_cycles_name);

                           if (unhalted_core_cycles == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                            return static_cast<double>(inst_retired) / static_cast<double>(unhalted_core_cycles); });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ilp()
        {

            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        static const MetricBuilder<uint64_t> &mlp()
        {

            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        // DRAM bandwidth
        static const MetricBuilder<uint64_t> &dram_bandwidth_gbs()
        {

            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        // Instruction per event
        static const MetricBuilder<uint64_t> &ip_call()
        {

            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        static const MetricBuilder<uint64_t> &ip_branch()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string branch_inst_retired_name = to_string(CoreEvents::BRANCH_INST_RETIRED);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(branch_inst_retired_name, amd::EventMapper::get(CoreEvents::BRANCH_INST_RETIRED))
                    .build("ip_branch",
                           [branch_inst_retired_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t branch_inst_retired = get_event_count(counts, branch_inst_retired_name);

                               // Avoid div by zero
                               if (branch_inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(branch_inst_retired);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_mem_load()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string mem_load_retired_name = to_string(CoreEvents::MEM_LOAD_RETIRED);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(mem_load_retired_name, amd::EventMapper::get(CoreEvents::MEM_LOAD_RETIRED))
                    .build("IpLoad",
                           [mem_load_retired_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t mem_load_retired = get_event_count(counts, mem_load_retired_name);

                               // Avoid div by zero
                               if (mem_load_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(mem_load_retired);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_mem_store()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string mem_store_retired_name = to_string(CoreEvents::MEM_STORE_RETIRED);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(mem_store_retired_name, amd::EventMapper::get(CoreEvents::MEM_STORE_RETIRED))
                    .build("IpStore",
                           [mem_store_retired_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t mem_store_retired = get_event_count(counts, mem_store_retired_name);

                               // Avoid div by zero
                               if (mem_store_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(mem_store_retired);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_mispredict()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(branch_misp_retired_name, amd::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                    .build("ip_mispredict",
                           [branch_misp_retired_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t branch_misp_retired = get_event_count(counts, branch_misp_retired_name);

                               // Avoid div by zero
                               if (branch_misp_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(branch_misp_retired);
                           });
            }();
            return metric;
        }

        // Floating-point operation metrics

        static const MetricBuilder<uint64_t> &ip_flop()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string retired_flops_any_name = to_string(CoreEvents::RETIRED_FLOPS_ANY);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(retired_flops_any_name, amd::EventMapper::get(CoreEvents::RETIRED_FLOPS_ANY))
                    .build("ip_flop",
                           [retired_flops_any_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t retired_flops_any = get_event_count(counts, retired_flops_any_name);

                               // Avoid div by zero
                               if (retired_flops_any == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(retired_flops_any);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_avx_any_flop()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string retired_sse_avx_flops_any_name = to_string(CoreEvents::RETIRED_VECTOR);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(retired_sse_avx_flops_any_name, amd::EventMapper::get(CoreEvents::RETIRED_VECTOR))
                    .build("ip_avx_any_flop",
                           [retired_sse_avx_flops_any_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t retired_sse_avx_flops_any = get_event_count(counts, retired_sse_avx_flops_any_name);

                               // Avoid div by zero
                               if (retired_sse_avx_flops_any == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(retired_sse_avx_flops_any);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &gflops()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string retired_flops_any_name = to_string(CoreEvents::RETIRED_FLOPS_ANY);
                return MetricBuilder<uint64_t>{}
                    .add(retired_flops_any_name, amd::EventMapper::get(CoreEvents::RETIRED_FLOPS_ANY))
                    .build("gflops",
                           [retired_flops_any_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               double duration_sec = get_event_count(counts, "duration_microsec") / 1.0e6;
                               uint64_t retired_flops_any = get_event_count(counts, retired_flops_any_name);

                               // Avoid div by zero
                               if (duration_sec == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(retired_flops_any) / duration_sec / 1.0e9;
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ai()
        {
#if OPTKIT_ENV_CPU_MICROARCH_ZEN5
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string scalar_single_name = to_string(amd::NativeEvents::SCALAR_SINGLE_FLOPS);
                std::string packed_single_name = to_string(amd::NativeEvents::PACKED_SINGLE_FLOPS);
                std::string scalar_double_name = to_string(amd::NativeEvents::SCALAR_DOUBLE_FLOPS);
                std::string packed_double_name = to_string(amd::NativeEvents::PACKED_DOUBLE_FLOPS);
                std::string mem_inst_retired_name = to_string(CoreEvents::MEM_INST_RETIRED);
                return MetricBuilder<uint64_t>{}
                    .add(scalar_single_name, amd::EventMapper::get(amd::NativeEvents::SCALAR_SINGLE_FLOPS))
                    .add(packed_single_name, amd::EventMapper::get(amd::NativeEvents::PACKED_SINGLE_FLOPS))
                    .add(scalar_double_name, amd::EventMapper::get(amd::NativeEvents::SCALAR_DOUBLE_FLOPS))
                    .add(packed_double_name, amd::EventMapper::get(amd::NativeEvents::PACKED_DOUBLE_FLOPS))
                    .add(mem_inst_retired_name, amd::EventMapper::get(CoreEvents::MEM_INST_RETIRED))
                    .build("ai",
                           [packed_double_name, scalar_double_name, packed_single_name, scalar_single_name, mem_inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t scalar_single = get_event_count(counts, scalar_single_name);
                               uint64_t packed_single = get_event_count(counts, packed_single_name);
                               uint64_t scalar_double = get_event_count(counts, scalar_double_name);
                               uint64_t packed_double = get_event_count(counts, packed_double_name);
                               uint64_t mem_inst_retired = get_event_count(counts, mem_inst_retired_name);

                               double retired_flops_any = scalar_single + packed_single + scalar_double + packed_double;

                               // Avoid div by zero
                               if (retired_flops_any == 0)
                                   return std::numeric_limits<double>::quiet_NaN();

                               double single_ratio = (scalar_single + packed_single) / retired_flops_any;
                               double double_ratio = (scalar_double + packed_double) / retired_flops_any;

                               // Estimate bytes transferred: weighted average of 4 bytes (SP) and 8 bytes (DP)
                               double avg_bytes_per_mem_op = (4.0 * single_ratio + 8.0 * double_ratio);
                               double mem_bytes = mem_inst_retired * avg_bytes_per_mem_op;

                               // Avoid div by zero
                               if (mem_bytes == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(retired_flops_any) / mem_bytes;
                           });
            }();
            return metric;
#else
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string retired_flops_any_name = to_string(CoreEvents::RETIRED_FLOPS_ANY);
                std::string mem_inst_retired_name = to_string(CoreEvents::MEM_INST_RETIRED);
                return MetricBuilder<uint64_t>{}
                    .add(retired_flops_any_name, amd::EventMapper::get(CoreEvents::RETIRED_FLOPS_ANY))
                    .add(mem_inst_retired_name, amd::EventMapper::get(CoreEvents::MEM_INST_RETIRED))
                    .build("ai",
                           [retired_flops_any_name, mem_inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t retired_flops_any = get_event_count(counts, retired_flops_any_name);
                               uint64_t mem_inst_retired = get_event_count(counts, mem_inst_retired_name);
                               double mem_bytes = mem_inst_retired * 8.0; // Assuming 8 bytes per memory instruction
                               // Avoid div by zero
                               if (mem_bytes == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(retired_flops_any) / mem_bytes;
                           });
            }();
            return metric;
#endif
        }

        static const MetricBuilder<uint64_t> &ip_arith()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        static const MetricBuilder<uint64_t> &ip_arith_scalar_sp()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        static const MetricBuilder<uint64_t> &ip_arith_scalar_dp()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        static const MetricBuilder<uint64_t> &ip_arith_avx128()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        static const MetricBuilder<uint64_t> &ip_arith_avx256()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        static const MetricBuilder<uint64_t> &ip_arith_avx512()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        static const MetricBuilder<uint64_t> &ip_arith_vector_any()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        static const MetricBuilder<uint64_t> &scalarp_arith_vector()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        // Software prefetch
        static const MetricBuilder<uint64_t> &ip_swpf()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string sw_load_prefetch_name = to_string(CoreEvents::SW_LOAD_PREFETCH_ACCESS);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(sw_load_prefetch_name, amd::EventMapper::get(CoreEvents::SW_LOAD_PREFETCH_ACCESS))
                    .build("ip_swpf", [sw_load_prefetch_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t sw_load_prefetch = get_event_count(counts, sw_load_prefetch_name);

                               // Avoid div by zero
                               if (sw_load_prefetch == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(sw_load_prefetch); });
            }();
            return metric;
        }

        // Topdown (Pipeline Utilisation) Analysis L1
        static const MetricBuilder<uint64_t> &frontend_bound()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string no_ops_from_frontend_name = to_string(amd::NativeEvents::DISPATCH_STALLS_1);
                return MetricBuilder<uint64_t>{}
                    .add(no_ops_from_frontend_name, amd::EventMapper::get(performance::amd::NativeEvents::DISPATCH_STALLS_1))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("frontend_bound__%", [dispatch_slots_name, no_ops_from_frontend_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t no_ops_from_frontend = get_event_count(counts, no_ops_from_frontend_name);
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * static_cast<double>(no_ops_from_frontend) / (static_cast<double>(dispatch_slots)); });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &bad_speculation()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string retired_ops_name = to_string(amd::NativeEvents::RETIRED_OPS);
                std::string ops_source_dispatched_from_decoder_name = to_string(amd::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER);

                return MetricBuilder<uint64_t>{}
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(retired_ops_name, amd::EventMapper::get(performance::amd::NativeEvents::RETIRED_OPS))
                    .add(ops_source_dispatched_from_decoder_name, amd::EventMapper::get(performance::amd::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER))
                    .build("bad_speculation__%", [dispatch_slots_name, retired_ops_name, ops_source_dispatched_from_decoder_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t retired_ops = get_event_count(counts, retired_ops_name);
                               uint64_t ops_source_dispatched_from_decoder = get_event_count(counts, ops_source_dispatched_from_decoder_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * (static_cast<double>(ops_source_dispatched_from_decoder) - static_cast<double>(retired_ops)) / (static_cast<double>(dispatch_slots)); });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &backend_bound()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string backend_stalls_name = to_string(amd::NativeEvents::BACKEND_STALLS_1);
                return MetricBuilder<uint64_t>{}
                    .add(backend_stalls_name, amd::EventMapper::get(performance::amd::NativeEvents::BACKEND_STALLS_1))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("backend_bound__%", [dispatch_slots_name, backend_stalls_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t backend_stalls = get_event_count(counts, backend_stalls_name);
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * static_cast<double>(backend_stalls) / (static_cast<double>(dispatch_slots)); });
            }();
            return metric;
        }
        static const MetricBuilder<uint64_t> &retiring()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string retired_ops_name = to_string(amd::NativeEvents::RETIRED_OPS);

                return MetricBuilder<uint64_t>{}
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(retired_ops_name, amd::EventMapper::get(performance::amd::NativeEvents::RETIRED_OPS))
                    .build("Retiring__%", [dispatch_slots_name, retired_ops_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t retired_ops = get_event_count(counts, retired_ops_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * (static_cast<double>(retired_ops)) / (static_cast<double>(dispatch_slots)); });
            }();
            return metric;
        }
        static const MetricBuilder<uint64_t> &smt_contention()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string smt_stalls_name = to_string(amd::NativeEvents::SMT_STALLS_1);
                return MetricBuilder<uint64_t>{}
                    .add(smt_stalls_name, amd::EventMapper::get(performance::amd::NativeEvents::SMT_STALLS_1))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("smt_contention__%", [dispatch_slots_name, smt_stalls_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t smt_stalls = get_event_count(counts, smt_stalls_name);
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * static_cast<double>(smt_stalls) / (static_cast<double>(dispatch_slots)); });
            }();
            return metric;
        }

        // Topdown (Pipeline Utilisation) Analysis L1
        static const MetricBuilder<uint64_t> &frontend_bound_latency()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string no_ops_from_frontend_0x6flag_name = to_string(amd::NativeEvents::DISPATCH_STALLS_1_0x6);
                return MetricBuilder<uint64_t>{}
                    .add(no_ops_from_frontend_0x6flag_name, amd::EventMapper::get(performance::amd::NativeEvents::DISPATCH_STALLS_1_0x6))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("frontend_bound_latency__%", [dispatch_slots_name, no_ops_from_frontend_0x6flag_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t no_ops_from_frontend_0x6flag = SUPERSCALAR_WIDE * get_event_count(counts, no_ops_from_frontend_0x6flag_name); // this is latency specific. it is dispatch_stalls/dispatch_slots no multiply with cpu wide.
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * static_cast<double>(no_ops_from_frontend_0x6flag) / (static_cast<double>(dispatch_slots)); });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &frontend_bound_bw()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string no_ops_from_frontend_name = to_string(amd::NativeEvents::DISPATCH_STALLS_1);
                std::string no_ops_from_frontend_0x6flag_name = to_string(amd::NativeEvents::DISPATCH_STALLS_1_0x6);
                return MetricBuilder<uint64_t>{}
                    .add(no_ops_from_frontend_name, amd::EventMapper::get(performance::amd::NativeEvents::DISPATCH_STALLS_1))
                    .add(no_ops_from_frontend_0x6flag_name, amd::EventMapper::get(performance::amd::NativeEvents::DISPATCH_STALLS_1_0x6))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("frontend_bound_bw__%", [dispatch_slots_name, no_ops_from_frontend_name, no_ops_from_frontend_0x6flag_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t no_ops_from_frontend = get_event_count(counts, no_ops_from_frontend_name); // this is latency specific. it is backend_stalls/dispatch_slots no multiply with cpu wide.
                               uint64_t no_ops_from_frontend_0x6flag = SUPERSCALAR_WIDE * get_event_count(counts, no_ops_from_frontend_0x6flag_name);
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * (static_cast<double>(no_ops_from_frontend) - static_cast<double>(no_ops_from_frontend_0x6flag)) / (static_cast<double>(dispatch_slots)); });
            }();
            return metric;
        }
        static const MetricBuilder<uint64_t> &bad_speculation_mispredicts()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
                std::string resyncs_name = to_string(amd::NativeEvents::RESYNCS);
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string retired_ops_name = to_string(amd::NativeEvents::RETIRED_OPS);
                std::string ops_source_dispatched_from_decoder_name = to_string(amd::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER);

                return MetricBuilder<uint64_t>{}
                    .add(branch_misp_retired_name, amd::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(resyncs_name, amd::EventMapper::get(amd::NativeEvents::RESYNCS))
                    .add(retired_ops_name, amd::EventMapper::get(performance::amd::NativeEvents::RETIRED_OPS))
                    .add(ops_source_dispatched_from_decoder_name, amd::EventMapper::get(performance::amd::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER))
                    .build("bad_speculation_mispredicts__%", [branch_misp_retired_name, dispatch_slots_name, resyncs_name, retired_ops_name, ops_source_dispatched_from_decoder_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t branch_misp_retired = get_event_count(counts, branch_misp_retired_name);
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t resyncs = get_event_count(counts, resyncs_name);
                               uint64_t retired_ops = get_event_count(counts, retired_ops_name);
                               uint64_t ops_source_dispatched_from_decoder = get_event_count(counts, ops_source_dispatched_from_decoder_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0 || (branch_misp_retired + resyncs) == 0)
                                   return std::numeric_limits<double>::quiet_NaN();

                               double bad_speculation = (static_cast<double>(ops_source_dispatched_from_decoder) - static_cast<double>(retired_ops)) / (static_cast<double>(dispatch_slots));
                               return 100 * (bad_speculation * branch_misp_retired) / (branch_misp_retired + resyncs); });
            }();
            return metric;
        }
        static const MetricBuilder<uint64_t> &bad_speculation_pipeline_restarts()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string resyncs_name = to_string(amd::NativeEvents::RESYNCS);
                std::string retired_ops_name = to_string(amd::NativeEvents::RETIRED_OPS);
                std::string ops_source_dispatched_from_decoder_name = to_string(amd::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER);

                return MetricBuilder<uint64_t>{}
                    .add(branch_misp_retired_name, amd::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(resyncs_name, amd::EventMapper::get(amd::NativeEvents::RESYNCS))
                    .add(retired_ops_name, amd::EventMapper::get(performance::amd::NativeEvents::RETIRED_OPS))
                    .add(ops_source_dispatched_from_decoder_name, amd::EventMapper::get(performance::amd::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER))
                    .build("bad_speculation_pipeline_restarts__%", [branch_misp_retired_name, dispatch_slots_name, resyncs_name, retired_ops_name, ops_source_dispatched_from_decoder_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t branch_misp_retired = get_event_count(counts, branch_misp_retired_name);
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t resyncs = get_event_count(counts, resyncs_name);
                               uint64_t retired_ops = get_event_count(counts, retired_ops_name);
                               uint64_t ops_source_dispatched_from_decoder = get_event_count(counts, ops_source_dispatched_from_decoder_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0 || (branch_misp_retired + resyncs) == 0)
                                   return std::numeric_limits<double>::quiet_NaN();

                               double bad_speculation = (static_cast<double>(ops_source_dispatched_from_decoder) - static_cast<double>(retired_ops)) / (static_cast<double>(dispatch_slots));
                               return 100 * (bad_speculation * resyncs) / (branch_misp_retired + resyncs); });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &backend_bound_memory()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string backend_stalls_name = to_string(amd::NativeEvents::BACKEND_STALLS_1);
                std::string cycles_no_retire_not_complete_name = to_string(amd::NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE);
                std::string cycles_no_retire_load_not_complete_name = to_string(amd::NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE);

                return MetricBuilder<uint64_t>{}
                    .add(backend_stalls_name, amd::EventMapper::get(performance::amd::NativeEvents::BACKEND_STALLS_1))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(cycles_no_retire_not_complete_name, amd::EventMapper::get(performance::amd::NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE))
                    .add(cycles_no_retire_load_not_complete_name, amd::EventMapper::get(performance::amd::NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE))
                    .build("backend_bound_memory__%", [dispatch_slots_name, backend_stalls_name, cycles_no_retire_not_complete_name, cycles_no_retire_load_not_complete_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t backend_stalls = get_event_count(counts, backend_stalls_name);
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t cycles_no_retire_not_complete = get_event_count(counts, cycles_no_retire_not_complete_name);
                               uint64_t cycles_no_retire_load_not_complete = get_event_count(counts, cycles_no_retire_load_not_complete_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0 || cycles_no_retire_load_not_complete == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               double backend_bound = static_cast<double>(backend_stalls) / (static_cast<double>(dispatch_slots));
                               return 100 * backend_bound * (static_cast<double>(cycles_no_retire_not_complete) / static_cast<double>(cycles_no_retire_load_not_complete)); });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &backend_bound_cpu()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string backend_stalls_name = to_string(amd::NativeEvents::BACKEND_STALLS_1);
                std::string cycles_no_retire_not_complete_name = to_string(amd::NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE);
                std::string cycles_no_retire_load_not_complete_name = to_string(amd::NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE);

                return MetricBuilder<uint64_t>{}
                    .add(backend_stalls_name, amd::EventMapper::get(performance::amd::NativeEvents::BACKEND_STALLS_1))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(cycles_no_retire_not_complete_name, amd::EventMapper::get(performance::amd::NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE))
                    .add(cycles_no_retire_load_not_complete_name, amd::EventMapper::get(performance::amd::NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE))
                    .build("backend_bound_cpu__%", [dispatch_slots_name, backend_stalls_name, cycles_no_retire_not_complete_name, cycles_no_retire_load_not_complete_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t backend_stalls = get_event_count(counts, backend_stalls_name);
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t cycles_no_retire_not_complete = get_event_count(counts, cycles_no_retire_not_complete_name);
                               uint64_t cycles_no_retire_load_not_complete = get_event_count(counts, cycles_no_retire_load_not_complete_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0 || cycles_no_retire_load_not_complete == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               double backend_bound = static_cast<double>(backend_stalls) / (static_cast<double>(dispatch_slots));
                               return 100 * backend_bound * (1.0 - (static_cast<double>(cycles_no_retire_not_complete) / static_cast<double>(cycles_no_retire_load_not_complete))); });
            }();
            return metric;
        }
        static const MetricBuilder<uint64_t> &retiring_fastpath()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string retired_microcode_ops_name = to_string(amd::NativeEvents::RETIRED_MICROCODE_OPS);
                std::string retired_ops_name = to_string(amd::NativeEvents::RETIRED_OPS);

                return MetricBuilder<uint64_t>{}
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(retired_microcode_ops_name, amd::EventMapper::get(amd::NativeEvents::RETIRED_MICROCODE_OPS))
                    .add(retired_ops_name, amd::EventMapper::get(performance::amd::NativeEvents::RETIRED_OPS))
                    .build("retiring_fastpath__%", [dispatch_slots_name, retired_microcode_ops_name, retired_ops_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t retired_ops = get_event_count(counts, retired_ops_name);
                               uint64_t retired_microcode_ops = get_event_count(counts, retired_microcode_ops_name);

                               // Avoid div by zero
                               if (retired_ops == 0 || dispatch_slots)
                                   return std::numeric_limits<double>::quiet_NaN();
                               double retiring = (static_cast<double>(retired_ops)) / (static_cast<double>(dispatch_slots));

                               return 100 * retiring * (retired_ops - retired_microcode_ops) / retired_ops; });
            }();
            return metric;
        }
        static const MetricBuilder<uint64_t> &retiring_microcode()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string retired_microcode_ops_name = to_string(amd::NativeEvents::RETIRED_MICROCODE_OPS);
                std::string retired_ops_name = to_string(amd::NativeEvents::RETIRED_OPS);

                return MetricBuilder<uint64_t>{}
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(retired_microcode_ops_name, amd::EventMapper::get(amd::NativeEvents::RETIRED_MICROCODE_OPS))
                    .add(retired_ops_name, amd::EventMapper::get(performance::amd::NativeEvents::RETIRED_OPS))
                    .build("retiring_microcode__%", [dispatch_slots_name, retired_microcode_ops_name, retired_ops_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t retired_ops = get_event_count(counts, retired_ops_name);
                               uint64_t retired_microcode_ops = get_event_count(counts, retired_microcode_ops_name);

                               // Avoid div by zero
                               if (retired_ops == 0 || dispatch_slots)
                                   return std::numeric_limits<double>::quiet_NaN();
                               double retiring = (static_cast<double>(retired_ops)) / (static_cast<double>(dispatch_slots));

                               return 100 * retiring * retired_microcode_ops / retired_ops; });
            }();
            return metric;
        }

        // Aggregated Metrics

        // Roofline model metrics
        static const MetricBuilder<uint64_t> &carm()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(ai());
                mb.add(gflops());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &topdown_l1()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(frontend_bound());
                mb.add(backend_bound());
                mb.add(retiring());
                mb.add(bad_speculation());
                mb.add(smt_contention());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &topdown_l2()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(topdown_l2_fe());
                mb.add(topdown_l2_be());
                mb.add(topdown_l2_retiring());
                mb.add(topdown_l2_bad_spec());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &topdown_l2_fe()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(frontend_bound_latency());
                mb.add(frontend_bound_bw());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &topdown_l2_be()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(backend_bound_memory());
                mb.add(backend_bound_cpu());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &topdown_l2_retiring()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(retiring_fastpath());
                mb.add(retiring_microcode());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &topdown_l2_bad_spec()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(bad_speculation_mispredicts());
                mb.add(bad_speculation_pipeline_restarts());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &all_topdown()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(topdown_l1());
                mb.add(topdown_l2());
                return mb;
            }();
            return mb;
        }

        // Aggregate all cache miss metrics
        static const MetricBuilder<uint64_t> &all_mpki()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(l1_mpki());
                mb.add(l2_mpki());
                mb.add(l3_mpki());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &all_cache_hit_ratio()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(l2_hit_ratio());
                mb.add(l3_hit_ratio());
                return mb;
            }();
            return mb;
        }

        // Aggregate all STLB MPKI metrics
        static const MetricBuilder<uint64_t> &AllStlb_mpki()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(tlb_mpki());
                mb.add(itlb_mpki());
                mb.add(dtlb_mpki());
                return mb;
            }();
            return mb;
        }

        // Aggregate all latency and parallelism metrics
        static const MetricBuilder<uint64_t> &all_latency_and_parallelism()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(load_miss_latency());
                mb.add(ilp());
                mb.add(mlp());
                return mb;
            }();
            return mb;
        }

        // Aggregate all DRAM bandwidth metrics
        static const MetricBuilder<uint64_t> &all_dram_bandwidth()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(dram_bandwidth_gbs());
                return mb;
            }();
            return mb;
        }

        // Aggregate all instruction-per-event metrics
        static const MetricBuilder<uint64_t> &all_ip_metrics()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(ipc());
                mb.add(ip_call());
                mb.add(ip_branch());
                mb.add(ip_mem_load());
                mb.add(ip_mem_store());
                mb.add(ip_mispredict());
                mb.add(ip_flop());
                mb.add(ip_arith());
                mb.add(ip_arith_scalar_sp());
                mb.add(ip_arith_scalar_dp());
                mb.add(ip_swpf());
                return mb;
            }();
            return mb;
        }

        // Aggregate all branch-related metrics
        static const MetricBuilder<uint64_t> &all_branch_metrics()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(branch_mispr_ratio());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &all_metrics()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(all_mpki());
                mb.add(AllStlb_mpki());
                mb.add(all_latency_and_parallelism());
                mb.add(all_dram_bandwidth());
                mb.add(all_ip_metrics());
                mb.add(all_branch_metrics());
                mb.add(all_topdown());
                return mb;
            }();
            return mb;
        }
    };
}
#endif // close OPTKIT_ENV_CPU_AMD