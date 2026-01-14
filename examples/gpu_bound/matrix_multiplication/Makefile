CXX = g++
NVCC = nvcc
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -fopenmp
NVCCFLAGS = -std=c++17 -O2 -arch=sm_60 --compiler-options -Wall

# Detect CUDA installation - check multiple common paths
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

# Check if CUDA is available
ifndef CUDA_PATH
$(error CUDA installation not found. Please install CUDA toolkit or set CUDA_HOME/CUDA_PATH environment variable.)
endif

# Verify CUDA installation
ifeq ($(wildcard $(CUDA_PATH)/bin/nvcc),)
$(error NVCC compiler not found at $(CUDA_PATH)/bin/nvcc. Please check CUDA installation.)
endif

# Paths
OPTKIT_ROOT = ../../..
OPTKIT_LIB = $(OPTKIT_ROOT)/bin/Release/liboptkit_static.a
OPTKIT_INCLUDE = $(OPTKIT_ROOT)/src
SPDLOG_INCLUDE = $(OPTKIT_ROOT)/lib/spdlog/include
LIBPFM4_INCLUDE = $(OPTKIT_ROOT)/lib/libpfm4/include

# Include directories
INCLUDES = -I$(OPTKIT_INCLUDE) \
           -I$(SPDLOG_INCLUDE) \
           -I$(LIBPFM4_INCLUDE) \
           -I$(CUDA_PATH)/include

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

# Detect additional GPU libraries
NVML_LIB := $(shell \
	if [ -f $(CUDA_LIB_DIR)/libnvidia-ml.so ] || [ -f /usr/lib/x86_64-linux-gnu/libnvidia-ml.so ] || command -v nvidia-ml-py >/dev/null 2>&1; then \
		echo -lnvidia-ml; \
	fi)
ifneq ($(NVML_LIB),)
    LDFLAGS += $(NVML_LIB)
endif

# Detect ROCm/AMD SMI installation (if available alongside CUDA)
ROCM_INCLUDE := $(shell if [ -d /opt/rocm/include ]; then echo /opt/rocm/include; elif [ -d /opt/rocm-5.7.1/include ]; then echo /opt/rocm-5.7.1/include; fi)
ifneq ($(ROCM_INCLUDE),)
    INCLUDES += -I$(ROCM_INCLUDE)
    LDFLAGS += -L/opt/rocm/lib -lamd_smi
endif

# Targets
TARGET = matrix_multiplication_example
CU_SOURCES = solution.cu solution_patch.cu
CC_SOURCES = main.cc init.cc
CU_OBJECTS = $(CU_SOURCES:.cu=.o)
CC_OBJECTS = $(CC_SOURCES:.cc=.o)
OBJECTS = $(CU_OBJECTS) $(CC_OBJECTS)

# Default target
all: check-cuda $(TARGET)

# Check CUDA installation
check-cuda:
	@echo "=== CUDA Installation Check ==="
	@echo "CUDA Path: $(CUDA_PATH)"
	@echo "CUDA Lib Dir: $(CUDA_LIB_DIR)"
	@echo "NVCC Version:"
	@$(CUDA_PATH)/bin/nvcc --version | grep "release" || echo "Could not determine NVCC version"
	@echo "CUDA installation verified"

# Link the executable
$(TARGET): $(OBJECTS) $(OPTKIT_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(OPTKIT_LIB) $(LDFLAGS)

# Compile CUDA source files
%.o: %.cu solution.hh solution_patch.hh constants.hh
	$(NVCC) $(NVCCFLAGS) $(INCLUDES) -c $< -o $@

# Compile C++ source files
%.o: %.cc solution.hh solution_patch.hh constants.hh
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run the example
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJECTS)

# Rebuild from scratch
rebuild: clean all

# Check if OPTKIT library exists
check-lib:
	@if [ ! -f $(OPTKIT_LIB) ]; then \
		echo "Error: OPTKIT library not found at $(OPTKIT_LIB)"; \
		echo "Please build OPTKIT first by running 'make' in the root directory"; \
		exit 1; \
	fi

# Check GPU capabilities and system info
gpu-info:
	@echo "=== GPU Information ==="
	@if command -v nvidia-smi >/dev/null 2>&1; then \
		echo "GPU Details:"; \
		nvidia-smi --query-gpu=name,memory.total,compute_cap --format=csv,noheader,nounits; \
		echo ""; \
		echo "Driver Version:"; \
		nvidia-smi --query-gpu=driver_version --format=csv,noheader,nounits; \
	else \
		echo "nvidia-smi not available - GPU may not be present or drivers not installed"; \
	fi
	@echo ""
	@echo "CUDA Compiler:"
	@if [ -f $(CUDA_PATH)/bin/nvcc ]; then \
		$(CUDA_PATH)/bin/nvcc --version | head -4; \
	else \
		echo "NVCC not found"; \
	fi

# Show build environment
env-info: check-cuda gpu-info
	@echo ""
	@echo "=== Build Environment ==="
	@echo "CXX: $(CXX)"
	@echo "NVCC: $(NVCC)"
	@echo "CUDA_PATH: $(CUDA_PATH)"
	@echo "CUDA_LIB_DIR: $(CUDA_LIB_DIR)"
	@echo "NVML_LIB: $(NVML_LIB)"
	@echo "CXXFLAGS: $(CXXFLAGS)"
	@echo "NVCCFLAGS: $(NVCCFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"

# Phony targets
.PHONY: all run clean rebuild check-lib check-cuda gpu-info env-info