#include "core/pmu/gpu/nvidia/block_profiler.hh"

#if OPTKIT_ENV_LIB_NVML
#include <memory>
#include <cupti_version.h>

namespace optkit::pmu::gpu::nvidia
{
#if defined(CUPTI_API_VERSION) && (CUPTI_API_VERSION >= 16)
    using ActivityKernel = CUpti_ActivityKernel6;
    using ActivityMemcpy = CUpti_ActivityMemcpy6;
#else
    using ActivityKernel = CUpti_ActivityKernel4;
    using ActivityMemcpy = CUpti_ActivityMemcpy4;
#endif

    static std::unique_ptr<ActivityKernel> g_activity_kernel = nullptr;
    static std::vector<std::unique_ptr<ActivityMemcpy>> g_activity_memcpy;

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
                    ActivityKernel *kernel = (ActivityKernel *)record;

                    // Allocate and copy kernel data before buffer is freed
                    if (g_activity_kernel == nullptr)
                    {
                        g_activity_kernel.reset(new ActivityKernel());
                    }
                    memcpy(g_activity_kernel.get(), kernel, sizeof(ActivityKernel));
                }
                else if (record->kind == CUPTI_ACTIVITY_KIND_MEMCPY)
                {
                    ActivityMemcpy *memcpyCmd = (ActivityMemcpy *)record;

                    std::unique_ptr<ActivityMemcpy> ptr{new ActivityMemcpy()};
                    memcpy(ptr.get(), memcpyCmd, sizeof(ActivityMemcpy));
                    g_activity_memcpy.push_back(std::move(ptr));
                    // print those
                    // std::cout << "Memcpy of " << g_activity_memcpy.back()->bytes << " bytes from "
                    //           << getMemKindString((CUpti_ActivityMemoryKind)g_activity_memcpy.back()->srcKind)
                    //           << " to " << getMemKindString((CUpti_ActivityMemoryKind)g_activity_memcpy.back()->dstKind)
                    //           << " took " << (g_activity_memcpy.back()->end - g_activity_memcpy.back()->start) * 1e-6 << " ms\n";
                }
            }
        }
        free(buffer);
    }

    BlockProfiler::BlockProfiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<std::string> &mb)
        : BaseProfiler{static_cast<const ProfilerConfig &>(profiler_config)}, profiler_config{profiler_config}, metric_builder{mb}
    {
        start = std::chrono::high_resolution_clock::now();
        std::vector<std::string> event_names = mb.event_names();

        CUPTI_API_CALL(cuptiActivityRegisterCallbacks(BufferRequested, BufferCompleted));
        CUPTI_API_CALL(cuptiActivityEnable(CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL));
        CUPTI_API_CALL(cuptiActivityEnable(CUPTI_ACTIVITY_KIND_MEMCPY));
    }

    struct MemcpyEvent
    {
        double duration_ms;
        uint64_t bytes;
        std::string src_memory_kind;
        std::string dst_memory_kind;
        uint64_t copy_kind;

        std::string to_string() const
        {
            std::stringstream ss;
            ss << "{duration_ms:" << duration_ms << ", "
               << "bytes:" << bytes << ", "
               << "src_kind:" << src_memory_kind << ", "
               << "dst_kind:" << dst_memory_kind << ", "
               << "copy_kind:" << copy_kind << "}";
            return ss.str();
        }
    };

    BlockProfiler ::~BlockProfiler()
    {
        CUPTI_API_CALL(cuptiActivityFlushAll(1));
        CUPTI_API_CALL(cuptiActivityDisable(CUPTI_ACTIVITY_KIND_MEMCPY));
        CUPTI_API_CALL(cuptiActivityDisable(CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL));
        this->read_and_store();
        this->metric_results = this->metric_builder.calculate(aggregate());
        this->event_results.push_back({"kernel_duration_ms", g_activity_kernel != nullptr ? std::to_string((g_activity_kernel->end - g_activity_kernel->start) * 1e-6) : "0"});
        double memcpy_total_duration_ms = 0.0;
        double htod_duration_ms = 0.0;
        double dtoh_duration_ms = 0.0;
        double dtod_duration_ms = 0.0;
        double htoh_duration_ms = 0.0;

        for (auto &memcpy_rec_ptr : g_activity_memcpy)
        {
            if (memcpy_rec_ptr != nullptr)
            {
                MemcpyEvent event;
                event.duration_ms = (memcpy_rec_ptr->end - memcpy_rec_ptr->start) * 1e-6;
                memcpy_total_duration_ms += event.duration_ms;
                event.bytes = memcpy_rec_ptr->bytes;
                event.src_memory_kind = getMemKindString((CUpti_ActivityMemoryKind)memcpy_rec_ptr->srcKind);
                event.dst_memory_kind = getMemKindString((CUpti_ActivityMemoryKind)memcpy_rec_ptr->dstKind);
                event.copy_kind = static_cast<uint64_t>(memcpy_rec_ptr->copyKind);
                this->event_results.push_back({"memcpy", event.to_string()});

                switch (memcpy_rec_ptr->copyKind)
                {
                case CUPTI_ACTIVITY_MEMCPY_KIND_HTOD:
                    htod_duration_ms += event.duration_ms;
                    break;
                case CUPTI_ACTIVITY_MEMCPY_KIND_DTOH:
                    dtoh_duration_ms += event.duration_ms;
                    break;
                case CUPTI_ACTIVITY_MEMCPY_KIND_DTOD:
                    dtod_duration_ms += event.duration_ms;
                    break;
                case CUPTI_ACTIVITY_MEMCPY_KIND_HTOH:
                    htoh_duration_ms += event.duration_ms;
                    break;
                default:
                    break;
                }
            }
            memcpy_rec_ptr.reset();
        }
        this->event_results.insert(this->event_results.begin() + 1, {"memcpy_duration_ms", std::to_string(memcpy_total_duration_ms)});
        this->event_results.insert(this->event_results.begin() + 2, {"memcpy_htod_duration_ms", std::to_string(htod_duration_ms)});
        this->event_results.insert(this->event_results.begin() + 2, {"memcpy_dtoh_duration_ms", std::to_string(dtoh_duration_ms)});
        this->event_results.insert(this->event_results.begin() + 2, {"memcpy_dtod_duration_ms", std::to_string(dtod_duration_ms)});
        this->event_results.insert(this->event_results.begin() + 2, {"memcpy_htoh_duration_ms", std::to_string(htoh_duration_ms)});
        g_activity_kernel.reset();
        g_activity_memcpy.clear();

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
        ss << utils::to_json<std::string>(this->total_duration_ms, this->config.measurement_type, this->event_results, this->metric_results);
        ss << "]\n";
        return ss.str();
    }

    std::vector<std::string> BlockProfiler::read()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};

        std::vector<std::string> result;
        return result;
    }
    std::unordered_map<std::string, std::string> BlockProfiler::aggregate()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};
        double total_duration = 0.0;
        std::unordered_map<std::string, std::string> aggregated_events;
        const std::vector<std::string> &event_names = this->metric_builder.event_names();

        for (const auto &entry : read_buffer)
        {
            total_duration += entry.first;

            const std::vector<std::string> &values = entry.second;

            // std::cout << "read buffer:";
            for (size_t j = 0; j < values.size(); ++j)
            {
                // std::cout << event_names[j] << ":" << values[j] << "\n";
                aggregated_events[event_names[j]] += values[j];
            }
        }
        std::vector<std::pair<std::string, std::string>> event_value(
            aggregated_events.begin(), aggregated_events.end());

        this->event_results = std::move(event_value);
        this->total_duration_ms = total_duration;

        aggregated_events["duration_microsec"] = this->total_duration_ms * 1000.0; // convert to microseconds
        return aggregated_events;
    }

} // namespace optkit::pmu::gpu::nvidia

#endif