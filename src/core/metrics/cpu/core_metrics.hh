#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include "utils/metric_builder.hh"
namespace optkit::metrics::cpu
{
    /**
     * @class Metrics
     * @brief Interface for accessing CPU performance metrics. Each method returns a MetricBuilder<uint64_t> instance that defines a metric and its associated events.
     *
     * The actual implementation should reside in CPU vendor-specific modules, not in this interface. These methods are not purely abstract because a metric might be defined generically but not supported on a particular CPU. In such cases, the method may return an empty list.
     *
     * The metric formulas described in this documentation are pseudocode representations. The actual event names used in implementations may differ from those shown here.
     *
     * @note It is recommended to use the MetricBuilder<uint64_t> class to construct metrics, as it offers a flexible and architecture-agnostic way to define and compute metrics.
     * @note For performance reasons, implementations return references to `static const` MetricBuilder<uint64_t> instances, usually defined through static lambdas.
     * An example is shown below: This way, the metric is only built once and can be reused without reinitialization.
     *
     * static const MetricBuilder<uint64_t>& MyMetric()
     * {
     *     static const MetricBuilder<uint64_t> metric = [] {
     *         return MetricBuilder<uint64_t>{}
     *             .add("event_name", event_id)
     *             .build("MyMetric",
     *                    [](const std::unordered_map<std::string, uint64_t>& counts) -> double {
     *                        // Compute the metric value from event counts
     *                        return 0.0;
     *                    });
     *     }();
     *     return metric;
     * }
     */

    template <typename T>
    class CoreMetrics
    {
    public:
        // CPU Utilization
        static MetricBuilder<uint64_t> CPUMaxCapacityBasedUtilization() { return {}; } ///< 100 * (UNHALTED_CLK_CYCLES / (OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS * max_freq_khz * 1000  * duration_sec)))

        // Cache miss per kilo instruction (MPKI)
        static MetricBuilder<uint64_t> L1MPKI() { return {}; } ///< 1000 * L1_MISSES / INST_RETIRED -- L1 cache true misses per kilo instruction for retired demand loads.
        static MetricBuilder<uint64_t> L2MPKI() { return {}; } ///< 1000 * L2_MISSES / INST_RETIRED -- L2 cache true misses per kilo instruction for retired demand loads.
        static MetricBuilder<uint64_t> L3MPKI() { return {}; } ///< 1000 * L3_MISSES / INST_RETIRED -- L3 cache true misses per kilo instruction for retired demand loads.

        // Branch
        static MetricBuilder<uint64_t> BranchMisprRatio() { return {}; } ///< BR_MISP_RETIRED.ALL_BRANCHES / BR_INST_RETIRED.ALL_BRANCHES -- Ratio of all branches which mispredict

        // TLB MPKI metrics
        static MetricBuilder<uint64_t> ITLBMPKI() { return {}; } ///< 1000 * ITLB_MISSES.WALK_COMPLETED / INST_RETIRED -- ITLB miss per kilo instructions
        static MetricBuilder<uint64_t> DTLBMPKI() { return {}; } ///< 1000 * DTLB_MISSES.WALK_COMPLETED / INST_RETIRED -- DTLB miss per kilo instructions
        static MetricBuilder<uint64_t> TLBMPKI() { return {}; }  ///< 1000 * TLB_MISSES.WALK_COMPLETED / INST_RETIRED -- TLB miss per kilo instructions

        // Latency and parallelism metrics
        static MetricBuilder<uint64_t> LoadMissLatency() { return {}; } ///< L1D_PEND_MISS.PENDING / MEM_LD_COMPLETED.L1_MISS_ANY -- Average latency for L1 D-cache miss demand load operations (in core cycles)
        static MetricBuilder<uint64_t> ILP() { return {}; }             ///< UOPS_EXECUTED.THREAD / (is_smt_enabled? 2 : 1 ) * UOPS_EXECUTED.CORE_CYCLES_GE1 -- Instr. level parallelism per core (average number of µops executed when there is execution)
        static MetricBuilder<uint64_t> MLP() { return {}; }             ///< L1D_PEND_MISS.PENDING / L1D_PEND_MISS.PENDING_CYCLES -- Memory level parallelism per-thread (average number of L1 miss demand loads when there is at least one such miss.)

        // DRAM bandwidth
        static MetricBuilder<uint64_t> DRAMBandwidthGBs() { return {}; } ///< (64 * (RD + WR)) / (Time * 1GB)

        // Instruction per event
        static MetricBuilder<uint64_t> IpC() { return {}; }          ///< INST_RETIRED / UNHALTED_CLK_CYCLES  -- Instructions per cycle
        static MetricBuilder<uint64_t> IpCall() { return {}; }       ///< INST_RETIRED / BR_INST_RETIRED.NEAR_CALL  -- Instructions per near (function/method) call (lower number means higher occurrence rate) | in current systems all function calls are considered near.
        static MetricBuilder<uint64_t> IpBranch() { return {}; }     ///< INST_RETIRED / BR_INST_RETIRED.ALL_BRANCHES  -- Instructions per branch
        static MetricBuilder<uint64_t> IpMemLoad() { return {}; }    ///< INST_RETIRED / MEM_INST_RETIRED.ALL_LOADS_PS  -- Instructions per memory load instructions
        static MetricBuilder<uint64_t> IpMemStore() { return {}; }   ///< INST_RETIRED / MEM_INST_RETIRED.ALL_STORES_PS -- Instructions per memory store instructions
        static MetricBuilder<uint64_t> IpMispredict() { return {}; } ///< INST_RETIRED / BR_MISP_RETIRED.ALL_BRANCHES  -- Instructions per mispredictions

        // Floating-point operation metrics
        static MetricBuilder<uint64_t> IpFLOP() { return {}; }       ///< Instructions per FP operation  -- Instructions per floating point operations
        static MetricBuilder<uint64_t> IpAVXAnyFLOP() { return {}; } ///< INST_RETIRED / (RETIRED_SSE_AVX_FLOPS_ANY)  -- Instructions per vector floating point operations (not vector instructions, actual fp computation count)
        static MetricBuilder<uint64_t> GFLOPs() { return {}; }       ///< GFLOP/seconds (Performance) -- Floating point operations per second (FLOP/s)
        static MetricBuilder<uint64_t> AI() { return {}; }           ///< FLOP/Byte   (Arithmetic Intensity) -- Ratio of floating point operations to memory bandwidth

        // static MetricBuilder<uint64_t> IpArith() { return {}; }            ///< INST_RETIRED / (FP_ARITH_INST_RETIRED_SCALAR + FP_ARITH_INST_RETIRED_VECTOR)  -- Instructions per Scalar Float FP instructions (equals the scalar fp count)
        static MetricBuilder<uint64_t> IpArithScalarSP() { return {}; }    ///< INST_RETIRED / FP_ARITH_INST.SCALAR_SINGLE  -- Instructions per Scalar Double FP instructions (equals the scalar fp count)
        static MetricBuilder<uint64_t> IpArithScalarDP() { return {}; }    ///< INST_RETIRED / FP_ARITH_INST.SCALAR_DOUBLE -- Instructions per vector operation
        static MetricBuilder<uint64_t> IpArithAVX128() { return {}; }      ///< INST_RETIRED / (128B_PACKED_DOUBLE + 128B_PACKED_SINGLE) -- Instructions per vector operation
        static MetricBuilder<uint64_t> IpArithAVX256() { return {}; }      ///< INST_RETIRED / (256B_PACKED_DOUBLE + 256B_PACKED_SINGLE) -- Instructions per vector operation
        static MetricBuilder<uint64_t> IpArithAVX512() { return {}; }      ///< INST_RETIRED / (512B_PACKED_DOUBLE + 512B_PACKED_SINGLE) -- Instructions per vector operation
        static MetricBuilder<uint64_t> IpArithVectorAny() { return {}; }   ///< INST_RETIRED / (RETIRED_SSE_AVX_FLOPS_ANY) -- Instructions per vector operation
        static MetricBuilder<uint64_t> ScalarpArithVector() { return {}; } ///< SCALAR_FP / RETIRED_SSE_AVX_FLOPS_ANY -- FP operations per vector operation instruction

        // Software prefetch
        static MetricBuilder<uint64_t> IpSWPF() { return {}; } ///< INST_RETIRED / SW_PREFETCH_ACCESS.T0:u0xF -- instructions per software prefetch operation

        // Topdown (Pipeline Utilisation) Analysis L1
        static MetricBuilder<uint64_t> FrontendBound() { return {}; }  ///< IDQ_UOPS_NOT_DELIVERED.CORE / (4 * CPU_CLK_UNHALTED.THREAD) — Fraction of slots not delivered by frontend
        static MetricBuilder<uint64_t> BadSpeculation() { return {}; } ///< (BR_MISP_RETIRED.ALL_BRANCHES + MACHINE_CLEARS.COUNT) / (4 * CPU_CLK_UNHALTED.THREAD) — Fraction of slots wasted due to branch mispredicts or other speculation issues
        static MetricBuilder<uint64_t> BackendBound() { return {}; }   ///< BACKEND_BOUND.SLOTS / (4 * CPU_CLK_UNHALTED.THREAD) — Fraction of slots where backend was unable to accept uops
        static MetricBuilder<uint64_t> Retiring() { return {}; }       ///< UOPS_RETIRED.RETIRE_SLOTS / (4 * CPU_CLK_UNHALTED.THREAD) — Fraction of slots retired successfully (i.e., useful work done)
        static MetricBuilder<uint64_t> SMTContention() { return {}; }  ///< DISPATHC_SLOTS / (4 * CPU_CLK_UNHALTED.THREAD) — Fraction of unused dispatch slots because the other thread was selected

        // Topdown (Pipeline Utilisation) Analysis L1
        static MetricBuilder<uint64_t> FrontendBound_Latency() { return {}; }           ///< ICACHE.MISSES + ITLB_MISSES.STLB_HIT / IDQ_UOPS_NOT_DELIVERED.CORE — Portion of FrontendBound due to instruction cache or TLB latency
        static MetricBuilder<uint64_t> FrontendBound_BW() { return {}; }                ///< DECODE_STALL.CYCLES / IDQ_UOPS_NOT_DELIVERED.CORE — Portion of FrontendBound due to bandwidth limitations (decode/queue saturation)
        static MetricBuilder<uint64_t> BadSpeculation_Mispredicts() { return {}; }      ///< BR_MISP_RETIRED.ALL_BRANCHES / (4 * CPU_CLK_UNHALTED.THREAD) — Portion of BadSpeculation due to branch mispredicts
        static MetricBuilder<uint64_t> BadSpeculation_PipelineRestarts() { return {}; } ///< MACHINE_CLEARS.COUNT / (4 * CPU_CLK_UNHALTED.THREAD) — Portion of BadSpeculation due to pipeline clears (e.g., memory ordering violations)
        static MetricBuilder<uint64_t> BackendEndbound_Memory() { return {}; }          ///< MEM_BOUND / BACKEND_BOUND — Portion of BackendBound due to memory issues (DRAM, L3 misses, etc.)
        static MetricBuilder<uint64_t> BackendEndbound_CPU() { return {}; }             ///< CORE_BOUND / BACKEND_BOUND — Portion of BackendBound due to non-memory backend issues (e.g., execution unit contention)
        static MetricBuilder<uint64_t> Retiring_Fastpath() { return {}; }               ///< UOPS_RETIRED.RETIRE_SLOTS (from scalar/simple ops) / TotalSlots — Portion of Retiring that was serviced via fast-path execution
        static MetricBuilder<uint64_t> Retiring_Microcode() { return {}; }              ///< MICROCODE.SEQUENCER_UOPS / TotalSlots — Portion of Retiring that came from microcode assists or complex flows

        // Aggregated Metrics
        static MetricBuilder<uint64_t> AllMPKI() { return {}; }
        static MetricBuilder<uint64_t> AllCacheHitRatio() { return {}; }
        static MetricBuilder<uint64_t> AllSTLBMPKI() { return {}; }
        static MetricBuilder<uint64_t> AllLatencyAndParallelism() { return {}; }
        static MetricBuilder<uint64_t> AllDRAMBandwidth() { return {}; }
        static MetricBuilder<uint64_t> AllIpMetrics() { return {}; }
        static MetricBuilder<uint64_t> AllBranchMetrics() { return {}; }

        static MetricBuilder<uint64_t> CARM() { return {}; }
        static MetricBuilder<uint64_t> TopdownL1() { return {}; }
        static MetricBuilder<uint64_t> TopdownL2_FE() { return {}; }
        static MetricBuilder<uint64_t> TopdownL2_BE() { return {}; }
        static MetricBuilder<uint64_t> TopdownL2_Retiring() { return {}; }
        static MetricBuilder<uint64_t> TopdownL2_BadSpec() { return {}; }
        static MetricBuilder<uint64_t> TopdownL2() { return {}; }
        static MetricBuilder<uint64_t> AllTopdown() { return {}; }

        static MetricBuilder<uint64_t> AllMetrics() { return {}; }

    private:
        CoreMetrics() {}
        ~CoreMetrics() {}
    };
} // namespace optkit::metrics::cpu