#include "core/profiling/rapl/rapl_utils.hh"

namespace optkit::core::pmu
{
    nlohmann::json to_json(const char *event_name, const std::vector<std::pair<uint64_t, std::string>> &raw_events, const std::vector<std::pair<double, std::vector<uint64_t>>> &pmu_pair_list)
    {
        nlohmann::json result;
        for (const auto &pmu_pair : pmu_pair_list)
        {
            nlohmann::json packageJson;
            packageJson["duration"] = pmu_pair.first;
            packageJson["package_number"] = -1; // TODO::Make this package specific -1 means all of them IG?
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

    // TODO: Fix this reading according to return parameter.
    std::vector<std::pair<double, uint64_t>> from_json(const std::string &json)
    {
        const auto json_obj = nlohmann::json::parse(json);
        std::vector<std::pair<double, uint64_t>> result;
        // std::map<int32_t, uint64_t> rapl_map;

        // if (json_obj.is_array())
        // {
        //     for (const auto &array_elem : json_obj)
        //     {
        //         for (const auto &packageJson : array_elem["readings"])
        //         {
        //             int32_t duration = packageJson["duration"];
        //             int32_t package_number = packageJson["package_number"];
        //             std::map<optkit::core::RaplDomain, double> inner_map;

        //             for (const auto &metricJson : packageJson["metrics_set"])
        //             {
        //                 std::string metric_name = metricJson["metric_name"];
        //                 double value = metricJson["value"];
        //                 std::string units = metricJson["units"];
        //                 std::string description = metricJson["description"];
        //                 optkit::core::RaplDomain domain = mapMetricNameToRaplDomain(metric_name);

        //                 inner_map[domain] = value;
        //             }
        //             rapl_map[package_number] = inner_map;
        //             result.push_back({duration, rapl_map});
        //         }
        //     }
        // }
        return result;
    }
} // namespace optkit::core
