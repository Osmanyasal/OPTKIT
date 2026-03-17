CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall -Wextra -fopenmp
LDFLAGS = -lpthread -ldl

# Paths
OPTKIT_ROOT = ../..
OPTKIT_LIB = $(OPTKIT_ROOT)/bin/Release/liboptkit_static.a
OPTKIT_INCLUDE = $(OPTKIT_ROOT)/src
SPDLOG_INCLUDE = $(OPTKIT_ROOT)/lib/spdlog/include
LIBPFM4_INCLUDE = $(OPTKIT_ROOT)/lib/libpfm4/include

# Detect CUDA/NVML installation
CUDA_INCLUDE := $(shell if [ -d /usr/local/cuda/include ]; then echo /usr/local/cuda/include; elif [ -d /usr/local/cuda-12.9/include ]; then echo /usr/local/cuda-12.9/include; elif [ -d /usr/local/cuda/targets/x86_64-linux/include ]; then echo /usr/local/cuda/targets/x86_64-linux/include; fi)

# Detect ROCm/AMD SMI installation
ROCM_INCLUDE := $(shell if [ -d /opt/rocm/include ]; then echo /opt/rocm/include; elif [ -d /opt/rocm-5.7.1/include ]; then echo /opt/rocm-5.7.1/include; fi)

INCLUDES = -I$(OPTKIT_INCLUDE) \
           -I$(SPDLOG_INCLUDE) \
           -I$(LIBPFM4_INCLUDE)

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

TARGET = dummy_workload
SOURCES = main.cc
OBJECTS = $(SOURCES:.cc=.o)

all: $(TARGET)

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
		echo "Please build OPTKIT first by running 'make -j$(nproc) config=release optkit_static' in the repo root."; \
		exit 1; \
	fi

.PHONY: all run clean rebuild check-lib
