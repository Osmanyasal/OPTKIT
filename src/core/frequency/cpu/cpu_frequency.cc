#include "core/frequency/cpu/cpu_frequency.hh"

namespace optkit::core::frequency
{
    // Define static member variables

    // Socket id - cpus belonging to that socket
    // e.g. {0: [0, 1, 2, 3], 1: [8, 9, 10, 11]} means socket 0 has cores 0-3 and socket 1 has cores 4-7
    static const std::map<int32_t, std::vector<int32_t>> &package_info = core::Query::detect_cpu_packages();

#define TRAVERSE_CORES(socket)                           \
    if (package_info.find(socket) == package_info.end()) \
    {                                                    \
        OPTKIT_CORE_WARN("Invalid socket {}", socket);   \
    }                                                    \
    else                                                 \
        for (int32_t __cpu : package_info.at(socket))

    int64_t CPUFrequency::convert_frequency_with_unit(const std::string &freq_str, Unit target_unit)
    {
        size_t i = 0;
        while (i < freq_str.size() && (std::isdigit(freq_str[i]) || freq_str[i] == '.'))
            ++i;

        if (i == 0)
            throw std::invalid_argument("No numeric value in frequency string: " + std::string(freq_str));

        double number = std::stod(std::string(freq_str.substr(0, i)));

        std::string unit_str;
        for (char c : freq_str.substr(i))
            if (!std::isspace(c))
                unit_str += static_cast<char>(std::tolower(c));

        // Normalize for comparison (to_string returns capitalized, so lower it)
        auto lower = [](const std::string &s)
        {
            std::string r;
            for (char c : s)
                r += std::tolower(c);
            return r;
        };

        static const std::string u_hz = lower(to_string(Unit::Hz));
        static const std::string u_khz = lower(to_string(Unit::KHz));
        static const std::string u_mhz = lower(to_string(Unit::MHz));
        static const std::string u_ghz = lower(to_string(Unit::GHz));

        double base_hz = 0;
        if (unit_str.empty() || unit_str == u_hz)
            base_hz = number;
        else if (unit_str == u_khz)
            base_hz = number * 1e3;
        else if (unit_str == u_mhz)
            base_hz = number * 1e6;
        else if (unit_str == u_ghz)
            base_hz = number * 1e9;
        else
            throw std::invalid_argument("Unknown frequency unit: " + unit_str);

        switch (target_unit)
        {
        case Unit::Hz:
            return static_cast<int64_t>(base_hz);
        case Unit::KHz:
            return static_cast<int64_t>(base_hz / 1e3);
        case Unit::MHz:
            return static_cast<int64_t>(base_hz / 1e6);
        case Unit::GHz:
            return static_cast<int64_t>(base_hz / 1e9);
        default:
            throw std::invalid_argument("Unknown target unit: " + to_string(target_unit));
        }
    }

    void CPUFrequency::set_core_frequency(int64_t frequency, int16_t socket)
    {
        try
        {
            // Static variables for caching frequency limits - queried once per method call
            static int64_t min_freq = QueryCPUFrequency::get_cpuinfo_min_freq(package_info.at(socket)[0]);
            static int64_t max_freq = QueryCPUFrequency::get_cpuinfo_max_freq(package_info.at(socket)[0]);

            // Validate frequency range
            if (frequency < min_freq || frequency > max_freq)
            {
                OPTKIT_CORE_WARN("Frequency {} is outside valid range [{} - {}] KHz for socket {}", frequency, min_freq, max_freq, socket);
                return;
            }

            // Set core frequency for all cores
            TRAVERSE_CORES(socket)
            {
                optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(__cpu) + "/cpufreq/scaling_max_freq", std::to_string(frequency));
                optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(__cpu) + "/cpufreq/scaling_min_freq", std::to_string(frequency));
            }
        }
        catch (const std::runtime_error &err)
        {
            OPTKIT_CORE_ERROR(err.what());
        }
    }

    void CPUFrequency::set_core_frequency(int64_t frequency, int16_t cpu, int16_t socket)
    {
        if (cpu >= 0 && cpu < Query::num_logical_cores)
        {
            try
            {
                // Get frequency limits directly for this CPU
                static int64_t min_freq = QueryCPUFrequency::get_cpuinfo_min_freq(cpu);
                static int64_t max_freq = QueryCPUFrequency::get_cpuinfo_max_freq(cpu);

                // Validate frequency range
                if (frequency < min_freq || frequency > max_freq)
                {

                    OPTKIT_CORE_WARN("Frequency {} is outside valid range [{} - {}] KHz for CPU {}", frequency, min_freq, max_freq, cpu);
                    return;
                }

                TRAVERSE_CORES(socket)
                {
                    if (cpu == __cpu)
                    {
                        optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(__cpu) + "/cpufreq/scaling_max_freq", std::to_string(frequency));
                        optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(__cpu) + "/cpufreq/scaling_min_freq", std::to_string(frequency));
                    }
                }
            }
            catch (const std::runtime_error &err)
            {
                OPTKIT_CORE_ERROR(err.what());
            }
        }
        else
        {
            OPTKIT_CORE_WARN("Invalid range cpu={}", cpu);
        }
    }

    void CPUFrequency::set_core_frequency(int64_t frequency, int16_t cpu_start, int16_t cpu_end, int16_t socket)
    {
        if (cpu_start < 0 || cpu_end >= Query::num_logical_cores || cpu_start > cpu_end)
        {
            OPTKIT_CORE_WARN("Invalid range cpu_start={} cpu_end={}", cpu_start, cpu_end);
            return;
        }

        try
        {
            static int64_t min_freq = QueryCPUFrequency::get_cpuinfo_min_freq(cpu_start);
            static int64_t max_freq = QueryCPUFrequency::get_cpuinfo_max_freq(cpu_start);

            if (frequency < min_freq || frequency > max_freq)
            {
                OPTKIT_CORE_WARN("Frequency {} is outside valid range [{} - {}] KHz for CPU {}", frequency, min_freq, max_freq, cpu_start);
                return;
            }

            TRAVERSE_CORES(socket)
            {
                if (__cpu < cpu_start || __cpu > cpu_end)
                    continue;

                optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(__cpu) + "/cpufreq/scaling_max_freq", std::to_string(frequency));
                optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(__cpu) + "/cpufreq/scaling_min_freq", std::to_string(frequency));
            }
        }
        catch (const std::runtime_error &err)
        {
            OPTKIT_CORE_ERROR(err.what());
        }
    }

    int64_t CPUFrequency::get_core_frequency(int16_t cpu)
    {
        try
        {
            if (cpu >= 0 && cpu < Query::num_logical_cores)
            {
                return std::atol(optkit::utils::read_file("/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/cpufreq/scaling_cur_freq").c_str());
            }
            else
            {
                OPTKIT_CORE_WARN("Invalid range cpu={}", cpu);
                return -1;
            }
        }
        catch (const std::runtime_error &err)
        {
            OPTKIT_CORE_ERROR(err.what());
        }
        return -1;
    }

    std::vector<int64_t> CPUFrequency::get_core_frequencies(int16_t socket)
    {
        std::vector<int64_t> core_frequencies;
        try
        {
            TRAVERSE_CORES(socket)
            {
                core_frequencies.push_back(std::atol(optkit::utils::read_file("/sys/devices/system/cpu/cpu" + std::to_string(__cpu) + "/cpufreq/scaling_cur_freq").c_str()));
            }
        }
        catch (const std::runtime_error &err)
        {
            OPTKIT_CORE_ERROR(err.what());
        }
        return core_frequencies;
    }

    std::vector<int64_t> CPUFrequency::get_core_frequency(int16_t cpu_start, int16_t cpu_end, int16_t socket)
    {
        if (cpu_start < 0 || cpu_end < cpu_start || cpu_end >= Query::num_logical_cores)
        {
            OPTKIT_CORE_WARN("Invalid range cpu_start={} cpu_end={}", cpu_start, cpu_end);
            return {};
        }

        std::vector<int64_t> core_frequencies;
        try
        {
            TRAVERSE_CORES(socket)
            {

                if (__cpu < cpu_start || __cpu > cpu_end)
                    continue;

                core_frequencies.push_back(std::atol(optkit::utils::read_file("/sys/devices/system/cpu/cpu" + std::to_string(__cpu) + "/cpufreq/scaling_cur_freq").c_str()));
            }
        }
        catch (const std::runtime_error &err)
        {
            OPTKIT_CORE_ERROR(err.what());
        }
        return core_frequencies;
    }

#if OPTKIT_ENV_CPU_INTEL
    int64_t CPUFrequency::get_uncore_frequency(int16_t socket)
    {
        uint64_t MSR_UNCORE_RATIO_LIMIT_bits = 0;
        optkit::utils::read_msr(package_info.at(socket)[0], MSR_UNCORE_RATIO_LIMIT, &MSR_UNCORE_RATIO_LIMIT_bits);

        int64_t uncore_freq = (MSR_UNCORE_RATIO_LIMIT_bits & MSR_UNCORE_CURRENT_RATIO_mask) * 100000;
        return uncore_freq;
    }

    std::pair<int64_t, int64_t> CPUFrequency::get_uncore_min_max(int16_t socket)
    {
        std::pair<int64_t, int64_t> result{0, 0};

        uint64_t MSR_UNCORE_RATIO_LIMIT_bits = 0;
        optkit::utils::read_msr(package_info.at(socket)[0], MSR_UNCORE_RATIO_LIMIT, &MSR_UNCORE_RATIO_LIMIT_bits);

        // min uncore freq
        result.first = ((MSR_UNCORE_RATIO_LIMIT_bits & MSR_UNCORE_RATIO_LIMIT_min_mask) >> MSR_UNCORE_RATIO_LIMIT_min_shift) * 100000;

        // max uncore freq
        result.second = (MSR_UNCORE_RATIO_LIMIT_bits & MSR_UNCORE_RATIO_LIMIT_max_mask) * 100000;

        return result;
    }
    void CPUFrequency::reset_uncore_frequency(int16_t socket)
    {
        std::pair<int64_t, int64_t> default_uncore = get_uncore_min_max(socket);
        uint64_t MSR_UNCORE_RATIO_LIMIT_bits = ((default_uncore.first / 100000) << MSR_UNCORE_RATIO_LIMIT_min_shift) + default_uncore.second / 100000;
        optkit::utils::write_msr(socket, MSR_UNCORE_RATIO_LIMIT, MSR_UNCORE_RATIO_LIMIT_bits);
    }

    void CPUFrequency::set_uncore_frequency(int64_t frequency, int16_t socket)
    {
        uint64_t MSR_UNCORE_RATIO_LIMIT_bits = ((frequency / 100000) << MSR_UNCORE_RATIO_LIMIT_min_shift) + frequency / 100000;
        optkit::utils::write_msr(socket, MSR_UNCORE_RATIO_LIMIT, MSR_UNCORE_RATIO_LIMIT_bits);
    }
#else
    int64_t CPUFrequency::get_uncore_frequency(int16_t socket) {};
    std::pair<int64_t, int64_t> CPUFrequency::get_uncore_min_max(int16_t socket) {};
    void CPUFrequency::reset_uncore_frequency(int16_t socket) {};
    void CPUFrequency::set_uncore_frequency(int64_t frequency, int16_t socket) {};
#endif
    void CPUFrequency::reset_core_frequency(int16_t socket)
    {
        try
        {
            // Set core frequency for all cores
            TRAVERSE_CORES(socket)
            {
                optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(__cpu) + "/cpufreq/scaling_max_freq", std::to_string(QueryCPUFrequency::get_cpuinfo_max_freq(__cpu)));
                optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(__cpu) + "/cpufreq/scaling_min_freq", std::to_string(QueryCPUFrequency::get_cpuinfo_min_freq(__cpu)));
            }
        }
        catch (const std::runtime_error &err)
        {
            OPTKIT_CORE_ERROR(err.what());
        }
    }
    std::string to_string(const std::pair<int64_t, int64_t> &pair)
    {
        std::ostringstream oss;
        oss << pair;
        return oss.str();
    }
    std::ostream &operator<<(std::ostream &os, const std::pair<int64_t, int64_t> &pair)
    {
        os << "(" << pair.first << ", " << pair.second << ")";
        return os;
    }
    std::string to_string(CPUFrequency::Unit unit)
    {
        using Unit = CPUFrequency::Unit;
        switch (unit)
        {
        case Unit::Hz:
            return "Hz";
        case Unit::KHz:
            return "KHz";
        case Unit::MHz:
            return "MHz";
        case Unit::GHz:
            return "GHz";
        default:
            return "Unknown";
        }
    }

#undef TRAVERSE_CORES
}
