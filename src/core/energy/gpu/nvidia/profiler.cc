#include "core/energy/gpu/nvidia/profiler.hh"

namespace optkit::energy::gpu::nvidia
{
    // this is the sampling function that runs in a separate thread
    // it accumulates power readings every sampling_frequency_sec seconds
    // unordered-map stands for device-id <-> {power (Watts)}
    OPT_FORCE_INLINE void sampling_function(std::unordered_map<uint32_t, double> &snapshot, uint32_t sampling_frequency_sec = 1) // in seconds
    {
        uint32_t device_count;
        optkit::gpu::GpuVendor vendor = optkit::gpu::GpuVendor::NVIDIA;
        if (!optkit::gpu::Query::get_device_count(vendor, device_count))
        {
            OPTKIT_ERROR("Failed to get device count for NVIDIA GPUs.");
            return;
        }
        for (uint32_t i = 0; i < device_count; i++)
        {
            double power_watts = 0.0;
            if (OPT_LIKELY(optkit::gpu::Query::get_device_power(vendor, i, power_watts)))
            {
                snapshot[i] = power_watts * sampling_frequency_sec; // power in Watts * time in seconds = energy in Joules
                // std::cout << "snapshot[" << i << "] = " << snapshot[i] << " Joules\n"; // debug
            }
            else
            {
                snapshot[i] = 0; // Not supported
                OPTKIT_WARN("Power monitoring not supported for GPU {}", i);
            }
        }
    }
    Profiler::Profiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<double> &mb, const uint32_t sampling_frequency_sec)
        : BaseProfiler{profiler_config}, metric_builder{mb}, sampling_frequency_sec{sampling_frequency_sec}, sampling_counter{0}, is_sampling{true}
    {
        auto vendor = optkit::gpu::GpuVendor::NVIDIA;
        uint32_t device_count = 0;
        optkit::gpu::Query::get_device_count(vendor, device_count);
        if (device_count == 0)
        {
            std::cout << "No NVIDIA GPUs found. Disabling NVIDIA GPU Energy Profiler.\n";
            this->is_enabled = false;
            return;
        }
        this->sampling_thread = std::thread([this]()
                                            {
                                            while (this->is_sampling)
                                            { 
                                                this->read_and_store();
                                                this->sampling_counter++;
                                                std::this_thread::sleep_for(std::chrono::seconds(this->sampling_frequency_sec));
                                            } });
        OPTKIT_INFO("Initialized NVIDIA GPU Energy Profiler with sampling frequency: {} seconds.", sampling_frequency_sec);
    }

    Profiler::~Profiler()
    {
        if (!this->is_enabled)
            return;

        this->is_sampling = false;             // stop sampling thread
        this->sampling_thread.join();          // wait for it to join.
        this->read_and_store();                // read any remaining samples
        auto aggregated_results = aggregate(); // make sure to aggregate before calculating metrics

        // Parse metric names like "gpu_0" into metric type and device ID
        // Calculate metrics per device, then store with device suffix
        std::unordered_map<std::string, std::unordered_map<uint32_t, double>> metrics_by_type;

        // Group devices by metric type (e.g., "gpu_0", "gpu_1" -> "gpu")
        for (const auto &metric_pair : aggregated_results)
        {
            const std::string &full_metric_name = metric_pair.first;

            // Find first underscore to separate metric type from device ID
            size_t underscore_pos = full_metric_name.find('_');
            if (underscore_pos != std::string::npos)
            {
                std::string metric_type = full_metric_name.substr(0, underscore_pos);
                std::string device_suffix = full_metric_name.substr(underscore_pos + 1);
                uint32_t device_id = std::stoul(device_suffix);

                // Store energy value for this device under the metric type
                for (const auto &device_pair : metric_pair.second)
                {
                    metrics_by_type[metric_type][device_id] = device_pair.second;
                }
            }
        }

        // Calculate metrics for each metric type and store results per device
        this->metric_results.clear();
        for (const auto &type_pair : metrics_by_type)
        {
            const std::string &metric_type = type_pair.first;

            // For each device, create input map and calculate metrics
            for (const auto &device_pair : type_pair.second)
            {
                uint32_t device_id = device_pair.first;
                double energy_value = device_pair.second;

                // Create input map for metric calculation
                std::unordered_map<std::string, double> device_input;
                device_input[metric_type] = energy_value;

                // Calculate metrics for this device
                auto device_metrics = this->metric_builder.calculate(device_input);

                // Store results with device suffix
                for (const auto &result_pair : device_metrics)
                {
                    std::string result_key = result_pair.first + "_" + std::to_string(device_id);
                    this->metric_results.emplace_back(result_key, result_pair.second);
                }
            }
        }

        // this->metric_results = this->metric_builder.calculate(aggregate());

        if (OPT_LIKELY(Query::create_folder))
            this->save();

        if (OPT_LIKELY(this->config.verbose))
        {
            std::cout << std::fixed << "\033[1;33m" // Yellow for temperature
                      << "Block: " << this->config.block_name << ":" << this->config.measurement_type << "\033[0m"
                      << " [" << this->total_duration_ms << "ms] Measured\n";

            if (OPT_UNLIKELY(this->metric_builder.print_events))
                for (auto &&event : this->event_results)
                {
                    for (auto &&device : event.second)
                        std::cout << "GPU[" << device.first << "]=" << device.second << " Joules ";
                    std::cout << std::endl;
                }

            for (auto &&metric : this->metric_results)
                std::cout << std::fixed << "\t" << metric.first << ": " << metric.second << std::endl;
        }
    }

    std::unordered_map<uint32_t, double> Profiler::read()
    {
        if (!this->is_enabled)
            return {};

        optkit::energy::gpu::nvidia::sampling_function(this->snapshot, this->sampling_frequency_sec);
        return this->snapshot;
    }

    std::unordered_map<std::string, std::unordered_map<uint32_t, double>> Profiler::aggregate()
    {
        if (!this->is_enabled)
            return {};

        double total_duration = 0.0;
        std::unordered_map<std::string, std::unordered_map<uint32_t, double>> aggregated_events;
        const std::vector<std::string> &event_names = this->metric_builder.event_names();

        for (const auto &entry : read_buffer)
        {
            total_duration += entry.first;
            const auto &values = entry.second;

            for (auto &&i : values)
            {
                for (auto &&event_name : event_names)
                {
                    std::string key = event_name + "_" + std::to_string(i.first);
                    aggregated_events[key][i.first] += i.second;
                    std::cout << std::fixed << key << " Aggregated GPU[" << i.first << "] += " << aggregated_events[key][i.first] << " Joules\n"; // debug
                }
            }
        }
        std::vector<std::pair<std::string, std::unordered_map<uint32_t, double>>> event_value(
            aggregated_events.begin(), aggregated_events.end());

        this->event_results = event_value;
        this->total_duration_ms = total_duration;

        return aggregated_events;
    }

    std::string Profiler::to_json()
    {
        if (!this->is_enabled)
            return {};

        std::stringstream ss;
        ss << "[\n";
        ss << utils::to_json<std::unordered_map<uint32_t, double>>(this->total_duration_ms, this->config.measurement_type, this->event_results, this->metric_results);
        ss << "]\n";
        return ss.str();
    }
} // namespace optkit::energy::gpu::nvidia