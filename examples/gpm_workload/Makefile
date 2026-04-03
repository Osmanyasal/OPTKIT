CXX = g++
NVCC = nvcc
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -fopenmp
NVCCFLAGS = -std=c++17 -O2 -arch=sm_60 --compiler-options -Wall

# Detect CUDA installation
CUDA_PATH := $(shell \
	if [ -d /usr/local/cuda ]; then \
		echo /usr/local/cuda; \
	elif [ -n "$$CUDA_HOME" ] && [ -d "$$CUDA_HOME" ]; then \
		echo $$CUDA_HOME; \
	elif [ -n "$$CUDA_PATH" ] && [ -d "$$CUDA_PATH" ]; then \
		echo $$CUDA_PATH; \
	elif command -v nvcc >/dev/null 2>&1; then \
		dirname $$(dirname $$(which nvcc)); \
	else \
		for cuda_dir in /usr/local/cuda-* /opt/cuda /usr/cuda /usr/local/nvidia/cuda*; do \
			if [ -d "$$cuda_dir" ] && [ -f "$$cuda_dir/bin/nvcc" ]; then \
				echo $$cuda_dir; \
				break; \
			fi; \
		done; \
	fi)

ifndef CUDA_PATH
$(error CUDA installation not found. Please install CUDA toolkit or set CUDA_HOME/CUDA_PATH environment variable.)
endif

ifeq ($(wildcard $(CUDA_PATH)/bin/nvcc),)
$(error NVCC compiler not found at $(CUDA_PATH)/bin/nvcc. Please check CUDA installation.)
endif

# Paths
OPTKIT_ROOT = ../../
OPTKIT_LIB = $(OPTKIT_ROOT)/bin/Release/liboptkit_static.a
OPTKIT_INCLUDE = $(OPTKIT_ROOT)/src
SPDLOG_INCLUDE = $(OPTKIT_ROOT)/lib/spdlog/include
LIBPFM4_INCLUDE = $(OPTKIT_ROOT)/lib/libpfm4/include

# Detect ROCm include directory for AMD SMI headers when OPTKIT was built with AMD GPU support
ROCM_INCLUDE := $(shell \
	if [ -d /opt/rocm/include/amd_smi ]; then \
		echo /opt/rocm/include; \
	else \
		for rocm_dir in /opt/rocm-*/include; do \
			if [ -f "$$rocm_dir/amd_smi/amdsmi.h" ]; then \
				echo $$rocm_dir; \
				break; \
			fi; \
		done; \
	fi)

# Include directories
INCLUDES = -I$(OPTKIT_INCLUDE) \
           -I$(SPDLOG_INCLUDE) \
           -I$(LIBPFM4_INCLUDE) \
           -I$(CUDA_PATH)/include \
           -I$(CUDA_PATH)/extras/CUPTI/include

ifneq ($(ROCM_INCLUDE),)
    INCLUDES += -I$(ROCM_INCLUDE)
endif

# Detect CUDA library architecture (lib64 vs lib)
CUDA_LIB_DIR := $(shell \
	if [ -d $(CUDA_PATH)/lib64 ]; then \
		echo $(CUDA_PATH)/lib64; \
	elif [ -d $(CUDA_PATH)/lib ]; then \
		echo $(CUDA_PATH)/lib; \
	else \
		echo $(CUDA_PATH)/lib64; \
	fi)

LDFLAGS = -lpthread -ldl -L$(CUDA_LIB_DIR) -lcudart -lcupti

# Detect NVML
NVML_LIB := $(shell \
	if [ -f $(CUDA_LIB_DIR)/libnvidia-ml.so ] || [ -f /usr/lib/x86_64-linux-gnu/libnvidia-ml.so ] || command -v nvidia-ml-py >/dev/null 2>&1; then \
		echo -lnvidia-ml; \
	fi)
ifneq ($(NVML_LIB),)
    LDFLAGS += $(NVML_LIB)
endif

# Detect ROCm/AMD SMI (needed if liboptkit_static.a was built with AMD SMI support)
ROCM_LIB := $(shell \
	if [ -d /opt/rocm/lib ]; then \
		echo /opt/rocm/lib; \
	else \
		for rocm_dir in /opt/rocm-*/lib; do \
			if [ -f "$$rocm_dir/libamd_smi.so" ]; then \
				echo $$rocm_dir; \
				break; \
			fi; \
		done; \
	fi)
ifneq ($(ROCM_LIB),)
    LDFLAGS += -L$(ROCM_LIB) -lamd_smi
endif

# Targets
TARGET = gpm_workload_example
CU_SOURCES = workload.cu
CC_SOURCES = main.cc
CU_OBJECTS = $(CU_SOURCES:.cu=.o)
CC_OBJECTS = $(CC_SOURCES:.cc=.o)
OBJECTS = $(CU_OBJECTS) $(CC_OBJECTS)

all: check-cuda $(TARGET)

check-cuda:
	@echo "=== CUDA Installation Check ==="
	@echo "CUDA Path: $(CUDA_PATH)"
	@echo "CUDA Lib Dir: $(CUDA_LIB_DIR)"
	@$(CUDA_PATH)/bin/nvcc --version | grep "release" || echo "Could not determine NVCC version"

$(TARGET): $(OBJECTS) $(OPTKIT_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(OPTKIT_LIB) $(LDFLAGS)

%.o: %.cu
	$(NVCC) $(NVCCFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)

rebuild: clean all

.PHONY: all check-cuda run clean rebuild
