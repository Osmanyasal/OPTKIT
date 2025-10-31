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

## CLI Tools of The OPTKIT

<details>
<summary><strong>optkit</strong> - Performance and Energy Profiling Tool</summary>

### Features

```bash
OPTKIT - Performance and Energy Profiling & Optimization Tool

USAGE:
    optkit <COMMAND> [OPTIONS] [-- <PROGRAM>]

COMMANDS:
    topology [cpu|gpu]              Show system topology
    list <TYPE> [cpu|gpu]           List available components
    stat [OPTIONS] -- <PROGRAM>     Run single-shot profiling (like perf stat)

TOPOLOGY:
    optkit topology                 Show complete system topology
    optkit topology cpu             Show CPU topology only
    optkit topology gpu             Show GPU topology only

LIST:
    optkit list [all|cpu|gpu]           List all PMU capabilities
    optkit list [all|cpu|gpu] pmu       List available PMU info
    optkit list [all|cpu|gpu] events    List available PMU events
    optkit list [all|cpu|gpu] metrics   List available metrics

PROFILING (stat):
    Single execution profiling - runs program once and collects metrics
    
    optkit stat -- <program>                                 Default profiling
    optkit stat -e <event> -- <program>                      Profile specific event
    optkit stat -m <metric> -- <program>                     Profile specific metric
    optkit stat -e <event> -m <metric> -- <program>          Profile event + metric

BENCHMARKING (--bench):
    Multiple execution analysis - runs program multiple times with different configurations
    
    optkit stat --bench freq-scaling -- <program>            Frequency scaling analysis
    optkit stat --bench core-scaling -- <program>            Core scaling analysis
    optkit stat --affinity <STRATEGY> -- <program>           Affinity analysis

    Options can be interleaved:
    optkit stat --bench freq-scaling -e cycles -m ipc -- <program>

AFFINITY STRATEGIES:
    --affinity compact              Pack threads on fewer cores (cache locality)
    --affinity scatter              Spread threads across cores (avoid contention)
    --affinity numa                 NUMA-aware placement (memory locality)
    --affinity manual               Manual affinity control

EXAMPLES:
    # Topology queries
    optkit topology
    optkit topology cpu
    optkit topology gpu

    # List capabilities
    optkit list all
    optkit list cpu all
    optkit list cpu events
    optkit list cpu metrics
    optkit list gpu all
    optkit list gpu events
    optkit list gpu metrics

    # Single-shot profiling (executes once)
    optkit stat -- ./my_program
    optkit stat -e cycles -e instructions -- ./app
    optkit stat -m ipc -m cache-miss-rate -- ./benchmark

    # Benchmark analysis (executes multiple times)
    optkit stat --bench freq-scaling -- ./compute_heavy
    optkit stat --bench core-scaling -- ./parallel_app
    optkit stat --affinity scatter -- ./threaded_app
    optkit stat --affinity numa -- ./parallel_workload

    # Interleaved options (benchmark + specific profiling)
    optkit stat --bench freq-scaling -e cycles -m ipc -- ./program --input data.txt
    optkit stat -e cache-misses -m energy --bench core-scaling -- ./app
    optkit stat --affinity compact -e instructions -m ipc -- ./multithreaded

NOTE:
    - 'stat' without --bench or --affinity: Single execution, collects specified events/metrics
    - 'stat' with --bench or --affinity: Multiple executions with varying configurations
      (e.g., different frequencies, core counts, affinity patterns)

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
- `sysfs` for modifying CPU core frequencies it is necessary to have access to `/sys/devices/system/cpu/cpu*/cpufreq/**`
- `googletest` for comprehensive unit testing
- `spdlog` for advanced logging capabilities
- `python3` for various utility tools
- `msr-safe (optional)` library for direct CPU MSR access -- please install manually [check how to](https://github.com/LLNL/msr-safe)
  
Each utility is automatically built locally and linked during compilation except msr-safe which requires admin priviledges to load the module and mrs\_allowlist under `/dev/cpu/msr_allowlist`. 

**note:** users also need to update the msr_allowlist, they can find their list under `lib/msr-safe/allowlists`, they need to remove comment. `(0x620 for uncore frequency in intel cpus)`

### Architecture Support

- **CPU**: Intel, AMD, ARM
- **GPU**: NVIDIA, AMD

### Monitoring Capabilities

- **Energy**: RAPL-based energy monitoring for CPU and GPU
- **Performance**: PMU events monitoring with architecture-specific optimizations
- **Disk I/O**: Comprehensive disk performance profiling
- **Temperature**: CPU and GPU temperature monitoring
- **Frequency**: Real-time frequency monitoring and control
- **Memory**: Memory info

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

<details>
<summary><strong>📊 Supported Metrics in OPTKIT</strong></summary>

This document lists all currently supported performance and energy metrics in **OptKit**.  
Each metric is implemented via a `MetricBuilder<T>` function and categorized by domain. Althoguh we support many important metrics, users of the library are free to implement their own metrics and pass to a Profiler.

```cpp
// ✅ Example: MetricBuilder to build IPC metric for intel architectures.
static const MetricBuilder<uint64_t> &ipc()
{
    static const MetricBuilder<uint64_t> metric = []
    {
        std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
        std::string unhalted_core_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);

        return MetricBuilder<uint64_t>{}
            .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED)) // event_name -- event_code
            .add(unhalted_core_cycles_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
            .build("ipc", [inst_retired_name, unhalted_core_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                    {
                    uint64_t inst_retired = get_event_count(counts,inst_retired_name);
                    uint64_t unhalted_core_cycles = get_event_count(counts,unhalted_core_cycles_name);

                    if (unhalted_core_cycles == 0)
                            return std::numeric_limits<double>::quiet_NaN();
                    return static_cast<double>(inst_retired) / static_cast<double>(unhalted_core_cycles); });
    }();
    return metric;
}
```

---

### CPU Utilization

| Metric | Description |
|--------|--------------|
| **cpu_max_capacity_based_utilization** | `100 * (UNHALTED_CLK_CYCLES / (OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS * max_freq_khz * 1000 * duration_sec))` — CPU utilization normalized by maximum frequency and logical cores. |

---

### Cache Metrics

| Metric | Description |
|--------|--------------|
| **l1_mpki** | `1000 * L1_MISSES / INST_RETIRED` — L1 cache true misses per kilo instruction. |
| **l2_mpki** | `1000 * L2_MISSES / INST_RETIRED` — L2 cache true misses per kilo instruction. |
| **l3_mpki** | `1000 * L3_MISSES / INST_RETIRED` — L3 cache true misses per kilo instruction. |
| **l1_hit_ratio** | `100 * (L1_CACHE_ACCESSES - L1_MISSES) / L1_CACHE_ACCESSES` — L1 cache hit ratio. |
| **l2_hit_ratio** | `100 * (L2_CACHE_ACCESSES - L2_MISSES) / L2_CACHE_ACCESSES` — L2 cache hit ratio. |
| **l3_hit_ratio** | `100 * (L3_CACHE_ACCESSES - L3_MISSES) / L3_CACHE_ACCESSES` — L3 cache hit ratio. |

---

### Branch Metrics

| Metric | Description |
|--------|--------------|
| **branch_mispr_ratio** | `BR_MISP_RETIRED.ALL_BRANCHES / BR_INST_RETIRED.ALL_BRANCHES` — Ratio of all branches that mispredict. |

---

### TLB Metrics

| Metric | Description |
|--------|--------------|
| **itlb_mpki** | `1000 * ITLB_MISSES.WALK_COMPLETED / INST_RETIRED` — ITLB miss per kilo instruction. |
| **dtlb_mpki** | `1000 * DTLB_MISSES.WALK_COMPLETED / INST_RETIRED` — DTLB miss per kilo instruction. |
| **tlb_mpki** | `1000 * TLB_MISSES.WALK_COMPLETED / INST_RETIRED` — Combined TLB MPKI. |

---

### Latency & Parallelism

| Metric | Description |
|--------|--------------|
| **load_miss_latency** | `L1D_PEND_MISS.PENDING / MEM_LD_COMPLETED.L1_MISS_ANY` — Average latency for L1 D-cache miss demand load operations (in core cycles). |
| **ilp** | `UOPS_EXECUTED.THREAD / ((is_smt_enabled? 2 : 1 ) * UOPS_EXECUTED.CORE_CYCLES_GE1)` — Instruction-level parallelism per core. |
| **mlp** | `L1D_PEND_MISS.PENDING / L1D_PEND_MISS.PENDING_CYCLES` — Memory-level parallelism per thread. |

---

### DRAM Bandwidth

| Metric | Description |
|--------|--------------|
| **dram_bandwidth_gbs** | `(64 * (RD + WR)) / (Time * 1GB)` — DRAM bandwidth in GB/s. |

---

### Instruction-per-Event Metrics

| Metric | Description |
|--------|--------------|
| **ipc** | `INST_RETIRED / UNHALTED_CLK_CYCLES` — Instructions per cycle. |
| **ip_call** | `INST_RETIRED / BR_INST_RETIRED.NEAR_CALL` — Instructions per near call. |
| **ip_branch** | `INST_RETIRED / BR_INST_RETIRED.ALL_BRANCHES` — Instructions per branch. |
| **ip_mem_load** | `INST_RETIRED / MEM_INST_RETIRED.ALL_LOADS_PS` — Instructions per memory load. |
| **ip_mem_store** | `INST_RETIRED / MEM_INST_RETIRED.ALL_STORES_PS` — Instructions per memory store. |
| **ip_mispredict** | `INST_RETIRED / BR_MISP_RETIRED.ALL_BRANCHES` — Instructions per misprediction. |

---

### Floating-Point & Vector Metrics

| Metric | Description |
|--------|--------------|
| **ip_flop** | Instructions per FP operation. |
| **ip_avx_any_flop** | Instructions per vector floating point operation. |
| **gflops** | GFLOPs per second — floating point performance. |
| **ai** | FLOP/Byte — arithmetic intensity. |
| **ip_arith_scalar_sp** | Instructions per scalar single-precision FP op. |
| **ip_arith_scalar_dp** | Instructions per scalar double-precision FP op. |
| **ip_arith_avx128** | Instructions per 128-bit vector FP op. |
| **ip_arith_avx256** | Instructions per 256-bit vector FP op. |
| **ip_arith_avx512** | Instructions per 512-bit vector FP op. |
| **ip_arith_vector_any** | Instructions per vector FP op (any width). |
| **scalarp_arith_vector** | Scalar FP per vector FP operation. |

---

### Software Prefetch

| Metric | Description |
|--------|--------------|
| **ip_swpf** | `INST_RETIRED / SW_PREFETCH_ACCESS.T0:u0xF` — Instructions per software prefetch. |

---

### Topdown (Pipeline Utilization) — Level 1

| Metric | Description |
|--------|--------------|
| **frontend_bound** | Fraction of slots not delivered by frontend. |
| **bad_speculation** | Fraction of slots wasted due to speculation. |
| **backend_bound** | Fraction of slots where backend could not accept µops. |
| **retiring** | Fraction of slots retired successfully. |
| **smt_contention** | Fraction of unused dispatch slots due to SMT contention. |

---

### Topdown — Level 2

| Metric | Description |
|--------|--------------|
| **frontend_bound_latency** | Portion of FrontendBound due to cache/TLB latency. |
| **frontend_bound_bw** | Portion of FrontendBound due to decode/queue bandwidth limits. |
| **bad_speculation_mispredicts** | Portion of BadSpeculation from branch mispredicts. |
| **bad_speculation_pipeline_restarts** | Portion of BadSpeculation from pipeline clears. |
| **backend_bound_memory** | Portion of BackendBound due to memory issues. |
| **backend_bound_cpu** | Portion of BackendBound due to core (non-memory) issues. |
| **retiring_fastpath** | Portion of Retiring serviced via fast-path execution. |
| **retiring_microcode** | Portion of Retiring from microcode or complex assists. |

---

### Aggregated & Composite Metrics

| Metric | Description |
|--------|--------------|
| **all_mpki** | Combined MPKI across all cache levels. |
| **all_cache_hit_ratio** | Combined cache hit ratios. |
| **all_stlb_mpki** | Aggregated STLB MPKI. |
| **all_latency_and_parallelism** | Combined latency and parallelism metrics. |
| **all_dram_bandwidth** | Combined DRAM bandwidth metrics. |
| **all_ip_metrics** | Combined instruction-per-event metrics. |
| **all_branch_metrics** | Combined branch metrics. |
| **carm** | Cache-Aware Roofline Model metric set. |
| **topdown_l1** | Aggregated L1 topdown metrics. |
| **topdown_l2_fe** | L2 frontend metrics. |
| **topdown_l2_be** | L2 backend metrics. |
| **topdown_l2_retiring** | L2 retiring subset. |
| **topdown_l2_bad_spec** | L2 bad speculation subset. |
| **topdown_l2** | Full L2 topdown view. |
| **all_topdown** | Combined Topdown (L1 + L2). |
| **all_metrics** | Full OptKit metric set (all categories). |

---
</details>