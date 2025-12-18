## Overview

`optkit-setenv` is a comprehensive system environment configuration tool designed to set, tune, and isolate system parameters for reproducible, low-noise, and energy-aware benchmarking and profiling. It integrates closely with OptKit's profiling ecosystem but can be used standalone to control CPU, memory, disk, OS, GPU, and cgroup resources.

Users can either set values directly through CLI parameters or run `optkit-setenv init` to generate an empty configuration file with all adjustable fields. After modifying that file, they simply load it. Examples are shown below.


### Example Commands

```bash
# sudo is a must
sudo su

# backup current system settings (recommended)
optkit-setenv --backup

# create an empty env.json file
optkit-setenv --init

# Load configuration from file (after changes)
optkit-setenv ./env.json

# Execute script (optionally created for LD_PRELOAD settings)
./optkit_execute_me.sh

## Execute your program here!!
./<your_program>

# Restore system to defaults (backup file is restored as current settings)
optkit-setenv --restore

exit
```

### Requirements

- Root privileges (or `CAP_SYS_ADMIN`) is required.
- Supported on Linux systems with appropriate kernel interfaces.
- GPU controls require NVIDIA or ROCm drivers and libraries.

 

### CPU Controls

| Feature              | Flag                  | Possible Values                     | Description                                   |
|----------------------|-----------------------|-----------------------------------|-----------------------------------------------|
| SMT (Hyperthreading) | `smt=on\|off`        | `on`, `off`                       | Enable or disable simultaneous multithreading. |
| CPU frequency         | `cpu-freq=<MHz>`     | Numeric frequency (e.g., `2800`) | Set fixed CPU core frequency in MHz.           |
| Per-core frequency    | `cpu-freq-cores=0:2800,1:2800` | List of `core:freq` pairs          | Set frequencies individually per core.         |
| CPU governor          | `governor=<name>`    | `performance`, `powersave`, `userspace`, `ondemand`, `schedutil` | CPU frequency scaling governor. |
| Turbo Boost           | `turbo=on\|off`       | `on`, `off`                       | Enable or disable Intel Turbo Boost.           |
| Uncore frequency      | `uncore-freq=<MHz>`  | Numeric frequency                  | Set uncore/cache frequency (Intel-specific).  |
| C-states control      | `cstates=on\|off`     | `on`, `off`                       | Enable or disable CPU idle states.             |
| CPU core online/offline | `offline-cores=1,3` | Comma-separated core IDs           | Take specific cores offline to reduce noise.  |
| CPU affinity          | `affinity=0-3`       | List or range of CPU cores         | Restrict benchmark processes to specified cores. |
| P-State limits        | `pstate-min=<%> --pstate-max=<%>` | Numeric percent (e.g., 80, 100) | Limit CPU P-State performance range.           |

### Memory & NUMA Controls

| Feature               | Flag                   | Possible Values                   | Description                                       |
|-----------------------|------------------------|---------------------------------|-------------------------------------------------|
| Transparent HugePages  | `thp=<mode>`         | `never`, `always`, `madvise`    | Control Transparent HugePages usage.             |
| Static HugePages       | `hugepages=<count>`  | Numeric count                   | Reserve number of 2MB hugepages.                  |
| Drop page caches       | `drop-caches`        | (flag)                         | Clear page cache, dentries, and inodes before run. |
| Swappiness            | `swappiness=<value>` | Numeric (0–100)                 | Kernel swap aggressiveness.                        |
| NUMA memory policy    | `numa-policy=<mode>` | `local`, `interleave`, `preferred=<node>` | NUMA memory allocation policy.              |
| Malloc backend        | `malloc=<name>`      | `glibc`, `jemalloc`, `tcmalloc` | Use a specific malloc implementation.             |
| Malloc arena max      | `arena-max=<number>` | Numeric                        | Limit number of malloc arenas.                      |
| Memory locking        | `mlock-all`          | (flag)                        | Lock all memory to prevent swapping.                |
| OOM kill behavior     | `oom-kill-task=<0\|1>` | `0` or `1`                    | Kernel OOM killer behavior.                          |

### Disk & I/O Controls

| Feature                | Flag                      | Possible Values                          | Description                                         |
|------------------------|---------------------------|----------------------------------------|---------------------------------------------------|
| I/O scheduler          | `io-scheduler=<name>`   | `none`, `mq-deadline`, `cfq`, `bfq`   | Set I/O scheduler for block devices.                |
| Flush disk cache       | `sync-disk`             | (flag)                                | Force sync and drop caches before test.             |
| Mount options          | `mount-options=<path>:<options>` | e.g., `/mnt/data:noatime,nobarrier` | Remount filesystem with specific options.           |
| Direct I/O usage       | `use-direct-io`         | (flag)                                | Enable direct I/O bypassing page cache (app-level). |
| Async I/O queue depth  | `aio-max-nr=<number>`   | Numeric                              | Maximum number of asynchronous I/O requests.        |

### OS & Kernel Tuning

| Feature                | Flag                       | Possible Values                     | Description                                       |
|------------------------|----------------------------|-----------------------------------|-------------------------------------------------|
| IRQ balancing          | `irqbalance=on\|off`       | `on`, `off`                      | Enable or disable IRQ balancing service.          |
| IRQ affinity isolation | `isolate-irqs=<cpus>`     | List or range of CPUs             | Pin IRQs to specified CPUs.                        |
| CPU isolation          | `isolate-cpus=<cpus>`     | List or range of CPUs             | Isolate CPUs from scheduler (kernel boot param). |
| Kernel mitigations     | `mitigations=on\|off`       | `on`, `off`                      | Enable or disable kernel security mitigations.   |
| Kernel watchdogs       | `disable-watchdogs`        | (flag)                          | Disable kernel watchdog timers.                    |
| Scheduler tuning      | `sched-min-granularity=<ms>` | Numeric ms                      | Set kernel scheduler minimum granularity.         |
| File descriptor limits | `ulimit-n=<number>`        | Numeric                        | Set max open file descriptors.                     |
| Clocksource selection  | `clocksource=<name>`       | `tsc`, `hpet`, `acpi_pm`       | Set kernel clocksource for timing.                 |

### GPU Controls (If NVIDIA/ROCm support compiled)

| Feature               | Flag                          | Possible Values                      | Description                                      |
|-----------------------|-------------------------------|------------------------------------|------------------------------------------------|
| GPU frequency lock    | `gpu-freq=lock:<MHz>`        | Numeric frequency in MHz            | Lock GPU core clock frequency.                   |
| GPU memory clock lock | `gpu-mem-freq=lock:<MHz>`    | Numeric frequency in MHz            | Lock GPU memory clock frequency.                 |
| GPU power cap        | `gpu-power-limit=<watts>`     | Numeric in Watts                    | Set maximum GPU power consumption.               |
| GPU persistence mode | `gpu-persistence=on\|off`       | `on`, `off`                       | Keep GPU initialized between runs.               |
| GPU fan speed        | `gpu-fan=<percentage>%`       | Numeric percentage (e.g., `50%`)  | Fix fan speed to reduce thermal throttling.     |
| GPU driver stats reset | `gpu-reset-stats`            | (flag)                            | Reset GPU driver statistics/counters.            |

### cgroups Resource Control

| Feature                 | Flag                              | Possible Values                  | Description                                       |
|-------------------------|----------------------------------|--------------------------------|-------------------------------------------------|
| CPU quota (µs per period) | `cgroup-cpu-quota=<microsecs>`  | Numeric                        | CPU time quota for cgroup.                        |
| CPU period (µs)          | `cgroup-cpu-period=<microsecs>` | Numeric                        | Scheduling period for quota.                       |
| CPU affinity             | `cgroup-cpuset=<cpus>`          | List or range of CPUs          | CPUs assigned to cgroup.                           |
| Memory limit             | `cgroup-mem-limit=<bytes\|human>` | Numeric or human readable (e.g., `4G`) | Max memory usage for cgroup.                |
| Memory swappiness        | `cgroup-mem-swappiness=<0-100>` | Numeric                       | Swap tendency within cgroup.                       |
| Block IO read limit      | `cgroup-io-limit-read=<major>:<minor>:<bytes>` | Device major:minor and limit (e.g., `8:0:10MB`) | Throttle read bandwidth.  |
| Block IO write limit     | `cgroup-io-limit-write=<major>:<minor>:<bytes>` | As above                     | Throttle write bandwidth.                          |
| Freeze/thaw processes    | `cgroup-freeze=freeze\|thaw`       | `freeze`, `thaw`              | Freeze or thaw processes in cgroup.               |

### Runtime & Usability Options

| Feature               | Flag                          | Possible Values               | Description                                   |
|-----------------------|-------------------------------|------------------------------|-----------------------------------------------|
| Dry-run mode          | `dry-run`                    | (flag)                      | Print planned changes without applying.       |
| Restore defaults      | `restore`                    | (flag)                      | Restore system to default settings.           |
| JSON config file      | `config=<file>`              | File path                   | Load environment settings from JSON profile.  |
| Save current state    | `save-env=<file>`            | File path                   | Save current environment snapshot.            |
| Profile presets       | `profile=<name>`             | e.g., `low-noise`, `energy-saving` | Apply predefined setting profiles.           |
| Verbose logging      | `verbose`                    | (flag)                      | Print detailed logs of actions.                |
| Log output to file    | `logfile=<path>`             | File path                   | Write logs to a file.                           |

</details>
