#include "core/pmu/cpu/pmu_utils.hh"

namespace optkit::core::pmu::cpu::perf
{
    nlohmann::json to_json(const char *event_name, const std::vector<std::pair<uint64_t, std::string>> &raw_events, const std::vector<std::pair<double, std::vector<uint64_t>>> &pmu_pair_list)
    {
        nlohmann::json result;
        for (const auto &pmu_pair : pmu_pair_list)
        {
            nlohmann::json packageJson;
            packageJson["duration"] = pmu_pair.first;
            packageJson["package_number"] = -1; // TODO::Make this package specific -1 means all the sockets and cores that the program uses.
            packageJson["event_name"] = event_name;

            int32_t i = 0;
            for (const auto &values : pmu_pair.second)
            {
                packageJson["metrics_set"].push_back({{"metric_name", raw_events.at(i++).second},
                                                      {"value", values},
                                                      {"units", "int"},
                                                      {"description", "Counted"}});
            }
            result["readings"].push_back(packageJson);
        }
        return result;
    }
} // namespace optkit::core
