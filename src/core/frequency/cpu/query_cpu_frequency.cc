#include "core/frequency/cpu/query_cpu_frequency.hh"

namespace optkit::core::frequency
{

    std::vector<int64_t> QueryCPUFrequency::get_scaling_available_frequencies(int32_t core)
    {
        std::vector<int64_t> frequencies;
        std::string file_content = optkit::utils::read_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_available_frequencies");
        std::istringstream iss(file_content);
        int64_t freq;
        while (iss >> freq)
        {
            frequencies.push_back(freq * 1000);
        }
        return frequencies;
    }

    int64_t QueryCPUFrequency::get_bios_limit(int32_t core)
    {
        try
        {
            static std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/bios_limit";
            std::string file_content = optkit::utils::read_file(path);
            return std::stol(file_content) * 1000;
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get BIOS limit for core {}: {}", core, e.what());
            return -1; // or another sentinel value or rethrow if preferred
        }
    }

    std::string QueryCPUFrequency::get_scaling_driver(int32_t core)
    {
        try
        {
            static std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_driver";
            return optkit::utils::read_file(path);
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get scaling driver for core {}: {}", core, e.what());
            return "";
        }
    }

    std::string QueryCPUFrequency::get_scaling_governor(int32_t core)
    {
        try
        {
            static std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_governor";
            return optkit::utils::read_file(path);
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get scaling governor for core {}: {}", core, e.what());
            return "";
        }
    }

    std::vector<std::string> QueryCPUFrequency::get_available_governors(int32_t core)
    {
        try
        {
            static std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_available_governors";
            std::string available_governors = optkit::utils::read_file(path);
            return optkit::utils::str_split(available_governors, " ");
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get available governors for core {}: {}", core, e.what());
            return {};
        }
    }

    int64_t QueryCPUFrequency::get_scaling_max_limit(int32_t core)
    {
        try
        {
            static std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_max_freq";
            std::string content = optkit::utils::read_file(path);
            return std::stol(content) * 1000;
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get scaling max limit for core {}: {}", core, e.what());
            return -1;
        }
    }

    int64_t QueryCPUFrequency::get_scaling_min_limit(int32_t core)
    {
        try
        {
            static std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_min_freq";
            std::string content = optkit::utils::read_file(path);
            return std::stol(content) * 1000;
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get scaling min limit for core {}: {}", core, e.what());
            return -1;
        }
    }

    int64_t QueryCPUFrequency::get_cpuinfo_max_freq(int32_t core)
    {
        try
        {
            static std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/cpuinfo_max_freq";
            std::string content = optkit::utils::read_file(path);
            return std::stol(content) * 1000;
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get cpuinfo max freq for core {}: {}", core, e.what());
            return -1;
        }
    }

    int64_t QueryCPUFrequency::get_cpuinfo_min_freq(int32_t core)
    {
        try
        {
            static std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/cpuinfo_min_freq";
            std::string content = optkit::utils::read_file(path);
            return std::stol(content) * 1000;
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get cpuinfo min freq for core {}: {}", core, e.what());
            return -1;
        }
    }

}