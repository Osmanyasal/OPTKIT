
#include "core/energy/cpu/rapl/profiler_config.hh"

namespace optkit::core::energy::rapl
{
    RaplConfig::RaplConfig(rapl::RaplReadMethods read_method, int32_t monitor_domain, bool is_reset_after_read, bool dump_results_to_file) : read_method{read_method}, monitor_domain{monitor_domain}, is_reset_after_read{is_reset_after_read}, dump_results_to_file{dump_results_to_file}
    {
    }

    std::ostream &operator<<(std::ostream &os, const optkit::core::energy::rapl::RaplConfig &rapl_config)
    {

        os << "Rapl Config(Read Method - Monitor Domain):\n";
        os << "  " << optkit::core::energy::rapl::rapl_read_method_name_mapping.at((int32_t)rapl_config.read_method) << " ";

        if (rapl_config.monitor_domain & (int32_t)optkit::core::energy::rapl::RaplDomain::PP0)
        {
            os << "PP0 ";
        }
        if (rapl_config.monitor_domain & (int32_t)optkit::core::energy::rapl::RaplDomain::PP1)
        {
            os << "PP1 ";
        }
        if (rapl_config.monitor_domain & (int32_t)optkit::core::energy::rapl::RaplDomain::PACKAGE)
        {
            os << "PACKAGE ";
        }
        if (rapl_config.monitor_domain & (int32_t)optkit::core::energy::rapl::RaplDomain::PSYS)
        {
            os << "PSYS ";
        }
        if (rapl_config.monitor_domain & (int32_t)optkit::core::energy::rapl::RaplDomain::DRAM)
        {
            os << "DRAM ";
        }
        return os;
    }
} // namespace optkit::core::energy::rapl