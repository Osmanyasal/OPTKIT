#include "core/freq/query_frequency.hh"

namespace optkit::core::freq
{

    std::vector<int64_t> QueryFreq::get_scaling_available_frequencies(int32_t core)
    {
        std::vector<int64_t> frequencies;
        std::string file_content = read_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_available_frequencies", false);
        std::istringstream iss(file_content);
        int64_t freq;
        while (iss >> freq)
        {
            frequencies.push_back(freq * 1000);
        }
        return frequencies;
    }

    int64_t QueryFreq::get_bios_limit(int32_t core)
    {
        static std::string file_content = read_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/bios_limit", false);
        return std::stol(file_content) * 1000;
    }

    std::string QueryFreq::get_scaling_driver(int32_t core)
    {
        static std::string scaling_driver = read_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_driver", false);
        return scaling_driver;
    }

    std::string QueryFreq::get_scaling_governor(int32_t core)
    { 
        std::string scaling_governor = read_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_governor", false);
        return scaling_governor;
    }

    std::vector<std::string> QueryFreq::get_available_governors(int32_t core)
    {
        static std::string available_governors = read_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_available_governors", false);
        return str_split(available_governors, " ");
    }

    int64_t QueryFreq::get_scaling_max_limit(int32_t core)
    {
        std::string file_content = read_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_max_freq", false);
        return std::stol(file_content) * 1000;
    }

    int64_t QueryFreq::get_scaling_min_limit(int32_t core)
    {
        std::string file_content = read_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_min_freq", false);
        return std::stol(file_content) * 1000;
    }

    int64_t QueryFreq::get_cpuinfo_max_freq(int32_t core)
    {
        static std::string file_content = read_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/cpuinfo_max_freq", false);
        return std::stol(file_content) * 1000;
    }

    int64_t QueryFreq::get_cpuinfo_min_freq(int32_t core)
    {
        static std::string file_content = read_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/cpuinfo_min_freq", false);
        return std::stol(file_content) * 1000;
    }
}