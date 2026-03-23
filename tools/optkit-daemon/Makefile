CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall -Wextra -fopenmp
LDFLAGS = -lpthread -ldl

# Paths
OPTKIT_ROOT = ../..
OPTKIT_LIB = $(OPTKIT_ROOT)/bin/Release/liboptkit_static.a
OPTKIT_INCLUDE = $(OPTKIT_ROOT)/src
SPDLOG_INCLUDE = $(OPTKIT_ROOT)/lib/spdlog/include
LIBPFM4_INCLUDE = $(OPTKIT_ROOT)/lib/libpfm4/include

# Detect ONNX Runtime installation
ONNX_INCLUDE := $(shell if [ -f /usr/include/onnxruntime_cxx_api.h ]; then echo /usr/include; elif [ -f /usr/local/include/onnxruntime_cxx_api.h ]; then echo /usr/local/include; elif [ -d /usr/include/onnxruntime ]; then echo /usr/include/onnxruntime; elif [ -d /usr/local/include/onnxruntime ]; then echo /usr/local/include/onnxruntime; elif [ -d /opt/onnxruntime/include ]; then echo /opt/onnxruntime/include; fi)
ONNX_LIB_DIR := $(shell if [ -d /usr/lib/x86_64-linux-gnu ]; then echo /usr/lib/x86_64-linux-gnu; elif [ -d /usr/local/lib ]; then echo /usr/local/lib; elif [ -d /opt/onnxruntime/lib ]; then echo /opt/onnxruntime/lib; fi)

# Optional ONNX support (auto-detected by default)
HAVE_ONNX := $(shell if [ -n "$(ONNX_INCLUDE)" ] && [ -n "$(ONNX_LIB_DIR)" ] && [ -f "$(ONNX_LIB_DIR)/libonnxruntime.so" ]; then echo 1; else echo 0; fi)
WITH_ONNX ?= $(HAVE_ONNX)

# Detect CUDA/NVML installation
CUDA_INCLUDE := $(shell if [ -d /usr/local/cuda/include ]; then echo /usr/local/cuda/include; elif [ -d /usr/local/cuda-12.9/include ]; then echo /usr/local/cuda-12.9/include; elif [ -d /usr/local/cuda/targets/x86_64-linux/include ]; then echo /usr/local/cuda/targets/x86_64-linux/include; fi)

# Detect ROCm/AMD SMI installation
ROCM_INCLUDE := $(shell if [ -d /opt/rocm/include ]; then echo /opt/rocm/include; elif [ -d /opt/rocm-5.7.1/include ]; then echo /opt/rocm-5.7.1/include; fi)

# Include directories
INCLUDES = -I$(OPTKIT_INCLUDE) \
           -I$(SPDLOG_INCLUDE) \
           -I$(LIBPFM4_INCLUDE)

# Add ONNX Runtime include and library (only when enabled)
ifeq ($(WITH_ONNX),1)
	ifeq ($(HAVE_ONNX),0)
		$(error ONNX Runtime not found (headers or libonnxruntime.so). Install it or build with WITH_ONNX=0)
	endif
	INCLUDES += -I$(ONNX_INCLUDE)
	LDFLAGS += -L$(ONNX_LIB_DIR) -Wl,-rpath,$(ONNX_LIB_DIR) -lonnxruntime
endif

# Add CUDA include and library if found
ifneq ($(CUDA_INCLUDE),)
    INCLUDES += -I$(CUDA_INCLUDE)
    LDFLAGS += -lnvidia-ml
endif

# Add ROCm include and library if found
ifneq ($(ROCM_INCLUDE),)
    INCLUDES += -I$(ROCM_INCLUDE)
    LDFLAGS += -L/opt/rocm/lib -lamd_smi
endif

# Targets
TARGET = optkit-daemon

ifeq ($(WITH_ONNX),1)
	SOURCES = main.cc onnx_api.cc
else
	SOURCES = main.cc
endif
OBJECTS = $(SOURCES:.cc=.o)

# Default target
all: check-lib $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS) $(OPTKIT_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(OPTKIT_LIB) $(LDFLAGS)

# Compile source files
%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run the CLI tool
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

# Check if ONNX Runtime is available
check-onnx:
	@if [ "$(HAVE_ONNX)" != "1" ]; then \
		echo "Error: ONNX Runtime not found"; \
		echo "Expected headers under: /usr/include, /usr/local/include, /usr/include/onnxruntime, /usr/local/include/onnxruntime, /opt/onnxruntime/include"; \
		echo "Expected libonnxruntime.so under: /usr/lib/x86_64-linux-gnu, /usr/local/lib, /opt/onnxruntime/lib"; \
		exit 1; \
	fi

# Show build environment
env-info:
	@echo "=== Build Environment ==="
	@echo "CXX: $(CXX)"
	@echo "CXXFLAGS: $(CXXFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "OPTKIT_LIB: $(OPTKIT_LIB)"
	@echo "CUDA_INCLUDE: $(CUDA_INCLUDE)"
	@echo "ROCM_INCLUDE: $(ROCM_INCLUDE)"
	@echo "ONNX_INCLUDE: $(ONNX_INCLUDE)"
	@echo "ONNX_LIB_DIR: $(ONNX_LIB_DIR)"
	@echo "HAVE_ONNX: $(HAVE_ONNX)"
	@echo "WITH_ONNX: $(WITH_ONNX)"
	@echo "INCLUDES: $(INCLUDES)"

# Check GPU support
gpu-info:
	@echo "=== GPU Support Check ==="
	@if [ -n "$(CUDA_INCLUDE)" ]; then \
		echo "NVIDIA CUDA/NVML: Found at $(CUDA_INCLUDE)"; \
		if command -v nvidia-smi >/dev/null 2>&1; then \
			echo "nvidia-smi available"; \
		else \
			echo "nvidia-smi not available"; \
		fi; \
	else \
		echo "NVIDIA CUDA/NVML: Not found"; \
	fi
	@if [ -n "$(ROCM_INCLUDE)" ]; then \
		echo "AMD ROCm: Found at $(ROCM_INCLUDE)"; \
		if command -v rocm-smi >/dev/null 2>&1; then \
			echo "rocm-smi available"; \
		else \
			echo "rocm-smi not available"; \
		fi; \
	else \
		echo "AMD ROCm: Not found"; \
	fi

# Install the binary (optional)
install: $(TARGET)
	@echo "Installing $(TARGET) to /usr/local/bin/"
	sudo cp $(TARGET) /usr/local/bin/

# Uninstall the binary (optional)
uninstall:
	@echo "Removing $(TARGET) from /usr/local/bin/"
	sudo rm -f /usr/local/bin/$(TARGET)

# Do not keep intermediate object files after a successful build
.INTERMEDIATE: $(OBJECTS)

# If a command fails, delete the target to avoid leaving partial artifacts
.DELETE_ON_ERROR:

# Phony targets
.PHONY: all run clean rebuild check-lib check-onnx env-info gpu-info install uninstall