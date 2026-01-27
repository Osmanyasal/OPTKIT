# OPTKIT Python API (optkit_py)

This is a practical, copy/paste-oriented cheat sheet for the `optkit_py` Python extension.

## Import

If you built OPTKIT and the extension lands under `bin/Release`, one simple way is:

```py
import sys
sys.path.append("../../bin/Release")
import optkit_py
```

## Example scripts

This folder includes runnable scripts (one feature per file). After you build `optkit_py` into `bin/Release`, run them from `examples/python/`:

```bash
cd examples/python

# Query-only smoke tests first (lowest privilege / best-effort)
python3 query_cpu.py
python3 query_gpu.py

# Profilers (may require kernel permissions / hardware support)
python3 perf.py
python3 callstack.py
python3 energy_cpu.py
python3 energy_gpu.py
python3 disk.py
python3 temperature_hwmon.py
python3 temperature_gpu.py

# Frequency (query-only by default)
python3 frequency.py
```

Supporting files:

- `workload.py`: shared workload helpers
- `test_perf.py`: older template / scratchpad (kept for reference)

## Engine lifecycle

```py
optkit_py.init(create_folder=True, execution_file="my_run")
# ... do work / start-stop profilers ...
optkit_py.finalize()
```

## Profiling modules (start/stop)

All profilers follow **stack** semantics: each `start()` pushes a new profiler instance; `stop()` pops the most recent one.

### Perf (CPU PMU)

```py
optkit_py.perf.start("block", metrics=["carm"], events=[])
# ... workload ...
optkit_py.perf.stop()
```

### Callstack

```py
optkit_py.callstack.start("callstack")
# ... workload ...
optkit_py.callstack.stop()
```

### Energy

`energy.start()` is best-effort: it always starts CPU RAPL, and tries NVIDIA + AMD GPU energy (prints a warning if unavailable).

```py
optkit_py.energy.start("energy_all")
# ... workload ...
optkit_py.energy.stop()
```

CPU only:

```py
optkit_py.energy.cpu.start("cpu_energy")
# ... workload ...
optkit_py.energy.cpu.stop()
```

GPU only (best-effort NVIDIA + AMD):

```py
optkit_py.energy.gpu.start("gpu_energy")
# ... workload ...
optkit_py.energy.gpu.stop()
```

### Disk I/O

```py
optkit_py.disk.start("disk_io")
# ... workload ...
optkit_py.disk.stop()
```

### Temperature

`temperature.start()` is best-effort: it starts HWMON and tries GPU temperature.

```py
optkit_py.temperature.start("temp_all")
# ... workload ...
optkit_py.temperature.stop()
```

HWMON only:

```py
optkit_py.temperature.hwmon.start("temp_hwmon")
# ... workload ...
optkit_py.temperature.hwmon.stop()
```

GPU only:

```py
optkit_py.temperature.gpu.start("temp_gpu")
# ... workload ...
optkit_py.temperature.gpu.stop()
```

## Frequency

### Units + conversion

```py
optkit_py.frequency.convert("2400 MHz", optkit_py.frequency.Unit.KHz)
```

Available units:

- `optkit_py.frequency.Unit.Hz`
- `optkit_py.frequency.Unit.KHz`
- `optkit_py.frequency.Unit.MHz`
- `optkit_py.frequency.Unit.GHz`

### Query (typically unprivileged)

```py
optkit_py.frequency.cpu.query.available_governors(core=0)
optkit_py.frequency.cpu.query.get_governor(core=0)
optkit_py.frequency.cpu.query.get_scaling_driver(core=0)

optkit_py.frequency.cpu.query.available_core_frequencies(core=0, step_khz=200000)

optkit_py.frequency.cpu.query.get_bios_limit(core=0)
optkit_py.frequency.cpu.query.get_scaling_min_limit(core=0)
optkit_py.frequency.cpu.query.get_scaling_max_limit(core=0)

optkit_py.frequency.cpu.query.get_cpuinfo_min_freq(core=0)
optkit_py.frequency.cpu.query.get_cpuinfo_max_freq(core=0)
```

### Setters (often require root / CAP_SYS_ADMIN)

```py
# Governor helpers
optkit_py.frequency.cpu.query.set_governor("performance", socket=0)
optkit_py.frequency.cpu.query.set_governor_percore("performance", core=0)

# Core frequency (kHz)
optkit_py.frequency.cpu.set_core_frequency(freq_khz=2400000, socket=0)
optkit_py.frequency.cpu.set_core_frequency_core(freq_khz=2400000, cpu=0, socket=0)
optkit_py.frequency.cpu.set_core_frequency_range(freq_khz=2400000, cpu_start=0, cpu_end=7, socket=0)
optkit_py.frequency.cpu.reset_core_frequency(socket=0)

# Uncore frequency (kHz)
optkit_py.frequency.cpu.set_uncore_frequency(freq_khz=2400000, socket=0)
optkit_py.frequency.cpu.get_uncore_frequency(socket=0)
optkit_py.frequency.cpu.reset_uncore_frequency(socket=0)
optkit_py.frequency.cpu.get_uncore_min_max(socket=0)
optkit_py.frequency.cpu.get_scaling_available_uncore_frequencies(socket=0, step_khz=200000)
```

## Query APIs

These are exported under `optkit_py.query.*`.

Important: low-level lifecycle methods (like init/destroy) are **not** exposed. Where needed, the bindings lazily initialize internal backends on first use.

### System/CPU: `optkit_py.query.system.Query`

```py
optkit_py.query.system.Query.num_sockets
optkit_py.query.system.Query.num_logical_cores
optkit_py.query.system.Query.is_root_priv_enabled

optkit_py.query.system.Query.paranoid()
optkit_py.query.system.Query.is_smt_enabled()
optkit_py.query.system.Query.is_turbo_enabled()
optkit_py.query.system.Query.detect_cpu_packages()
```

### PMU (libpfm4): `optkit_py.query.pmu.Query`

These helpers return strings where the underlying library uses complex C structs.

```py
pmu_ids = optkit_py.query.pmu.Query.avail_pmu_ids()

# Human-readable strings
optkit_py.query.pmu.Query.default_pmu_info_str()
optkit_py.query.pmu.Query.pmu_info_str(pmu_ids[0])

events = optkit_py.query.pmu.Query.get_avail_events(pmu_ids[0])

# A single event detail as a formatted string
optkit_py.query.pmu.Query.event_detail_str(pmu_ids[0], event_code=0)
```

### RAPL: `optkit_py.query.rapl.Query`

Exported types:

- `optkit_py.query.rapl.RaplDomain`
- `optkit_py.query.rapl.RaplReadMethods`
- `optkit_py.query.rapl.RaplDomainInfo`

Queries:

```py
optkit_py.query.rapl.Query.avail_rapl_read_methods()
optkit_py.query.rapl.Query.is_rapl_perf_avail()
optkit_py.query.rapl.Query.is_rapl_sysfs_avail()
optkit_py.query.rapl.Query.is_rapl_msr_avail()

infos = optkit_py.query.rapl.Query.rapl_domain_info()  # list[RaplDomainInfo]
```

### GPU: `optkit_py.query.gpu.Query`

Exported types:

- `optkit_py.query.gpu.GpuVendor`
- `optkit_py.query.gpu.GpuBasicInfo`
- `optkit_py.query.gpu.GpuVersionInfo`
- `optkit_py.query.gpu.GpuComputeInfo`
- `optkit_py.query.gpu.GpuMemoryInfo`
- `optkit_py.query.gpu.GpuClockInfo`
- `optkit_py.query.gpu.GpuPowerInfo`
- `optkit_py.query.gpu.GpuTemperatureInfo`
- `optkit_py.query.gpu.GpuUtilizationInfo`
- `optkit_py.query.gpu.GpuHardwareInfo`
- `optkit_py.query.gpu.GpuCapabilitiesInfo`
- `optkit_py.query.gpu.GpuDeviceInfo`

Most GPU query methods return `(ok, value)` where `ok` is a boolean and `value` is either a struct or a number/string.

#### Basic usage

```py
vendor = optkit_py.query.gpu.GpuVendor.NVIDIA

ok, count = optkit_py.query.gpu.Query.get_device_count(vendor)

if ok and count > 0:
    ok, dev = optkit_py.query.gpu.Query.device_query(vendor, device_index=0)
    if ok:
        print(dev.basic.device_name)

    ok, watts = optkit_py.query.gpu.Query.get_device_power(vendor, device_index=0)
    print("power ok?", ok, "watts=", watts)
```

#### Selected read-only helpers

```py
ok, basic = optkit_py.query.gpu.Query.get_basic_info(vendor, device_index=0)
ok, clocks = optkit_py.query.gpu.Query.get_clock_info(vendor, device_index=0)
ok, temp = optkit_py.query.gpu.Query.get_temperature_info(vendor, device_index=0)
ok, util = optkit_py.query.gpu.Query.get_utilization_info(vendor, device_index=0)
ok, power = optkit_py.query.gpu.Query.get_power_info(vendor, device_index=0)

ok, (limit, default_p, min_p, max_p, is_configurable) = optkit_py.query.gpu.Query.get_device_power_limits(vendor, device_index=0)
ok, (dev_c, mem_c) = optkit_py.query.gpu.Query.get_device_temperature(vendor, device_index=0)
ok, name = optkit_py.query.gpu.Query.get_device_name(vendor, device_index=0)
ok, (max_gpu, max_mem, min_gpu, min_mem) = optkit_py.query.gpu.Query.get_device_temperature_thresholds(vendor, device_index=0)
```

#### Setters (may require privileges / may not be supported)

```py
# These may fail on consumer GPUs or without elevated privileges
optkit_py.query.gpu.Query.set_clock(vendor, device_index=0, mem_clk_mhz=0, graphics_clk_mhz=0)
optkit_py.query.gpu.Query.set_power_limit(vendor, device_index=0, power_limit_watts=250)
```
