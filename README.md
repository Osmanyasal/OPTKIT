# OPTKIT Overview

OPTKIT is a highly customizable C++11 library and toolset designed for measuring energy consumption, detecting performance bottlenecks, and tuning hardware parameters at runtime to improve overall energy efficiency. Its overhead remains low, primarily depending on the frequency of measurements and the number of monitored regions.

OPTKIT integrates seamlessly into the development workflow like any other library. It can assist developers during development by providing energy and performance bottleneck-related insights to guide code improvements and refactoring, or it can be embedded into production environments to dynamically optimize hardware settings for greater energy efficiency.

The library provides comprehensive monitoring capabilities for CPUs, GPUs, and I/O systems across multiple architectures including Intel, AMD, ARM, and NVIDIA platforms. It supports real-time frequency control, energy monitoring via RAPL, performance monitoring through PMU events, and includes a rich set of utility tools for performance analysis and optimization.


## Hardware Support Matrix

| Feature            | Intel | AMD Zen | ARM Neoverse | NVIDIA GPU | AMD GPU |
|--------------------|:-----:|:-------:|:------------:|:----------:|:-------:|
| PMU Events         | ✅    | ✅      | ✅ (N1+, V1+)  | ❌         | ❌      |
| Energy (RAPL)      | ✅    | ✅ (Zen2+) | N/A         | N/A         | N/A      |
| GPU Power          | N/A   | N/A     | N/A          | ✅         | ✅      |
| Temperature        | ✅    | ✅      | ✅           | ✅         | ✅      |
| Frequency Tuning   | ✅    | ✅      | ✅           | ✅         | ✅      |
| Callstack Analysis | ✅    | ✅      | ?           | N/A         | N/A      |



## Download and Install 🚀

```bash
git clone https://github.com/Osmanyasal/OPTKIT.git
cd ./OPTKIT
git submodule update --force --recursive --init --remote
cd lib/carm-roofline; python3 run.py; cd ../../ #(optional to build carm roofline)
premake5 gmake

## To create libraries:
make -j$(nproc) config=release optkit_static  # for static 
make -j$(nproc) config=release optkit_dynamic  # for dynamic 

## To compile tools
cd tools
cd optkit-cli; make -j$(nproc); alias optkit-cli=$(pwd)/optkit; cd ..
cd optkit-setenv; make -j$(nproc); alias optkit-setenv=$(pwd)/optkit-setenv; cd ..
cd ..

## To Run Tests:
make -j$(nproc) config=test optkit_static ## this converts some private or protected fields to public and being tested
make -j$(nproc) config=debug optkit_test  ## no optimization in tests, raw results are viewed.
./bin/Debug/optkit_test

```

For quick introduction check out the [Introduction Slide](https://docs.google.com/presentation/d/1ghTQz3BauL1c7P96y3Rod4U9ncjqSUFk3FekvKx_URA/edit?usp=sharing)
For more details and tutorials, check out the [OPTKIT Wiki](https://github.com/Osmanyasal/OPTKIT/wiki/OPTKIT)
