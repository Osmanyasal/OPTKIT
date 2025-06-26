#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include "core/metrics/metric_builder.hh"
namespace optkit::core::metrics::cpu
{
    /**
     * @class Metrics
     * @brief Interface for retrieving CPU performance metrics. Each metric method returns the set of events required for its computation.
     *
     * Implementation should take place in cpu vendors not here. The reason these are not fully abstract is that it is possible metric is generic but not exist in any cpu.
     * So it returns empty list.
     */
    template <typename T>
    class CoreMetrics
    {
    public:
        // Cache miss per kilo instruction (MPKI)
        static MetricBuilder L1MPKI() { return {}; } ///< 1000 * L1_MISSES / INST_RETIRED
        static MetricBuilder L2MPKI() { return {}; } ///< 1000 * L2_MISSES / INST_RETIRED
        static MetricBuilder L3MPKI() { return {}; } ///< 1000 * L3_MISSES / INST_RETIRED

        // Branch
        static MetricBuilder BranchMisprRatio() { return {}; } ///< BR_MISP_RETIRED.ALL_BRANCHES / BR_INST_RETIRED.ALL_BRANCHES

        // TLB MPKI metrics
        static MetricBuilder ITLBMPKI() { return {}; } ///< 1000 * ITLB_MISSES.WALK_COMPLETED / INST_RETIRED
        static MetricBuilder DTLBMPKI() { return {}; } ///< 1000 * DTLB_MISSES.WALK_COMPLETED / INST_RETIRED
        static MetricBuilder TLBMPKI() { return {}; }  ///< 1000 * TLB_MISSES.WALK_COMPLETED / INST_RETIRED
        // static MetricBuilder LoadSTLBMPKI() { return {}; }  ///< 1000 * DTLB_LD_MISSES.WALK_COMPLETED / INST_RETIRED
        // static MetricBuilder StoreSTLBMPKI() { return {}; } ///< 1000 * DTLB_ST_MISSES.WALK_COMPLETED / INST_RETIRED

        // Latency and parallelism metrics
        static MetricBuilder LoadMissLatency() { return {}; } ///< L1D_PEND_MISS.PENDING / MEM_LD_COMPLETED.L1_MISS_ANY
        static MetricBuilder ILP() { return {}; }             ///< UOPS_EXECUTED.THREAD / UOPS_EXECUTED.CORE_CYCLES_GE1
        static MetricBuilder MLP() { return {}; }             ///< L1D_PEND_MISS.PENDING / L1D_PEND_MISS.PENDING_CYCLES

        // DRAM bandwidth
        static MetricBuilder DRAMBandwidthGBs() { return {}; } ///< (64 * (RD + WR)) / (Time * 1GB)

        // Instruction per event
        static MetricBuilder IpC() { return {}; }          ///< INST_RETIRED / UNHALTED_CLK_CYCLES
        static MetricBuilder IpCall() { return {}; }       ///< INST_RETIRED / BR_INST_RETIRED.NEAR_CALL
        static MetricBuilder IpBranch() { return {}; }     ///< INST_RETIRED / BR_INST_RETIRED.ALL_BRANCHES
        static MetricBuilder IpLoad() { return {}; }       ///< INST_RETIRED / MEM_INST_RETIRED.ALL_LOADS_PS
        static MetricBuilder IpStore() { return {}; }      ///< INST_RETIRED / MEM_INST_RETIRED.ALL_STORES_PS
        static MetricBuilder IpMispredict() { return {}; } ///< INST_RETIRED / BR_MISP_RETIRED.ALL_BRANCHES

        // Floating-point operation metrics
        static MetricBuilder IpFLOP() { return {}; }          ///< Instructions per FP operation
        static MetricBuilder IpArith() { return {}; }         ///< Instructions per FP arithmetic instruction
        static MetricBuilder IpArithScalarSP() { return {}; } ///< INST_RETIRED / FP_ARITH_INST.SCALAR_SINGLE
        static MetricBuilder IpArithScalarDP() { return {}; } ///< INST_RETIRED / FP_ARITH_INST.SCALAR_DOUBLE
        static MetricBuilder IpArithAVX128() { return {}; }   ///< INST_RETIRED / (128B_PACKED_DOUBLE + 128B_PACKED_SINGLE)
        static MetricBuilder IpArithAVX256() { return {}; }   ///< INST_RETIRED / (256B_PACKED_DOUBLE + 256B_PACKED_SINGLE)
        static MetricBuilder IpArithAVX512() { return {}; }   ///< INST_RETIRED / (512B_PACKED_DOUBLE + 512B_PACKED_SINGLE)
        static MetricBuilder IpArithAVXAny() { return {}; }   ///< INST_RETIRED / (RETIRED_SSE_AVX_FLOPS_ANY)

        // Software prefetch
        static MetricBuilder IpSWPF() { return {}; } ///< INST_RETIRED / SW_PREFETCH_ACCESS.T0:u0xF

        // Topdown (Pipeline Utilisation) Analysis L1
        static MetricBuilder FrontendBound() { return {}; }  ///< IDQ_UOPS_NOT_DELIVERED.CORE / (4 * CPU_CLK_UNHALTED.THREAD) — Fraction of slots not delivered by frontend
        static MetricBuilder BadSpeculation() { return {}; } ///< (BR_MISP_RETIRED.ALL_BRANCHES + MACHINE_CLEARS.COUNT) / (4 * CPU_CLK_UNHALTED.THREAD) — Fraction of slots wasted due to branch mispredicts or other speculation issues
        static MetricBuilder BackendBound() { return {}; }   ///< BACKEND_BOUND.SLOTS / (4 * CPU_CLK_UNHALTED.THREAD) — Fraction of slots where backend was unable to accept uops
        static MetricBuilder Retiring() { return {}; }       ///< UOPS_RETIRED.RETIRE_SLOTS / (4 * CPU_CLK_UNHALTED.THREAD) — Fraction of slots retired successfully (i.e., useful work done)
        static MetricBuilder SMTContention() { return {}; }  ///< DISPATHC_SLOTS / (4 * CPU_CLK_UNHALTED.THREAD) — Fraction of unused dispatch slots because the other thread was selected

        // Topdown (Pipeline Utilisation) Analysis L1
        static MetricBuilder FrontendBound_Latency() { return {}; }           ///< ICACHE.MISSES + ITLB_MISSES.STLB_HIT / IDQ_UOPS_NOT_DELIVERED.CORE — Portion of FrontendBound due to instruction cache or TLB latency
        static MetricBuilder FrontendBound_BW() { return {}; }                ///< DECODE_STALL.CYCLES / IDQ_UOPS_NOT_DELIVERED.CORE — Portion of FrontendBound due to bandwidth limitations (decode/queue saturation)
        static MetricBuilder BadSpeculation_Mispredicts() { return {}; }      ///< BR_MISP_RETIRED.ALL_BRANCHES / (4 * CPU_CLK_UNHALTED.THREAD) — Portion of BadSpeculation due to branch mispredicts
        static MetricBuilder BadSpeculation_PipelineRestarts() { return {}; } ///< MACHINE_CLEARS.COUNT / (4 * CPU_CLK_UNHALTED.THREAD) — Portion of BadSpeculation due to pipeline clears (e.g., memory ordering violations)
        static MetricBuilder BackendEndbound_Memory() { return {}; }          ///< MEM_BOUND / BACKEND_BOUND — Portion of BackendBound due to memory issues (DRAM, L3 misses, etc.)
        static MetricBuilder BackendEndbound_CPU() { return {}; }             ///< CORE_BOUND / BACKEND_BOUND — Portion of BackendBound due to non-memory backend issues (e.g., execution unit contention)
        static MetricBuilder Retiring_Fastpath() { return {}; }               ///< UOPS_RETIRED.RETIRE_SLOTS (from scalar/simple ops) / TotalSlots — Portion of Retiring that was serviced via fast-path execution
        static MetricBuilder Retiring_Microcode() { return {}; }              ///< MICROCODE.SEQUENCER_UOPS / TotalSlots — Portion of Retiring that came from microcode assists or complex flows

        // Aggregated Metrics
        static MetricBuilder AllMPKI() { return {}; }
        static MetricBuilder AllCacheHitRatio() { return {}; }
        static MetricBuilder AllSTLBMPKI() { return {}; }
        static MetricBuilder AllLatencyAndParallelism() { return {}; }
        static MetricBuilder AllDRAMBandwidth() { return {}; }
        static MetricBuilder AllIpMetrics() { return {}; }
        static MetricBuilder AllBranchMetrics() { return {}; }

        static MetricBuilder TopdownL1() { return {}; }
        static MetricBuilder TopdownL2() { return {}; }
        static MetricBuilder AllTopdown() { return {}; }

        static MetricBuilder AllMetrics() { return {}; }

    private:
        CoreMetrics() {}
        ~CoreMetrics() {}
    };
} // namespace optkit::core::metrics::cpu