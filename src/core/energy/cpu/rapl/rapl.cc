#include "core/energy/cpu/rapl/rapl.hh"

namespace optkit::energy::rapl
{
    const std::unordered_map<int32_t, std::string> rapl_domain_name_mapping = {
        {static_cast<int32_t>(RaplDomain::BEGIN), "begin"},
        {static_cast<int32_t>(RaplDomain::PP0), "energy-cores"},
        {static_cast<int32_t>(RaplDomain::PP1), "energy-gpu"},
        {static_cast<int32_t>(RaplDomain::PACKAGE), "energy-pkg"},
        {static_cast<int32_t>(RaplDomain::PSYS), "energy-psys"},
        {static_cast<int32_t>(RaplDomain::DRAM), "energy-dram"},
        {static_cast<int32_t>(RaplDomain::END), "end"},
        {static_cast<int32_t>(RaplDomain::ALL), "All domains"}};

    const std::unordered_map<int32_t, std::string> rapl_read_method_name_mapping = {
        {static_cast<int32_t>(RaplReadMethods::PERF), "perf"},
        {static_cast<int32_t>(RaplReadMethods::MSR), "msr"},
        {static_cast<int32_t>(RaplReadMethods::SYSFS), "sysfs"}};

    RaplDomain metric_name_to_rapl_domain(const std::string &metric_name)
    {
        // Implement the mapping logic based on the rapl_domain_name_mapping
        auto it = optkit::energy::rapl::rapl_domain_name_mapping.begin();
        while (it != optkit::energy::rapl::rapl_domain_name_mapping.end())
        {
            if (it->second == metric_name)
            {
                return static_cast<optkit::energy::rapl::RaplDomain>(it->first);
            }
            ++it;
        }

        OPTKIT_CORE_WARN("Unknown metric_name: {}", metric_name);
        // Return a default or handle the error accordingly
        return optkit::energy::rapl::RaplDomain::BEGIN;
    }

    std::string to_string(const optkit::energy::rapl::RaplDomain &domain)
    {
        std::ostringstream oss;
        oss << domain;
        return oss.str();
    }
    std::string to_string(const optkit::energy::rapl::RaplDomainInfo &domain_info)
    {
        std::ostringstream oss;
        oss << domain_info;
        return oss.str();
    }
    std::string to_string(const optkit::energy::rapl::RaplReadMethods &read_method)
    {
        std::ostringstream oss;
        oss << read_method;
        return oss.str();
    }

    // Overload << operator for RaplDomain
    std::ostream &operator<<(std::ostream &os, const optkit::energy::rapl::RaplDomain &domain)
    {
        switch (domain)
        {
        case optkit::energy::rapl::RaplDomain::PP0:
            os << "energy-cores";
            break;
        case optkit::energy::rapl::RaplDomain::PP1:
            os << "energy-gpu";
            break;
        case optkit::energy::rapl::RaplDomain::PACKAGE:
            os << "energy-pkg";
            break;
        case optkit::energy::rapl::RaplDomain::PSYS:
            os << "energy-psys";
            break;
        case optkit::energy::rapl::RaplDomain::DRAM:
            os << "energy-dram";
            break;
        default:
            break;
        }

        return os;
    }

    std::ostream &operator<<(std::ostream &os, const optkit::energy::rapl::RaplDomainInfo &domain_info)
    {
        os << "Event=" << domain_info.event << ", "
           << "Config=" << std::hex << "0x" << domain_info.config << ", "
           << "scale=" << std::dec << std::fixed << domain_info.scale << ", "
           << "units=" << domain_info.units;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const optkit::energy::rapl::RaplReadMethods &read_method)
    {
        switch (read_method)
        {
        case optkit::energy::rapl::RaplReadMethods::PERF:
            os << "perf";
            break;
        case optkit::energy::rapl::RaplReadMethods::MSR:
            os << "msr";
            break;
        case optkit::energy::rapl::RaplReadMethods::SYSFS:
            os << "sysfs";
            break;
        default:
            break;
        }
        return os;
    }

} // namespace optkit::energy::rapl