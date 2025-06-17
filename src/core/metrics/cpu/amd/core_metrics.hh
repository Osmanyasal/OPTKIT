#pragma once

#include "core/metrics/cpu/amd/event_mapper.hh"
#include "core/metrics/cpu/core_metrics.hh"

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
        // Cache miss per kilo instruction (MPKI)
        static MetricBuilder L1MPKI()
        {
            std::string l1_misses_name = to_string(CoreEvents::L1_MISSES);
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            return MetricBuilder{}
                .add(l1_misses_name, amd::EventMapper::get(cpu::CoreEvents::L1_MISSES))
                .add(inst_retired_name, amd::EventMapper::get(cpu::CoreEvents::INST_RETIRED))
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
                .add(l2_misses_name, amd::EventMapper::get(cpu::CoreEvents::L2_MISSES))
                .add(inst_retired_name, amd::EventMapper::get(cpu::CoreEvents::INST_RETIRED))
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
                .add(l3_misses_name, amd::EventMapper::get(cpu::CoreEvents::L3_MISSES))
                .add(inst_retired_name, amd::EventMapper::get(cpu::CoreEvents::INST_RETIRED))
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
            std::string branch_inst_retired_name = to_string(cpu::CoreEvents::BRANCH_INST_RETIRED);
            std::string branch_misp_retired_name = to_string(cpu::CoreEvents::BRANCH_MISP_RETIRED);

            return MetricBuilder{}
                .add(branch_inst_retired_name, amd::EventMapper::get(cpu::CoreEvents::BRANCH_INST_RETIRED))
                .add(branch_misp_retired_name, amd::EventMapper::get(cpu::CoreEvents::BRANCH_MISP_RETIRED))
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
            std::string itlb_misses_name = to_string(cpu::CoreEvents::ITLB_MISSES);
            std::string inst_retired_name = to_string(cpu::CoreEvents::INST_RETIRED);

            return MetricBuilder{}
                .add(itlb_misses_name, amd::EventMapper::get(cpu::CoreEvents::ITLB_MISSES))
                .add(inst_retired_name, amd::EventMapper::get(cpu::CoreEvents::INST_RETIRED))
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
            std::string dtlb_misses_name = to_string(cpu::CoreEvents::DTLB_MISSES);
            std::string inst_retired_name = to_string(cpu::CoreEvents::INST_RETIRED);

            return MetricBuilder{}
                .add(dtlb_misses_name, amd::EventMapper::get(cpu::CoreEvents::DTLB_MISSES))
                .add(inst_retired_name, amd::EventMapper::get(cpu::CoreEvents::INST_RETIRED))
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
            std::string dtlb_misses_name = to_string(cpu::CoreEvents::DTLB_MISSES);
            std::string itlb_misses_name = to_string(cpu::CoreEvents::ITLB_MISSES);
            std::string inst_retired_name = to_string(cpu::CoreEvents::INST_RETIRED);

            return MetricBuilder{}
                .add(itlb_misses_name, amd::EventMapper::get(cpu::CoreEvents::DTLB_MISSES))
                .add(dtlb_misses_name, amd::EventMapper::get(cpu::CoreEvents::ITLB_MISSES))
                .add(inst_retired_name, amd::EventMapper::get(cpu::CoreEvents::INST_RETIRED))
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
            std::string inst_retired_name = to_string(cpu::CoreEvents::INST_RETIRED);
            std::string unhalted_core_cycles_name = to_string(cpu::CoreEvents::UNHALTED_CORE_CYCLES);

            return MetricBuilder{}
                .add(inst_retired_name, amd::EventMapper::get(cpu::CoreEvents::INST_RETIRED))
                .add(unhalted_core_cycles_name, amd::EventMapper::get(cpu::CoreEvents::UNHALTED_CORE_CYCLES))
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
                .add(inst_retired_name, amd::EventMapper::get(cpu::CoreEvents::INST_RETIRED))
                .add(branch_inst_retired_name, amd::EventMapper::get(cpu::CoreEvents::BRANCH_INST_RETIRED))
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
                .add(inst_retired_name, amd::EventMapper::get(cpu::CoreEvents::INST_RETIRED))
                .add(mem_load_retired_name, amd::EventMapper::get(cpu::CoreEvents::MEM_LOAD_RETIRED))
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
                .add(inst_retired_name, amd::EventMapper::get(cpu::CoreEvents::INST_RETIRED))
                .add(mem_store_retired_name, amd::EventMapper::get(cpu::CoreEvents::MEM_STORE_RETIRED))
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
                .add(inst_retired_name, amd::EventMapper::get(cpu::CoreEvents::INST_RETIRED))
                .add(branch_misp_retired_name, amd::EventMapper::get(cpu::CoreEvents::BRANCH_MISP_RETIRED))
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
            return {};
        } ///< Instructions per FP operation

        static MetricBuilder IpArith()
        {
            return {};
        } ///< Instructions per FP arithmetic instruction

        static MetricBuilder IpArithScalarSP()
        {
            return {};
        } ///< Instructions per FP scalar single precision instruction

        static MetricBuilder IpArithScalarDP()
        {
            return {};
        } ///< INST_RETIRED / FP_ARITH_INST.SCALAR_DOUBLE

        static MetricBuilder IpArithAVX128()
        {
            return {};
        } ///< INST_RETIRED / (128B_PACKED_DOUBLE + 128B_PACKED_SINGLE)

        static MetricBuilder IpArithAVX256()
        {
            return {};
        } ///< INST_RETIRED / (256B_PACKED_DOUBLE + 256B_PACKED_SINGLE)

        static MetricBuilder IpArithAVX512()
        {
            return {};
        } ///< INST_RETIRED / (512B_PACKED_DOUBLE + 512B_PACKED_SINGLE)

        static MetricBuilder IpArithAVXAny()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string retired_sse_avx_flops_any_name = to_string(CoreEvents::RETIRED_SSE_AVX_FLOPS_ANY);
            return MetricBuilder{}
                .add(inst_retired_name, amd::EventMapper::get(cpu::CoreEvents::INST_RETIRED))
                .add(retired_sse_avx_flops_any_name, amd::EventMapper::get(cpu::CoreEvents::RETIRED_SSE_AVX_FLOPS_ANY))
                .build("IpArithAVXAny",
                       [retired_sse_avx_flops_any_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                       {
                           uint64_t inst_retired = counts.at(inst_retired_name);
                           uint64_t retired_sse_avx_flops_any = counts.at(retired_sse_avx_flops_any_name);

                           // Avoid div by zero
                           if (retired_sse_avx_flops_any == 0)
                               std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(inst_retired) / static_cast<double>(retired_sse_avx_flops_any);
                       });

        } ///< INST_RETIRED / (RETIRED_SSE_AVX_FLOPS_ANY)

        // Software prefetch
        static MetricBuilder IpSWPF()
        {
            std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
            std::string sw_load_prefetch_name = to_string(CoreEvents::SW_LOAD_PREFETCH_ACCESS);
            return MetricBuilder{}
                .add(inst_retired_name, amd::EventMapper::get(cpu::CoreEvents::INST_RETIRED))
                .add(sw_load_prefetch_name, amd::EventMapper::get(cpu::CoreEvents::SW_LOAD_PREFETCH_ACCESS))
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

        // Aggregated Metrics

        // Aggregate all cache miss metrics
        static MetricBuilder AllMPKI()
        {
            MetricBuilder mb;
            mb.add(L1MPKI());
            mb.add(L2MPKI());
            mb.add(L3MPKI());
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
            mb.add(IpLoad());
            mb.add(IpStore());
            mb.add(IpMispredict());
            mb.add(IpFLOP());
            mb.add(IpArith());
            mb.add(IpArithScalarSP());
            mb.add(IpArithScalarDP());
            // mb.add(IpArithAVX128());
            // mb.add(IpArithAVX256());
            // mb.add(IpArithAVX512());
            // mb.add(IpArithAVXAny());
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

        // Aggregate all metrics
        static MetricBuilder AllMetrics()
        {
            MetricBuilder all;
            all.add(AllMPKI());
            all.add(AllSTLBMPKI());
            all.add(AllLatencyAndParallelism());
            all.add(AllDRAMBandwidth());
            all.add(AllIpMetrics());
            all.add(AllBranchMetrics());
            return all;
        }
    };
}