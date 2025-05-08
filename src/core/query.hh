#pragma once

#include <ostream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <unistd.h>

#include "utils/utils.hh"
#include "core/profiling/rapl/rapl.hh"
#include "core/profiling/pmu/libpfm4_wrapper.hh"

namespace optkit::core
{
    std::ostream &operator<<(std::ostream &out, const std::map<int32_t, std::vector<int32_t>> &packages);
    /**
     * @brief  ASK System & CPU related queries here<br>
     * This Query class uses <b>libpfm4</b> to retrieve information
     *
     * Don't forget to call init() before using it and destroy() when you're done with it.<br>
     * These 2 method calls can be done at the beginning and end of the application.
     */
    class Query final
    {
    public:
    public:
        static int64_t OPTKIT_SOCKET0__ENABLED;
        static int64_t OPTKIT_SOCKET1__ENABLED;
        static int64_t OPTKIT_SOCKET0__CORE_FREQ;
        static int64_t OPTKIT_SOCKET1__CORE_FREQ;
        static int64_t OPTKIT_SOCKET0__UNCORE_FREQ;
        static int64_t OPTKIT_SOCKET1__UNCORE_FREQ;

        static const int16_t num_cores;
        static const int16_t num_sockets;
        static const bool is_root_priv_enabled;

        /**
         * @brief Gets package - # of cores information
         * @return const ref of static std::unordered_map<int32_t,std::vector<int32_t>> object: package - # of cores
         */
        static const std::map<int32_t, std::vector<int32_t>> &detect_packages();

    private:
        Query() = delete;
        ~Query() = delete;
    };

} // namespace optkit::core
