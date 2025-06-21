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
} // namespace optkit::core
