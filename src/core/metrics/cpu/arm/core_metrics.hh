#pragma once
#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_ARM

#include "core/metrics/cpu/core_metrics.hh"
#include "core/metrics/cpu/arm/event_mapper.hh"
#include "core/metrics/cpu/arm/native_events.hh"

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
namespace optkit::core::metrics::cpu
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
        // Native Metric implementations (not included in CoreMetrics)
        static MetricBuilder L2HitRatio()
        {
            std::string l2_cache_accesses_name = to_string(arm::NativeEvents::L2_CACHE_ACCESSES);
            std::string l2_misses_name = to_string(CoreEvents::L2_MISSES);
            return MetricBuilder{}
                .add(l2_cache_accesses_name, arm::EventMapper::get(arm::NativeEvents::L2_CACHE_ACCESSES))
                .add(l2_misses_name, arm::EventMapper::get(CoreEvents::L2_MISSES))
                .build("L2HitRatio__%",
                       [l2_misses_name, l2_cache_accesses_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t l2_misses = get_event_count(counts, l2_misses_name);
                           uint64_t l2_cache_accesses = get_event_count(counts, l2_cache_accesses_name);

                           // Avoid div by zero
                           if (l2_cache_accesses == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100 * (static_cast<double>(l2_cache_accesses - l2_misses) / static_cast<double>(l2_cache_accesses));
                       });
        }

        static MetricBuilder L3HitRatio()
        {
            std::string l3_cache_accesses_name = to_string(arm::NativeEvents::L3_CACHE_ACCESSES);
            std::string l3_misses_name = to_string(CoreEvents::L3_MISSES);
            return MetricBuilder{}
                .add(l3_cache_accesses_name, arm::EventMapper::get(arm::NativeEvents::L3_CACHE_ACCESSES))
                .add(l3_misses_name, arm::EventMapper::get(CoreEvents::L3_MISSES))
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
        }

    public:
        // CoreMetrics Implementation

        // Cache miss per kilo instruction (MPKI)
        static MetricBuilder L1MPKI()
        {
            std::string l1_misses_name = to_string(CoreEvents::L1_MISSES);
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            return MetricBuilder{}
                .add(l1_misses_name, arm::EventMapper::get(CoreEvents::L1_MISSES))
                .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
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
                .add(l2_misses_name, arm::EventMapper::get(CoreEvents::L2_MISSES))
                .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
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
                .add(l3_misses_name, arm::EventMapper::get(CoreEvents::L3_MISSES))
                .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
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
                .add(branch_inst_retired_name, arm::EventMapper::get(CoreEvents::BRANCH_INST_RETIRED))
                .add(branch_misp_retired_name, arm::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
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
                .add(itlb_misses_name, arm::EventMapper::get(CoreEvents::ITLB_MISSES))
                .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
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
                .add(dtlb_misses_name, arm::EventMapper::get(CoreEvents::DTLB_MISSES))
                .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
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
                .add(itlb_misses_name, arm::EventMapper::get(CoreEvents::DTLB_MISSES))
                .add(dtlb_misses_name, arm::EventMapper::get(CoreEvents::ITLB_MISSES))
                .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
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
                .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(unhalted_core_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .build("IpC", [inst_retired_name, unhalted_core_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = get_event_count(counts,inst_retired_name);
                           uint64_t unhalted_core_cycles = get_event_count(counts,unhalted_core_cycles_name);

                           if (unhalted_core_cycles == 0)
                               return -1;
                            return static_cast<double>(inst_retired) / static_cast<double>(unhalted_core_cycles); });
        }

        static MetricBuilder ILP()
        {
            return {};
        }

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
            return {};
        }

        static MetricBuilder IpBranch()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string branch_inst_retired_name = to_string(CoreEvents::BRANCH_INST_RETIRED);
            return MetricBuilder{}
                .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(branch_inst_retired_name, arm::EventMapper::get(CoreEvents::BRANCH_INST_RETIRED))
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
        }

        static MetricBuilder IpMemStore()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string mem_store_retired_name = to_string(CoreEvents::MEM_STORE_RETIRED);
            return MetricBuilder{}
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
        }

        static MetricBuilder IpMispredict()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
            return MetricBuilder{}
                .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(branch_misp_retired_name, arm::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
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
            return {};
        }

        static MetricBuilder IpAVXAnyFlop()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string retired_sse_avx_flops_any_name = to_string(CoreEvents::RETIRED_VECTOR);
            return MetricBuilder{}
                .add(inst_retired_name, arm::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(retired_sse_avx_flops_any_name, arm::EventMapper::get(CoreEvents::RETIRED_VECTOR))
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
        }

        static MetricBuilder IpArith()
        {
            return {};
        }

        static MetricBuilder IpArithScalarSP()
        {
            return {};
        }

        static MetricBuilder IpArithScalarDP()
        {
            return {};
        }

        static MetricBuilder IpArithAVX128()
        {
            return {};
        }

        static MetricBuilder IpArithAVX256()
        {
            return {};
        }

        static MetricBuilder IpArithAVX512()
        {
            return {};
        }

        static MetricBuilder IpArithVectorAny()
        {
            return {};
        }

        static MetricBuilder ScalarpArithVector()
        {
            return {};
        }

        // Software prefetch
        static MetricBuilder IpSWPF()
        {
            return {};
        }

        // Topdown (Pipeline Utilisation) Analysis L1

#if OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_V3 || OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N3
        static MetricBuilder FrontendBound()
        {
            std::string stall_slot_fe_name = to_string(arm::NativeEvents::STALL_SLOT_FRONTEND);
            std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string stall_frontend_flush_name = to_string(arm::NativeEvents::STALL_FRONTEND_FLUSH);
            return MetricBuilder{}
                .add(stall_slot_fe_name, arm::EventMapper::get(arm::NativeEvents::STALL_SLOT_FRONTEND))
                .add(unhalted_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(stall_frontend_flush_name, arm::EventMapper::get(arm::NativeEvents::STALL_FRONTEND_FLUSH))
                .build("FrontendBound__%",
                       [stall_slot_fe_name, unhalted_cycles_name, stall_frontend_flush_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t cycles = get_event_count(counts, unhalted_cycles_name);
                           uint64_t slots = SUPERSCALAR_WIDE * cycles;
                           uint64_t stall_slot_fe = get_event_count(counts, stall_slot_fe_name);
                           uint64_t stall_frontend_flush = get_event_count(counts, stall_frontend_flush_name);

                           return 100 * ((static_cast<double>(stall_slot_fe) / (static_cast<double>(slots))) - (static_cast<double>(stall_frontend_flush) / static_cast<double>(cycles)));
                       });
        }

#elif OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N1 || OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_V1 || OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N2 || OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_V2
        static MetricBuilder FrontendBound()
        {
            std::string stall_slot_fe_name = to_string(arm::NativeEvents::STALL_SLOT_FRONTEND);
            std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string branch_misp_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
            return MetricBuilder{}
                .add(stall_slot_fe_name, arm::EventMapper::get(arm::NativeEvents::STALL_SLOT_FRONTEND))
                .add(unhalted_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(branch_misp_name, arm::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                .build("FrontendBound__%",
                       [stall_slot_fe_name, unhalted_cycles_name, branch_misp_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t cycles = get_event_count(counts, unhalted_cycles_name);
                           uint64_t slots = SUPERSCALAR_WIDE * cycles;
                           uint64_t stall_slot_fe = get_event_count(counts, stall_slot_fe_name);
                           uint64_t branch_misp = get_event_count(counts, branch_misp_name);

                           return 100 * ((static_cast<double>(stall_slot_fe) / (static_cast<double>(slots))) - (static_cast<double>(branch_misp) / static_cast<double>(cycles)));
                       });

#else
        static MetricBuilder FrontendBound() { return {}; }
#endif

#if OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_V3 || OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N3
        static MetricBuilder BadSpeculation()
        {
            std::string stall_slots_name = to_string(arm::NativeEvents::STALL_SLOT);
            std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string retired_ops_name = to_string(arm::NativeEvents::RETIRED_OPS);
            std::string op_spec_name = to_string(arm::NativeEvents::OP_SPEC);
            std::string stall_flush_name = to_string(arm::NativeEvents::STALL_FRONTEND_FLUSH);

            return MetricBuilder{}
                .add(stall_slots_name, arm::EventMapper::get(arm::NativeEvents::STALL_SLOT))
                .add(unhalted_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(retired_ops_name, arm::EventMapper::get(arm::NativeEvents::RETIRED_OPS))
                .add(op_spec_name, arm::EventMapper::get(arm::NativeEvents::OP_SPEC))
                .add(stall_flush_name, arm::EventMapper::get(arm::NativeEvents::STALL_FRONTEND_FLUSH))
                .build("BadSpeculation__%",
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
        }
#elif OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N2 || OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_V2
            static MetricBuilder BadSpeculation()
            {
                std::string stall_slots_name = to_string(arm::NativeEvents::STALL_SLOT);
                std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string retired_ops_name = to_string(arm::NativeEvents::OP_RETIRED);
                std::string op_spec_name = to_string(arm::NativeEvents::OP_SPEC);
                std::string br_mispred_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);

                return MetricBuilder{}
                    .add(stall_slots_name, arm::EventMapper::get(arm::NativeEvents::STALL_SLOT))
                    .add(unhalted_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(retired_ops_name, arm::EventMapper::get(arm::NativeEvents::OP_RETIRED))
                    .add(op_spec_name, arm::EventMapper::get(arm::NativeEvents::OP_SPEC))
                    .add(br_mispred_name, arm::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                    .build("BadSpeculation__%",
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
                           });
            }
#elif OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_V1
        static MetricBuilder BadSpeculation()
        {
            std::string stall_slots_name = to_string(arm::NativeEvents::STALL_SLOT);
            std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string retired_ops_name = to_string(arm::NativeEvents::OP_RETIRED);
            std::string op_spec_name = to_string(arm::NativeEvents::OP_SPEC);

            return MetricBuilder{}
                .add(stall_slots_name, arm::EventMapper::get(arm::NativeEvents::STALL_SLOT))
                .add(unhalted_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(retired_ops_name, arm::EventMapper::get(arm::NativeEvents::OP_RETIRED))
                .add(op_spec_name, arm::EventMapper::get(arm::NativeEvents::OP_SPEC))
                .build("BadSpeculation__%",
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
        }
#else
        static MetricBuilder BadSpeculation() { return {}; }
#endif

#if !OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N1
        static MetricBuilder Retiring()
        {
            std::string stall_slots_name = to_string(arm::NativeEvents::STALL_SLOT);
            std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string retired_ops_name = to_string(arm::NativeEvents::OP_RETIRED);
            std::string op_spec_name = to_string(arm::NativeEvents::OP_SPEC);

            return MetricBuilder{}
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
        }
#else
            static MetricBuilder Retiring() { return {}; }
#endif

#if !OPTKIT_ENV_CPU_MICROARCH_NEOVERSE_N1
        static MetricBuilder BackendBound()
        {
            std::string stall_slot_backend_name = to_string(arm::NativeEvents::STALL_SLOT_BACKEND);
            std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);

            return MetricBuilder{}
                .add(stall_slot_backend_name, arm::EventMapper::get(arm::NativeEvents::STALL_SLOT_BACKEND))
                .add(unhalted_cycles_name, arm::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .build("BackendBound__%",
                       [stall_slot_backend_name, unhalted_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t cycles = get_event_count(counts, unhalted_cycles_name);
                           if (cycles == 0)
                               return std::numeric_limits<double>::quiet_NaN();

                           uint64_t stall_slot_backend = get_event_count(counts, stall_slot_backend_name);
                           double total_slots = cycles * SUPERSCALAR_WIDE;

                           return 100.0 * (static_cast<double>(stall_slot_backend) / total_slots);
                       });
        }
#else
            static MetricBuilder BackendBound() { return {}; }
#endif

        static MetricBuilder SMTContention(){ return {}; }


        // Topdown (Pipeline Utilisation) Analysis L1
        static MetricBuilder FrontendBound_Latency() { return {}; }
        static MetricBuilder FrontendBound_BW() { return {}; }
        static MetricBuilder BadSpeculation_Mispredicts() { return {}; }
        static MetricBuilder BadSpeculation_PipelineRestarts() { return {}; }
        static MetricBuilder BackendEndbound_Memory() { return {}; }
        static MetricBuilder BackendEndbound_CPU() { return {}; }
        static MetricBuilder Retiring_Fastpath() { return {}; }
        static MetricBuilder Retiring_Microcode() { return {}; }

        // Aggregated Metrics

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
            mb.add(FrontendBound_Latency());
            mb.add(FrontendBound_BW());
            mb.add(BadSpeculation_Mispredicts());
            mb.add(BadSpeculation_PipelineRestarts());
            mb.add(BackendEndbound_Memory());
            mb.add(BackendEndbound_CPU());
            mb.add(Retiring_Fastpath());
            mb.add(Retiring_Microcode());
            return mb;
        }

        static MetricBuilder AllTopdown()
        {
            MetricBuilder mb;
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