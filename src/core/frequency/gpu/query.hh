#pragma once

#include <ostream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <unistd.h>
#include <cstdint>
#include <unordered_map>

#include "utils/utils.hh"
#include "utils/gpu.hh"

namespace optkit::frequency::gpu
{

    /**
     * @brief All frequency values are in kilohertz (kHz).
     *
     * Users must provide and interpret frequencies in kHz.
     *
     * Examples:
     *   800000   -> 800 MHz   (0.8 GHz)
     *  1200000   -> 1200 MHz  (1.2 GHz)
     *  4600000   -> 4600 MHz  (4.6 GHz)
     */
    class Query final
    {
    public:
        static std::vector<int64_t> get_scaling_available_frequencies(int32_t core = 0);
        static int64_t get_bios_limit(int32_t core = 0);
        static std::string get_scaling_driver(int32_t core = 0);
        static std::string get_scaling_governor(int32_t core = 0);
        static void set_scaling_governor(const std::string &governor, int32_t socket = 0);
        static void set_scaling_governor_percore(const std::string &governor, int32_t core = 0);
        static std::vector<std::string> get_available_governors(int32_t core = 0);

        static int64_t get_scaling_max_limit(int32_t core = 0);
        static int64_t get_scaling_min_limit(int32_t core = 0);

        static int64_t get_gpuinfo_max_freq(int32_t core = 0);
        static int64_t get_gpuinfo_min_freq(int32_t core = 0);

    private:
        Query() = delete;
        ~Query() = delete;
    };

} // namespace optkit::frequency
