# OPTKIT Overview

OPTKIT is a highly customizable C++11 library and toolset designed for measuring energy consumption, detecting performance bottlenecks, and tuning hardware parameters at runtime to improve overall energy efficiency. Its overhead remains low, primarily depending on the frequency of measurements and the number of monitored regions.

OPTKIT integrates seamlessly into the development workflow like any other library. It can assist developers during development by providing energy and performance bottleneck-related insights to guide code improvements and refactoring, or it can be embedded into production environments to dynamically optimize hardware settings for greater energy efficiency.


## Download and Install 🚀 <br>
```
git clone https://github.com/Osmanyasal/OPTKIT.git
cd ./OPTKIT
git submodule update --force --recursive --init --remote
premake5 gmake
make config=release -j$(nproc) all
```

## OPTKIT Utility Tools

| Utility             | Detail                                                                 |
|---------------------|------------------------------------------------------------------------|
| `beacon_rapl`       | Emits energy consumption of the CPU at a given frequency               |
| `beacon_freq`       | Emits CPU core frequencies for each core at a given frequency          |
| `freq`              | Changes core-uncore frequency or resets to default values              |
| `probe`             | Lists system specs for RAPL, PMU & CPU Frequency                       |
| `enrich`            | Enriches Energy & PMU reading *json reports to power, total energy consumption, EDP, etc. |
| `core_scaling`      | Performs core strong scaling for OpenMP programs by changing `OMP_NUM_THREADS` |
| `co_schedule`       | Co-schedules programs to specified cores and sockets                   |
| `freq_scaling`      | Strong scales core-uncore frequencies for a given application          |
| `vis_line`          | Creates line chart based on the result of `core_scaling` tool          |
| `vis_bar`           | Creates bar chart based on the input file                              |
| `vis_freq_heatmap`  | Creates a heatmap using the results of `freq_scaling`                  |

## Key Features

Each feature in OPTKIT is implemented using only a few classes, with numerous monitoring configurations available for each. While users can customize these settings as needed, OPTKIT provides C-style macros with default configurations that cover most common use cases.

OPTKIT uses the `perf_event_open` system call to monitor both PMU (Performance Monitoring Unit) events and RAPL energy metrics without needing additional *root* privileges, provided that global configurations are all set. It relies on:

- `perf_event_open` linux kernel call for both energy and performance monitoring.
- `msr-safe` library for direct CPU msr access (low level)
- `libpfm4` for PMU-related queries and event code database
- `sysfs` for modifying CPU core frequencies
- `bash` and `python3` for various utility tools

The functionality of OPTKIT depends on its utility programs, as outlined above.

## OPTKIT API

**OPTKIT** provides C-style macros with default configurations that cover the most common performance monitoring scenarios. All macros are prefixed with `OPTKIT`.

**OPTKIT** is structured based on the RAII principle.  **Energy**, **Performance**, and **Collector** macros perform measurements starting from the point they're defined and continue until the end of their enclosing scope. After the scope ends, results are automatically written to an output file.

- **Frequency macros** are *immediate*: they take effect as soon as they're invoked.
  - Changing **CPU frequency** typically takes ~1 ms.
  - Adjusting **GPU frequency** may take longer — usually between 5 ms and 200 ms, depending on the GPU.

Often, **OPTKIT** uses the `perf_event_open` kernel call to monitor both PMC (performance monitoring counters) and RAPL (energy consumption). By default, it is configured to monitor **only the current program**, explicitly excluding kernel events.

Additionally, if the monitoring thread (the one that calls `perf_event_open`) spawns new threads, OPTKIT is set up to automatically monitor these child threads as well and aggregate their results at the end of the measurement.

However, to provide more flexibility and support a wider range of use cases, **OPTKIT** also supports `msr-safe` for direct access to CPU **Model-Specific Registers (MSRs)**.

Using `msr-safe` allows OPTKIT to read and write MSRs safely without requiring elevated privileges or risking system stability. This approach is particularly useful for collecting low-level hardware performance data that may not be accessible through `perf_event_open`.

**Also note:** *msr-safe* provides system-wide access since it interacts directly with the CPU registers. Because *msr-safe* operates outside the OS and kernel context, it cannot limit monitoring to just a single program. Instead, it performs system-wide event counting, meaning that other running programs also contribute to the measurements.

This behavior is similar to using the command `perf stat -a`, which collects performance data across the entire system.

By combining both `perf_event_open` and `msr-safe`, OPTKIT offers a comprehensive toolkit for performance and energy monitoring across diverse hardware platforms and configurations, giving users the best of both worlds in terms of accuracy and ease of use.

```cpp
#include <optkit.hh>
int main(){ 
    OPTKIT_RUNTIME; // Init OPTKIT
    OPTKIT_FREQ_GOVERNOR; // Embedded Freq Governor 
    OPTKIT_RUNTIME_DATA_COLLECTOR; // Runtime data collector mode 
    
    // ****** Energy Monitoring ********* //
    OPTKIT_CPU_ENERGY(var_name, block_name);
    OPTKIT_GPU_ENERGY(var_name, block_name);
    
    // ****** PMU Event Monitoring ********* //
    OPTKIT_CPU_EVENTS(block_name, event_name, {{"event_name", event_code},...});
    OPTKIT_CPU_BLOCK_EVENTS(block_name, event_name, {{"event_name", event_code},...});

    OPTKIT_GPU_EVENTS(block_name, event_name, {{"event_name", event_code},...});
    OPTKIT_GPU_BLOCK_EVENTS(block_name, event_name, {{"event_name", event_code},...});
    
    // ****** Frequency Setting ********* //
    OPTKIT_SET_CPU_FREQUENCY(core_freq, uncore_freq, socket);
    OPTKIT_SET_CPU_CORE_FREQUENCY(frequency, socket);
    OPTKIT_SET_CPU_UNCORE_FREQUENCY(frequency, socket);

    OPTKIT_RESET_CPU_FREQUENCY(socket);
    OPTKIT_RESET_CPU_CORE_FREQUENCY(socket);
    OPTKIT_RESET_CPU_UNCORE_FREQUENCY(socket);   

    OPTKIT_SET_GPU_FREQUENCY(core_freq, uncore_freq, socket);
    OPTKIT_SET_GPU_CORE_FREQUENCY(frequency, socket);
    OPTKIT_SET_GPU_UNCORE_FREQUENCY(frequency, socket);

    OPTKIT_RESET_GPU_FREQUENCY(socket);
    OPTKIT_RESET_GPU_CORE_FREQUENCY(socket);
    OPTKIT_RESET_GPU_UNCORE_FREQUENCY(socket);   
    
    // ****** High Level Performance Measurements ********* //
    // OPTKIT_TMA_ANALYSIS(block_name, var_name, LXMetric::XXX);
    // OPTKIT_COMPUTATIONAL_INTENSITY(block_name, var_name);
    // OPTKIT_CACHE_INTENSITY(block_name, var_name);
    // OPTKIT_DRAM_INTENSITY(block_name, var_name);
    
    // ****** Queries ********* //
    Query::<anything>; 
    QueryFreq::<anything>;
    QueryPMU::<anything>; 
}
```

## Development

Project structure and explanations are given below.

```text
src/
│
├── core/
│   ├── metrics/                  # Unified abstraction layer of performance metrics
│   │   ├── cpu/                  # CPU specific performance metrics
│   │   │   ├── intel/            # Intel Generic Metrics (available for all)
│   │   │   ├── amd/              # AMD Generic Metrics(available for all)
│   │   │   └── ...               # Other CPU vendors or generic metric implementations
│   │   └── gpu/
│   │       ├── nvidia/           # NVIDIA GPU-specific performance metrics
│   │       ├── amd/              # AMD ROCm GPU-specific performance metrics
│   │       └── ...               # Support for additional GPU platforms
│   │
│   ├── frequency/               # Interfaces to access real-time frequency data
│   │   ├── cpu/                 # CPU frequency readers (sysfs, MSR-safe, etc.)
│   │   │   ├── intel/           # Intel-specific frequency sources
│   │   │   ├── amd/             # AMD-specific frequency sources
│   │   │   └── ...              # Generic CPU frequency source (sysfs)
│   │   └── gpu/                 # GPU frequency readers via NVML, ROCm, etc.
│   │       ├── nvidia/
│   │       ├── amd/
│   │       └── ...
│   │
│   ├── energy/                  # Modules to monitor energy consumption
│   │   ├── cpu/                 # CPU energy data sources
│   │   │   ├── rapl/            # Intel & AMD RAPL (Running Average Power Limit) interface
│   │   │   ├── msr/             # Direct MSR access for Intel and AMD CPUs
│   │   │   └── ...
│   │   ├── gpu/                 # GPU energy data sources
│   │   │   ├── nvidia/          # NVIDIA Management Library interface
│   │   │   ├── amd/             # ROCm System Management Interface (SMI)
│   │   │   └── ...
│   │
│   └── pmu/                     # Interfaces for accessing hardware performance counters
│       ├── cpu/                 # CPU Counters
│       │   ├── msr/             # MSR-based access to performance counters
│       │   ├── perf/            # Linux perf_event_open-based access
│       │   │   ├── events/      # Architecture-specific PMU event codes generated to be used with perf_event_open
│       │   │   │   ├── intel/   # Intel-specific PMU events
│       │   │   │   ├── arm/     # ARM-specific PMU events
│       │   │   │   └── ...
│       ├── gpu/                 # GPU performance monitoring interfaces
│       │   ├── nvidia/          # NVIDIA PMU event access (via NVML)
│       │   ├── amd/             # AMD ROCm-based PMU access
│       │   └── ...
│
├── utils/                       # General-purpose utilities and helpers
│   ├── logging/                 # Logging infrastructure and configuration
│   ├── optimizations/           # Runtime optimizations utilities
│   ├── environment_config.hh    # Generated on compile-time for the current system. It is used internally in the project.
│   ├── pmu_parser.py            # PMU event spec parser (used to auto-generate or verify metrics)
│   ├── utils.hh                 # Header for shared inline utility functions
│   └── utils.cc                 # Implementation of utility functions
```
