#!/bin/bash

CONFIG_FILE="./src/environment_config.hh"
ALIGN_WIDTH=50

echo "[⚙️ generate_environment_config.sh executing!]"
echo "[environment: CXX=${CXX:-g++}, INCLUDE_DIRS=${2:-$INCLUDE_DIRS}]"

CXX_COMPILER="${CXX:-g++}"
INCLUDE_DIRS="${2:-$INCLUDE_DIRS}"

# Truncate or create config file
> "$CONFIG_FILE"
echo -e "#pragma once" >> "$CONFIG_FILE"

# Utility: print aligned status
print_status() {
    printf "%-${ALIGN_WIDTH}s %s\n" "$1" "$2"
}

# Utility: Header check if exists or not
check_header() {
    local header="$1"
    local macro_name=$(basename "$header" | sed 's/\.[^.]*$//' | tr '[:lower:]' '[:upper:]')

    printf "%-${ALIGN_WIDTH}s" "Checking header <$header>:"
    echo "#include <$header>" | "$CXX_COMPILER" -E -x c++ - $INCLUDE_DIRS - > /dev/null 2>&1

    if [ $? -eq 0 ]; then
        echo " ✅"
        echo "#define OPTKIT_ENV_LIB_${macro_name}" >> "$CONFIG_FILE"
    else
        echo " ❌"
    fi
}

write_headers() {
    echo -e "\n// Headers" >> "$CONFIG_FILE"
    check_header "linux/perf_event.h"
    check_header "nvml.h"
    check_header "rocm_smi/rocm_smi.h"
    check_header "msr_safe.h"
}

write_compiler_macro() {
    echo -e "\n// Compiler" >> "$CONFIG_FILE"
    local base=$(basename "$CXX_COMPILER")
    local macro=$(echo "$base" | tr '[:lower:]' '[:upper:]' | sed 's/+/P/g')
    echo "#define OPTKIT_ENV_COMPILER_${macro}" >> "$CONFIG_FILE"
}

write_cpu_vendor() {
    echo -e "\n// CPU Vendor" >> "$CONFIG_FILE"
    if [ -f /proc/cpuinfo ]; then
        vendor=$(grep -m1 'vendor_id' /proc/cpuinfo | awk '{print $3}')
        case "$vendor" in
            GenuineIntel)
                print_status "Checking CPU Vendor:" "Intel"
                echo "#define OPTKIT_ENV_CPU_INTEL" >> "$CONFIG_FILE"
                ;;
            AuthenticAMD)
                print_status "Checking CPU Vendor:" "AMD"
                echo "#define OPTKIT_ENV_CPU_AMD" >> "$CONFIG_FILE"
                ;;
            *)
                print_status "Checking CPU Vendor:" "$vendor (Unknown)"
                ;;
        esac
    fi
}

write_cpu_arch() {
    echo -e "\n// CPU Architecture" >> "$CONFIG_FILE"
    arch=$(uname -m)
    case "$arch" in
        x86_64)
            print_status "Checking CPU Architecture:" "x86_64"
            echo "#define OPTKIT_ENV_CPU_ARCH_X86_64" >> "$CONFIG_FILE"
            ;;
        aarch64)
            print_status "Checking CPU Architecture:" "ARM"
            echo "#define OPTKIT_ENV_CPU_ARCH_ARM" >> "$CONFIG_FILE"
            ;;
        riscv64)
            print_status "Checking CPU Architecture:" "RISC-V"
            echo "#define OPTKIT_ENV_CPU_ARCH_RISCV64" >> "$CONFIG_FILE"
            ;;
        *)
            print_status "Checking CPU Architecture:" "$arch (Unknown)"
            ;;
    esac
}

write_cpu_topology() {
    # Number of sockets
    sockets=$(ls -d /sys/devices/system/cpu/cpu[0-9]* | \
        xargs -n1 -I{} cat {}/topology/physical_package_id 2>/dev/null | sort -u | wc -l)
    echo "#define OPTKIT_ENV_CPU_NUM_SOCKET $sockets" >> "$CONFIG_FILE"
    printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "OPTKIT_ENV_CPU_NUM_SOCKET" "$sockets"

    # Unique physical cores (package_id + core_id)
    cores=$(for cpu in /sys/devices/system/cpu/cpu[0-9]*; do
        pkg=$(<"$cpu/topology/physical_package_id")
        core=$(<"$cpu/topology/core_id")
        echo "$pkg-$core"
    done | sort -u | wc -l)

    # Cores per socket
    cores_per_socket=$((cores / sockets))
    echo "#define OPTKIT_ENV_CPU_PHYSICAL_CORES_PER_SOCKET $cores_per_socket" >> "$CONFIG_FILE"
    printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "OPTKIT_ENV_CPU_PHYSICAL_CORES_PER_SOCKET" "$cores_per_socket"

    # Logical CPUs (e.g. hyperthreads)
    logical=$(ls -d /sys/devices/system/cpu/cpu[0-9]* | wc -l)

    # Threads per core
    threads_per_core=$((logical / cores))
    echo "#define OPTKIT_CPU_ENV_THREADS_PER_CORE $threads_per_core" >> "$CONFIG_FILE"
    printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "OPTKIT_ENV_CPU_THREADS_PER_CORE" "$threads_per_core"

    # Total logical CPUs (like lscpu's "CPU(s)")
    total_logical=$((sockets * cores_per_socket * threads_per_core))
    echo "#define OPTKIT_CPU_ENV_TOTAL_LOGICAL_CPUS $total_logical" >> "$CONFIG_FILE"
    printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS" "$total_logical"
}

write_cpu_cache_info() {
    echo "" >> "$CONFIG_FILE"
    local llc_level=0 llc_name="" llc_size=0

    while read -r name value; do
        if [[ -n "$value" && "$value" != 0 ]]; then
            macro="OPTKIT_ENV_CPU_${name//[^A-Za-z0-9]/_}"
            printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "$macro" "$value"
            echo "#define $macro $value" >> "$CONFIG_FILE"

            if [[ "$name" == LEVEL*_CACHE_LINESIZE ]]; then
                level_num=$(echo "$name" | grep -oE '[0-9]+')
                if (( level_num > llc_level )); then
                    llc_level=$level_num
                    llc_name=$macro
                    llc_size=$value
                fi
            fi
        fi
    done < <(getconf -a | grep CACHE)

    if [[ -n "$llc_name" ]]; then
        printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "OPTKIT_ENV_CPU_LLC_CACHE_LINESIZE" "$llc_size"
        echo "#define OPTKIT_ENV_CPU_LLC_CACHE_LINESIZE $llc_name" >> "$CONFIG_FILE"
    fi
}

main() {
    write_headers
    write_compiler_macro
    ## CPU
    write_cpu_vendor
    write_cpu_arch
    write_cpu_topology
    write_cpu_cache_info

    echo "[✅ generate_environment_config.sh executed]"
}

main
