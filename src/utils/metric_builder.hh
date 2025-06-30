#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include "utils/utils.hh"
namespace optkit::core::metrics
{
    /**
     * @class MetricBuilder
     * @brief Utility class for aggregating unique performance metric events and associating metric calculations.
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
     * Following steps are automatically done in OPTKIT_CPU_METRICS macro, you just need to provide block name and MetricBuilder class of your own.
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
     *
     * @param print_events tells profiler to print the events (note that metrics are always printed.)
     */

    class MetricBuilder
    {
    public:
        using CalculationFunc = std::function<double(const std::unordered_map<std::string, uint64_t> &)>;

        MetricBuilder(bool print_events = true) : print_events{print_events} {};

        // Add event codes with a name (no change here)
        MetricBuilder &add(const std::string &name, const std::vector<uint64_t> &event_codes)
        {
            if (OPT_UNLIKELY(event_codes.empty()))
                return *this; // nothing to add

            for (uint64_t code : event_codes)
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
            if (OPT_UNLIKELY(events.empty()))
                return *this; // nothing to add

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
            if (mb.metric_events.empty())
                return *this;

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

        // Pass event results and calculate all metrics defined then return the result
        std::vector<std::pair<std::string, double>> calculate(const std::unordered_map<std::string, uint64_t> &results) const
        {
            if (calculation_funcs.empty())
                return {};

            std::vector<std::pair<std::string, double>> computed_metrics;
            for (auto it = calculation_funcs.begin(); it != calculation_funcs.end(); ++it)
            {
                const std::string &name = it->first;
                const CalculationFunc &func = it->second;
                computed_metrics.push_back(std::make_pair(name, func(results)));
            }

            return computed_metrics;
        }

        std::vector<std::string> metric_names() const
        {
            std::vector<std::string> names;
            for (std::unordered_map<std::string, CalculationFunc>::const_iterator it = calculation_funcs.begin(); it != calculation_funcs.end(); ++it)
            {
                names.push_back(it->first);
            }
            return names;
        }

        std::vector<std::string> event_names() const
        {
            std::vector<std::string> names;
            for (auto it = metric_events.begin(); it != metric_events.end(); ++it)
            {
                names.push_back(it->first);
            }
            return names;
        }

        CalculationFunc metric_calculation_func(const std::string &name) const
        {
            auto it = calculation_funcs.find(name);
            if (it == calculation_funcs.end())
            {
                throw std::runtime_error("Metric function '" + name + "' not found.");
            }
            return it->second;
        }

    public:
        std::vector<std::pair<std::string, uint64_t>> metric_events;
        bool print_events;

    private:
        std::unordered_set<std::string> added_keys_;
        std::unordered_map<std::string, CalculationFunc> calculation_funcs;
    };

    std::string to_string(const MetricBuilder &mb);
    std::ostream &operator<<(std::ostream &os, const MetricBuilder &mb);

} // namespace optkit::core::metrics

using optkit::core::metrics::operator<<;