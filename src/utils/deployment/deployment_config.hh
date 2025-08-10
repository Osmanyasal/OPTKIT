#pragma once

#include "utils/environment_config.hh"
/**
 * @brief Should be included by the user to the host program that's using the library.
 *
 */
// =============================================
// User-configurable switches (can be defined before including this header)
// =============================================

// Allow users to override macro enable flags
#ifndef OPTKIT_CONF_PMU_MACROS_ENABLED
#define OPTKIT_CONF_PMU_MACROS_ENABLED 1 // Default: enabled
#endif

#ifndef OPTKIT_CONF_RAPL_MACROS_ENABLED
#define OPTKIT_CONF_RAPL_MACROS_ENABLED 1
#endif

#ifndef OPTKIT_CONF_FREQ_MACROS_ENABLED
#define OPTKIT_CONF_FREQ_MACROS_ENABLED 1
#endif

#ifndef OPTKIT_CONF_DISK_MACROS_ENABLED
#define OPTKIT_CONF_DISK_MACROS_ENABLED 1 // Default: enabled
#endif

#ifndef OPTKIT_CONF_TEMPERATURE_MACROS_ENABLED
#define OPTKIT_CONF_TEMPERATURE_MACROS_ENABLED 1
#endif

#ifndef OPTKIT_CONF_PMU_USE_PERF
#define OPTKIT_CONF_PMU_USE_PERF (OPTKIT_CONF_PMU_MACROS_ENABLED && OPTKIT_ENV_LIB_PERF_EVENT) // Default: enabled if system has perf_event
#endif

#ifndef OPTKIT_CONF_PMU_USE_MSR
#define OPTKIT_CONF_PMU_USE_MSR (!OPTKIT_CONF_PMU_USE_PERF && (OPTKIT_CONF_PMU_MACROS_ENABLED && OPTKIT_ENV_LIB_MSR_SAFE)) // if perf is not enabled and system has msr_safe and selected, then go for it.
#endif

// =============================================
// Backend selection logic (user-controllable)
// =============================================

// Case 1: User explicitly requested perf
#if OPTKIT_CONF_PMU_USE_PERF
#undef OPTKIT_CONF_PMU_USE_MSR  // Ensure MSR is not set
#undef OPTKIT_CONF_PMU_USE_PERF // Ensure MSR is not set
#define OPTKIT_CONF_PMU_USE_MSR 0
#define OPTKIT_CONF_PMU_USE_PERF 1

// Case 2: User explicitly requested MSR
#elif OPTKIT_CONF_PMU_USE_MSR
#undef OPTKIT_CONF_PMU_USE_PERF
#undef OPTKIT_CONF_PMU_USE_MSR
#define OPTKIT_CONF_PMU_USE_PERF 0
#define OPTKIT_CONF_PMU_USE_MSR 1

// Case 3: Auto-detect (only if neither is set)
#else
#undef OPTKIT_CONF_PMU_MACROS_ENABLED
#define OPTKIT_CONF_PMU_MACROS_ENABLED 0
#undef OPTKIT_CONF_PMU_USE_PERF
#define OPTKIT_CONF_PMU_USE_PERF 0
#undef OPTKIT_CONF_PMU_USE_MSR
#define OPTKIT_CONF_PMU_USE_MSR 0
#pragma message("PMU disabled: No available backend (neither PERF nor MSR)")
#endif

// Final validation
#if OPTKIT_CONF_PMU_USE_PERF && OPTKIT_CONF_PMU_USE_MSR
#error "PMU configuration conflict: Both USE_PERF and USE_MSR cannot be enabled"
#endif