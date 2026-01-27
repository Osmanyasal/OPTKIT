#!/usr/bin/env python3
"""Example: frequency module (conversion + query helpers).

This example sticks to query-only calls so it should run without root.
"""

import sys

sys.path.append("../../bin/Release")

import optkit_py


def main() -> None:
    optkit_py.init(create_folder=True, execution_file="frequency")
    try:
        print("convert 2400 MHz -> kHz:", optkit_py.frequency.convert("2400 MHz", optkit_py.frequency.Unit.KHz))

        print("governors:", optkit_py.frequency.cpu.query.available_governors(core=0))
        print("governor:", optkit_py.frequency.cpu.query.get_governor(core=0))
        print("driver:", optkit_py.frequency.cpu.query.get_scaling_driver(core=0))

        print("bios_limit_khz:", optkit_py.frequency.cpu.query.get_bios_limit(core=0))
        print("scaling_min_khz:", optkit_py.frequency.cpu.query.get_scaling_min_limit(core=0))
        print("scaling_max_khz:", optkit_py.frequency.cpu.query.get_scaling_max_limit(core=0))

        print("cpuinfo_min_khz:", optkit_py.frequency.cpu.query.get_cpuinfo_min_freq(core=0))
        print("cpuinfo_max_khz:", optkit_py.frequency.cpu.query.get_cpuinfo_max_freq(core=0))

        freqs = optkit_py.frequency.cpu.query.available_core_frequencies(core=0, step_khz=200000)
        print("available_core_freqs(first 10):", freqs[:10])
    finally:
        optkit_py.finalize()


if __name__ == "__main__":
    main()
