# OPTKIT Overview

OPTKIT is a highly customizable C++11 library and toolset designed for measuring energy consumption, detecting performance bottlenecks, and tuning hardware parameters at runtime to improve overall energy efficiency. Its overhead remains low, primarily depending on the frequency of measurements and the number of monitored regions.

OPTKIT integrates seamlessly into the development workflow like any other library. It can assist developers during development by providing energy and performance bottleneck-related insights to guide code improvements and refactoring, or it can be embedded into production environments to dynamically optimize hardware settings for greater energy efficiency.

The library provides comprehensive monitoring capabilities for CPUs, GPUs, and I/O systems across multiple architectures including Intel, AMD, ARM, and NVIDIA platforms. It supports real-time frequency control, energy monitoring via RAPL, performance monitoring through PMU events, and includes a rich set of utility tools for performance analysis and optimization.

## Download and Install 🚀

```bash
git clone https://github.com/Osmanyasal/OPTKIT.git
cd ./OPTKIT
git submodule update --force --recursive --init --remote
cd lib/carm-roofline; python3 run.py; cd .. #(optional to build carm roofline)
premake5 gmake

## To create libraries:
make -j$(nproc) config=release optkit_static  # for static 
make -j$(nproc) config=release optkit_dynamic  # for dynamic 

## To Run Tests:
make -j$(nproc) config=test optkit_static ## this converts some private or protected fields to public and being tested
make -j$(nproc) config=debug optkit_test  ## no optimization in tests, raw results are viewed.
./bin/Debug/optkit_test

## 🔍 List All Available Tests
./bin/Debug/optkit_test --gtest_list_tests

## ▶️ Run Specific Test(s)
./bin/Debug/optkit_test --gtest_filter=MyTestSuite.MyTestCase
./bin/Debug/optkit_test --gtest_filter="CPUFreqTest.*"
./bin/Debug/optkit_test --gtest_filter="*Freq*:MemoryTest.*"

```

For more details and tutorials, check out the [wiki page]([https://github.com/Osmanyasal/OPTKIT/wiki/OPTKIT%E2%80%90API-(Start-Here)](https://github.com/Osmanyasal/OPTKIT/wiki/OPTKIT))
