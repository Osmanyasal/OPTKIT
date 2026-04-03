# RISC-V Perf Example

This example shows how to use the RISC-V perf helper configs with `BlockProfiler`.

It demonstrates two cases:

- `instructions` using `instructions_profiler_config(...)`
- `LLC-load-misses` and `LLC-store-misses` using `llc_load_misses_profiler_config(...)`

## Build

From the repository root, build the static library first:

```bash
make -j$(nproc) config=release optkit_static
```

Then build the example:

```bash
cd examples/riscv_perf
make
```

## Run

```bash
./riscv_perf_example
```

## Notes

The current `BlockProfiler` API still needs a `MetricBuilder` with event codes. The RISC-V config helper supplies the correct `perf_event_attr` fields such as:

- event `type`
- default flags like `disabled`, `inherit`, `enable_on_exec`
- event-specific filters like `exclude_kernel` and `exclude_hv` where applicable

For system-wide per-core monitoring, pass:

```cpp
pid = -1;
cpu = OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS;
```

That causes the underlying perf profiler to open one event fd per core.