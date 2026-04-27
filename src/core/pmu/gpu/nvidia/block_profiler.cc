#include "core/pmu/gpu/nvidia/block_profiler.hh"

#if OPTKIT_ENV_LIB_NVML
#include <cmath>
#include <iomanip>
#include <memory>
#include <cupti_version.h>
#include "core/gpu_query.hh"
#include "core/metrics/performance/gpu/nvidia/event_mapper.hh"

namespace optkit::pmu::gpu::nvidia
{
    namespace
    {
        constexpr double READ_BUFFER_FLUSH_PERIOD_MS = 5000.0;

        static std::unordered_map<std::string, double> event_counts_from_sample(
            const std::vector<std::string> &event_names,
            const std::vector<double> &values)
        {
            std::unordered_map<std::string, double> counts;
            for (size_t j = 0; j < values.size(); ++j)
                counts[event_names[j % event_names.size()]] += values[j];
            return counts;
        }
    }

#if defined(CUPTI_API_VERSION) && (CUPTI_API_VERSION >= 16)
    using ActivityKernel = CUpti_ActivityKernel6;
    using ActivityMemcpy = CUpti_ActivityMemcpy6;
    using ActivityOverhead = CUpti_ActivityOverhead3;
#else
    using ActivityKernel = CUpti_ActivityKernel4;
    using ActivityMemcpy = CUpti_ActivityMemcpy4;
    using ActivityOverhead = CUpti_ActivityOverhead3;
#endif

    static std::unique_ptr<ActivityKernel> g_activity_kernel = nullptr;
    static std::vector<std::unique_ptr<ActivityMemcpy>> g_activity_memcpy;
    static std::vector<std::unique_ptr<ActivityOverhead>> g_activity_overhead;

    // Callback for buffer requests
    static void BufferRequested(uint8_t **buffer, size_t *size, size_t *maxNumRecords)
    {
        *size = 10 * 1024 * 1024; // 10MB buffer
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
                switch (record->kind)
                {
                case CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL:
                {
                    ActivityKernel *kernel = (ActivityKernel *)record;

                    // Allocate and copy kernel data before buffer is freed
                    if (g_activity_kernel == nullptr)
                    {
                        g_activity_kernel.reset(new ActivityKernel());
                    }
                    memcpy(g_activity_kernel.get(), kernel, sizeof(ActivityKernel));
                    break;
                }
                case CUPTI_ACTIVITY_KIND_MEMCPY:
                {
                    ActivityMemcpy *memcpyCmd = (ActivityMemcpy *)record;

                    std::unique_ptr<ActivityMemcpy> ptr{new ActivityMemcpy()};
                    memcpy(ptr.get(), memcpyCmd, sizeof(ActivityMemcpy));
                    g_activity_memcpy.push_back(std::move(ptr));
                    break;
                }

                case CUPTI_ACTIVITY_KIND_OVERHEAD:
                {
                    ActivityOverhead *overhead = (ActivityOverhead *)record;
                    std::unique_ptr<ActivityOverhead> ptr{new ActivityOverhead()};
                    memcpy(ptr.get(), overhead, sizeof(ActivityOverhead));
                    g_activity_overhead.push_back(std::move(ptr));
                    break;
                }
                default:
                    break;
                }
            }
        }
        free(buffer);
    }

    BlockProfiler::BlockProfiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<double> &mb,
                                  uint32_t gpm_sample_period_us)
        : BaseProfiler{static_cast<const ProfilerConfig &>(profiler_config)}, profiler_config{profiler_config}, metric_builder{mb}
    {
        start = std::chrono::high_resolution_clock::now();
        std::vector<std::string> event_names = mb.event_names();

        CUPTI_API_CALL(cuptiActivityRegisterCallbacks(BufferRequested, BufferCompleted));
        CUPTI_API_CALL(cuptiActivityEnable(CUPTI_ACTIVITY_KIND_OVERHEAD));
        CUPTI_API_CALL(cuptiActivityEnable(CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL));
        CUPTI_API_CALL(cuptiActivityEnable(CUPTI_ACTIVITY_KIND_MEMCPY));

        // Build GPM metric ID list from the MetricBuilder events
        std::vector<nvmlGpmMetricId_t> gpm_ids;
        std::vector<std::string> gpm_names;
        for (const auto &event : mb.metric_events)
        {
            const auto metric_ids = optkit::metrics::performance::gpu::nvidia::EventMapper::get(event.first);
            if (metric_ids.empty())
            {
                continue;
            }

            for (uint64_t metric_id : metric_ids)
            {
                gpm_ids.push_back(static_cast<nvmlGpmMetricId_t>(metric_id));
                gpm_names.push_back(event.first);
            }
        }

        gpm_metric_names_ = gpm_names;

        if (!gpm_ids.empty())
        {
            nvmlDevice_t device = optkit::gpu::Query::get_nvml_device(0);
            if (device != nullptr)
            {
                gpm_sampler_.reset(new GpmSampler(device, gpm_ids, gpm_names, gpm_sample_period_us));
                gpm_sampler_->start();
            }
        }
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

    struct OverheadEvent
    {
        double duration_ms;
        std::string overhead_kind;
        std::string object_kind;

        std::string to_string() const
        {
            std::stringstream ss;
            ss << "{duration_ms:" << duration_ms << ", "
               << "overhead_kind:" << overhead_kind << ", "
               << "object_kind:" << object_kind << "}";
            return ss.str();
        }
    };

    BlockProfiler ::~BlockProfiler()
    {
        // Stop GPM sampling
        if (gpm_sampler_)
            gpm_sampler_->stop();

        auto append_detail_event = [this](const std::string &name, const std::string &value)
        {
            this->detail_event_results.push_back({name, value});
        };
        
        auto append_detail_double = [&append_detail_event](const std::string &name, double value)
        {
            std::ostringstream ss;
            ss << std::fixed << value;
            append_detail_event(name, ss.str());
        };

        CUPTI_API_CALL(cuptiActivityFlushAll(1));
        CUPTI_API_CALL(cuptiActivityDisable(CUPTI_ACTIVITY_KIND_MEMCPY));
        CUPTI_API_CALL(cuptiActivityDisable(CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL));
        CUPTI_API_CALL(cuptiActivityDisable(CUPTI_ACTIVITY_KIND_OVERHEAD));
        this->detail_event_results.clear();
        this->read_and_store();
        this->metric_results = this->metric_builder.calculate(aggregate());
        append_detail_double("kernel_total_duration_ms", g_activity_kernel != nullptr ? (g_activity_kernel->end - g_activity_kernel->start) * 1e-6 : 0.0);
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
                append_detail_event("memcpy", event.to_string());

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
        append_detail_double("memcpy_total_duration_ms", memcpy_total_duration_ms);
        append_detail_double("memcpy_htod_duration_ms", htod_duration_ms);
        append_detail_double("memcpy_dtoh_duration_ms", dtoh_duration_ms);
        append_detail_double("memcpy_dtod_duration_ms", dtod_duration_ms);
        append_detail_double("memcpy_htoh_duration_ms", htoh_duration_ms);

        std::unordered_map<std::string, OverheadEvent> overhead_map;
        double total_overhead_duration_ms = 0.0;
        for (auto &overhead_ptr : g_activity_overhead)
        {
            if (overhead_ptr != nullptr)
            {
                OverheadEvent event;
                event.duration_ms = (overhead_ptr->end - overhead_ptr->start) * 1e-6;
                total_overhead_duration_ms += event.duration_ms;

                event.overhead_kind = getOverheadKindString(overhead_ptr->overheadKind);
                event.object_kind = getObjectKindString(overhead_ptr->objectKind);
                auto it = overhead_map.find(event.overhead_kind);
                if (it != overhead_map.end())
                    it->second.duration_ms += event.duration_ms;
                else
                    overhead_map[event.overhead_kind] = event;
            }
            overhead_ptr.reset();
        }
        for (const auto &entry : overhead_map)
        {
            this->detail_event_results.insert(this->detail_event_results.begin(), {"overhead_" + entry.first, entry.second.to_string()});
        }
        this->detail_event_results.insert(this->detail_event_results.begin(), {"cupti_total_overhead_ms", [&]() {
                                                  std::ostringstream ss;
                                                  ss << std::fixed << total_overhead_duration_ms;
                                                  return ss.str();
                                              }()});
        g_activity_kernel.reset();
        g_activity_memcpy.clear();
        g_activity_overhead.clear();

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

            for (auto &&event : this->detail_event_results)
                std::cout << "\t" << event.first << ": " << event.second << std::endl;

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

        auto split_name_and_unit = [](const std::string &full_name) -> std::pair<std::string, std::string>
        {
            size_t pos = full_name.rfind("__");
            if (pos != std::string::npos && pos + 2 < full_name.size())
                return std::make_pair(full_name.substr(0, pos), full_name.substr(pos + 2));

            return std::make_pair(full_name, "None");
        };

        nlohmann::json payload = utils::to_json<double>(this->total_duration_ms, this->config.measurement_type, this->event_results, this->metric_results);
        nlohmann::json *measurements = nullptr;
        if (payload.contains("readings") && payload["readings"].is_array() && !payload["readings"].empty())
            measurements = &payload["readings"][0]["measurements"];

        for (const auto &entry_pair : this->detail_event_results)
        {
            std::pair<std::string, std::string> parsed = split_name_and_unit(entry_pair.first);

            nlohmann::json entry;
            entry["type"] = "event";
            entry["name"] = parsed.first;
            entry["value"] = entry_pair.second;
            entry["value_unit"] = parsed.second;
            entry["dtype"] = "string";

            if (measurements != nullptr)
                measurements->push_back(entry);
        }

        std::stringstream ss;
        ss << "[\n" << payload << "]\n";
        return ss.str();
    }

    std::vector<double> BlockProfiler::read()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};

        std::vector<double> result;

        const std::vector<std::string> &event_names = this->metric_builder.event_names();
        if (event_names.empty() || !gpm_sampler_ || !gpm_sampler_->is_enabled())
            return result;

        const auto averages = gpm_sampler_->average_results();
        result.reserve(event_names.size());
        for (const auto &event_name : event_names)
        {
            auto it = averages.find(event_name);
            double value = (it != averages.end()) ? it->second : 0.0;
            result.push_back(value);
        }

        return result;
    }

    void BlockProfiler::on_sample_stored(const std::pair<double, std::vector<double>> &sample)
    {
        this->buffered_duration_ms += sample.first;
        if (this->buffered_duration_ms < READ_BUFFER_FLUSH_PERIOD_MS)
            return;

        flush_compacted_samples();
    }

    void BlockProfiler::flush_compacted_samples()
    {
        if (this->read_buffer.empty())
            return;

        const std::vector<std::string> &event_names = this->metric_builder.event_names();
        for (size_t index = 0; index < this->read_buffer.size(); ++index)
        {
            const std::unordered_map<std::string, double> sample_counts =
                event_counts_from_sample(event_names, this->read_buffer[index].second);
            for (std::unordered_map<std::string, double>::const_iterator it = sample_counts.begin(); it != sample_counts.end(); ++it)
                this->compacted_event_counts[it->first] += it->second;
        }

        this->compacted_duration_ms += this->buffered_duration_ms;
        this->buffered_duration_ms = 0.0;
        this->read_buffer.clear();
    }

    std::unordered_map<std::string, double> BlockProfiler::aggregate()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};
        double total_duration = this->compacted_duration_ms;
        std::unordered_map<std::string, double> aggregated_events = this->compacted_event_counts;
        const std::vector<std::string> &event_names = this->metric_builder.event_names();

        for (size_t index = 0; index < read_buffer.size(); ++index)
        {
            total_duration += read_buffer[index].first;
            const std::unordered_map<std::string, double> sample_counts =
                event_counts_from_sample(event_names, read_buffer[index].second);
            for (std::unordered_map<std::string, double>::const_iterator it = sample_counts.begin(); it != sample_counts.end(); ++it)
                aggregated_events[it->first] += it->second;
        }
        std::vector<std::pair<std::string, double>> event_value(
            aggregated_events.begin(), aggregated_events.end());

        this->event_results = std::move(event_value);
        this->total_duration_ms = total_duration;

        aggregated_events["duration_microsec"] = this->total_duration_ms * 1000.0;
        return aggregated_events;
    }

} // namespace optkit::pmu::gpu::nvidia

#endif