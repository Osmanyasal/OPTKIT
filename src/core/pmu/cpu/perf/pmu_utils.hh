#pragma once

#include <string>
#include <sstream>

#include "utils/json.hh"
#include "core/pmu/cpu/perf/query_pmu.hh"

namespace optkit::core::pmu::cpu::perf
{
    nlohmann::json to_json(const char *event_name, const std::vector<std::pair<uint64_t, std::string>> &raw_events, const std::vector<std::pair<double, std::vector<uint64_t>>> &pmu_pair_list);
    std::vector<std::pair<double, uint64_t>> from_json(const std::string &json);

} // namespace optkit::core
