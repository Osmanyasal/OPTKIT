
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
    msrmod [OPTIONS]                Read/write MSRs via /dev/cpu/<cpu>/msr_safe
    train <FOLDER> [TRAIN_OPTS...]                        Train a frequency model from screenshot JSON datasets

TRAIN (train):
    Trains a GRU network from existing `*_screenshot/` datasets and exports an ONNX model.

    Dataset format:
        <FOLDER>/target_range.txt
            core_min:<GHz>
            core_max:<GHz>
            (optional) uncore_min:<GHz>
            (optional) uncore_max:<GHz>

        <FOLDER>/*_screenshot/
            target.txt              # core:<GHz> (optional uncore:<GHz>)
            stat__cpu_pmu.json
            stat__disk_io.json

        Notes:
            - In each stat__*.json file, the first reading is the whole-execution aggregate and is skipped.
            - Remaining readings are treated as time-series samples (typically ~1 second).

    Generating datasets:
        Use screenshot tracing when profiling:

            optkit stat -ss -e <event> -m <metric> -o -- <program>

    Outputs:
        <FOLDER>/optkit_train/model.onnx
            - INT8-quantized ONNX model (weights quantized; outputs still in GHz).
            - Input: x with shape (1, seq_len, num_features)
            - Output: y in GHz (core or [core, uncore] when available)
            - Normalization and min/max scaling are baked into the ONNX graph.

        <FOLDER>/optkit_train/model_fp32.onnx
            - FP32 ONNX model (exported before quantization).

        <FOLDER>/optkit_train/meta.json
            - Feature names, target ranges, normalization parameters, and trainer hyperparameters.

    Passing training options:
        `optkit train` forwards extra flags after <FOLDER> to the Python trainer, e.g.:

            optkit train /path/to/dataset_folder --epochs 50 --hidden-size 32 --window 10

        Supported training options (forwarded to the trainer):
            --epochs N           (default: 30)
            --batch-size N       (default: 256)
            --lr LR              (default: 1e-3)
            --hidden-size N      (default: 32)
            --num-layers N       (default: 1)
            --window W           (default: 0 = auto)
            --opset N            (default: 17)
            --device cpu|cuda    (default: cpu)

        Defaults:
            - `--hidden-size` defaults to 32.

    Notes (Python deps):
        Some Linux distros block system-wide `pip install` (PEP 668).

        `optkit train` will automatically:
            1) create tools/optkit-cli/.venv-train (if missing)
            2) install deps from requirements-train.txt (if missing)
            3) run the trainer using that venv

        Manual setup (optional):

            cd tools/optkit-cli
            python3 -m venv .venv-train
            ./.venv-train/bin/pip install -r requirements-train.txt

        You can override the Python interpreter used for training:

            OPTKIT_TRAIN_PYTHON=/path/to/python optkit train /path/to/dataset_folder

TOPOLOGY:
    optkit topology                 Show complete system topology
    optkit topology cpu             Show CPU topology only
    optkit topology gpu             Show GPU topology only

LIST:
    optkit list [cpu|gpu] pmu                   List available PMU info
    optkit list [cpu|gpu|disk|memory]           List available events + metrics
    optkit list [cpu|gpu|disk|memory] events    List available events
    optkit list [cpu|gpu|disk|memory] metrics   List available metrics

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

    Options can be interleaved:
    optkit stat --bench freq-scaling -e cycles -m ipc -- <program> 

MSRMOD:
    Read/write a Model-Specific Register (MSR) using msr-safe (msr_safe device).

        optkit msrmod -r -c <cpu> -a <msr_addr>
        optkit msrmod -w -c <cpu> -a <msr_addr> -v <value>

        Notes:
            - Requires msr-safe (and its `msr_safe` devices) to be available.
            - Requires sufficient permissions.

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

    # MSR access (requires msr module + permissions)
    optkit msrmod -r -c 0 -a 0x1b1
    optkit msrmod -w -c 0 -a 0x1b1 -v 0x1234

    # Train a frequency model from an existing dataset folder
    optkit train /path/to/dataset_folder

NOTE:
    - 'stat' without --bench or --affinity: Single execution, collects specified events/metrics
    - 'stat' with --bench or --affinity: Multiple executions with varying configurations
      (e.g., different frequencies, core counts, affinity patterns)

```
