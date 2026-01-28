#!/usr/bin/env python3
"""Example: perf profiling (CARM) via optkit_py.perf.start/stop."""

import sys

sys.path.append("../../bin/Release")

import optkit_py

from workload import workload_flops


def main() -> None:
    optkit_py.init(create_folder=True, execution_file="perf")
    try:
        optkit_py.perf.start("perf", metrics=["carm"], events=[])
        optkit_py.energy.cpu.start("cpu_energy")

        total = 0
        total = total + workload_flops(iterations=50000, size=256)

        optkit_py.energy.cpu.stop()
        optkit_py.perf.stop()
        print("perf example complete; total=", total)
    finally:
        optkit_py.finalize()


if __name__ == "__main__":
    main()



