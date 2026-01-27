#!/usr/bin/env python3
"""Query example (GPU-side): uses optkit_py.query.gpu.

Runs best-effort; prints results for NVIDIA and AMD if available.
"""

import sys

sys.path.append("../../bin/Release")

import optkit_py


def _print_basic(info: "optkit_py.query.gpu.GpuBasicInfo") -> None:
    print("  basic:")
    print("    id:", info.id)
    print("    device_name:", info.device_name)
    print("    vendor:", info.vendor)
    print("    architecture:", info.architecture)
    print("    vendor_string:", info.vendor_string)
    print("    is_integrated:", info.is_integrated)


def _print_version(info: "optkit_py.query.gpu.GpuVersionInfo") -> None:
    print("  version:")
    print("    driver_major_minor:", info.driver_major_minor)
    print("    driver_version_string:", info.driver_version_string)
    print("    library_version_string:", info.library_version_string)


def _print_compute(info: "optkit_py.query.gpu.GpuComputeInfo") -> None:
    print("  compute:")
    print("    compute_capability:", f"{info.compute_capability_major}.{info.compute_capability_minor}")
    print("    multiprocessor_count:", info.multiprocessor_count)
    print("    cores_per_mp:", info.cores_per_mp)
    print("    total_cores:", info.total_cores)
    print("    warp_size:", info.warp_size)


def _print_memory(info: "optkit_py.query.gpu.GpuMemoryInfo") -> None:
    print("  memory:")
    print("    total_global_memory_MBytes:", info.total_global_memory_MBytes)
    print("    free_memory_MBytes:", info.free_memory_MBytes)
    print("    used_memory_MBytes:", info.used_memory_MBytes)
    print("    memory_bus_width_bits:", info.memory_bus_width_bits)
    print("    memory_utilization_percent:", info.memory_utilization_percent)


def _print_clocks(info: "optkit_py.query.gpu.GpuClockInfo") -> None:
    print("  clocks:")
    print("    current_sm_clock_MHz:", info.current_sm_clock_MHz)
    print("    current_video_clock_MHz:", info.current_video_clock_MHz)
    print("    current_graphics_clock_MHz:", info.current_graphics_clock_MHz)
    print("    current_memory_clock_MHz:", info.current_memory_clock_MHz)
    print("    max_sm_clock_MHz:", info.max_sm_clock_MHz)
    print("    max_video_clock_MHz:", info.max_video_clock_MHz)
    print("    max_graphics_clock_MHz:", info.max_graphics_clock_MHz)
    print("    max_memory_clock_MHz:", info.max_memory_clock_MHz)
    print("    min_sm_clock_MHz:", info.min_sm_clock_MHz)
    print("    min_video_clock_MHz:", info.min_video_clock_MHz)
    print("    min_graphics_clock_MHz:", info.min_graphics_clock_MHz)
    print("    min_memory_clock_MHz:", info.min_memory_clock_MHz)
    print("    has_frequency_control:", info.has_frequency_control)

    # memory_supported_clock_rates_MHz is a vector -> Python list
    mem_rates = info.memory_supported_clock_rates_MHz
    print("    memory_supported_clock_rates_MHz(first 10):", mem_rates[:10])

    # graphics_supported_clock_rates_MHz is a map[mem_clock_mhz -> list[graphics_clock_mhz]] -> Python dict
    gfx = info.graphics_supported_clock_rates_MHz
    if isinstance(gfx, dict):
        mem_keys = sorted(gfx.keys())
        print("    graphics_supported_mem_clocks_MHz(first 10):", mem_keys[:10])
        if mem_keys:
            k0 = mem_keys[0]
            print(f"    graphics_supported_for_mem_{k0}_MHz(first 10):", gfx[k0][:10])
    else:
        # Fallback (in case binding changes)
        print("    graphics_supported_clock_rates_MHz:", gfx)


def _print_power(info: "optkit_py.query.gpu.GpuPowerInfo") -> None:
    print("  power:")
    print("    current_power_watts:", info.current_power_watts)
    print("    power_limit_watts:", info.power_limit_watts)
    print("    min_power_watts:", info.min_power_watts)
    print("    max_power_watts:", info.max_power_watts)
    print("    default_power_watts:", info.default_power_watts)
    print("    has_power_monitoring:", info.has_power_monitoring)
    print("    is_configurable:", info.is_configurable)


def _print_temperature(info: "optkit_py.query.gpu.GpuTemperatureInfo") -> None:
    print("  temperature:")
    print("    current_device_temperature_celsius:", info.current_device_temperature_celsius)
    print("    current_memory_temperature_celsius:", info.current_memory_temperature_celsius)
    print("    max_device_temperature_celsius:", info.max_device_temperature_celsius)
    print("    max_memory_temperature_celsius:", info.max_memory_temperature_celsius)
    print("    min_device_temperature_celsius:", info.min_device_temperature_celsius)
    print("    min_memory_temperature_celsius:", info.min_memory_temperature_celsius)
    print("    has_temperature_monitoring:", info.has_temperature_monitoring)


def _print_utilization(info: "optkit_py.query.gpu.GpuUtilizationInfo") -> None:
    print("  utilization:")
    print("    gpu_utilization_percent:", info.gpu_utilization_percent)
    print("    memory_utilization_percent:", info.memory_utilization_percent)
    print("    has_utilization_monitoring:", info.has_utilization_monitoring)


def _print_hardware(info: "optkit_py.query.gpu.GpuHardwareInfo") -> None:
    print("  hardware:")
    print("    pci_bus_id:", info.pci_bus_id)
    print("    pci_device_id:", info.pci_device_id)
    print("    pci_subsystem_id:", info.pci_subsystem_id)
    print("    board_id:", info.board_id)
    print("    multi_gpu_board:", info.multi_gpu_board)


def _print_capabilities(info: "optkit_py.query.gpu.GpuCapabilitiesInfo") -> None:
    print("  capabilities:")
    print("    ecc_enabled:", info.ecc_enabled)
    print("    supports_unified_memory:", info.supports_unified_memory)
    print("    persistence_mode_enabled:", info.persistence_mode_enabled)


def _query_vendor(name: str, vendor) -> None:
    print(f"\n== {name} ==")
    print("  is_device_exists:", optkit_py.query.gpu.Query.is_device_exists(vendor))

    ok, driver_ver = optkit_py.query.gpu.Query.get_driver_version(vendor)
    print("  get_driver_version:", ok, driver_ver)

    ok, lib_ver = optkit_py.query.gpu.Query.get_library_version(vendor)
    print("  get_library_version:", ok, lib_ver)

    ok, count = optkit_py.query.gpu.Query.get_device_count(vendor)
    print("  get_device_count:", ok, count)
    if not ok or count == 0:
        return

    device_index = 0

    ok, device_name = optkit_py.query.gpu.Query.get_device_name(vendor, device_index)
    print("  get_device_name:", ok, device_name)

    ok, fan_count = optkit_py.query.gpu.Query.get_fan_count(vendor, device_index)
    print("  get_fan_count:", ok, fan_count)

    ok, warp_size = optkit_py.query.gpu.Query.get_warp_size(vendor, device_index)
    print("  get_warp_size:", ok, warp_size)

    ok, arch = optkit_py.query.gpu.Query.get_architecture(vendor, device_index)
    print("  get_architecture:", ok, arch)

    ok, basic = optkit_py.query.gpu.Query.get_basic_info(vendor, device_index)
    print("  get_basic_info:", ok)
    if ok:
        _print_basic(basic)

    ok, version = optkit_py.query.gpu.Query.get_version_info(vendor, device_index)
    print("  get_version_info:", ok)
    if ok:
        _print_version(version)

    ok, compute = optkit_py.query.gpu.Query.get_compute_info(vendor, device_index)
    print("  get_compute_info:", ok)
    if ok:
        _print_compute(compute)

    ok, memory = optkit_py.query.gpu.Query.get_memory_info(vendor, device_index)
    print("  get_memory_info:", ok)
    if ok:
        _print_memory(memory)

    ok, clocks = optkit_py.query.gpu.Query.get_clock_info(vendor, device_index)
    print("  get_clock_info:", ok)
    if ok:
        _print_clocks(clocks)

    ok, power = optkit_py.query.gpu.Query.get_power_info(vendor, device_index)
    print("  get_power_info:", ok)
    if ok:
        _print_power(power)

    ok, temp = optkit_py.query.gpu.Query.get_temperature_info(vendor, device_index)
    print("  get_temperature_info:", ok)
    if ok:
        _print_temperature(temp)

    ok, util = optkit_py.query.gpu.Query.get_utilization_info(vendor, device_index)
    print("  get_utilization_info:", ok)
    if ok:
        _print_utilization(util)

    ok, hw = optkit_py.query.gpu.Query.get_hardware_info(vendor, device_index)
    print("  get_hardware_info:", ok)
    if ok:
        _print_hardware(hw)

    ok, caps = optkit_py.query.gpu.Query.get_capabilities_info(vendor, device_index)
    print("  get_capabilities_info:", ok)
    if ok:
        _print_capabilities(caps)

    ok, watts = optkit_py.query.gpu.Query.get_device_power(vendor, device_index)
    print("  get_device_power:", ok, watts)

    ok, limit, default_p, min_p, max_p, is_cfg = optkit_py.query.gpu.Query.get_device_power_limits(vendor, device_index)
    print("  get_device_power_limits:", ok)
    if ok:
        print("    power_limit_watts:", limit)
        print("    default_power_watts:", default_p)
        print("    min_power_watts:", min_p)
        print("    max_power_watts:", max_p)
        print("    is_configurable:", is_cfg)

    ok, dev_c, mem_c = optkit_py.query.gpu.Query.get_device_temperature(vendor, device_index)
    print("  get_device_temperature:", ok)
    if ok:
        print("    device_celsius:", dev_c)
        print("    memory_celsius:", mem_c)

    ok, max_gpu, max_mem, min_gpu, min_mem = optkit_py.query.gpu.Query.get_device_temperature_thresholds(vendor, device_index)
    print("  get_device_temperature_thresholds:", ok)
    if ok:
        print("    max_gpu_celsius:", max_gpu)
        print("    max_mem_celsius:", max_mem)
        print("    min_gpu_celsius:", min_gpu)
        print("    min_mem_celsius:", min_mem)

    ok, dev = optkit_py.query.gpu.Query.device_query(vendor, device_index)
    print("  device_query:", ok)
    if ok:
        _print_basic(dev.basic)
        _print_version(dev.version)
        _print_compute(dev.compute)
        _print_memory(dev.memory)
        _print_clocks(dev.clocks)
        _print_power(dev.power)
        _print_temperature(dev.temperature)
        _print_utilization(dev.utilization)
        _print_hardware(dev.hardware)
        _print_capabilities(dev.capabilities)


def main() -> None:
    optkit_py.init(create_folder=False, execution_file="query_gpu")
    try:
        _query_vendor("NVIDIA", optkit_py.query.gpu.GpuVendor.NVIDIA)
        _query_vendor("AMD", optkit_py.query.gpu.GpuVendor.AMD)
    finally:
        optkit_py.finalize()


if __name__ == "__main__":
    main()
