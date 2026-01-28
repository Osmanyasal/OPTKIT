#!/usr/bin/env python3
"""Example: GPU energy profiling (best-effort NVIDIA+AMD) via optkit_py.energy.gpu.start/stop."""

import sys

sys.path.append("../../bin/Release")

import optkit_py

from workload import workload_flops


def main() -> None:
    optkit_py.init(create_folder=True, execution_file="energy_gpu")
    try:
        # May print warnings if no supported backend is available.
        optkit_py.energy.gpu.start("gpu_energy")
        total = workload_flops(iterations=10, size=256)
        optkit_py.energy.gpu.stop()
        print("gpu energy example complete; total=", total)
    finally:
        optkit_py.finalize()


if __name__ == "__main__":
    main()
