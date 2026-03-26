# HWMON Energy Profiler for Grace Chips

This module implements energy profiling support for ARM-based systems (particularly NVIDIA Grace chips) using the Linux HWMON (Hardware Monitoring) interface.

## Overview

The HWMON profiler reads power consumption data from `/sys/class/hwmon` and provides the same interface as the RAPL profiler, making it easy to use on ARM systems where RAPL is not available.

## Supported Power Domains

The HWMON profiler supports the following power domains on Grace systems:

- **CPU_POWER**: CPU power consumption per socket
- **MODULE_POWER**: Total module power (Grace-Hopper: Grace CPU + GPU)
- **SYSIO_POWER**: System I/O power consumption
- **GRACE_POWER**: Grace CPU power (on Grace-Grace systems)
- **GPU_POWER**: GPU power (derived: Module - Grace)

## File Structure

```
src/core/energy/cpu/hwmon/
├── hwmon.hh          # Domain definitions and enums
├── hwmon.cc          # Domain name mappings and operators
├── query.hh          # Query interface for HWMON availability
├── query.cc          # Implementation of HWMON detection
├── utils.hh          # Utility functions for JSON conversion
├── utils.cc          # Utility implementations
├── profiler.hh       # Main profiler class definition
├── profiler.cc       # Profiler implementation
├── module.hh         # Convenience macros for easy usage
└── clear.hh          # Macro cleanup
```

## How It Works

### Power Sensor Detection

The profiler automatically scans `/sys/class/hwmon/hwmonN` directories for power sensors:

1. Looks for files matching `power*_average` (instantaneous power in microwatts)
2. Reads corresponding `power*_oem_info` files to identify the domain
3. Parses labels like "CPU Power Socket 0", "Grace Power Socket 1", etc.
4. Records the socket ID and power domain mapping

### Energy Calculation

Since HWMON provides instantaneous power (Watts), not accumulated energy like RAPL:

1. Power is sampled at regular intervals
2. Energy is calculated as: **Energy (J) = Power (W) × Time (s)**
3. Multiple samples are integrated to compute total energy consumption

### Example Paths on Grace Systems

```bash
# Finding Grace power sensors
$ grep -r "Grace" /sys/class/hwmon/*/device/power*_oem_info
/sys/class/hwmon/hwmon3/device/power1_oem_info:Grace 0
/sys/class/hwmon/hwmon3/device/power2_oem_info:Grace 1

# Reading current power (in microwatts)
$ cat /sys/class/hwmon/hwmon3/device/power1_average
285000000  # 285 Watts

# Reading sampling interval (in milliseconds)
$ cat /sys/class/hwmon/hwmon3/device/power1_average_interval
100  # 100ms sampling period
```

## Usage

### Method 1: Using Macros (Recommended)

```cpp
#include "core/energy/cpu/hwmon/module.hh"

// Basic usage
OPTKIT_HWMON_ENERGY("my_workload")
{
    // Your code here
}

// With custom metrics
auto metrics = optkit::metrics::energy::cpu_metrics::all_metrics();
OPTKIT_HWMON_ENERGY_WITH_METRICS("my_workload", metrics)
{
    // Your code here
}

// Repeated measurements
OPTKIT_HWMON_ENERGY_REPEAT("benchmark", 10)
{
    run_iteration();
}

// Sampling mode (background thread samples every second)
OPTKIT_HWMON_ENERGY_SAMPLING("long_workload")
{
    // Your long-running code here
}
```

### Method 2: Manual Profiler Usage

```cpp
#include "core/energy/cpu/hwmon/profiler.hh"

// Create profiler
optkit::ProfilerConfig config{
    "my_block",          // block_name
    "hwmon_energy",      // measurement_type
    false,               // is_reset_after_read
    false,               // is_sampling
    true,                // dump_results_to_file
    true                 // verbose
};

auto metrics = optkit::metrics::energy::cpu_metrics::all_metrics();
optkit::energy::hwmon::Profiler profiler(config, metrics);

// Your workload
do_work();

// Optionally take manual readings
auto reading = profiler.read();
// reading is: map<socket_id, map<domain, watts>>

// Destructor automatically aggregates and saves results
```

### Method 3: Querying HWMON Availability

```cpp
#include "core/energy/cpu/hwmon/query.hh"

// Check if HWMON is available
if (optkit::energy::hwmon::Query::is_hwmon_sysfs_avail())
{
    std::cout << "HWMON is available!" << std::endl;
}

// Get available domains
const auto& domains = optkit::energy::hwmon::Query::hwmon_domain_info();
for (const auto& domain : domains)
{
    std::cout << "Domain: " << domain.label 
              << " Socket: " << domain.socket_id
              << " Path: " << domain.path << std::endl;
}
```

## Output Format

The profiler generates JSON output similar to RAPL:

```json
[
  {
    "block_name": "my_workload",
    "measurement_type": "hwmon_energy",
    "socket_id": 0,
    "duration_ms": 1250.5,
    "events": {
      "power-cpu__Joules": 125.3,
      "power-grace__Joules": 142.7,
      "power-module__Joules": 285.0
    },
    "metrics": {
      "total_power_watts": 228.0,
      "average_power_watts": 228.0
    }
  }
]
```

## Compilation

To enable HWMON support in your build, ensure the configuration flag is set:

```cpp
#define OPTKIT_CONF_HWMON_MACROS_ENABLED 1
```

## Example

See `examples/hwmon_example.cc` for a complete working example.

## Comparison with RAPL

| Feature | RAPL (x86) | HWMON (ARM/Grace) |
|---------|------------|-------------------|
| Interface | perf_event_open + MSR | sysfs (/sys/class/hwmon) |
| Measurement | Accumulated energy (J) | Instantaneous power (W) |
| Domains | Package, PP0, PP1, DRAM, PSYS | CPU, Module, SysIO, Grace, GPU |
| Sampling | Hardware counter | Software polling |
| Overhead | Very low | Low (sysfs reads) |
| Accuracy | High | High (depends on sensor) |

## Requirements

- Linux kernel with HWMON support
- Grace or compatible ARM system with power sensors
- Access to `/sys/class/hwmon` (typically requires root or specific permissions)

## Troubleshooting

### No sensors found

```bash
# Check if hwmon interface exists
ls /sys/class/hwmon/

# Check for power sensors
find /sys/class/hwmon -name "power*_average"

# Check labels
find /sys/class/hwmon -name "power*_oem_info" -exec cat {} \;
```

### Permission issues

```bash
# May need to run with sudo or adjust permissions
sudo chmod 644 /sys/class/hwmon/hwmon*/device/power*
```

## Notes

- Power readings are instantaneous, so more frequent sampling provides better energy estimates
- The profiler automatically handles per-socket measurements
- Sampling thread runs at 1 Hz by default (can be modified in profiler.cc)
- Energy is computed by integrating power over time between samples
- All power domains available on the system are automatically detected

## License

Part of the OPTKIT profiling framework.
