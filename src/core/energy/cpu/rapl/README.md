# RAPL Energy Profiler for x86 CPUs

This module implements energy profiling for Intel/AMD x86 CPUs using the RAPL (Running Average Power Limit) interface, via the Linux `perf_event_open` system call. This provides access to energy consumption counters exposed by the kernel's perf subsystem.

## Overview
The RAPL profiler reads energy and power data from the Linux perf subsystem, supporting multiple power domains and sockets. It provides a unified interface for energy measurement, compatible with OPTKIT's energy framework.

## Supported Power Domains
- **PACKAGE_ENERGY**: Total energy consumed by the CPU package (socket)
- **CORE_ENERGY**: Energy consumed by CPU cores (if available)
- **UNCORE_ENERGY**: Energy consumed by uncore components (e.g., LLC, memory controller)
- **DRAM_ENERGY**: Energy consumed by attached DRAM (if supported)
- **PSYS_ENERGY**: Platform/system energy (if supported)

## File Structure
```
src/core/energy/cpu/rapl/
├── rapl.hh         # RAPL domain definitions and enums
├── rapl.cc         # Domain name mappings and helpers
├── query.hh        # Query interface for RAPL availability
├── query.cc        # Implementation of RAPL detection
├── utils.hh        # Utility functions for JSON conversion
├── utils.cc        # Utility implementations
├── profiler.hh     # Main profiler class definition
├── profiler.cc     # Profiler implementation (uses perf_event_open)
├── module.hh       # Convenience macros for easy usage
└── clear.hh        # Macro cleanup
```

## How It Works
### perf_event_open Access
- The profiler uses the `perf_event_open` syscall to open file descriptors for RAPL energy events.
- Each power domain is mapped to a perf event (e.g., `energy-pkg`, `energy-cores`, `energy-dram`).
- The kernel exposes these events under `/sys/bus/event_source/devices/power/events/`.

### Energy Calculation
- Energy counters are sampled at the start and end of a region.
- Energy (Joules) is calculated as the difference, accounting for counter wraparound.
- Power (Watts) can be derived by dividing energy by elapsed time.

### Example: Reading Package Energy with perf_event_open
```cpp
#include <linux/perf_event.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <cstring>

struct perf_event_attr attr;
memset(&attr, 0, sizeof(attr));
attr.type = /* value from /sys/bus/event_source/devices/power/type */;
attr.config = /* value for energy-pkg domain */;
int fd = syscall(__NR_perf_event_open, &attr, -1, 0, -1, 0);
uint64_t val;
read(fd, &val, sizeof(val));
close(fd);
```

## Usage
### Method 1: Using Macros (Recommended)
```cpp
#include "core/energy/cpu/rapl/module.hh"

// Basic usage
OPTKIT_RAPL_ENERGY("my_workload")
{
    // Your code here
}

// With custom metrics
auto metrics = optkit::metrics::energy::cpu_metrics::all_metrics();
OPTKIT_RAPL_ENERGY_WITH_METRICS("my_workload", metrics)
{
    // Your code here
}
```

## Notes
- Requires Linux kernel 3.14+ with perf_event RAPL support.
- No root access is required, but the kernel must expose RAPL events via perf.
- For older systems or unsupported CPUs, use the MSR or sysfs methods if available.
- For ARM/other architectures, use the HWMON or PDU profilers.
