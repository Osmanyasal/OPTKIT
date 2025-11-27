#include "core/frequency/cpu/frequency.hh"

namespace optkit::frequency::cpu
{
    // Define static member variables

    // Socket id - cpus belonging to that socket
    // e.g. {0: [0, 1, 2, 3], 1: [8, 9, 10, 11]} means socket 0 has cores 0-3 and socket 1 has cores 4-7
    static const std::map<int32_t, std::vector<int32_t>> &package_info = optkit::Query::detect_cpu_packages();

#ifndef MSR_UNCORE_RATIO_LIMIT_max_mask
#define MSR_UNCORE_RATIO_LIMIT_max_mask 0x7F
#endif
#ifndef MSR_UNCORE_RATIO_LIMIT_min_mask
#define MSR_UNCORE_RATIO_LIMIT_min_mask 0x7F00
#endif
#ifndef MSR_UNCORE_RATIO_LIMIT_min_shif
#define MSR_UNCORE_RATIO_LIMIT_min_shift 8
#endif
#ifndef MSR_UNCORE_CURRENT_RATIO_mask
#define MSR_UNCORE_CURRENT_RATIO_mask 0x7F
#endif
#ifndef MSR_UNCORE_RATIO_LIMIT
#define MSR_UNCORE_RATIO_LIMIT 0x620
#endif

#define TRAVERSE_CORES(socket)                           \
    if (package_info.find(socket) == package_info.end()) \
    {                                                    \
        OPTKIT_CORE_WARN("Invalid socket {}", socket);   \
    }                                                    \
    else                                                 \
        for (int32_t __cpu : package_info.at(socket))

    bool Frequency::set_core_frequency(int64_t frequency, int16_t socket)
    {
        try
        {
            // Static variables for caching frequency limits - queried once per method call
            static int64_t min_freq = Query::get_cpuinfo_min_freq(package_info.at(socket)[0]);
            static int64_t max_freq = Query::get_cpuinfo_max_freq(package_info.at(socket)[0]);

            // Validate frequency range
            if (frequency < min_freq || frequency > max_freq)
            {
                OPTKIT_CORE_WARN("Frequency {} is outside valid range [{} - {}] KHz for socket {}", frequency, min_freq, max_freq, socket);
                return false;
            }

            // Set optkit frequency for all cores
            TRAVERSE_CORES(socket)
            {
                optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(__cpu) + "/cpufreq/scaling_max_freq", std::to_string(frequency));
                optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(__cpu) + "/cpufreq/scaling_min_freq", std::to_string(frequency));
            }
        }
        catch (const std::runtime_error &err)
        {
            OPTKIT_CORE_ERROR(err.what());
            return false;
        }
        return true;
    }

    bool Frequency::set_core_frequency(int64_t frequency, int16_t cpu, int16_t socket)
    {
        if (cpu >= 0 && cpu < OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS)
        {
            try
            {
                // Get frequency limits directly for this CPU
                static int64_t min_freq = Query::get_cpuinfo_min_freq(cpu);
                static int64_t max_freq = Query::get_cpuinfo_max_freq(cpu);

                // Validate frequency range
                if (frequency < min_freq || frequency > max_freq)
                {

                    OPTKIT_CORE_WARN("Frequency {} is outside valid range [{} - {}] KHz for CPU {}", frequency, min_freq, max_freq, cpu);
                    return false;
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
                return false;
            }
        }
        else
        {
            OPTKIT_CORE_WARN("Invalid range cpu={}", cpu);
            return false;
        }
        return true;
    }

    bool Frequency::set_core_frequency(int64_t frequency, int16_t cpu_start, int16_t cpu_end, int16_t socket)
    {
        if (cpu_start < 0 || cpu_end >= OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS || cpu_start > cpu_end)
        {
            OPTKIT_CORE_WARN("Invalid range cpu_start={} cpu_end={}", cpu_start, cpu_end);
            return false;
        }

        try
        {
            static int64_t min_freq = Query::get_cpuinfo_min_freq(cpu_start);
            static int64_t max_freq = Query::get_cpuinfo_max_freq(cpu_start);

            if (frequency < min_freq || frequency > max_freq)
            {
                OPTKIT_CORE_WARN("Frequency {} is outside valid range [{} - {}] KHz for CPU {}", frequency, min_freq, max_freq, cpu_start);
                return false;
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
            return false;
        }
        return true;
    }

    int64_t Frequency::get_core_frequency(int16_t cpu)
    {
        try
        {
            if (cpu >= 0 && cpu < OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS)
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

    std::vector<int64_t> Frequency::get_core_frequencies(int16_t socket)
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

    std::vector<int64_t> Frequency::get_core_frequency(int16_t cpu_start, int16_t cpu_end, int16_t socket)
    {
        if (cpu_start < 0 || cpu_end < cpu_start || cpu_end >= OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS)
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
    int64_t Frequency::get_uncore_frequency(int16_t socket)
    {
        uint64_t MSR_UNCORE_RATIO_LIMIT_bits = 0;
        optkit::utils::read_msr(package_info.at(socket)[0], MSR_UNCORE_RATIO_LIMIT, &MSR_UNCORE_RATIO_LIMIT_bits);

        int64_t uncore_freq = (MSR_UNCORE_RATIO_LIMIT_bits & MSR_UNCORE_CURRENT_RATIO_mask) * 100000;
        return uncore_freq;
    }

    std::pair<int64_t, int64_t> Frequency::get_uncore_min_max(int16_t socket)
    {
        static std::unordered_map<int16_t, std::pair<int64_t, int64_t>> cache;

        // Check cache first
        auto it = cache.find(socket);
        if (it != cache.end())
            return it->second;

        // Otherwise, read from MSR
        uint64_t bits = 0;
        optkit::utils::read_msr(package_info.at(socket)[0], MSR_UNCORE_RATIO_LIMIT, &bits);

        const int64_t min_ratio = (bits & MSR_UNCORE_RATIO_LIMIT_min_mask) >> MSR_UNCORE_RATIO_LIMIT_min_shift;
        const int64_t max_ratio = (bits & MSR_UNCORE_RATIO_LIMIT_max_mask);

        std::pair<int64_t, int64_t> result{
            min_ratio * 100000, // kHz
            max_ratio * 100000  // kHz
        };

        // Cache it
        cache[socket] = result;

        return result;
    }

    std::vector<int64_t> Frequency::get_scaling_available_uncore_frequencies(int16_t socket, int64_t step_khz)
    {
        std::vector<int64_t> frequencies;
        try
        {
            std::pair<int64_t, int64_t> avail_freqs = optkit::frequency::cpu::Frequency::get_uncore_min_max(socket);
            int64_t min_freq = avail_freqs.first;
            int64_t max_freq = avail_freqs.second;
            if (min_freq > 0 && max_freq > 0 && max_freq >= min_freq)
            {
                // Units: all kernel cpufreq interfaces expose kHz.
                int64_t TURBO_OFFSET_KHZ = 1000; // 1 MHz tail sometimes present on turbo advertised max

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
            }
        }
        catch (const std::exception &e)
        {
            OPTKIT_CORE_WARN("Failed to read scaling available uncore frequencies for socket {}", socket);
        }
        return frequencies;
    }

    bool Frequency::reset_uncore_frequency(int16_t socket)
    {
        std::pair<int64_t, int64_t> default_uncore = get_uncore_min_max(socket);
        uint64_t MSR_UNCORE_RATIO_LIMIT_bits = ((default_uncore.first / 100000) << MSR_UNCORE_RATIO_LIMIT_min_shift) + default_uncore.second / 100000;
        return optkit::utils::write_msr(socket, MSR_UNCORE_RATIO_LIMIT, MSR_UNCORE_RATIO_LIMIT_bits);
    }

    bool Frequency::set_uncore_frequency(int64_t frequency, int16_t socket)
    {
        uint64_t MSR_UNCORE_RATIO_LIMIT_bits = ((frequency / 100000) << MSR_UNCORE_RATIO_LIMIT_min_shift) + frequency / 100000;
        return optkit::utils::write_msr(socket, MSR_UNCORE_RATIO_LIMIT, MSR_UNCORE_RATIO_LIMIT_bits);
    }
#else
    int64_t Frequency::get_uncore_frequency(int16_t socket) { return -1; };
    std::pair<int64_t, int64_t> Frequency::get_uncore_min_max(int16_t socket) { return {}; };
    bool Frequency::reset_uncore_frequency(int16_t socket) { return false; };
    bool Frequency::set_uncore_frequency(int64_t frequency, int16_t socket) { return false; };
    std::vector<int64_t> Frequency::get_scaling_available_uncore_frequencies(int16_t socket, int64_t step_khz) { return {}; };
#endif
    bool Frequency::reset_core_frequency(int16_t socket)
    {
        try
        {
            // Set optkit frequency for all cores
            TRAVERSE_CORES(socket)
            {
                optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(__cpu) + "/cpufreq/scaling_max_freq", std::to_string(Query::get_cpuinfo_max_freq(__cpu)));
                optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(__cpu) + "/cpufreq/scaling_min_freq", std::to_string(Query::get_cpuinfo_min_freq(__cpu)));
            }
        }
        catch (const std::runtime_error &err)
        {
            OPTKIT_CORE_ERROR(err.what());
            return false;
        }
        return true;
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

#undef TRAVERSE_CORES
#undef MSR_UNCORE_RATIO_LIMIT_max_mask
#undef MSR_UNCORE_RATIO_LIMIT_min_mask
#undef MSR_UNCORE_RATIO_LIMIT_min_shift
#undef MSR_UNCORE_CURRENT_RATIO_mask
#undef MSR_UNCORE_RATIO_LIMIT
}
