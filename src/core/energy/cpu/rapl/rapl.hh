#pragma once

#include <iostream>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include "utils/logging/logger.hh"
#include <limits>

namespace optkit::energy::rapl
{
    /**
     * @brief All available rapl domain by 2023<br>
     * check intel_rapl and its AMD implementation for more detail.
     *
     */
    enum class RaplDomain
    {
        BEGIN = 0,

        PP0 = (1 << 0),     // CORES
        PP1 = (1 << 1),     // INTEGRATED GPU
        PACKAGE = (1 << 2), // PP0 + PP1 + SYSTEM AGENT + LAST_LEVEL_CACHE MEMORY CONTROLLER
        PSYS = (1 << 3),    // PACKAGE + eDRAM + PCH
        DRAM = (1 << 4),    // DRAM DIMM 0 and DRAM DIMM 1

        END = (1 << 5),

        ALL = 0x1F, // All domains
    };
    extern const std::unordered_map<int32_t, std::string> rapl_domain_name_mapping;

    RaplDomain metric_name_to_rapl_domain(const std::string &metric_name);

    /**
     * @brief Rapl Read Methods
     *
     */
    enum class RaplReadMethods
    {
        PERF = (1 << 0),
        MSR = (1 << 1),
        SYSFS = (1 << 2),
    };

    extern const std::unordered_map<int32_t, std::string> rapl_read_method_name_mapping;

    struct RaplDomainInfo
    {
        RaplDomain domain;
        std::string event;
        uint64_t config;
        double scale;
        std::string units;
    };

    std::string to_string(const optkit::energy::rapl::RaplDomain &domain);
    std::string to_string(const optkit::energy::rapl::RaplDomainInfo &domain_info);
    std::string to_string(const optkit::energy::rapl::RaplReadMethods &read_method);

    std::ostream &operator<<(std::ostream &os, const optkit::energy::rapl::RaplDomain &domain);
    std::ostream &operator<<(std::ostream &os, const optkit::energy::rapl::RaplDomainInfo &domain_info);
    std::ostream &operator<<(std::ostream &os, const optkit::energy::rapl::RaplReadMethods &read_method);

} // namespace optkit::energy::rapl
