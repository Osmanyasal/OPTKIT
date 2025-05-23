#pragma once

#include <string>
#include <sstream>
#include "utils/json.hh"
#include "core/energy/cpu/rapl/rapl.hh"
#include "core/energy/cpu/rapl/query_rapl.hh"

namespace optkit::core::energy::rapl
{                                                  // vector<duration, map<package_num, map<rapl_domain, value>>>
    nlohmann::json to_json(const char *event_name, const std::vector<std::pair<double, std::map<int32_t, std::map<optkit::core::energy::rapl::RaplDomain, double>>>> &rapl_pair_list);
    std::map<uint32_t, std::vector<std::pair<double, std::map<int32_t, std::map<optkit::core::energy::rapl::RaplDomain, double>>>>> from_json(const std::string &json);

} // namespace optkit::core
