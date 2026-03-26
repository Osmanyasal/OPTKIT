#include "core/energy/cpu/hwmon/hwmon.hh"

namespace optkit::energy::hwmon
{
    const std::unordered_map<int32_t, std::string> hwmon_domain_name_mapping = {
        {static_cast<int32_t>(HwmonDomain::BEGIN), "begin"},
        {static_cast<int32_t>(HwmonDomain::CPU_POWER), "power-cpu"},
        {static_cast<int32_t>(HwmonDomain::MODULE_POWER), "power-module"},
        {static_cast<int32_t>(HwmonDomain::SYSIO_POWER), "power-sysio"},
        {static_cast<int32_t>(HwmonDomain::GRACE_POWER), "power-grace"},
        {static_cast<int32_t>(HwmonDomain::GPU_POWER), "power-gpu"},
        {static_cast<int32_t>(HwmonDomain::END), "end"},
        {static_cast<int32_t>(HwmonDomain::ALL), "All domains"}};

    const std::unordered_map<int32_t, std::string> hwmon_read_method_name_mapping = {
        {static_cast<int32_t>(HwmonReadMethods::SYSFS), "sysfs"}};

    HwmonDomain metric_name_to_hwmon_domain(const std::string &metric_name)
    {
        auto it = optkit::energy::hwmon::hwmon_domain_name_mapping.begin();
        while (it != optkit::energy::hwmon::hwmon_domain_name_mapping.end())
        {
            if (it->second == metric_name)
            {
                return static_cast<optkit::energy::hwmon::HwmonDomain>(it->first);
            }
            ++it;
        }

        OPTKIT_CORE_WARN("Unknown metric_name: {}", metric_name);
        return optkit::energy::hwmon::HwmonDomain::BEGIN;
    }

    std::string to_string(const optkit::energy::hwmon::HwmonDomain &domain)
    {
        std::ostringstream oss;
        oss << domain;
        return oss.str();
    }

    std::string to_string(const optkit::energy::hwmon::HwmonDomainInfo &domain_info)
    {
        std::ostringstream oss;
        oss << domain_info;
        return oss.str();
    }

    std::string to_string(const optkit::energy::hwmon::HwmonReadMethods &read_method)
    {
        std::ostringstream oss;
        oss << read_method;
        return oss.str();
    }

    // Overload << operator for HwmonDomain
    std::ostream &operator<<(std::ostream &os, const optkit::energy::hwmon::HwmonDomain &domain)
    {
        switch (domain)
        {
        case optkit::energy::hwmon::HwmonDomain::CPU_POWER:
            os << "power-cpu";
            break;
        case optkit::energy::hwmon::HwmonDomain::MODULE_POWER:
            os << "power-module";
            break;
        case optkit::energy::hwmon::HwmonDomain::SYSIO_POWER:
            os << "power-sysio";
            break;
        case optkit::energy::hwmon::HwmonDomain::GRACE_POWER:
            os << "power-grace";
            break;
        case optkit::energy::hwmon::HwmonDomain::GPU_POWER:
            os << "power-gpu";
            break;
        default:
            break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const optkit::energy::hwmon::HwmonDomainInfo &domain_info)
    {
        os << "Label=" << domain_info.label << ", "
           << "Path=" << domain_info.path << ", "
           << "Scale=" << std::scientific << std::setprecision(std::numeric_limits<double>::max_digits10) << domain_info.scale << ", "
           << "Units=" << domain_info.units << ", "
           << "SocketID=" << domain_info.socket_id;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const optkit::energy::hwmon::HwmonReadMethods &read_method)
    {
        switch (read_method)
        {
        case optkit::energy::hwmon::HwmonReadMethods::SYSFS:
            os << "sysfs";
            break;
        default:
            break;
        }
        return os;
    }

} // namespace optkit::energy::hwmon
