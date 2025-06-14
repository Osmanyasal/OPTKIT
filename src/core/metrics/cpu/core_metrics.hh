/**
 * @file metrics.hpp
 * @brief Defines the extended Metrics interface for CPU performance counters.
 */

#pragma once

#include <vector>
#include <cstdint>
#include <unordered_set>
#include <string>

namespace optkit::core::metrics::cpu
{
    /**
     * @class MetricsBuilder
     * @brief Utility class for aggregating unique CPU performance metric events.
     *
     * MetricsBuilder implements a builder pattern to accumulate CPU performance metric events,
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
     * MetricsBuilder builder;
     * builder.add("L1MPKI", {1001, 1002, 1003})
     *        .add("IpFLOP", {2001});
     * auto all_events = builder.getEvents();
     * @endcode
     *
     * This class is especially useful for consolidating performance monitoring events
     * from different CPU vendors or metric sources into a unified list for monitoring.
     */

    class MetricsBuilder
    {
    public:
        MetricsBuilder() = default;

        // New method accepting a single name and associated event codes
        MetricsBuilder &add(const std::string &name, const std::vector<uint64_t> &codes)
        {
            for (uint64_t code : codes)
            {
                std::string key = std::to_string(code) + "_" + name;
                if (added_keys_.insert(key).second)
                {
                    collected_events_.emplace_back(code, name);
                }
            }
            return *this;
        }

        // to add from another MetricsBuilder's event vector
        MetricsBuilder &add(const std::vector<std::pair<uint64_t, std::string>> &events)
        {
            for (const auto &[code, name] : events)
            {
                std::string key = std::to_string(code) + "_" + name;
                if (added_keys_.insert(key).second)
                {
                    collected_events_.emplace_back(code, name);
                }
            }
            return *this;
        }

        const std::vector<std::pair<uint64_t, std::string>> &getEvents() const
        {
            return collected_events_;
        }

    private:
        std::vector<std::pair<uint64_t, std::string>> collected_events_;
        std::unordered_set<std::string> added_keys_;
    };

    /**
     * @class Metrics
     * @brief Interface for retrieving CPU performance metrics. Each metric method returns the set of events required for its computation.
     *
     * Implementation should take place in cpu vendors not here. The reason these are not fully abstract is that it is possible metric is generic but not exist in any cpu.
     * So it returns empty list.
     */
    class Metrics
    {
    public:
        Metrics() {}
        virtual ~Metrics() {}

        // Cache miss per kilo instruction (MPKI)
        virtual std::vector<std::pair<uint64_t, std::string>> L1MPKI() { return {}; } ///< 1000 * L1_MISSES / INST_RETIRED
        virtual std::vector<std::pair<uint64_t, std::string>> L2MPKI() { return {}; } ///< 1000 * L2_MISSES / INST_RETIRED
        virtual std::vector<std::pair<uint64_t, std::string>> L3MPKI() { return {}; } ///< 1000 * L3_MISSES / INST_RETIRED

        // Branch
        virtual std::vector<std::pair<uint64_t, std::string>> BranchMisprRatio() { return {}; } ///< BR_MISP_RETIRED.ALL_BRANCHES / BR_INST_RETIRED.ALL_BRANCHES

        // STLB MPKI metrics
        virtual std::vector<std::pair<uint64_t, std::string>> CodeSTLBMPKI() { return {}; }  ///< 1000 * ITLB_MISSES.WALK_COMPLETED / INST_RETIRED
        virtual std::vector<std::pair<uint64_t, std::string>> LoadSTLBMPKI() { return {}; }  ///< 1000 * DTLB_LD_MISSES.WALK_COMPLETED / INST_RETIRED
        virtual std::vector<std::pair<uint64_t, std::string>> StoreSTLBMPKI() { return {}; } ///< 1000 * DTLB_ST_MISSES.WALK_COMPLETED / INST_RETIRED

        // Latency and parallelism metrics
        virtual std::vector<std::pair<uint64_t, std::string>> LoadMissLatency() { return {}; } ///< L1D_PEND_MISS.PENDING / MEM_LD_COMPLETED.L1_MISS_ANY
        virtual std::vector<std::pair<uint64_t, std::string>> ILP() { return {}; }             ///< UOPS_EXECUTED.THREAD / UOPS_EXECUTED.CORE_CYCLES_GE1
        virtual std::vector<std::pair<uint64_t, std::string>> MLP() { return {}; }             ///< L1D_PEND_MISS.PENDING / L1D_PEND_MISS.PENDING_CYCLES

        // DRAM bandwidth
        virtual std::vector<std::pair<uint64_t, std::string>> DRAMBandwidthGBs() { return {}; } ///< (64 * (RD + WR)) / (Time * 1GB)

        // Instruction per event
        virtual std::vector<std::pair<uint64_t, std::string>> IpC() { return {}; }          ///< INST_RETIRED / UNHALTED_CLK_CYCLES
        virtual std::vector<std::pair<uint64_t, std::string>> IpCall() { return {}; }       ///< INST_RETIRED / BR_INST_RETIRED.NEAR_CALL
        virtual std::vector<std::pair<uint64_t, std::string>> IpBranch() { return {}; }     ///< INST_RETIRED / BR_INST_RETIRED.ALL_BRANCHES
        virtual std::vector<std::pair<uint64_t, std::string>> IpLoad() { return {}; }       ///< INST_RETIRED / MEM_INST_RETIRED.ALL_LOADS_PS
        virtual std::vector<std::pair<uint64_t, std::string>> IpStore() { return {}; }      ///< INST_RETIRED / MEM_INST_RETIRED.ALL_STORES_PS
        virtual std::vector<std::pair<uint64_t, std::string>> IpMispredict() { return {}; } ///< INST_RETIRED / BR_MISP_RETIRED.ALL_BRANCHES

        // Floating-point operation metrics
        virtual std::vector<std::pair<uint64_t, std::string>> IpFLOP() { return {}; }          ///< Instructions per FP operation
        virtual std::vector<std::pair<uint64_t, std::string>> IpArith() { return {}; }         ///< Instructions per FP arithmetic instruction
        virtual std::vector<std::pair<uint64_t, std::string>> IpArithScalarSP() { return {}; } ///< INST_RETIRED / FP_ARITH_INST.SCALAR_SINGLE
        virtual std::vector<std::pair<uint64_t, std::string>> IpArithScalarDP() { return {}; } ///< INST_RETIRED / FP_ARITH_INST.SCALAR_DOUBLE
        virtual std::vector<std::pair<uint64_t, std::string>> IpArithAVX128() { return {}; }   ///< INST_RETIRED / (128B_PACKED_DOUBLE + 128B_PACKED_SINGLE)
        virtual std::vector<std::pair<uint64_t, std::string>> IpArithAVX256() { return {}; }   ///< INST_RETIRED / (256B_PACKED_DOUBLE + 256B_PACKED_SINGLE)
        virtual std::vector<std::pair<uint64_t, std::string>> IpArithAVX512() { return {}; }   ///< INST_RETIRED / (512B_PACKED_DOUBLE + 512B_PACKED_SINGLE)
        virtual std::vector<std::pair<uint64_t, std::string>> IpArithAVXAny() { return {}; }   ///< INST_RETIRED / (RETIRED_SSE_AVX_FLOPS_ANY)

        // Software prefetch
        virtual std::vector<std::pair<uint64_t, std::string>> IpSWPF() { return {}; } ///< INST_RETIRED / SW_PREFETCH_ACCESS.T0:u0xF

        // Aggregated Metrics
        virtual std::vector<std::pair<uint64_t, std::string>> AllMPKI() { return {}; }
        virtual std::vector<std::pair<uint64_t, std::string>> AllSTLBMPKI() { return {}; }
        virtual std::vector<std::pair<uint64_t, std::string>> AllLatencyAndParallelism() { return {}; }
        virtual std::vector<std::pair<uint64_t, std::string>> AllDRAMBandwidth() { return {}; }
        virtual std::vector<std::pair<uint64_t, std::string>> AllIpMetrics() { return {}; }
        virtual std::vector<std::pair<uint64_t, std::string>> AllBranchMetrics() { return {}; }
        virtual std::vector<std::pair<uint64_t, std::string>> AllMetrics() { return {}; }
    };

} // namespace optkit::core::metrics::cpu
