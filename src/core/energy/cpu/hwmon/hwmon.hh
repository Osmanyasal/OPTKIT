#pragma once

#include <iostream>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <limits>
#include "utils/logging/logger.hh"

namespace optkit::energy::hwmon
{
    /**
     * @brief HWMON power domains for Grace and other ARM-based systems
     * These correspond to power sensors available via /sys/class/hwmon
     */
    enum class HwmonDomain
    {
        BEGIN = 0,

        CPU_POWER = (1 << 0),    // CPU Power per socket
        MODULE_POWER = (1 << 1), // Module Power (Grace-Hopper: Grace + GPU)
        SYSIO_POWER = (1 << 2),  // System I/O Power
        GRACE_POWER = (1 << 3),  // Grace CPU Power (Grace-Grace systems)
        GPU_POWER = (1 << 4),    // GPU Power (derived: Module - Grace)

        END = (1 << 5),

        ALL = 0x1F, // All domains
    };

    extern const std::unordered_map<int32_t, std::string> hwmon_domain_name_mapping;

    HwmonDomain metric_name_to_hwmon_domain(const std::string &metric_name);

    /**
     * @brief HWMON Read Methods
     */
    enum class HwmonReadMethods
    {
        SYSFS = (1 << 0), // Read via /sys/class/hwmon
    };

    extern const std::unordered_map<int32_t, std::string> hwmon_read_method_name_mapping;

    struct HwmonDomainInfo
    {
        HwmonDomain domain;
        std::string label;       // e.g., "CPU Power Socket", "Grace Power Socket"
        std::string path;        // e.g., "/sys/class/hwmon/hwmon3/device/power1_average"
        double scale;            // Conversion factor (typically 1e-6 for uW to W)
        std::string units;       // "Watts"
        uint64_t sample_period;  // Sampling period in nanoseconds
        int socket_id;           // Socket/package identifier
    };

    std::string to_string(const optkit::energy::hwmon::HwmonDomain &domain);
    std::string to_string(const optkit::energy::hwmon::HwmonDomainInfo &domain_info);
    std::string to_string(const optkit::energy::hwmon::HwmonReadMethods &read_method);

    std::ostream &operator<<(std::ostream &os, const optkit::energy::hwmon::HwmonDomain &domain);
    std::ostream &operator<<(std::ostream &os, const optkit::energy::hwmon::HwmonDomainInfo &domain_info);
    std::ostream &operator<<(std::ostream &os, const optkit::energy::hwmon::HwmonReadMethods &read_method);

} // namespace optkit::energy::hwmon
