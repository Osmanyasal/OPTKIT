# CARM Workload (C++)

A minimal C++ example that runs a FLOPS-heavy workload and collects the `carm` metric set using OPTKIT's perf-based block profiler.

## Build
1) From the repo root build the static library if you have not already:
```bash
make -j$(nproc) config=release optkit_static
```
2) Build this example:
```bash
cd examples/carm_workload
make -j$(nproc)
```

## Run
```bash
./carm_workload
```
This creates `perf_smoke_cpp/carm_cpp_block__cpu_pmu.json` with the readings.

## Notes
- Sampling is disabled (`is_sampling=false`) so the duration matches the workload runtime.
- The workload uses a naive matrix multiply to generate floating-point operations without extra dependencies.
