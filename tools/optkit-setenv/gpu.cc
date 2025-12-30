#include "gpu.hh"

const std::string GPU::name = "gpu";

std::string GPU::to_string() const
{
    std::ostringstream oss;
    oss << "GPU{device_name=" << device_name
        << ", persistence=" << persistence_mode
        << ", fan=" << fan_speed
        << ", core_freq=" << core_freq_mhz << "MHz"
        << ", mem_freq=" << mem_freq_mhz << "MHz"
        << ", power_limit=" << power_limit_watts << "W"
        << ", reset_device=" << (reset_device ? "yes" : "no")
        << "}";
    return oss.str();
}
bool GPU::is_valid() const
{
    bool result = true;

    OPTKIT_GPU_VENDOR_TRAVERSE(vendor)
    {
        if (optkit::gpu::Query::is_device_exists(vendor))
        {
            optkit::gpu::GpuDeviceInfo device_info{};
            optkit::gpu::Query::device_query(vendor, 0, device_info);

            if (fan_speed != "auto")
            {
                try
                {
                    int speed = std::stoi(fan_speed);
                    if (speed < 0 || speed > 100)
                    {
                        OPTKIT_WARN("Fan speed must be between 0 and 100: fan_speed={}", speed);
                        result = false;
                    }
                }
                catch (...)
                {
                    OPTKIT_WARN("Invalid fan speed format (must be 'auto' or 0-100): fan_speed={}", fan_speed);
                    result = false;
                }
            }
            if (core_freq_mhz < device_info.clocks.min_graphics_clock_MHz || core_freq_mhz > device_info.clocks.max_graphics_clock_MHz)
            {
                OPTKIT_WARN("Core frequency out of range: core_freq_mhz={} (min {} max {})", core_freq_mhz, device_info.clocks.min_graphics_clock_MHz, device_info.clocks.max_graphics_clock_MHz);
                result = false;
            }
            if (mem_freq_mhz < device_info.clocks.min_memory_clock_MHz || mem_freq_mhz > device_info.clocks.max_memory_clock_MHz)
            {
                OPTKIT_WARN("Memory frequency out of range: mem_freq_mhz={} (min {} max {})", mem_freq_mhz, device_info.clocks.min_memory_clock_MHz, device_info.clocks.max_memory_clock_MHz);
                result = false;
            }
        }
    }

    return result;
}

bool GPU::apply(pid_t pid)
{
    bool result = true;
    OPTKIT_GPU_VENDOR_TRAVERSE(vendor)
    {
        if (optkit::gpu::Query::is_device_exists(vendor))
        {
            uint32_t device_count = 0;
            optkit::gpu::Query::get_device_count(vendor, device_count);
            for (int device_index = 0; device_index < device_count; ++device_index)
            {
                // Apply all settings, continue on failure but track overall result
                if (!optkit::gpu::Query::set_persistence_mode(vendor, device_index, (persistence_mode == "on")))
                    result = false;

                if (fan_speed != "auto")
                {
                    if (!optkit::gpu::Query::set_fan_speed(vendor, device_index, fan_speed))
                        result = false;
                }
                else
                {
                    if (!optkit::gpu::Query::reset_fan_speed(vendor, device_index))
                        result = false;
                }

                if (core_freq_mhz > 0 && mem_freq_mhz > 0)
                {
                    if (!optkit::gpu::Query::set_clock(vendor, device_index, static_cast<uint32_t>(mem_freq_mhz), static_cast<uint32_t>(core_freq_mhz)))
                        result = false;
                }
                if (power_limit_watts > 0)
                {
                    if (!optkit::gpu::Query::set_power_limit(vendor, device_index, static_cast<double>(power_limit_watts)))
                        result = false;
                }
                if (reset_device)
                {
                    if (!optkit::gpu::Query::reset_device(vendor, device_index))
                        result = false;
                }
            }
        }
    }
    return result;
}

void GPU::load_current_settings(pid_t pid)
{
    (void)pid; // Unused for GPU module
    // GPU settings are typically per-device, not per-process
    // Would need device_index parameter to query specific GPU
    OPTKIT_GPU_VENDOR_TRAVERSE(vendor)
    {
        if (optkit::gpu::Query::is_device_exists(vendor))
        {
            optkit::gpu::GpuDeviceInfo device_info{};
            optkit::gpu::Query::device_query(vendor, 0, device_info);
            this->device_name = device_info.basic.device_name;
            this->persistence_mode = device_info.capabilities.persistence_mode_enabled ? "on" : "off";
            this->fan_speed = "auto";
            this->core_freq_mhz = device_info.clocks.current_graphics_clock_MHz;
            this->mem_freq_mhz = device_info.clocks.current_memory_clock_MHz;
            this->power_limit_watts = device_info.power.power_limit_watts;
            this->reset_device = false;
            break;
        }
    }
}

nlohmann::json GPU::to_json() const
{
    nlohmann::json j;
    j["device_name"] = device_name;
    j["persistence_mode"] = persistence_mode;
    j["fan_speed"] = fan_speed;
    j["core_freq_mhz"] = core_freq_mhz;
    j["mem_freq_mhz"] = mem_freq_mhz;
    j["power_limit_watts"] = power_limit_watts;
    j["reset_device"] = reset_device;
    return j;
}
std::string GPU::possible_values() const
{
    std::ostringstream oss;
    OPTKIT_GPU_VENDOR_TRAVERSE(vendor)
    {
        if (optkit::gpu::Query::is_device_exists(vendor))
        {
            optkit::gpu::GpuDeviceInfo device_info{};
            optkit::gpu::Query::device_query(vendor, 0, device_info);
            oss << "persistence_mode: on, off\n";
            oss << "fan_speed: auto, 0-100%\n";
            oss << "\tmem_freq_mhz: ";
            for (auto &&i : device_info.clocks.memory_supported_clock_rates_MHz)
                oss << i << ",";

            if (!device_info.clocks.memory_supported_clock_rates_MHz.empty())
                oss.seekp(-1, oss.cur);
            oss << "\n";

            oss << "\tcore_freq_mhz: ";
            for (auto &&i : device_info.clocks.graphics_supported_clock_rates_MHz)
            {
                oss << "[" << i.first << "]:";
                for (auto &&j : i.second)
                    oss << j << ",";
                oss << "\n";
            }

            if (!device_info.clocks.graphics_supported_clock_rates_MHz.empty())
                oss.seekp(-1, oss.cur);
            oss << "\n";
            oss << "\tpower_limit_watts: " << device_info.power.min_power_watts << "-" << device_info.power.max_power_watts << " limit:" << device_info.power.power_limit_watts << "\n";
            oss << "\treset_device: true, false\n";
        }
    }
    return oss.str();
}