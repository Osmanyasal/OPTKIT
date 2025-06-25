#pragma once

#include <string>
#include <sstream>
#include <iomanip>

#include "utils/json.hh"
namespace optkit::core::pmu::cpu::perf
{
    nlohmann::json to_json(double duration, const char *metric_name, const std::vector<std::pair<std::string, uint64_t>> &results, const std::vector<std::pair<std::string, double>>& metric_results);

} // namespace optkit::core
