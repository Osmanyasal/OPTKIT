#include "core/pmu/cpu/pmu_utils.hh"

namespace optkit::core::pmu::cpu::perf
{
    
nlohmann::json to_json(double duration, const char *metric_name,
                       const std::vector<std::pair<std::string, uint64_t>> &results,
                       const std::vector<std::pair<std::string, double>>& metric_results)
{
    nlohmann::json result;

    nlohmann::json packageJson;
    packageJson["duration"] = duration;
    packageJson["duration_unit"] = "ms";
    packageJson["socket_number"] = -1;
    packageJson["type"] = metric_name;

    for (int32_t i = 0; i < results.size(); i++)
    {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(3) << static_cast<double>(results[i].second);

        packageJson["measurements"].push_back({
            {"type", "event"},
            {"name", results[i].first},
            {"value", ss.str()},
            {"units", "uint"},
        });
    }

    for (int32_t i = 0; i < metric_results.size(); i++)
    {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(3) << metric_results[i].second;

        packageJson["measurements"].push_back({
            {"type", "metric"},
            {"name", metric_results[i].first},
            {"value", ss.str()},
            {"units", "double"},
        });
    }

    result["readings"].push_back(packageJson);
    return result;
}
} // namespace optkit::core
