
#include "core/profiling/rapl/query_rapl.hh"

namespace optkit::core::rapl
{

    int32_t QueryRapl::avail_rapl_read_methods()
    {
        int32_t result = 0;

        if (is_rapl_powercap_avail())
            result = result | (int32_t)rapl::RaplReadMethods::POWERCAP;

        if (is_rapl_perf_avail())
            result = result | (int32_t)rapl::RaplReadMethods::PERF;

        if (is_rapl_msr_avail())
            result = result | (int32_t)rapl::RaplReadMethods::MSR;

        return result;
    }

    bool QueryRapl::is_rapl_msr_avail()
    {
        OPTKIT_CORE_WARN("MSR avail check not implemented in this version!");
        return false;
    }

    bool QueryRapl::is_rapl_perf_avail()
    {
        if (is_path_exists("/sys/bus/event_source/devices/power/type"))
            return true;
        else
        {
            OPTKIT_CORE_WARN("No perf_event rapl support found (requires Linux 3.14).");
            return false;
        }
    }
    bool QueryRapl::is_rapl_powercap_avail()
    {
        if (is_path_exists("/sys/class/powercap/intel-rapl/intel-rapl:0/"))
            return true;
        else
        {
            OPTKIT_CORE_WARN("No powercap support found.");
            return false;
        }
    }

    const std::vector<rapl::RaplDomainInfo> &QueryRapl::rapl_domain_info()
    {
        static std::vector<rapl::RaplDomainInfo> res;

        if (res.size() == 0)
        {

            for (int32_t domain = static_cast<int32_t>(rapl::RaplDomain::PP0); domain < static_cast<int32_t>(rapl::RaplDomain::END); domain = domain << 1)
            {
                std::string domain_name = rapl::rapl_domain_name_mapping.at(domain);
                try
                {
                    std::string config = read_file("/sys/bus/event_source/devices/power/events/" + domain_name);
                    std::string scale = read_file("/sys/bus/event_source/devices/power/events/" + domain_name + ".scale");
                    std::string units = read_file("/sys/bus/event_source/devices/power/events/" + domain_name + ".unit");

                    config.erase(std::remove(config.begin(), config.end(), '\n'), config.end());
                    scale.erase(std::remove(scale.begin(), scale.end(), '\n'), scale.end());
                    units.erase(std::remove(units.begin(), units.end(), '\n'), units.end());

                    config = config.substr(config.find("=") + 1);
                    std::stringstream ss;
                    ss << std::hex << config;
                    uint64_t l_conf;
                    ss >> l_conf;

                    res.push_back({(rapl::RaplDomain)domain, domain_name, l_conf, std::stod(scale), units});
                }
                catch (const std::exception &e)
                {
                    // OPTKIT_CORE_ERROR("{}", e.what());
                    // res.push_back({(rapl::RaplDomain)domain, domain_name, 0, 0, "Joules"});
                }
            }
        }

        return res;
    }

} // namespace optkit::core
