#include "core/pmu/cpu/pmu_utils.hh"

namespace optkit::core::pmu::cpu::perf
{
    nlohmann::json to_json(double duration, const char *metric_name, const std::vector<std::pair<std::string, uint64_t>> &results, std::vector<std::pair<std::string, double>> metric_results)
    {
        nlohmann::json result;

        nlohmann::json packageJson;
        packageJson["duration"] = duration;
        packageJson["duration_unit"] = "ms";
        packageJson["socket_number"] = -1; // TODO::Make this package specific -1 means all the sockets and cores that the program uses. but this can be set by the user, sayin, execute on this socket, those cores etc.
        packageJson["type"] = metric_name;

        for (int32_t i = 0; i < results.size(); i++)
            packageJson["measurements"].push_back({
                {"type", "event"},
                {"name", results[i].first},
                {"value", results[i].second},
                {"units", "uint"},
            });

        for (int32_t i = 0; i < metric_results.size(); i++)
            packageJson["measurements"].push_back({
                {"type", "metric"},
                {"name", metric_results[i].first},
                {"value", metric_results[i].second},
                {"units", "double"},
            });

        result["readings"].push_back(packageJson);
        return result;
    }
    
    nlohmann::json to_json(const char *event_name, const std::vector<std::pair<std::string, uint64_t>> &raw_events, const std::vector<std::pair<double, std::vector<uint64_t>>> &pmu_pair_list)
    {
        nlohmann::json result;
        for (const auto &pmu_pair : pmu_pair_list)
        {
            nlohmann::json packageJson;
            packageJson["duration"] = pmu_pair.first;
            packageJson["duration_unit"] = "ms";
            packageJson["socket_number"] = -1; // TODO::Make this package specific -1 means all the sockets and cores that the program uses. but this can be set by the user, sayin, execute on this socket, those cores etc.
            packageJson["type"] = event_name;

            int32_t i = 0;
            for (const auto &values : pmu_pair.second)
            {
                packageJson["metrics_set"].push_back({
                    {"event_name", raw_events.at(i++).first},
                    {"value", values},
                    {"units", "uint"},
                    //   {"description", "Counted"}
                });
            }
            result["readings"].push_back(packageJson);
        }
        return result;
    }
} // namespace optkit::core
