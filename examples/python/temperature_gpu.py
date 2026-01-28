#!/usr/bin/env python3
"""Example: GPU temperature profiling via optkit_py.temperature.gpu.start/stop.

This may fail if no supported GPU backend is available.
"""

import sys

sys.path.append("../../bin/Release")

import optkit_py

from workload import workload_flops


def main() -> None:
    optkit_py.init(create_folder=True, execution_file="temperature_gpu")
    try:
        optkit_py.temperature.gpu.start("temp_gpu")
        total = workload_flops(iterations=5, size=256)
        optkit_py.temperature.gpu.stop()
        print("gpu temperature example complete; total=", total)
    finally:
        optkit_py.finalize()


if __name__ == "__main__":
    main()
