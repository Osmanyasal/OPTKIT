#include "core/temperature/gpu/profiler.hh"
namespace optkit::temperature::gpu
{
    namespace
    {
        constexpr double READ_BUFFER_FLUSH_PERIOD_MS = 5000.0;

        static std::unordered_map<std::string, std::pair<double, double>> event_counts_from_sample(
            const std::vector<std::string> &event_names,
            const std::vector<std::pair<double, double>> &values)
        {
            std::unordered_map<std::string, std::pair<double, double>> counts;
            for (size_t j = 0; j < values.size(); ++j)
            {
                counts[event_names[j % event_names.size()]].first += values[j].first;
                counts[event_names[j % event_names.size()]].second += values[j].second;
            }
            return counts;
        }
    }

    Profiler::Profiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<std::pair<double, double>> &mb)
        : BaseProfiler(profiler_config), metric_builder(mb)
    {
        // Define array of vendors to support
        std::vector<optkit::gpu::GpuVendor> vendors;
        for (optkit::gpu::GpuVendor vendor = optkit::gpu::GpuVendor::BEGIN; vendor < optkit::gpu::GpuVendor::END; vendor = static_cast<optkit::gpu::GpuVendor>(static_cast<int>(vendor) + 1))
        {
            vendors.push_back(vendor);
        }

        uint32_t device_index = 0;
        // Initialize temperature snapshots for all vendors and their devices
        for (std::vector<optkit::gpu::GpuVendor>::const_iterator vendor_it = vendors.begin();
             vendor_it != vendors.end(); ++vendor_it)
        {
            optkit::gpu::GpuVendor vendor = *vendor_it;
            uint32_t device_count;

            if (optkit::gpu::Query::get_device_count(vendor, device_count))
            {
                for (uint32_t i = 0; i < device_count; i++)
                {
                    double temp_device_celsius = 0.0;
                    double temp_mem_celsius = 0.0;
                    if (optkit::gpu::Query::get_device_temperature(vendor, i, temp_device_celsius, temp_mem_celsius))
                    {
                        last_snapshot[device_index] = std::make_pair(temp_device_celsius, temp_mem_celsius);
                        // std::cout << "Initial snapshot: " << device_index << " -> " << temp_device_celsius << "°C (GPU), " << temp_mem_celsius << "°C (Memory)" << std::endl;
                    }
                    else
                        last_snapshot[device_index] = std::make_pair(0.0, 0.0); // Default for unsupported devices
                    device_index++;
                }
            }
        }

        // Initialize metric builder and add GPU metrics to default
        metric_builder = optkit::metrics::MetricBuilder<std::pair<double, double>>();
        for (std::unordered_map<uint32_t, std::pair<double, double>>::const_iterator it = last_snapshot.begin();
             it != last_snapshot.end(); ++it)
            metric_builder.add("gpu[" + std::to_string(it->first) + "]", {0x0});
    }

    Profiler::~Profiler()
    {
        this->read_and_store();
        this->metric_results = this->metric_builder.calculate(aggregate());

        if (OPT_LIKELY(Query::create_folder))
            this->save();

        if (OPT_LIKELY(this->config.verbose))
        {
            std::cout << std::fixed << "\033[1;33m" // Yellow for temperature
                      << "Block: " << this->config.block_name << "\033[0m"
                      << " [" << this->total_duration_ms << "ms] Measured\n";

            if (OPT_UNLIKELY(this->metric_builder.print_events))
                for (auto &&event : this->event_results)
                    std::cout << std::fixed << "\t" << event.first << ": " << event.second.first << "°C (GPU), " << event.second.second << "°C (Memory)" << std::endl;

            std::cout << "\tMetrics: \n";
            for (auto &&metric : this->metric_results)
                std::cout << std::fixed << "\t\t" << metric.first << ": " << metric.second << std::endl;
        }
    }

    std::vector<std::pair<double, double>> Profiler::read()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};
        // Define array of vendors to support (same as constructor)
        std::vector<optkit::gpu::GpuVendor> vendors;
        vendors.push_back(optkit::gpu::GpuVendor::NVIDIA);
        vendors.push_back(optkit::gpu::GpuVendor::AMD);

        std::unordered_map<uint32_t, std::pair<double, double>> current_snapshot;
        uint32_t device_index = 0;

        // Read temperature from all vendors and their devices
        for (std::vector<optkit::gpu::GpuVendor>::const_iterator vendor_it = vendors.begin();
             vendor_it != vendors.end(); ++vendor_it)
        {
            optkit::gpu::GpuVendor vendor = *vendor_it;
            uint32_t device_count;

            if (optkit::gpu::Query::get_device_count(vendor, device_count))
            {
                for (uint32_t i = 0; i < device_count; i++)
                {
                    double temp_device_celsius = 0.0;
                    double temp_mem_celsius = 0.0;
                    if (optkit::gpu::Query::get_device_temperature(vendor, i, temp_device_celsius, temp_mem_celsius))
                    {
                        current_snapshot[device_index] = std::make_pair(temp_device_celsius, temp_mem_celsius);
                        // std::cout << "Initial snapshot: " << device_index << " -> " << temp_device_celsius << "°C (GPU), " << temp_mem_celsius << "°C (Memory)" << std::endl;
                    }
                    else
                        current_snapshot[device_index] = std::make_pair(0.0, 0.0); // Default for unsupported devices
                    device_index++;
                }
            }
        }

        std::vector<std::pair<double, double>> current_temps;
        for (const auto &cs : current_snapshot)
        {
            // std::cout << "Current snapshot: " << cs.first << " -> " << cs.second << std::endl;
            double curr_gpu_temp_val = current_snapshot.at(cs.first).first;
            double prev_gpu_temp_val = last_snapshot.at(cs.first).first;
            double delta_gpu_temp_val = curr_gpu_temp_val - prev_gpu_temp_val;

            double curr_mem_temp_val = current_snapshot.at(cs.first).second;
            double prev_mem_temp_val = last_snapshot.at(cs.first).second;
            double delta_mem_temp_val = curr_mem_temp_val - prev_mem_temp_val;

            last_snapshot = current_snapshot;
            current_temps.push_back(std::make_pair(delta_gpu_temp_val, delta_mem_temp_val)); // store the delta
        }

        return current_temps;
    }

    void Profiler::on_sample_stored(const std::pair<double, std::vector<std::pair<double, double>>> &sample)
    {
        this->buffered_duration_ms += sample.first;
        if (this->buffered_duration_ms < READ_BUFFER_FLUSH_PERIOD_MS)
            return;

        flush_compacted_samples();
    }

    void Profiler::flush_compacted_samples()
    {
        if (this->read_buffer.empty())
            return;

        const std::vector<std::string> &event_names = this->metric_builder.event_names();
        for (size_t index = 0; index < this->read_buffer.size(); ++index)
        {
            const std::unordered_map<std::string, std::pair<double, double>> sample_counts =
                event_counts_from_sample(event_names, this->read_buffer[index].second);
            for (std::unordered_map<std::string, std::pair<double, double>>::const_iterator it = sample_counts.begin(); it != sample_counts.end(); ++it)
            {
                this->compacted_event_counts[it->first].first += it->second.first;
                this->compacted_event_counts[it->first].second += it->second.second;
            }
        }

        this->compacted_duration_ms += this->buffered_duration_ms;
        this->buffered_duration_ms = 0.0;
        this->read_buffer.clear();
    }

    std::unordered_map<std::string, std::pair<double, double>> Profiler::aggregate()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};
        double total_duration = this->compacted_duration_ms;
        std::unordered_map<std::string, std::pair<double, double>> aggregated_events = this->compacted_event_counts;
        const std::vector<std::string> &event_names = this->metric_builder.event_names();

        for (size_t index = 0; index < read_buffer.size(); ++index)
        {
            total_duration += read_buffer[index].first;
            const std::unordered_map<std::string, std::pair<double, double>> sample_counts =
                event_counts_from_sample(event_names, read_buffer[index].second);
            for (std::unordered_map<std::string, std::pair<double, double>>::const_iterator it = sample_counts.begin(); it != sample_counts.end(); ++it)
            {
                aggregated_events[it->first].first += it->second.first;
                aggregated_events[it->first].second += it->second.second;
            }
        }
        std::vector<std::pair<std::string, std::pair<double, double>>> event_value(
            aggregated_events.begin(), aggregated_events.end());

        this->event_results = event_value;
        this->total_duration_ms = total_duration;

        return aggregated_events;
    }

    std::string Profiler::to_json()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};
        std::stringstream ss;
        ss << "[\n";
        ss << utils::to_json<std::pair<double, double>>(this->total_duration_ms, this->config.measurement_type, this->event_results, this->metric_results);
        ss << "]\n";
        return ss.str();
    }
}