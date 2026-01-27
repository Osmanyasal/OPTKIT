#!/usr/bin/env python3
"""Example: CPU energy profiling (RAPL) via optkit_py.energy.cpu.start/stop."""

import sys

sys.path.append("../../bin/Release")

import optkit_py

from workload import workload_flops


def main() -> None:
    optkit_py.init(create_folder=True, execution_file="energy_cpu")
    try:
        optkit_py.energy.cpu.start("smoke_cpu_energy")
        total = workload_flops(iterations=20, size=256)
        optkit_py.energy.cpu.stop()
        print("cpu energy example complete; total=", total)
    finally:
        optkit_py.finalize()


if __name__ == "__main__":
    main()
