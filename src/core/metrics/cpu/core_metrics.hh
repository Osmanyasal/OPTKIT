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

        // Aggregated Metrics
        static MetricBuilder AllMPKI() { return {}; }
        static MetricBuilder AllSTLBMPKI() { return {}; }
        static MetricBuilder AllLatencyAndParallelism() { return {}; }
        static MetricBuilder AllDRAMBandwidth() { return {}; }
        static MetricBuilder AllIpMetrics() { return {}; }
        static MetricBuilder AllBranchMetrics() { return {}; }
        static MetricBuilder AllMetrics() { return {}; }

    private:
        CoreMetrics() {}
        ~CoreMetrics() {}
    };
} // namespace optkit::core::metrics::cpu