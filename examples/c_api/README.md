# OPTKIT C API Examples

This directory contains examples demonstrating how to use the OPTKIT C wrapper API.

## Building

```bash
make
```

This builds all example programs. The Makefile assumes OPTKIT libraries are in `../../bin/Release`.

## Examples

### Basic Usage

- **main.c** - Basic demo showing initialization, system queries, and disk profiling
- **query_cpu.c** - CPU-related queries (system info, PMU, RAPL)
- **query_gpu.c** - GPU device queries (NVIDIA/AMD)

### Profiling

- **perf.c** - Performance monitoring with hardware counters
- **energy_cpu.c** - CPU energy measurement using RAPL
- **disk.c** - Disk I/O profiling
- **temperature.c** - Temperature monitoring (hwmon and GPU)

## Running Examples

After building, run with:

```bash
make run         # Runs the main demo
./perf           # Performance profiling
./query_cpu      # CPU queries
./query_gpu      # GPU queries
./energy_cpu     # CPU energy
./disk           # Disk I/O
./temperature    # Temperature monitoring
```

**Note:** Some examples require:
- Root privileges or relaxed perf_event_paranoid for performance counters
- NVIDIA/AMD drivers for GPU examples
- RAPL support for energy measurements

## API Usage Patterns

### Initialization

```c
int8_t is_init;
optkit_is_initialized(&is_init);
if (!is_init) {
    optkit_init(1, "my_program");  // 1 = create output folder
}
```

### Error Handling

```c
if (status != OPTKIT_STATUS_OK) {
    const char *err;
    optkit_last_error_message(&err);
    fprintf(stderr, "Error: %s\n", err);
}
```

### Performance Profiling

```c
const char *metrics[] = {"IPC", "CPI", "L1D_MISS_RATE"};
optkit_perf_start("block_name", metrics, 3, NULL, 0);
// ... code to profile ...
optkit_perf_stop();
```

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

## Available Metrics

Common performance metrics:
- `IPC` - Instructions per cycle
- `CPI` - Cycles per instruction
- `L1D_MISS_RATE` - L1 data cache miss rate
- `L2_MISS_RATE` - L2 cache miss rate
- `BRANCH_MISS_RATE` - Branch misprediction rate
- `CARM` - Cache-aware roofline model metrics

For custom hardware events, see PMU documentation or use `query_cpu` example to list available events.
