# RISC-V Perf Example

This example shows how to use the shared CPU perf interface on RISC-V.

It demonstrates two cases:

- `INST_RETIRED` through the same `OPTKIT_CPU_EVENTS(...)` macro used on other CPU backends
- `LLC-load-misses` and `LLC-store-misses` through that same macro, with the RISC-V perf resolver selecting the correct perf event attributes internally

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

The current `BlockProfiler` API still consumes a `MetricBuilder` with event codes. On RISC-V, the shared perf layer now resolves those event names and codes to the right `perf_event_attr` fields such as:

- event `type`
- default flags like `disabled`, `inherit`, `enable_on_exec`
- event-specific filters like `exclude_kernel` and `exclude_hv` where applicable

The example intentionally uses the public `cpu_metrics` interface instead of RISC-V-only profiler config helpers.
