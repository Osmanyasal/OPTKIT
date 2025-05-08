#pragma once

#include <ostream>
#include <map>
#include <vector>
#include <sys/ioctl.h>

#include "core/profiling/rapl/rapl.hh"
#include "utils/utils.hh"
#include "core/profiling/base_profiler.hh"
#include "core/profiling/rapl/query_rapl.hh"
namespace optkit::core::rapl
{
 
    struct RaplPerfReaderConfig
    {
        // Constructor
        RaplPerfReaderConfig(
            const std::map<int32_t, std::vector<int32_t>> &packages,
            const std::vector<RaplDomainInfo> &avail_domains,
            const RaplConfig &rapl_config,
            const bool dump_results_to_file = true) : packages(packages),
                                                      avail_domains(avail_domains),
                                                      rapl_config(rapl_config)
        {
        }
        const std::map<int32_t, std::vector<int32_t>> packages;
        const std::vector<RaplDomainInfo> avail_domains;
        const RaplConfig rapl_config;
    };

    class RaplPerfReader
    {
    public:
        RaplPerfReader(const RaplPerfReaderConfig &rapl_perf_config);
        virtual ~RaplPerfReader();

        /**
         * @brief Returns sockect - domain - value relation
         *
         * @return std::map<int32_t, std::map<RaplDomain, int32_t>>
         */
        virtual std::map<int32_t, std::map<RaplDomain, double>> read_val();

    protected:
        RaplPerfReaderConfig rapl_perf_config;
        std::vector<std::vector<int32_t>> fd_package_domain;
    };

    std::ostream &operator<<(std::ostream &os, const optkit::core::rapl::RaplPerfReaderConfig &config);
    std::ostream &operator<<(std::ostream &os, const std::map<int32_t, std::map<optkit::core::rapl::RaplDomain, double>> &map);
} // namespace optkit::core::rapl
 