#pragma once

#include <vector>
#include <map>
#include <utility>

#include "utils/utils.hh"
#include "core/query.hh"
#include "core/frequency/cpu/query.hh"
#include "core/frequency/msrs.hh"
#include "core/frequency/utils.hh"

namespace optkit::frequency::cpu
{

    /**
     * @class CPUFrequency
     * @brief Provides low-level control and querying of core and uncore CPU frequencies.
     *
     * This class interfaces with the Linux `cpufreq` subsystem and Model-Specific Registers (MSRs)
     * to query and control both core and uncore frequencies on a per-core and per-socket basis.
     *
     * @details
     * - All frequency values are expressed in **kilohertz (kHz)** for consistency with the Linux cpufreq interface.
     * - Core frequencies are managed via sysfs: /sys/devices/system/cpu/cpu*\/cpufreq
     * - Uncore frequencies are managed via MSR registers Intel Only (e.g., `MSR_UNCORE_RATIO_LIMIT`).
     *
     * ### Features:
     * - Query available core frequencies, scaling governors, and BIOS limits.
     * - Set core frequency ranges and scaling governors per core or socket.
     * - Read and modify uncore frequency ratios via MSR for a specific socket.
     * - Convert frequency units (Hz ↔ kHz ↔ MHz ↔ GHz).
     * - Restore default uncore frequency limits.
     *
     * Users must provide and interpret frequencies in kHz.
     *
     * ### Example Frequencies:
     * - 800000 kHz → 800 MHz (0.8 GHz)
     * - 2400000 kHz → 2400 MHz (2.4 GHz)
     * - 4600000 kHz → 4600 MHz (4.6 GHz)
     *
     * @note Most operations require elevated privileges (e.g., root) to access sysfs or MSR interfaces.
     */

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
#ifndef U_MSR_PMON_FIXED_CTL_shift
#define U_MSR_PMON_FIXED_CTL_shift 22
#endif
#ifndef MSR_UNCORE_RATIO_LIMIT
#define MSR_UNCORE_RATIO_LIMIT 0x620
#endif
    class Frequency final
    {
    public:
        static void set_core_frequency(int64_t frequency, int16_t socket);
        static void set_core_frequency(int64_t frequency, int16_t cpu, int16_t socket);
        static void set_core_frequency(int64_t frequency, int16_t cpu_start, int16_t cpu_end, int16_t socket);
        static int64_t get_core_frequency(int16_t cpu);
        static std::vector<int64_t> get_core_frequencies(int16_t socket);
        static std::vector<int64_t> get_core_frequency(int16_t cpu_start, int16_t cpu_end, int16_t socket);

        // #if OPTKIT_ENV_CPU_INTEL
        static std::pair<int64_t, int64_t> get_uncore_min_max(int16_t socket);
        static int64_t get_uncore_frequency(int16_t socket);
        static void set_uncore_frequency(int64_t frequency, int16_t socket);
        static void reset_uncore_frequency(int16_t socket);
        // #endif
        static void reset_core_frequency(int16_t socket);

    private:
        Frequency() = delete;
        ~Frequency() = delete;
    };

    std::string to_string(const std::pair<int64_t, int64_t> &pair);
    std::ostream &operator<<(std::ostream &os, const std::pair<int64_t, int64_t> &pair);
}

using optkit::frequency::cpu::operator<<; // make available to global namespace