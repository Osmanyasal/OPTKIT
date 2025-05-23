#pragma once

#include <cstring>
#include "core/energy/cpu/rapl/rapl.hh" // for rapl monitoring

namespace optkit::core::energy::rapl
{
    struct RaplConfig
    {
        /**
         * @brief Value must be the combination of RaplDomains.
         *        e.g RaplDomain::DRAM | RaplDomain::Core... by default try to monitor ALL of them
         * @param monitor_domain
         */
        RaplConfig(rapl::RaplReadMethods read_method = rapl::RaplReadMethods::PERF, int32_t monitor_domain = (int32_t)rapl::RaplDomain::ALL, bool is_reset_after_read = true, bool dump_results_to_file = true);
        rapl::RaplReadMethods read_method;
        int32_t monitor_domain;
        bool is_reset_after_read;
        const bool dump_results_to_file;
    };

    std::ostream &operator<<(std::ostream &os, const optkit::core::energy::rapl::RaplConfig &rapl_config);

} // optkit::core::energy::rapl

using optkit::core::energy::rapl::operator<<; // make available to global namespace