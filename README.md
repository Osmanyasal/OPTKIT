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

- `libpfm4` for PMU-related queries  
- `MSR-SAFE` library to adjust CPU uncore frequency (Intel only)  
- `sysfs` for modifying CPU core frequencies  
- Bash and Python3 for various utility tools

The functionality of OPTKIT depends on its utility programs, as outlined above.

## OPTKIT Building Blocks

```cpp
#include <optkit.hh>
int main(){ 
    OPTKIT_RUNTIME; // Init OPTKIT
    OPTKIT_FREQ_GOVERNOR; // Embedded Freq Governor 
    OPTKIT_RUNTIME_DATA_COLLECTOR; // Runtime data collector mode 
    
    // ****** RAPL Monitoring ********* //
    OPTKIT_RAPL(var_name, block_name);
    
    // ****** PMU Event Monitoring ********* //
    OPTKIT_PERFORMANCE_EVENTS(block_name, event_name, var_name, ...);
    
    // ****** Frequency Setting ********* //
    OPTKIT_SET_CPU_CORE_FREQ(frequency, socket);
    OPTKIT_SET_CPU_UNCORE_FREQ(frequency, socket);
    OPTKIT_SET_CPU_FREQ(core_freq, uncore_freq, socket);
    OPTKIT_RESET_CPU_FREQ(socket); 
    OPTKIT_RESET_CPU_CORE_FREQ(socket);
    OPTKIT_RESET_CPU_UNCORE_FREQ(socket);   
    
    // ****** High Level Performance Measurements ********* //
    OPTKIT_TMA_ANALYSIS(block_name, var_name, LXMetric::XXX);
    OPTKIT_COMPUTATIONAL_INTENSITY(block_name, var_name);
    OPTKIT_CACHE_INTENSITY(block_name, var_name);
    OPTKIT_DRAM_INTENSITY(block_name, var_name);
    
    // ****** Queries ********* //
    Query::<anything>; 
    QueryFreq::<anything>;
    QueryPMU::<anything>; 
}
```

The most commonly used building blocks of OPTKIT are shown above. OPTKIT is structured based on the RAII principle, automatically monitoring execution from the point of its definition until the end of the associated scope. Measurement reports are saved once the analysis is completed.

## Development

Project structure and explanations are given below. 

```
src/
│
├── core/
│   ├── metrics/                  # Unified abstraction layer of performance metrics
│   │   ├── cpu/                  # CPU specific performance metrics
│   │   │   ├── intel/
│   │   │   │   ├── icl/          # Intel Icelake-specific metric formulations
│   │   │   |   └── ...           # Intel Generic Metrics (available for all)
│   │   │   ├── amd/
│   │   │   │   └── fam19h_zen4/  # AMD FAM19_Zen4 architecture-specific metrics
│   │   │   |   └── ...           # AMD Generic Metrics(available for all)
│   │   │   └── ...               # Other CPU vendors or generic metric implementations
│   │   └── gpu/
│   │       ├── nvidia/           # NVIDIA GPU-specific performance metrics
│   │       ├── amd/             # AMD ROCm GPU-specific performance metrics
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
│   │   │   ├── msr/             # Direct MSR access for Intel CPUs
│   │   │   └── ...
│   │   ├── gpu/                 # GPU energy data sources
│   │   │   ├── nvidia/            # NVIDIA Management Library interface
│   │   │   ├── amd/            # ROCm System Management Interface (SMI)
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
│       │   ├── nvidia/            # NVIDIA PMU event access (via NVML)
│       │   ├── amd/            # AMD ROCm-based PMU access
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
