#!/usr/bin/env python3
"""Example: disk I/O profiling via optkit_py.disk.start/stop."""

import os
import sys

sys.path.append("../../bin/Release")

import optkit_py


def _small_io_workload(tmp_path: str = "/tmp/optkit_disk_smoke.bin", mb: int = 16) -> None:
    chunk = b"x" * (1024 * 1024)
    with open(tmp_path, "wb") as f:
        for _ in range(mb):
            f.write(chunk)
        f.flush()
        os.fsync(f.fileno())

    with open(tmp_path, "rb") as f:
        while f.read(1024 * 1024):
            pass


def main() -> None:
    optkit_py.init(create_folder=True, execution_file="disk")
    try:
        optkit_py.disk.start("smoke_disk")
        _small_io_workload()
        optkit_py.disk.stop()
        print("disk example complete")
    finally:
        optkit_py.finalize()


if __name__ == "__main__":
    main()
