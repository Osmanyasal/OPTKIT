#!/bin/bash

SRC_CONFIG_FILE="./src/utils/environment_config.hh"
ALIGN_WIDTH=50

echo "[⚙️ generate_environment_config.sh executing!]"
echo "[environment: CXX=${CXX:-g++}, INCLUDE_DIRS=${2:-$INCLUDE_DIRS}]"

CXX_COMPILER="${CXX:-g++}"
INCLUDE_DIRS="${2:-$INCLUDE_DIRS}"

# Truncate or create config file
> "$SRC_CONFIG_FILE"
echo -e "#pragma once" >> "$SRC_CONFIG_FILE"

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
        echo "#define OPTKIT_ENV_LIB_${macro_name} 1" >> "$SRC_CONFIG_FILE"
    else
        echo "#define OPTKIT_ENV_LIB_${macro_name} 0" >> "$SRC_CONFIG_FILE"
        echo " ❌"
    fi
}

write_headers() {
    echo -e "\n// Headers" >> "$SRC_CONFIG_FILE"
    check_header "linux/perf_event.h"
    check_header "msr_safe.h"
    check_header "nvml.h"
    check_header "rocm_smi/rocm_smi.h"
}

write_compiler_macro() {
    echo -e "\n// Compiler" >> "$SRC_CONFIG_FILE"
    local base=$(basename "$CXX_COMPILER")
    local macro=$(echo "$base" | tr '[:lower:]' '[:upper:]' | sed 's/+/P/g')
    echo "#define OPTKIT_ENV_COMPILER_${macro}" >> "$SRC_CONFIG_FILE"
}
write_cpu_info() {
    echo -e "\n// CPU Vendor" >> "$SRC_CONFIG_FILE"

    # Default all vendors to 0
    echo "#define OPTKIT_ENV_CPU_INTEL 0" >> "$SRC_CONFIG_FILE"
    echo "#define OPTKIT_ENV_CPU_AMD 0" >> "$SRC_CONFIG_FILE"
    echo "#define OPTKIT_ENV_CPU_ARM 0" >> "$SRC_CONFIG_FILE" 
    echo "#define OPTKIT_ENV_CPU_RISCV 0" >> "$SRC_CONFIG_FILE" 
    echo "#define OPTKIT_ENV_CPU_MIPS 0" >> "$SRC_CONFIG_FILE"
    echo "#define OPTKIT_ENV_CPU_POWERPC 0" >> "$SRC_CONFIG_FILE"

    echo -e "\n// CPU Architecture" >> "$SRC_CONFIG_FILE"

    echo "#define OPTKIT_ENV_CPU_ARCH_X86_64 0" >> "$SRC_CONFIG_FILE" 
    echo "#define OPTKIT_ENV_CPU_ARCH_ARM32 0" >> "$SRC_CONFIG_FILE"
    echo "#define OPTKIT_ENV_CPU_ARCH_ARM64 0" >> "$SRC_CONFIG_FILE"
    echo "#define OPTKIT_ENV_CPU_ARCH_RISCV32 0" >> "$SRC_CONFIG_FILE"
    echo "#define OPTKIT_ENV_CPU_ARCH_RISCV64 0" >> "$SRC_CONFIG_FILE"
    echo "#define OPTKIT_ENV_CPU_ARCH_MIPS 0" >> "$SRC_CONFIG_FILE"
    echo "#define OPTKIT_ENV_CPU_ARCH_POWERPC 0" >> "$SRC_CONFIG_FILE"

    if [ -f /proc/cpuinfo ]; then
        arch=$(uname -m)
        vendor=$(grep -m1 'vendor_id' /proc/cpuinfo | awk '{print $3}')

        case "$arch" in
            x86_64|i386|i686)
                print_status "Checking CPU Architecture:" "x86_64"
                sed -i "s/^#define OPTKIT_ENV_CPU_ARCH_X86_64.*/#define OPTKIT_ENV_CPU_ARCH_X86_64 1/" "$SRC_CONFIG_FILE"

                case "$vendor" in
                    GenuineIntel)
                        print_status "Checking CPU Vendor:" "Intel"
                        sed -i "s/^#define OPTKIT_ENV_CPU_INTEL.*/#define OPTKIT_ENV_CPU_INTEL 1/" "$SRC_CONFIG_FILE"
                        ;;
                    AuthenticAMD)
                        print_status "Checking CPU Vendor:" "AMD"
                        sed -i "s/^#define OPTKIT_ENV_CPU_AMD.*/#define OPTKIT_ENV_CPU_AMD 1/" "$SRC_CONFIG_FILE"
                        ;;
                    *)
                        print_status "Checking CPU Vendor:" "Unknown x86 vendor: $vendor"
                        ;;
                esac
                ;;
            aarch64)
                print_status "Checking CPU Architecture:" "ARM64 (AArch64)"
                sed -i "s/^#define OPTKIT_ENV_CPU_ARM.*/#define OPTKIT_ENV_CPU_ARM 1/" "$SRC_CONFIG_FILE"
                sed -i "s/^#define OPTKIT_ENV_CPU_ARCH_ARM64.*/#define OPTKIT_ENV_CPU_ARCH_ARM64 1/" "$SRC_CONFIG_FILE"
                ;;
            armv7l|armv8l)
                print_status "Checking CPU Architecture:" "ARM32"
                sed -i "s/^#define OPTKIT_ENV_CPU_ARM.*/#define OPTKIT_ENV_CPU_ARM 1/" "$SRC_CONFIG_FILE"
                sed -i "s/^#define OPTKIT_ENV_CPU_ARCH_ARM32.*/#define OPTKIT_ENV_CPU_ARCH_ARM32 1/" "$SRC_CONFIG_FILE"
                ;;
            riscv64)
                print_status "Checking CPU Architecture:" "RISC-V 64-bit"
                sed -i "s/^#define OPTKIT_ENV_CPU_RISCV.*/#define OPTKIT_ENV_CPU_RISCV 1/" "$SRC_CONFIG_FILE"
                sed -i "s/^#define OPTKIT_ENV_CPU_ARCH_RISCV64.*/#define OPTKIT_ENV_CPU_ARCH_RISCV64 1/" "$SRC_CONFIG_FILE"
                ;;
            riscv32)
                print_status "Checking CPU Architecture:" "RISC-V 32-bit"
                sed -i "s/^#define OPTKIT_ENV_CPU_RISCV.*/#define OPTKIT_ENV_CPU_RISCV 1/" "$SRC_CONFIG_FILE"
                sed -i "s/^#define OPTKIT_ENV_CPU_ARCH_RISCV32.*/#define OPTKIT_ENV_CPU_ARCH_RISCV32 1/" "$SRC_CONFIG_FILE"
                ;;
            mips|mips64)
                print_status "Checking CPU Architecture:" "MIPS"
                sed -i "s/^#define OPTKIT_ENV_CPU_MIPS.*/#define OPTKIT_ENV_CPU_MIPS 1/" "$SRC_CONFIG_FILE"
                sed -i "s/^#define OPTKIT_ENV_CPU_ARCH_MIPS.*/#define OPTKIT_ENV_CPU_ARCH_MIPS 1/" "$SRC_CONFIG_FILE"
                ;;
            powerpc|ppc64|ppc64le)
                print_status "Checking CPU Architecture:" "PowerPC"
                sed -i "s/^#define OPTKIT_ENV_CPU_POWERPC.*/#define OPTKIT_ENV_CPU_POWERPC 1/" "$SRC_CONFIG_FILE"
                sed -i "s/^#define OPTKIT_ENV_CPU_ARCH_POWERPC.*/#define OPTKIT_ENV_CPU_ARCH_POWERPC 1/" "$SRC_CONFIG_FILE"
                ;;
            *)
                print_status "Checking CPU Architecture:" "Unknown architecture: $arch"
                ;;
        esac
    fi
}


write_cpu_topology() {

    echo -e "\n// CPU U-Architecture" >> "$SRC_CONFIG_FILE"

    if [ -f /proc/cpuinfo ]; then
        family=$(grep -m1 "cpu family" /proc/cpuinfo | awk '{print $4}')
        model=$(grep -m1 "model" /proc/cpuinfo | awk '{print $3}')

        # Convert to hexadecimal
        family_hex=$(printf "%X" "$family")
        model_hex=$(printf "%X" "$model")
        combined="${family_hex}_${model_hex}H"

        echo "#define OPTKIT_ENV_CPU_FAMILY 0x$family_hex" >> "$SRC_CONFIG_FILE"
        echo "#define OPTKIT_ENV_CPU_MODEL 0x$model_hex" >> "$SRC_CONFIG_FILE"
        echo "#define OPTKIT_ENV_CPU_FAMILY_MODEL $combined" >> "$SRC_CONFIG_FILE"

        printf "\t%-$(($ALIGN_WIDTH - 8))s 0x%s\n" "OPTKIT_ENV_CPU_FAMILY" "$family_hex"
        printf "\t%-$(($ALIGN_WIDTH - 8))s 0x%s\n" "OPTKIT_ENV_CPU_MODEL" "$model_hex"
        printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "OPTKIT_ENV_CPU_FAMILY_MODEL" "$combined"
    else
        echo "// Unable to determine CPU family/model" >> "$SRC_CONFIG_FILE"
    fi

    # Number of sockets
    sockets=$(ls -d /sys/devices/system/cpu/cpu[0-9]* | \
        xargs -n1 -I{} cat {}/topology/physical_package_id 2>/dev/null | sort -u | wc -l)
    echo "#define OPTKIT_ENV_CPU_NUM_SOCKETS $sockets" >> "$SRC_CONFIG_FILE"
    printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "OPTKIT_ENV_CPU_NUM_SOCKETS" "$sockets"

    # Unique physical cores (package_id + core_id)
    cores=$(for cpu in /sys/devices/system/cpu/cpu[0-9]*; do
        pkg=$(<"$cpu/topology/physical_package_id")
        core=$(<"$cpu/topology/core_id")
        echo "$pkg-$core"
    done | sort -u | wc -l)

    # Cores per socket
    cores_per_socket=$((cores / sockets))
    echo "#define OPTKIT_ENV_CPU_PHYSICAL_CORES_PER_SOCKET $cores_per_socket" >> "$SRC_CONFIG_FILE"
    printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "OPTKIT_ENV_CPU_PHYSICAL_CORES_PER_SOCKET" "$cores_per_socket"

    # Logical CPUs (e.g. hyperthreads)
    logical=$(ls -d /sys/devices/system/cpu/cpu[0-9]* | wc -l)

    # Threads per core
    threads_per_core=$((logical / cores))
    echo "#define OPTKIT_ENV_CPU_THREADS_PER_CORE $threads_per_core" >> "$SRC_CONFIG_FILE"
    printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "OPTKIT_ENV_CPU_THREADS_PER_CORE" "$threads_per_core"

    # Total logical CPUs (like lscpu's "CPU(s)")
    total_logical=$((sockets * cores_per_socket * threads_per_core))
    echo "#define OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS $total_logical" >> "$SRC_CONFIG_FILE"
    printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS" "$total_logical"
}

write_cpu_microarch() {

    # Reset all known uArch macros to 0
    for uarch in SKL SPR HSW KBL CFL ICL TGL RKL ADL RPL MTL \
                 ZEN ZENPLUS ZEN2 ZEN3 ZEN4 ZEN5 UNKNOWN; do
        echo "#define OPTKIT_ENV_CPU_MICROARCH_$uarch 0" >> "$SRC_CONFIG_FILE"
    done

    # Get pre-defined family_model from header file
    fm_model_hex=$(grep '#define OPTKIT_ENV_CPU_FAMILY_MODEL' "$SRC_CONFIG_FILE" | awk '{ print $3 }')

    # Match known uArch
    case "$fm_model_hex" in
        6_5EH)   uarch="SKL" ;;
        6_9EH)   uarch="KBL" ;;
        6_8EH)   uarch="CFL" ;;
        6_7EH)   uarch="ICL" ;;
        6_8CH)   uarch="TGL" ;;
        6_A7H)   uarch="RKL" ;;
        6_97H)   uarch="ADL" ;;
        6_AFH)   uarch="RPL" ;;
        6_B7H)   uarch="MTL" ;;
        17_01H)  uarch="ZEN" ;;
        17_08H)  uarch="ZENPLUS" ;;
        17_18H)  uarch="ZEN2" ;;
        19_21H)  uarch="ZEN3" ;;
        19_61H)  uarch="ZEN4" ;;
        1E_00H)  uarch="ZEN5" ;;  # Hypothetical
    esac

    if [ -n "$uarch" ]; then
        sed -i "s/^#define OPTKIT_ENV_CPU_MICROARCH_$uarch 0/#define OPTKIT_ENV_CPU_MICROARCH_$uarch 1/" "$SRC_CONFIG_FILE"
        print_status "Detected microarchitecture:" "$uarch"
    else
        print_status "Detected microarchitecture:" "Unknown ($fm_model_hex)"
    fi
}


write_cpu_cache_info() {
    echo "" >> "$SRC_CONFIG_FILE"
    local llc_level=0 llc_name="" llc_size=0

    while read -r name value; do
        if [[ -n "$value" && "$value" != 0 ]]; then
            macro="OPTKIT_ENV_CPU_${name//[^A-Za-z0-9]/_}"
            printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "$macro" "$value"
            echo "#define $macro $value" >> "$SRC_CONFIG_FILE"

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
        echo "#define OPTKIT_ENV_CPU_LLC_CACHE_LINESIZE $llc_name" >> "$SRC_CONFIG_FILE"
    fi
}

main() {
    write_headers
    write_compiler_macro
    ## CPU
    write_cpu_info
    write_cpu_topology
    write_cpu_microarch
    write_cpu_cache_info

    echo "[✅ generate_environment_config.sh executed]"
    # cp $SRC_CONFIG_FILE ./test/utils/    ## copy this to test directory.
}

main
