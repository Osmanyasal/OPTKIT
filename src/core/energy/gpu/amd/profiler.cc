#include "core/energy/gpu/amd/profiler.hh"

namespace optkit::energy::gpu::amd
{
    // this is the sampling function that runs in a separate thread
    // it accumulates power readings every sampling_frequency_sec seconds
    // unordered-map stands for device-id <-> {power (Watts)}
    OPT_FORCE_INLINE void sampling_function(std::unordered_map<uint32_t, double> &snapshot, uint32_t sampling_frequency_sec = 1) // in seconds
    {
        uint32_t device_count;
        auto vendor = optkit::utils::GpuDevice::AMD;
        if (!optkit::gpu::Query::get_device_count(vendor, device_count))
        {
            OPTKIT_ERROR("Failed to get device count for AMD GPUs.");
            return;
        }
        for (uint32_t i = 0; i < device_count; i++)
        {
            amdsmi_processor_handle device = Query::gpu_handles_amdsmi.at(i);
            uint64_t energy_accumulator = 0.0;
            float counter_resolution = 0.0;
            uint64_t time_stamp = 0.0;

            ROCM_EXEC_IF_SUPPORTS(
                "amdsmi_get_energy_count",
                device,
                result,
                &energy_accumulator,
                &counter_resolution,
                &time_stamp);

            snapshot[i] = snapshot[i - 1] - energy_accumulator * 1e-6; // micro Joules to Joules
        }
    }
    Profiler::Profiler(const ProfilerConfig &profiler_config, const uint32_t sampling_frequency_sec, const optkit::metrics::MetricBuilder<std::unordered_map<uint32_t, double>> &mb)
        : BaseProfiler{profiler_config}, metric_builder{mb}, sampling_frequency_sec{sampling_frequency_sec}, sampling_counter{0}, is_sampling{true}
    {

        metric_builder = {};
        const static uint32_t device_count = optkit::gpu::Query::get_device_count();
        for (uint32_t i = 0; i < device_count; i++)
        {
            metric_builder.add("gpu[" + std::to_string(i) + "]", {0x0});
        }

        // we need to record the initial energy count to calculate deltas later
        for (uint32_t i = 0; i < device_count; i++)
        {
            amdsmi_processor_handle device = Query::gpu_handles_amdsmi.at(i);
            uint64_t energy_accumulator = 0.0;
            float counter_resolution = 0.0;
            uint64_t time_stamp = 0.0;

            ROCM_EXEC_IF_SUPPORTS(
                "amdsmi_get_energy_count",
                device,
                result,
                &energy_accumulator,
                &counter_resolution,
                &time_stamp);

            snapshot[i] = energy_accumulator * 1e-6; // micro Joules to Joules
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
        this->read_and_store();
        this->is_sampling = false;    // stop sampling thread
        this->sampling_thread.join(); // wait for it to join.
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
        optkit::energy::gpu::nvidia::sampling_function(this->snapshot, this->sampling_frequency_sec);
        return this->snapshot;
    }

    std::unordered_map<std::string, std::unordered_map<uint32_t, double>> Profiler::aggregate()
    {
        double total_duration = 0.0;
        std::unordered_map<std::string, std::unordered_map<uint32_t, double>> aggregated_events;
        const std::vector<std::string> &event_names = this->metric_builder.event_names();

        for (const auto &entry : read_buffer)
        {
            total_duration += entry.first;
            const auto &values = entry.second;

            for (auto &&i : values)
            {
                // std::cout << std::fixed << "adding..:" << i.second << " Joules \n";
                aggregated_events[event_names[i.first]][i.first] += i.second;
                // std::cout << std::fixed << "Aggregated GPU[" << i.first << "] += " << aggregated_events[event_names[i.first]][i.first] << " Joules\n"; // debug
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
        std::stringstream ss;
        ss << "[\n";
        ss << utils::to_json<std::unordered_map<uint32_t, double>>(this->total_duration_ms, this->config.measurement_type, this->event_results, this->metric_results);
        ss << "]\n";
        return ss.str();
    }
} // namespace optkit::energy::gpu::amd