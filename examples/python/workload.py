"""Reusable workloads for OPTKIT Python examples."""

from __future__ import annotations


def workload_flops(iterations: int = 50, size: int = 256) -> float:
    """Lightweight, deterministic flops-heavy workload.

    Tries NumPy first; falls back to a pure-Python dot-product loop.
    """

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
        total = 0.0
        vec = [1.1] * size
        for _ in range(iterations * 4):
            acc = 0.0
            for i in range(size):
                acc += vec[i] * vec[i]
            total += acc
        return total
