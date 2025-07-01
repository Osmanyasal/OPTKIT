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

    ## Following grouping is constructed from https://en.wikichip.org/wiki/intel/cpuid

    # Reset all known uArch macros to 0
    for uarch in SPR EMR GRN SKL KBL CFL CML ICL TGL RKL ADL RPL MTL HSW BDW SFR NHM WSM SNB IVB SLM TMT KNL KNM MER P6\
                 ZEN ZENPLUS ZEN2 ZEN3 ZEN4 ZEN5 UNKNOWN; do
        echo "#define OPTKIT_ENV_CPU_MICROARCH_$uarch 0" >> "$SRC_CONFIG_FILE"
    done

    # Get pre-defined family_model from header file
    fm_model_hex=$(grep '#define OPTKIT_ENV_CPU_FAMILY_MODEL' "$SRC_CONFIG_FILE" | awk '{ print $3 }')

    # Match known uArch
    case "$fm_model_hex" in

        ## INTEL CPUs
        6_8FH)                    uarch="SPR" ;; # Sapphire Rapids
        6_CFH)                    uarch="EMR" ;; # Emerald Rapids (successor of Sapphire Rapids)
        6_ADH|6_AEH)              uarch="GRN" ;; # Granite Rapids (successor of Sapphire Rapids)

        6_55H|6_4EH|6_5EH)        uarch="SKL" ;; # Skylake
        6_8EH)                    uarch="KBL" ;;        # Kaby Lake (7th gen)
        6_9EH)                    uarch="CFL" ;;        # Coffee Lake (8th/9th gen)
        6_A5H|6_A6H)              uarch="CML" ;; # CometLake (successor of CoffeeLake)

        6_6AH|6_6CH|6_7DH|6_7EH)  uarch="ICL" ;; # IceLake
        6_8CH|6_8DH)              uarch="TGL" ;; # TigerLake (Successor of IceLake)
        6_A7H)                    uarch="RKL" ;; # RocketLake (successor of TigerLake)
        6_97H|6_9AH)              uarch="ADL" ;; # AlderLake (Both IceLake and TigerLake)
        6_B7H|6_BAH|6_BEH|6_BFH)  uarch="RPL" ;; # RaptorLake (sucessor to AlderLake)
        6_AAH|6_ABH|6_ACH)        uarch="MTL" ;; # MeteorLake (successor to RaptorLake)
        

        6_3FH|6_3CH|6_45H|6_46H)  uarch="HSW" ;; # Haswell
        6_4FH|6_56H|6_3DH|6_47H)  uarch="BDW" ;; # Broadwell
        6_AFH)                    uarch="SFR" ;; # Sierra Forest 
        6_1AH|6_1EH|6_1FH|6_2EH)  uarch="NHM" ;; # Nehalem
        6_25H|6_2CH|6_2FH)        uarch="WSM" ;; # Westmere

        6_2AH|6_2DH)              uarch="SNB" ;; # Sandy Bridge
        6_3AH|6_3EH)              uarch="IVB" ;; # Ivy Bridge

        6_5DH|6_5AH|6_4DH|6_4AH|6_37H)    uarch="SLM" ;; # Silvermont
        6_8AH|6_96H|6_9CH)        uarch="TMT" ;; # Tremont

        6_57H)                    uarch="KNL" ;; # KNL Knights Landing
        6_85H)                    uarch="KNM" ;; # Knights Mill (KNL based)

        6_FH|6_16H)               uarch="MER" ;; # Merom/Penryn (Core 2 era) (based on core pmu architecture)
        6_9H|6_AH|6_DH|6_15H)     uarch="P6"  ;; # P6 (Pentium Pro/MII/III)

        ## AMD CPUs
        17_1H|17_11H|17_18H|17_20H)  uarch="ZEN" ;; # Zen
        17_8h|17_18H)                uarch="ZENPLUS" ;; # Zen+
        17_31H|17_47H|17_60H\
        |17_68H|17_71H|17_90H\
        |17_98H|17_AAH)           uarch="ZEN2" ;; # Zen 2

        19_00H|19_01H|19_10H\
        |19_12H|19_31H|19_40H\
        |19_41H|19_50H)            uarch="ZEN3" ;; # Zen 3

        19_AAH|19_90H|19_80H\
        |19_7CH|19_78H|19_75H\
        |19_74H|19_61H|19_18H\
        |19_11H|19_10H)           uarch="ZEN4" ;; # Zen 4 family variants

        1A_00H|1A_20H)            uarch="ZEN5" ;; # Zen 5
        *)                        uarch="UNKNOWN" ;;
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
