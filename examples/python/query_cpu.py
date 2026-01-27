#!/usr/bin/env python3
"""Query example (CPU-side): system + PMU + RAPL.

These are read-only queries.
"""

import sys

sys.path.append("../../bin/Release")

import optkit_py


def main() -> None:
    optkit_py.init(create_folder=False, execution_file="query_cpu")
    try:
        print("num_sockets=", optkit_py.query.system.Query.num_sockets)
        print("num_logical_cores=", optkit_py.query.system.Query.num_logical_cores)
        print("is_root_priv_enabled=", optkit_py.query.system.Query.is_root_priv_enabled)
        print("perf_event_paranoid=", optkit_py.query.system.Query.paranoid())
        print("smt_enabled=", optkit_py.query.system.Query.is_smt_enabled())
        print("turbo_enabled=", optkit_py.query.system.Query.is_turbo_enabled())
        print("cpu_packages=", optkit_py.query.system.Query.detect_cpu_packages())

        try:
            pmu_ids = optkit_py.query.pmu.Query.avail_pmu_ids()
            print("pmu_ids(first 8)=", pmu_ids[:8])
            if pmu_ids:
                print("default_pmu_info_str=", optkit_py.query.pmu.Query.default_pmu_info_str())
                print("pmu_info_str(first)=", optkit_py.query.pmu.Query.pmu_info_str(pmu_ids[0]))
                events = optkit_py.query.pmu.Query.get_avail_events(pmu_ids[0])
                print("events(first pmu, first 10)=", events[:10])
        except Exception as e:
            print("PMU query skipped:", e)

        try:
            print("rapl read methods bitmask=", optkit_py.query.rapl.Query.avail_rapl_read_methods())
            print("rapl perf avail=", optkit_py.query.rapl.Query.is_rapl_perf_avail())
            print("rapl sysfs avail=", optkit_py.query.rapl.Query.is_rapl_sysfs_avail())
            print("rapl msr avail=", optkit_py.query.rapl.Query.is_rapl_msr_avail())
            infos = optkit_py.query.rapl.Query.rapl_domain_info()
            print("rapl domains=", len(infos))
            if infos:
                i0 = infos[0]
                print("first domain=", i0.domain, i0.event, i0.units)
        except Exception as e:
            print("RAPL query skipped:", e)
    finally:
        optkit_py.finalize()


if __name__ == "__main__":
    main()
