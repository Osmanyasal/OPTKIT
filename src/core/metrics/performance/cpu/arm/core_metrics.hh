#pragma once
#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_ARM

#include "core/metrics/performance/cpu/core_metrics.hh"
#include "core/metrics/performance/cpu/arm/event_mapper.hh"
#include "core/metrics/performance/cpu/arm/native_events.hh"

#if OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_V1
#define SUPERSCALAR_WIDE 8
#elif OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_V2
#define SUPERSCALAR_WIDE 8
#elif OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_V3
#define SUPERSCALAR_WIDE 10
#elif OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N1
#define SUPERSCALAR_WIDE 5
#elif OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N2
#define SUPERSCALAR_WIDE 5
#elif OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N3
#define SUPERSCALAR_WIDE 5
#elif OPTKIT_ENV_CPU_MICROARCH_A64FX // set
#define SUPERSCALAR_WIDE 10
#else
#define SUPERSCALAR_WIDE 8
#endif

/**
 * @brief ARM CoreEvent implementation for Zen+ architecture.
 *
 * This implementation is based on performance events available on ARM Neoverse CPUs.
 * Event compatibility with other ARM architectures (e.g., ARMv7,ARMv8) is not guaranteed.
 *
 * Note: ARM's official Top-Down analysis is documented primarily for Neoverse processors.
 * https://developer.arm.com/documentation/109542/0100/Arm-Topdown-methodology/Stage-1--Topdown-analysis?lang=en
 *
 * Perf implementation:
 * https://github.com/torvalds/linux/blob/master/tools/perf/pmu-events/arch/arm64/arm/neoverse-v3/metrics.json
 */

// Warn: to use template initialisation for a certain type, they must be in the same namespace. so do NOT change it.
namespace optkit::metrics::performance
{
    /**
     * @class ARMMetricsImpl
     * @brief Interface for retrieving ARM CPU performance metrics. Each metric method returns the set of events required for its computation.
     *
     */
    class ARMMetricsImpl
    {
    };

    template <>
    class CoreMetrics<ARMMetricsImpl>
    {
    public:
        static const MetricBuilder<uint64_t> cpu_max_capacity_based_utilization()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string unhalted_core_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                return MetricBuilder<uint64_t>{}
                    .add(unhalted_core_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
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

        // Native Metric implementations (not included in CoreMetrics)
        static const MetricBuilder<uint64_t> &l2_hit_ratio()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string l2_cache_accesses_name = to_string(arm::NativeEvents::L2_CACHE_ACCESSES);
                std::string l2_misses_name = to_string(CoreEvents::L2_MISSES);
                return MetricBuilder<uint64_t>{}
                    .add(l2_cache_accesses_name, arm::EventMapper::get(arm::NativeEvents::L2_CACHE_ACCESSES))
                    .add(l2_misses_name, arm::EventMapper::get(CoreEvents::L2_MISSES))
                    .build("l2_hit_ratio__%",
                           [l2_misses_name, l2_cache_accesses_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t l2_misses = get_event_count(counts, l2_misses_name);
                               uint64_t l2_cache_accesses = get_event_count(counts, l2_cache_accesses_name);

                               // Avoid div by zero
                               if (l2_cache_accesses == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * (static_cast<double>(l2_cache_accesses - l2_misses) / static_cast<double>(l2_cache_accesses));
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &l3_hit_ratio()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string l3_cache_accesses_name = to_string(arm::NativeEvents::L3_CACHE_ACCESSES);
                std::string l3_misses_name = to_string(CoreEvents::L3_MISSES);
                return MetricBuilder<uint64_t>{}
                    .add(l3_cache_accesses_name, arm::EventMapper::get(arm::NativeEvents::L3_CACHE_ACCESSES))
                    .add(l3_misses_name, arm::EventMapper::get(CoreEvents::L3_MISSES))
                    .build("l3_hit_ratio__%",
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

        // Cache miss per kilo instruction (MPKI)
        static const MetricBuilder<uint64_t> &l1_mpki()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string l1_misses_name = to_string(CoreEvents::L1_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                return MetricBuilder<uint64_t>{}
                    .add(l1_misses_name, arm::EventMapper::get(CoreEvents::L1_MISSES))
                    .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
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
                    .add(l2_misses_name, arm::EventMapper::get(CoreEvents::L2_MISSES))
                    .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
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
                    .add(l3_misses_name, arm::EventMapper::get(CoreEvents::L3_MISSES))
                    .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
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
                    .add(branch_inst_retired_name, arm::EventMapper::get(CoreEvents::BRANCH_INST_RETIRED))
                    .add(branch_misp_retired_name, arm::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
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
                .add(itlb_misses_name, arm::EventMapper::get(CoreEvents::ITLB_MISSES))
                .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
                .build("itlb_mpki",
                       [itlb_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t itlb_misses = get_event_count(counts, itlb_misses_name);
                           uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                           if (inst_retired == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 1000.0 * static_cast<double>(itlb_misses) / static_cast<double>(inst_retired);
                       }); }();
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
                    .add(dtlb_misses_name, arm::EventMapper::get(CoreEvents::DTLB_MISSES))
                    .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
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
                    .add(itlb_misses_name, arm::EventMapper::get(CoreEvents::DTLB_MISSES))
                    .add(dtlb_misses_name, arm::EventMapper::get(CoreEvents::ITLB_MISSES))
                    .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
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
                    .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(unhalted_core_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
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
                    .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(branch_inst_retired_name, arm::EventMapper::get(CoreEvents::BRANCH_INST_RETIRED))
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
                    .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(mem_load_retired_name, arm::EventMapper::get(CoreEvents::MEM_LOAD_RETIRED))
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
                    .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(mem_store_retired_name, arm::EventMapper::get(CoreEvents::MEM_STORE_RETIRED))
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
                    .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(branch_misp_retired_name, arm::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
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
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        static const MetricBuilder<uint64_t> &ip_avx_any_flop()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string retired_sse_avx_flops_any_name = to_string(CoreEvents::RETIRED_VECTOR);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(retired_sse_avx_flops_any_name, arm::EventMapper::get(CoreEvents::RETIRED_VECTOR))
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
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        // Topdown (Pipeline Utilisation) Analysis L1

#if OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_V3 || OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N3
        static const MetricBuilder<uint64_t> &frontend_bound()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string stall_slot_fe_name = to_string(arm::NativeEvents::STALL_SLOT_FRONTEND);
                std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string stall_frontend_flush_name = to_string(arm::NativeEvents::STALL_FRONTEND_FLUSH);
                return MetricBuilder<uint64_t>{}
                    .add(stall_slot_fe_name, arm::EventMapper::get(arm::NativeEvents::STALL_SLOT_FRONTEND))
                    .add(unhalted_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(stall_frontend_flush_name, arm::EventMapper::get(arm::NativeEvents::STALL_FRONTEND_FLUSH))
                    .build("frontend_bound__%",
                           [stall_slot_fe_name, unhalted_cycles_name, stall_frontend_flush_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t cycles = get_event_count(counts, unhalted_cycles_name);
                               uint64_t slots = SUPERSCALAR_WIDE * cycles;
                               uint64_t stall_slot_fe = get_event_count(counts, stall_slot_fe_name);
                               uint64_t stall_frontend_flush = get_event_count(counts, stall_frontend_flush_name);

                               return 100 * ((static_cast<double>(stall_slot_fe) / (static_cast<double>(slots))) - (static_cast<double>(stall_frontend_flush) / static_cast<double>(cycles)));
                           });
            }();
            return metric;
        }

#elif OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N1 || OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_V1 || OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N2 || OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_V2
        static const MetricBuilder<uint64_t> &frontend_bound()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string stall_slot_fe_name = to_string(arm::NativeEvents::STALL_SLOT_FRONTEND);
                std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string branch_misp_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
                return MetricBuilder<uint64_t>{}
                    .add(stall_slot_fe_name, arm::EventMapper::get(arm::NativeEvents::STALL_SLOT_FRONTEND))
                    .add(unhalted_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(branch_misp_name, arm::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                    .build("frontend_bound__%",
                           [stall_slot_fe_name, unhalted_cycles_name, branch_misp_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t cycles = get_event_count(counts, unhalted_cycles_name);
                               uint64_t slots = SUPERSCALAR_WIDE * cycles;
                               uint64_t stall_slot_fe = get_event_count(counts, stall_slot_fe_name);
                               uint64_t branch_misp = get_event_count(counts, branch_misp_name);

                               return 100 * ((static_cast<double>(stall_slot_fe) / (static_cast<double>(slots))) - (static_cast<double>(branch_misp) / static_cast<double>(cycles)));
                           });
            }();
            return metric;
        }

#else
        static const MetricBuilder<uint64_t> &frontend_bound()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }
#endif

#if OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_V3 || OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N3
        static const MetricBuilder<uint64_t> &bad_speculation()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string stall_slots_name = to_string(arm::NativeEvents::STALL_SLOT);
                std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string retired_ops_name = to_string(arm::NativeEvents::RETIRED_OPS);
                std::string op_spec_name = to_string(arm::NativeEvents::OP_SPEC);
                std::string stall_flush_name = to_string(arm::NativeEvents::STALL_FRONTEND_FLUSH);

                return MetricBuilder<uint64_t>{}
                    .add(stall_slots_name, arm::EventMapper::get(arm::NativeEvents::STALL_SLOT))
                    .add(unhalted_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(retired_ops_name, arm::EventMapper::get(arm::NativeEvents::RETIRED_OPS))
                    .add(op_spec_name, arm::EventMapper::get(arm::NativeEvents::OP_SPEC))
                    .add(stall_flush_name, arm::EventMapper::get(arm::NativeEvents::STALL_FRONTEND_FLUSH))
                    .build("bad_speculation__%",
                           [stall_slots_name, unhalted_cycles_name, retired_ops_name, op_spec_name, stall_flush_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t cycles = get_event_count(counts, unhalted_cycles_name);
                               uint64_t stall_slots = get_event_count(counts, stall_slots_name);
                               uint64_t retired_ops = get_event_count(counts, retired_ops_name);
                               uint64_t op_spec = get_event_count(counts, op_spec_name);
                               uint64_t stall_flush = get_event_count(counts, stall_flush_name);

                               // Calculate components
                               double slot_utilization = 1.0 - (static_cast<double>(stall_slots) / (SUPERSCALAR_WIDE * cycles));
                               double spec_efficiency = 1.0 - (static_cast<double>(retired_ops) / op_spec);
                               double flush_penalty = static_cast<double>(stall_flush) / cycles;

                               return (slot_utilization * spec_efficiency * 100.0) + (flush_penalty * 100.0);
                           });
            }();
            return metric;
        }
#elif OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N2 || OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_V2
        static const MetricBuilder<uint64_t> &bad_speculation()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string stall_slots_name = to_string(arm::NativeEvents::STALL_SLOT);
                std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string retired_ops_name = to_string(arm::NativeEvents::OP_RETIRED);
                std::string op_spec_name = to_string(arm::NativeEvents::OP_SPEC);
                std::string br_mispred_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);

                return MetricBuilder<uint64_t>{}
                    .add(stall_slots_name, arm::EventMapper::get(arm::NativeEvents::STALL_SLOT))
                    .add(unhalted_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(retired_ops_name, arm::EventMapper::get(arm::NativeEvents::OP_RETIRED))
                    .add(op_spec_name, arm::EventMapper::get(arm::NativeEvents::OP_SPEC))
                    .add(br_mispred_name, arm::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                    .build("bad_speculation__%",
                           [stall_slots_name, unhalted_cycles_name, retired_ops_name, op_spec_name, br_mispred_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t cycles = get_event_count(counts, unhalted_cycles_name);
                               if (cycles == 0)
                                   return std::numeric_limits<double>::quiet_NaN();

                               uint64_t stall_slots = get_event_count(counts, stall_slots_name);
                               uint64_t retired_ops = get_event_count(counts, retired_ops_name);
                               uint64_t op_spec = get_event_count(counts, op_spec_name);
                               uint64_t br_mispred = get_event_count(counts, br_mispred_name);

                               uint64_t adjusted_stall_slots = stall_slots - cycles;

                               double slot_utilization = 1.0 - (static_cast<double>(adjusted_stall_slots) /
                                                                (cycles * SUPERSCALAR_WIDE));
                               double spec_efficiency = 1.0 - (static_cast<double>(retired_ops) / op_spec);
                               double mispred_penalty = (br_mispred * 4.0) / cycles;
                               return 100.0 * (spec_efficiency * slot_utilization) + mispred_penalty;
                           }); }();
            return metric;
        }
#elif OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_V1
        static const MetricBuilder<uint64_t> &bad_speculation()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string stall_slots_name = to_string(arm::NativeEvents::STALL_SLOT);
                std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string retired_ops_name = to_string(arm::NativeEvents::OP_RETIRED);
                std::string op_spec_name = to_string(arm::NativeEvents::OP_SPEC);

                return MetricBuilder<uint64_t>{}
                    .add(stall_slots_name, arm::EventMapper::get(arm::NativeEvents::STALL_SLOT))
                    .add(unhalted_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(retired_ops_name, arm::EventMapper::get(arm::NativeEvents::OP_RETIRED))
                    .add(op_spec_name, arm::EventMapper::get(arm::NativeEvents::OP_SPEC))
                    .build("bad_speculation__%",
                           [stall_slots_name, unhalted_cycles_name, retired_ops_name, op_spec_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t cycles = get_event_count(counts, unhalted_cycles_name);
                               if (cycles == 0)
                                   return std::numeric_limits<double>::quiet_NaN();

                               uint64_t stall_slots = get_event_count(counts, stall_slots_name);
                               uint64_t retired_ops = get_event_count(counts, retired_ops_name);
                               uint64_t op_spec = get_event_count(counts, op_spec_name);

                               // Calculate components
                               double slot_utilization = 1.0 - (static_cast<double>(stall_slots) /
                                                                (cycles * SUPERSCALAR_WIDE));
                               double spec_efficiency = 1.0 - (static_cast<double>(retired_ops) / op_spec);
                               return 100.0 * (spec_efficiency * slot_utilization);
                           });
            }();
            return metric;
        }
#else
        static const MetricBuilder<uint64_t> &bad_speculation()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }
#endif

#if !OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N1
        static const MetricBuilder<uint64_t> &retiring()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string stall_slots_name = to_string(arm::NativeEvents::STALL_SLOT);
                std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string retired_ops_name = to_string(arm::NativeEvents::OP_RETIRED);
                std::string op_spec_name = to_string(arm::NativeEvents::OP_SPEC);

                return MetricBuilder<uint64_t>{}
                    .add(stall_slots_name, arm::EventMapper::get(arm::NativeEvents::STALL_SLOT))
                    .add(unhalted_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(retired_ops_name, arm::EventMapper::get(arm::NativeEvents::OP_RETIRED))
                    .add(op_spec_name, arm::EventMapper::get(arm::NativeEvents::OP_SPEC))
                    .build("Retiring__%",
                           [stall_slots_name, unhalted_cycles_name, retired_ops_name, op_spec_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t cycles = get_event_count(counts, unhalted_cycles_name);
                               if (cycles == 0)
                                   return std::numeric_limits<double>::quiet_NaN();

                               uint64_t stall_slots = get_event_count(counts, stall_slots_name);
                               uint64_t retired_ops = get_event_count(counts, retired_ops_name);
                               uint64_t op_spec = get_event_count(counts, op_spec_name);

                               // Architecture-specific stall slot adjustment
                               double slot_utilization = 1.0 - (static_cast<double>((stall_slots - cycles)) /
                                                                (cycles * SUPERSCALAR_WIDE));
                               double retirement_ratio = static_cast<double>(retired_ops) / op_spec;
                               return 100.0 * (retirement_ratio * slot_utilization);
                           });
            }();
            return metric;
        }
#else
        static const MetricBuilder<uint64_t> &retiring()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }
#endif

#if !OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N1
        static const MetricBuilder<uint64_t> &backend_bound()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string stall_slot_backend_name = to_string(arm::NativeEvents::STALL_SLOT_BACKEND);
                std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);

                return MetricBuilder<uint64_t>{}
                    .add(stall_slot_backend_name, arm::EventMapper::get(arm::NativeEvents::STALL_SLOT_BACKEND))
                    .add(unhalted_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("backend_bound__%",
                           [stall_slot_backend_name, unhalted_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t cycles = get_event_count(counts, unhalted_cycles_name);
                               if (cycles == 0)
                                   return std::numeric_limits<double>::quiet_NaN();

                               uint64_t stall_slot_backend = get_event_count(counts, stall_slot_backend_name);
                               double total_slots = cycles * SUPERSCALAR_WIDE;

                               return 100.0 * (static_cast<double>(stall_slot_backend) / total_slots);
                           });
            }();
            return metric;
        }
#else
        static const MetricBuilder<uint64_t> &backend_bound()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }
#endif

        static const MetricBuilder<uint64_t> &smt_contention()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        // Topdown (Pipeline Utilisation) Analysis L1
        static const MetricBuilder<uint64_t> &frontend_bound_latency()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }
        static const MetricBuilder<uint64_t> &frontend_bound_bw()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }
        static const MetricBuilder<uint64_t> &bad_speculation_mispredicts()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }
        static const MetricBuilder<uint64_t> &bad_speculation_pipeline_restarts()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }
        static const MetricBuilder<uint64_t> &backend_bound_memory()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }
        static const MetricBuilder<uint64_t> &backend_bound_cpu()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }
        static const MetricBuilder<uint64_t> &retiring_fastpath()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }
        static const MetricBuilder<uint64_t> &retiring_microcode()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        // Aggregated Metrics

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

        static const MetricBuilder<uint64_t> &all_ip_metrics()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
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
#endif // OPTKIT_ENV_CPU_ARM