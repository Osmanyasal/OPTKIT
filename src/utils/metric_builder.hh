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
namespace optkit::metrics
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
     * MetricBuilder<uint64_t> builder;
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
     *     {"cache_misses", 25'000},
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
     * @param allow_duplicates if true, allows adding duplicate (name, code) pairs.
     *
     * @note In what order the event names and codes are added is IMPORTANT! it is read as it is added.
     *       If you add event1, event2, event3 then the read buffer will contain the values in the same order.
     *       Given the reason, we used vectors and pairs to store the data in metric Builder.
     */
    template <typename eventResultType>
    class MetricBuilder
    {
    public:
        using CalculationFunc = std::function<double(const std::unordered_map<std::string, eventResultType> &)>;

        MetricBuilder(bool print_events = true, bool allow_duplicates = false) : print_events{print_events}, allow_duplicates{allow_duplicates}, ill_formed{false} {};

        // Add event codes with a name (no change here)
        MetricBuilder &add(const std::string &name, const std::vector<uint64_t> &event_codes)
        {
            if (OPT_UNLIKELY(event_codes.empty()))
                return *this; // nothing to add

            for (uint64_t code : event_codes)
            {
                std::string key = name + "_" + std::to_string(code);
                if (OPT_LIKELY(allow_duplicates || added_keys_.insert(key).second))
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
                if (OPT_LIKELY(allow_duplicates || added_keys_.insert(key).second))
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
            for (const auto &pair : mb.calculation_funcs)
            {
                const std::string &name = pair.first;
                const CalculationFunc &func = pair.second;
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
        std::vector<std::pair<std::string, double>> calculate(const std::unordered_map<std::string, eventResultType> &results) const
        {
            if (calculation_funcs.empty())
                return {};

            std::vector<std::pair<std::string, double>> computed_metrics;
            for (const auto &pair : calculation_funcs)
            {
                const std::string &name = pair.first;
                const CalculationFunc &func = pair.second;
                computed_metrics.push_back(std::make_pair(name, func(results)));
            }

            return computed_metrics;
        }

        std::vector<std::string> metric_names() const
        {
            std::vector<std::string> names;
            for (const auto &pair : calculation_funcs)
            {
                names.push_back(pair.first);
            }
            return names;
        }

        std::vector<std::string> event_names() const
        {
            std::vector<std::string> names;
            for (const auto &pair : metric_events)
            {
                names.push_back(pair.first);
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
        bool allow_duplicates; // if true, allows adding duplicate events (name, code) pairs

    private:
        std::unordered_set<std::string> added_keys_;
        std::unordered_map<std::string, CalculationFunc> calculation_funcs;

        bool ill_formed; // if true, the MetricBuilder is not well formed, i.e. no events or no calculations.
    };

    template <typename eventResultType>
    OPT_FORCE_INLINE eventResultType get_event_count(const std::unordered_map<std::string, eventResultType> &counts, const std::string &name, const eventResultType &default_value = eventResultType{})
    {
        auto it = counts.find(name);
        return (it != counts.end()) ? it->second : default_value;
    }

    template <typename eventResultType>
    std::string to_string(const MetricBuilder<eventResultType> &mb)
    {
        std::ostringstream oss;

        // Header summary
        oss << "MetricBuilder Summary:\n";
        oss << "  Total Events: " << mb.metric_events.size() << "\n";
        oss << "  Defined Metrics: " << mb.metric_names().size() << "\n\n";

        // List metric names
        oss << "Metrics:\n";
        for (const auto &name : mb.metric_names())
        {
            oss << "  - " << name << "\n";
        }

        // List event codes
        oss << "\nEvents:\n";
        for (const auto &pair : mb.metric_events)
        {
            oss << "  " << pair.first << " = 0x" << std::hex << pair.second << std::dec << "\n";
        }

        return oss.str();
    }
    template <typename eventResultType>
    std::ostream &operator<<(std::ostream &os, const MetricBuilder<eventResultType> &mb)
    {
        return os << to_string(mb);
    }

} // namespace optkit::metrics

using optkit::metrics::operator<<;