#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
namespace optkit::core::metrics::cpu
{
    /**
     * @class MetricBuilder
     * @brief Utility class for aggregating unique CPU performance metric events and associating metric calculations.
     *
     * MetricBuilder implements a builder pattern to:
     * - Accumulate CPU performance metric events (pairs of event names and event codes)
     * - Attach one or more calculation functions to generate named metrics from the collected event results
     *
     * 🔧 Key Features:
     * - Each raw event is uniquely identified by the combination of its event name and event code.
     * - Duplicate entries, (name, code) pairs, are added only once.
     * - Events can be added in groups using either:
     *     - `add(name, codes)` — associates multiple codes with a single name
     *     - `add(events)` — adds a list of (name, code) pairs usually coming from other MetricBuilder objects.
     * - Multiple named metric calculations can be registered via `build(name, func)`
     * - Use `calculate(results)` to compute **all registered metrics** at once, returning a vector of (metric_name, value) pairs
     * - Use `metric_names()` to list all registered metric calculations
     *
     * ✅ Example:
     * @code
     * MetricBuilder builder;
     * builder.add("inst_retired", {0x00c0})
     *        .add("cpu_cycles", {0x003c})
     *        .add("cache_misses", {0x412e})
     *        .build("IPC", [](const auto &m) {
     *            return m.at("inst_retired") / static_cast<double>(m.at("cpu_cycles"));
     *        })
     *        .build("MPKI", [](const auto &m) {
     *            return m.at("cache_misses") * 1000.0 / m.at("inst_retired");
     *        });
     *
     * std::vector<std::pair<std::string, uint64_t>> results = {
     *     {"inst_retired", 5'000'000},
     *     {"cpu_cycles", 10'000'000},
     *     {"cache_misses", 25'000}
     * };
     *
     * auto all_metrics = builder.calculate(results);
     * for (const auto &[name, value] : all_metrics) {
     *     std::cout << name << ": " << value << "\n";
     * }
     * @endcode
     *
     * 📌 Use Case:
     * MetricBuilder is ideal for defining and computing CPU-level performance metrics in tools
     * that use PMUs (Performance Monitoring Units), like profilers, simulators, or monitoring agents.
     * It helps cleanly organize both raw events and derived metrics using a simple declarative API.
     */

    class MetricBuilder
    {
    public:
        using CalculationFunc = std::function<double(const std::unordered_map<std::string, uint64_t> &)>;

        MetricBuilder() = default;

        // Add event codes with a name (no change here)
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

        MetricBuilder &add(const MetricBuilder &mb)
        {
            this->add(mb.metric_events);

            // Add calculation functions (overwrites if names collide)
            for (std::unordered_map<std::string, CalculationFunc>::const_iterator it = mb.calculation_funcs.begin(); it != mb.calculation_funcs.end(); ++it)
            {
                const std::string &name = it->first;
                const CalculationFunc &func = it->second;
                this->calculation_funcs[name] = func;
            }

            return *this;
        }

        MetricBuilder &build(const std::string &metric_name, CalculationFunc func)
        {
            calculation_funcs[metric_name] = func;
            return *this;
        }

        // Get the result of a specific metric
        std::vector<std::pair<std::string, double>> calculate(const std::vector<std::pair<std::string, uint64_t>> &results) const
        {
            if (calculation_funcs.empty())
                return {};

            std::vector<std::pair<std::string, double>> computed_metrics;

            std::unordered_map<std::string, uint64_t> results_map;
            for (auto it = results.begin(); it != results.end(); ++it)
                results_map[it->first] += it->second; // accumulate

            for (auto it = calculation_funcs.begin(); it != calculation_funcs.end(); ++it)
            {
                const std::string &name = it->first;
                const CalculationFunc &func = it->second;
                computed_metrics.push_back(std::make_pair(name, func(results_map)));
            }

            return computed_metrics;
        }

        // Optional: get list of supported metrics
        std::vector<std::string> metric_names() const
        {
            std::vector<std::string> names;
            for (std::unordered_map<std::string, CalculationFunc>::const_iterator it = calculation_funcs.begin(); it != calculation_funcs.end(); ++it)
            {
                names.push_back(it->first);
            }
            return names;
        }

    public:
        std::vector<std::pair<std::string, uint64_t>> metric_events;

    private:
        std::unordered_set<std::string> added_keys_;
        std::unordered_map<std::string, CalculationFunc> calculation_funcs;
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
        static MetricBuilder AllMPKI() { return {}; }
        static MetricBuilder AllSTLBMPKI() { return {}; }
        static MetricBuilder AllLatencyAndParallelism() { return {}; }
        static MetricBuilder AllDRAMBandwidth() { return {}; }
        static MetricBuilder AllIpMetrics() { return {}; }
        static MetricBuilder AllBranchMetrics() { return {}; }
        static MetricBuilder AllMetrics() { return {}; }

    private:
        Metrics() {}
        ~Metrics() {}
    };

    std::string to_string(const MetricBuilder &mb);
    std::ostream &operator<<(std::ostream &os, const MetricBuilder &mb);

} // namespace optkit::core::metrics::cpu

using optkit::core::metrics::cpu::operator<<;