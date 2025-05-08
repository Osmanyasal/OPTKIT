#pragma once

#include <ostream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <map> 
#include <unistd.h>

#include "core/profiling/pmu/libpfm4_wrapper.hh"
#include "core/profiling/rapl/rapl.hh"
#include "utils/utils.hh"
namespace optkit::core::rapl
{
    /**
     * @brief  ASK CPU RAPL related queries here<br>
     *
     */
    class QueryRapl final
    {

    public:
    
        /**
         * @brief Returns available Rapl read methods in combination of RaplReadMethods as bitwise OR.
         * @see RaplReadMethods
         * @return int32_t
         */
        static int32_t avail_rapl_read_methods();

        static bool is_rapl_perf_avail();

        static bool is_rapl_powercap_avail();

        static bool is_rapl_msr_avail();

        /**
         * @brief Returns rapl domain info in the system
         *
         *
         * @return const ref of std::vector<RaplDomainInfo> static object!
         */
        static const std::vector<rapl::RaplDomainInfo> &rapl_domain_info();

    private:
        QueryRapl() = delete;
        ~QueryRapl() = delete;

    };

} // namespace optkit::core
