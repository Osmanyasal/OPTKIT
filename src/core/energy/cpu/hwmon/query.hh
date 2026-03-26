#pragma once

#include <ostream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <unistd.h>

#include "core/energy/cpu/hwmon/hwmon.hh"
#include "utils/utils.hh"

namespace optkit::energy::hwmon
{
    /**
     * @brief Query CPU HWMON-related information here
     */
    class Query final
    {
    public:
        /**
         * @brief Returns available HWMON read methods in combination of HwmonReadMethods as bitwise OR.
         * @see HwmonReadMethods
         * @return int32_t
         */
        static int32_t avail_hwmon_read_methods();

        /**
         * @brief Check if HWMON sysfs interface is available
         * @return bool
         */
        static bool is_hwmon_sysfs_avail();

        /**
         * @brief Returns hwmon domain info in the system
         * 
         * This function scans /sys/class/hwmon for power sensors and collects
         * information about available power domains (CPU, Module, SysIO, Grace, GPU)
         * 
         * @return const ref of std::vector<HwmonDomainInfo> static object!
         */
        static const std::vector<hwmon::HwmonDomainInfo> &hwmon_domain_info();

    private:
        Query() = delete;
        ~Query() = delete;
    };

} // namespace optkit::energy::hwmon
