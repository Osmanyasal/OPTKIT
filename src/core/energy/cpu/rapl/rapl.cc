#include "core/energy/cpu/rapl/rapl.hh"

namespace optkit::core::energy::rapl
{
    const std::unordered_map<int32_t, std::string> rapl_domain_name_mapping = {
        {static_cast<int32_t>(RaplDomain::BEGIN), "begin"},
        {static_cast<int32_t>(RaplDomain::PP0), "energy-cores"},
        {static_cast<int32_t>(RaplDomain::PP1), "energy-gpu"},
        {static_cast<int32_t>(RaplDomain::PACKAGE), "energy-pkg"},
        {static_cast<int32_t>(RaplDomain::PSYS), "energy-psys"},
        {static_cast<int32_t>(RaplDomain::DRAM), "energy-ram"},
        {static_cast<int32_t>(RaplDomain::END), "end"},
        {static_cast<int32_t>(RaplDomain::ALL), "All domains"}};

    const std::unordered_map<int32_t, std::string> rapl_read_method_name_mapping = {
        {static_cast<int32_t>(RaplReadMethods::PERF), "PERF"},
        {static_cast<int32_t>(RaplReadMethods::MSR), "MSR"},
        {static_cast<int32_t>(RaplReadMethods::POWERCAP), "POWERCAP"}};

    RaplDomain mapMetricNameToRaplDomain(const std::string &metric_name)
    {
        // Implement the mapping logic based on the rapl_domain_name_mapping
        auto it = optkit::core::energy::rapl::rapl_domain_name_mapping.begin();
        while (it != optkit::core::energy::rapl::rapl_domain_name_mapping.end())
        {
            if (it->second == metric_name)
            {
                return static_cast<optkit::core::energy::rapl::RaplDomain>(it->first);
            }
            ++it;
        }

        OPTKIT_CORE_WARN("Unknown metric_name: {}", metric_name);
        // Return a default or handle the error accordingly
        return optkit::core::energy::rapl::RaplDomain::BEGIN;
    }

    std::string to_string(const optkit::core::energy::rapl::RaplDomain &domain)
    {
        switch (domain)
        {
        case RaplDomain::PP0:
            return "energy-cores";
        case RaplDomain::PP1:
            return "energy-gpu";
        case RaplDomain::PACKAGE:
            return "energy-pkg";
        case RaplDomain::PSYS:
            return "energy-psys";
        case RaplDomain::DRAM:
            return "energy-ram";
        default:
            return "unknown";
        }
    }
    std::string to_string(const optkit::core::energy::rapl::RaplDomainInfo &domain_info)
    {
        std::ostringstream stream;
        stream << std::scientific << domain_info.scale;
        std::string scale_scf = stream.str();

        std::ostringstream result;
        result << "Event=" << domain_info.event << ", "
               << "Config=" << domain_info.config << ", "
               << "scale=" << scale_scf << ", "
               << "units=" << domain_info.units;
        return result.str();
    }
    std::string to_string(const optkit::core::energy::rapl::RaplReadMethods &read_method)
    {
        switch (read_method)
        {
        case RaplReadMethods::PERF:
            return "Perf";
        case RaplReadMethods::MSR:
            return "MSR";
        case RaplReadMethods::POWERCAP:
            return "PowerCap";
        default:
            return "Unknown";
        }
    }

    // Overload << operator for RaplDomain
    std::ostream &operator<<(std::ostream &os, const optkit::core::energy::rapl::RaplDomain &domain)
    {
        switch (domain)
        {
        case optkit::core::energy::rapl::RaplDomain::PP0:
            os << "energy-cores";
            break;
        case optkit::core::energy::rapl::RaplDomain::PP1:
            os << "energy-gpu";
            break;
        case optkit::core::energy::rapl::RaplDomain::PACKAGE:
            os << "energy-pkg";
            break;
        case optkit::core::energy::rapl::RaplDomain::PSYS:
            os << "energy-psys";
            break;
        case optkit::core::energy::rapl::RaplDomain::DRAM:
            os << "energy-ram";
            break;
        default:
            break;
        }

        return os;
    }

    std::ostream &operator<<(std::ostream &os, const optkit::core::energy::rapl::RaplDomainInfo &domain_info)
    {
        std::ostringstream stream;
        stream << std::scientific << domain_info.scale;
        std::string scale_scf = stream.str();

        os << "Event=" << domain_info.event << ", "
           << "Config=" << domain_info.config << ", "
           << "scale=" << scale_scf << ", "
           << "units=" << domain_info.units;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const optkit::core::energy::rapl::RaplReadMethods &read_method)
    {

        switch (read_method)
        {
        case optkit::core::energy::rapl::RaplReadMethods::PERF:
            os << "Perf";
            break;
        case optkit::core::energy::rapl::RaplReadMethods::MSR:
            os << "MSR";
            break;
        case optkit::core::energy::rapl::RaplReadMethods::POWERCAP:
            os << "PowerCap";
            break;
        default:
            break;
        }
        return os;
    }

} // namespace optkit::core::energy::rapl