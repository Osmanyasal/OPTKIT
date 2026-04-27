#include "core/disk/disk_profiler.hh"

#include <unordered_set>

namespace optkit::disk
{

    constexpr double READ_BUFFER_FLUSH_PERIOD_MS = 5000.0;

    OPT_FORCE_INLINE std::vector<std::string> unique_event_names_from(const std::vector<std::string> &event_names)
    {
        std::unordered_set<std::string> seen;
        std::vector<std::string> unique;
        unique.reserve(event_names.size());
        for (const auto &name : event_names)
        {
            if (seen.insert(name).second)
                unique.push_back(name);
        }
        return unique;
    }

    OPT_FORCE_INLINE std::vector<std::pair<std::string, uint64_t>> event_values_from_counts(
        const std::vector<std::string> &unique_event_names,
        const std::unordered_map<std::string, uint64_t> &counts)
    {
        std::vector<std::pair<std::string, uint64_t>> event_values;
        event_values.reserve(unique_event_names.size());
        for (const auto &name : unique_event_names)
        {
            auto it = counts.find(name);
            event_values.emplace_back(name, (it == counts.end()) ? 0 : it->second);
        }
        return event_values;
    }

    OPT_FORCE_INLINE std::unordered_map<std::string, uint64_t> event_counts_from_sample(
        const std::vector<std::string> &event_names,
        const std::vector<uint64_t> &values,
        double duration_ms)
    {
        std::unordered_map<std::string, uint64_t> counts;
        if (event_names.empty() || values.empty())
        {
            counts["duration_microsec"] = static_cast<uint64_t>(duration_ms * 1000.0);
            return counts;
        }

        for (size_t j = 0; j < values.size(); ++j)
        {
            counts[event_names[j % event_names.size()]] += values[j];
        }
        counts["duration_microsec"] = static_cast<uint64_t>(duration_ms * 1000.0);
        return counts;
    }

    OPT_FORCE_INLINE std::unordered_map<std::string, uint64_t> aggregate_counts_from_read_buffer(
        const std::vector<std::string> &event_names,
        const std::vector<std::pair<double, std::vector<uint64_t>>> &read_buffer,
        double total_duration_ms)
    {
        std::unordered_map<std::string, uint64_t> aggregated_counts;
        for (const auto &sample : read_buffer)
        {
            const std::unordered_map<std::string, uint64_t> sample_counts =
                event_counts_from_sample(event_names, sample.second, sample.first);
            for (std::unordered_map<std::string, uint64_t>::const_iterator it = sample_counts.begin(); it != sample_counts.end(); ++it)
            {
                if (it->first == "duration_microsec")
                    continue;
                aggregated_counts[it->first] += it->second;
            }
        }
        aggregated_counts["duration_microsec"] = static_cast<uint64_t>(total_duration_ms * 1000.0);
        return aggregated_counts;
    }

    void IoDiskProfiler::on_sample_stored(const std::pair<double, std::vector<uint64_t>> &sample)
    {
        this->buffered_duration_ms += sample.first;
        if (this->buffered_duration_ms < READ_BUFFER_FLUSH_PERIOD_MS)
            return;

        flush_compacted_samples();
    }

    void IoDiskProfiler::flush_compacted_samples()
    {
        if (this->read_buffer.empty())
            return;

        const double flushed_duration_ms = this->buffered_duration_ms;
        const std::unordered_map<std::string, uint64_t> aggregated_counts =
            aggregate_counts_from_read_buffer(this->metric_builder.event_names(), this->read_buffer, flushed_duration_ms);

        for (std::unordered_map<std::string, uint64_t>::const_iterator it = aggregated_counts.begin(); it != aggregated_counts.end(); ++it)
        {
            if (it->first == "duration_microsec")
                continue;
            this->compacted_event_counts[it->first] += it->second;
        }
        this->compacted_duration_ms += flushed_duration_ms;
        this->buffered_duration_ms = 0.0;
        this->read_buffer.clear();
    }

    OPT_FORCE_INLINE void append_reading_json(
        nlohmann::json &out,
        double duration_ms,
        const char *measurement_type,
        const std::vector<std::pair<std::string, uint64_t>> &event_values,
        const std::vector<std::pair<std::string, double>> &metric_values)
    {
        nlohmann::json single = utils::to_json<uint64_t>(duration_ms, measurement_type, event_values, metric_values);
        if (single.contains("readings") && single["readings"].is_array() && !single["readings"].empty())
            out["readings"].push_back(single["readings"][0]);
    }

    IoDiskProfiler::IoDiskProfiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<uint64_t> &mb)
        : BaseProfiler(profiler_config), metric_builder(mb), is_sampling(false)
    {
        last_snapshot = read_selected_io_counters();

        if (OPT_UNLIKELY(this->config.is_sampling))
        {
            this->is_sampling = true;
            this->sampling_thread = std::thread([this]()
                                                {
                while (this->is_sampling)
                {
                    this->read_and_store();
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                } });
        }
    }

    IoDiskProfiler::~IoDiskProfiler()
    {
        if (this->config.is_sampling && this->sampling_thread.joinable())
        {
            this->is_sampling = false;
            this->sampling_thread.join();
        }

        this->read_and_store();

        if (OPT_UNLIKELY(this->config.is_screenshot))
        {
            double total_duration = 0.0;
            for (const auto &entry : this->read_buffer)
                total_duration += entry.first;
            this->total_duration_ms = total_duration;

            this->event_results.clear();
            this->metric_results.clear();
        }
        else
        {
            this->metric_results = this->metric_builder.calculate(aggregate());
        }

        if (OPT_LIKELY(this->config.dump_results_to_file))
            this->save();

        if (OPT_LIKELY(this->config.verbose))
        {
            if (OPT_UNLIKELY(this->config.is_screenshot))
            {
                std::cout << "\033[1;33m"
                          << "Block: " << this->config.block_name << "\033[0m"
                          << " [" << this->total_duration_ms << "ms] Screenshot samples (" << this->read_buffer.size() << ")\n";

                const std::vector<std::string> &event_names = this->metric_builder.event_names();
                const auto unique_event_names = unique_event_names_from(event_names);

                if (!this->read_buffer.empty())
                {
                    const std::unordered_map<std::string, uint64_t> aggregated_counts =
                        aggregate_counts_from_read_buffer(event_names, this->read_buffer, this->total_duration_ms);
                    const auto aggregated_metrics = this->metric_builder.calculate(aggregated_counts);

                    std::cout << std::fixed << "\t[Aggregated] duration_ms: " << this->total_duration_ms << std::endl;
                    if (OPT_UNLIKELY(this->metric_builder.print_events))
                    {
                        for (const auto &name : unique_event_names)
                        {
                            auto it = aggregated_counts.find(name);
                            std::cout << std::fixed << "\t\t" << name << ": " << ((it == aggregated_counts.end()) ? 0 : it->second) << std::endl;
                        }
                    }
                    for (const auto &metric : aggregated_metrics)
                        std::cout << std::fixed << "\t\t" << metric.first << ": " << metric.second << std::endl;
                }

                for (size_t sample_idx = 0; sample_idx < this->read_buffer.size(); ++sample_idx)
                {
                    const auto &sample = this->read_buffer[sample_idx];
                    const double duration_ms = sample.first;
                    const std::unordered_map<std::string, uint64_t> sample_counts =
                        event_counts_from_sample(event_names, sample.second, duration_ms);
                    const auto sample_metrics = this->metric_builder.calculate(sample_counts);

                    std::cout << std::fixed << "\t[Sample " << sample_idx << "] duration_ms: " << duration_ms << std::endl;

                    if (OPT_UNLIKELY(this->metric_builder.print_events))
                    {
                        for (const auto &name : unique_event_names)
                        {
                            auto it = sample_counts.find(name);
                            std::cout << std::fixed << "\t\t" << name << ": " << ((it == sample_counts.end()) ? 0 : it->second) << std::endl;
                        }
                    }

                    for (const auto &metric : sample_metrics)
                        std::cout << std::fixed << "\t\t" << metric.first << ": " << metric.second << std::endl;
                }
            }
            else
            {
                std::cout << std::fixed << "\033[1;33m"
                          << "Block: " << this->config.block_name << "\033[0m"
                          << " [" << this->total_duration_ms << "ms] Measured\n";

                if (OPT_UNLIKELY(this->metric_builder.print_events))
                    for (auto &&event : this->event_results)
                        std::cout << std::fixed << "\t" << event.first << ": " << event.second << std::endl;

                std::cout << "\tMetrics: \n";
                for (auto &&metric : this->metric_results)
                    std::cout << std::fixed << "\t\t" << metric.first << ": " << metric.second << std::endl;
            }
        }
    }

    // Read delta counters for the keys_to_read and accumulate into results
    std::vector<uint64_t> IoDiskProfiler::read()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};
        std::vector<uint64_t> result;
        auto curr_snapshot = read_selected_io_counters();
        const std::vector<std::string> &event_names = this->metric_builder.event_names();
        for (const auto &key : event_names)
        {
            // std::cout << "key:" << key << "\n";
            uint64_t curr_val = curr_snapshot.at(key);
            uint64_t prev_val = last_snapshot.at(key);

            // Calculate delta, careful with possible counter reset (wraparound)
            uint64_t delta = curr_val - prev_val;

            // std::cout << "\t" << "cur_val:" << curr_val << " - " << "prev_val:" << prev_val << " delta:" << delta << "\n";

            // Update last snapshot with current one.
            last_snapshot.at(key) = curr_val;
            result.push_back(delta); // store the delta
        }
        return result;
    }

    std::unordered_map<std::string, uint64_t> IoDiskProfiler::aggregate()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};
        double total_duration = this->compacted_duration_ms;
        std::unordered_map<std::string, uint64_t> aggregated_events = this->compacted_event_counts;
        const std::vector<std::string> &event_names = this->metric_builder.event_names();

        double buffered_duration = 0.0;
        for (size_t index = 0; index < read_buffer.size(); ++index)
            buffered_duration += read_buffer[index].first;

        const std::unordered_map<std::string, uint64_t> buffered_counts =
            aggregate_counts_from_read_buffer(event_names, this->read_buffer, buffered_duration);

        total_duration += buffered_duration;
        for (std::unordered_map<std::string, uint64_t>::const_iterator it = buffered_counts.begin(); it != buffered_counts.end(); ++it)
        {
            if (it->first == "duration_microsec")
                continue;
            aggregated_events[it->first] += it->second;
        }
        std::vector<std::pair<std::string, uint64_t>> event_value(
            aggregated_events.begin(), aggregated_events.end());

        this->event_results = event_value;
        this->total_duration_ms = total_duration;

        aggregated_events["duration_microsec"] = this->total_duration_ms * 1000.0; // convert to microseconds
        return aggregated_events;
    }

    std::string IoDiskProfiler::to_json()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};

        if (OPT_UNLIKELY(this->config.is_screenshot))
        {
            nlohmann::json out;

            const std::vector<std::string> &event_names = this->metric_builder.event_names();
            const auto unique_event_names = unique_event_names_from(event_names);

            if (!this->read_buffer.empty())
            {
                const std::unordered_map<std::string, uint64_t> aggregated_counts =
                    aggregate_counts_from_read_buffer(event_names, this->read_buffer, this->total_duration_ms);
                const auto aggregated_event_values = event_values_from_counts(unique_event_names, aggregated_counts);
                const auto aggregated_metric_values = this->metric_builder.calculate(aggregated_counts);
                append_reading_json(out,
                                   this->total_duration_ms,
                                   this->config.measurement_type,
                                   aggregated_event_values,
                                   aggregated_metric_values);
            }

            for (const auto &sample : this->read_buffer)
            {
                const double duration_ms = sample.first;
                const std::unordered_map<std::string, uint64_t> sample_counts =
                    event_counts_from_sample(event_names, sample.second, duration_ms);
                const auto event_values = event_values_from_counts(unique_event_names, sample_counts);
                const auto metric_values = this->metric_builder.calculate(sample_counts);

                append_reading_json(out,
                                   duration_ms,
                                   this->config.measurement_type,
                                   event_values,
                                   metric_values);
            }

            std::stringstream ss;
            ss << "[\n";
            ss << out.dump(2);
            ss << "\n]\n";
            return ss.str();
        }
        std::stringstream ss;
        ss << "[\n";
        ss << utils::to_json<uint64_t>(this->total_duration_ms, this->config.measurement_type, this->event_results, this->metric_results);
        ss << "]\n";
        return ss.str();
    }

    // Your keys of interest
    std::unordered_map<std::string, uint64_t> IoDiskProfiler::read_selected_io_counters()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};
        std::unordered_map<std::string, uint64_t> results;
        std::unordered_map<std::string, bool> wanted;
        for (const auto &k : this->metric_builder.event_names())
        {
            wanted[k] = true;
            results[k] = 0;
        }

        const std::string content = utils::read_file("/proc/self/io");
        std::istringstream iss(content);
        std::string line;

        while (std::getline(iss, line))
        {
            auto pos = line.find(':');
            if (pos == std::string::npos)
                continue;

            std::string key = line.substr(0, pos);
            if (wanted.find(key) == wanted.end())
                continue;

            std::string val_str = line.substr(pos + 1);
            results[key] = std::stoull(val_str);
        }

        return results;
    }
}
