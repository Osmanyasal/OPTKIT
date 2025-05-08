#pragma once

#include <vector>
#include <string>
#include "utils/utils.hh"

namespace optkit::core::freq
{

    class QueryFreq final
    {
    public:
        static std::vector<int64_t> get_scaling_available_frequencies(int32_t core = 0);
        static int64_t get_bios_limit(int32_t core = 0);
        static std::string get_scaling_driver(int32_t core = 0);
        static std::string get_scaling_governor(int32_t core = 0);
        static std::vector<std::string> get_available_governors(int32_t core = 0);

        static int64_t get_scaling_max_limit(int32_t core = 0);
        static int64_t get_scaling_min_limit(int32_t core = 0);

        static int64_t get_cpuinfo_max_freq(int32_t core = 0);
        static int64_t get_cpuinfo_min_freq(int32_t core = 0);

    private:
        QueryFreq() = delete;
        ~QueryFreq() = delete;
    };

} // namespace optkit::core::freq
