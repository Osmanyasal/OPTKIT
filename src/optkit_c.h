#pragma once

// C wrapper API for OPTKIT (C++ library)
// This header is C-compatible.

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(__GNUC__)
#define OPTKIT_C_API __attribute__((visibility("default")))
#else
#define OPTKIT_C_API
#endif

    typedef enum optkit_status
    {
        OPTKIT_STATUS_OK = 0,
        OPTKIT_STATUS_ERROR = 1,
        OPTKIT_STATUS_NOT_INITIALIZED = 2,
        OPTKIT_STATUS_INVALID_ARGUMENT = 3,
    } optkit_status_t;

    // -----------------------------------------------------------------------------
    // Common utilities
    // -----------------------------------------------------------------------------

    // Returns a thread-local error message for the last failing call.
    OPTKIT_C_API optkit_status_t optkit_last_error_message(const char **err_message);

    // Clears the thread-local error message.
    OPTKIT_C_API optkit_status_t optkit_clear_error(void);

    // -----------------------------------------------------------------------------
    // Engine lifecycle
    // -----------------------------------------------------------------------------

    OPTKIT_C_API optkit_status_t optkit_is_initialized(int8_t *is_init);
    OPTKIT_C_API optkit_status_t optkit_init(int8_t create_folder, const char *execution_file);
    OPTKIT_C_API optkit_status_t optkit_finalize(void);

    // -----------------------------------------------------------------------------
    // Query: system / CPU
    // -----------------------------------------------------------------------------

    OPTKIT_C_API optkit_status_t optkit_query_system_num_sockets(int16_t *out_num_sockets);
    OPTKIT_C_API optkit_status_t optkit_query_system_num_logical_cores(int16_t *out_num_logical_cores);
    OPTKIT_C_API optkit_status_t optkit_query_system_is_root_priv_enabled(int8_t *out_enabled);

    OPTKIT_C_API optkit_status_t optkit_query_system_paranoid(int32_t *out_paranoid);
    OPTKIT_C_API optkit_status_t optkit_query_system_is_smt_enabled(int8_t *out_enabled);
    OPTKIT_C_API optkit_status_t optkit_query_system_is_turbo_enabled(int8_t *out_enabled);
    OPTKIT_C_API optkit_status_t optkit_query_system_detect_cpu_packages_str(char **out_str);

    // -----------------------------------------------------------------------------
    // Query: PMU (libpfm4)
    // -----------------------------------------------------------------------------

    // Prints to stdout.
    OPTKIT_C_API optkit_status_t optkit_query_pmu_list_avail_pmus(void);

    // Prints to stdout.
    OPTKIT_C_API optkit_status_t optkit_query_pmu_list_avail_events(int32_t pmu_id);

    OPTKIT_C_API optkit_status_t optkit_query_pmu_avail_pmu_ids(int32_t **out_ids, size_t *out_count);

    // Returns "pmu_name::event_name" list separated by newlines.
    OPTKIT_C_API optkit_status_t optkit_query_pmu_get_avail_events_str(int32_t pmu_id, char **out_str);

    OPTKIT_C_API optkit_status_t optkit_query_pmu_pmu_info_str(int32_t pmu_id, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_query_pmu_default_pmu_info_str(char **out_str);
    OPTKIT_C_API optkit_status_t optkit_query_pmu_event_detail_str(int32_t pmu_id, uint32_t event_code, char **out_str);

    // -----------------------------------------------------------------------------
    // Query: RAPL
    // -----------------------------------------------------------------------------

    // Bitmask of optkit::energy::rapl::RaplReadMethods.
    OPTKIT_C_API optkit_status_t optkit_query_rapl_avail_read_methods(int32_t *out_methods);
    OPTKIT_C_API optkit_status_t optkit_query_rapl_is_perf_avail(int8_t *out_avail);
    OPTKIT_C_API optkit_status_t optkit_query_rapl_is_sysfs_avail(int8_t *out_avail);
    OPTKIT_C_API optkit_status_t optkit_query_rapl_is_msr_avail(int8_t *out_avail);

    // Returns domain info list separated by newlines.
    OPTKIT_C_API optkit_status_t optkit_query_rapl_domain_info_str(char **out_str);

    // -----------------------------------------------------------------------------
    // Query: GPU
    // -----------------------------------------------------------------------------

    typedef enum optkit_gpu_vendor
    {
        OPTKIT_GPU_VENDOR_NVIDIA = 1,
        OPTKIT_GPU_VENDOR_AMD = 2,
        OPTKIT_GPU_VENDOR_INTEL = 3,
        OPTKIT_GPU_VENDOR_ARM_MALI = 4,
        OPTKIT_GPU_VENDOR_QUALCOMM_ADRENO = 5,
        OPTKIT_GPU_VENDOR_IMAGINATION_POWERVR = 6,
        OPTKIT_GPU_VENDOR_UNKNOWN = 255,
    } optkit_gpu_vendor_t;

    OPTKIT_C_API optkit_status_t optkit_query_gpu_init(optkit_gpu_vendor_t vendor);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_shutdown(optkit_gpu_vendor_t vendor);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_is_init(optkit_gpu_vendor_t vendor, int8_t *out_is_init);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_is_device_exists(optkit_gpu_vendor_t vendor, int8_t *out_exists);

    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_device_count(optkit_gpu_vendor_t vendor, uint32_t *out_count);

    // Struct-returning queries exposed as strings (useful from plain C).
    OPTKIT_C_API optkit_status_t optkit_query_gpu_device_query_str(optkit_gpu_vendor_t vendor, uint32_t device_index, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_basic_info_str(optkit_gpu_vendor_t vendor, uint32_t device_index, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_version_info_str(optkit_gpu_vendor_t vendor, uint32_t device_index, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_memory_info_str(optkit_gpu_vendor_t vendor, uint32_t device_index, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_clock_info_str(optkit_gpu_vendor_t vendor, uint32_t device_index, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_temperature_info_str(optkit_gpu_vendor_t vendor, uint32_t device_index, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_compute_info_str(optkit_gpu_vendor_t vendor, uint32_t device_index, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_power_info_str(optkit_gpu_vendor_t vendor, uint32_t device_index, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_utilization_info_str(optkit_gpu_vendor_t vendor, uint32_t device_index, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_hardware_info_str(optkit_gpu_vendor_t vendor, uint32_t device_index, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_capabilities_info_str(optkit_gpu_vendor_t vendor, uint32_t device_index, char **out_str);

    // Controls
    OPTKIT_C_API optkit_status_t optkit_query_gpu_set_clock(optkit_gpu_vendor_t vendor, uint32_t device_index, uint32_t mem_clk_mhz, uint32_t graphics_clk_mhz);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_reset_clock(optkit_gpu_vendor_t vendor, uint32_t device_index);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_reset_device(optkit_gpu_vendor_t vendor, uint32_t device_index);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_set_persistence_mode(optkit_gpu_vendor_t vendor, uint32_t device_index, int enable);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_set_fan_speed(optkit_gpu_vendor_t vendor, uint32_t device_index, uint32_t fan_speed_percent);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_reset_fan_speed(optkit_gpu_vendor_t vendor, uint32_t device_index);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_set_power_limit(optkit_gpu_vendor_t vendor, uint32_t device_index, double power_limit_watts);

    // Simple getters
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_fan_count(optkit_gpu_vendor_t vendor, uint32_t device_index, uint32_t *out_count);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_warp_size(optkit_gpu_vendor_t vendor, uint32_t device_index, uint32_t *out_warp_size);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_driver_version(optkit_gpu_vendor_t vendor, double *out_version);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_library_version(optkit_gpu_vendor_t vendor, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_device_power(optkit_gpu_vendor_t vendor, uint32_t device_index, double *out_watts);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_architecture(optkit_gpu_vendor_t vendor, uint32_t device_index, uint32_t *out_arch);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_device_power_limits(optkit_gpu_vendor_t vendor, uint32_t device_index,
                                                                          double *out_limit_watts, double *out_default_power, double *out_min_limit_watts,
                                                                          double *out_max_limit_watts, int *out_is_configurable);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_device_temperature(optkit_gpu_vendor_t vendor, uint32_t device_index, double *out_device_temp_c, double *out_mem_temp_c);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_device_name(optkit_gpu_vendor_t vendor, uint32_t device_index, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_query_gpu_get_device_temperature_thresholds(optkit_gpu_vendor_t vendor, uint32_t device_index,
                                                                                    double *out_max_gpu_temp_c, double *out_max_mem_temp_c,
                                                                                    double *out_min_gpu_temp_c, double *out_min_mem_temp_c);

    // -----------------------------------------------------------------------------
    // Profilers (start/stop semantics)
    // -----------------------------------------------------------------------------

    // PERF / CPU events
    OPTKIT_C_API optkit_status_t optkit_perf_start(const char *block_name,
                                                   const char *const *metrics, size_t metrics_count,
                                                   const char *const *events, size_t events_count);
    OPTKIT_C_API optkit_status_t optkit_perf_stop(void);

    // Energy
    OPTKIT_C_API optkit_status_t optkit_energy_start(const char *block_name);
    OPTKIT_C_API optkit_status_t optkit_energy_stop(void);
    OPTKIT_C_API optkit_status_t optkit_energy_cpu_start(const char *block_name);
    OPTKIT_C_API optkit_status_t optkit_energy_cpu_stop(void);
    OPTKIT_C_API optkit_status_t optkit_energy_gpu_start(const char *block_name);
    OPTKIT_C_API optkit_status_t optkit_energy_gpu_stop(void);

    // Callstack
    OPTKIT_C_API optkit_status_t optkit_callstack_start(const char *block_name);
    OPTKIT_C_API optkit_status_t optkit_callstack_stop(void);

    // Disk
    OPTKIT_C_API optkit_status_t optkit_disk_start(const char *block_name);
    OPTKIT_C_API optkit_status_t optkit_disk_stop(void);

    // Temperature
    OPTKIT_C_API optkit_status_t optkit_temperature_start(const char *block_name);
    OPTKIT_C_API optkit_status_t optkit_temperature_stop(void);
    OPTKIT_C_API optkit_status_t optkit_temperature_hwmon_start(const char *block_name);
    OPTKIT_C_API optkit_status_t optkit_temperature_hwmon_stop(void);
    OPTKIT_C_API optkit_status_t optkit_temperature_gpu_start(const char *block_name);
    OPTKIT_C_API optkit_status_t optkit_temperature_gpu_stop(void);

    // -----------------------------------------------------------------------------
    // Frequency
    // -----------------------------------------------------------------------------

    typedef enum optkit_frequency_unit
    {
        OPTKIT_FREQUENCY_UNIT_HZ = 0,
        OPTKIT_FREQUENCY_UNIT_KHZ = 1,
        OPTKIT_FREQUENCY_UNIT_MHZ = 2,
        OPTKIT_FREQUENCY_UNIT_GHZ = 3,
    } optkit_frequency_unit_t;

    OPTKIT_C_API optkit_status_t optkit_frequency_convert(const char *freq_str, optkit_frequency_unit_t target_unit, double *out_value);

    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_set_core_frequency(int64_t freq_khz, int16_t socket);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_set_core_frequency_core(int64_t freq_khz, int16_t cpu, int16_t socket);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_set_core_frequency_range(int64_t freq_khz, int16_t cpu_start, int16_t cpu_end, int16_t socket);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_get_core_frequency(int16_t cpu, int64_t *out_freq_khz);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_get_core_frequencies(int16_t socket, int64_t *out_freqs_khz, size_t capacity, size_t *out_count);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_reset_core_frequency(int16_t socket);

    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_get_uncore_frequency(int16_t socket, int64_t *out_freq_khz);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_set_uncore_frequency(int64_t freq_khz, int16_t socket);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_reset_uncore_frequency(int16_t socket);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_get_uncore_min_max(int16_t socket, int64_t *out_min_khz, int64_t *out_max_khz);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_get_scaling_available_uncore_frequencies(int16_t socket, int64_t step_khz,
                                                                                               int64_t *out_freqs_khz, size_t capacity, size_t *out_count);

    // Frequency queries (strings for list-like outputs)
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_query_available_core_frequencies(int32_t core, int64_t step_khz,
                                                                                       int64_t *out_freqs_khz, size_t capacity, size_t *out_count);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_query_available_governors_str(int32_t core, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_query_get_governor(int32_t core, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_query_set_governor(const char *governor, int32_t socket);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_query_get_cpuinfo_max_freq(int32_t core, int64_t *out_freq_khz);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_query_get_cpuinfo_min_freq(int32_t core, int64_t *out_freq_khz);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_query_get_bios_limit(int32_t core, int64_t *out_freq_khz);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_query_get_scaling_driver(int32_t core, char **out_str);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_query_set_governor_percore(const char *governor, int32_t core);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_query_get_scaling_max_limit(int32_t core, int64_t *out_freq_khz);
    OPTKIT_C_API optkit_status_t optkit_frequency_cpu_query_get_scaling_min_limit(int32_t core, int64_t *out_freq_khz);

#ifdef __cplusplus
}
#endif
