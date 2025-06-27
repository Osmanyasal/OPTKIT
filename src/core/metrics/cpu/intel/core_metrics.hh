#pragma once
#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_AMD // TODO: Change it to Intel

#include "core/metrics/metric_builder.hh"
#include "core/metrics/cpu/core_metrics.hh"
#include "core/metrics/cpu/intel/event_mapper.hh"
#include "core/metrics/cpu/intel/native_events.hh"

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
                .build("L1HitRatio",
                       [l1_hits_name, l1_misses_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t l1_hits = counts.at(l1_hits_name);
                           uint64_t l1_misses = counts.at(l1_misses_name);
                           uint64_t l1_cache_accesses = l1_hits + l1_misses;
                           // Avoid div by zero
                           if (l1_cache_accesses == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return 100 * (static_cast<double>(l1_hits) / static_cast<double>(l1_cache_accesses));
                       });
        } ///< (L2_Hits/L2_Accesses)
        // Native Metric implementations (not included in CoreMetrics)
        static MetricBuilder L2HitRatio()
        {
            std::string l2_misses_name = to_string(CoreEvents::L2_MISSES);
            std::string l2_hits_name = to_string(CoreEvents::L2_HITS);
            return MetricBuilder{}
                .add(l2_misses_name, intel::EventMapper::get(CoreEvents::L2_MISSES))
                .add(l2_hits_name, intel::EventMapper::get(CoreEvents::L2_HITS))
                .build("L2HitRatio",
                       [l2_hits_name, l2_misses_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t l2_hits = counts.at(l2_hits_name);
                           uint64_t l2_misses = counts.at(l2_misses_name);
                           uint64_t l2_cache_accesses = l2_hits + l2_misses;
                           // Avoid div by zero
                           if (l2_cache_accesses == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return 100 * (static_cast<double>(l2_hits) / static_cast<double>(l2_cache_accesses));
                       });
        } ///< (L2_Hits/L2_Accesses)

        static MetricBuilder L3HitRatio()
        {
            std::string l3_misses_name = to_string(CoreEvents::L3_MISSES);
            std::string l3_hits_name = to_string(CoreEvents::L3_HITS);
            return MetricBuilder{}
                .add(l3_misses_name, intel::EventMapper::get(CoreEvents::L3_MISSES))
                .add(l3_hits_name, intel::EventMapper::get(CoreEvents::L3_HITS))
                .build("L3HitRatio",
                       [l3_hits_name, l3_misses_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t l3_hits = counts.at(l3_hits_name);
                           uint64_t l3_misses = counts.at(l3_misses_name);
                           uint64_t l3_cache_accesses = l3_hits + l3_misses;
                           // Avoid div by zero
                           if (l3_cache_accesses == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return 100 * (static_cast<double>(l3_hits) / static_cast<double>(l3_cache_accesses));
                       });
        } ///< 1 - (L2_Misses/L3_Accesses)

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
                           uint64_t l1_misses = counts.at(l1_misses_name);
                           uint64_t inst_retired = counts.at(inst_retired_name);

                           // Avoid div by zero
                           if (inst_retired == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return 1000.0 * static_cast<double>(l1_misses) / static_cast<double>(inst_retired);
                       });

        } ///< 1000 * L1_MISSES / INST_RETIRED

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
                           uint64_t l2_misses = counts.at(l2_misses_name);
                           uint64_t inst_retired = counts.at(inst_retired_name);

                           // Avoid div by zero
                           if (inst_retired == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return 1000.0 * static_cast<double>(l2_misses) / static_cast<double>(inst_retired);
                       });
        } ///< 1000 * L2_MISSES / INST_RETIRED

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
                           uint64_t l3_misses = counts.at(l3_misses_name);
                           uint64_t inst_retired = counts.at(inst_retired_name);

                           // Avoid div by zero
                           if (inst_retired == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return 1000.0 * static_cast<double>(l3_misses) / static_cast<double>(inst_retired);
                       });
        } ///< 1000 * L3_MISSES / INST_RETIRED

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
                           uint64_t branch_misp = counts.at(branch_misp_retired_name);
                           uint64_t branch_inst = counts.at(branch_inst_retired_name);

                           // Avoid div by zero
                           if (branch_inst == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(branch_misp) / static_cast<double>(branch_inst);
                       });
        } ///< BR_MISP_RETIRED.ALL_BRANCHES / BR_INST_RETIRED.ALL_BRANCHES

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
                           uint64_t itlb_misses = counts.at(itlb_misses_name);
                           uint64_t inst_retired = counts.at(inst_retired_name);

                           if (inst_retired == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return 1000.0 * static_cast<double>(itlb_misses) / static_cast<double>(inst_retired);
                       });
        } ///< 1000 * ITLB_MISSES / INST_RETIRED

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
                           uint64_t itlb_misses = counts.at(dtlb_misses_name);
                           uint64_t inst_retired = counts.at(inst_retired_name);

                           if (inst_retired == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return 1000.0 * static_cast<double>(itlb_misses) / static_cast<double>(inst_retired);
                       });
        } ///< 1000 * DTLB_MISSES / INST_RETIRED

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
                           uint64_t itlb_misses = counts.at(itlb_misses_name);
                           uint64_t dtlb_misses = counts.at(dtlb_misses_name);
                           uint64_t inst_retired = counts.at(inst_retired_name);

                           if (inst_retired == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return 1000.0 * (static_cast<double>(dtlb_misses) + static_cast<double>(itlb_misses)) / static_cast<double>(inst_retired);
                       });
        } ///< 1000 * DTLB_MISSES.WALK_COMPLETED / INST_RETIRED

        // Latency and parallelism metrics
        static MetricBuilder LoadMissLatency()
        {
            return {};
        } ///< L1D_PEND_MISS.PENDING / MEM_LD_COMPLETED.L1_MISS_ANY

        static MetricBuilder IpC()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string unhalted_core_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);

            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(unhalted_core_cycles_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .build("IpC", [inst_retired_name, unhalted_core_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t unhalted_core_cycles = counts.at(unhalted_core_cycles_name);

                           if (unhalted_core_cycles == 0)
                               return -1;
                            return static_cast<double>(inst_retired) / static_cast<double>(unhalted_core_cycles); });
        } ///< INST_RETIRED / UNHALTED_CLK_CYCLES

        static MetricBuilder ILP()
        {
            return {};
        } ///< UOPS_EXECUTED.THREAD / UOPS_EXECUTED.CORE_CYCLES_GE1

        static MetricBuilder MLP()
        {
            return {};
        } ///< L1D_PEND_MISS.PENDING / L1D_PEND_MISS.PENDING_CYCLES

        // DRAM bandwidth
        static MetricBuilder DRAMBandwidthGBs()
        {
            return {};
        } ///< (64 * (RD + WR)) / (Time * 1GB)

        // Instruction per event
        static MetricBuilder IpCall()
        {
            return {};
        } ///< INST_RETIRED / BR_INST_RETIRED.NEAR_CALL

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
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t branch_inst_retired = counts.at(branch_inst_retired_name);

                           // Avoid div by zero
                           if (branch_inst_retired == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(branch_inst_retired);
                       });
        } ///< INST_RETIRED / BR_INST_RETIRED.ALL_BRANCHES

        static MetricBuilder IpLoad()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string mem_load_retired_name = to_string(CoreEvents::MEM_LOAD_RETIRED);
            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(mem_load_retired_name, intel::EventMapper::get(CoreEvents::MEM_LOAD_RETIRED))
                .build("IpLoad",
                       [mem_load_retired_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t mem_load_retired = counts.at(mem_load_retired_name);

                           // Avoid div by zero
                           if (mem_load_retired == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(mem_load_retired);
                       });
        } ///< INST_RETIRED / MEM_INST_RETIRED.ALL_LOADS_PS

        static MetricBuilder IpStore()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string mem_store_retired_name = to_string(CoreEvents::MEM_STORE_RETIRED);
            return MetricBuilder{}
                .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                .add(mem_store_retired_name, intel::EventMapper::get(CoreEvents::MEM_STORE_RETIRED))
                .build("IpStore",
                       [mem_store_retired_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t mem_store_retired = counts.at(mem_store_retired_name);

                           // Avoid div by zero
                           if (mem_store_retired == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(mem_store_retired);
                       });
        } ///< INST_RETIRED / MEM_INST_RETIRED.ALL_STORES_PS

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
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t branch_misp_retired = counts.at(branch_misp_retired_name);

                           // Avoid div by zero
                           if (branch_misp_retired == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(branch_misp_retired);
                       });
        } ///< INST_RETIRED / BR_MISP_RETIRED.ALL_BRANCHES

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
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t retired_scalar_sp_any = counts.at(retired_scalar_sp_any_name);
                           uint64_t retired_scalar_dp_any = counts.at(retired_scalar_dp_any_name);
                           uint64_t inst_packed_128_double = counts.at(inst_packed_128_double_name);
                           uint64_t inst_packed_128_single = counts.at(inst_packed_128_single_name);
                           uint64_t inst_packed_256_double = counts.at(inst_packed_256_double_name);
                           uint64_t inst_packed_256_single = counts.at(inst_packed_256_single_name);
                           uint64_t inst_packed_512_double = counts.at(inst_packed_512_double_name);
                           uint64_t inst_packed_512_single = counts.at(inst_packed_512_single_name);

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
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(total_flops);
                       });
        } ///< Instructions per FP operation

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
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t inst_packed_128_double = counts.at(inst_packed_128_double_name);
                           uint64_t inst_packed_128_single = counts.at(inst_packed_128_single_name);
                           uint64_t inst_packed_256_double = counts.at(inst_packed_256_double_name);
                           uint64_t inst_packed_256_single = counts.at(inst_packed_256_single_name);
                           uint64_t inst_packed_512_double = counts.at(inst_packed_512_double_name);
                           uint64_t inst_packed_512_single = counts.at(inst_packed_512_single_name);

                           uint64_t total_flops =
                               (inst_packed_128_double * 2) +
                               (inst_packed_128_single * 4) +
                               (inst_packed_256_double * 4) +
                               (inst_packed_256_single * 8) +
                               (inst_packed_512_double * 8) +
                               (inst_packed_512_single * 16);

                           // Avoid div by zero
                           if (total_flops == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(total_flops);
                       });

        } ///< INST_RETIRED / (RETIRED_SSE_AVX_FLOPS_ANY)

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
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t inst_retired_scalar = counts.at(inst_retired_scalar_name);
                           uint64_t inst_retired_vector = counts.at(inst_retired_vector_name);
                           uint64_t arith = inst_retired / (inst_retired_scalar + inst_retired_vector);
                           // Avoid div by zero
                           if (inst_retired == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(inst_retired);
                       });
        } ///< INST_RETIRED / (FP_ARITH_INST_RETIRED_SCALAR + FP_ARITH_INST_RETIRED_VECTOR)

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
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t inst_scalar_single = counts.at(inst_scalar_single_name);

                           // Avoid div by zero
                           if (inst_scalar_single == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(inst_scalar_single);
                       });
        } ///< INST_RETIRED / FP_ARITH_INST_RETIRED_SCALAR_SINGLE

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
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t inst_scalar_double = counts.at(inst_scalar_double_name);

                           // Avoid div by zero
                           if (inst_scalar_double == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(inst_scalar_double);
                       });
        } ///< INST_RETIRED / FP_ARITH_INST_RETIRED_SCALAR_DOUBLE

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
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t inst_packed_128_double = counts.at(inst_packed_128_double_name);
                           uint64_t inst_packed_128_single = counts.at(inst_packed_128_single_name);

                           uint64_t inst_packed_128 = inst_packed_128_double + inst_packed_128_single;
                           // Avoid div by zero
                           if (inst_packed_128 == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(inst_packed_128);
                       });
        } ///< INST_RETIRED / (128B_PACKED_DOUBLE + 128B_PACKED_SINGLE)

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
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t inst_packed_256_double = counts.at(inst_packed_256_double_name);
                           uint64_t inst_packed_256_single = counts.at(inst_packed_256_single_name);

                           uint64_t inst_packed_256 = inst_packed_256_double + inst_packed_256_single;
                           // Avoid div by zero
                           if (inst_packed_256 == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(inst_packed_256);
                       });

        } ///< INST_RETIRED / (256B_PACKED_DOUBLE + 256B_PACKED_SINGLE)

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
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t inst_packed_512_double = counts.at(inst_packed_512_double_name);
                           uint64_t inst_packed_512_single = counts.at(inst_packed_512_single_name);

                           uint64_t inst_packed_512 = inst_packed_512_double + inst_packed_512_single;
                           // Avoid div by zero
                           if (inst_packed_512 == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(inst_packed_512);
                       });

        } ///< INST_RETIRED / (512B_PACKED_DOUBLE + 512B_PACKED_SINGLE)

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
                           uint64_t inst_vector = counts.at(inst_vector_name);
                           uint64_t inst_scalar = counts.at(inst_scalar_name);
                           // Avoid div by zero
                           if (inst_vector == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_scalar) / static_cast<double>(inst_vector);
                       });

        } ///< INST_RETIRED / (RETIRED_SSE_AVX_FLOPS_ANY)

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
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t inst_packed_128_double = counts.at(inst_packed_128_double_name);
                           uint64_t inst_packed_128_single = counts.at(inst_packed_128_single_name);
                           uint64_t inst_packed_256_double = counts.at(inst_packed_256_double_name);
                           uint64_t inst_packed_256_single = counts.at(inst_packed_256_single_name);
                           uint64_t inst_packed_512_double = counts.at(inst_packed_512_double_name);
                           uint64_t inst_packed_512_single = counts.at(inst_packed_512_single_name);

                           uint64_t total_avx_instr =
                               (inst_packed_128_double) +
                               (inst_packed_128_single) +
                               (inst_packed_256_double) +
                               (inst_packed_256_single) +
                               (inst_packed_512_double) +
                               (inst_packed_512_single);

                           // Avoid div by zero
                           if (total_avx_instr == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(total_avx_instr);
                       });

        } ///< INST_RETIRED / (RETIRED_SSE_AVX_FLOPS_ANY)

        static MetricBuilder ScalarpArithVector()
        {
            return {};
        } ///< SCALAR_FP / RETIRED_SSE_AVX_INSTR_ANY

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
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t sw_load_prefetch = counts.at(sw_load_prefetch_name);

                           // Avoid div by zero
                           if (sw_load_prefetch == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(sw_load_prefetch);
                       });
        } ///< INST_RETIRED / SW_PREFETCH_ACCESS.T0:u0xF

#if OPTKIT_ENV_CPU_MICROARCH_ZEN4 || OPTKIT_ENV_CPU_MICROARCH_SPR || OPTKIT_ENV_CPU_MICROARCH_SKL || OPTKIT_ENV_CPU_MICROARCH_ICL || OPTKIT_ENV_CPU_MICROARCH_HSW
        // Topdown (Pipeline Utilisation) Analysis L1
        static MetricBuilder FrontendBound()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string no_ops_from_frontend_name = to_string(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE);
            return MetricBuilder{}
                .add(no_ops_from_frontend_name, intel::EventMapper::get(cpu::intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE))
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .build("FrontendBound",
                       [dispatch_slots_name, no_ops_from_frontend_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t no_ops_from_frontend = counts.at(no_ops_from_frontend_name);
                           uint64_t dispatch_slots = 4 * counts.at(dispatch_slots_name);

                           // Avoid div by zero
                           if (dispatch_slots == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return 100 * static_cast<double>(no_ops_from_frontend) / (static_cast<double>(dispatch_slots));
                       });
        } ///< IDQ_UOPS_NOT_DELIVERED.CORE / (4 *CPU_CLK_UNHALTED.THREAD) — Fraction of slots not delivered by frontend

        static MetricBuilder BadSpeculation()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string uops_issued_name = to_string(intel::NativeEvents::UOPS_ISSUED);
            std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);

            return MetricBuilder{}
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(uops_issued_name, intel::EventMapper::get(intel::NativeEvents::UOPS_ISSUED))
                .add(uops_retired_slots_name, intel::EventMapper::get(cpu::intel::NativeEvents::UOPS_RETIRED_SLOTS))
                .build("BadSpeculation",
                       [dispatch_slots_name, uops_issued_name, uops_retired_slots_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t dispatch_slots = 4 * counts.at(dispatch_slots_name);
                           uint64_t uops_issued = counts.at(uops_issued_name);
                           uint64_t uops_retired_slots = counts.at(uops_retired_slots_name);

                           // Avoid div by zero
                           if (dispatch_slots == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return 100 * (static_cast<double>(uops_issued) - static_cast<double>(uops_retired_slots) + 4 * static_cast<double>(uops_retired_slots)) / (static_cast<double>(dispatch_slots));
                       });

        } ///< (OPS_SOURCE_DISPATCHED_FROM_DECODER - RETIRED_OPS) / (4 *CPU_CLK_UNHALTED.THREAD) — Fraction of dispatched ops that did not retire.

        static MetricBuilder Retiring()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);

            return MetricBuilder{}
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(uops_retired_slots_name, intel::EventMapper::get(intel::NativeEvents::UOPS_RETIRED_SLOTS))
                .build("Retiring",
                       [dispatch_slots_name, uops_retired_slots_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t dispatch_slots = 4 * counts.at(dispatch_slots_name);
                           uint64_t uops_retired_slots = counts.at(uops_retired_slots_name);

                           // Avoid div by zero
                           if (dispatch_slots == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return 100 * (static_cast<double>(uops_retired_slots)) / (static_cast<double>(dispatch_slots));
                       });
        } ///< RETIRED_OPS / (4 *CPU_CLK_UNHALTED.THREAD) — Fraction of slots retired successfully (i.e., useful work done)

        static MetricBuilder BackendBound()
        {

            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string no_ops_from_frontend_name = to_string(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE);
            std::string uops_issued_name = to_string(intel::NativeEvents::UOPS_ISSUED);
            std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);

            return MetricBuilder{}
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(no_ops_from_frontend_name, intel::EventMapper::get(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE))
                .add(uops_issued_name, intel::EventMapper::get(intel::NativeEvents::UOPS_ISSUED))
                .add(uops_retired_slots_name, intel::EventMapper::get(intel::NativeEvents::UOPS_RETIRED_SLOTS))
                .build("BackendBound",
                       [dispatch_slots_name, no_ops_from_frontend_name, uops_issued_name, uops_retired_slots_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t dispatch_slots = 4 * counts.at(dispatch_slots_name);
                           uint64_t no_ops_from_frontend = counts.at(no_ops_from_frontend_name);
                           uint64_t uops_issued = counts.at(uops_issued_name);
                           uint64_t uops_retired_slots = counts.at(uops_retired_slots_name);

                            double retiring = static_cast<double>(uops_retired_slots) / (static_cast<double>(dispatch_slots));
                            double frontend_bound = static_cast<double>(no_ops_from_frontend) / (static_cast<double>(dispatch_slots));
                            double bad_speculation = (static_cast<double>(uops_issued) - static_cast<double>(uops_retired_slots) + 4 * static_cast<double>(uops_retired_slots)) / (static_cast<double>(dispatch_slots));
                            // Avoid div by zero
                            if (dispatch_slots == 0)
                                std::numeric_limits<double>::quiet_NaN();
                            return 100 * (1 - (frontend_bound + bad_speculation + retiring));
                       });
        } ///< (1 - (frontend_bound + bad_speculation + retiring));

        static MetricBuilder SMTContention()
        {
            // std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            // std::string smt_stalls_name = to_string(intel::NativeEvents::SMT_STALLS_1);
            // return MetricBuilder{}
            //     .add(smt_stalls_name, intel::EventMapper::get(cpu::intel::NativeEvents::SMT_STALLS_1))
            //     .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
            //     .build("SMTContention",
            //            [dispatch_slots_name, smt_stalls_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
            //            {
            //                uint64_t smt_stalls = counts.at(smt_stalls_name);
            //                uint64_t dispatch_slots = 4 * counts.at(dispatch_slots_name);

            //                // Avoid div by zero
            //                if (dispatch_slots == 0)
            //                    std::numeric_limits<double>::quiet_NaN();
            //                return 100 * static_cast<double>(smt_stalls) / (static_cast<double>(dispatch_slots));
            //            });
            return {};
        } ///< SMT_STALLS / (4 *CPU_CLK_UNHALTED.THREAD) — Fraction of unused dispatch slots because the other thread was selected

        // Topdown (Pipeline Utilisation) Analysis L1
        static MetricBuilder FrontendBound_Latency()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string no_ops_from_frontend_0x6flag_name = to_string(intel::NativeEvents::DISPATCH_STALLS_1_0x6);
            return MetricBuilder{}
                .add(no_ops_from_frontend_0x6flag_name, intel::EventMapper::get(cpu::intel::NativeEvents::DISPATCH_STALLS_1_0x6))
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .build("FrontendBound_Latency",
                       [dispatch_slots_name, no_ops_from_frontend_0x6flag_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t no_ops_from_frontend_0x6flag = 4 * counts.at(no_ops_from_frontend_0x6flag_name); // this is latency specific. it is dispatch_stalls/dispatch_slots no multiply with cpu wide.
                           uint64_t dispatch_slots = 4 * counts.at(dispatch_slots_name);

                           // Avoid div by zero
                           if (dispatch_slots == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return 100 * static_cast<double>(no_ops_from_frontend_0x6flag) / (static_cast<double>(dispatch_slots));
                       });
        } ///< Fraction of dispatch slots that remained unused because of a latency bottleneck in the frontend, such as Instruction Cache or ITLB misses.

        static MetricBuilder FrontendBound_BW()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string no_ops_from_frontend_name = to_string(intel::NativeEvents::DISPATCH_STALLS_1);
            std::string no_ops_from_frontend_0x6flag_name = to_string(intel::NativeEvents::DISPATCH_STALLS_1_0x6);
            return MetricBuilder{}
                .add(no_ops_from_frontend_name, intel::EventMapper::get(cpu::intel::NativeEvents::DISPATCH_STALLS_1))
                .add(no_ops_from_frontend_0x6flag_name, intel::EventMapper::get(cpu::intel::NativeEvents::DISPATCH_STALLS_1_0x6))
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .build("FrontendBound_BW",
                       [dispatch_slots_name, no_ops_from_frontend_name, no_ops_from_frontend_0x6flag_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t no_ops_from_frontend = counts.at(no_ops_from_frontend_name); // this is latency specific. it is backend_stalls/dispatch_slots no multiply with cpu wide.
                           uint64_t no_ops_from_frontend_0x6flag = 4 * counts.at(no_ops_from_frontend_0x6flag_name);
                           uint64_t dispatch_slots = 4 * counts.at(dispatch_slots_name);

                           // Avoid div by zero
                           if (dispatch_slots == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return 100 * (static_cast<double>(no_ops_from_frontend) - static_cast<double>(no_ops_from_frontend_0x6flag)) / (static_cast<double>(dispatch_slots));
                       });
        } ///< Fraction of dispatch slots that remained unused because of a bandwidth bottleneck in the frontend, such as decode bandwidth or Op Cache fetch bandwidth.
        static MetricBuilder BadSpeculation_Mispredicts()
        {

            std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
            std::string resyncs_name = to_string(intel::NativeEvents::RESYNCS);
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string retired_ops_name = to_string(intel::NativeEvents::RETIRED_OPS);
            std::string ops_source_dispatched_from_decoder_name = to_string(intel::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER);

            return MetricBuilder{}
                .add(branch_misp_retired_name, intel::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(resyncs_name, intel::EventMapper::get(intel::NativeEvents::RESYNCS))
                .add(retired_ops_name, intel::EventMapper::get(cpu::intel::NativeEvents::RETIRED_OPS))
                .add(ops_source_dispatched_from_decoder_name, intel::EventMapper::get(cpu::intel::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER))
                .build("BadSpeculation_Mispredicts",
                       [branch_misp_retired_name, dispatch_slots_name, resyncs_name, retired_ops_name, ops_source_dispatched_from_decoder_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t branch_misp_retired = counts.at(branch_misp_retired_name);
                           uint64_t dispatch_slots = 4 * counts.at(dispatch_slots_name);
                           uint64_t resyncs = counts.at(resyncs_name);
                           uint64_t retired_ops = counts.at(retired_ops_name);
                           uint64_t ops_source_dispatched_from_decoder = counts.at(ops_source_dispatched_from_decoder_name);

                           // Avoid div by zero
                           if (dispatch_slots == 0 || (branch_misp_retired + resyncs) == 0)
                               std::numeric_limits<double>::quiet_NaN();

                           double bad_speculation = (static_cast<double>(ops_source_dispatched_from_decoder) - static_cast<double>(retired_ops)) / (static_cast<double>(dispatch_slots));
                           return 100 * (bad_speculation * branch_misp_retired) / (branch_misp_retired + resyncs);
                       });

        } ///< Fraction of dispatched ops that were flushed due to branch mispredicts
        static MetricBuilder BadSpeculation_PipelineRestarts()
        {

            std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string resyncs_name = to_string(intel::NativeEvents::RESYNCS);
            std::string retired_ops_name = to_string(intel::NativeEvents::RETIRED_OPS);
            std::string ops_source_dispatched_from_decoder_name = to_string(intel::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER);

            return MetricBuilder{}
                .add(branch_misp_retired_name, intel::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(resyncs_name, intel::EventMapper::get(intel::NativeEvents::RESYNCS))
                .add(retired_ops_name, intel::EventMapper::get(cpu::intel::NativeEvents::RETIRED_OPS))
                .add(ops_source_dispatched_from_decoder_name, intel::EventMapper::get(cpu::intel::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER))
                .build("BadSpeculation_PipelineRestarts",
                       [branch_misp_retired_name, dispatch_slots_name, resyncs_name, retired_ops_name, ops_source_dispatched_from_decoder_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t branch_misp_retired = counts.at(branch_misp_retired_name);
                           uint64_t dispatch_slots = 4 * counts.at(dispatch_slots_name);
                           uint64_t resyncs = counts.at(resyncs_name);
                           uint64_t retired_ops = counts.at(retired_ops_name);
                           uint64_t ops_source_dispatched_from_decoder = counts.at(ops_source_dispatched_from_decoder_name);

                           // Avoid div by zero
                           if (dispatch_slots == 0 || (branch_misp_retired + resyncs) == 0)
                               std::numeric_limits<double>::quiet_NaN();

                           double bad_speculation = (static_cast<double>(ops_source_dispatched_from_decoder) - static_cast<double>(retired_ops)) / (static_cast<double>(dispatch_slots));
                           return 100 * (bad_speculation * resyncs) / (branch_misp_retired + resyncs);
                       });
        } ///< Fraction of dispatched ops that were flushed due to pipeline restarts (resyncs).

        static MetricBuilder BackendEndbound_Memory()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string backend_stalls_name = to_string(intel::NativeEvents::BACKEND_STALLS_1);
            std::string cycles_no_retire_not_complete_name = to_string(intel::NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE);
            std::string cycles_no_retire_load_not_complete_name = to_string(intel::NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE);

            return MetricBuilder{}
                .add(backend_stalls_name, intel::EventMapper::get(cpu::intel::NativeEvents::BACKEND_STALLS_1))
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(cycles_no_retire_not_complete_name, intel::EventMapper::get(cpu::intel::NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE))
                .add(cycles_no_retire_load_not_complete_name, intel::EventMapper::get(cpu::intel::NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE))
                .build("BackendEndbound_Memory",
                       [dispatch_slots_name, backend_stalls_name, cycles_no_retire_not_complete_name, cycles_no_retire_load_not_complete_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t backend_stalls = counts.at(backend_stalls_name);
                           uint64_t dispatch_slots = 4 * counts.at(dispatch_slots_name);
                           uint64_t cycles_no_retire_not_complete = counts.at(cycles_no_retire_not_complete_name);
                           uint64_t cycles_no_retire_load_not_complete = counts.at(cycles_no_retire_load_not_complete_name);

                           // Avoid div by zero
                           if (dispatch_slots == 0 || cycles_no_retire_load_not_complete == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           double backend_bound = static_cast<double>(backend_stalls) / (static_cast<double>(dispatch_slots));
                           return 100 * backend_bound * (static_cast<double>(cycles_no_retire_not_complete) / static_cast<double>(cycles_no_retire_load_not_complete));
                       });
        } ///< Fraction of dispatched slots that remained unused because of stalls due to the memory subsystem.

        static MetricBuilder BackendEndbound_CPU()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string backend_stalls_name = to_string(intel::NativeEvents::BACKEND_STALLS_1);
            std::string cycles_no_retire_not_complete_name = to_string(intel::NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE);
            std::string cycles_no_retire_load_not_complete_name = to_string(intel::NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE);

            return MetricBuilder{}
                .add(backend_stalls_name, intel::EventMapper::get(cpu::intel::NativeEvents::BACKEND_STALLS_1))
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(cycles_no_retire_not_complete_name, intel::EventMapper::get(cpu::intel::NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE))
                .add(cycles_no_retire_load_not_complete_name, intel::EventMapper::get(cpu::intel::NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE))
                .build("BackendEndbound_CPU",
                       [dispatch_slots_name, backend_stalls_name, cycles_no_retire_not_complete_name, cycles_no_retire_load_not_complete_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t backend_stalls = counts.at(backend_stalls_name);
                           uint64_t dispatch_slots = 4 * counts.at(dispatch_slots_name);
                           uint64_t cycles_no_retire_not_complete = counts.at(cycles_no_retire_not_complete_name);
                           uint64_t cycles_no_retire_load_not_complete = counts.at(cycles_no_retire_load_not_complete_name);

                           // Avoid div by zero
                           if (dispatch_slots == 0 || cycles_no_retire_load_not_complete == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           double backend_bound = static_cast<double>(backend_stalls) / (static_cast<double>(dispatch_slots));
                           return 100 * backend_bound * (1.0 - (static_cast<double>(cycles_no_retire_not_complete) / static_cast<double>(cycles_no_retire_load_not_complete)));
                       });

        } ///< CORE_BOUND / BACKEND_BOUND — Portion of BackendBound due to non-memory backend issues (e.g., execution unit contention)
        static MetricBuilder Retiring_Fastpath()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string retired_microcode_ops_name = to_string(intel::NativeEvents::RETIRED_MICROCODE_OPS);
            std::string retired_ops_name = to_string(intel::NativeEvents::RETIRED_OPS);

            return MetricBuilder{}
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(retired_microcode_ops_name, intel::EventMapper::get(intel::NativeEvents::RETIRED_MICROCODE_OPS))
                .add(retired_ops_name, intel::EventMapper::get(cpu::intel::NativeEvents::RETIRED_OPS))
                .build("Retiring_Fastpath",
                       [dispatch_slots_name, retired_microcode_ops_name, retired_ops_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t dispatch_slots = 4 * counts.at(dispatch_slots_name);
                           uint64_t retired_ops = counts.at(retired_ops_name);
                           uint64_t retired_microcode_ops = counts.at(retired_microcode_ops_name);

                           // Avoid div by zero
                           if (retired_ops == 0 || dispatch_slots)
                               std::numeric_limits<double>::quiet_NaN();
                           double retiring = (static_cast<double>(retired_ops)) / (static_cast<double>(dispatch_slots));

                           return 100 * retiring * (retired_ops - retired_microcode_ops) / retired_ops;
                       });
        } ///< Fraction of dispatch slots used by fastpath ops that retired
        static MetricBuilder Retiring_Microcode()
        {
            std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            std::string retired_microcode_ops_name = to_string(intel::NativeEvents::RETIRED_MICROCODE_OPS);
            std::string retired_ops_name = to_string(intel::NativeEvents::RETIRED_OPS);

            return MetricBuilder{}
                .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                .add(retired_microcode_ops_name, intel::EventMapper::get(intel::NativeEvents::RETIRED_MICROCODE_OPS))
                .add(retired_ops_name, intel::EventMapper::get(cpu::intel::NativeEvents::RETIRED_OPS))
                .build("Retiring_Microcode",
                       [dispatch_slots_name, retired_microcode_ops_name, retired_ops_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t dispatch_slots = 4 * counts.at(dispatch_slots_name);
                           uint64_t retired_ops = counts.at(retired_ops_name);
                           uint64_t retired_microcode_ops = counts.at(retired_microcode_ops_name);

                           // Avoid div by zero
                           if (retired_ops == 0 || dispatch_slots)
                               std::numeric_limits<double>::quiet_NaN();
                           double retiring = (static_cast<double>(retired_ops)) / (static_cast<double>(dispatch_slots));

                           return 100 * retiring * retired_microcode_ops / retired_ops;
                       });
        } ///< Fraction of dispatch slots used by microcode ops that retired.

#else
        // Topdown (Pipeline Utilisation) Analysis L1
        static MetricBuilder FrontendBound() { return {}; } ///< IDQ_UOPS_NOT_DELIVERED.CORE / (4 *CPU_CLK_UNHALTED.THREAD) — Fraction of slots not delivered by frontend

        static MetricBuilder BadSpeculation() { return {}; } ///< (BR_MISP_RETIRED.ALL_BRANCHES + MACHINE_CLEARS.COUNT) / (4 *CPU_CLK_UNHALTED.THREAD) — Fraction of slots wasted due to branch mispredicts or other speculation issues
        static MetricBuilder BackendBound() { return {}; }   ///< BACKEND_BOUND.SLOTS / (4 *CPU_CLK_UNHALTED.THREAD) — Fraction of slots where backend was unable to accept uops
        static MetricBuilder Retiring() { return {}; }       ///< UOPS_RETIRED.RETIRE_SLOTS / (4 *CPU_CLK_UNHALTED.THREAD) — Fraction of slots retired successfully (i.e., useful work done)
        static MetricBuilder SMTContention() { return {}; }  ///< DISPATHC_SLOTS / (4 *CPU_CLK_UNHALTED.THREAD) — Fraction of unused dispatch slots because the other thread was selected

        // Topdown (Pipeline Utilisation) Analysis L1
        static MetricBuilder FrontendBound_Latency() { return {}; }           ///< ICACHE.MISSES + ITLB_MISSES.STLB_HIT / IDQ_UOPS_NOT_DELIVERED.CORE — Portion of FrontendBound due to instruction cache or TLB latency
        static MetricBuilder FrontendBound_BW() { return {}; }                ///< DECODE_STALL.CYCLES / IDQ_UOPS_NOT_DELIVERED.CORE — Portion of FrontendBound due to bandwidth limitations (decode/queue saturation)
        static MetricBuilder BadSpeculation_Mispredicts() { return {}; }      ///< BR_MISP_RETIRED.ALL_BRANCHES / (4 *CPU_CLK_UNHALTED.THREAD) — Portion of BadSpeculation due to branch mispredicts
        static MetricBuilder BadSpeculation_PipelineRestarts() { return {}; } ///< MACHINE_CLEARS.COUNT / (4 *CPU_CLK_UNHALTED.THREAD) — Portion of BadSpeculation due to pipeline clears (e.g., memory ordering violations)
        static MetricBuilder BackendEndbound_Memory() { return {}; }          ///< MEM_BOUND / BACKEND_BOUND — Portion of BackendBound due to memory issues (DRAM, L3 misses, etc.)
        static MetricBuilder BackendEndbound_CPU() { return {}; }             ///< CORE_BOUND / BACKEND_BOUND — Portion of BackendBound due to non-memory backend issues (e.g., execution unit contention)
        static MetricBuilder Retiring_Fastpath() { return {}; }               ///< UOPS_RETIRED.RETIRE_SLOTS (from scalar/simple ops) / TotalSlots — Portion of Retiring that was serviced via fast-path execution
        static MetricBuilder Retiring_Microcode() { return {}; }              ///< MICROCODE.SEQUENCER_UOPS / TotalSlots — Portion of Retiring that came from microcode assists or complex flows
#endif
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
            mb.add(IpAVXAnyFLOP());
            mb.add(IpArithScalarSP());
            mb.add(IpArithScalarDP());
            mb.add(IpArithAVX128());
            mb.add(IpArithAVX256());
            mb.add(IpArithAVX512());
            mb.add(IpArithVectorAny());
            mb.add(ScalarpArithVector());
            mb.add(IpBranch());
            mb.add(IpLoad());
            mb.add(IpStore());
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