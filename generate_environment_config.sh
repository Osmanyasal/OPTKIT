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
    check_header "/usr/local/cuda/include/nvml.h"
    check_header "/opt/rocm/include/rocm_smi/rocm_smi.h"
    check_header "/usr/include/rocm_smi/rocm_smi.h"
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
        # Detect architecture first
        arch=$(uname -m)
        
        if [[ "$arch" == "x86_64" || "$arch" == "i386" || "$arch" == "i686" ]]; then
            # x86/x86_64 architecture
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
        elif [[ "$arch" == "aarch64" || "$arch" == "arm64" || "$arch" == "armv7l" || "$arch" == "armv8l" ]]; then
            # ARM architecture
            cpu_part=$(grep -m1 "CPU part" /proc/cpuinfo | awk '{print $4}' | sed 's/0x//')
            cpu_implementer=$(grep -m1 "CPU implementer" /proc/cpuinfo | awk '{print $4}' | sed 's/0x//')
            cpu_variant=$(grep -m1 "CPU variant" /proc/cpuinfo | awk '{print $4}' | sed 's/0x//' | head -c 1)
            
            # Create ARM-specific identifier
            if [ -n "$cpu_implementer" ] && [ -n "$cpu_part" ]; then
                # Use implementer and part as family/model equivalent
                family_hex=$(printf "%X" "0x$cpu_implementer")
                model_hex="$cpu_part"
                combined="ARM_${cpu_implementer}_${cpu_part}H"
                
                echo "#define OPTKIT_ENV_CPU_FAMILY 0x$family_hex" >> "$SRC_CONFIG_FILE"
                echo "#define OPTKIT_ENV_CPU_MODEL 0x$model_hex" >> "$SRC_CONFIG_FILE"
                echo "#define OPTKIT_ENV_CPU_FAMILY_MODEL $combined" >> "$SRC_CONFIG_FILE"
                echo "#define OPTKIT_ENV_CPU_ARM_IMPLEMENTER 0x$cpu_implementer" >> "$SRC_CONFIG_FILE"
                echo "#define OPTKIT_ENV_CPU_ARM_PART 0x$cpu_part" >> "$SRC_CONFIG_FILE"
                
                printf "\t%-$(($ALIGN_WIDTH - 8))s 0x%s\n" "OPTKIT_ENV_CPU_FAMILY" "$family_hex"
                printf "\t%-$(($ALIGN_WIDTH - 8))s 0x%s\n" "OPTKIT_ENV_CPU_MODEL" "$model_hex"
                printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "OPTKIT_ENV_CPU_FAMILY_MODEL" "$combined"
                printf "\t%-$(($ALIGN_WIDTH - 8))s 0x%s\n" "OPTKIT_ENV_CPU_ARM_IMPLEMENTER" "$cpu_implementer"
                printf "\t%-$(($ALIGN_WIDTH - 8))s 0x%s\n" "OPTKIT_ENV_CPU_ARM_PART" "$cpu_part"
            else
                echo "// Unable to determine ARM CPU implementer/part" >> "$SRC_CONFIG_FILE"
            fi
        elif [[ "$arch" == "riscv64" || "$arch" == "riscv32" ]]; then
            # RISC-V architecture
            isa=$(grep -m1 "isa" /proc/cpuinfo | awk '{print $3}')
            uarch=$(grep -m1 "uarch" /proc/cpuinfo | awk '{print $3}')
            mvendorid=$(grep -m1 "mvendorid" /proc/cpuinfo | awk '{print $3}' | sed 's/0x//')
            marchid=$(grep -m1 "marchid" /proc/cpuinfo | awk '{print $3}' | sed 's/0x//')
            mimpid=$(grep -m1 "mimpid" /proc/cpuinfo | awk '{print $3}' | sed 's/0x//')
            
            # Create RISC-V specific identifier
            if [ -n "$mvendorid" ] && [ -n "$marchid" ]; then
                # Use mvendorid and marchid as family/model equivalent
                family_hex=$(printf "%X" "0x$mvendorid" 2>/dev/null || echo "$mvendorid")
                model_hex=$(printf "%X" "0x$marchid" 2>/dev/null || echo "$marchid")
                combined="RISCV_${mvendorid}_${marchid}H"
                
                echo "#define OPTKIT_ENV_CPU_FAMILY 0x$family_hex" >> "$SRC_CONFIG_FILE"
                echo "#define OPTKIT_ENV_CPU_MODEL 0x$model_hex" >> "$SRC_CONFIG_FILE"
                echo "#define OPTKIT_ENV_CPU_FAMILY_MODEL $combined" >> "$SRC_CONFIG_FILE"
                echo "#define OPTKIT_ENV_CPU_RISCV_MVENDORID 0x$mvendorid" >> "$SRC_CONFIG_FILE"
                echo "#define OPTKIT_ENV_CPU_RISCV_MARCHID 0x$marchid" >> "$SRC_CONFIG_FILE"
                
                if [ -n "$mimpid" ]; then
                    echo "#define OPTKIT_ENV_CPU_RISCV_MIMPID 0x$mimpid" >> "$SRC_CONFIG_FILE"
                fi
                
                if [ -n "$isa" ]; then
                    echo "#define OPTKIT_ENV_CPU_RISCV_ISA \"$isa\"" >> "$SRC_CONFIG_FILE"
                fi
                
                if [ -n "$uarch" ]; then
                    echo "#define OPTKIT_ENV_CPU_RISCV_UARCH \"$uarch\"" >> "$SRC_CONFIG_FILE"
                fi
                
                printf "\t%-$(($ALIGN_WIDTH - 8))s 0x%s\n" "OPTKIT_ENV_CPU_FAMILY" "$family_hex"
                printf "\t%-$(($ALIGN_WIDTH - 8))s 0x%s\n" "OPTKIT_ENV_CPU_MODEL" "$model_hex"
                printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "OPTKIT_ENV_CPU_FAMILY_MODEL" "$combined"
                printf "\t%-$(($ALIGN_WIDTH - 8))s 0x%s\n" "OPTKIT_ENV_CPU_RISCV_MVENDORID" "$mvendorid"
                printf "\t%-$(($ALIGN_WIDTH - 8))s 0x%s\n" "OPTKIT_ENV_CPU_RISCV_MARCHID" "$marchid"
                
                if [ -n "$mimpid" ]; then
                    printf "\t%-$(($ALIGN_WIDTH - 8))s 0x%s\n" "OPTKIT_ENV_CPU_RISCV_MIMPID" "$mimpid"
                fi
                
                if [ -n "$isa" ]; then
                    printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "OPTKIT_ENV_CPU_RISCV_ISA" "$isa"
                fi
                
                if [ -n "$uarch" ]; then
                    printf "\t%-$(($ALIGN_WIDTH - 8))s %s\n" "OPTKIT_ENV_CPU_RISCV_UARCH" "$uarch"
                fi
            else
                echo "// Unable to determine RISC-V CPU vendor/arch IDs" >> "$SRC_CONFIG_FILE"
            fi
        else
            echo "// Unsupported architecture: $arch" >> "$SRC_CONFIG_FILE"
        fi
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

    # Reset all known uArch macros to 0 (including ARM and RISC-V architectures)
    for uarch in SPR EMR GRN SKL KBL CFL CML ICL TGL RKL ADL RPL MTL HSW BDW SFR NHM WSM SNB IVB SLM TMT KNL KNM MER P6\
                 ZEN ZENPLUS ZEN2 ZEN3 ZEN4 ZEN5 \
                 ARMV7_A8 ARMV7_A9 ARMV7_A15 ARMV8_A57 ARMV8_A53 ARMV8_A55 ARMV8_A72 ARMV8_A76 \
                 XGENE KRAIT A64FX NEOVERSE_V1 NEOVERSE_V2 NEOVERSE_V3 NEOVERSE_N1 NEOVERSE_N2 NEOVERSE_N3 KUNPENG920 \
                 SIFIVE_U74 SIFIVE_U54 SIFIVE_E31 SIFIVE_E21 SIFIVE_S76 SIFIVE_P550 SIFIVE_P270 \
                 THEAD_C906 THEAD_C910 THEAD_C920 CANAAN_K210 STARFIVE_U74 ANDES_A25 ANDES_AX45 \
                 VENTANA_VEYRON_V1 TENSTORRENT_ASCALON ESPRESSIF_C3 BOUFFALOLAB_T_HEAD \
                 UNKNOWN; do
        echo "#define OPTKIT_ENV_CPU_MICROARCH_$uarch 0" >> "$SRC_CONFIG_FILE"
    done

    # Detect architecture
    arch=$(uname -m)
    
    if [[ "$arch" == "x86_64" || "$arch" == "i386" || "$arch" == "i686" ]]; then
        # x86/x86_64 microarchitecture detection
        
        # Get pre-defined family_model from header file
        fm_model_hex=$(grep '#define OPTKIT_ENV_CPU_FAMILY_MODEL' "$SRC_CONFIG_FILE" | awk '{ print $3 }')

        # Match known uArch
        case "$fm_model_hex" in

            ## INTEL CPUs
            6_8FH)                    uarch="SPR" ;; # Sapphire Rapids
            6_CFH)                    uarch="EMR" ;; # Emerald Rapids (successor of Sapphire Rapids)
            6_ADH|6_AEH)              uarch="GRN" ;; # Granite Rapids (successor of Sapphire Rapids)

            6_55H|6_4EH|6_5EH)        uarch="SKL" ;; # Skylake
            6_8EH)                    uarch="KBL" ;; # Kaby Lake (7th gen) (successor of Skylake)
            6_9EH)                    uarch="CFL" ;; # Coffee Lake (8th/9th gen) (successor of Kaby Lake)
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

            19_1H|19_10H\
            |19_12H|19_31H|19_40H\
            |19_41H|19_50H)            uarch="ZEN3" ;; # Zen 3

            19_AAH|19_90H|19_80H\
            |19_7CH|19_78H|19_75H\
            |19_74H|19_61H|19_18H\
            |19_11H|19_10H)           uarch="ZEN4" ;; # Zen 4 family variants

            1A_00H|1A_20H)            uarch="ZEN5" ;; # Zen 5
            *)                        uarch="UNKNOWN" ;;
        esac



        # Map of predecessors: key = uarch, value = predecessor uarch or empty if none
        declare -A predecessor=(
            [EMR]="SPR"
            [GRN]="EMR"
            [KBL]="SKL"
            [CFL]="KBL"
            [CML]="CFL"
            [TGL]="ICL"
            [RKL]="TGL"
            [ADL]="RKL"
            [RPL]="ADL"
            [MTL]="RPL"
        )

    elif [[ "$arch" == "aarch64" || "$arch" == "arm64" || "$arch" == "armv7l" || "$arch" == "armv8l" ]]; then
        # ARM microarchitecture detection
        
        cpu_part=$(grep -m1 "CPU part" /proc/cpuinfo | awk '{print $4}' | sed 's/0x//')
        cpu_implementer=$(grep -m1 "CPU implementer" /proc/cpuinfo | awk '{print $4}' | sed 's/0x//')
        
        # ARM microarchitecture detection based on implementer and part
        case "${cpu_implementer}_${cpu_part}" in
            ## ARM Ltd (0x41) - Official ARM designs
            "41_c08")                 uarch="ARMV7_A8" ;;   # Cortex-A8
            "41_c09")                 uarch="ARMV7_A9" ;;   # Cortex-A9
            "41_c0f")                 uarch="ARMV7_A15" ;;  # Cortex-A15
            "41_d07")                 uarch="ARMV8_A57" ;;  # Cortex-A57
            "41_d03")                 uarch="ARMV8_A53" ;;  # Cortex-A53
            "41_d05")                 uarch="ARMV8_A55" ;;  # Cortex-A55
            "41_d08")                 uarch="ARMV8_A72" ;;  # Cortex-A72
            "41_d0b")                 uarch="ARMV8_A76" ;;  # Cortex-A76
            "41_d40")                 uarch="NEOVERSE_V1" ;; # Neoverse V1
            "41_d4f")                 uarch="NEOVERSE_V2" ;; # Neoverse V2
            "41_d84")                 uarch="NEOVERSE_V3" ;; # Neoverse V3
            "41_d0c")                 uarch="NEOVERSE_N1" ;; # Neoverse N1
            "41_d49")                 uarch="NEOVERSE_N2" ;; # Neoverse N2
            "41_d8e")                 uarch="NEOVERSE_N3" ;; # Neoverse N3
            
            ## Applied Micro (0x50) - X-Gene
            "50_000")                 uarch="XGENE" ;;      # X-Gene
            
            ## Qualcomm (0x51) - Krait
            "51_00f"|"51_02d"|"51_04d"|"51_06f")  uarch="KRAIT" ;;  # Krait variants
            
            ## Fujitsu (0x46) - A64FX
            "46_001")                 uarch="A64FX" ;;      # A64FX
            
            ## HiSilicon (0x48) - Kunpeng
            "48_d01")                 uarch="KUNPENG920" ;; # Kunpeng 920
            
            *)                        uarch="UNKNOWN" ;;
        esac

        # ARM predecessor relationships (simpler than x86)
        declare -A predecessor=(
            [ARMV8_A57]="ARMV7_A15"
            [ARMV8_A53]="ARMV7_A9"
            [ARMV8_A55]="ARMV8_A53"
            [ARMV8_A72]="ARMV8_A57"
            [ARMV8_A76]="ARMV8_A72"
            [NEOVERSE_V2]="NEOVERSE_V1"
            [NEOVERSE_V3]="NEOVERSE_V2"
            [NEOVERSE_N2]="NEOVERSE_N1"
            [NEOVERSE_N3]="NEOVERSE_N2"
        )
    elif [[ "$arch" == "riscv64" || "$arch" == "riscv32" ]]; then
        # RISC-V microarchitecture detection
        
        mvendorid=$(grep -m1 "mvendorid" /proc/cpuinfo | awk '{print $3}' | sed 's/0x//')
        marchid=$(grep -m1 "marchid" /proc/cpuinfo | awk '{print $3}' | sed 's/0x//')
        mimpid=$(grep -m1 "mimpid" /proc/cpuinfo | awk '{print $3}' | sed 's/0x//')
        uarch_name=$(grep -m1 "uarch" /proc/cpuinfo | awk '{print $3}')
        
        # RISC-V microarchitecture detection based on vendor ID and arch ID
        case "${mvendorid}_${marchid}" in
            ## SiFive (0x489) - SiFive cores
            "489_1")                  uarch="SIFIVE_U74" ;;    # SiFive U74
            "489_2")                  uarch="SIFIVE_U54" ;;    # SiFive U54
            "489_3")                  uarch="SIFIVE_E31" ;;    # SiFive E31
            "489_4")                  uarch="SIFIVE_E21" ;;    # SiFive E21
            "489_5")                  uarch="SIFIVE_S76" ;;    # SiFive S76
            "489_8")                  uarch="SIFIVE_P550" ;;   # SiFive P550
            "489_9")                  uarch="SIFIVE_P270" ;;   # SiFive P270
            
            ## T-Head (0x5b7) - Alibaba T-Head cores
            "5b7_0")                  uarch="THEAD_C906" ;;    # T-Head C906
            "5b7_1")                  uarch="THEAD_C910" ;;    # T-Head C910
            "5b7_2")                  uarch="THEAD_C920" ;;    # T-Head C920
            
            ## Canaan (0x4b1) - Kendryte K210
            "4b1_0")                  uarch="CANAAN_K210" ;;   # Kendryte K210
            
            ## StarFive (0x57c) - StarFive cores
            "57c_1")                  uarch="STARFIVE_U74" ;;  # StarFive U74
            
            ## Andes (0x31e) - Andes cores
            "31e_8000000000000007")   uarch="ANDES_A25" ;;     # Andes A25
            "31e_8000000000000025")   uarch="ANDES_AX45" ;;    # Andes AX45
            
            ## Ventana (0x999) - Ventana cores
            "999_1")                  uarch="VENTANA_VEYRON_V1" ;; # Ventana Veyron V1
            
            ## Tenstorrent (0x8a5) - Tenstorrent cores
            "8a5_0")                  uarch="TENSTORRENT_ASCALON" ;; # Tenstorrent Ascalon
            
            ## Espressif (0x6b9) - ESP32 series
            "6b9_0")                  uarch="ESPRESSIF_C3" ;;  # ESP32-C3
            
            ## Bouffalo Lab (0x4c1) - BL series
            "4c1_0")                  uarch="BOUFFALOLAB_T_HEAD" ;; # BL series with T-Head
            
            *)                        
                # Try to detect based on uarch name if vendor/arch IDs don't match
                case "$uarch_name" in
                    "sifive,u74-mc"|"u74")    uarch="SIFIVE_U74" ;;
                    "sifive,u54"|"u54")       uarch="SIFIVE_U54" ;;
                    "sifive,e31"|"e31")       uarch="SIFIVE_E31" ;;
                    "sifive,e21"|"e21")       uarch="SIFIVE_E21" ;;
                    "thead,c906"|"c906")      uarch="THEAD_C906" ;;
                    "thead,c910"|"c910")      uarch="THEAD_C910" ;;
                    "thead,c920"|"c920")      uarch="THEAD_C920" ;;
                    *)                        uarch="UNKNOWN" ;;
                esac
                ;;
        esac

        # RISC-V predecessor relationships (based on evolution/performance)
        declare -A predecessor=(
            [SIFIVE_U74]="SIFIVE_U54"
            [SIFIVE_P550]="SIFIVE_S76"
            [SIFIVE_P270]="SIFIVE_P550"
            [THEAD_C910]="THEAD_C906"
            [THEAD_C920]="THEAD_C910"
            [ANDES_AX45]="ANDES_A25"
        )
    else
        uarch="UNKNOWN"
        declare -A predecessor=()
    fi

    set_macro_ancestors() {
        local arch=$1
        while [ -n "$arch" ] && [ "$arch" != "UNKNOWN" ]; do
            sed -i "s/^#define OPTKIT_ENV_CPU_MICROARCH_$arch 0/#define OPTKIT_ENV_CPU_MICROARCH_$arch 1/" "$SRC_CONFIG_FILE"
            print_status "Enabling microarchitecture:" "$arch"
            arch=${predecessor[$arch]}
        done
    }

    if [ -n "$uarch" ] && [ "$uarch" != "UNKNOWN" ]; then
        set_macro_ancestors "$uarch"
    else
        if [[ "$arch" == "aarch64" || "$arch" == "arm64" || "$arch" == "armv7l" || "$arch" == "armv8l" ]]; then
            print_status "Detected ARM microarchitecture:" "Unknown (${cpu_implementer}_${cpu_part})"
        elif [[ "$arch" == "riscv64" || "$arch" == "riscv32" ]]; then
            print_status "Detected RISC-V microarchitecture:" "Unknown (${mvendorid}_${marchid})"
        else
            print_status "Detected microarchitecture:" "Unknown ($fm_model_hex)"
        fi
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
