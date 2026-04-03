#pragma once

#include "core/metrics/performance/cpu/core_metrics.hh"
#include "utils/deployment/deployment_config.hh"

#if OPTKIT_ENV_LIB_NVML
#include "core/metrics/performance/gpu/nvidia/core_metrics.hh"
#else
#include "core/metrics/performance/gpu/core_metrics.hh"
#endif

#if OPTKIT_ENV_CPU_INTEL
#include "core/metrics/performance/cpu/intel/core_metrics.hh"
#include "core/metrics/performance/cpu/intel/event_mapper.hh"
namespace optkit::metrics::performance
{
    using cpu_metrics = cpu::CoreMetrics<cpu::IntelMetricsImpl>;
    using cpu_mapper = cpu::intel::EventMapper;
    using cpu_events = cpu::CoreEvents;
    using cpu_native_events = cpu::intel::NativeEvents;
#if OPTKIT_ENV_LIB_NVML
    using gpu_metrics = gpu::CoreMetrics<gpu::NvidiaMetricsImpl>;
#else
    using gpu_metrics = gpu::CoreMetrics<void>;
#endif
    inline const std::vector<std::string> &cpu_get_native_events()
    {
        static const std::vector<std::string> events = cpu::intel::get_native_events();
        return events;
    }
    inline const std::vector<std::string> &cpu_get_supported_core_events()
    {
        static const std::vector<std::string> events = cpu::intel::EventMapper::get_supported_core_events();
        return events;
    }
}
#elif OPTKIT_ENV_CPU_AMD
#include "core/metrics/performance/cpu/amd/core_metrics.hh"
#include "core/metrics/performance/cpu/amd/event_mapper.hh" 
namespace optkit::metrics::performance
{
    using cpu_metrics = cpu::CoreMetrics<cpu::AMDMetricsImpl>;
    using cpu_mapper = cpu::amd::EventMapper;
    using cpu_events = cpu::CoreEvents;
    using cpu_native_events = cpu::amd::NativeEvents;
#if OPTKIT_ENV_LIB_NVML
    using gpu_metrics = gpu::CoreMetrics<gpu::NvidiaMetricsImpl>;
#else
    using gpu_metrics = gpu::CoreMetrics<void>;
#endif
    inline const std::vector<std::string> &cpu_get_native_events()
    {
        static const std::vector<std::string> events = cpu::amd::get_native_events();
        return events;
    }
    inline const std::vector<std::string> &cpu_get_supported_core_events()
    {
        static const std::vector<std::string> events = cpu::amd::EventMapper::get_supported_core_events();
        return events;
    }
}
#elif OPTKIT_ENV_CPU_ARM
#include "core/metrics/performance/cpu/arm/core_metrics.hh"
#include "core/metrics/performance/cpu/arm/event_mapper.hh"
namespace optkit::metrics::performance
{
    using cpu_metrics = cpu::CoreMetrics<cpu::ARMMetricsImpl>;
    using cpu_mapper = cpu::arm::EventMapper;
    using cpu_events = cpu::CoreEvents;
    using cpu_native_events = cpu::arm::NativeEvents;
#if OPTKIT_ENV_LIB_NVML
    using gpu_metrics = gpu::CoreMetrics<gpu::NvidiaMetricsImpl>;
#else
    using gpu_metrics = gpu::CoreMetrics<void>;
#endif
    inline const std::vector<std::string> &cpu_get_native_events()
    {
        static const std::vector<std::string> events = cpu::arm::get_native_events();
        return events;
    }
    inline const std::vector<std::string> &cpu_get_supported_core_events()
    {
        static const std::vector<std::string> events = cpu::arm::EventMapper::get_supported_core_events();
        return events;
    }
}
#elif OPTKIT_ENV_CPU_RISCV
#include "core/metrics/performance/cpu/riscv/core_metrics.hh"
#include "core/metrics/performance/cpu/riscv/event_mapper.hh"
namespace optkit::metrics::performance
{
    using cpu_metrics = cpu::CoreMetrics<cpu::RISCVMetricsImpl>;
    using cpu_mapper = cpu::riscv::EventMapper;
    using cpu_events = cpu::CoreEvents;
    using cpu_native_events = cpu::riscv::NativeEvents;
#if OPTKIT_ENV_LIB_NVML
    using gpu_metrics = gpu::CoreMetrics<gpu::NvidiaMetricsImpl>;
#else
    using gpu_metrics = gpu::CoreMetrics<void>;
#endif
    inline const std::vector<std::string> &cpu_get_supported_core_events()
    {
        static const std::vector<std::string> events = cpu::riscv::EventMapper::get_supported_core_events();
        return events;
    }
    inline const std::vector<std::string> &cpu_get_native_events()
    {
        static const std::vector<std::string> events = cpu::riscv::get_native_events();
        return events;
    }
}
#elif OPTKIT_ENV_CPU_MIPS
#elif OPTKIT_ENV_CPU_POWERPC
#else
#endif