## Overview

**Some examples are taken from https://github.com/dendibakh/perf-ninja**

**Problem Analysis:** OPTKIT was used to pinpoint the known performance bottleneck in the initial code.

**Solution Validation:** A subsequent analysis on the optimized code verified the removal of this bottleneck.

### Compile and Execute🚀

First compile OPTKIT to its static version then build optkit-cli
```bash
$ premake5 gmake
$ make -j$(nproc) config=release optkit_static 
$ cd tools/optkit-cli && make -j$(nproc) && alias optkit-cli=$(pwd)/optkit
```
Then enter a problem set and execute the following
```bash
$ make
$ ./<program>
```

Each problem set begins with an introduction and a baseline implementation that intentionally includes a performance bottleneck. The accompanying `solution_patch` file offers an optimized version that eliminates this limitation. In the `main` program, both the baseline and optimized versions are executed, their correctness is validated, and performance metrics—such as execution time and speedup—are measured and reported. This allows users to observe how OPTKIT provides practical, step-by-step performance improvements.

`NAS_BENCH` and `STREAM` are two standard benchmarks integrated for execution via `optkit-cli`. Other benchmarks are already instrumented and can be run directly. Each benchmark includes a **Performance Analysis Results** section in its `README.md`, where performance outcomes and evaluations are presented.

The system used to test all these examples is configured with the following specifications:
```bash
            .-/+oossssoo+/-.               XXXXXXXXXXXXXXXX 
        `:+ssssssssssssssssss+:`           ---------------- 
      -+ssssssssssssssssssyyssss+-         OS: Ubuntu 24.04.3 LTS x86_64 
    .ossssssssssssssssssdMMMNysssso.       Host: MS-7E51 1.0 
   /ssssssssssshdmmNNmmyNMMMMhssssss/      Kernel: 6.14.0-33-generic 
  +ssssssssshmydMMMMMMMNddddyssssssss+     Uptime: 6 days, 5 hours, 36 mins 
 /sssssssshNMMMyhhyyyyhmNMMMNhssssssss/    Packages: 2704 (dpkg), 17 (snap) 
.ssssssssdMMMNhsssssssssshNMMMdssssssss.   Shell: bash 5.2.21 
+sssshhhyNMMNyssssssssssssyNMMMysssssss+   Resolution: 2560x1440, 1080x1920 
ossyNMMMNyMMhsssssssssssssshmmmhssssssso   DE: GNOME 46.0 
ossyNMMMNyMMhsssssssssssssshmmmhssssssso   WM: Mutter 
+sssshhhyNMMNyssssssssssssyNMMMysssssss+   WM Theme: Adwaita 
.ssssssssdMMMNhsssssssssshNMMMdssssssss.   Theme: Yaru-dark [GTK2/3] 
 /sssssssshNMMMyhhyyyyhdNMMMNhssssssss/    Icons: Yaru [GTK2/3] 
  +sssssssssdmydMMMMMMMMddddyssssssss+     Terminal: x-terminal-emul 
   /ssssssssssshdmNNNNmyNMMMMhssssss/      CPU: AMD Ryzen 9 7950X (32) @ 5.883GHz 
    .ossssssssssssssssssdMMMNysssso.       GPU: AMD ATI 70:00.0 Raphael 
      -+sssssssssssssssssyyyssss+-         GPU: NVIDIA GeForce RTX 5080 
        `:+ssssssssssssssssss+:`           Memory: 12491MiB / 63341MiB 
            .-/+oossssoo+/-.
```