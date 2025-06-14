#pragma once

#include "core/metrics/cpu/amd/event_wrapper.hh"
#include "core/metrics/cpu/core_metrics.hh"

namespace optkit::core::metrics::cpu::amd
{
    /**
     * @class AMDMetricsImpl
     * @brief Interface for retrieving AMD CPU performance metrics. Each metric method returns the set of events required for its computation.
     *
     */
    class AMDMetricsImpl : public cpu::Metrics
    {
    public:
        AMDMetricsImpl() {}
        virtual ~AMDMetricsImpl() {}

        // Cache miss per kilo instruction (MPKI)
        virtual std::vector<std::pair<uint64_t, std::string>> L1MPKI() override
        {
            return metrics::cpu::MetricsBuilder{}
                .add(to_string(metrics::cpu::CoreEvents::L1_MISSES), AMDEventWrapper::get(cpu::CoreEvents::L1_MISSES))
                .add(to_string(metrics::cpu::CoreEvents::INST_RETIRED), AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED))
                .getEvents();
        } ///< 1000 * L1_MISSES / INST_RETIRED

        virtual std::vector<std::pair<uint64_t, std::string>> L2MPKI() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::L2_MISSES), to_string(metrics::cpu::CoreEvents::L2_MISSES)},
                    {AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), metrics::cpu::to_string(metrics::cpu::CoreEvents::INST_RETIRED)}};
        } ///< 1000 * L2_MISSES / INST_RETIRED

        virtual std::vector<std::pair<uint64_t, std::string>> L3MPKI() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::L3_MISSES), to_string(metrics::cpu::CoreEvents::L3_MISSES)},
                    {AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), metrics::cpu::to_string(metrics::cpu::CoreEvents::INST_RETIRED)}};
        } ///< 1000 * L3_MISSES / INST_RETIRED

        // Branch
        virtual std::vector<std::pair<uint64_t, std::string>> BranchMisprRatio() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::BRANCH_INST_RETIRED), to_string(cpu::CoreEvents::BRANCH_INST_RETIRED)},
                    {AMDEventWrapper::get(cpu::CoreEvents::BRANCH_MISP_RETIRED), to_string(cpu::CoreEvents::BRANCH_MISP_RETIRED)}};
        } ///< BR_MISP_RETIRED.ALL_BRANCHES / BR_INST_RETIRED.ALL_BRANCHES

        // STLB MPKI metrics
        virtual std::vector<std::pair<uint64_t, std::string>> CodeSTLBMPKI() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)},
                    {AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)}};
        } ///< 1000 * ITLB_MISSES.WALK_COMPLETED / INST_RETIRED

        virtual std::vector<std::pair<uint64_t, std::string>> LoadSTLBMPKI() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)},
                    {AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)}};
        } ///< 1000 * DTLB_LD_MISSES.WALK_COMPLETED / INST_RETIRED

        virtual std::vector<std::pair<uint64_t, std::string>> StoreSTLBMPKI() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)},
                    {AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)}};
        } ///< 1000 * DTLB_ST_MISSES.WALK_COMPLETED / INST_RETIRED

        // Latency and parallelism metrics
        virtual std::vector<std::pair<uint64_t, std::string>> LoadMissLatency() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)},
                    {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)}};
        } ///< L1D_PEND_MISS.PENDING / MEM_LD_COMPLETED.L1_MISS_ANY

        virtual std::vector<std::pair<uint64_t, std::string>> IpC() override
        {
            return {{cpu::CoreEvents::INST_RETIRED, to_string(cpu::CoreEvents::INST_RETIRED)},
                    {cpu::CoreEvents::UNHALTED_CORE_CYCLES, to_string(cpu::CoreEvents::UNHALTED_CORE_CYCLES)}};
        } ///< INST_RETIRED / UNHALTED_CLK_CYCLES

        virtual std::vector<std::pair<uint64_t, std::string>> ILP() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)},
                    {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)}};
        } ///< UOPS_EXECUTED.THREAD / UOPS_EXECUTED.CORE_CYCLES_GE1

        virtual std::vector<std::pair<uint64_t, std::string>> MLP() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)},
                    {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)}};
        } ///< L1D_PEND_MISS.PENDING / L1D_PEND_MISS.PENDING_CYCLES

        // DRAM bandwidth
        virtual std::vector<std::pair<uint64_t, std::string>> DRAMBandwidthGBs()
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::),
                     to_string(cpu::CoreEvents::)}};
        } ///< (64 * (RD + WR)) / (Time * 1GB)

        // Instruction per event
        virtual std::vector<std::pair<uint64_t, std::string>> IpCall() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)},
                    {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)}};
        } ///< INST_RETIRED / BR_INST_RETIRED.NEAR_CALL

        virtual std::vector<std::pair<uint64_t, std::string>> IpBranch() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)},
                    {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)}};
        } ///< INST_RETIRED / BR_INST_RETIRED.ALL_BRANCHES

        virtual std::vector<std::pair<uint64_t, std::string>> IpLoad() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)},
                    {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)}};
        } ///< INST_RETIRED / MEM_INST_RETIRED.ALL_LOADS_PS

        virtual std::vector<std::pair<uint64_t, std::string>> IpStore() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)},
                    {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)}};
        } ///< INST_RETIRED / MEM_INST_RETIRED.ALL_STORES_PS

        virtual std::vector<std::pair<uint64_t, std::string>> IpMispredict() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)},
                    {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)}};
        } ///< INST_RETIRED / BR_MISP_RETIRED.ALL_BRANCHES

        // Floating-point operation metrics
        virtual std::vector<std::pair<uint64_t, std::string>> IpFLOP() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)},
                    {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)}};
        } ///< Instructions per FP operation

        virtual std::vector<std::pair<uint64_t, std::string>> IpArith() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)},
                    {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)}};
        } ///< Instructions per FP arithmetic instruction

        virtual std::vector<std::pair<uint64_t, std::string>> IpArithScalarSP() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)},
                    {AMDEventWrapper::get(amd::CoreEvents::), to_string(amd::CoreEvents::)}};
        } ///< INST_RETIRED / FP_ARITH_INST.SCALAR_SINGLE

        virtual std::vector<std::pair<uint64_t, std::string>> IpArithScalarDP() override
        {
            std::vector<std::pair<uint64_t, std::string>> events{};

#if OPTKIT_ENV_CPU_MICROARCH_ZEN

#elif OPTKIT_ENV_CPU_MICROARCH_ZENPLUS

#elif OPTKIT_ENV_CPU_MICROARCH_ZEN2

#elif OPTKIT_ENV_CPU_MICROARCH_ZEN3

#elif OPTKIT_ENV_CPU_MICROARCH_ZEN4
            events = {{AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)},
                      {AMDEventWrapper::get(amd::CoreEvents::), to_string(amd::CoreEvents::)}};
#endif

            return events;
        } ///< INST_RETIRED / FP_ARITH_INST.SCALAR_DOUBLE

        // virtual std::vector<std::pair<uint64_t, std::string>> IpArithAVX128() override
        // {
        //     return {{AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)},
        //             {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)},
        //             {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)}};
        // } ///< INST_RETIRED / (128B_PACKED_DOUBLE + 128B_PACKED_SINGLE)

        // virtual std::vector<std::pair<uint64_t, std::string>> IpArithAVX256() override
        // {
        //     return {{AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)},
        //             {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)},
        //             {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)}};
        // } ///< INST_RETIRED / (256B_PACKED_DOUBLE + 256B_PACKED_SINGLE)

        // virtual std::vector<std::pair<uint64_t, std::string>> IpArithAVX512() override
        // {
        //     return {{AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)},
        //             {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)},
        //             {AMDEventWrapper::get(cpu::CoreEvents::), to_string(cpu::CoreEvents::)}};
        // } ///< INST_RETIRED / (512B_PACKED_DOUBLE + 512B_PACKED_SINGLE)

        virtual std::vector<std::pair<uint64_t, std::string>> IpArithAVXAny() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)},
                    {AMDEventWrapper::get(amd::CoreEvents::RETIRED_SSE_AVX_FLOPS_ANY), to_string(amd::CoreEvents::RETIRED_SSE_AVX_FLOPS_ANY)}};
        } ///< INST_RETIRED / (RETIRED_SSE_AVX_FLOPS_ANY)

        // Software prefetch
        virtual std::vector<std::pair<uint64_t, std::string>> IpSWPF() override
        {
            return {{AMDEventWrapper::get(cpu::CoreEvents::INST_RETIRED), to_string(cpu::CoreEvents::INST_RETIRED)},
                    {AMDEventWrapper::get(cpu::CoreEvents::SW_LOAD_PREFETCH_ACCESS), to_string(cpu::CoreEvents::SW_LOAD_PREFETCH_ACCESS)}};
        } ///< INST_RETIRED / SW_PREFETCH_ACCESS.T0:u0xF

        // Aggregated Metrics

        // Aggregate all cache miss metrics
        virtual std::vector<std::pair<uint64_t, std::string>> AllMPKI()
        {
            return metrics::cpu::MetricsBuilder{}
                .add(L1MPKI())
                .add(L2MPKI())
                .add(L3MPKI())
                .getEvents();
        }

        // Aggregate all STLB MPKI metrics
        virtual std::vector<std::pair<uint64_t, std::string>> AllSTLBMPKI()
        {
            return metrics::cpu::MetricsBuilder{}
                .add(CodeSTLBMPKI())
                .add(LoadSTLBMPKI())
                .add(StoreSTLBMPKI())
                .getEvents();
        }

        // Aggregate all latency and parallelism metrics
        virtual std::vector<std::pair<uint64_t, std::string>> AllLatencyAndParallelism()
        {
            return metrics::cpu::MetricsBuilder{}
                .add(LoadMissLatency())
                .add(ILP())
                .add(MLP())
                .getEvents();
        }

        // Aggregate all DRAM bandwidth metrics
        virtual std::vector<std::pair<uint64_t, std::string>> AllDRAMBandwidth()
        {
            return metrics::cpu::MetricsBuilder{}
                .add(DRAMBandwidthGBs())
                .getEvents();
        }

        // Aggregate all instruction-per-event metrics
        virtual std::vector<std::pair<uint64_t, std::string>> AllIpMetrics()
        {
            return metrics::cpu::MetricsBuilder{}
                .add(IpCall())
                .add(IpBranch())
                .add(IpLoad())
                .add(IpStore())
                .add(IpMispredict())
                .add(IpFLOP())
                .add(IpArith())
                .add(IpArithScalarSP())
                .add(IpArithScalarDP())
                .add(IpArithAVX128())
                .add(IpArithAVX256())
                .add(IpArithAVX512())
                .add(IpArithAVXALL())
                .add(IpSWPF())
                .getEvents();
        }

        // Aggregate all branch-related metrics
        virtual std::vector<std::pair<uint64_t, std::string>> AllBranchMetrics()
        {
            return metrics::cpu::MetricsBuilder{}
                .add(BranchMisprRatio())
                .getEvents();
        }

        // Aggregate everything
        virtual std::vector<std::pair<uint64_t, std::string>> AllMetrics()
        {
            return metrics::cpu::MetricsBuilder{}
                .add(AllMPKI())
                .add(AllSTLBMPKI())
                .add(AllLatencyAndParallelism())
                .add(AllDRAMBandwidth())
                .add(AllIpMetrics())
                .add(AllBranchMetrics())
                .getEvents();
        }

    private:
        // TODO: create a method to fill std::vector<std::pair<uint64_t, std::string>> to the unordered_map and process, which will save processing method to another data structure and call it.
        std::unordered_map<std::string, uint64_t> results; // event_name - event_value
    };
}