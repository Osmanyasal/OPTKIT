#!/bin/bash

echo "[⚙️ generate_environment_config.sh executing!]"
echo "[environment: CXX=${CXX:-g++}, INCLUDE_DIRS=${2:-$INCLUDE_DIRS}]"

CONFIG_FILE="./src/environment_config.hh"
ALIGN_WIDTH=50  # Width for aligned status messages

> "$CONFIG_FILE"  # Truncate file
echo -e "#pragma once" >> $CONFIG_FILE

CXX_COMPILER="${CXX:-g++}"  # Global compiler variable
INCLUDE_DIRS="${2:-$INCLUDE_DIRS}"  # Use second arg, else fallback to env var

printf "%-${ALIGN_WIDTH}s %s\n" "Checking Compiler:" "$CXX_COMPILER"

check_header() {
    local header="$1" 
    local header_name=$(basename "$header" | sed 's/\.[^.]*$//' | tr '[:lower:]' '[:upper:]')

    printf "%-${ALIGN_WIDTH}s" "Checking header <$header>:"
    echo "#include <$header>" | "$CXX_COMPILER" -E -x c++ - $INCLUDE_DIRS - > /dev/null 2>&1

    if [ $? -eq 0 ]; then
        echo " ✅"
        echo "#define OPTKIT_LIB_$(basename $header_name)" >> "$CONFIG_FILE"
        return 0
    else
        echo " ❌"
        return 1
    fi
}

compiler_basename=$(basename "$CXX_COMPILER") # get basename of the compiler (if path specified)
compiler_macro=$(echo "$compiler_basename" | tr '[:lower:]' '[:upper:]' | sed 's/+/P/g') # Replace '+' with 'P', keep the rest, and capitalize everything

echo -e "\n// Compiler" >> "$CONFIG_FILE"
echo "#define OPTKIT_COMPILER_$compiler_macro" >> "$CONFIG_FILE"

echo -e "\n//CPU vendor" >> $CONFIG_FILE
if [ -f /proc/cpuinfo ]; then
    cpu_vendor=$(grep -m 1 'vendor_id' /proc/cpuinfo | awk '{print $3}')
    if [ "$cpu_vendor" = "GenuineIntel" ]; then
        printf "%-${ALIGN_WIDTH}s %s\n" "Checking CPU Vendor:" "Intel"
        echo "#define OPTKIT_CPU_INTEL" >> "$CONFIG_FILE"
    elif [ "$cpu_vendor" = "AuthenticAMD" ]; then
        printf "%-${ALIGN_WIDTH}s %s\n" "Checking CPU Vendor:" "AMD"
        echo "#define OPTKIT_CPU_AMD" >> "$CONFIG_FILE"
    else
        printf "%-${ALIGN_WIDTH}s %s\n" "Checking CPU Vendor:" "$cpu_vendor (Unknown)"
    fi
fi

echo -e "\n//CPU Architecture" >> $CONFIG_FILE
arch_type=$(uname -m)
if [ "$arch_type" = "x86_64" ]; then
    printf "%-${ALIGN_WIDTH}s %s\n" "Checking CPU Architecture:" "x86_64"
    echo "#define OPTKIT_CPU_ARCH_X86_64" >> "$CONFIG_FILE"
elif [ "$arch_type" = "aarch64" ]; then
    printf "%-${ALIGN_WIDTH}s %s\n" "Checking CPU Architecture:" "ARM"
    echo "#define OPTKIT_CPU_ARCH_ARM" >> "$CONFIG_FILE"
elif [ "$arch_type" = "riscv64" ]; then
    printf "%-${ALIGN_WIDTH}s %s\n" "Checking CPU Architecture:" "RISC-V"
    echo "#define OPTKIT_CPU_ARCH_RISCV64" >> "$CONFIG_FILE"
else
    printf "%-${ALIGN_WIDTH}s %s\n" "Checking CPU Architecture:" "$arch_type (Unknown)"
fi


echo -e "\n//Headers" >> $CONFIG_FILE

check_header "linux/perf_event.h"
check_header "nvml.h"
check_header "rocm_smi/rocm_smi.h"
check_header "msr_safe.h"

echo "[generate_environment_config.sh executed!]"
