CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

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
			if [ -d "$$cuda_dir" ] && [ -f "$$cuda_dir/include/nvml.h" ]; then \
				echo $$cuda_dir; \
				break; \
			fi; \
		done; \
	fi)

ifndef CUDA_PATH
$(error CUDA installation not found. Please install CUDA toolkit or set CUDA_HOME/CUDA_PATH environment variable.)
endif

OPTKIT_ROOT = ../..
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

INCLUDES = -I$(OPTKIT_INCLUDE) \
           -I$(SPDLOG_INCLUDE) \
	       -I$(LIBPFM4_INCLUDE) \
	       -I$(CUDA_PATH)/include \
	       -I$(CUDA_PATH)/extras/CUPTI/include

ifneq ($(ROCM_INCLUDE),)
	INCLUDES += -I$(ROCM_INCLUDE)
endif

CUDA_LIB_DIR := $(shell \
	if [ -d $(CUDA_PATH)/lib64 ]; then \
		echo $(CUDA_PATH)/lib64; \
	elif [ -d $(CUDA_PATH)/lib ]; then \
		echo $(CUDA_PATH)/lib; \
	else \
		echo $(CUDA_PATH)/lib64; \
	fi)

LDFLAGS = -lpthread -ldl -L$(CUDA_LIB_DIR) -lcudart -lcupti

NVML_LIB := $(shell \
	if [ -f $(CUDA_LIB_DIR)/libnvidia-ml.so ] || [ -f /usr/lib/x86_64-linux-gnu/libnvidia-ml.so ]; then \
		echo -lnvidia-ml; \
	fi)
ifneq ($(NVML_LIB),)
	LDFLAGS += $(NVML_LIB)
endif

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

TARGET = riscv_perf_example
SOURCES = main.cc
OBJECTS = $(SOURCES:.cc=.o)

all: check-lib $(TARGET)

$(TARGET): $(OBJECTS) $(OPTKIT_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(OPTKIT_LIB) $(LDFLAGS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)

rebuild: clean all

check-lib:
	@if [ ! -f $(OPTKIT_LIB) ]; then \
		echo "Error: OPTKIT library not found at $(OPTKIT_LIB)"; \
		echo "Please build OPTKIT first by running 'make -j$$(nproc) config=release optkit_static' in the repo root."; \
		exit 1; \
	fi

.PHONY: all run clean rebuild check-lib