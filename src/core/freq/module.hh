#pragma once

#include "utils/deployment/deployment_config.hh"
#include "core/freq/cpu_frequency.hh"
#include "core/freq/query_frequency.hh"

#if defined(CONF__FREQ__MACROS__ENABLED) && CONF__FREQ__MACROS__ENABLED == 1

#define OPTKIT_SET_CPU_CORE_FREQ(frequency, socket) CPUFrequency::set_core_frequency(frequency, socket)
#define OPTKIT_SET_CPU_UNCORE_FREQ(frequency, socket) CPUFrequency::set_uncore_frequency(frequency, socket)
#define OPTKIT_SET_CPU_FREQ(core_freq,uncore_freq, socket)     \
    OPTKIT_SET_CPU_UNCORE_FREQ(core_freq, socket); \
    OPTKIT_SET_CPU_CORE_FREQ(uncore_freq, socket)

#define OPTKIT_RESET_CPU_CORE_FREQ(socket) CPUFrequency::reset_core_frequency(socket)
#define OPTKIT_RESET_CPU_UNCORE_FREQ(socket) CPUFrequency::reset_uncore_frequency(socket)
#else

#define OPTKIT_SET_CPU_CORE_FREQ(frequency, socket)
#define OPTKIT_SET_CPU_UNCORE_FREQ(frequency, socket)
#define OPTKIT_SET_CPU_FREQ(frequency, socket)
#define OPTKIT_RESET_CPU_CORE_FREQ(socket)   
#define OPTKIT_RESET_CPU_UNCORE_FREQ(socket) 

#endif


using optkit::core::freq::operator<<; // make available to global namespace