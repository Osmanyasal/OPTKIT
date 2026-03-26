#pragma once

#include <string>
#include <sstream>
#include "utils/json.hh"
#include "core/energy/cpu/hwmon/hwmon.hh"
#include "core/energy/cpu/hwmon/query.hh"

namespace optkit::energy::hwmon
{
    /**
     * @brief Convert HWMON readings to JSON format
     * 
     * @param event_name Name of the event/block being measured
     * @param hwmon_pair_list Vector of (duration, socket->domain->value) pairs
     * @return nlohmann::json JSON representation of the data
     */
    nlohmann::json to_json(const char *event_name, 
                          const std::vector<std::pair<double, std::map<int32_t, std::map<optkit::energy::hwmon::HwmonDomain, double>>>> &hwmon_pair_list);

} // namespace optkit::energy::hwmon
