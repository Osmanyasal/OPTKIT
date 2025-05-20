# OPTKIT Overview

OPTKIT is a highly customizable C++11 library and toolset designed for measuring energy consumption, detecting performance bottlenecks, and tuning hardware parameters at runtime to improve overall energy efficiency. Its overhead remains low, primarily depending on the frequency of measurements and the number of monitored regions.

OPTKIT integrates seamlessly into the development workflow like any other library. It can assist developers during development by providing energy and performance bottleneck-related insights to guide code improvements and refactoring, or it can be embedded into production environments to dynamically optimize hardware settings for greater energy efficiency.


## Download and Install 🚀 <br>
```
git clone https://github.com/Osmanyasal/OPTKIT.git
cd ./OPTKIT
git submodule update --force --recursive --init --remote
premake5 gmake
make configure=release -j$(nproc) all
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
│   ├── metrics/                  # Unified abstraction layer for performance metrics
│   │   ├── cpu/
│   │   │   ├── intel/
│   │   │   │   ├── icl/          # Icelake-specific formulas
│   │   │   │   └── ...
│   │   │   ├── amd/
│   │   │   │   └── zen/..        # Zen, Zen2, etc.
│   │   │   └── ...
│   │   └── gpu/
│   │       ├── nvidia/
│   │       ├── rocm/
│   │       └── ...
│   │
│   ├── events/                  # PMU Event Codes
│   │   ├── intel/               # Can be used directly with perf_event_open. 
│   │   ├── arm/                 # TODO: GPU events should also be provided
│   │   └── ...
│   │
│   ├── freq/                    # Frequency interface for CPU and GPU
│   │   ├── cpu/                 # access can be via sysfs, msr-safe, nvml for gpu or rocm interface.
│   │   │   ├── intel/
│   │   │   ├── amd/
│   │   │   └── ...
│   │   └── gpu/
│   │       ├── nvidia/
│   │       ├── rocm/
│   │       └── ...
│   │
│   ├── energy/                  # Energy consumption modules
│   │   ├── rapl/                # Intel RAPL interface
│   │   ├── nvml/                # NVIDIA Management Library
│   │   └── rocm/                # ROCm SMI
│   │
│   └── pmu/                     # Low-level access
│       ├── perf/                # Linux perf interface
│       ├── msr/                 # Model-specific registers
│       └── ...
│
├── utils/                       # Cross-cutting utilities
│   ├── logging/                 # Logging infra
│   ├── optimizations/           # Optional optimization helpers
│   ├── environment_config.hh    # Default configs for PMU/paths
│   ├── pmu_parser.py            # (Optional) parses perf/PMU events from spec
│   ├── utils.hh                 # Inline shared helpers
│   └── utils.cc

```