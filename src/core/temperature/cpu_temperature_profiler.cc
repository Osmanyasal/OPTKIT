#include "core/temperature/cpu_temperature_profiler.hh"
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>

namespace optkit::core::temperature
{
    // Static member definition
    std::unordered_map<std::string, std::string> CPUTemperatureProfiler::sensor_paths;

    void CPUTemperatureProfiler::init()
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

    CPUTemperatureProfiler::CPUTemperatureProfiler(const char *block_name, const core::metrics::MetricBuilder<int64_t> &mb, bool verbose)
        : BaseProfiler(block_name, "temperature", verbose), metric_builder(mb)
    {
        // Take initial snapshot
        last_snapshot = read_selected_temperature_sensors(metric_builder.event_names());
        std::cout << "READ INITIAL SNAPSHOT: " << std::endl;
        // traverse snapshot and print
        for (const auto &i : last_snapshot)
            std::cout << i.first << " -> " << i.second << std::endl;
    }

    CPUTemperatureProfiler::~CPUTemperatureProfiler()
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

    std::vector<int64_t> CPUTemperatureProfiler::read()
    {

        const std::vector<std::string> &event_names = this->metric_builder.event_names();
        auto current_snapshot = read_selected_temperature_sensors(event_names);
        std::vector<int64_t> current_temps;

        for (const auto &cs : current_snapshot)
        {
            std::cout << "Current snapshot: " << cs.first << " -> " << cs.second << std::endl;
            int64_t curr_val = current_snapshot.at(cs.first);
            int64_t prev_val = last_snapshot.at(cs.first);

            int64_t delta = curr_val - prev_val;
            last_snapshot.at(cs.first) = curr_val;
            current_temps.push_back(delta); // store the delta
        }

        return current_temps;
    }

    std::unordered_map<std::string, int64_t> CPUTemperatureProfiler::aggregate()
    {
        double total_duration = 0.0;
        std::unordered_map<std::string, int64_t> aggregated_events;
        const std::vector<std::string> &event_names = this->metric_builder.event_names();

        for (const auto &entry : read_buffer)
        {
            total_duration += entry.first;

            const std::vector<int64_t> &values = entry.second;

            for (size_t j = 0; j < values.size(); ++j)
            {
                aggregated_events[event_names[j]] += values[j];
            }
        }
        std::vector<std::pair<std::string, int64_t>> event_value(
            aggregated_events.begin(), aggregated_events.end());

        this->event_results = event_value;
        this->total_duration_ms = total_duration;

        return aggregated_events;
    }

    std::string CPUTemperatureProfiler::to_json()
    {
        std::stringstream ss;
        ss << "[\n";
        ss << utils::to_json<int64_t>(this->total_duration_ms, this->measurement_type, this->event_results, this->metric_results);
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

    static std::string get_temp_label_or_num(const std::string &hwmon_path, const std::string &temp_num_str)
    {
        std::string label_path = hwmon_path + "/temp" + temp_num_str + "_label";
        if (optkit::utils::is_path_exists(label_path))
        {
            try
            {
                std::string label = optkit::utils::read_file(label_path);
                label.erase(label.find_last_not_of(" \n\r\t") + 1);
                if (!label.empty())
                {
                    // Replace spaces with underscores for consistency
                    std::replace(label.begin(), label.end(), ' ', '_');
                    return label;
                }
            }
            catch (...)
            {
                // ignore and fall back
            }
        }
        return "TEMP_" + temp_num_str;
    }

    std::string CPUTemperatureProfiler::build_sensor_name(const std::string &hwmon_name,
                                                          const std::string &temp_num_str)
    {
        // Group patterns
        static const std::vector<std::pair<std::vector<std::string>, std::string>> categories = {
            // CPU package / CPU sensors
            {{"coretemp", "k10temp", "zenpower", "k8temp", "peci-cputemp", "fam15h_power", "amd_energy", "intel_powerclamp"}, "CPU_PACKAGE_TEMP_"},

            // GPUs
            {{"amdgpu", "radeon", "nvidia", "nouveau", "i915", "intel_xe", "intel_xe_gpu"}, "GPU_TEMP_"},

            // NVMe / SSD / block device temps
            {{"nvme", "nvme_hwmon", "drivetemp", "ssd"}, "STORAGE_NVME_TEMP_"},
            {{"drivetemp", "hddtemp", "ata-temp"}, "STORAGE_DRIVE_TEMP_"},

            // Motherboard / SuperIO chips (motherboard sensors & fans)
            {{"nct6775", "nct6683", "nct7363", "nct7802", "nct7904", "it87", "w83627ehf", "w83627hf",
              "f71805f", "f71882fg", "vt1211", "via686a", "pc87360", "pc87427", "adm1025", "adm1031"},
             "MOTHERBOARD_TEMP_"},

            // AIO / pump / vendor water-cooling drivers & controllers
            {{"nzxt-kraken2", "nzxt-kraken3", "nzxt-smart2", "kraken", "gigabyte_waterforce", "corsair", "cm-psu", "gigabyte_waterforce"}, "AIO_COOLANT_TEMP_"},

            // Memory / SPD / DIMM sensors
            {{"spd5118", "jc42", "ts3000", "peci-dimmtemp"}, "MEMORY_TEMP_"},

            // Network adapters (rare, but sometimes NIC controllers expose temps)
            {{"r8169", "e1000", "igb", "ixgbe", "mlx", "mlxreg-fan", "lan966x-hwmon"}, "NETWORK_TEMP_"},

            // Power supplies / PSU / Board power monitors
            {{"seasonic", "evga", "ibm-cffps", "ibm-cffps", "power_meter", "pm6764tr"}, "PSU_TEMP_"},

            // ACPI / thermal zones
            {{"acpitz", "acpi_thermal", "thermal_zone", "acpi"}, "ACPI_THERMAL_TEMP_"},

            // USB / Type-C / PD controllers and power-management ICs
            {{"usb", "thunderbolt", "typec", "tps40422", "tps25990", "tps53679", "tps23861"}, "USB_CONTROLLER_TEMP_"},

            // BMC / server-baseboard sensors
            {{"ibmaem", "menf21bmc_hwmon", "intel-m10-bmc-hwmon", "ibm-cffps", "ipmi"}, "BMC_TEMP_"},

            // Fans / PWM controllers
            {{"pwm-fan", "mlxreg-fan", "surface_fan", "gxp-fan-ctrl", "pwm_fan", "mps"}, "FAN_RPM_"},

            // Battery / UPS related
            {{"kbatt", "battery", "ups"}, "BATTERY_TEMP_"},

            // Generic I2C / SPI temperature chips (lots of lm_* , tmp*, ltc*, max* chips)
            {{"lm75", "lm63", "lm70", "lm73", "lm77", "lm78", "lm80", "lm83", "lm85", "lm87", "lm90", "lm92", "lm93",
              "tmp102", "tmp103", "tmp108", "tmp401", "tmp421", "ltc2992", "ltc2947", "max31722", "max31790", "ina2xx",
              "ina3221", "isl28022", "isl68137", "adt7410", "ads7828", "emc2103", "emc2305", "mpq8785"},
             "GENERIC_I2C_TEMP_"},

            // Misc / SoC / platform-specific hwmon
            {{"raspberrypi-hwmon", "vexpress", "occ-hwmon", "surface_fan", "ampere-smpro", "intel_soc_dts_gen"}, "SOC_PLATFORM_TEMP_"},

            // GPU/accelerator (vendor specific) or unknown vendor hwmon nodes
            {{"nv_host", "nvidia_gpu", "gpu_temp"}, "ACCELERATOR_TEMP_"},

            // Fallback / unknown - keep this last so it's used only if nothing else matches.
            {{"composite", "unknown", "hwmon", "composite:0"}, "UNKNOWN_HWMON_"}};

        for (auto &cat : categories)
        {
            if (match_any(hwmon_name, cat.first))
            {
                return cat.second + temp_num_str;
            }
        }

        // Generic fallback
        return hwmon_name + "_TEMP_" + temp_num_str;
    }

    std::unordered_map<std::string, std::string> CPUTemperatureProfiler::discover_hwmon_sensors()
    {
        std::unordered_map<std::string, std::string> sensors;

        const std::string base_path = "/sys/class/hwmon/";
        if (!optkit::utils::is_path_exists(base_path))
        {
            OPTKIT_CORE_WARN("hwmon directory not found: {}", base_path);
            return sensors;
        }

        // Get all hwmon directories directly
        std::vector<std::string> hwmon_dirs = optkit::utils::get_all_files(base_path);

        for (const std::string &hwmon_dir : hwmon_dirs)
        {
            // Skip non-hwmon directories (only process hwmonX)
            if (hwmon_dir.size() < 6 || hwmon_dir.substr(0, 5) != "hwmon")
                continue;

            std::string hwmon_path = base_path + hwmon_dir;

            // Read hwmon device name
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

            // Get all files in this hwmon directory
            // Iterate through temperature sensors (temp1_input, temp2_input, ...) until none found
            std::vector<std::string> files = optkit::utils::get_all_files(hwmon_path);

            for (const std::string &filename : files)
            {
                // Check if it's a temperature input file (tempX_input)
                if (filename.size() >= 11 &&
                    filename.substr(0, 4) == "temp" &&
                    filename.substr(filename.size() - 6) == "_input")
                {
                    std::string full_temp_path = hwmon_path + "/" + filename;

                    // Extract the number between "temp" and "_input"
                    std::string temp_num_str = filename.substr(4, filename.size() - 10); // handles temp10+
                    std::string label_or_num = get_temp_label_or_num(hwmon_path, temp_num_str);
                    std::string sensor_name = build_sensor_name(hwmon_name, label_or_num);

                    // Handle duplicates by appending hwmon directory name
                    if (sensors.count(sensor_name))
                    {
                        // Extract hwmon number (hwmon2 -> 2)
                        std::string hwmon_num = hwmon_dir.substr(5); // Remove "hwmon" prefix
                        sensor_name += "_" + hwmon_num;
                    }

                    sensors[sensor_name] = full_temp_path;
                }
            }
        }

        // Debug output (remove for production)
        for (auto &&i : sensors)
        {
            std::cout << i.first << " -> " << i.second << std::endl;
        }

        return sensors;
    }

    int64_t CPUTemperatureProfiler::read_hwmon_temperature(const std::string &hwmon_path)
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

    std::unordered_map<std::string, int64_t> CPUTemperatureProfiler::read_selected_temperature_sensors(const std::vector<std::string> &sensor_names)
    {
        std::unordered_map<std::string, int64_t> results;

        for (auto it = sensor_paths.begin(); it != sensor_paths.end(); ++it)
        {
            const std::string &sensor_sysfs_name = it->first;

            for (std::vector<std::string>::const_iterator name_it = sensor_names.begin();
                 name_it != sensor_names.end(); ++name_it)
            {
                const std::string &name = *name_it;

                if (sensor_sysfs_name.size() >= name.size() &&
                    sensor_sysfs_name.compare(0, name.size(), name) == 0)
                {
                    results[sensor_sysfs_name] = read_hwmon_temperature(it->second);
                }
            }
        }

        return results;
    }
}