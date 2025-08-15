#pragma once

#include <string>
#include <ostream>

namespace optkit::core::metrics::temperature
{
    // Extended CoreEvents (BEGIN/END sentinels preserved)
    enum class CoreEvents
    {
        BEGIN = 0,

        // CPU temperature sensors (coretemp, k10temp, peci-cputemp, fam15h_power, ...)
        CPU,

        // Storage (NVMe / SSD / HDD)
        STORAGE,

        // CPU's integrated GPU (iGPU readings that are reported from the CPU package)
        CPUGPU,

        // External / discrete GPUs and accelerators (amdgpu, nvidia, nouveau, radeon, i915, etc.)
        GPU,

        // Network interface controllers exposing hwmon (some NICs export temps)
        NETWORK,

        // Motherboard / SuperIO chips (nct6775, it87, w836*, f718*, etc.)
        MOTHERBOARD,

        // Memory / DIMM SPD sensors (spd5118, jc42, peci-dimmtemp, ...)
        MEMORY,

        // USB / Type-C / PD / Thunderbolt controllers (tps*, usb, thunderbolt)
        USB,

        // AIO / coolant sensors (nzxt-kraken*, corsair, gigabyte_waterforce, etc.)
        AIO_COOLANT,

        // Power supply / PSU sensors (seasonic, corsair-psu, ibm-cffps, ...)
        PSU,

        // Baseboard Management Controller / server BMC hwmon nodes (ibmaem, menf21bmc_hwmon, ipmi)
        BMC,

        // Fans / PWM controllers (pwm-fan, mlxreg-fan, surface_fan)
        FAN,

        // Battery / UPS related
        BATTERY,

        // ACPI / thermal zones
        ACPI_THERMAL,

        // Generic I2C/SPI temp chips (lm*, tmp*, ltc*, max*, adt*, etc.)
        GENERIC_I2C,

        // SoC / platform-specific hwmon (raspberrypi-hwmon, vexpress, ampere-smpro, occ-hwmon)
        SOC_PLATFORM,

        // Accelerators distinct from GPUs (e.g., specialized inference / accelerator hwmon nodes)
        ACCELERATOR,

        // Fallback/unknown
        UNKNOWN,

        END
    };

    std::string to_string(CoreEvents event);
    std::ostream &operator<<(std::ostream &os, CoreEvents event);
}