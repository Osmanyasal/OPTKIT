#!/usr/bin/env python3
"""Minimal smoke test for optkit_py perf.start/stop (CARM metrics).

Run from this directory after building OPTKIT so the local optkit_py*.so link resolves.
"""
import sys
sys.path.append("../../bin/Release")  # Adjust as needed to find optkit_py

import optkit_py

from workload import workload_flops


# API cheat sheet (GitHub-ready Markdown):
#   See examples/python/OPTKIT_PY_API.md

def main() -> None:
    # Initialize the engine (creates output folder by default)
    optkit_py.init(create_folder=True, execution_file="perf_carm_python")
    optkit_py.perf.start("carm_test", ["carm"])

    # Do a flops-heavy workload to exercise CARM metrics
    total = workload_flops(iterations=100, size=5000)

    optkit_py.perf.stop()

    # Finalize to clean up
    optkit_py.finalize()
    print("perf carm test complete; total=", total)


if __name__ == "__main__":
    main()
