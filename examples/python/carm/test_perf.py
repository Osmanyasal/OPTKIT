#!/usr/bin/env python3
"""Minimal smoke test for optkit_py perf.start/stop (CARM metrics).

Run from this directory after building OPTKIT so the local optkit_py*.so link resolves.
"""
import optkit_py


def _workload(iterations: int = 50, size: int = 256) -> float:
    """Lightweight, deterministic flops-heavy workload for CARM metrics."""
    try:
        import numpy as np  # type: ignore

        a = np.full((size, size), 1.1, dtype=np.float64)
        b = np.full((size, size), 1.2, dtype=np.float64)
        total = 0.0
        for _ in range(iterations):
            c = a @ b
            total += float(c[0, 0])
        return total
    except Exception:
        # Fallback: pure Python dot product to still produce flops
        total = 0.0
        vec = [1.1] * size
        for _ in range(iterations * 4):
            acc = 0.0
            for i in range(size):
                acc += vec[i] * vec[i]
            total += acc
        return total


def main() -> None:
    # Initialize the engine (creates output folder by default)
    optkit_py.init(create_folder=True, execution_file="perf_smoke_python")

    # Start profiling a block named "smoke" with no explicit metrics/events
    # (uses the default config on the C++ side)
    optkit_py.perf.start("smoke", ["carm"])

    # Do a flops-heavy workload to exercise CARM metrics
    total = _workload(iterations=60, size=192)

    # Stop profiling
    optkit_py.perf.stop()

    # Finalize to clean up
    optkit_py.finalize()

    print("perf smoke test complete; total=", total)


if __name__ == "__main__":
    main()
