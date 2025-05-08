#pragma once

#include <vector>
#include <map>
#include <utility>

#include "utils/utils.hh"
#include "core/query.hh"
#include "core/freq/query_frequency.hh"
#include "core/freq/msr_util.hh"
#include "core/freq/msrs.hh"

namespace optkit::core::freq
{

    /**
     * @brief All frequencies are HZ !
     *
     */
    class CPUFrequency final
    {
    public:
        static void set_core_frequency(int64_t frequency, int16_t socket);
        static void set_core_frequency(int64_t frequency, int16_t cpu, int16_t socket);
        static void set_core_frequency(int64_t frequency, int16_t cpu_start, int16_t cpu_end, int16_t socket);
        static void set_core_frequency(int64_t frequency, std::vector<int16_t> cpu_list);
        static int64_t get_core_frequency(int16_t cpu);
        static std::vector<int64_t> get_core_frequencies(int16_t socket);
        static std::vector<int64_t> get_core_frequency(int16_t cpu_start, int16_t cpu_end, int16_t socket);
        static std::pair<int64_t, int64_t> get_uncore_min_max(int16_t socket);

        static int64_t get_uncore_frequency(int16_t socket);
        static void set_uncore_frequency(int64_t frequency, int16_t socket);

        static void reset_core_frequency(int16_t socket);
        static void reset_uncore_frequency(int16_t socket);

    private:
        static const std::map<int32_t, std::vector<int32_t>> &package_info;

    private:
        CPUFrequency() = delete;
        ~CPUFrequency() = delete;
    };

    std::ostream &operator<<(std::ostream &os, const std::pair<int64_t, int64_t> &pair);
}

using optkit::core::freq::operator<<; // make available to global namespace