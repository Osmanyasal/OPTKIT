#!/usr/bin/env python3
"""Example: HWMON temperature profiling via optkit_py.temperature.hwmon.start/stop."""

import sys

sys.path.append("../../bin/Release")

import optkit_py

from workload import workload_flops


def main() -> None:
    optkit_py.init(create_folder=True, execution_file="temperature_hwmon")
    try:
        optkit_py.temperature.hwmon.start("smoke_temp_hwmon")
        total = workload_flops(iterations=10, size=256)
        optkit_py.temperature.hwmon.stop()
        print("hwmon temperature example complete; total=", total)
    finally:
        optkit_py.finalize()


if __name__ == "__main__":
    main()
