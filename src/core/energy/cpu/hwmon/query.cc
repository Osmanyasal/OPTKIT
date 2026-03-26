#include "core/energy/cpu/hwmon/query.hh"
#include <dirent.h>
#include <regex>
#include <fstream>

namespace optkit::energy::hwmon
{
    int32_t Query::avail_hwmon_read_methods()
    {
        int32_t result = 0;

        if (is_hwmon_sysfs_avail())
            result = result | static_cast<int32_t>(hwmon::HwmonReadMethods::SYSFS);

        return result;
    }

    bool Query::is_hwmon_sysfs_avail()
    {
        if (optkit::utils::is_path_exists("/sys/class/hwmon"))
            return true;
        else
        {
            OPTKIT_CORE_WARN("No hwmon sysfs support found.");
            return false;
        }
    }

    const std::vector<hwmon::HwmonDomainInfo> &Query::hwmon_domain_info()
    {
        static std::vector<hwmon::HwmonDomainInfo> res;

        if (OPT_LIKELY(res.empty()))
        {
            const std::string hwmon_root = "/sys/class/hwmon";
            
            // Map label patterns to domains
            std::map<std::string, HwmonDomain> label_to_domain = {
                {"CPU Power Socket", HwmonDomain::CPU_POWER},
                {"Module Power Socket", HwmonDomain::MODULE_POWER},
                {"SysIO Power Socket", HwmonDomain::SYSIO_POWER},
                {"Grace Power Socket", HwmonDomain::GRACE_POWER},
                {"GPU Power Socket", HwmonDomain::GPU_POWER}
            };

            // Iterate through hwmon devices
            DIR *dir = opendir(hwmon_root.c_str());
            if (!dir)
            {
                OPTKIT_CORE_WARN("Failed to open {}", hwmon_root);
                return res;
            }

            struct dirent *entry;
            std::regex hwmon_regex("hwmon[0-9]+");

            while ((entry = readdir(dir)) != nullptr)
            {
                std::string hwmon_name = entry->d_name;
                if (!std::regex_match(hwmon_name, hwmon_regex))
                    continue;

                std::string hwmon_path = hwmon_root + "/" + hwmon_name;
                std::string device_path = hwmon_path + "/device";

                // Check if device path exists, if not try the hwmon path directly
                if (!optkit::utils::is_path_exists(device_path))
                    device_path = hwmon_path;

                // Iterate through power sensors in device
                DIR *device_dir = opendir(device_path.c_str());
                if (!device_dir)
                    continue;

                struct dirent *device_entry;
                std::regex power_regex("power([0-9]+)_average");

                while ((device_entry = readdir(device_dir)) != nullptr)
                {
                    std::string filename = device_entry->d_name;
                    std::smatch match;

                    if (!std::regex_match(filename, match, power_regex))
                        continue;

                    std::string sensor_num = match[1].str();
                    std::string power_path = device_path + "/" + filename;
                    std::string label_path = device_path + "/power" + sensor_num + "_oem_info";

                    // Read the label to identify the domain
                    std::string label;
                    try
                    {
                        label = optkit::utils::read_file(label_path);
                        // Remove trailing newline
                        if (!label.empty() && label.back() == '\n')
                            label.pop_back();
                    }
                    catch (...)
                    {
                        continue; // Skip if we can't read the label
                    }

                    // Parse label to get base label and socket ID
                    // Format: "CPU Power Socket 0", "Grace Power Socket 1", etc.
                    std::regex label_regex("^(.+)\\s+([0-9]+)$");
                    std::smatch label_match;

                    if (!std::regex_match(label, label_match, label_regex))
                        continue;

                    std::string base_label = label_match[1].str();
                    int socket_id = std::stoi(label_match[2].str());

                    // Find matching domain
                    auto domain_it = label_to_domain.find(base_label);
                    if (domain_it == label_to_domain.end())
                        continue;

                    // Read sampling interval if available
                    uint64_t sample_period = 100000000; // Default 100ms in nanoseconds
                    std::string interval_path = device_path + "/power" + sensor_num + "_average_interval";
                    try
                    {
                        std::string interval_str = optkit::utils::read_file(interval_path);
                        int interval_ms = std::stoi(interval_str);
                        sample_period = interval_ms * 1000000ull; // Convert ms to ns
                    }
                    catch (...)
                    {
                        // Use default if can't read interval
                    }

                    // Add to result
                    res.push_back(hwmon::HwmonDomainInfo{
                        domain_it->second,
                        label,
                        power_path,
                        1.0e-6, // Scale from microwatts to watts
                        "Watts",
                        sample_period,
                        socket_id
                    });
                }

                closedir(device_dir);
            }

            closedir(dir);

            if (res.empty())
            {
                OPTKIT_CORE_WARN("No HWMON power sensors found in {}", hwmon_root);
            }
        }

        return res;
    }

} // namespace optkit::energy::hwmon
