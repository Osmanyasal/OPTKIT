#pragma once
#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_INTEL

#include "utils/metric_builder.hh"
#include "core/metrics/cpu/core_metrics.hh"
#include "core/metrics/cpu/intel/event_mapper.hh"
#include "core/metrics/cpu/intel/native_events.hh"

// Intel (decode width)
#if OPTKIT_ENV_CPU_MICROARCH_P6 || OPTKIT_ENV_CPU_MICROARCH_WSM ||  \
    OPTKIT_ENV_CPU_MICROARCH_NHM || OPTKIT_ENV_CPU_MICROARCH_SNB || \
    OPTKIT_ENV_CPU_MICROARCH_IVB || OPTKIT_ENV_CPU_MICROARCH_HSW || \
    OPTKIT_ENV_CPU_MICROARCH_BDW || OPTKIT_ENV_CPU_MICROARCH_SKL || \
    OPTKIT_ENV_CPU_MICROARCH_KBL || OPTKIT_ENV_CPU_MICROARCH_CFL || \
    OPTKIT_ENV_CPU_MICROARCH_CML
#define SUPERSCALAR_WIDE 4 // 4-wide decode (P6 to Skylake)
#elif OPTKIT_ENV_CPU_MICROARCH_ICL || OPTKIT_ENV_CPU_MICROARCH_TGL || \
    OPTKIT_ENV_CPU_MICROARCH_RKL || OPTKIT_ENV_CPU_MICROARCH_ADL ||   \
    OPTKIT_ENV_CPU_MICROARCH_RPL || OPTKIT_ENV_CPU_MICROARCH_MTL ||   \
    OPTKIT_ENV_CPU_MICROARCH_SPR || OPTKIT_ENV_CPU_MICROARCH_EMR ||   \
    OPTKIT_ENV_CPU_MICROARCH_GRN
#define SUPERSCALAR_WIDE 6 // 6-wide decode (Ice Lake to Meteor Lake)
#else
#define SUPERSCALAR_WIDE 6 // Fallback (modern Intel default)
#endif

/**
 * @brief AMD CoreEvent implementation for Zen+ architecture.
 *
 * This implementation is based on performance events available on Intel SPR,SKL,ICL,HSW CPUs.
 * Event compatibility with other Intel architectures is not guaranteed.
 *
 * https://en.wikichip.org/wiki/intel/cpuid
 *
 * Perf imlementation:
 * https://github.com/torvalds/linux/tree/master/tools/perf/pmu-events/arch/x86
 */

// Warn: to use template initialisation for a certain type, they must be in the same namespace. so do NOT change it.
namespace optkit::core::metrics::cpu
{
    /**
     * @class IntelMetricsImpl
     * @brief Interface for retrieving AMD CPU performance metrics. Each metric method returns the set of events required for its computation.
     *
     */
    class IntelMetricsImpl
    {
    };

    template <>
    class CoreMetrics<IntelMetricsImpl>
    {
    public:
        // Native Metric implementations (not included in CoreMetrics)
        static MetricBuilder L1HitRatio()
        {
            std::string l1_misses_name = to_string(CoreEvents::L1_MISSES);
            std::string l1_hits_name = to_string(CoreEvents::L1_HITS);
            return MetricBuilder{}
                .add(l1_misses_name, intel::EventMapper::get(CoreEvents::L1_MISSES))
                .add(l1_hits_name, intel::EventMapper::get(CoreEvents::L1_HITS))
                .build("L1HitRatio__%",
                       [l1_hits_name, l1_misses_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t l1_hits = get_event_count(counts, l1_hits_name);
                           uint64_t l1_misses = get_event_count(counts, l1_misses_name);
                           uint64_t l1_cache_accesses = l1_hits + l1_misses;
                           // Avoid div by zero
                           if (l1_cache_accesses == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100 * (static_cast<double>(l1_hits) / static_cast<double>(l1_cache_accesses));
                       });
        }
        // Native Metric implementations (not included in CoreMetrics)
        static MetricBuilder L2HitRatio()
        {
            std::string l2_misses_name = to_string(CoreEvents::L2_MISSES);
            std::string l2_hits_name = to_string(CoreEvents::L2_HITS);
            return MetricBuilder{}
                .add(l2_misses_name, intel::EventMapper::get(CoreEvents::L2_MISSES))
                .add(l2_hits_name, intel::EventMapper::get(CoreEvents::L2_HITS))
                .build("L2HitRatio__%",
                       [l2_hits_name, l2_misses_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t l2_hits = get_event_count(counts, l2_hits_name);
                           uint64_t l2_misses = get_event_count(counts, l2_misses_name);
                           uint64_t l2_cache_accesses = l2_hits + l2_misses;
                           // Avoid div by zero
                           if (l2_cache_accesses == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100 * (static_cast<double>(l2_hits) / static_cast<double>(l2_cache_accesses));
                       });
        }

        static MetricBuilder L3HitRatio()
        {
            std::string l3_misses_name = to_string(CoreEvents::L3_MISSES);
#if OPTKIT_ENV_CPU_MICROARCH_KNL
            std::string l3_hits_name = to_string(intel::NativeEvents::L3_DEMAND_REFERENCES);
#else
            std::string l3_hits_name = to_string(CoreEvents::L3_HITS);
#endif
            return MetricBuilder{}
                .add(l3_misses_name, intel::EventMapper::get(CoreEvents::L3_MISSES))
#if OPTKIT_ENV_CPU_MICROARCH_KNL
                .add(l3_hits_name, intel::EventMapper::get(intel::NativeEvents::L3_DEMAND_REFERENCES))
#else
                .add(l3_hits_name, intel::EventMapper::get(CoreEvents::L3_HITS))
#endif
                .build("L3HitRatio__%",
                       [l3_hits_name, l3_misses_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t l3_hits = get_event_count(counts, l3_hits_name);
                           uint64_t l3_misses = get_event_count(counts, l3_misses_name);
#if OPTKIT_ENV_CPU_MICROARCH_KNL
                           l3_hits = l3_hits - l3_misses; // L3_DEMANDS - L3_MISSES
#endif

                           uint64_t l3_cache_accesses = l3_hits + l3_misses;
                           if (l3_cache_accesses == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100.0 * static_cast<double>(l3_hits) / static_cast<double>(l3_cache_accesses);
                       });
        }

    public:
        // CoreMetrics Implementation

        // Cache miss per kilo instruction (MPKI)
        static MetricBuilder L1MPKI()
        {
            std::string l1_misses_name = to_string(CoreEvents::L1_MISSES);
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            return MetricBuilder{}
                .add(l1_misses_name, intel::EventMapper::get(CoreEvents::L1_MISSES))
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
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
        }

        static MetricBuilder L2MPKI()
        {
            std::string l2_misses_name = to_string(CoreEvents::L2_MISSES);
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

            return MetricBuilder{}
                .add(l2_misses_name, intel::EventMapper::get(CoreEvents::L2_MISSES))
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
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
        }

        static MetricBuilder L3MPKI()
        {
            std::string l3_misses_name = to_string(CoreEvents::L3_MISSES);
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

            return MetricBuilder{}
                .add(l3_misses_name, intel::EventMapper::get(CoreEvents::L3_MISSES))
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
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
        }

        // Branch
        static MetricBuilder BranchMisprRatio()
        {
            std::string branch_inst_retired_name = to_string(CoreEvents::BRANCH_INST_RETIRED);
            std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);

            return MetricBuilder{}
                .add(branch_inst_retired_name, intel::EventMapper::get(CoreEvents::BRANCH_INST_RETIRED))
                .add(branch_misp_retired_name, intel::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
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
        }

        // ITLB MPKI metrics
        static MetricBuilder ITLBMPKI()
        {
            std::string itlb_misses_name = to_string(CoreEvents::ITLB_MISSES);
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

            return MetricBuilder{}
                .add(itlb_misses_name, intel::EventMapper::get(CoreEvents::ITLB_MISSES))
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .build("ITLBMPKI",
                       [itlb_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t itlb_misses = get_event_count(counts, itlb_misses_name);
                           uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                           if (inst_retired == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 1000.0 * static_cast<double>(itlb_misses) / static_cast<double>(inst_retired);
                       });
        }

        // DTLB MPKI metrics
        static MetricBuilder DTLBMPKI()
        {
            std::string dtlb_misses_name = to_string(CoreEvents::DTLB_MISSES);
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

            return MetricBuilder{}
                .add(dtlb_misses_name, intel::EventMapper::get(CoreEvents::DTLB_MISSES))
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .build("DTLBMPKI",
                       [dtlb_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t itlb_misses = get_event_count(counts, dtlb_misses_name);
                           uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                           if (inst_retired == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 1000.0 * static_cast<double>(itlb_misses) / static_cast<double>(inst_retired);
                       });
        }

        // TLB MPKI metrics
        static MetricBuilder TLBMPKI()
        {
            std::string dtlb_misses_name = to_string(CoreEvents::DTLB_MISSES);
            std::string itlb_misses_name = to_string(CoreEvents::ITLB_MISSES);
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

            return MetricBuilder{}
                .add(itlb_misses_name, intel::EventMapper::get(CoreEvents::DTLB_MISSES))
                .add(dtlb_misses_name, intel::EventMapper::get(CoreEvents::ITLB_MISSES))
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
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
        }

        // Latency and parallelism metrics
        static MetricBuilder LoadMissLatency()
        {
            return {};
        }

        static MetricBuilder IpC()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string unhalted_core_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);

            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(unhalted_core_cycles_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .build("IpC", [inst_retired_name, unhalted_core_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = get_event_count(counts,inst_retired_name);
                           uint64_t unhalted_core_cycles = get_event_count(counts,unhalted_core_cycles_name);

                           if (unhalted_core_cycles == 0)
                               return -1;
                            return static_cast<double>(inst_retired) / static_cast<double>(unhalted_core_cycles); });
        }

        // #if !OPTKIT_ENV_CPU_MICROARCH_KNL // NOT KNL
        static MetricBuilder ILP()
        {
            std::string uops_cycles_thread_name = to_string(intel::NativeEvents::UOPS_CORE_CYCLES_THREAD);
            std::string core_cycles_ge_1_name = to_string(intel::NativeEvents::UOPS_CORE_CYCLES_GE_1);
            return MetricBuilder{}
                .add(uops_cycles_thread_name, intel::EventMapper::get(intel::NativeEvents::UOPS_CORE_CYCLES_THREAD))
                .add(core_cycles_ge_1_name, intel::EventMapper::get(intel::NativeEvents::UOPS_CORE_CYCLES_GE_1))
                .build("ILP__%",
                       [uops_cycles_thread_name, core_cycles_ge_1_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t uops_cycles_thread = get_event_count(counts, uops_cycles_thread_name);
                           uint64_t core_cycles_ge_1 = get_event_count(counts, core_cycles_ge_1_name);

                           // Avoid div by zero
                           if (core_cycles_ge_1 == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100 * (static_cast<double>(uops_cycles_thread)) / ((Query::is_smt_enabled() ? 2 : 1) * static_cast<double>(core_cycles_ge_1));
                       });
        }
        // #else
        //         static MetricBuilder ILP() { return {}; }
        // #endif

        static MetricBuilder MLP()
        {
            return {};
        }

        // DRAM bandwidth
        static MetricBuilder DRAMBandwidthGBs()
        {
            return {};
        }

        // Instruction per event
        static MetricBuilder IpCall()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string inst_retired_near_call_name = to_string(intel::NativeEvents::BR_INST_RETIRED_NEAR_CALL);
            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(inst_retired_near_call_name, intel::EventMapper::get(intel::NativeEvents::BR_INST_RETIRED_NEAR_CALL))
                .build("IpCall",
                       [inst_retired_name, inst_retired_near_call_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                           uint64_t inst_retired_near_call = get_event_count(counts, inst_retired_near_call_name);

                           // Avoid div by zero
                           if (inst_retired_near_call == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(inst_retired_near_call);
                       });
        }

        static MetricBuilder IpBranch()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string branch_inst_retired_name = to_string(CoreEvents::BRANCH_INST_RETIRED);
            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(branch_inst_retired_name, intel::EventMapper::get(CoreEvents::BRANCH_INST_RETIRED))
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
        }

        static MetricBuilder IpMemLoad()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string mem_load_retired_name = to_string(CoreEvents::MEM_LOAD_RETIRED);
            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(mem_load_retired_name, intel::EventMapper::get(CoreEvents::MEM_LOAD_RETIRED))
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
        }

        static MetricBuilder IpMemStore()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string mem_store_retired_name = to_string(CoreEvents::MEM_STORE_RETIRED);
            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(mem_store_retired_name, intel::EventMapper::get(CoreEvents::MEM_STORE_RETIRED))
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
        }

        static MetricBuilder IpMispredict()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(branch_misp_retired_name, intel::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
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
        }

        // Floating-point operation metrics
        static MetricBuilder IpFLOP()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string retired_scalar_sp_any_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE);
            std::string retired_scalar_dp_any_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE);
            std::string inst_packed_128_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE);
            std::string inst_packed_128_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE);
            std::string inst_packed_256_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE);
            std::string inst_packed_256_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE);
            std::string inst_packed_512_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE);
            std::string inst_packed_512_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE);

            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(retired_scalar_sp_any_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE))
                .add(retired_scalar_dp_any_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE))
                .add(inst_packed_128_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE))
                .add(inst_packed_128_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE))
                .add(inst_packed_256_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE))
                .add(inst_packed_256_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE))
                .add(inst_packed_512_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE))
                .add(inst_packed_512_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE))
                .build("IpFLOP",
                       [inst_retired_name, retired_scalar_sp_any_name, retired_scalar_dp_any_name,
                        inst_packed_128_double_name, inst_packed_128_single_name,
                        inst_packed_256_double_name, inst_packed_256_single_name,
                        inst_packed_512_double_name, inst_packed_512_single_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                           uint64_t retired_scalar_sp_any = get_event_count(counts, retired_scalar_sp_any_name);
                           uint64_t retired_scalar_dp_any = get_event_count(counts, retired_scalar_dp_any_name);
                           uint64_t inst_packed_128_double = get_event_count(counts, inst_packed_128_double_name);
                           uint64_t inst_packed_128_single = get_event_count(counts, inst_packed_128_single_name);
                           uint64_t inst_packed_256_double = get_event_count(counts, inst_packed_256_double_name);
                           uint64_t inst_packed_256_single = get_event_count(counts, inst_packed_256_single_name);
                           uint64_t inst_packed_512_double = get_event_count(counts, inst_packed_512_double_name);
                           uint64_t inst_packed_512_single = get_event_count(counts, inst_packed_512_single_name);

                           uint64_t total_flops =
                               (retired_scalar_sp_any * 1) +
                               (retired_scalar_dp_any * 1) +
                               (inst_packed_128_double * 2) +
                               (inst_packed_128_single * 4) +
                               (inst_packed_256_double * 4) +
                               (inst_packed_256_single * 8) +
                               (inst_packed_512_double * 8) +
                               (inst_packed_512_single * 16);

                           // Avoid div by zero
                           if (total_flops == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(total_flops);
                       });
        }

        static MetricBuilder IpAVXAnyFLOP()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string inst_packed_128_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE);
            std::string inst_packed_128_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE);
            std::string inst_packed_256_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE);
            std::string inst_packed_256_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE);
            std::string inst_packed_512_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE);
            std::string inst_packed_512_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE);

            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(inst_packed_128_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE))
                .add(inst_packed_128_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE))
                .add(inst_packed_256_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE))
                .add(inst_packed_256_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE))
                .add(inst_packed_512_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE))
                .add(inst_packed_512_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE))
                .build("IpAVXAnyFLOP",
                       [inst_retired_name,
                        inst_packed_128_double_name, inst_packed_128_single_name,
                        inst_packed_256_double_name, inst_packed_256_single_name,
                        inst_packed_512_double_name, inst_packed_512_single_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                           uint64_t inst_packed_128_double = get_event_count(counts, inst_packed_128_double_name);
                           uint64_t inst_packed_128_single = get_event_count(counts, inst_packed_128_single_name);
                           uint64_t inst_packed_256_double = get_event_count(counts, inst_packed_256_double_name);
                           uint64_t inst_packed_256_single = get_event_count(counts, inst_packed_256_single_name);
                           uint64_t inst_packed_512_double = get_event_count(counts, inst_packed_512_double_name);
                           uint64_t inst_packed_512_single = get_event_count(counts, inst_packed_512_single_name);

                           uint64_t total_flops =
                               (inst_packed_128_double * 2) +
                               (inst_packed_128_single * 4) +
                               (inst_packed_256_double * 4) +
                               (inst_packed_256_single * 8) +
                               (inst_packed_512_double * 8) +
                               (inst_packed_512_single * 16);

                           // Avoid div by zero
                           if (total_flops == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(total_flops);
                       });
        }

        static MetricBuilder IpArith()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string inst_retired_scalar_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR);
            std::string inst_retired_vector_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_VECTOR);
            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(inst_retired_scalar_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR))
                .add(inst_retired_vector_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_VECTOR))
                .build("IpArith",
                       [inst_retired_name, inst_retired_scalar_name, inst_retired_vector_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                           uint64_t inst_retired_scalar = get_event_count(counts, inst_retired_scalar_name);
                           uint64_t inst_retired_vector = get_event_count(counts, inst_retired_vector_name);
                           uint64_t arith = inst_retired / (inst_retired_scalar + inst_retired_vector);
                           // Avoid div by zero
                           if (inst_retired == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(inst_retired);
                       });
        }

        static MetricBuilder IpArithScalarSP()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string inst_scalar_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE);
            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(inst_scalar_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE))
                .build("IpArithScalarSP",
                       [inst_scalar_single_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                           uint64_t inst_scalar_single = get_event_count(counts, inst_scalar_single_name);

                           // Avoid div by zero
                           if (inst_scalar_single == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(inst_scalar_single);
                       });
        }

        static MetricBuilder IpArithScalarDP()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string inst_scalar_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE);
            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(inst_scalar_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE))
                .build("IpArithScalarDP",
                       [inst_scalar_double_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                           uint64_t inst_scalar_double = get_event_count(counts, inst_scalar_double_name);

                           // Avoid div by zero
                           if (inst_scalar_double == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(inst_scalar_double);
                       });
        }

        static MetricBuilder IpArithAVX128()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string inst_packed_128_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE);
            std::string inst_packed_128_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE);
            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(inst_packed_128_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE))
                .add(inst_packed_128_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE))
                .build("IpArithAVX128",
                       [inst_packed_128_double_name, inst_packed_128_single_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                           uint64_t inst_packed_128_double = get_event_count(counts, inst_packed_128_double_name);
                           uint64_t inst_packed_128_single = get_event_count(counts, inst_packed_128_single_name);

                           uint64_t inst_packed_128 = inst_packed_128_double + inst_packed_128_single;
                           // Avoid div by zero
                           if (inst_packed_128 == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(inst_packed_128);
                       });
        }

        static MetricBuilder IpArithAVX256()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string inst_packed_256_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE);
            std::string inst_packed_256_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE);
            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(inst_packed_256_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE))
                .add(inst_packed_256_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE))
                .build("IpArithAVX256",
                       [inst_packed_256_double_name, inst_packed_256_single_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                           uint64_t inst_packed_256_double = get_event_count(counts, inst_packed_256_double_name);
                           uint64_t inst_packed_256_single = get_event_count(counts, inst_packed_256_single_name);

                           uint64_t inst_packed_256 = inst_packed_256_double + inst_packed_256_single;
                           // Avoid div by zero
                           if (inst_packed_256 == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(inst_packed_256);
                       });
        }

        static MetricBuilder IpArithAVX512()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string inst_packed_512_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE);
            std::string inst_packed_512_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE);
            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(inst_packed_512_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE))
                .add(inst_packed_512_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE))
                .build("IpArithAVX512",
                       [inst_packed_512_double_name, inst_packed_512_single_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                           uint64_t inst_packed_512_double = get_event_count(counts, inst_packed_512_double_name);
                           uint64_t inst_packed_512_single = get_event_count(counts, inst_packed_512_single_name);

                           uint64_t inst_packed_512 = inst_packed_512_double + inst_packed_512_single;
                           // Avoid div by zero
                           if (inst_packed_512 == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(inst_packed_512);
                       });
        }

        static MetricBuilder ScalarArithpVector()
        {
            std::string inst_vector_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_VECTOR);
            std::string inst_scalar_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR);
            return MetricBuilder{}
                .add(inst_vector_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_VECTOR))
                .add(inst_scalar_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR))
                .build("ScalarArithpVector",
                       [inst_vector_name, inst_scalar_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_vector = get_event_count(counts, inst_vector_name);
                           uint64_t inst_scalar = get_event_count(counts, inst_scalar_name);
                           // Avoid div by zero
                           if (inst_vector == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_scalar) / static_cast<double>(inst_vector);
                       });
        }

        static MetricBuilder IpArithVectorAny()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string inst_packed_128_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE);
            std::string inst_packed_128_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE);
            std::string inst_packed_256_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE);
            std::string inst_packed_256_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE);
            std::string inst_packed_512_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE);
            std::string inst_packed_512_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE);

            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(inst_packed_128_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE))
                .add(inst_packed_128_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE))
                .add(inst_packed_256_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE))
                .add(inst_packed_256_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE))
                .add(inst_packed_512_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE))
                .add(inst_packed_512_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE))
                .build("IpArithAVXAny",
                       [inst_retired_name,
                        inst_packed_128_double_name, inst_packed_128_single_name,
                        inst_packed_256_double_name, inst_packed_256_single_name,
                        inst_packed_512_double_name, inst_packed_512_single_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                           uint64_t inst_packed_128_double = get_event_count(counts, inst_packed_128_double_name);
                           uint64_t inst_packed_128_single = get_event_count(counts, inst_packed_128_single_name);
                           uint64_t inst_packed_256_double = get_event_count(counts, inst_packed_256_double_name);
                           uint64_t inst_packed_256_single = get_event_count(counts, inst_packed_256_single_name);
                           uint64_t inst_packed_512_double = get_event_count(counts, inst_packed_512_double_name);
                           uint64_t inst_packed_512_single = get_event_count(counts, inst_packed_512_single_name);

                           uint64_t total_avx_instr =
                               (inst_packed_128_double) +
                               (inst_packed_128_single) +
                               (inst_packed_256_double) +
                               (inst_packed_256_single) +
                               (inst_packed_512_double) +
                               (inst_packed_512_single);

                           // Avoid div by zero
                           if (total_avx_instr == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(total_avx_instr);
                       });
        }

        static MetricBuilder ScalarpArithVector()
        {
            return {};
        }

        // Software prefetch
        static MetricBuilder IpSWPF()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string sw_load_prefetch_name = to_string(CoreEvents::SW_LOAD_PREFETCH_ACCESS);
            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(sw_load_prefetch_name, intel::EventMapper::get(CoreEvents::SW_LOAD_PREFETCH_ACCESS))
                .build("IpSWPF",
                       [sw_load_prefetch_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                           uint64_t sw_load_prefetch = get_event_count(counts, sw_load_prefetch_name);

                           // Avoid div by zero
                           if (sw_load_prefetch == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(sw_load_prefetch);
                       });
        }

        // Topdown (Pipeline Utilisation) Analysis L1
        static MetricBuilder FrontendBound()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string no_ops_from_frontend_name = to_string(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE);
            return MetricBuilder{}
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(no_ops_from_frontend_name, intel::EventMapper::get(cpu::intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE))
                .build("FrontendBound__%",
                       [dispatch_slots_name, no_ops_from_frontend_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                           uint64_t no_ops_from_frontend = get_event_count(counts, no_ops_from_frontend_name);

                           // Avoid div by zero
                           if (dispatch_slots == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100 * static_cast<double>(no_ops_from_frontend) / (static_cast<double>(dispatch_slots));
                       });
        }
 
        static MetricBuilder BadSpeculation()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string uops_issued_name = to_string(intel::NativeEvents::UOPS_ISSUED);
            std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);
            std::string recovery_cycles_name = to_string(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES);

            return MetricBuilder{}
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(uops_issued_name, intel::EventMapper::get(intel::NativeEvents::UOPS_ISSUED))
                .add(uops_retired_slots_name, intel::EventMapper::get(cpu::intel::NativeEvents::UOPS_RETIRED_SLOTS))
                .add(recovery_cycles_name, intel::EventMapper::get(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES))
                .build("BadSpeculation__%",
                       [dispatch_slots_name, uops_issued_name, uops_retired_slots_name, recovery_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                           uint64_t uops_issued = get_event_count(counts, uops_issued_name);
                           uint64_t uops_retired_slots = get_event_count(counts, uops_retired_slots_name);
                           uint64_t recovery_cycles = get_event_count(counts, recovery_cycles_name);

                           // Avoid div by zero
                           if (dispatch_slots == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100 * (static_cast<double>(uops_issued) - static_cast<double>(uops_retired_slots) + SUPERSCALAR_WIDE * static_cast<double>(recovery_cycles)) / (static_cast<double>(dispatch_slots));
                       });
        }
        // #else
        //         static MetricBuilder BadSpeculation() { return {}; }
        // #endif

        static MetricBuilder Retiring()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);

            return MetricBuilder{}
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(uops_retired_slots_name, intel::EventMapper::get(intel::NativeEvents::UOPS_RETIRED_SLOTS))
                .build("Retiring__%",
                       [dispatch_slots_name, uops_retired_slots_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                           uint64_t uops_retired_slots = get_event_count(counts, uops_retired_slots_name);

                           // Avoid div by zero
                           if (dispatch_slots == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100 * (static_cast<double>(uops_retired_slots)) / (static_cast<double>(dispatch_slots));
                       });
        }

        static MetricBuilder
        SMTContention()
        {
            // std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            // std::string smt_stalls_name = to_string(intel::NativeEvents::SMT_STALLS_1);
            // return MetricBuilder{}
            //     .add(smt_stalls_name, intel::EventMapper::get(cpu::intel::NativeEvents::SMT_STALLS_1))
            //     .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
            //     .build("SMTContention",
            //            [dispatch_slots_name, smt_stalls_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
            //            {
            //                uint64_t smt_stalls = get_event_count(counts,smt_stalls_name);
            //                uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts,dispatch_slots_name);

            //                // Avoid div by zero
            //                if (dispatch_slots == 0)
            //                    return std::numeric_limits<double>::quiet_NaN();
            //                return 100 * static_cast<double>(smt_stalls) / (static_cast<double>(dispatch_slots));
            //            });
            return {};
        }

        static MetricBuilder BackendBound()
        {

            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string no_ops_from_frontend_name = to_string(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE);
            std::string uops_issued_name = to_string(intel::NativeEvents::UOPS_ISSUED);
            std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);
            std::string recovery_cycles_name = to_string(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES);

            return MetricBuilder{}
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(no_ops_from_frontend_name, intel::EventMapper::get(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE))
                .add(uops_issued_name, intel::EventMapper::get(intel::NativeEvents::UOPS_ISSUED))
                .add(uops_retired_slots_name, intel::EventMapper::get(intel::NativeEvents::UOPS_RETIRED_SLOTS))
                .add(recovery_cycles_name, intel::EventMapper::get(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES))
                .build("BackendBound__%",
                       [dispatch_slots_name, no_ops_from_frontend_name, uops_issued_name, uops_retired_slots_name, recovery_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                           uint64_t no_ops_from_frontend = get_event_count(counts, no_ops_from_frontend_name);
                           uint64_t uops_issued = get_event_count(counts, uops_issued_name);
                           uint64_t uops_retired_slots = get_event_count(counts, uops_retired_slots_name);
                           uint64_t recovery_cycles = get_event_count(counts, recovery_cycles_name);

                           double retiring = static_cast<double>(uops_retired_slots) / (static_cast<double>(dispatch_slots));
                           double frontend_bound = static_cast<double>(no_ops_from_frontend) / (static_cast<double>(dispatch_slots));
                           double bad_speculation = (static_cast<double>(uops_issued) - static_cast<double>(uops_retired_slots) + SUPERSCALAR_WIDE * static_cast<double>(recovery_cycles)) / (static_cast<double>(dispatch_slots));
                           // Avoid div by zero
                           return 100 * (1 - (frontend_bound + bad_speculation + retiring));
                       });
        }
        // Topdown (Pipeline Utilisation) Analysis L1
        static MetricBuilder FrontendBound_Latency()
        {
            std::string clocks_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string no_ops_from_frontend_name = to_string(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE);
            std::string uops_not_delivered_cycles_name = to_string(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CYCLES_0);

            return MetricBuilder{}
                .add(clocks_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(no_ops_from_frontend_name, intel::EventMapper::get(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE))
                .add(uops_not_delivered_cycles_name, intel::EventMapper::get(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CYCLES_0))
                .build("FrontendBound_Latency__%",
                       [clocks_name, no_ops_from_frontend_name, uops_not_delivered_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t clocks = get_event_count(counts, clocks_name);
                           uint64_t dispatch_slots = SUPERSCALAR_WIDE * clocks;
                           uint64_t no_ops_from_frontend = get_event_count(counts, no_ops_from_frontend_name);
                           uint64_t uops_not_delivered_cycles = get_event_count(counts, uops_not_delivered_cycles_name);

                           double frontend_bw = (static_cast<double>(uops_not_delivered_cycles) / static_cast<double>(clocks));
                           // Avoid div by zero
                           if (dispatch_slots == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100 * (static_cast<double>(no_ops_from_frontend) / static_cast<double>(dispatch_slots) - frontend_bw);
                       });
        }

        static MetricBuilder FrontendBound_BW()
        {
            std::string clocks_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string uops_not_delivered_cycles_name = to_string(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CYCLES_0);

            return MetricBuilder{}
                .add(clocks_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(uops_not_delivered_cycles_name, intel::EventMapper::get(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CYCLES_0))
                .build("FrontendBound_BW__%",
                       [clocks_name, uops_not_delivered_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t clocks = get_event_count(counts, clocks_name);
                           uint64_t uops_not_delivered_cycles = get_event_count(counts, uops_not_delivered_cycles_name);
                           // Avoid div by zero
                           if (clocks == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100 * (static_cast<double>(uops_not_delivered_cycles) / static_cast<double>(clocks));
                       });
        }

        static MetricBuilder BadSpeculation_Mispredicts()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
            std::string machine_clears_count_name = to_string(intel::NativeEvents::MACHINE_CLEARS_COUNT);
            std::string uops_issued_name = to_string(intel::NativeEvents::UOPS_ISSUED);
            std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);
            std::string recovery_cycles_name = to_string(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES);

            return MetricBuilder{}
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(branch_misp_retired_name, intel::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                .add(machine_clears_count_name, intel::EventMapper::get(intel::NativeEvents::MACHINE_CLEARS_COUNT))
                .add(uops_issued_name, intel::EventMapper::get(intel::NativeEvents::UOPS_ISSUED))
                .add(uops_retired_slots_name, intel::EventMapper::get(intel::NativeEvents::UOPS_RETIRED_SLOTS))
                .add(recovery_cycles_name, intel::EventMapper::get(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES))
                .build("BadSpeculation_Mispredicts__%",
                       [dispatch_slots_name, branch_misp_retired_name, machine_clears_count_name, uops_issued_name, uops_retired_slots_name, recovery_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                           uint64_t branch_misp_retired = get_event_count(counts, branch_misp_retired_name);
                           uint64_t machine_clears_count = get_event_count(counts, machine_clears_count_name);
                           uint64_t uops_issued = get_event_count(counts, uops_issued_name);
                           uint64_t uops_retired_slots = get_event_count(counts, uops_retired_slots_name);
                           uint64_t recovery_cycles = get_event_count(counts, recovery_cycles_name);

                           // Avoid div by zero
                           if (dispatch_slots == 0 || (branch_misp_retired + machine_clears_count) == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           double bad_speculation = (static_cast<double>(uops_issued) - static_cast<double>(uops_retired_slots) + SUPERSCALAR_WIDE * static_cast<double>(uops_retired_slots)) / (static_cast<double>(dispatch_slots));

                           return 100 * bad_speculation * (static_cast<double>(branch_misp_retired / (branch_misp_retired + machine_clears_count)));
                       });
        }
        static MetricBuilder BadSpeculation_PipelineRestarts()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
            std::string machine_clears_count_name = to_string(intel::NativeEvents::MACHINE_CLEARS_COUNT);
            std::string uops_issued_name = to_string(intel::NativeEvents::UOPS_ISSUED);
            std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);
            std::string recovery_cycles_name = to_string(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES);

            return MetricBuilder{}
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(branch_misp_retired_name, intel::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                .add(machine_clears_count_name, intel::EventMapper::get(intel::NativeEvents::MACHINE_CLEARS_COUNT))
                .add(uops_issued_name, intel::EventMapper::get(intel::NativeEvents::UOPS_ISSUED))
                .add(uops_retired_slots_name, intel::EventMapper::get(intel::NativeEvents::UOPS_RETIRED_SLOTS))
                .add(recovery_cycles_name, intel::EventMapper::get(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES))
                .build("BadSpeculation_PipelineRestarts__%",
                       [dispatch_slots_name, branch_misp_retired_name, machine_clears_count_name, uops_issued_name, uops_retired_slots_name, recovery_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                           uint64_t branch_misp_retired = get_event_count(counts, branch_misp_retired_name);
                           uint64_t machine_clears_count = get_event_count(counts, machine_clears_count_name);
                           uint64_t uops_issued = get_event_count(counts, uops_issued_name);
                           uint64_t uops_retired_slots = get_event_count(counts, uops_retired_slots_name);
                           uint64_t recovery_cycles = get_event_count(counts, recovery_cycles_name);

                           // Avoid div by zero
                           if (dispatch_slots == 0 || (branch_misp_retired + machine_clears_count) == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           double bad_speculation = (static_cast<double>(uops_issued) - static_cast<double>(uops_retired_slots) + SUPERSCALAR_WIDE * static_cast<double>(uops_retired_slots)) / (static_cast<double>(dispatch_slots));

                           return 100 * bad_speculation - (bad_speculation * (static_cast<double>(branch_misp_retired / (branch_misp_retired + machine_clears_count))));
                       });
        }
 
        static MetricBuilder BackendEndbound_Memory()
        {
            std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string stalls_l1d_miss_name = to_string(intel::NativeEvents::STALLS_L1D_MISS);
            std::string stalls_l2_miss_name = to_string(intel::NativeEvents::STALLS_L2_MISS);
            std::string stalls_l3_miss_name = to_string(intel::NativeEvents::STALLS_L3_MISS);
            std::string resource_stalls_sb_name = to_string(intel::NativeEvents::RESOURCE_STALLS_SB);

            return MetricBuilder{}
                .add(unhalted_cycles_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(stalls_l1d_miss_name, intel::EventMapper::get(intel::NativeEvents::STALLS_L1D_MISS))
                .add(stalls_l2_miss_name, intel::EventMapper::get(intel::NativeEvents::STALLS_L2_MISS))
                .add(stalls_l3_miss_name, intel::EventMapper::get(intel::NativeEvents::STALLS_L3_MISS))
                .add(resource_stalls_sb_name, intel::EventMapper::get(intel::NativeEvents::RESOURCE_STALLS_SB))
                .build("BackendEndbound_Memory__%",
                       [unhalted_cycles_name, stalls_l1d_miss_name, stalls_l2_miss_name, stalls_l3_miss_name, resource_stalls_sb_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t unhalted_cycles = get_event_count(counts, unhalted_cycles_name);
                           uint64_t stalls_l1d_miss = get_event_count(counts, stalls_l1d_miss_name);
                           uint64_t stalls_l2_miss = get_event_count(counts, stalls_l2_miss_name);
                           uint64_t stalls_l3_miss = get_event_count(counts, stalls_l3_miss_name);
                           uint64_t resource_stalls_sb = get_event_count(counts, resource_stalls_sb_name);
                           // Avoid div by zero
                           if (unhalted_cycles == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100 * ((static_cast<double>(stalls_l1d_miss) +
                                          static_cast<double>(stalls_l2_miss) +
                                          static_cast<double>(stalls_l3_miss) -
                                          static_cast<double>(resource_stalls_sb)) /
                                         unhalted_cycles);
                       });
        }

        static MetricBuilder BackendEndbound_CPU()
        {
            std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string stalls_l1d_miss_name = to_string(intel::NativeEvents::STALLS_L1D_MISS);
            std::string stalls_l2_miss_name = to_string(intel::NativeEvents::STALLS_L2_MISS);
            std::string stalls_l3_miss_name = to_string(intel::NativeEvents::STALLS_L3_MISS);
            std::string resource_stalls_sb_name = to_string(intel::NativeEvents::RESOURCE_STALLS_SB);

            return MetricBuilder{}
                .add(unhalted_cycles_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(stalls_l1d_miss_name, intel::EventMapper::get(intel::NativeEvents::STALLS_L1D_MISS))
                .add(stalls_l2_miss_name, intel::EventMapper::get(intel::NativeEvents::STALLS_L2_MISS))
                .add(stalls_l3_miss_name, intel::EventMapper::get(intel::NativeEvents::STALLS_L3_MISS))
                .add(resource_stalls_sb_name, intel::EventMapper::get(intel::NativeEvents::RESOURCE_STALLS_SB))
                .build("BackendEndbound_CPU__%",
                       [unhalted_cycles_name, stalls_l1d_miss_name, stalls_l2_miss_name, stalls_l3_miss_name, resource_stalls_sb_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t unhalted_cycles = get_event_count(counts, unhalted_cycles_name);
                           uint64_t stalls_l1d_miss = get_event_count(counts, stalls_l1d_miss_name);
                           uint64_t stalls_l2_miss = get_event_count(counts, stalls_l2_miss_name);
                           uint64_t stalls_l3_miss = get_event_count(counts, stalls_l3_miss_name);
                           uint64_t resource_stalls_sb = get_event_count(counts, resource_stalls_sb_name);
                           // Avoid div by zero
                           if (unhalted_cycles == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           double backend_memory_bound = ((static_cast<double>(stalls_l1d_miss) +
                                                           static_cast<double>(stalls_l2_miss) +
                                                           static_cast<double>(stalls_l3_miss) -
                                                           static_cast<double>(resource_stalls_sb)) /
                                                          unhalted_cycles);
                           return 100.0 * (1 - backend_memory_bound);
                       });
        }

        static MetricBuilder Retiring_Fastpath()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);
            std::string idq_ms_uops_name = to_string(intel::NativeEvents::IDQ_MS_UOPS);
            std::string uops_issued_name = to_string(intel::NativeEvents::UOPS_ISSUED);

            return MetricBuilder{}
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(uops_retired_slots_name, intel::EventMapper::get(intel::NativeEvents::UOPS_RETIRED_SLOTS))
                .add(idq_ms_uops_name, intel::EventMapper::get(intel::NativeEvents::IDQ_MS_UOPS))
                .add(uops_issued_name, intel::EventMapper::get(intel::NativeEvents::UOPS_ISSUED))
                .build("Retiring_Fastpath__%",
                       [dispatch_slots_name, uops_retired_slots_name, idq_ms_uops_name, uops_issued_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                           uint64_t uops_retired_slots = get_event_count(counts, uops_retired_slots_name);
                           uint64_t idq_ms_uops = get_event_count(counts, idq_ms_uops_name);
                           uint64_t uops_issued = get_event_count(counts, uops_issued_name);

                           // Avoid div by zero
                           if (dispatch_slots)
                               return std::numeric_limits<double>::quiet_NaN();
                           double retiring_microcode = ((static_cast<double>(uops_retired_slots) / static_cast<double>(dispatch_slots)) * (static_cast<double>(idq_ms_uops) / static_cast<double>(dispatch_slots)));
                           return 100 * ((static_cast<double>(uops_retired_slots) / static_cast<double>(dispatch_slots)) - retiring_microcode);
                       });
        }

        static MetricBuilder Retiring_Microcode()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);
            std::string idq_ms_uops_name = to_string(intel::NativeEvents::IDQ_MS_UOPS);
            std::string uops_issued_name = to_string(intel::NativeEvents::UOPS_ISSUED);

            return MetricBuilder{}
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(uops_retired_slots_name, intel::EventMapper::get(intel::NativeEvents::UOPS_RETIRED_SLOTS))
                .add(idq_ms_uops_name, intel::EventMapper::get(intel::NativeEvents::IDQ_MS_UOPS))
                .add(uops_issued_name, intel::EventMapper::get(intel::NativeEvents::UOPS_ISSUED))
                .build("Retiring_Microcode__%",
                       [dispatch_slots_name, uops_retired_slots_name, idq_ms_uops_name, uops_issued_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                           uint64_t uops_retired_slots = get_event_count(counts, uops_retired_slots_name);
                           uint64_t idq_ms_uops = get_event_count(counts, idq_ms_uops_name);
                           uint64_t uops_issued = get_event_count(counts, uops_issued_name);

                           // Avoid div by zero
                           if (dispatch_slots)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100 * ((static_cast<double>(uops_retired_slots) / static_cast<double>(dispatch_slots)) * (static_cast<double>(idq_ms_uops) / static_cast<double>(dispatch_slots)));
                       });
        }

        
        // Aggregated Metrics
        
        // Topdown (Pipeline Utilisation) Analysis L1
        static MetricBuilder TopdownL1()
        {
            MetricBuilder mb{};
            mb.add(FrontendBound());
            mb.add(BackendBound());
            mb.add(Retiring());
            mb.add(BadSpeculation());
            mb.add(SMTContention());
            return mb;
        }

        static MetricBuilder TopdownL2()
        {
            MetricBuilder mb{};
            mb.add(TopdownL2_FE());
            mb.add(TopdownL2_BE());
            mb.add(TopdownL2_Retiring());
            mb.add(TopdownL2_BadSpec());
            return mb;
        }
        
        static MetricBuilder TopdownL2_FE()
        {
            MetricBuilder mb{};
            mb.add(FrontendBound_Latency());
            mb.add(FrontendBound_BW());
            return mb;
        }
        static MetricBuilder TopdownL2_BE()
        {
            MetricBuilder mb{};
            mb.add(BackendEndbound_Memory());
            mb.add(BackendEndbound_CPU());
            return mb;
        }
        static MetricBuilder TopdownL2_Retiring()
        {
            MetricBuilder mb{};
            mb.add(Retiring_Fastpath());
            mb.add(Retiring_Microcode());
            return mb;
        }
        static MetricBuilder TopdownL2_BadSpec()
        {
            MetricBuilder mb{};
            mb.add(BadSpeculation_Mispredicts());
            mb.add(BadSpeculation_PipelineRestarts());
            return mb;
        }

        static MetricBuilder AllTopdown()
        {
            MetricBuilder mb{};
            mb.add(TopdownL1());
            mb.add(TopdownL2());
            return mb;
        }
        // Aggregate all cache miss metrics
        static MetricBuilder AllMPKI()
        {
            MetricBuilder mb;
            mb.add(L1MPKI());
            mb.add(L2MPKI());
            mb.add(L3MPKI());
            return mb;
        }

        static MetricBuilder AllCacheHitRatio()
        {
            MetricBuilder mb;
            mb.add(L2HitRatio());
            mb.add(L3HitRatio());
            return mb;
        }

        // Aggregate all STLB MPKI metrics
        static MetricBuilder AllSTLBMPKI()
        {
            MetricBuilder mb;
            mb.add(TLBMPKI());
            mb.add(ITLBMPKI());
            mb.add(DTLBMPKI());
            return mb;
        }

        // Aggregate all latency and parallelism metrics
        static MetricBuilder AllLatencyAndParallelism()
        {
            MetricBuilder mb;
            mb.add(LoadMissLatency());
            mb.add(ILP());
            mb.add(MLP());
            return mb;
        }

        // Aggregate all DRAM bandwidth metrics
        static MetricBuilder AllDRAMBandwidth()
        {
            MetricBuilder mb;
            mb.add(DRAMBandwidthGBs());
            return mb;
        }

        // Aggregate all instruction-per-event metrics
        static MetricBuilder AllIpMetrics()
        {
            MetricBuilder mb; 
            mb.add(IpC()); 
            mb.add(IpCall());
            mb.add(IpAVXAnyFLOP());
            mb.add(IpArithScalarSP());
            mb.add(IpArithScalarDP());
            mb.add(IpArithAVX128());
            mb.add(IpArithAVX256());
            mb.add(IpArithAVX512());
            mb.add(IpArithVectorAny());
            mb.add(ScalarpArithVector());
            mb.add(IpBranch());
            mb.add(IpMemLoad());
            mb.add(IpMemStore());
            mb.add(IpMispredict());
            mb.add(IpFLOP());
            mb.add(IpArith());
            mb.add(IpSWPF());
            return mb;
        }

        // Aggregate all branch-related metrics
        static MetricBuilder AllBranchMetrics()
        {
            MetricBuilder mb;
            mb.add(BranchMisprRatio());
            return mb;
        }

        static MetricBuilder AllMetrics()
        {
            MetricBuilder all;
            all.add(AllMPKI());
            all.add(AllSTLBMPKI());
            all.add(AllLatencyAndParallelism());
            all.add(AllDRAMBandwidth());
            all.add(AllIpMetrics());
            all.add(AllBranchMetrics());
            all.add(AllTopdown());
            return all;
        }
    };
}
#endif