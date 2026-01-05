
#include "core/pmu/gpu/amd/block_profiler.hh"

#if OPTKIT_ENV_LIB_NVML
namespace optkit::pmu::gpu::amd
{

    CUpti_ActivityKernel9 *g_activity_kernel = nullptr;

    // Callback for buffer requests
    static void BufferRequested(uint8_t **buffer, size_t *size, size_t *maxNumRecords)
    {
        *size = 8 * 1024 * 1024; // 8MB buffer
        *maxNumRecords = 0;
        *buffer = (uint8_t *)malloc(*size);
    }

    // Callback for buffer completed
    static void BufferCompleted(CUcontext ctx, uint32_t streamId, uint8_t *buffer, size_t size, size_t validSize)
    {
        CUpti_Activity *record = NULL;

        if (validSize > 0)
        {
            // Parse CUPTI activity records here, print kernel name and duration
            while (cuptiActivityGetNextRecord(buffer, validSize, &record) == CUPTI_SUCCESS)
            {
                if (record->kind == CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL)
                {
                    g_activity_kernel = (CUpti_ActivityKernel9 *)record;
                }
            }
        }
        free(buffer);
    }

    BlockProfiler::BlockProfiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<uint64_t> &mb)
        : BaseProfiler{static_cast<const ProfilerConfig &>(profiler_config)}, profiler_config{profiler_config}, metric_builder{mb}
    {
        start = std::chrono::high_resolution_clock::now();
        std::vector<std::string> event_names = mb.event_names();

        CUPTI_API_CALL(cuptiActivityRegisterCallbacks(BufferRequested, BufferCompleted));
        CUPTI_API_CALL(cuptiActivityEnable(CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL));
    }

    BlockProfiler ::~BlockProfiler()
    {

        CUPTI_API_CALL(cuptiActivityFlushAll(1));
        CUPTI_API_CALL(cuptiActivityDisable(CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL));

        printf("activity kind=%d\n", g_activity_kernel->kind);
        printf("kernel name = %s\n", g_activity_kernel->name);
        printf("kernel duration (ns) = %llu\n", (unsigned long long)(g_activity_kernel->end - g_activity_kernel->start));
        return;

        this->read_and_store();
        this->metric_results = this->metric_builder.calculate(aggregate());
        if (OPT_LIKELY(this->config.dump_results_to_file))
            this->save();

        if (OPT_LIKELY(this->config.verbose))
        {
            std::cout << "\033[1;35m"
                      << "Block: " << this->config.block_name << "\033[0m"
                      << " [" << this->total_duration_ms << "ms] Measured\n";

            if (OPT_UNLIKELY(this->metric_builder.print_events))
                for (auto &&event : this->event_results)
                    std::cout << std::fixed << "\t" << event.first << ": " << event.second << std::endl;

            for (auto &&metric : this->metric_results)
                std::cout << std::fixed << "\t" << metric.first << ": " << metric.second << std::endl;
        }
    }

    void BlockProfiler::disable()
    {
    }
    void BlockProfiler::enable()
    {
    }

    void BlockProfiler::reset()
    {
    }

    std::string BlockProfiler::to_json()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};

        std::stringstream ss;
        ss << "[\n";
        // based on the insertion order.
        ss << utils::to_json<uint64_t>(this->total_duration_ms, this->config.measurement_type, this->event_results, this->metric_results);
        ss << "]\n";
        return ss.str();
    }

    std::vector<uint64_t> BlockProfiler::read()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};

        std::vector<uint64_t> result;
        return result;
    }
    std::unordered_map<std::string, uint64_t> BlockProfiler::aggregate()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};
        double total_duration = 0.0;
        std::unordered_map<std::string, uint64_t> aggregated_events;
        const std::vector<std::string> &event_names = this->metric_builder.event_names();

        for (const auto &entry : read_buffer)
        {
            total_duration += entry.first;

            const std::vector<uint64_t> &values = entry.second;

            // std::cout << "read buffer:";
            for (size_t j = 0; j < values.size(); ++j)
            {
                // std::cout << event_names[j] << ":" << values[j] << "\n";
                aggregated_events[event_names[j]] += values[j];
            }
        }
        std::vector<std::pair<std::string, uint64_t>> event_value(
            aggregated_events.begin(), aggregated_events.end());

        this->event_results = std::move(event_value);
        this->total_duration_ms = total_duration;

        aggregated_events["duration_microsec"] = this->total_duration_ms * 1000.0; // convert to microseconds
        return aggregated_events;
    }

} // namespace optkit::pmu::gpu::amd

#endif