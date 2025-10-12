## Overview

**Examples are taken from https://github.com/dendibakh/perf-ninja**

**Problem Analysis:** OPTKIT was used to pinpoint the known performance bottleneck in the initial code.

**Solution Validation:** A subsequent analysis on the optimized code verified the removal of this bottleneck.

### Compile and Execute🚀

First compile OPTKIT to its static version
```bash
make -j$(nproc) config=release optkit_static 
```
Then enter a problem set and execute the following
```bash
$ make
$ ./<program>
```

Each problem set includes an introduction to the problem and a baseline solution that contains a performance bottleneck. The `solution_patch` file provides an optimized version that resolves this issue. In the `main` program, both the original and patched solutions are executed, their correctness is verified, and performance metrics, execution time and speedup are measured and reported. Thus user expected to see how OPTKIT can be useful step by step.

Current system where all these examples tested has the following features
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