#include "core/temperature/temperature_profiler.hh"
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>

namespace optkit::core::temperature
{
    // Static member definition
    std::unordered_map<std::string, std::string> TemperatureProfiler::sensor_paths;

    void TemperatureProfiler::init()
    {
        sensor_paths = discover_hwmon_sensors();
        if (sensor_paths.empty())
        {
            OPTKIT_CORE_WARN("No temperature sensors found in /sys/class/hwmon/");
        }
        else
        {
            OPTKIT_CORE_INFO("Discovered {} temperature sensors", sensor_paths.size());
        }
    }

    TemperatureProfiler::TemperatureProfiler(const char *block_name, const core::metrics::MetricBuilder &mb, bool verbose)
        : BaseProfiler<std::vector<uint64_t>>(block_name, "temperature", verbose), metric_builder(mb)
    {
        // Take initial snapshot
        last_snapshot = read_selected_temperature_sensors(metric_builder.event_names());
    }

    TemperatureProfiler::~TemperatureProfiler()
    {
        this->read_and_store();
        this->metric_results = this->metric_builder.calculate(aggregate());

        if (OPT_LIKELY(Query::create_folder))
            this->save();

        if (OPT_LIKELY(this->verbose))
        {
            std::cout << std::fixed << "\033[1;33m" // Yellow for temperature
                      << "Block: " << this->block_name << "\033[0m"
                      << " [" << this->total_duration_ms << "ms] Measured\n";

            if (OPT_UNLIKELY(this->metric_builder.print_events))
                for (auto &&event : this->event_results)
                    std::cout << std::fixed << "\t" << event.first << ": " << event.second / 1000.0 << "°C" << std::endl;

            for (auto &&metric : this->metric_results)
                std::cout << std::fixed << "\t" << metric.first << ": " << metric.second << std::endl;
        }
    }

    std::vector<uint64_t> TemperatureProfiler::read()
    {
        auto current_snapshot = read_selected_temperature_sensors(metric_builder.event_names());
        std::vector<uint64_t> current_temps;

        for (const auto &sensor_name : metric_builder.event_names())
        {
            auto it = current_snapshot.find(sensor_name);
            uint64_t temp = (it != current_snapshot.end()) ? it->second : 0;
            current_temps.push_back(temp);

            // Store for aggregation (temperatures are instantaneous, not cumulative)
            this->event_results.push_back({sensor_name, temp});
        }

        return current_temps;
    }

    std::unordered_map<std::string, uint64_t> TemperatureProfiler::aggregate()
    {
        std::unordered_map<std::string, uint64_t> aggregated;
        std::unordered_map<std::string, std::vector<uint64_t>> sensor_readings;

        // Group readings by sensor
        for (const auto &event : this->event_results)
        {
            sensor_readings[event.first].push_back(event.second);
        }

        // Calculate average temperature for each sensor
        for (const auto &sensor_pair : sensor_readings)
        {
            if (!sensor_pair.second.empty())
            {
                uint64_t sum = 0;
                for (uint64_t temp : sensor_pair.second)
                {
                    sum += temp;
                }
                aggregated[sensor_pair.first] = sum / sensor_pair.second.size();
            }
        }

        return aggregated;
    }

    std::string TemperatureProfiler::to_json()
    {
        std::stringstream ss;
        ss << "[\n";
        ss << utils::to_json(this->total_duration_ms, this->measurement_type, this->event_results, this->metric_results);
        ss << "]\n";
        return ss.str();
    }

    static bool match_any(const std::string &name, const std::vector<std::string> &patterns)
    {
        for (auto &pat : patterns)
        {
            if (name.find(pat) != std::string::npos)
                return true;
        }
        return false;
    }

    std::string TemperatureProfiler::build_sensor_name(const std::string &hwmon_name,
                                                       const std::string &temp_num_str)
    {
        // Group patterns
        static const std::vector<std::pair<std::vector<std::string>, std::string>> categories = {
            {{"coretemp"}, "CPU_PACKAGE_TEMP"},
            {{"k10temp", "zenpower"}, "CPU_PACKAGE_TEMP"},
            {{"nvme"}, "NVME_TEMP_"},
            {{"amdgpu"}, "AMDGPU_TEMP_"},
            {{"nvidia", "nouveau"}, "NVIDIA_GPU_TEMP_"},
            {{"radeon"}, "RADEON_GPU_TEMP_"},
            {{"i915"}, "INTEL_GPU_TEMP_"},
            {{"kraken", "corsair", "nzxt", "aquacomputer", "liquidctl"}, "AIO_COOLANT_TEMP_"},
            {{"spd5118", "jc42", "ts3000"}, "MEMORY_TEMP_"},
            {{"r8169", "e1000", "igb", "ixgbe", "mlx"}, "NETWORK_TEMP_"},
            {{"nct", "it87", "w83", "f71", "asus", "gigabyte", "msi"}, "MOTHERBOARD_TEMP_"},
            {{"acpi", "thermal_zone"}, "ACPI_THERMAL_TEMP_"},
            {{"seasonic", "evga"}, "PSU_TEMP_"},
            {{"usb", "thunderbolt", "typec"}, "USB_CONTROLLER_TEMP_"}};

        for (auto &cat : categories)
        {
            if (match_any(hwmon_name, cat.first))
            {
                // If the mapping is complete name, don't append number
                if (cat.second.find("_TEMP_") != std::string::npos)
                    return cat.second + temp_num_str;
                return cat.second; // For fixed-name categories
            }
        }

        // Generic fallback
        return hwmon_name + "_TEMP_" + temp_num_str;
    }

    std::unordered_map<std::string, std::string> TemperatureProfiler::discover_hwmon_sensors()
    {
        std::unordered_map<std::string, std::string> sensors;

        const std::string base_path = "/sys/class/hwmon/";
        if (!optkit::utils::is_path_exists(base_path))
        {
            std::cerr << "[WARN] hwmon directory not found: " << base_path << "\n";
            return sensors;
        }

        DIR *hwmon_dir = opendir(base_path.c_str());
        if (!hwmon_dir)
        {
            std::cerr << "[WARN] Failed to open " << base_path << "\n";
            return sensors;
        }

        struct dirent *entry;
        while ((entry = readdir(hwmon_dir)) != nullptr)
        {
            if (entry->d_name[0] == '.')
                continue;

            std::string hwmon_path = base_path + entry->d_name;

            struct stat st;
            if (stat(hwmon_path.c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
                continue;

            std::string name_path = hwmon_path + "/name";
            if (!optkit::utils::is_path_exists(name_path))
                continue;

            std::string hwmon_name;
            try
            {
                hwmon_name = optkit::utils::read_file(name_path);
                hwmon_name.erase(hwmon_name.find_last_not_of(" \n\r\t") + 1);
            }
            catch (...)
            {
                continue;
            }

            DIR *temp_dir = opendir(hwmon_path.c_str());
            if (!temp_dir)
                continue;

            struct dirent *temp_entry;
            while ((temp_entry = readdir(temp_dir)) != nullptr)
            {
                std::string filename = temp_entry->d_name;
                if (filename.size() >= 11 &&
                    filename.substr(0, 4) == "temp" &&
                    filename.substr(filename.size() - 6) == "_input")
                {

                    std::string full_temp_path = hwmon_path + "/" + filename;
                    if (!optkit::utils::is_path_exists(full_temp_path))
                        continue;

                    std::string temp_num_str = filename.substr(4, 1);
                    std::string sensor_name = build_sensor_name(hwmon_name, temp_num_str);

                    // Handle duplicates
                    if (sensors.count(sensor_name))
                    {
                        std::string hwmon_num = std::string(entry->d_name).substr(5);
                        sensor_name += "_" + hwmon_num;
                    }

                    sensors[sensor_name] = full_temp_path;
                }
            }
            closedir(temp_dir);
        }
        closedir(hwmon_dir);

        for (auto &&i : sensors)
        {
            std::cout << i.first << " -> " << i.second << std::endl;
        }

        exit(0);

        return sensors;
    }

    uint64_t TemperatureProfiler::read_hwmon_temperature(const std::string &hwmon_path)
    {
        try
        {
            std::string content = optkit::utils::read_file(hwmon_path);
            return std::stoull(content); // hwmon returns millidegrees Celsius
        }
        catch (...)
        {
            return 0;
        }
    }

    std::unordered_map<std::string, uint64_t> TemperatureProfiler::read_selected_temperature_sensors(const std::vector<std::string> &sensor_names)
    {
        std::unordered_map<std::string, uint64_t> results;

        for (const auto &sensor_name : sensor_names)
        {
            uint64_t temperature = 0;

            auto it = sensor_paths.find(sensor_name);
            if (it != sensor_paths.end())
            {
                // Read from hwmon
                temperature = read_hwmon_temperature(it->second);
            }
            else if (sensor_name.find("GPU") != std::string::npos)
            {
                // Read GPU temperature via API (placeholder)
                temperature = read_gpu_temperature();
            }

            results[sensor_name] = temperature;
        }

        return results;
    }

    uint64_t TemperatureProfiler::read_gpu_temperature()
    {
        // TODO: Implement GPU temperature reading via NVML/ROCm
        // For now, try to find GPU in hwmon
        try
        {
            // Look for GPU-related hwmon sensors
            for (const auto &sensor_pair : sensor_paths)
            {
                if (sensor_pair.first.find("GPU") != std::string::npos)
                {
                    return read_hwmon_temperature(sensor_pair.second);
                }
            }
        }
        catch (...)
        {
        }

        return 0;
    }
}