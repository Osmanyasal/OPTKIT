
#include "core/energy/cpu/rapl/query_rapl.hh"

namespace optkit::energy::rapl
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
        if (optkit::utils::is_path_exists("/sys/bus/event_source/devices/power/type"))
            return true;
        else
        {
            OPTKIT_CORE_WARN("No perf_event rapl support found (requires Linux 3.14).");
            return false;
        }
    }
    bool QueryRapl::is_rapl_powercap_avail()
    {
        if (optkit::utils::is_path_exists("/sys/class/powercap/intel-rapl/intel-rapl:0/"))
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

        if (res.empty())
        {
            res.reserve(optkit::utils::count_trailing_zeros(static_cast<int>(rapl::RaplDomain::END))); // max possible domains

            for (int32_t domain = static_cast<int32_t>(rapl::RaplDomain::PP0);
                 domain < static_cast<int32_t>(rapl::RaplDomain::END);
                 domain <<= 1)
            {
                const std::string &domain_name = rapl::rapl_domain_name_mapping.at(domain);

                try
                {
                    std::string config = optkit::utils::read_file("/sys/bus/event_source/devices/power/events/" + domain_name);
                    std::string scale = optkit::utils::read_file("/sys/bus/event_source/devices/power/events/" + domain_name + ".scale");
                    std::string units = optkit::utils::read_file("/sys/bus/event_source/devices/power/events/" + domain_name + ".unit");

                    // strip trailing newline if present
                    if (!config.empty() && config.back() == '\n')
                        config.pop_back();
                    if (!scale.empty() && scale.back() == '\n')
                        scale.pop_back();
                    if (!units.empty() && units.back() == '\n')
                        units.pop_back();

                    // Parse config: "event=0x123"
                    std::size_t pos = config.find('=');
                    if (pos == std::string::npos)
                        continue; // skip malformed line

                    uint64_t l_conf = std::stoull(config.substr(pos + 1), 0, 16);

                    res.push_back(rapl::RaplDomainInfo{
                        static_cast<rapl::RaplDomain>(domain),
                        domain_name, // copy is fine (small string)
                        l_conf,
                        std::stod(scale),
                        units});
                }
                catch (const std::exception &)
                {
                    // silently skip unavailable domains
                    // could log here if needed
                    std::cerr << "Failed to read RAPL domain info for " << domain_name << "\n";
                }
            }
        }

        return res;
    }

} // namespace optkit
