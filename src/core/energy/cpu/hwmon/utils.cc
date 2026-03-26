#include "core/energy/cpu/hwmon/utils.hh"

namespace optkit::energy::hwmon
{
    nlohmann::json to_json(const char *event_name,
                          const std::vector<std::pair<double, std::map<int32_t, std::map<optkit::energy::hwmon::HwmonDomain, double>>>> &hwmon_pair_list)
    {
        nlohmann::json result;
        result["event_name"] = event_name;
        result["measurements"] = nlohmann::json::array();

        for (const auto &pair : hwmon_pair_list)
        {
            nlohmann::json measurement;
            measurement["duration_ms"] = pair.first;
            measurement["sockets"] = nlohmann::json::array();

            for (const auto &socket_pair : pair.second)
            {
                nlohmann::json socket_data;
                socket_data["socket_id"] = socket_pair.first;
                socket_data["domains"] = nlohmann::json::object();

                for (const auto &domain_pair : socket_pair.second)
                {
                    std::string domain_name = to_string(domain_pair.first);
                    socket_data["domains"][domain_name] = domain_pair.second;
                }

                measurement["sockets"].push_back(socket_data);
            }

            result["measurements"].push_back(measurement);
        }

        return result;
    }

} // namespace optkit::energy::hwmon
