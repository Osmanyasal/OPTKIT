#include "core/energy/cpu/rapl/utils.hh"

namespace optkit::energy::rapl
{

    nlohmann::json to_json(const char *event_name, const std::vector<std::pair<double, std::unordered_map<int32_t, std::unordered_map<RaplDomain, double>>>> &rapl_pair_list)
    {
        const std::vector<RaplDomainInfo> &avail_domains = Query::rapl_domain_info();
        nlohmann::json result;

        for (const auto &rapl_pair : rapl_pair_list)
        {
            nlohmann::json packageJson;
            packageJson["event_name"] = event_name;
            packageJson["duration"] = rapl_pair.first;

            for (const auto &innerpair : rapl_pair.second)
            {
                packageJson["package_number"] = innerpair.first;
                packageJson["metrics_set"] = {}; // Reset metrics_set array
                for (const auto &domain_value : innerpair.second)
                {
                    for (const auto &info : avail_domains)
                    {
                        if (info.domain == domain_value.first)
                        {
                            packageJson["metrics_set"].push_back({{"metric_name", info.event},
                                                                  {"value", domain_value.second},
                                                                  {"units", info.units},
                                                                  {"description", "Consumed"}});
                        }
                    }
                }
                result["readings"].push_back(packageJson);
            }
        }
        return result;
    }
} // namespace optkit
