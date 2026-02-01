# OPTKIT C API (optkit_c)

Practical, copy/paste-oriented cheat sheet for the OPTKIT C wrapper API.

The C API is declared in `src/optkit_c.h` and is designed to be C ABI-friendly: most functions return `optkit_status_t` and write results via output parameters.

## Build + run

Build all examples:

```bash
make
```

Run an example (ensure shared libraries can be found):

```bash
LD_LIBRARY_PATH=../../bin/Release:$LD_LIBRARY_PATH ./query_cpu
```

The Makefile also provides:

```bash
make run
```

## Example programs

One feature per file:

- `main.c`: init + a couple system queries + disk scope
- `query_cpu.c`: system + PMU + RAPL queries
- `query_gpu.c`: NVIDIA/AMD basic queries (best-effort)
- `perf.c`: perf/PMU profiling (`optkit_perf_start/stop`)
- `energy_cpu.c`: CPU energy profiling (RAPL)
- `disk.c`: disk profiling scope
- `temperature.c`: hwmon + GPU temperature profiling

## Engine lifecycle

```c
#include <stdio.h>
#include <stdint.h>

#include "../../src/optkit_c.h"

static int optkit_init_or_die(const char *execution_file)
{
    if (optkit_init(/*create_folder=*/1, execution_file) != OPTKIT_STATUS_OK)
    {
        const char *err = NULL;
        optkit_last_error_message(&err);
        fprintf(stderr, "optkit_init failed: %s\n", err ? err : "(null)");
        return 1;
    }
    return 0;
}
```

Shutdown:

```c
optkit_finalize();
```

## Error handling

Most functions return `optkit_status_t`. On failure, query the thread-local last error message:

```c
optkit_status_t st = /* ... */;
if (st != OPTKIT_STATUS_OK)
{
    const char *err = NULL;
    optkit_last_error_message(&err);
    fprintf(stderr, "OPTKIT error: %s\n", err ? err : "(null)");
}
```

## Profilers (start/stop)

All profilers use stack semantics: each `*_start()` pushes a new profiler instance; `*_stop()` pops the most recent one.

### Perf (CPU PMU)

```c
const char *metrics[] = {"carm", "ipc"};

/* For an empty events list, pass NULL and events_count=0. */
optkit_perf_start("block", metrics, 2, /*events=*/NULL, /*events_count=*/0);
/* ... workload ... */
optkit_perf_stop();
```

One-liner (C99 compound literals):

```c
optkit_perf_start("block",
                 (const char*[]){"carm", "ipc"}, 2,
                 (const char*[]){"L1_MISSES"}, 1);
```

Tip: use `optkit-cli list cpu` to discover metric/event names.

### Energy

CPU only:

```c
optkit_energy_cpu_start("cpu_energy");
/* ... workload ... */
optkit_energy_cpu_stop();
```

All energy backends (best-effort):

```c
optkit_energy_start("energy_all");
/* ... workload ... */
optkit_energy_stop();
```

### Disk

```c
optkit_disk_start("disk_io");
/* ... workload ... */
optkit_disk_stop();
```

### Temperature

HWMON only:

```c
optkit_temperature_hwmon_start("temp_hwmon");
/* ... workload ... */
optkit_temperature_hwmon_stop();
```

GPU temperature profiling (best-effort; vendor must be initialized):

```c
optkit_gpu_vendor_t vendor = OPTKIT_GPU_VENDOR_NVIDIA;
if (optkit_query_gpu_init(vendor) == OPTKIT_STATUS_OK)
{
    optkit_temperature_gpu_start("temp_gpu");
    /* ... */
    optkit_temperature_gpu_stop();
    optkit_query_gpu_shutdown(vendor);
}
```

## Query APIs

### System / CPU

```c
int16_t sockets = 0;
optkit_query_system_num_sockets(&sockets);

int8_t smt = 0;
optkit_query_system_is_smt_enabled(&smt);

int32_t paranoid = 0;
optkit_query_system_paranoid(&paranoid);
```

`*_str` functions return human-readable strings:

```c
#include <stdlib.h>

char *packages = NULL;
if (optkit_query_system_detect_cpu_packages_str(&packages) == OPTKIT_STATUS_OK)
{
    puts(packages);
    free(packages);
}
```

### PMU (libpfm4)

`optkit_query_pmu_avail_pmu_ids` allocates an output array:

```c
#include <stdlib.h>

int32_t *pmu_ids = NULL;
size_t pmu_count = 0;
if (optkit_query_pmu_avail_pmu_ids(&pmu_ids, &pmu_count) == OPTKIT_STATUS_OK)
{
    /* ... use pmu_ids[0..pmu_count-1] ... */
    free(pmu_ids);
}
```

### RAPL

Availability checks use out-params:

```c
int32_t methods = 0;
optkit_query_rapl_avail_read_methods(&methods);

int8_t avail = 0;
optkit_query_rapl_is_perf_avail(&avail);
optkit_query_rapl_is_sysfs_avail(&avail);
optkit_query_rapl_is_msr_avail(&avail);
```

Domain info as a string:

```c
char *rapl_info = NULL;
if (optkit_query_rapl_domain_info_str(&rapl_info) == OPTKIT_STATUS_OK)
{
    puts(rapl_info);
    free(rapl_info);
}
```

### GPU

Basic pattern:

```c
#include <stdlib.h>

optkit_gpu_vendor_t vendor = OPTKIT_GPU_VENDOR_NVIDIA;
if (optkit_query_gpu_init(vendor) == OPTKIT_STATUS_OK)
{
    int8_t exists = 0;
    optkit_query_gpu_is_device_exists(vendor, &exists);

    if (exists)
    {
        uint32_t count = 0;
        optkit_query_gpu_get_device_count(vendor, &count);

        if (count > 0)
        {
            char *name = NULL;
            if (optkit_query_gpu_get_device_name(vendor, 0, &name) == OPTKIT_STATUS_OK)
            {
                puts(name);
                free(name);
            }
        }
    }

    optkit_query_gpu_shutdown(vendor);
}
```

## Memory ownership

If a C API function returns a heap-allocated pointer (e.g. `*_str(&out_str)` or `optkit_query_pmu_avail_pmu_ids(&out_ids, &out_count)`), free it with `free()`.

## Permissions / prerequisites

- PERF counters often require root or a permissive `perf_event_paranoid`.
- GPU queries require vendor drivers and permissions.
- RAPL availability depends on platform/kernel configuration.

### Memory Management

Strings returned by OPTKIT must be freed:

```c
char *info = NULL;
if (optkit_query_system_detect_cpu_packages_str(&info) == OPTKIT_STATUS_OK) {
    printf("%s\n", info);
    optkit_free(info);  // Important!
}
```

### Cleanup

```c
optkit_finalize();
```


