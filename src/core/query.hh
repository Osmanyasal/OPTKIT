#pragma once

#include <ostream>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <unistd.h>

namespace optkit
{
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
        // TODO: there can be many sockets, make this an array using env_config socket count.
        static int64_t OPTKIT_SOCKET0__ENABLED;
        static int64_t OPTKIT_SOCKET1__ENABLED;
        static int64_t OPTKIT_SOCKET0__CORE_FREQ;
        static int64_t OPTKIT_SOCKET1__CORE_FREQ;
        static int64_t OPTKIT_SOCKET0__UNCORE_FREQ;
        static int64_t OPTKIT_SOCKET1__UNCORE_FREQ;

        static bool create_folder;
        static const int16_t num_sockets;
        static const int16_t num_logical_cores;
        static const bool is_root_priv_enabled;
        /**
         * @brief Gets package - # of cores information
         * @return const ref of static std::unordered_map<int32_t,std::vector<int32_t>> object: package - # of cores
         */
        static const std::map<int32_t, std::vector<int32_t>> &detect_cpu_packages();

        static bool is_smt_enabled()
        {
            std::string content = optkit::utils::read_file("/sys/devices/system/cpu/smt/active");
            if (content.empty())
                return false; // file missing or empty
            if (!content.empty() && content.back() == '\n')
                content.pop_back();
            return std::strtol(content.c_str(), nullptr, 10) != 0;
        }

        /**
         * @brief Returns current perf_event_paranoid value from "/proc/sys/kernel/perf_event_paranoid"<br>
         * Suggested value is -1 but 0 is also okay. cannot gurantee to accuretely measure for values above >0.
         *
         * @return int32_t paranoid value.
         */
        static int32_t paranoid();

    private:
        Query() = delete;
        ~Query() = delete;
    };

    std::string to_string(const std::map<int32_t, std::vector<int32_t>> &packages);
    std::ostream &operator<<(std::ostream &out, const std::map<int32_t, std::vector<int32_t>> &packages);
} // namespace optkit

using optkit::operator<<; // make available to global namespace