#pragma once

#include "utils/deployment/deployment_config.hh"
#include "core/frequency/cpu/frequency.hh"
#include "core/frequency/cpu/query.hh"
#include "core/gpu_query.hh"

#if OPTKIT_CONF_FREQ_MACROS_ENABLED

#define OPTKIT_SET_CPU_CORE_FREQ(frequency, socket) Frequency::set_core_frequency(frequency, socket)
#define OPTKIT_SET_CPU_UNCORE_FREQ(frequency, socket) Frequency::set_uncore_frequency(frequency, socket)
#define OPTKIT_SET_CPU_FREQ(core_freq, uncore_freq, socket) \
    OPTKIT_SET_CPU_UNCORE_FREQ(core_freq, socket);          \
    OPTKIT_SET_CPU_CORE_FREQ(uncore_freq, socket)

#define OPTKIT_SET_GPU_FREQ(vendor, device_id, mem_clk_mhz, graphics_clk_mhz) optkit::gpu::Query::set_clock(vendor, device_id, mem_clk_mhz, graphics_clk_mhz)

#define OPTKIT_RESET_CPU_CORE_FREQ(socket) Frequency::reset_core_frequency(socket)
#define OPTKIT_RESET_CPU_UNCORE_FREQ(socket) Frequency::reset_uncore_frequency(socket)
#define OPTKIT_RESET_GPU_FREQ(vendor, device_id) optkit::gpu::Query::reset_clock(vendor, device_id)
#else

#define OPTKIT_SET_CPU_CORE_FREQ(frequency, socket)
#define OPTKIT_SET_CPU_UNCORE_FREQ(frequency, socket)
#define OPTKIT_SET_CPU_FREQ(frequency, socket)
#define OPTKIT_RESET_CPU_CORE_FREQ(socket)
#define OPTKIT_RESET_CPU_UNCORE_FREQ(socket)

#define OPTKIT_SET_GPU_FREQ(vendor, device_id, mem_clk_mhz, graphics_clk_mhz)
#define OPTKIT_RESET_GPU_FREQ(vendor, device_id)

#endif