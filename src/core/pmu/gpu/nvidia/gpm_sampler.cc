#include "core/pmu/gpu/nvidia/gpm_sampler.hh"

#if OPTKIT_ENV_LIB_NVML
#include <unistd.h>
#include "utils/logging/logger.hh"

namespace optkit::pmu::gpu::nvidia
{
    // ── GpmSampler ──────────────────────────────────────────────────────
    GpmSampler::GpmSampler(nvmlDevice_t device,
                           const std::vector<nvmlGpmMetricId_t> &metric_ids,
                           const std::vector<std::string> &metric_names,
                           uint32_t sample_period_us)
        : metric_ids_{metric_ids},
          metric_names_{metric_names},
          sample_period_us_{sample_period_us},
          device_{device}
    {
    }

    GpmSampler::~GpmSampler()
    {
        stop();
    }

    bool GpmSampler::start()
    {
        if (metric_ids_.empty())
            return false;

        // NVML is already initialised by gpu::Query::init(). Just verify GPM support.
        nvmlGpmSupport_t gpm_support{};
        gpm_support.version = NVML_GPM_SUPPORT_VERSION;
        nvmlReturn_t ret = nvmlGpmQueryDeviceSupport(device_, &gpm_support);
        if (ret != NVML_SUCCESS || !gpm_support.isSupportedDevice)
        {
            OPTKIT_CORE_WARN("GpmSampler: GPM not supported on this device (nvml ret: {})", static_cast<int>(ret));
            return false;
        }

        enabled_ = true;
        running_.store(true);
        thread_ = std::thread(&GpmSampler::sample_loop_, this);
        return true;
    }

    void GpmSampler::stop()
    {
        if (running_.load())
        {
            running_.store(false);
            if (thread_.joinable())
                thread_.join();
        }
    }

    size_t GpmSampler::sample_count() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return samples_.size();
    }

    std::unordered_map<std::string, double> GpmSampler::average_results() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::unordered_map<std::string, double> result;

        if (samples_.empty())
            return result;

        // Accumulate
        for (const auto &name : metric_names_)
            result[name] = 0.0;

        for (const auto &sample : samples_)
        {
            for (const auto &entry : sample)
                result[entry.first] += entry.second;
        }

        // Average
        double n = static_cast<double>(samples_.size());
        for (auto &entry : result)
            entry.second /= n;

        return result;
    }

    void GpmSampler::sample_loop_()
    {
        nvmlGpmSample_t s1, s2;
        nvmlReturn_t ret;

        ret = nvmlGpmSampleAlloc(&s1);
        if (ret != NVML_SUCCESS)
        {
            OPTKIT_CORE_WARN("GpmSampler: Failed to allocate GPM sample 1: {}", nvmlErrorString(ret));
            return;
        }
        ret = nvmlGpmSampleAlloc(&s2);
        if (ret != NVML_SUCCESS)
        {
            OPTKIT_CORE_WARN("GpmSampler: Failed to allocate GPM sample 2: {}", nvmlErrorString(ret));
            nvmlGpmSampleFree(s1);
            return;
        }

        // Configure metrics request
        nvmlGpmMetricsGet_t metricsGet{};
        metricsGet.version = NVML_GPM_METRICS_GET_VERSION;
        metricsGet.numMetrics = static_cast<unsigned int>(metric_ids_.size());
        for (unsigned int i = 0; i < metricsGet.numMetrics; i++)
        {
            metricsGet.metrics[i].metricId = metric_ids_[i];
        }

        while (running_.load())
        {
            ret = nvmlGpmSampleGet(device_, s1);
            if (ret != NVML_SUCCESS)
            {
                OPTKIT_CORE_WARN("GpmSampler: GPM sample get (s1) failed: {}", nvmlErrorString(ret));
                break;
            }

            usleep(sample_period_us_);

            if (!running_.load())
                break;

            ret = nvmlGpmSampleGet(device_, s2);
            if (ret != NVML_SUCCESS)
            {
                OPTKIT_CORE_WARN("GpmSampler: GPM sample get (s2) failed: {}", nvmlErrorString(ret));
                break;
            }

            metricsGet.sample1 = s1;
            metricsGet.sample2 = s2;

            ret = nvmlGpmMetricsGet(&metricsGet);
            if (ret != NVML_SUCCESS)
            {
                OPTKIT_CORE_WARN("GpmSampler: GPM metrics get failed: {}", nvmlErrorString(ret));
                break;
            }

            // Store this sample's results
            std::unordered_map<std::string, double> sample;
            for (unsigned int i = 0; i < metricsGet.numMetrics; i++)
            {
                sample[metric_names_[i]] = metricsGet.metrics[i].value;
            }

            {
                std::lock_guard<std::mutex> lock(mutex_);
                samples_.push_back(std::move(sample));
            }
        }

        nvmlGpmSampleFree(s1);
        nvmlGpmSampleFree(s2);
    }

} // namespace optkit::pmu::gpu::nvidia

#endif
