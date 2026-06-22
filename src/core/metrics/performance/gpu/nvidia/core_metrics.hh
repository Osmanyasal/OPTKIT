#pragma once
#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_LIB_NVML

#include "core/metrics/performance/gpu/core_metrics.hh"
#include "core/metrics/performance/gpu/nvidia/event_mapper.hh"
#include "core/metrics/performance/gpu/nvidia/native_events.hh"
#include "core/frequency/cpu/query.hh"
#include <vector>

/**
 * @brief NVIDIA CoreEvent implementation for GPU architecture.
 *
 * This implementation is based on performance events available on NVIDIA GPUs.
 * Event compatibility with other GPU architectures is not guaranteed.
 *
 * Note: NVIDIA's official performance analysis is documented primarily for their GPUs.
 * https://developer.nvidia.com/gpu-performance-analysis
 *
 * GPM Implementation:
 * https://github.com/AlessandroVacca/nv-gpm-metrics
 */

// Warn: to use template initialisation for a certain type, they must be in the same namespace. so do NOT change it.
namespace optkit::metrics::performance::gpu
{
    /**
     * @class NvidiaMetricsImpl
     * @brief Interface for retrieving NVIDIA GPU performance metrics. Each metric method returns the set of events required for its computation.
     *
     */
    class NvidiaMetricsImpl
    {
    };

    template <>
    class CoreMetrics<NvidiaMetricsImpl>
    {
    public:
        // Return all supported metric names that can be passed to get_metric().
        static const std::vector<std::string> &get_all_metrics()
        {
            static const std::vector<std::string> names = {
                "graphics_util",
                "sm_util",
                "sm_occupancy",
                "integer_util",
                "tensor_util",
                "dfma_util",
                "hmma_util",
                "imma_util",
                "dram_bw_util",
                "fp64_util",
                "fp32_util",
                "fp16_util",
            };
            return names;
        }

        // Fetch a metric by its method name (e.g., "l2_mpki").
        // Returns a const reference to a static MetricBuilder.
        static const MetricBuilder<double> &get_metric(const std::string &metric_name)
        {
            // Common metrics
            if (metric_name == "graphics_util")
                return graphics_util();
            if (metric_name == "sm_util")
                return sm_util();
            if (metric_name == "sm_occupancy")
                return sm_occupancy();
            if (metric_name == "integer_util")
                return integer_util();
            if (metric_name == "tensor_util")
                return tensor_util();
            if (metric_name == "dfma_util")
                return dfma_util();
            if (metric_name == "hmma_util")
                return hmma_util();
            if (metric_name == "imma_util")
                return imma_util();
            if (metric_name == "dram_bw_util")
                return dram_bw_util();
            if (metric_name == "fp64_util")
                return fp64_util();
            if (metric_name == "fp32_util")
                return fp32_util();
            if (metric_name == "fp16_util")
                return fp16_util();

            OPTKIT_CORE_WARN("Requested unknown NVIDIA metric: {}", metric_name);
            static const MetricBuilder<double> empty{};
            return empty;
        }

        static const MetricBuilder<double> &graphics_util()
        {
            static const MetricBuilder<double> metric = []
            {
                std::string graphics_util_name = to_string(CoreEvents::GRAPHICS_UTIL);
                return MetricBuilder<double>{false}
                    .add(graphics_util_name, nvidia::EventMapper::get(CoreEvents::GRAPHICS_UTIL))
                    .build("graphics_util__%",
                           [graphics_util_name](const std::unordered_map<std::string, double> &counts) -> double
                           {
                               double graphics_util = get_event_count(counts, graphics_util_name);
                               return graphics_util;
                           });
            }();
            return metric;
        }

        static const MetricBuilder<double> &sm_util()
        {
            static const MetricBuilder<double> metric = []
            {
                std::string sm_util_name = to_string(CoreEvents::SM_UTIL);
                return MetricBuilder<double>{false}
                    .add(sm_util_name, nvidia::EventMapper::get(CoreEvents::SM_UTIL))
                    .build("sm_util__%",
                           [sm_util_name](const std::unordered_map<std::string, double> &counts) -> double
                           {
                               double sm_util = get_event_count(counts, sm_util_name);
                               return sm_util;
                           });
            }();
            return metric;
        }

        static const MetricBuilder<double> &sm_occupancy()
        {
            static const MetricBuilder<double> metric = []
            {
                std::string sm_occupancy_name = to_string(CoreEvents::SM_OCCUPANCY);
                return MetricBuilder<double>{false}
                    .add(sm_occupancy_name, nvidia::EventMapper::get(CoreEvents::SM_OCCUPANCY))
                    .build("sm_occupancy__%",
                           [sm_occupancy_name](const std::unordered_map<std::string, double> &counts) -> double
                           {
                               double sm_occupancy = get_event_count(counts, sm_occupancy_name);
                               return sm_occupancy;
                           });
            }();
            return metric;
        }

        static const MetricBuilder<double> &integer_util()
        {
            static const MetricBuilder<double> metric = []
            {
                std::string integer_util_name = to_string(CoreEvents::INTEGER_UTIL);
                return MetricBuilder<double>{false}
                    .add(integer_util_name, nvidia::EventMapper::get(CoreEvents::INTEGER_UTIL))
                    .build("integer_util__%",
                           [integer_util_name](const std::unordered_map<std::string, double> &counts) -> double
                           {
                               double integer_util = get_event_count(counts, integer_util_name);
                               return integer_util;
                           });
            }();
            return metric;
        }

        static const MetricBuilder<double> &tensor_util()
        {
            static const MetricBuilder<double> metric = []
            {
                std::string tensor_util_name = to_string(CoreEvents::ANY_TENSOR_UTIL);
                return MetricBuilder<double>{false}
                    .add(tensor_util_name, nvidia::EventMapper::get(CoreEvents::ANY_TENSOR_UTIL))
                    .build("tensor_util__%",
                           [tensor_util_name](const std::unordered_map<std::string, double> &counts) -> double
                           {
                               double tensor_util = get_event_count(counts, tensor_util_name);
                               return tensor_util;
                           });
            }();
            return metric;
        }

        static const MetricBuilder<double> &dfma_util()
        {
            static const MetricBuilder<double> metric = []
            {
                std::string dfma_util_name = to_string(CoreEvents::DFMA_TENSOR_UTIL);
                return MetricBuilder<double>{false}
                    .add(dfma_util_name, nvidia::EventMapper::get(CoreEvents::DFMA_TENSOR_UTIL))
                    .build("dfma_util__%",
                           [dfma_util_name](const std::unordered_map<std::string, double> &counts) -> double
                           {
                               double dfma_util = get_event_count(counts, dfma_util_name);
                               return dfma_util;
                           });
            }();
            return metric;
        } // Double Fused Multiply-Add

        static const MetricBuilder<double> &hmma_util()
        {
            static const MetricBuilder<double> metric = []
            {
                std::string hmma_util_name = to_string(CoreEvents::HMMA_TENSOR_UTIL);
                return MetricBuilder<double>{false}
                    .add(hmma_util_name, nvidia::EventMapper::get(CoreEvents::HMMA_TENSOR_UTIL))
                    .build("hmma_util__%",
                           [hmma_util_name](const std::unordered_map<std::string, double> &counts) -> double
                           {
                               double hmma_util = get_event_count(counts, hmma_util_name);
                               return hmma_util;
                           });
            }();
            return metric;
        } // Half-precision Matrix Multiply-Accumulate

        static const MetricBuilder<double> &imma_util()
        {
            static const MetricBuilder<double> metric = []
            {
                std::string imma_util_name = to_string(CoreEvents::IMMA_TENSOR_UTIL);
                return MetricBuilder<double>{false}
                    .add(imma_util_name, nvidia::EventMapper::get(CoreEvents::IMMA_TENSOR_UTIL))
                    .build("imma_util__%",
                           [imma_util_name](const std::unordered_map<std::string, double> &counts) -> double
                           {
                               double imma_util = get_event_count(counts, imma_util_name);
                               return imma_util;
                           });
            }();
            return metric;
        } // Integer Matrix Multiply-Accumulate

        static const MetricBuilder<double> &dram_bw_util()
        {
            static const MetricBuilder<double> metric = []
            {
                std::string dram_bw_util_name = to_string(CoreEvents::DRAM_BW_UTIL);
                return MetricBuilder<double>{false}
                    .add(dram_bw_util_name, nvidia::EventMapper::get(CoreEvents::DRAM_BW_UTIL))
                    .build("dram_bw_util__%",
                           [dram_bw_util_name](const std::unordered_map<std::string, double> &counts) -> double
                           {
                               double dram_bw_util = get_event_count(counts, dram_bw_util_name);
                               return dram_bw_util;
                           });
            }();
            return metric;
        } // DRAM Bandwidth Utilization
        
        static const MetricBuilder<double> &fp64_util()
        {
            static const MetricBuilder<double> metric = []
            {
                std::string fp64_util_name = to_string(CoreEvents::FP64_UTIL);
                return MetricBuilder<double>{false}
                    .add(fp64_util_name, nvidia::EventMapper::get(CoreEvents::FP64_UTIL))
                    .build("fp64_util__%",
                           [fp64_util_name](const std::unordered_map<std::string, double> &counts) -> double
                           {
                               double fp64_util = get_event_count(counts, fp64_util_name);
                               return fp64_util;
                           });
            }();
            return metric;
        } // Double-precision Floating Point Utilization

        static const MetricBuilder<double> &fp32_util()
        {
            static const MetricBuilder<double> metric = []
            {
                std::string fp32_util_name = to_string(CoreEvents::FP32_UTIL);
                return MetricBuilder<double>{false}
                    .add(fp32_util_name, nvidia::EventMapper::get(CoreEvents::FP32_UTIL))
                    .build("fp32_util__%",
                           [fp32_util_name](const std::unordered_map<std::string, double> &counts) -> double
                           {
                               double fp32_util = get_event_count(counts, fp32_util_name);
                               return fp32_util;
                           });
            }();
            return metric;
        } // Single-precision Floating Point Utilization

        static const MetricBuilder<double> &fp16_util()
        {
            static const MetricBuilder<double> metric = []
            {
                std::string fp16_util_name = to_string(CoreEvents::FP16_UTIL);
                return MetricBuilder<double>{false}
                    .add(fp16_util_name, nvidia::EventMapper::get(CoreEvents::FP16_UTIL))
                    .build("fp16_util__%",
                           [fp16_util_name](const std::unordered_map<std::string, double> &counts) -> double
                           {
                               double fp16_util = get_event_count(counts, fp16_util_name);
                               return fp16_util;
                           });
            }();
            return metric;
        } // Half-precision Floating Point Utilization

        static const MetricBuilder<double> &all_metrics()
        {
            static const MetricBuilder<double> mb = []
            {
                MetricBuilder<double> mb{false};
                mb.add(graphics_util());
                mb.add(sm_util());
                mb.add(sm_occupancy());
                mb.add(integer_util());
                mb.add(tensor_util());
                mb.add(dfma_util());
                mb.add(hmma_util());
                mb.add(imma_util());
                mb.add(dram_bw_util());
                mb.add(fp64_util());
                mb.add(fp32_util());
                mb.add(fp16_util());
                return mb;
            }();
            return mb;
        }
    };
}
#endif // OPTKIT_ENV_CPU_ARM