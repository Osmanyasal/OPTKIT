#pragma once

#include <string>
#include <sstream>

#include "utils/json.hh"
namespace optkit::core::pmu::cpu::perf
{
    nlohmann::json to_json(double duration, const char *metric_name, const std::vector<std::pair<std::string, uint64_t>> &results, std::vector<std::pair<std::string, double>> metric_results);
    nlohmann::json to_json(const char *event_name, const std::vector<std::pair<std::string, uint64_t>> &raw_events, const std::vector<std::pair<double, std::vector<uint64_t>>> &pmu_pair_list);

} // namespace optkit::core
