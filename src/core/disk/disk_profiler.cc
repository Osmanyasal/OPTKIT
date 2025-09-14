#include "core/disk/disk_profiler.hh"

namespace optkit::disk
{
    IoDiskProfiler::IoDiskProfiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<uint64_t> &mb)
        : BaseProfiler(profiler_config), metric_builder(mb)
    {
        last_snapshot = read_selected_io_counters();
    }

    IoDiskProfiler::~IoDiskProfiler()
    {
        this->read_and_store();
        this->metric_results = this->metric_builder.calculate(aggregate());

        if (OPT_LIKELY(Query::create_folder))
            this->save();

        if (OPT_LIKELY(this->config.verbose))
        {
            std::cout << std::fixed << "\033[1;35m"
                      << "Block: " << this->config.block_name << "\033[0m"
                      << " [" << this->total_duration_ms << "ms] Measured\n";

            if (OPT_UNLIKELY(this->metric_builder.print_events))
                for (auto &&event : this->event_results)
                    std::cout << std::fixed << "\t" << event.first << ": " << event.second << std::endl;

            for (auto &&metric : this->metric_results)
                std::cout << std::fixed << "\t" << metric.first << ": " << metric.second << std::endl;
        }
    }

    // Read delta counters for the keys_to_read and accumulate into results
    std::vector<uint64_t> IoDiskProfiler::read()
    {
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
        double total_duration = 0.0;
        std::unordered_map<std::string, uint64_t> aggregated_events;
        const std::vector<std::string> &event_names = this->metric_builder.event_names();

        for (const auto &entry : read_buffer)
        {
            total_duration += entry.first;

            const std::vector<uint64_t> &values = entry.second;

            for (size_t j = 0; j < values.size(); ++j)
            {
                aggregated_events[event_names[j]] += values[j];
            }
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
        std::stringstream ss;
        ss << "[\n";
        ss << utils::to_json<uint64_t>(this->total_duration_ms, this->config.measurement_type, this->event_results, this->metric_results);
        ss << "]\n";
        return ss.str();
    }

    // Your keys of interest
    std::unordered_map<std::string, uint64_t> IoDiskProfiler::read_selected_io_counters()
    {
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
