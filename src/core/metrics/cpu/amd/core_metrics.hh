#pragma once
#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_AMD

#include "core/metrics/cpu/core_metrics.hh"
#include "core/metrics/cpu/amd/event_mapper.hh"
#include "core/metrics/cpu/amd/native_events.hh"

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
namespace optkit::core::metrics::cpu
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
        static const MetricBuilder &L2HitRatio()
        {
            static const MetricBuilder metric = []
            {
                std::string l2_misses_name = to_string(CoreEvents::L2_MISSES);
                std::string l2_hits_name = to_string(CoreEvents::L2_HITS);
                return MetricBuilder{}
                    .add(l2_misses_name, amd::EventMapper::get(CoreEvents::L2_MISSES))
                    .add(l2_hits_name, amd::EventMapper::get(CoreEvents::L2_HITS))
                    .build("L2HitRatio__%",
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

        static const MetricBuilder &L3HitRatio()
        {
            static const MetricBuilder metric = []
            {
                std::string l3_cache_accesses_name = to_string(amd::NativeEvents::L3_CACHE_ACCESSES);
                std::string l3_misses_name = to_string(CoreEvents::L3_MISSES);
                return MetricBuilder{}
                    .add(l3_cache_accesses_name, amd::EventMapper::get(amd::NativeEvents::L3_CACHE_ACCESSES))
                    .add(l3_misses_name, amd::EventMapper::get(CoreEvents::L3_MISSES))
                    .build("L3HitRatio__%",
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
        static const MetricBuilder &L1MPKI()
        {
            static const MetricBuilder metric = []
            {
                std::string l1_misses_name = to_string(CoreEvents::L1_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                return MetricBuilder{}
                    .add(l1_misses_name, amd::EventMapper::get(CoreEvents::L1_MISSES))
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("L1MPKI",
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

        static const MetricBuilder &L2MPKI()
        {
            static const MetricBuilder metric = []
            {
                std::string l2_misses_name = to_string(CoreEvents::L2_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder{}
                    .add(l2_misses_name, amd::EventMapper::get(CoreEvents::L2_MISSES))
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("L2MPKI",
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

        static const MetricBuilder &L3MPKI()
        {
            static const MetricBuilder metric = []
            {
                std::string l3_misses_name = to_string(CoreEvents::L3_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder{}
                    .add(l3_misses_name, amd::EventMapper::get(CoreEvents::L3_MISSES))
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("L3MPKI",
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
        static const MetricBuilder &BranchMisprRatio()
        {
            static const MetricBuilder metric = []
            {
                std::string branch_inst_retired_name = to_string(CoreEvents::BRANCH_INST_RETIRED);
                std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);

                return MetricBuilder{}
                    .add(branch_inst_retired_name, amd::EventMapper::get(CoreEvents::BRANCH_INST_RETIRED))
                    .add(branch_misp_retired_name, amd::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                    .build("BranchMisprRatio",
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
        static const MetricBuilder &ITLBMPKI()
        {
            static const MetricBuilder metric = []
            {
                std::string itlb_misses_name = to_string(CoreEvents::ITLB_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder{}
                    .add(itlb_misses_name, amd::EventMapper::get(CoreEvents::ITLB_MISSES))
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("ITLBMPKI",
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
        static const MetricBuilder &DTLBMPKI()
        {
            static const MetricBuilder metric = []
            {
                std::string dtlb_misses_name = to_string(CoreEvents::DTLB_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder{}
                    .add(dtlb_misses_name, amd::EventMapper::get(CoreEvents::DTLB_MISSES))
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("DTLBMPKI",
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
        static const MetricBuilder &TLBMPKI()
        {
            static const MetricBuilder metric = []
            {
                std::string dtlb_misses_name = to_string(CoreEvents::DTLB_MISSES);
                std::string itlb_misses_name = to_string(CoreEvents::ITLB_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder{}
                    .add(itlb_misses_name, amd::EventMapper::get(CoreEvents::DTLB_MISSES))
                    .add(dtlb_misses_name, amd::EventMapper::get(CoreEvents::ITLB_MISSES))
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("TLBMPKI",
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
        static const MetricBuilder &LoadMissLatency()
        {

            static const MetricBuilder empty{};
            return empty;
        }

        static const MetricBuilder &IpC()
        {
            static const MetricBuilder metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string unhalted_core_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);

                return MetricBuilder{}
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(unhalted_core_cycles_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("IpC", [inst_retired_name, unhalted_core_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                           uint64_t inst_retired = get_event_count(counts,inst_retired_name);
                           uint64_t unhalted_core_cycles = get_event_count(counts,unhalted_core_cycles_name);

                           if (unhalted_core_cycles == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                            return static_cast<double>(inst_retired) / static_cast<double>(unhalted_core_cycles); });
            }();
            return metric;
        }

        static const MetricBuilder &ILP()
        {

            static const MetricBuilder empty{};
            return empty;
        }

        static const MetricBuilder &MLP()
        {

            static const MetricBuilder empty{};
            return empty;
        }

        // DRAM bandwidth
        static const MetricBuilder &DRAMBandwidthGBs()
        {

            static const MetricBuilder empty{};
            return empty;
        }

        // Instruction per event
        static const MetricBuilder &IpCall()
        {

            static const MetricBuilder empty{};
            return empty;
        }

        static const MetricBuilder &IpBranch()
        {
            static const MetricBuilder metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string branch_inst_retired_name = to_string(CoreEvents::BRANCH_INST_RETIRED);
                return MetricBuilder{}
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(branch_inst_retired_name, amd::EventMapper::get(CoreEvents::BRANCH_INST_RETIRED))
                    .build("IpBranch",
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

        static const MetricBuilder &IpMemLoad()
        {
            static const MetricBuilder metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string mem_load_retired_name = to_string(CoreEvents::MEM_LOAD_RETIRED);
                return MetricBuilder{}
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

        static const MetricBuilder &IpMemStore()
        {
            static const MetricBuilder metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string mem_store_retired_name = to_string(CoreEvents::MEM_STORE_RETIRED);
                return MetricBuilder{}
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

        static const MetricBuilder &IpMispredict()
        {
            static const MetricBuilder metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
                return MetricBuilder{}
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(branch_misp_retired_name, amd::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                    .build("IpMispredict",
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

        static const MetricBuilder &IpFLOP()
        {
            static const MetricBuilder metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string retired_flops_any_name = to_string(CoreEvents::RETIRED_FLOPS_ANY);
                return MetricBuilder{}
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(retired_flops_any_name, amd::EventMapper::get(CoreEvents::RETIRED_FLOPS_ANY))
                    .build("IpFLOP",
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

        static const MetricBuilder &IpAVXAnyFlop()
        {
            static const MetricBuilder metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string retired_sse_avx_flops_any_name = to_string(CoreEvents::RETIRED_VECTOR);
                return MetricBuilder{}
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(retired_sse_avx_flops_any_name, amd::EventMapper::get(CoreEvents::RETIRED_VECTOR))
                    .build("IpAVXAnyFlop",
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

        static const MetricBuilder &GFLOPs()
        {
            static const MetricBuilder metric = []
            {
                std::string retired_flops_any_name = to_string(CoreEvents::RETIRED_FLOPS_ANY);
                return MetricBuilder{}
                    .add(retired_flops_any_name, amd::EventMapper::get(CoreEvents::RETIRED_FLOPS_ANY))
                    .build("GFLOPs",
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

        static const MetricBuilder &AI()
        {
#if OPTKIT_ENV_CPU_MICROARCH_ZEN5
            static const MetricBuilder metric = []
            {
                std::string scalar_single_name = to_string(amd::NativeEvents::SCALAR_SINGLE_FLOPS);
                std::string packed_single_name = to_string(amd::NativeEvents::PACKED_SINGLE_FLOPS);
                std::string scalar_double_name = to_string(amd::NativeEvents::SCALAR_DOUBLE_FLOPS);
                std::string packed_double_name = to_string(amd::NativeEvents::PACKED_DOUBLE_FLOPS);
                std::string mem_inst_retired_name = to_string(CoreEvents::MEM_INST_RETIRED);
                return MetricBuilder{}
                    .add(scalar_single_name, amd::EventMapper::get(amd::NativeEvents::SCALAR_SINGLE_FLOPS))
                    .add(packed_single_name, amd::EventMapper::get(amd::NativeEvents::PACKED_SINGLE_FLOPS))
                    .add(scalar_double_name, amd::EventMapper::get(amd::NativeEvents::SCALAR_DOUBLE_FLOPS))
                    .add(packed_double_name, amd::EventMapper::get(amd::NativeEvents::PACKED_DOUBLE_FLOPS))
                    .add(mem_inst_retired_name, amd::EventMapper::get(CoreEvents::MEM_INST_RETIRED))
                    .build("AI",
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
            static const MetricBuilder metric = []
            {
                std::string retired_flops_any_name = to_string(CoreEvents::RETIRED_FLOPS_ANY);
                std::string mem_inst_retired_name = to_string(CoreEvents::MEM_INST_RETIRED);
                return MetricBuilder{}
                    .add(retired_flops_any_name, amd::EventMapper::get(CoreEvents::RETIRED_FLOPS_ANY))
                    .add(mem_inst_retired_name, amd::EventMapper::get(CoreEvents::MEM_INST_RETIRED))
                    .build("AI",
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

        static const MetricBuilder &IpArith()
        {
            static const MetricBuilder empty{};
            return empty;
        }

        static const MetricBuilder &IpArithScalarSP()
        {
            static const MetricBuilder empty{};
            return empty;
        }

        static const MetricBuilder &IpArithScalarDP()
        {
            static const MetricBuilder empty{};
            return empty;
        }

        static const MetricBuilder &IpArithAVX128()
        {
            static const MetricBuilder empty{};
            return empty;
        }

        static const MetricBuilder &IpArithAVX256()
        {
            static const MetricBuilder empty{};
            return empty;
        }

        static const MetricBuilder &IpArithAVX512()
        {
            static const MetricBuilder empty{};
            return empty;
        }

        static const MetricBuilder &IpArithVectorAny()
        {
            static const MetricBuilder empty{};
            return empty;
        }

        static const MetricBuilder &ScalarpArithVector()
        {
            static const MetricBuilder empty{};
            return empty;
        }

        // Software prefetch
        static const MetricBuilder &IpSWPF()
        {
            static const MetricBuilder metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string sw_load_prefetch_name = to_string(CoreEvents::SW_LOAD_PREFETCH_ACCESS);
                return MetricBuilder{}
                    .add(inst_retired_name, amd::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(sw_load_prefetch_name, amd::EventMapper::get(CoreEvents::SW_LOAD_PREFETCH_ACCESS))
                    .build("IpSWPF", [sw_load_prefetch_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
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
        static const MetricBuilder &FrontendBound()
        {
            static const MetricBuilder metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string no_ops_from_frontend_name = to_string(amd::NativeEvents::DISPATCH_STALLS_1);
                return MetricBuilder{}
                    .add(no_ops_from_frontend_name, amd::EventMapper::get(cpu::amd::NativeEvents::DISPATCH_STALLS_1))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("FrontendBound__%", [dispatch_slots_name, no_ops_from_frontend_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
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

        static const MetricBuilder &BadSpeculation()
        {
            static const MetricBuilder metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string retired_ops_name = to_string(amd::NativeEvents::RETIRED_OPS);
                std::string ops_source_dispatched_from_decoder_name = to_string(amd::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER);

                return MetricBuilder{}
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(retired_ops_name, amd::EventMapper::get(cpu::amd::NativeEvents::RETIRED_OPS))
                    .add(ops_source_dispatched_from_decoder_name, amd::EventMapper::get(cpu::amd::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER))
                    .build("BadSpeculation__%", [dispatch_slots_name, retired_ops_name, ops_source_dispatched_from_decoder_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
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

        static const MetricBuilder &BackendBound()
        {
            static const MetricBuilder metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string backend_stalls_name = to_string(amd::NativeEvents::BACKEND_STALLS_1);
                return MetricBuilder{}
                    .add(backend_stalls_name, amd::EventMapper::get(cpu::amd::NativeEvents::BACKEND_STALLS_1))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("BackendBound__%", [dispatch_slots_name, backend_stalls_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
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
        static const MetricBuilder &Retiring()
        {
            static const MetricBuilder metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string retired_ops_name = to_string(amd::NativeEvents::RETIRED_OPS);

                return MetricBuilder{}
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(retired_ops_name, amd::EventMapper::get(cpu::amd::NativeEvents::RETIRED_OPS))
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
        static const MetricBuilder &SMTContention()
        {

            static const MetricBuilder metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string smt_stalls_name = to_string(amd::NativeEvents::SMT_STALLS_1);
                return MetricBuilder{}
                    .add(smt_stalls_name, amd::EventMapper::get(cpu::amd::NativeEvents::SMT_STALLS_1))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("SMTContention__%", [dispatch_slots_name, smt_stalls_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
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
        static const MetricBuilder &FrontendBound_Latency()
        {

            static const MetricBuilder metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string no_ops_from_frontend_0x6flag_name = to_string(amd::NativeEvents::DISPATCH_STALLS_1_0x6);
                return MetricBuilder{}
                    .add(no_ops_from_frontend_0x6flag_name, amd::EventMapper::get(cpu::amd::NativeEvents::DISPATCH_STALLS_1_0x6))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("FrontendBound_Latency__%", [dispatch_slots_name, no_ops_from_frontend_0x6flag_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
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

        static const MetricBuilder &FrontendBound_BW()
        {

            static const MetricBuilder metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string no_ops_from_frontend_name = to_string(amd::NativeEvents::DISPATCH_STALLS_1);
                std::string no_ops_from_frontend_0x6flag_name = to_string(amd::NativeEvents::DISPATCH_STALLS_1_0x6);
                return MetricBuilder{}
                    .add(no_ops_from_frontend_name, amd::EventMapper::get(cpu::amd::NativeEvents::DISPATCH_STALLS_1))
                    .add(no_ops_from_frontend_0x6flag_name, amd::EventMapper::get(cpu::amd::NativeEvents::DISPATCH_STALLS_1_0x6))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("FrontendBound_BW__%", [dispatch_slots_name, no_ops_from_frontend_name, no_ops_from_frontend_0x6flag_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
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
        static const MetricBuilder &BadSpeculation_Mispredicts()
        {

            static const MetricBuilder metric = []
            {
                std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
                std::string resyncs_name = to_string(amd::NativeEvents::RESYNCS);
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string retired_ops_name = to_string(amd::NativeEvents::RETIRED_OPS);
                std::string ops_source_dispatched_from_decoder_name = to_string(amd::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER);

                return MetricBuilder{}
                    .add(branch_misp_retired_name, amd::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(resyncs_name, amd::EventMapper::get(amd::NativeEvents::RESYNCS))
                    .add(retired_ops_name, amd::EventMapper::get(cpu::amd::NativeEvents::RETIRED_OPS))
                    .add(ops_source_dispatched_from_decoder_name, amd::EventMapper::get(cpu::amd::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER))
                    .build("BadSpeculation_Mispredicts__%", [branch_misp_retired_name, dispatch_slots_name, resyncs_name, retired_ops_name, ops_source_dispatched_from_decoder_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
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
        static const MetricBuilder &BadSpeculation_PipelineRestarts()
        {

            static const MetricBuilder metric = []
            {
                std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string resyncs_name = to_string(amd::NativeEvents::RESYNCS);
                std::string retired_ops_name = to_string(amd::NativeEvents::RETIRED_OPS);
                std::string ops_source_dispatched_from_decoder_name = to_string(amd::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER);

                return MetricBuilder{}
                    .add(branch_misp_retired_name, amd::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(resyncs_name, amd::EventMapper::get(amd::NativeEvents::RESYNCS))
                    .add(retired_ops_name, amd::EventMapper::get(cpu::amd::NativeEvents::RETIRED_OPS))
                    .add(ops_source_dispatched_from_decoder_name, amd::EventMapper::get(cpu::amd::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER))
                    .build("BadSpeculation_PipelineRestarts__%", [branch_misp_retired_name, dispatch_slots_name, resyncs_name, retired_ops_name, ops_source_dispatched_from_decoder_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
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

        static const MetricBuilder &BackendEndbound_Memory()
        {
            static const MetricBuilder metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string backend_stalls_name = to_string(amd::NativeEvents::BACKEND_STALLS_1);
                std::string cycles_no_retire_not_complete_name = to_string(amd::NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE);
                std::string cycles_no_retire_load_not_complete_name = to_string(amd::NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE);

                return MetricBuilder{}
                    .add(backend_stalls_name, amd::EventMapper::get(cpu::amd::NativeEvents::BACKEND_STALLS_1))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(cycles_no_retire_not_complete_name, amd::EventMapper::get(cpu::amd::NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE))
                    .add(cycles_no_retire_load_not_complete_name, amd::EventMapper::get(cpu::amd::NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE))
                    .build("BackendEndbound_Memory__%", [dispatch_slots_name, backend_stalls_name, cycles_no_retire_not_complete_name, cycles_no_retire_load_not_complete_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
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

        static const MetricBuilder &BackendEndbound_CPU()
        {
            static const MetricBuilder metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string backend_stalls_name = to_string(amd::NativeEvents::BACKEND_STALLS_1);
                std::string cycles_no_retire_not_complete_name = to_string(amd::NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE);
                std::string cycles_no_retire_load_not_complete_name = to_string(amd::NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE);

                return MetricBuilder{}
                    .add(backend_stalls_name, amd::EventMapper::get(cpu::amd::NativeEvents::BACKEND_STALLS_1))
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(cycles_no_retire_not_complete_name, amd::EventMapper::get(cpu::amd::NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE))
                    .add(cycles_no_retire_load_not_complete_name, amd::EventMapper::get(cpu::amd::NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE))
                    .build("BackendEndbound_CPU__%", [dispatch_slots_name, backend_stalls_name, cycles_no_retire_not_complete_name, cycles_no_retire_load_not_complete_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
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
        static const MetricBuilder &Retiring_Fastpath()
        {
            static const MetricBuilder metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string retired_microcode_ops_name = to_string(amd::NativeEvents::RETIRED_MICROCODE_OPS);
                std::string retired_ops_name = to_string(amd::NativeEvents::RETIRED_OPS);

                return MetricBuilder{}
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(retired_microcode_ops_name, amd::EventMapper::get(amd::NativeEvents::RETIRED_MICROCODE_OPS))
                    .add(retired_ops_name, amd::EventMapper::get(cpu::amd::NativeEvents::RETIRED_OPS))
                    .build("Retiring_Fastpath__%", [dispatch_slots_name, retired_microcode_ops_name, retired_ops_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
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
        static const MetricBuilder &Retiring_Microcode()
        {
            static const MetricBuilder metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::DISPATCH_SLOTS);
                std::string retired_microcode_ops_name = to_string(amd::NativeEvents::RETIRED_MICROCODE_OPS);
                std::string retired_ops_name = to_string(amd::NativeEvents::RETIRED_OPS);

                return MetricBuilder{}
                    .add(dispatch_slots_name, amd::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(retired_microcode_ops_name, amd::EventMapper::get(amd::NativeEvents::RETIRED_MICROCODE_OPS))
                    .add(retired_ops_name, amd::EventMapper::get(cpu::amd::NativeEvents::RETIRED_OPS))
                    .build("Retiring_Microcode__%", [dispatch_slots_name, retired_microcode_ops_name, retired_ops_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
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

        static const MetricBuilder &TopdownL1()
        {
            static const MetricBuilder mb = []
            {
                MetricBuilder mb{};
                mb.add(FrontendBound());
                mb.add(BackendBound());
                mb.add(Retiring());
                mb.add(BadSpeculation());
                mb.add(SMTContention());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder &TopdownL2()
        {
            static const MetricBuilder mb = []
            {
                MetricBuilder mb{};
                mb.add(TopdownL2_FE());
                mb.add(TopdownL2_BE());
                mb.add(TopdownL2_Retiring());
                mb.add(TopdownL2_BadSpec());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder &TopdownL2_FE()
        {
            static const MetricBuilder mb = []
            {
                MetricBuilder mb{};
                mb.add(FrontendBound_Latency());
                mb.add(FrontendBound_BW());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder &TopdownL2_BE()
        {
            static const MetricBuilder mb = []
            {
                MetricBuilder mb{};
                mb.add(BackendEndbound_Memory());
                mb.add(BackendEndbound_CPU());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder &TopdownL2_Retiring()
        {
            static const MetricBuilder mb = []
            {
                MetricBuilder mb{};
                mb.add(Retiring_Fastpath());
                mb.add(Retiring_Microcode());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder &TopdownL2_BadSpec()
        {
            static const MetricBuilder mb = []
            {
                MetricBuilder mb{};
                mb.add(BadSpeculation_Mispredicts());
                mb.add(BadSpeculation_PipelineRestarts());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder &AllTopdown()
        {
            static const MetricBuilder mb = []
            {
                MetricBuilder mb{};
                mb.add(TopdownL1());
                mb.add(TopdownL2());
                return mb;
            }();
            return mb;
        }

        // Aggregate all cache miss metrics
        static const MetricBuilder &AllMPKI()
        {
            static const MetricBuilder mb = []
            {
                MetricBuilder mb{};
                mb.add(L1MPKI());
                mb.add(L2MPKI());
                mb.add(L3MPKI());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder &AllCacheHitRatio()
        {
            static const MetricBuilder mb = []
            {
                MetricBuilder mb{};
                mb.add(L2HitRatio());
                mb.add(L3HitRatio());
                return mb;
            }();
            return mb;
        }

        // Aggregate all STLB MPKI metrics
        static const MetricBuilder &AllSTLBMPKI()
        {
            static const MetricBuilder mb = []
            {
                MetricBuilder mb{};
                mb.add(TLBMPKI());
                mb.add(ITLBMPKI());
                mb.add(DTLBMPKI());
                return mb;
            }();
            return mb;
        }

        // Aggregate all latency and parallelism metrics
        static const MetricBuilder &AllLatencyAndParallelism()
        {
            static const MetricBuilder mb = []
            {
                MetricBuilder mb{};
                mb.add(LoadMissLatency());
                mb.add(ILP());
                mb.add(MLP());
                return mb;
            }();
            return mb;
        }

        // Aggregate all DRAM bandwidth metrics
        static const MetricBuilder &AllDRAMBandwidth()
        {
            static const MetricBuilder mb = []
            {
                MetricBuilder mb{};
                mb.add(DRAMBandwidthGBs());
                return mb;
            }();
            return mb;
        }

        // Aggregate all instruction-per-event metrics
        static const MetricBuilder &AllIpMetrics()
        {
            static const MetricBuilder mb = []
            {
                MetricBuilder mb{};
                mb.add(IpC());
                mb.add(IpCall());
                mb.add(IpBranch());
                mb.add(IpMemLoad());
                mb.add(IpMemStore());
                mb.add(IpMispredict());
                mb.add(IpFLOP());
                mb.add(IpArith());
                mb.add(IpArithScalarSP());
                mb.add(IpArithScalarDP());
                mb.add(IpSWPF());
                return mb;
            }();
            return mb;
        }

        // Aggregate all branch-related metrics
        static const MetricBuilder &AllBranchMetrics()
        {
            static const MetricBuilder mb = []
            {
                MetricBuilder mb{};
                mb.add(BranchMisprRatio());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder &AllMetrics()
        {
            static const MetricBuilder mb = []
            {
                MetricBuilder mb{};
                mb.add(AllMPKI());
                mb.add(AllSTLBMPKI());
                mb.add(AllLatencyAndParallelism());
                mb.add(AllDRAMBandwidth());
                mb.add(AllIpMetrics());
                mb.add(AllBranchMetrics());
                mb.add(AllTopdown());
                return mb;
            }();
            return mb;
        }
    };
}
#endif // close OPTKIT_ENV_CPU_AMD