#include "core/temperature/gpu/profiler.hh"
namespace optkit::temperature::gpu
{
    Profiler::Profiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<double> &mb)
        : BaseProfiler(profiler_config), metric_builder(mb)
    {
        // Define array of vendors to support
        std::vector<optkit::gpu::GpuVendor> vendors;
        vendors.push_back(optkit::gpu::GpuVendor::NVIDIA);
        vendors.push_back(optkit::gpu::GpuVendor::AMD);

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
                    double temp_celsius;
                    if (optkit::gpu::Query::get_device_temperature(vendor, i, temp_celsius))
                    {
                        last_snapshot[device_index] = temp_celsius;
                        // std::cout << "Initial snapshot: " << device_index << " -> " << temp_celsius << std::endl;
                    }
                    else
                    {
                        last_snapshot[device_index] = 0.0; // Default for unsupported devices
                    }
                    device_index++;
                }
            }
        }

        // Initialize metric builder and add GPU metrics
        metric_builder = optkit::metrics::MetricBuilder<double>();
        for (std::unordered_map<uint32_t, double>::const_iterator it = last_snapshot.begin();
             it != last_snapshot.end(); ++it)
        {
            std::vector<uint64_t> init_value;
            init_value.push_back(0x0);
            metric_builder.add("gpu_" + std::to_string(it->first), init_value);
        }
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
                    std::cout << std::fixed << "\t" << event.first << ": " << event.second << "°C" << std::endl;

            for (auto &&metric : this->metric_results)
                std::cout << std::fixed << "\t" << metric.first << ": " << metric.second << std::endl;
        }
    }

    std::vector<double> Profiler::read()
    {
        // Define array of vendors to support (same as constructor)
        std::vector<optkit::gpu::GpuVendor> vendors;
        vendors.push_back(optkit::gpu::GpuVendor::NVIDIA);
        vendors.push_back(optkit::gpu::GpuVendor::AMD);

        std::unordered_map<uint32_t, double> current_snapshot;
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
                    double temp_celsius;
                    if (optkit::gpu::Query::get_device_temperature(vendor, i, temp_celsius))
                    {
                        current_snapshot[device_index] = temp_celsius;
                    }
                    else
                    {
                        current_snapshot[device_index] = 0.0; // Default for unsupported devices
                    }
                    device_index++;
                }
            }
        }

        std::vector<double> current_temps;
        for (const auto &cs : current_snapshot)
        {
            // std::cout << "Current snapshot: " << cs.first << " -> " << cs.second << std::endl;
            double curr_val = current_snapshot.at(cs.first);
            double prev_val = last_snapshot.at(cs.first);

            double delta = curr_val - prev_val;
            last_snapshot.at(cs.first) = curr_val;
            current_temps.push_back(delta); // store the delta
        }

        return current_temps;
    }

    std::unordered_map<std::string, double> Profiler::aggregate()
    {
        double total_duration = 0.0;
        std::unordered_map<std::string, double> aggregated_events;
        const std::vector<std::string> &event_names = this->metric_builder.event_names();

        for (const auto &entry : read_buffer)
        {
            total_duration += entry.first;

            const std::vector<double> &values = entry.second;

            for (size_t j = 0; j < values.size(); ++j)
            {
                aggregated_events[event_names[j]] += values[j];
            }
        }
        std::vector<std::pair<std::string, double>> event_value(
            aggregated_events.begin(), aggregated_events.end());

        this->event_results = event_value;
        this->total_duration_ms = total_duration;

        return aggregated_events;
    }

    std::string Profiler::to_json()
    {
        std::stringstream ss;
        ss << "[\n";
        ss << utils::to_json<double>(this->total_duration_ms, this->config.measurement_type, this->event_results, this->metric_results);
        ss << "]\n";
        return ss.str();
    }
}