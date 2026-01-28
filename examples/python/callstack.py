#!/usr/bin/env python3
"""Example: callstack profiling via optkit_py.callstack.start/stop."""

import sys

sys.path.append("../../bin/Release")

import optkit_py

from workload import workload_flops


def main() -> None:
    optkit_py.init(create_folder=True, execution_file="callstack")
    try:
        optkit_py.callstack.start("callstack")
        total = workload_flops(iterations=20, size=256)
        optkit_py.callstack.stop()
        print("callstack example complete; total=", total)
    finally:
        optkit_py.finalize()


if __name__ == "__main__":
    main()
