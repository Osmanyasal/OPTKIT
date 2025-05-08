#pragma once

#include <iostream>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include "utils/logging/logger.hh"
namespace optkit::core::rapl
{ 
    /**
     * @brief All available rapl domain by 2023<br>
     * check intel_rapl for more information.
     *
     */
    enum class RaplDomain
    {
        BEGIN = 1,

        PP0 = (1 << 1),     // CORES
        PP1 = (1 << 2),     // INTEGRATED GPU
        PACKAGE = (1 << 3), // PP0 + PP1 + SYSTEM AGENT + LAST_LEVEL_CACHE MEMORY CONTROLLER
        PSYS = (1 << 4),    // PACKAGE + eDRAM + PCH
        DRAM = (1 << 5),    // DRAM DIMM 0 and DRAM DIMM 1

        END = (1 << 6),

        ALL = 0b0111110, // All domains
    };
    extern const std::unordered_map<int32_t, std::string> rapl_domain_name_mapping;

    RaplDomain mapMetricNameToRaplDomain(const std::string &metric_name);

    /**
     * @brief Rapl Read Methods
     *
     */
    enum class RaplReadMethods
    {
        PERF = (1 << 1),
        MSR = (1 << 2),
        POWERCAP = (1 << 3),
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
    
    std::ostream &operator<<(std::ostream &os, const optkit::core::rapl::RaplDomain &domain);
    std::ostream &operator<<(std::ostream &os, const optkit::core::rapl::RaplDomainInfo &domain_info);
    std::ostream &operator<<(std::ostream &os, const optkit::core::rapl::RaplReadMethods &read_method);

} // namespace optkit::core::rapl
