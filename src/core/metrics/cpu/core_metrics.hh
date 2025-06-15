#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <functional>
#include <unordered_set>
#include <unordered_map>
namespace optkit::core::metrics::cpu
{
    /**
     * @class MetricBuilder
     * @brief Utility class for aggregating unique CPU performance metric events.
     *
     * MetricBuilder implements a builder pattern to accumulate CPU performance metric events,
     * represented as pairs of event names and their associated event codes.
     *
     * Key characteristics:
     * - Each event is uniquely identified by the combination of its event code and event name.
     * - Duplicate pairs of (event_code, event_name) are ignored, ensuring no repeated entries.
     * - An event name can be associated with multiple different event codes, allowing flexible grouping.
     *
     * The class provides two overloads of the add() method:
     * - add(name, codes): Adds multiple event codes under a single event name.
     * - add(events): Adds multiple (code, name) pairs at once from another event collection.
     *
     * Use getEvents() to retrieve the accumulated unique event pairs as a vector of (code, name),
     * compatible with PMU class constructors or similar consumers.
     *
     * Example usage:
     * @code
     * MetricBuilder builder;
     * builder.add("L1MPKI", {1001, 1002, 1003})
     *        .add("IpFLOP", {2001});
     * auto all_events = builder.getEvents();
     * @endcode
     *
     * This class is especially useful for consolidating performance monitoring events
     * from different CPU vendors or metric sources into a unified list for monitoring.
     */

    class MetricBuilder
    {
    public:
        using CalculationFunc = std::function<double(const std::unordered_map<std::string, uint64_t> &)>;
        MetricBuilder() = default;

        // New method accepting a single name and associated event codes
        MetricBuilder &add(const std::string &name, const std::vector<uint64_t> &codes)
        {
            for (uint64_t code : codes)
            {
                std::string key = name + "_" + std::to_string(code);
                if (added_keys_.insert(key).second)
                {
                    metric_events.emplace_back(name, code);
                }
            }
            return *this;
        }

        // to add from another MetricBuilder's event vector
        MetricBuilder &add(const std::vector<std::pair<std::string, uint64_t>> &events)
        {
            for (const auto &pair : events)
            {
                std::string key = pair.first + "_" + std::to_string(pair.second);
                if (added_keys_.insert(key).second)
                {
                    metric_events.emplace_back(pair.first, pair.second);
                }
            }
            return *this;
        }

        MetricBuilder &build(const std::string &metric_name, CalculationFunc calculation_func)
        {
            this->metric_name = metric_name;
            this->calculate_func = calculation_func;
            return *this;
        }

        double calculate(const std::vector<std::pair<std::string, uint64_t>> &results) const
        {
            if (!calculate_func)
                return -1;

            std::unordered_map<std::string, uint64_t> results_map;
            for (const auto &pair : results)
                results_map[pair.first] = pair.second;

            return calculate_func(results_map);
        }

    public:
        std::vector<std::pair<std::string, uint64_t>> metric_events;
        std::string metric_name;

    private:
        std::unordered_set<std::string> added_keys_;
        CalculationFunc calculate_func;
    };

    /**
     * @class Metrics
     * @brief Interface for retrieving CPU performance metrics. Each metric method returns the set of events required for its computation.
     *
     * Implementation should take place in cpu vendors not here. The reason these are not fully abstract is that it is possible metric is generic but not exist in any cpu.
     * So it returns empty list.
     */
    template <typename T>
    class Metrics
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
        static std::vector<MetricBuilder> AllMPKI() { return {}; }
        static std::vector<MetricBuilder> AllSTLBMPKI() { return {}; }
        static std::vector<MetricBuilder> AllLatencyAndParallelism() { return {}; }
        static std::vector<MetricBuilder> AllDRAMBandwidth() { return {}; }
        static std::vector<MetricBuilder> AllIpMetrics() { return {}; }
        static std::vector<MetricBuilder> AllBranchMetrics() { return {}; }
        static std::vector<MetricBuilder> AllMetrics() { return {}; }

    private:
        Metrics() {}
        ~Metrics() {}
    };

    std::string to_string(const MetricBuilder &mb);
    std::ostream &operator<<(std::ostream &os, const MetricBuilder &mb);

} // namespace optkit::core::metrics::cpu

using optkit::core::metrics::cpu::operator<<;