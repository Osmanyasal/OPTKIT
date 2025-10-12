# OPTKIT Overview

OPTKIT is a highly customizable C++11 library and toolset designed for measuring energy consumption, detecting performance bottlenecks, and tuning hardware parameters at runtime to improve overall energy efficiency. Its overhead remains low, primarily depending on the frequency of measurements and the number of monitored regions.

OPTKIT integrates seamlessly into the development workflow like any other library. It can assist developers during development by providing energy and performance bottleneck-related insights to guide code improvements and refactoring, or it can be embedded into production environments to dynamically optimize hardware settings for greater energy efficiency.

The library provides comprehensive monitoring capabilities for CPUs, GPUs, and I/O systems across multiple architectures including Intel, AMD, ARM, and NVIDIA platforms. It supports real-time frequency control, energy monitoring via RAPL, performance monitoring through PMU events, and includes a rich set of utility tools for performance analysis and optimization.

## Download and Install 🚀

```bash
git clone https://github.com/Osmanyasal/OPTKIT.git
cd ./OPTKIT
git submodule update --force --recursive --init --remote
premake5 gmake

## To create libraries:
make -j$(nproc) config=release optkit_static  # for static 
make -j$(nproc) config=release optkit_dynamic  # for dynamic 

## To Run Tests:
make -j$(nproc) config=test optkit_static ## this converts some private or protected fields to public and being tested
make -j$(nproc) config=debug optkit_test  ## no optimization in tests, raw results are viewed.
./bin/Debug/optkit_test

## 🔍 List All Available Tests
./bin/Debug/optkit_test --gtest_list_tests

## ▶️ Run Specific Test(s)
./bin/Debug/optkit_test --gtest_filter=MyTestSuite.MyTestCase
./bin/Debug/optkit_test --gtest_filter="CPUFreqTest.*"
./bin/Debug/optkit_test --gtest_filter="*Freq*:MemoryTest.*"

```

## CLI Tools

<details>
<summary><strong>optkit</strong> - Performance and Energy Profiling Tool</summary>

### Features

```bash
# Show CPU or GPU topology
optkit topology
optkit topology cpu
optkit topology gpu

# List PMU events for CPU or GPU
optkit list events cpu
optkit list events gpu

# List metrics available on CPU or GPU
optkit list metrics cpu
optkit list metrics gpu

# Run benchmarks with scaling options
optkit bench -- ./my_program
optkit bench freq-scaling -- ./my_program
optkit bench core-scaling -- ./my_program
optkit bench affinity --strategy=[compact|scatter|numa|manual] -- ./my_program

# Set frequencies and other system parameters manually or via config
optkit setenv core-freq=1800000 uncore-freq=1200000
optkit setenv --config ./env.config
```

### Usage Examples

```bash
# Profile CPU energy consumption
optkit bench --cpu-energy -- ./my_application

# Profile with specific PMU events
optkit bench --cpu-events=cycles,instructions -- ./my_application

# Frequency scaling analysis
optkit bench freq-scaling --min-freq=1200000 --max-freq=3000000 -- ./my_application

# Core scaling analysis
optkit bench core-scaling --min-cores=1 --max-cores=16 -- ./my_application
```

</details>

<details>
<summary><strong>optkit-setenv</strong> - System Environment Configuration Tool</summary>

### Overview

`optkit-setenv` is a comprehensive system environment configuration tool designed to set, tune, and isolate system parameters for reproducible, low-noise, and energy-aware benchmarking and profiling. It integrates closely with OptKit's profiling ecosystem but can be used standalone to control CPU, memory, disk, OS, GPU, and cgroup resources.

### CPU Controls

| Feature              | Flag                  | Possible Values                     | Description                                   |
|----------------------|-----------------------|-----------------------------------|-----------------------------------------------|
| SMT (Hyperthreading) | `--smt=on\|off`        | `on`, `off`                       | Enable or disable simultaneous multithreading. |
| CPU frequency         | `--cpu-freq=<MHz>`     | Numeric frequency (e.g., `2800`) | Set fixed CPU core frequency in MHz.           |
| Per-core frequency    | `--cpu-freq-cores=0:2800,1:2800` | List of `core:freq` pairs          | Set frequencies individually per core.         |
| CPU governor          | `--governor=<name>`    | `performance`, `powersave`, `userspace`, `ondemand`, `schedutil` | CPU frequency scaling governor. |
| Turbo Boost           | `--turbo=on\|off`       | `on`, `off`                       | Enable or disable Intel Turbo Boost.           |
| Uncore frequency      | `--uncore-freq=<MHz>`  | Numeric frequency                  | Set uncore/cache frequency (Intel-specific).  |
| C-states control      | `--cstates=on\|off`     | `on`, `off`                       | Enable or disable CPU idle states.             |
| CPU core online/offline | `--offline-cores=1,3` | Comma-separated core IDs           | Take specific cores offline to reduce noise.  |
| CPU affinity          | `--affinity=0-3`       | List or range of CPU cores         | Restrict benchmark processes to specified cores. |
| P-State limits        | `--pstate-min=<%> --pstate-max=<%>` | Numeric percent (e.g., 80, 100) | Limit CPU P-State performance range.           |

### Memory & NUMA Controls

| Feature               | Flag                   | Possible Values                   | Description                                       |
|-----------------------|------------------------|---------------------------------|-------------------------------------------------|
| Transparent HugePages  | `--thp=<mode>`         | `never`, `always`, `madvise`    | Control Transparent HugePages usage.             |
| Static HugePages       | `--hugepages=<count>`  | Numeric count                   | Reserve number of 2MB hugepages.                  |
| Drop page caches       | `--drop-caches`        | (flag)                         | Clear page cache, dentries, and inodes before run. |
| Swappiness            | `--swappiness=<value>` | Numeric (0–100)                 | Kernel swap aggressiveness.                        |
| NUMA memory policy    | `--numa-policy=<mode>` | `local`, `interleave`, `preferred=<node>` | NUMA memory allocation policy.              |
| Malloc backend        | `--malloc=<name>`      | `glibc`, `jemalloc`, `tcmalloc` | Use a specific malloc implementation.             |
| Malloc arena max      | `--arena-max=<number>` | Numeric                        | Limit number of malloc arenas.                      |
| Memory locking        | `--mlock-all`          | (flag)                        | Lock all memory to prevent swapping.                |
| OOM kill behavior     | `--oom-kill-task=<0\|1>` | `0` or `1`                    | Kernel OOM killer behavior.                          |

### Disk & I/O Controls

| Feature                | Flag                      | Possible Values                          | Description                                         |
|------------------------|---------------------------|----------------------------------------|---------------------------------------------------|
| I/O scheduler          | `--io-scheduler=<name>`   | `none`, `mq-deadline`, `cfq`, `bfq`   | Set I/O scheduler for block devices.                |
| Flush disk cache       | `--sync-disk`             | (flag)                                | Force sync and drop caches before test.             |
| Mount options          | `--mount-options=<path>:<options>` | e.g., `/mnt/data:noatime,nobarrier` | Remount filesystem with specific options.           |
| Direct I/O usage       | `--use-direct-io`         | (flag)                                | Enable direct I/O bypassing page cache (app-level). |
| Async I/O queue depth  | `--aio-max-nr=<number>`   | Numeric                              | Maximum number of asynchronous I/O requests.        |

### OS & Kernel Tuning

| Feature                | Flag                       | Possible Values                     | Description                                       |
|------------------------|----------------------------|-----------------------------------|-------------------------------------------------|
| IRQ balancing          | `--irqbalance=on\|off`       | `on`, `off`                      | Enable or disable IRQ balancing service.          |
| IRQ affinity isolation | `--isolate-irqs=<cpus>`     | List or range of CPUs             | Pin IRQs to specified CPUs.                        |
| CPU isolation          | `--isolate-cpus=<cpus>`     | List or range of CPUs             | Isolate CPUs from scheduler (kernel boot param). |
| Kernel mitigations     | `--mitigations=on\|off`       | `on`, `off`                      | Enable or disable kernel security mitigations.   |
| Kernel watchdogs       | `--disable-watchdogs`        | (flag)                          | Disable kernel watchdog timers.                    |
| Scheduler tuning      | `--sched-min-granularity=<ms>` | Numeric ms                      | Set kernel scheduler minimum granularity.         |
| File descriptor limits | `--ulimit-n=<number>`        | Numeric                        | Set max open file descriptors.                     |
| Clocksource selection  | `--clocksource=<name>`       | `tsc`, `hpet`, `acpi_pm`       | Set kernel clocksource for timing.                 |

### GPU Controls (If NVIDIA/ROCm support compiled)

| Feature               | Flag                          | Possible Values                      | Description                                      |
|-----------------------|-------------------------------|------------------------------------|------------------------------------------------|
| GPU frequency lock    | `--gpu-freq=lock:<MHz>`        | Numeric frequency in MHz            | Lock GPU core clock frequency.                   |
| GPU memory clock lock | `--gpu-mem-freq=lock:<MHz>`    | Numeric frequency in MHz            | Lock GPU memory clock frequency.                 |
| GPU power cap        | `--gpu-power-limit=<watts>`     | Numeric in Watts                    | Set maximum GPU power consumption.               |
| GPU persistence mode | `--gpu-persistence=on\|off`       | `on`, `off`                       | Keep GPU initialized between runs.               |
| GPU fan speed        | `--gpu-fan=<percentage>%`       | Numeric percentage (e.g., `50%`)  | Fix fan speed to reduce thermal throttling.     |
| GPU driver stats reset | `--gpu-reset-stats`            | (flag)                            | Reset GPU driver statistics/counters.            |

### cgroups Resource Control

| Feature                 | Flag                              | Possible Values                  | Description                                       |
|-------------------------|----------------------------------|--------------------------------|-------------------------------------------------|
| CPU quota (µs per period) | `--cgroup-cpu-quota=<microsecs>`  | Numeric                        | CPU time quota for cgroup.                        |
| CPU period (µs)          | `--cgroup-cpu-period=<microsecs>` | Numeric                        | Scheduling period for quota.                       |
| CPU affinity             | `--cgroup-cpuset=<cpus>`          | List or range of CPUs          | CPUs assigned to cgroup.                           |
| Memory limit             | `--cgroup-mem-limit=<bytes\|human>` | Numeric or human readable (e.g., `4G`) | Max memory usage for cgroup.                |
| Memory swappiness        | `--cgroup-mem-swappiness=<0-100>` | Numeric                       | Swap tendency within cgroup.                       |
| Block IO read limit      | `--cgroup-io-limit-read=<major>:<minor>:<bytes>` | Device major:minor and limit (e.g., `8:0:10MB`) | Throttle read bandwidth.  |
| Block IO write limit     | `--cgroup-io-limit-write=<major>:<minor>:<bytes>` | As above                     | Throttle write bandwidth.                          |
| Freeze/thaw processes    | `--cgroup-freeze=freeze\|thaw`       | `freeze`, `thaw`              | Freeze or thaw processes in cgroup.               |

### Runtime & Usability Options

| Feature               | Flag                          | Possible Values               | Description                                   |
|-----------------------|-------------------------------|------------------------------|-----------------------------------------------|
| Dry-run mode          | `--dry-run`                    | (flag)                      | Print planned changes without applying.       |
| Restore defaults      | `--restore`                    | (flag)                      | Restore system to default settings.           |
| JSON config file      | `--config=<file>`              | File path                   | Load environment settings from JSON profile.  |
| Save current state    | `--save-env=<file>`            | File path                   | Save current environment snapshot.            |
| Profile presets       | `--profile=<name>`             | e.g., `low-noise`, `energy-saving` | Apply predefined setting profiles.           |
| Verbose logging      | `--verbose`                    | (flag)                      | Print detailed logs of actions.                |
| Log output to file    | `--logfile=<path>`             | File path                   | Write logs to a file.                           |

### Example Commands

```bash
# Disable SMT, fix CPU freq to 2800 MHz, set performance governor, drop caches, and disable THP
optkit-setenv --smt=off --cpu-freq=2800 --governor=performance --drop-caches --thp=never

# Limit CPU quota to 50ms per 100ms period, pin to CPUs 0-3, limit memory to 4GB
optkit-setenv --cgroup-cpu-quota=50000 --cgroup-cpu-period=100000 --cgroup-cpuset=0-3 --cgroup-mem-limit=4G

# Lock GPU freq and set power cap
optkit-setenv --gpu-freq=lock:1200 --gpu-power-limit=150 --gpu-persistence=on

# Load configuration from file
optkit-setenv --config ./benchmark-env.json

# Restore system to defaults
optkit-setenv --restore
```

### Requirements

- Root privileges (or `CAP_SYS_ADMIN`) are required for many operations.
- Supported on Linux systems with appropriate kernel interfaces.
- GPU controls require NVIDIA or ROCm drivers and libraries.

</details>

## Key Features

Each feature in OPTKIT is implemented using only a few classes, with numerous monitoring configurations available for each. While users can customize these settings as needed, OPTKIT provides C-style macros with default configurations that cover most common use cases.

OPTKIT uses the `perf_event_open` system call to monitor both PMU (Performance Monitoring Unit) events and RAPL energy metrics without needing additional *root* privileges, provided that global configurations are all set. It relies on:

- `perf_event_open` linux kernel call for both energy and performance monitoring.
- `libpfm4` for PMU-related queries and event code database
- `sysfs` for modifying CPU core frequencies
- `googletest` for comprehensive unit testing
- `spdlog` for advanced logging capabilities
- `bash` and `python3` for various utility tools
- `msr-safe (optional)` library for direct CPU MSR access (low level)

### Architecture Support

- **CPU**: Intel (Broadwell, Skylake, Ice Lake), AMD, ARM
- **GPU**: NVIDIA, AMD ROCm, Intel
- **Monitoring**: CPU, GPU, Disk I/O, Temperature sensors
- **Frequency Control**: Core and uncore frequencies for both CPU and GPU

### Monitoring Capabilities

- **Energy**: RAPL-based energy monitoring for CPU and GPU
- **Performance**: PMU events monitoring with architecture-specific optimizations
- **Disk I/O**: Comprehensive disk performance profiling
- **Temperature**: CPU and GPU temperature monitoring
- **Frequency**: Real-time frequency monitoring and control

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
  
    OPTKIT_INIT();  // it will create json files for each block and prints results there.
    OPTKIT_INIT(false); // it won't create json files, prints std::cout instead
    
    // ****** CPU Energy Monitoring ********* //
    OPTKIT_CPU_ENERGY("block_name"); 
    
    // ****** PMU Event Monitoring ********* //
    OPTKIT_CPU_EVENTS("block_name", optkit::metrics::performance::cpu_metrics::*);
    OPTKIT_CPU_EVENTS("block_name", optkit::metrics::performance::cpu_metrics::*);
    OPTKIT_CPU_BLOCK_EVENTS("block_name", optkit::metrics::performance::cpu_metrics::*);

    // ****** GPU Event Monitoring ********* //
    OPTKIT_GPU_EVENTS("block_name", optkit::metrics::performance::gpu_metrics::*);

    // ****** GPU Energy Monitoring ******** //
    OPTKIT_GPU_ENERGY_EVENTS("block_name");
    OPTKIT_GPU_ENERGY_EVENTS_WITH_METRICS("block_name", optkit::metrics::energy::gpu_metrics::*);

    // ****** DISK Event Monitoring ********* //
    OPTKIT_DISK_EVENTS("block_name");
    OPTKIT_DISK_EVENTS_WITH_METRICS("block_name", optkit::metrics::disk::core_metrics::*);

    // ****** Temperature Monitoring ********* //
    OPTKIT_GPU_TEMPERATURE_EVENTS("block_name");
    OPTKIT_HWMON_TEMPERATURE_EVENTS("block_name"); 
    
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
    

    // ****** Queries ********* //
    optkit::Query::*; 
    optkit::gpu::Query::*; 
    optkit::pmu::cpu::Query::*
    optkit::frequency::cpu::Query::*
    optkit::frequency::gpu::Query::*
}
```

## Directory Structure

Project structure and explanations are given below.

```text
.
├── docs
├── examples
├── lib
│   ├── googletest  
│   ├── libpfm4 
│   └── spdlog 
├── src
│   ├── bindings
│   │   ├── c
│   │   └── python
│   ├── core
│   │   ├── disk
│   │   ├── energy
│   │   │   ├── cpu
│   │   │   └── gpu
│   │   ├── frequency
│   │   │   ├── cpu
│   │   │   └── gpu
│   │   ├── metrics
│   │   │   ├── cpu
│   │   │   ├── disk
│   │   │   ├── gpu
│   │   │   └── temperature
│   │   ├── pmu
│   │   │   ├── cpu
│   │   │   └── gpu
│   │   └── temperature
│   │       ├── gpu
│   │       └── hwmon
│   └── utils
│       ├── deployment
│       ├── logging
│       └── optimizations
├── test
│   ├── common
│   ├── core
│   │   ├── energy
│   │   │   ├── cpu
│   │   │   └── gpu
│   │   ├── frequency
│   │   │   ├── cpu
│   │   │   └── gpu
│   │   ├── metrics
│   │   │   ├── cpu
│   │   │   ├── disk
│   │   │   └── gpu
│   │   └── pmu
│   │       ├── cpu
│   │       └── gpu
│   └── utils
└── tools
    ├── optkit-cli
    └── optkit-setenv
