#include <fstream>
#include <cstdlib>
#include <regex>
#include <vector>
#include <algorithm>
#include "utils/logging/logger.hh"
#include "core/gpu_query.hh"

namespace optkit::gpu
{
    std::unordered_map<GpuVendor, bool> Query::initialized;

#if OPTKIT_ENV_LIB_NVML
    std::vector<nvmlDevice_t> Query::gpu_handles_nvml;
#endif

#if OPTKIT_ENV_LIB_AMDSMI
    std::vector<amdsmi_socket_handle> Query::socket_handles_amdsmi;
    std::vector<amdsmi_processor_handle> Query::gpu_handles_amdsmi;
#endif

    bool Query::init(GpuVendor vendor)
    {
        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            if (OPT_LIKELY(!initialized[GpuVendor::NVIDIA])) // add first then check, it is false (which by default is) so it will init.
            {
                nvmlReturn_t result = nvmlInit();
                initialized[GpuVendor::NVIDIA] = (result == NVML_SUCCESS);
                if (OPT_LIKELY(initialized[GpuVendor::NVIDIA]))
                {
                    OPTKIT_CORE_INFO("Initialized NVML library successfully");

                    // Get device count directly from NVML to avoid circular dependency
                    uint32_t device_count = 0;
                    nvmlReturn_t count_result = nvmlDeviceGetCount(&device_count);
                    if (count_result == NVML_SUCCESS)
                    {
                        Query::gpu_handles_nvml.reserve(device_count);
                        for (uint32_t i = 0; i < device_count; i++)
                        {
                            nvmlDevice_t device;
                            result = nvmlDeviceGetHandleByIndex(i, &device);
                            if (OPT_LIKELY(result == NVML_SUCCESS))
                                Query::gpu_handles_nvml.push_back(device);
                            else
                            {
                                OPTKIT_CORE_ERROR("NVML error in nvmlDeviceGetHandleByIndex: {}", std::string(nvmlErrorString(result)));
                                initialized[GpuVendor::NVIDIA] = false;
                                nvmlShutdown();
                                break;
                            }
                        }
                    }
                    else
                    {
                        OPTKIT_CORE_ERROR("NVML error in nvmlDeviceGetCount: {}", std::string(nvmlErrorString(count_result)));
                        initialized[GpuVendor::NVIDIA] = false;
                        nvmlShutdown();
                    }
                }
                else
                {
                    OPTKIT_CORE_ERROR("NVML error in nvmlInit: {}", std::string(nvmlErrorString(result)));
                }
            }
            return true;
#endif
        }
        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI
            if (OPT_LIKELY(!initialized[GpuVendor::AMD]))
            {
                amdsmi_status_t result = amdsmi_init(0);
                initialized[GpuVendor::AMD] = (result == AMDSMI_STATUS_SUCCESS);
                if (OPT_LIKELY(initialized[GpuVendor::AMD]))
                {
                    OPTKIT_CORE_INFO("Initialized ROCm SMI library successfully");

                    // Use the existing helper function to populate AMD device handles
                    uint32_t device_count = _amdsmi_populate_device_count_and_fill_handlers();
                    if (device_count == 0)
                    {
                        OPTKIT_CORE_WARN("No AMD devices found or failed to populate device handles");
                        shutdown_amdsmi();
                    }
                }
                else
                {
                    OPTKIT_CORE_ERROR("ROCm SMI error in amdsmi_init: {}", _amdsmi_status_to_string(result));
                    shutdown_amdsmi();
                }
            }
#endif
            return true;
        }
        else
        {
            OPTKIT_CORE_ERROR("Unsupported or unknown GPU vendor for initialization");
            return false;
        }

        return false;
    }

    bool Query::shutdown(GpuVendor vendor)
    {
        bool is_ok = true;
        if (vendor == GpuVendor::NVIDIA)
        {
            is_ok = is_ok && shutdown_nvml();
        }
        else if (vendor == GpuVendor::AMD)
        {
            is_ok = is_ok && shutdown_amdsmi();
        }
        else
        {
            OPTKIT_CORE_ERROR("Unsupported or unknown GPU vendor for shutdown");
            is_ok = false;
        }
        return is_ok;
    }

    bool Query::shutdown_nvml()
    {
#if OPTKIT_ENV_LIB_NVML
        if (initialized.at(GpuVendor::NVIDIA))
        {
            nvmlReturn_t result = nvmlShutdown();
            bool success = (result == NVML_SUCCESS);
            if (success)
            {
                OPTKIT_CORE_INFO("Shutdown NVML library successfully");
                initialized.at(GpuVendor::NVIDIA) = false;
                Query::gpu_handles_nvml.clear();
                return true;
            }
            else
            {
                OPTKIT_CORE_ERROR("Failed to shutdown NVML library: {}", nvmlErrorString(result));
                return false;
            }
        }
        return true;
#endif
    }
    bool Query::shutdown_amdsmi()
    {

#if OPTKIT_ENV_LIB_AMDSMI
        if (initialized.at(GpuVendor::AMD))
        {
            amdsmi_status_t result = amdsmi_shut_down();
            bool success = (result == AMDSMI_STATUS_SUCCESS);

            if (success)
            {
                OPTKIT_CORE_INFO("Shutdown AMD SMI library successfully");
                initialized.at(GpuVendor::AMD) = false;
                Query::gpu_handles_amdsmi.clear();
                Query::socket_handles_amdsmi.clear();
                return true;
            }
            else
            {
                OPTKIT_CORE_ERROR("Failed to shutdown ROCm SMI library: {}", _amdsmi_status_to_string(result));
                return false;
            }
        }
        return true;
#endif
    }

    GpuDeviceInfo Query::device_query(GpuVendor vendor, uint32_t gpu_id)
    {
        uint32_t device_count;
        get_device_count(vendor, device_count);
        if (device_count == 0)
        {
            OPTKIT_CORE_WARN("Vendor {} not found in device count results", to_string(vendor));
            return {};
        }
        if (OPT_UNLIKELY(gpu_id >= device_count))
        {
            OPTKIT_CORE_WARN("Invalid GPU ID: {}, Total device count: {}", gpu_id, device_count);
            return {};
        }
        GpuDeviceInfo info = {};
        get_basic_info(vendor, gpu_id, info.basic);
        get_version_info(vendor, gpu_id, info.version);
        get_memory_info(vendor, gpu_id, info.memory);
        get_compute_info(vendor, gpu_id, info.compute);
        get_clock_info(vendor, gpu_id, info.clocks);
        get_power_info(vendor, gpu_id, info.power);
        get_temperature_info(vendor, gpu_id, info.temperature);
        get_utilization_info(vendor, gpu_id, info.utilization);
        get_hardware_info(vendor, gpu_id, info.hardware);
        get_capabilities_info(vendor, gpu_id, info.capabilities);
        return info;
    }

    bool Query::get_warp_size(GpuVendor vendor, uint32_t device_index, uint32_t &warp_size)
    {
        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            // NVIDIA GPUs always have warp size of 32
            warp_size = 32;
            return true;
#endif
        }
        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI
            // Get architecture to determine wavefront size
            uint32_t architecture;
            if (get_architecture(vendor, device_index, architecture))
            {
                switch (architecture)
                {
                // GCN architectures use 64-thread wavefronts
                case AMDSMI_DEVICE_ARCH_GCN_1_0:
                case AMDSMI_DEVICE_ARCH_GCN_2_0:
                case AMDSMI_DEVICE_ARCH_GCN_3_0:
                case AMDSMI_DEVICE_ARCH_GCN_4_0:
                case AMDSMI_DEVICE_ARCH_GCN_5_0:
                    warp_size = 64;
                    return true;

                // RDNA architectures use 32-thread wavefronts (with dual-issue)
                case AMDSMI_DEVICE_ARCH_RDNA_1_0:
                case AMDSMI_DEVICE_ARCH_RDNA_2_0:
                case AMDSMI_DEVICE_ARCH_RDNA_3_0:
                    warp_size = 32; // Note: RDNA can also execute 64-thread wavefronts
                    return true;

                // CDNA architectures use 64-thread wavefronts
                case AMDSMI_DEVICE_ARCH_CDNA_1_0:
                case AMDSMI_DEVICE_ARCH_CDNA_2_0:
                case AMDSMI_DEVICE_ARCH_CDNA_3_0:
                    warp_size = 64;
                    return true;

                default:
                    // Default to 64 for unknown AMD architectures (most common)
                    warp_size = 64;
                    return true;
                }
            }
            else
            {
                // Fallback: try to query directly if AMDSMI provides this info
                // For now, default to 64 (most AMD GPUs)
                warp_size = 64;
                return true;
            }
#endif
        }
        else if (vendor == GpuVendor::INTEL)
        {
            // Intel GPUs typically use SIMD width of 16 or 32
            // This would require Intel GPU libraries to query properly
            warp_size = 32; // Common default
            return true;
        }

        // Unknown vendor
        OPTKIT_WARN("Warp size query not known for this GPU vendor, setting 32 by default!");
        warp_size = 32; // Safe default
        return false;
    }

    bool Query::get_basic_info(GpuVendor vendor, uint32_t device_index, GpuBasicInfo &basic_info)
    {
        bool is_ok = true; // stays true if all calls are being successfully made.
        basic_info = {};
        basic_info.id = device_index;
        basic_info.vendor = vendor;
        basic_info.vendor_string = to_string(vendor);
        is_ok = is_ok && Query::get_architecture(vendor, device_index, basic_info.architecture);
        is_ok = is_ok && Query::get_device_name(vendor, device_index, basic_info.device_name);
        return is_ok;
    }

    bool Query::get_version_info(GpuVendor vendor, uint32_t device_index, GpuVersionInfo &version_info)
    {
        version_info = {};
        bool result = true;
        result = result && get_driver_version(vendor, version_info.driver_major_minor);
        version_info.driver_version_string = std::to_string(version_info.driver_major_minor);
        result = result && get_library_version(vendor, version_info.library_version_string);
        return result;
    }

    bool Query::get_compute_info(GpuVendor vendor, uint32_t device_index, GpuComputeInfo &compute_info)
    {
        compute_info = {};
        bool is_ok = true; // stays true if all calls are being successfully made.

        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            nvmlReturn_t result;
            auto nvml_device = Query::gpu_handles_nvml.at(device_index);

            Query::get_warp_size(vendor, device_index, compute_info.warp_size);

            // Get compute capability directly from NVML instead of library version
            int major, minor;
            NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetCudaComputeCapability", nvml_device, result, &major, &minor);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                compute_info.compute_capability_major = major;
                compute_info.compute_capability_minor = minor;

                // Set cores per multiprocessor based on compute capability
                if (major == 2)
                {
                    compute_info.cores_per_mp = (minor == 1) ? 48 : 32;
                }
                else if (major == 3)
                {
                    compute_info.cores_per_mp = 192;
                }
                else if (major == 5)
                {
                    compute_info.cores_per_mp = 128;
                }
                else if (major == 6)
                {
                    compute_info.cores_per_mp = (minor == 0) ? 64 : 128;
                }
                else if (major == 7)
                {
                    compute_info.cores_per_mp = 64;
                }
                else if (major == 8)
                {
                    compute_info.cores_per_mp = (minor == 6) ? 128 : 64;
                }
                else if (major >= 9)
                {
                    compute_info.cores_per_mp = 128;
                }
                else
                {
                    compute_info.cores_per_mp = 32; // Default fallback
                }
            }
            else
            {
                // Fallback to library version method
                std::string version;
                Query::get_library_version(vendor, version);
                compute_info.compute_capability_major = std::strtol(version.substr(0, version.find(".")).c_str(), nullptr, 10);
                compute_info.compute_capability_minor = std::strtol(version.substr(version.find(".") + 1).c_str(), nullptr, 10);
                compute_info.cores_per_mp = 32; // Default fallback
                OPTKIT_WARN("nvmlDeviceGetCudaComputeCapability failed: {}, using fallback", nvmlErrorString(result));
            }

            nvmlDeviceAttributes_t attributes;
            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetAttributes",
                nvml_device,
                result,
                &attributes);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                compute_info.multiprocessor_count = attributes.multiprocessorCount;
                compute_info.total_cores = compute_info.multiprocessor_count * compute_info.cores_per_mp;
            }
            else
            {
                is_ok = false;
                OPTKIT_CORE_WARN("nvmlDeviceGetAttributes: {}", nvmlErrorString(result));
            }

#endif
        }
        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI

            Query::get_warp_size(vendor, device_index, compute_info.warp_size);

            // Initialize cores per compute unit for AMD GPUs
            compute_info.cores_per_mp = 64; // AMD GPUs typically have 64 stream processors per compute unit

            std::string version;
            Query::get_library_version(vendor, version);
            compute_info.compute_capability_major = std::strtol(version.substr(0, version.find(".")).c_str(), nullptr, 10);
            compute_info.compute_capability_minor = std::strtol(version.substr(version.find(".") + 1).c_str(), nullptr, 10);

            // TODO: Fix socket-device mapping issue
            // The current code incorrectly uses device_index for socket access
            // For now, use a safer approach and skip the complex socket enumeration

            amdsmi_status_t result;
            uint32_t processor_count = 0;

            // Skip the problematic socket access for now
            // ROCM_EXEC_IF_SUPPORTS("amdsmi_get_processor_handles",
            //                       Query::socket_handles_amdsmi.at(device_index),
            //                       result,
            //                       &processor_count,
            //                       Query::gpu_handles_amdsmi.at(device_index));

            // Use direct device handle if available
            if (device_index < Query::gpu_handles_amdsmi.size())
            {
                // Use a placeholder count until proper socket mapping is implemented
                compute_info.multiprocessor_count = 16; // Conservative estimate
                compute_info.total_cores = compute_info.multiprocessor_count * compute_info.cores_per_mp;
                OPTKIT_WARN("AMD multiprocessor count using placeholder - socket mapping needs implementation");
            }
            else
            {
                is_ok = false;
                compute_info.multiprocessor_count = 0;
                compute_info.total_cores = 0;
                OPTKIT_WARN("AMD GPU device index {} out of bounds (max: {})",
                            device_index, Query::gpu_handles_amdsmi.size());
            }
#endif
        }
        else
        {
            is_ok = false;
            OPTKIT_WARN("Compute info query not implemented for this GPU vendor");
        }

        return is_ok;
    }

    bool Query::get_memory_info(GpuVendor vendor, uint32_t device_index, GpuMemoryInfo &memory_info)
    {
        memory_info = {};  // Initialize all fields to zero
        bool is_ok = true; // stays true if all calls are being successfully made.

        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            nvmlReturn_t result;
            auto nvml_device = Query::gpu_handles_nvml.at(device_index);

            uint32_t busWidth;
            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetMemoryBusWidth",
                nvml_device,
                result,
                &busWidth);

            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                memory_info.memory_bus_width_bits = busWidth;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetMemoryBusWidth: {}", nvmlErrorString(result));
            }

            nvmlMemory_t nvml_mem_info;
            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetMemoryInfo",
                nvml_device,
                result,
                &nvml_mem_info);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                memory_info.total_global_memory_MBytes = nvml_mem_info.total / 1024.0 / 1024.0;
                memory_info.free_memory_MBytes = nvml_mem_info.free / 1024.0 / 1024.0;
                memory_info.used_memory_MBytes = nvml_mem_info.used / 1024.0 / 1024.0;
                memory_info.memory_utilization_percent = (static_cast<double>(memory_info.used_memory_MBytes) / static_cast<double>(memory_info.total_global_memory_MBytes)) * 100.0;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetMemoryInfo: {}", nvmlErrorString(result));
            }

            // Current memory clock
            uint32_t cur_mem_clock_MHz = 0;
            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetClockInfo",
                nvml_device,
                result,
                NVML_CLOCK_MEM,
                &cur_mem_clock_MHz);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                memory_info.memory_clock_rate_MHz = cur_mem_clock_MHz;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetClockInfo: {}", nvmlErrorString(result));
            }

            // Maximum memory clock
            uint32_t max_mem_clock_MHz = 0;
            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetMaxClockInfo",
                nvml_device,
                result,
                NVML_CLOCK_MEM,
                &max_mem_clock_MHz);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                memory_info.memory_clock_rate_max_MHz = max_mem_clock_MHz;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetMaxClockInfo: {}", nvmlErrorString(result));
            }

            // Get supported memory clocks (proper two-step process)
            uint32_t count = 100;
            // set max uint32_size
            std::vector<uint32_t> clocksMhz(count, std::numeric_limits<uint32_t>::max());
            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetSupportedMemoryClocks",
                nvml_device,
                result,
                &count,
                &clocksMhz[0]);

            if (result == NVML_SUCCESS && !clocksMhz.empty())
            {
                // Find the minimum memory clock from the supported clocks
                memory_info.memory_clock_rate_min_MHz = std::min_element(clocksMhz.begin(), clocksMhz.begin() + count) != clocksMhz.end()
                                                            ? *std::min_element(clocksMhz.begin(), clocksMhz.begin() + count)
                                                            : 200; // Safe fallback
            }
            else
            {
                OPTKIT_CORE_WARN("nvmlDeviceGetSupportedMemoryClocks (data): {}", nvmlErrorString(result));
                memory_info.memory_clock_rate_min_MHz = 200; // Safe fallback
            }
#endif
        }

        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI
            amdsmi_status_t result;
            uint32_t total_memory;
            auto rocm_device = Query::gpu_handles_amdsmi.at(device_index);
            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_gpu_memory_total",
                                  rocm_device,
                                  result,
                                  AMDSMI_MEM_TYPE_VRAM,
                                  &total_memory);
            if (result == AMDSMI_STATUS_SUCCESS)
            {
                memory_info.total_global_memory_MBytes = total_memory / 1024.0 / 1024.0;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("Failed to get AMD GPU total memory: {}", _amdsmi_status_to_string(result));
            }

            uint32_t used_memory;
            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_gpu_memory_usage",
                                  rocm_device,
                                  result,
                                  AMDSMI_MEM_TYPE_VRAM,
                                  &used_memory);
            if (result == AMDSMI_STATUS_SUCCESS)
            {
                memory_info.used_memory_MBytes = used_memory / 1024.0 / 1024.0;
                memory_info.free_memory_MBytes = memory_info.total_global_memory_MBytes - memory_info.used_memory_MBytes;
                memory_info.memory_utilization_percent = (static_cast<double>(memory_info.used_memory_MBytes) / static_cast<double>(memory_info.total_global_memory_MBytes)) * 100.0;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("Failed to get AMD GPU used memory: {}", _amdsmi_status_to_string(result));
            }
            amdsmi_clk_info_t clk_info;
            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_clock_info",
                                  rocm_device,
                                  result,
                                  AMDSMI_CLK_TYPE_MEM,
                                  &clk_info);
            if (result == AMDSMI_STATUS_SUCCESS)
            {
                memory_info.memory_clock_rate_max_MHz = clk_info.max_clk; // in MHz
                memory_info.memory_clock_rate_min_MHz = clk_info.min_clk; // in MHz
                memory_info.memory_clock_rate_MHz = clk_info.clk;         // in MHz
            }

#endif
        }
        else
        {
            is_ok = false;
            OPTKIT_WARN("Memory info query not implemented for this GPU vendor");
        }
        return is_ok;
    }

    bool Query::get_clock_info(GpuVendor vendor, uint32_t device_index, GpuClockInfo &clock_info)
    {
        bool is_ok = true;
        clock_info = {}; // Initialize all fields to zero
        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            // Get current clock rates
            nvmlReturn_t result;
            uint32_t clockRate;
            auto device = Query::gpu_handles_nvml.at(device_index);
            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetClockInfo",
                device,
                result,
                NVML_CLOCK_GRAPHICS,
                &clockRate);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                clock_info.current_graphics_clock_MHz = clockRate;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetClockInfo: {}", nvmlErrorString(result));
            }
            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetClockInfo",
                device,
                result,
                NVML_CLOCK_SM,
                &clockRate);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                clock_info.current_sm_clock_MHz = clockRate;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetClockInfo: {}", nvmlErrorString(result));
            }
            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetClockInfo",
                device,
                result,
                NVML_CLOCK_MEM,
                &clockRate);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                clock_info.current_memory_clock_MHz = clockRate;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetClockInfo: {}", nvmlErrorString(result));
            }
            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetClockInfo",
                device,
                result,
                NVML_CLOCK_VIDEO,
                &clockRate);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                clock_info.current_video_clock_MHz = clockRate;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetClockInfo: {}", nvmlErrorString(result));
            }

            // Get MAX clocks
            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetMaxClockInfo",
                device,
                result,
                NVML_CLOCK_GRAPHICS,
                &clockRate);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                clock_info.max_graphics_clock_MHz = clockRate;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetMaxClockInfo: {}", nvmlErrorString(result));
            }

            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetMaxClockInfo",
                device,
                result,
                NVML_CLOCK_SM,
                &clockRate);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                clock_info.max_sm_clock_MHz = clockRate;
            }
            else
            {
                is_ok = false;
                clock_info.max_sm_clock_MHz = 0;
                OPTKIT_WARN("nvmlDeviceGetMaxClockInfo: {}", nvmlErrorString(result));
            }

            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetMaxClockInfo",
                device,
                result,
                NVML_CLOCK_MEM,
                &clockRate);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                clock_info.max_memory_clock_MHz = clockRate;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetMaxClockInfo: {}", nvmlErrorString(result));
            }

            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetMaxClockInfo",
                device,
                result,
                NVML_CLOCK_VIDEO,
                &clockRate);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                clock_info.current_video_clock_MHz = clockRate;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetMaxClockInfo: {}", nvmlErrorString(result));
            }
            clock_info.has_frequency_control = true; // NVIDIA GPUs generally support frequency control
#endif
        }
        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI
            amdsmi_status_t result;
            // Get clock frequencies
            amdsmi_frequencies_t frequencies;

            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_clk_freq",
                                  Query::gpu_handles_amdsmi.at(device_index),
                                  result,
                                  AMDSMI_CLK_TYPE_SYS,
                                  &frequencies);
            if (result == AMDSMI_STATUS_SUCCESS && frequencies.num_supported > 0)
            {
                clock_info.current_sm_clock_MHz = frequencies.current;
                clock_info.max_sm_clock_MHz = frequencies.frequency[frequencies.num_supported - 1];
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("Failed to get AMD GPU SM clock info: {}", _amdsmi_status_to_string(result));
            }

            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_clk_freq",
                                  Query::gpu_handles_amdsmi.at(device_index),
                                  result,
                                  AMDSMI_CLK_TYPE_GFX,
                                  &frequencies);
            if (result == AMDSMI_STATUS_SUCCESS && frequencies.num_supported > 0)
            {
                clock_info.current_graphics_clock_MHz = frequencies.current;
                clock_info.max_graphics_clock_MHz = frequencies.frequency[frequencies.num_supported - 1];
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("Failed to get AMD GPU Graphics clock info: {}", _amdsmi_status_to_string(result));
            }

            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_clk_freq",
                                  Query::gpu_handles_amdsmi.at(device_index),
                                  result,
                                  AMDSMI_CLK_TYPE_VCLK0,
                                  &frequencies);
            if (result == AMDSMI_STATUS_SUCCESS && frequencies.num_supported > 0)
            {
                clock_info.current_video_clock_MHz = frequencies.current;
                clock_info.max_video_clock_MHz = frequencies.frequency[frequencies.num_supported - 1];
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("Failed to get AMD GPU Video clock info: {}", _amdsmi_status_to_string(result));
            }

            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_clk_freq",
                                  Query::gpu_handles_amdsmi.at(device_index),
                                  result,
                                  AMDSMI_CLK_TYPE_MEM,
                                  &frequencies);
            if (result == AMDSMI_STATUS_SUCCESS && frequencies.num_supported > 0)
            {
                clock_info.current_memory_clock_MHz = frequencies.current;
                clock_info.max_memory_clock_MHz = frequencies.frequency[frequencies.num_supported - 1];
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("Failed to get AMD GPU Memory clock info: {}", _amdsmi_status_to_string(result));
            }
            clock_info.has_frequency_control = true; // AMD GPUs generally support frequency control
#endif
        }

        return is_ok;
    }

    bool Query::get_power_info(GpuVendor vendor, uint32_t device_index, GpuPowerInfo &power_info)
    {
        power_info = {};
        bool is_ok = true; // stays true if all calls are being successfully made.

        get_device_power(vendor, device_index, power_info.current_power_watts);
        power_info.has_power_monitoring = (power_info.current_power_watts > 0.0);
        get_device_power_limits(vendor, device_index, power_info.power_limit_watts,
                                power_info.default_power_watts,
                                power_info.min_power_watts,
                                power_info.max_power_watts,
                                power_info.is_configurable);

        return is_ok;
    }

    bool Query::get_temperature_info(GpuVendor vendor, uint32_t device_index, GpuTemperatureInfo &temperature_info)
    {
        temperature_info = {};
        bool is_okay = true; // stays true if all calls are being successfully made.

        if (Query::get_device_temperature(vendor, device_index, temperature_info.current_temperature_celsius))
        {
            temperature_info.has_temperature_monitoring = true;
            is_okay = get_device_temperature_thresholds(vendor, device_index,
                                                        temperature_info.max_temperature_celsius);
        }
        else
        {
            temperature_info.has_temperature_monitoring = false;
            is_okay = false;
        }

        return is_okay;
    }

    bool Query::get_utilization_info(GpuVendor vendor, uint32_t device_index, GpuUtilizationInfo &utilization_info)
    {
        bool is_ok = true;
        utilization_info = {};
        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            nvmlUtilization_t utilization;
            nvmlReturn_t result;
            NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetUtilizationRates",
                                  Query::gpu_handles_nvml.at(device_index),
                                  result,
                                  &utilization);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                utilization_info.gpu_utilization_percent = utilization.gpu;
                utilization_info.memory_utilization_percent = utilization.memory;
                utilization_info.has_utilization_monitoring = true;
            }
            else
            {
                is_ok = false;
                utilization_info.has_utilization_monitoring = false;
                OPTKIT_WARN("Failed to get NVIDIA GPU utilization info: {}", std::string(nvmlErrorString(result)));
            }
#endif
        }
        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI
            amdsmi_status_t result;
            uint32_t count = 2;
            uint32_t timestamp;
            amdsmi_utilization_counter_t utilization_counters[count]{AMDSMI_COARSE_GRAIN_GFX_ACTIVITY, AMDSMI_FINE_GRAIN_MEM_ACTIVITY};

            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_utilization_count",
                                  Query::gpu_handles_amdsmi.at(device_index),
                                  result,
                                  utilization_counters,
                                  count,
                                  &timestamp);
            if (result == AMDSMI_STATUS_SUCCESS)
            {
                utilization_info.gpu_utilization_percent = utilization_counters[0].value;
                utilization_info.memory_utilization_percent = utilization_counters[1].value;
                utilization_info.has_utilization_monitoring = true;
            }
            else
            {
                is_ok = false;
                utilization_info.has_utilization_monitoring = false;
                OPTKIT_WARN("Failed to get AMD GPU utilization info: {}", _amdsmi_status_to_string(result));
            }
#endif
        }
        return is_ok;
    }

    // TODO: fill this function
    bool Query::get_hardware_info(GpuVendor vendor, uint32_t device_index, GpuHardwareInfo &hardware_info)
    {
        hardware_info = {};
        bool is_ok = true; // stays true if all calls are being successfully made.

        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            nvmlReturn_t result;
            nvmlPciInfo_t pci;
            auto device = Query::gpu_handles_nvml.at(device_index);
            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetPciInfo",
                device,
                result,
                &pci);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                hardware_info.pci_bus_id = std::string(pci.busId);
                hardware_info.pci_device_id = pci.pciDeviceId;
                hardware_info.pci_subsystem_id = pci.pciSubSystemId;
            }
            else
            {
                OPTKIT_WARN("nvmlDeviceGetPciInfo: {}", nvmlErrorString(result));
            }
            // Get board ID for multi-GPU detection
            uint32_t boardId;
            NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetBoardId", device, result, &boardId);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                hardware_info.board_id = boardId;
            }
            else
            {
                OPTKIT_WARN("nvmlDeviceGetBoardId: {}", nvmlErrorString(result));
            }
#endif
        }
        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI
            auto device = Query::gpu_handles_amdsmi.at(device_index);
            amdsmi_status_t result;
#endif
        }
        else
        {
            OPTKIT_WARN("Compute info query not implemented for this GPU vendor");
            return false;
        }

        return is_ok;
    }

    bool Query::get_capabilities_info(GpuVendor vendor, uint32_t device_index, GpuCapabilitiesInfo &capabilities_info)
    {
        bool is_ok = true;
        capabilities_info = {};
        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            nvmlEnableState_t current, pending;
            nvmlReturn_t result;
            auto device = Query::gpu_handles_nvml.at(device_index);
            NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetEccMode", device, result, &current, &pending);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                capabilities_info.ecc_enabled = (current == NVML_FEATURE_ENABLED);
            }
            else
            {
                OPTKIT_CORE_WARN("nvmlDeviceGetEccMode: {}", nvmlErrorString(result));
            }
            // Get persistence mode
            nvmlEnableState_t mode;
            NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetPersistenceMode", device, result, &mode);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                capabilities_info.persistence_mode_enabled = (mode == NVML_FEATURE_ENABLED);
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetPersistenceMode: {}", nvmlErrorString(result));
            }

            uint32_t arch;
            get_architecture(vendor, device_index, arch);

            // Unified Memory supported on Kepler (3) and later architectures
            capabilities_info.supports_unified_memory = (arch >= NVML_DEVICE_ARCH_KEPLER && arch != NVML_DEVICE_ARCH_UNKNOWN);
#endif
        }
        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI
            uint64_t enabled_blocks;
            amdsmi_status_t result;
            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_gpu_ecc_enabled",
                                  Query::gpu_handles_amdsmi.at(device_index),
                                  result,
                                  &enabled_blocks);
            if (result == AMDSMI_STATUS_SUCCESS)
            {
                capabilities_info.ecc_enabled = (enabled_blocks != 0);
            }
            else
            {
                OPTKIT_WARN("Failed to get AMD GPU ECC status: {}", _amdsmi_status_to_string(result));
            }

            capabilities_info.persistence_mode_enabled = false; // AMD GPUs do not have persistence mode

            amdsmi_vram_info_t vram_info;
            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_gpu_vram_info",
                                  Query::gpu_handles_amdsmi.at(device_index),
                                  result,
                                  &vram_info);
            if (result == AMDSMI_STATUS_SUCCESS)
            { // Check for HBM memory (indicates high-end GPU with advanced memory features)
                bool has_hbm = (vram_info.vram_type == AMDSMI_VRAM_TYPE_HBM ||
                                vram_info.vram_type == AMDSMI_VRAM_TYPE_HBM2 ||
                                vram_info.vram_type == AMDSMI_VRAM_TYPE_HBM2E ||
                                vram_info.vram_type == AMDSMI_VRAM_TYPE_HBM3);

                // Check VRAM size - larger VRAM typically indicates support for advanced features
                capabilities_info.supports_unified_memory = (vram_info.vram_size > 8000); // > 8GB suggests modern GPU
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("Failed to get AMD GPU unified memory capability: {}", _amdsmi_status_to_string(result));
            }
#endif
        }

        return is_ok;
    }

    bool Query::get_driver_version(GpuVendor vendor, double &driver_version)
    {
        bool is_ok = true;
        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            int32_t version;
            nvmlReturn_t result = nvmlSystemGetCudaDriverVersion(&version);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                int32_t major = NVML_CUDA_DRIVER_VERSION_MAJOR(version);
                int32_t minor = NVML_CUDA_DRIVER_VERSION_MINOR(version);
                driver_version = major + minor;
            }
            else
            {
                is_ok = false;
                driver_version = 0.0;
                OPTKIT_CORE_ERROR("NVML error in nvmlSystemGetCudaDriverVersion: {}", std::string(nvmlErrorString(result)));
            }
#endif
        }
        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI
            amdsmi_version_t amdsmi_version{};

            amdsmi_status_t status = amdsmi_get_lib_version(&amdsmi_version);
            if (status == AMDSMI_STATUS_SUCCESS)
            {
                int32_t major = amdsmi_version.major, minor = amdsmi_version.minor;
                driver_version = major + minor / 10.0; // major.minor as double
            }
            else
            {
                is_ok = false;
                driver_version = 0.0;
                OPTKIT_CORE_ERROR("ROCm SMI failed to get version: {}", _amdsmi_status_to_string(status));
            }

#endif
        }
        else
        {
            is_ok = false;
            OPTKIT_WARN("Driver version query not supported without NVML or ROCm SMI");
        }
        return is_ok;
    }

    bool Query::get_library_version(GpuVendor vendor, std::string &library_version)
    {
        bool is_ok = true;
        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            char version[NVML_SYSTEM_NVML_VERSION_BUFFER_SIZE] = {0};
            nvmlReturn_t result_nvidia = nvmlSystemGetNVMLVersion(version, NVML_SYSTEM_NVML_VERSION_BUFFER_SIZE);
            if (result_nvidia == NVML_SUCCESS)
            {
                library_version = std::string(version);
            }
            else
            {
                is_ok = false;
                library_version = "0.0";
                OPTKIT_CORE_ERROR("NVML error in nvmlSystemGetNVMLVersion: {}", std::string(nvmlErrorString(result_nvidia)));
            }
#endif
        }
        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI
            amdsmi_version_t amdsmi_version{};
            amdsmi_status_t result_amd = amdsmi_get_lib_version(&amdsmi_version);
            if (result_amd == AMDSMI_STATUS_SUCCESS)
            {
                int32_t major = amdsmi_version.major;
                int32_t minor = amdsmi_version.minor;
                library_version = std::to_string(major) + "." + std::to_string(minor); // major.minor as string
            }
            else
            {
                is_ok = false;
                library_version = "0.0";
                OPTKIT_CORE_ERROR("ROCm SMI failed to get version: {}", _amdsmi_status_to_string(result_amd));
            }
#endif
        }
        else
        {
            is_ok = false;
            OPTKIT_WARN("Library version query not supported without NVML or ROCm SMI");
        }
        return is_ok;
    }

    bool Query::get_device_count(GpuVendor vendor, uint32_t &device_count)
    {
        bool is_ok = true;
        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            uint32_t count;
            nvmlReturn_t result = nvmlDeviceGetCount(&count);
            device_count = (result == NVML_SUCCESS) ? count : 0;
#endif
        }
        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI
            device_count = _amdsmi_populate_device_count_and_fill_handlers();
#endif
        }
        else
        {
            is_ok = false;
            OPTKIT_WARN("Device count query not supported without NVML or ROCm AMDSMI");
        }
        return is_ok;
    }

    bool Query::get_device_power(GpuVendor vendor, uint32_t device_index, double &power_watts)
    {
        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            nvmlDevice_t device = Query::gpu_handles_nvml.at(device_index);
            nvmlReturn_t result_nvidia;
            uint32_t power_mw;
            NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetPowerUsage", device, result_nvidia, &power_mw);
            if (result_nvidia == NVML_SUCCESS)
            {
                // note that this is 1 seconds average power usage
                power_watts = power_mw / 1000.0; // Convert from milliwatts to watts
                return true;
            }
            else
            {
                OPTKIT_WARN("nvmlDeviceGetPowerUsage: {}", nvmlErrorString(result_nvidia));
                power_watts = 0.0; // Not supported
                return false;
            }
#endif
        }

        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI
            amdsmi_power_info_t power_info;
            amdsmi_status_t result_amd;
            // Returns the current power and voltage of the GPU.
            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_power_info",
                                  Query::gpu_handles_amdsmi.at(device_index),
                                  result_amd,
                                  &power_info);
            if (result_amd == AMDSMI_STATUS_SUCCESS)
            {
                power_watts = power_info.average_socket_power;
                return true;
            }
            else
            {
                OPTKIT_WARN("amdsmi_get_power_info: {}", _amdsmi_status_to_string(result_amd));
                power_watts = 0.0; // Not supported
                return false;
            }
#endif
        }
        OPTKIT_WARN("Device power query not supported without NVML or ROCm SMI");
        power_watts = 0.0;
        return false;
    }

    bool Query::get_device_power_limits(GpuVendor vendor, uint32_t device_index, double &limit_watts, double &default_power, double &min_limit_watts, double &max_limit_watts, bool &is_configurable)
    {
        bool is_ok = true;

        limit_watts = 0.0;
        min_limit_watts = 0.0;
        max_limit_watts = 0.0;
        default_power = 0.0;
        is_configurable = false;

        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            nvmlDevice_t device = Query::gpu_handles_nvml.at(device_index);
            nvmlReturn_t result_nvidia;
            uint32_t limit_mw;
            nvmlEnableState_t power_management_state;

            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetPowerManagementMode",
                device,
                result_nvidia,
                &power_management_state);
            if (result_nvidia == NVML_SUCCESS)
            {
                is_configurable = (power_management_state == NVML_FEATURE_ENABLED);
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetPowerManagementMode: {}", nvmlErrorString(result_nvidia));
            }

            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetPowerManagementLimit",
                device,
                result_nvidia,
                &limit_mw);
            if (result_nvidia == NVML_SUCCESS)
            {
                limit_watts = limit_mw / 1000.0; // Convert from milliwatts to watts
                return true;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetPowerManagementLimit: {}", nvmlErrorString(result_nvidia));
                limit_watts = 0.0; // Not supported
            }

            uint32_t min_power, max_power, default_power;
            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetPowerManagementLimitConstraints",
                device,
                result_nvidia,
                &min_power,
                &max_power);
            if (result_nvidia == NVML_SUCCESS)
            {
                min_limit_watts = min_power / 1000.0;
                max_limit_watts = max_power / 1000.0;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetPowerManagementLimitConstraints: {}", nvmlErrorString(result_nvidia));
            }

            NVML_EXEC_IF_SUPPORTS(
                "nvmlDeviceGetPowerManagementDefaultLimit",
                device,
                result_nvidia,
                &default_power);
            if (result_nvidia == NVML_SUCCESS)
            {
                default_power = default_power / 1000.0;
            }
            else
            {
                is_ok = false;
                OPTKIT_WARN("nvmlDeviceGetPowerManagementDefaultLimit: {}", nvmlErrorString(result_nvidia));
            }
#endif
        }

        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI

            amdsmi_power_info_t power_info;
            amdsmi_status_t result_amd;
            // Returns the current power and voltage of the GPU.
            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_power_info",
                                  Query::gpu_handles_amdsmi.at(device_index),
                                  result_amd,
                                  &power_info);
            if (result_amd == AMDSMI_STATUS_SUCCESS)
            {
                limit_watts = power_info.power_limit;
                return true;
            }
            else
            {
                OPTKIT_WARN("amdsmi_get_power_info: {}", _amdsmi_status_to_string(result_amd));
                limit_watts = 0.0; // Not supported
            }
#endif
        }
        else
        {
            limit_watts = 0.0;
            OPTKIT_WARN("Device power limit query not supported without NVML or ROCm SMI");
        }

        return is_ok;
    }

    bool Query::get_device_temperature(GpuVendor vendor, uint32_t device_index, double &temp_celsius)
    {
        temp_celsius = 0.0;
        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            nvmlDevice_t device = Query::gpu_handles_nvml.at(device_index);
            nvmlReturn_t result;
            uint32_t temp;

            NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetTemperature", device, result, NVML_TEMPERATURE_GPU, &temp);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                temp_celsius = static_cast<double>(temp);
                return true;
            }
            else
            {
                temp_celsius = 0.0; // Not supported
                OPTKIT_WARN("nvmlDeviceGetTemperature: {}", nvmlErrorString(result));
            }
#endif
            return false;
        }

        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI
            uint32_t temperature;
            amdsmi_status_t result;
            amdsmi_temperature_type_t sensor_type = AMDSMI_TEMPERATURE_TYPE_EDGE;
            amdsmi_temperature_metric_t metric = AMDSMI_TEMP_CURRENT;
            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_temp_metric",
                                  Query::gpu_handles_amdsmi.at(device_index),
                                  result,
                                  sensor_type,
                                  metric,
                                  &temperature);
            if (result == AMDSMI_STATUS_SUCCESS)
            {
                temp_celsius = static_cast<double>(temperature) / 1000.0; // Convert from millidegrees to degrees
                return true;
            }
            else
            {
                temp_celsius = 0.0; // Not supported
                OPTKIT_WARN("amdsmi_get_cpu_socket_temperature: {}", _amdsmi_status_to_string(result));
            }
#endif
        }

        // fallback to false
        return false;
    }

    bool Query::get_device_name(GpuVendor vendor, uint32_t device_index, std::string &device_name)
    {
        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            nvmlDevice_t device = Query::gpu_handles_nvml.at(device_index);
            nvmlReturn_t result;
            // Get device name
            char name[NVML_DEVICE_NAME_BUFFER_SIZE];
            NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetName", device, result, &name[0], NVML_DEVICE_NAME_BUFFER_SIZE);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                device_name = std::string(name);
                return true;
            }
            else
            {
                OPTKIT_WARN("nvmlDeviceGetName: {}", nvmlErrorString(result));
                return false;
            }
#endif
        }
        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI
            amdsmi_board_info_t info;
            amdsmi_status_t result;
            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_gpu_board_info",
                                  Query::gpu_handles_amdsmi.at(device_index),
                                  result,
                                  &info);
            if (result == AMDSMI_STATUS_SUCCESS)
            {
                device_name = std::string(info.product_name);
                return true;
            }
            else
            {
                OPTKIT_WARN("amdsmi_get_gpu_board_info: {}", _amdsmi_status_to_string(result));
                return false;
            }
#endif
        }
        else
        {
            OPTKIT_WARN("Device name query not supported without NVML or ROCm SMI");
            return false;
        }
    }

    bool Query::get_architecture(GpuVendor vendor, uint32_t device_index, uint32_t &architecture)
    {

        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            uint32_t arch;
            nvmlDevice_t device = Query::gpu_handles_nvml.at(device_index);
            nvmlReturn_t result_nvidia;
            NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetArchitecture", device, result_nvidia, &arch);
            if (result_nvidia == NVML_SUCCESS)
            {
                architecture = static_cast<uint32_t>(arch);
                return true;
            }
            else
            {
                architecture = NVML_DEVICE_ARCH_UNKNOWN; // Unknown architecture
                return false;
            }
#endif
        }

        if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI
            // Try to get device ID from ASIC info
            amdsmi_asic_info_t asic_info{};
            amdsmi_status_t result_amdsmi;
            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_gpu_asic_info",
                                  Query::gpu_handles_amdsmi.at(device_index),
                                  result_amdsmi,
                                  &asic_info);

            if (result_amdsmi == AMDSMI_STATUS_SUCCESS)
            {
                uint32_t device_id = static_cast<uint32_t>(asic_info.device_id);
                architecture = _map_amd_device_id_to_arch(device_id);
                return true;
            }
            else
            {
                architecture = AMDSMI_DEVICE_ARCH_UNKNOWN;
            }
            return false;
#endif
        }

        OPTKIT_WARN("Unsupported vendor for architecture query");
        return 0xFFFFFFFF; // return unknown architecture
    }

    bool Query::get_device_temperature_thresholds(GpuVendor vendor, uint32_t device_index, double &max_temp_celsius)
    {
        bool is_ok = true;

        if (vendor == GpuVendor::NVIDIA)
        {
#if OPTKIT_ENV_LIB_NVML
            uint32_t temp;
            nvmlReturn_t result;
            NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetTemperatureThreshold",
                                  Query::gpu_handles_nvml.at(device_index),
                                  result,
                                  NVML_TEMPERATURE_THRESHOLD_GPU_MAX,
                                  &temp);
            if (OPT_LIKELY(result == NVML_SUCCESS))
            {
                max_temp_celsius = static_cast<double>(temp);
            }
            else
            {
                is_ok = false;
                max_temp_celsius = 0;
                OPTKIT_WARN("Failed to get NVIDIA max temperature threshold: {}", std::string(nvmlErrorString(result)));
            }
#endif
        }
        else if (vendor == GpuVendor::AMD)
        {
#if OPTKIT_ENV_LIB_AMDSMI
            uint32_t temperature;
            amdsmi_status_t result;
            amdsmi_temperature_type_t sensor_type = AMDSMI_TEMPERATURE_TYPE_EDGE;
            amdsmi_temperature_metric_t metric = AMDSMI_TEMP_MAX;
            ROCM_EXEC_IF_SUPPORTS("amdsmi_get_temp_metric",
                                  Query::gpu_handles_amdsmi.at(device_index),
                                  result,
                                  sensor_type,
                                  metric,
                                  &temperature);
            if (result == AMDSMI_STATUS_SUCCESS)
            {
                max_temp_celsius = static_cast<double>(temperature) / 1000.0; // Convert from millidegrees to degrees
            }
            else
            {
                is_ok = false;
                max_temp_celsius = 0.0; // Not supported
                OPTKIT_WARN("amdsmi_get_cpu_socket_temperature: {}", _amdsmi_status_to_string(result));
            }
#endif
        }
        return is_ok;
    }

} // namespace optkit::gpu
