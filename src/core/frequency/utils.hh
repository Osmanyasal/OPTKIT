#pragma once

#include <string>
#include <stdexcept>
#include <cctype> // <-- needed for std::isdigit, std::isspace, std::tolower

#include "utils/optimizations/cpu_opt.hh"

namespace optkit::frequency
{
    enum class Unit
    {
        Hz,
        KHz,
        MHz,
        GHz
    };

    OPT_FORCE_INLINE std::string to_string(Unit unit)
    {
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

    /**
     * @brief Convert a frequency string to a target unit.
     *
     * @param freq_str The input frequency string (e.g., "2400 MHz").
     * @param target_unit The target unit to convert the frequency to.
     * @return double The converted frequency in the target unit.
     */
    inline double convert_frequency_with_unit(const std::string &freq_str, Unit target_unit = Unit::Hz)
    {
        size_t i = 0;
        while (i < freq_str.size() &&
               (std::isdigit(static_cast<unsigned char>(freq_str[i])) || freq_str[i] == '.'))
        {
            ++i;
        }

        if (i == 0)
            throw std::invalid_argument("No numeric value in frequency string: " + freq_str);

        double number = std::stod(freq_str.substr(0, i));

        // Extract and normalize unit string (remove spaces, lowercase)
        std::string unit_str;
        for (char c : freq_str.substr(i))
        {
            if (!std::isspace(static_cast<unsigned char>(c)))
                unit_str += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }

        // Normalized units for comparison
        auto lower = [](const std::string &s)
        {
            std::string r;
            r.reserve(s.size());
            for (char c : s)
                r += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            return r;
        };

        static const std::string u_hz = lower(to_string(Unit::Hz));
        static const std::string u_khz = lower(to_string(Unit::KHz));
        static const std::string u_mhz = lower(to_string(Unit::MHz));
        static const std::string u_ghz = lower(to_string(Unit::GHz));

        // Convert input to Hz first
        double base_hz = 0.0;
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

        // Convert Hz → target unit
        switch (target_unit)
        {
        case Unit::Hz:
            return base_hz;
        case Unit::KHz:
            return base_hz / 1e3;
        case Unit::MHz:
            return base_hz / 1e6;
        case Unit::GHz:
            return base_hz / 1e9;
        default:
            throw std::invalid_argument("Unknown target unit: " + to_string(target_unit));
        }
    }

} // namespace optkit::frequency
