#include "core/frequency/cpu/query.hh"
#include <algorithm> // std::count

namespace optkit::frequency::cpu
{

    // Socket id - cpus belonging to that socket
    // e.g. {0: [0, 1, 2, 3], 1: [8, 9, 10, 11]} means socket 0 has cores 0-3 and socket 1 has cores 4-7
    static const std::map<int32_t, std::vector<int32_t>> &package_info = optkit::Query::detect_cpu_packages();

#define TRAVERSE_CORES(socket)                           \
    if (package_info.find(socket) == package_info.end()) \
    {                                                    \
        OPTKIT_CORE_WARN("Invalid socket {}", socket);   \
    }                                                    \
    else                                                 \
        for (int32_t __cpu : package_info.at(socket))

    std::vector<int64_t> Query::get_scaling_available_core_frequencies(int32_t core, int64_t step_khz)
    {
        std::vector<int64_t> frequencies;
        try
        {
            std::string avail_freqs = optkit::utils::read_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_available_frequencies");
            std::istringstream iss(avail_freqs);
            int64_t freq;
            while (iss >> freq)
                frequencies.push_back(freq);
            return frequencies;
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_WARN("Failed to read scaling available core frequencies for core {}: generating list from available max and mins", core);
            // Generate list from available max and mins
            int64_t max_freq = get_cpuinfo_max_freq(core);
            int64_t min_freq = get_cpuinfo_min_freq(core);
            if (min_freq > 0 && max_freq > 0 && max_freq >= min_freq)
            {
                // Fallback synthesis of available frequencies when sysfs list is missing.
                // Units: all kernel cpufreq interfaces expose kHz.
                constexpr int64_t TURBO_OFFSET_KHZ = 1000; // 1 MHz tail sometimes present on turbo advertised max

                std::string max_freq_str = std::to_string(max_freq);

                if (std::count(max_freq_str.begin(), max_freq_str.end(), '1') > 1 &&
                    max_freq - TURBO_OFFSET_KHZ >= min_freq)
                    max_freq -= TURBO_OFFSET_KHZ;

                // Reserve approximate number of steps to avoid reallocations.
                if (max_freq > min_freq)
                {
                    auto approx_steps = (max_freq - min_freq) / step_khz + 2; // +2 for inclusive end & possible tail adjust
                    frequencies.reserve(static_cast<size_t>(approx_steps));
                }

                for (int64_t freq = max_freq; freq >= min_freq && freq > 0; freq -= step_khz)
                    frequencies.push_back(freq);

                // Ensure min frequency present (avoid duplicate if exact on last step).
                if (!frequencies.empty() && frequencies.back() != min_freq && min_freq > 0)
                    frequencies.push_back(min_freq);

                // Defensive: if somehow empty (e.g. max == min but loop skipped), push min.
                if (frequencies.empty())
                {
                    if (min_freq > 0)
                        frequencies.push_back(min_freq);
                    if (max_freq > 0)
                        frequencies.push_back(max_freq);
                }

                return frequencies;
            }
        }
    }

    int64_t Query::get_bios_limit(int32_t core)
    {
        try
        {
            std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/bios_limit";
            std::string bios_limit = optkit::utils::read_file(path);

            // Trim trailing newline
            if (!bios_limit.empty() && bios_limit.back() == '\n')
                bios_limit.pop_back();
            return std::stol(bios_limit);
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get BIOS limit for core {}: {}", core, e.what());
            return -1; // or another sentinel value or rethrow if preferred
        }
    }

    std::string Query::get_scaling_driver(int32_t core)
    {
        try
        {
            std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_driver";
            std::string scaling_driver = optkit::utils::read_file(path);

            // Trim trailing newline
            if (!scaling_driver.empty() && scaling_driver.back() == '\n')
                scaling_driver.pop_back();

            return scaling_driver;
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get scaling driver for core {}: {}", core, e.what());
            return "";
        }
    }

    std::string Query::get_scaling_governor(int32_t core)
    {
        try
        {
            std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_governor";
            std::string scaling_governor = optkit::utils::read_file(path);

            // Trim trailing newline
            if (!scaling_governor.empty() && scaling_governor.back() == '\n')
                scaling_governor.pop_back();

            return scaling_governor;
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get scaling governor for core {}: {}", core, e.what());
            return "";
        }
    }
    void Query::set_scaling_governor(const std::string &governor, int32_t socket)
    {
        try
        {
            auto it = package_info.find(socket);
            if (it == package_info.end() || it->second.empty())
            {
                OPTKIT_CORE_ERROR("No cores found for socket {}", socket);
                return;
            }

            int32_t sample_core = it->second.front(); // First core in this socket

            // Read available governors
            std::string gov_path = "/sys/devices/system/cpu/cpu" + std::to_string(sample_core) + "/cpufreq/scaling_available_governors";
            std::string available_raw = optkit::utils::read_file(gov_path);
            if (!available_raw.empty() && available_raw.back() == '\n')
                available_raw.pop_back();
            auto available_governors = optkit::utils::str_split(available_raw, " ");

            if (std::find(available_governors.begin(), available_governors.end(), governor) == available_governors.end())
            {
                OPTKIT_CORE_WARN("Governor {} is not supported on socket {}.", governor, socket);

                std::ostringstream msg;
                msg << "Governor '" << governor << "' is not supported on socket " << socket << ". Available governors:";
                for (const auto &g : available_governors)
                    msg << " " << g;
                OPTKIT_CORE_DEBUG("Available governors: {}", msg.str());
                return;
            }

            // Apply governor to all cores in socket
            TRAVERSE_CORES(socket)
            {
                std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(__cpu) + "/cpufreq/scaling_governor";
                optkit::utils::write_file(path, governor);
            }

            OPTKIT_CORE_DEBUG("Governor '{}' successfully set for all cores in socket {}", governor, socket);
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to set scaling governor for socket {}: {}", socket, e.what());
        }
    }

    void Query::set_scaling_governor_percore(const std::string &governor, int32_t core)
    {
        try
        {
            std::string gov_path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_available_governors";

            // Read available governors
            std::string available_raw = optkit::utils::read_file(gov_path);

            // Trim trailing newline
            if (!available_raw.empty() && available_raw.back() == '\n')
                available_raw.pop_back();

            auto available_governors = optkit::utils::str_split(available_raw, " ");

            if (std::find(available_governors.begin(), available_governors.end(), governor) == available_governors.end())
            {
                std::ostringstream msg;
                msg << "Governor '" << governor << "' is not supported on core " << core << ". Available governors:";
                for (const auto &g : available_governors)
                    msg << " " << g;

                OPTKIT_CORE_WARN("{}", msg.str());
                return;
            }

            // Apply governor
            std::string set_path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_governor";
            optkit::utils::write_file(set_path, governor);

            OPTKIT_CORE_DEBUG("Governor '{}' successfully set for core {}", governor, core);
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to set scaling governor for core {}: {}", core, e.what());
        }
    }

    std::vector<std::string> Query::get_available_governors(int32_t core)
    {
        try
        {
            std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_available_governors";
            std::string available_governors = optkit::utils::read_file(path);

            // Trim trailing newline
            if (!available_governors.empty() && available_governors.back() == '\n')
                available_governors.pop_back();

            return optkit::utils::str_split(available_governors, " ");
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get available governors for core {}: {}", core, e.what());
            return {};
        }
    }

    int64_t Query::get_scaling_max_limit(int32_t core)
    {
        try
        {
            std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_max_freq";
            std::string content = optkit::utils::read_file(path);
            // Trim trailing newline
            if (!content.empty() && content.back() == '\n')
                content.pop_back();
            return std::stol(content);
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get scaling max limit for core {}: {}", core, e.what());
            return -1;
        }
    }

    int64_t Query::get_scaling_min_limit(int32_t core)
    {
        try
        {
            std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/scaling_min_freq";
            std::string content = optkit::utils::read_file(path);
            // Trim trailing newline
            if (!content.empty() && content.back() == '\n')
                content.pop_back();
            return std::stol(content);
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get scaling min limit for core {}: {}", core, e.what());
            return -1;
        }
    }

    int64_t Query::get_cpuinfo_max_freq(int32_t core)
    {
        try
        {
            std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/cpuinfo_max_freq";
            std::string content = optkit::utils::read_file(path);
            // Trim trailing newline
            if (!content.empty() && content.back() == '\n')
                content.pop_back();
            return std::stol(content);
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get cpuinfo max freq for core {}: {}", core, e.what());
            return -1;
        }
    }

    int64_t Query::get_cpuinfo_min_freq(int32_t core)
    {
        try
        {
            std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/cpuinfo_min_freq";
            std::string content = optkit::utils::read_file(path);
            // Trim trailing newline
            if (!content.empty() && content.back() == '\n')
                content.pop_back();
            return std::stol(content);
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_ERROR("Failed to get cpuinfo min freq for core {}: {}", core, e.what());
            return -1;
        }
    }
}