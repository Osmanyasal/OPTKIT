
### Features

OPTKIT - Performance and Energy Profiling & Optimization Tool

A command-line interface mirroring the usability of linux/perf, providing scriptable access to the full suite of OPTKIT monitoring metrics without requiring code changes for any application.


```bash 

USAGE:
    optkit <COMMAND> [OPTIONS] [-- <PROGRAM>]

COMMANDS:
    topology [cpu|gpu]              Show system topology
    list <TYPE> [cpu|gpu]           List available components
    stat [OPTIONS] -- <PROGRAM>     Run single-shot profiling (like perf stat)

TOPOLOGY:
    optkit topology                 Show complete system topology
    optkit topology cpu             Show CPU topology only
    optkit topology gpu             Show GPU topology only

LIST:
    optkit list [all|cpu|gpu]           List all PMU capabilities
    optkit list [all|cpu|gpu] pmu       List available PMU info
    optkit list [all|cpu|gpu] events    List available PMU events
    optkit list [all|cpu|gpu] metrics   List available metrics

PROFILING (stat):
    Single execution profiling - runs program once and collects metrics
    
    optkit stat -- <program>                                                          Default profiling
    optkit stat -e <event> -- <program>                                               Profile specific event
    optkit stat -m <metric> -- <program>                                              Profile specific metric
    optkit stat -e <event> -m <metric> -- <program>                                   Profile event + metric
    optkit stat -e <event> -m <metric> -T <sampling_period_in_ms> -- <program>        Profile event + metric
    optkit stat -e <event> -m <metric> -T <sampling_period_in_ms> -S 0 -- <program>   Profile event + metric + socket 0

BENCHMARKING (--bench):
    Multiple execution analysis - runs program multiple times with different configurations
    
    optkit stat --bench freq-scaling -- <program>            Frequency scaling analysis
    optkit stat --bench core-scaling -- <program>            Core scaling analysis
    optkit stat --affinity <STRATEGY> -- <program>           Affinity analysis

    Options can be interleaved:
    optkit stat --bench freq-scaling -e cycles -m ipc -- <program>

AFFINITY STRATEGIES:
    --affinity compact              Pack threads on fewer cores (cache locality)
    --affinity scatter              Spread threads across cores (avoid contention)
    --affinity numa                 NUMA-aware placement (memory locality)
    --affinity manual               Manual affinity control

EXAMPLES:
    # Topology queries
    optkit topology
    optkit topology cpu
    optkit topology gpu

    # List capabilities
    optkit list all
    optkit list cpu all
    optkit list cpu events
    optkit list cpu metrics
    optkit list gpu all
    optkit list gpu events
    optkit list gpu metrics

    # Single-shot profiling (executes once)
    optkit stat -- ./my_program
    optkit stat -e cycles -e instructions -- ./app
    optkit stat -m ipc -m cache-miss-rate -- ./benchmark

    # Benchmark analysis (executes multiple times)
    optkit stat --bench freq-scaling -- ./compute_heavy
    optkit stat --bench core-scaling -- ./parallel_app
    optkit stat --affinity scatter -- ./threaded_app
    optkit stat --affinity numa -- ./parallel_workload

    # Interleaved options (benchmark + specific profiling)
    optkit stat --bench freq-scaling -e cycles -m ipc -- ./program --input data.txt
    optkit stat -e cache-misses -m energy --bench core-scaling -- ./app
    optkit stat --affinity compact -e instructions -m ipc -- ./multithreaded

NOTE:
    - 'stat' without --bench or --affinity: Single execution, collects specified events/metrics
    - 'stat' with --bench or --affinity: Multiple executions with varying configurations
      (e.g., different frequencies, core counts, affinity patterns)

```