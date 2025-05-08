#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::ivbep_unc_p{
	enum ivbep_unc_p : uint64_t {
		UNC_P_CLOCKTICKS = 0x00, // PCU Uncore clockticks
		UNC_P_CORE0_TRANSITION_CYCLES = 0x70, // Core 0 C State Transition Cycles
		UNC_P_CORE1_TRANSITION_CYCLES = 0x71, // Core 1 C State Transition Cycles
		UNC_P_CORE2_TRANSITION_CYCLES = 0x72, // Core 2 C State Transition Cycles
		UNC_P_CORE3_TRANSITION_CYCLES = 0x73, // Core 3 C State Transition Cycles
		UNC_P_CORE4_TRANSITION_CYCLES = 0x74, // Core 4 C State Transition Cycles
		UNC_P_CORE5_TRANSITION_CYCLES = 0x75, // Core 5 C State Transition Cycles
		UNC_P_CORE6_TRANSITION_CYCLES = 0x76, // Core 6 C State Transition Cycles
		UNC_P_CORE7_TRANSITION_CYCLES = 0x77, // Core 7 C State Transition Cycles
		UNC_P_CORE8_TRANSITION_CYCLES = 0x78, // Core 8 C State Transition Cycles
		UNC_P_CORE9_TRANSITION_CYCLES = 0x79, // Core 9 C State Transition Cycles
		UNC_P_CORE10_TRANSITION_CYCLES = 0x7a, // Core 10 C State Transition Cycles
		UNC_P_CORE11_TRANSITION_CYCLES = 0x7b, // Core 11 C State Transition Cycles
		UNC_P_CORE12_TRANSITION_CYCLES = 0x7c, // Core 12 C State Transition Cycles
		UNC_P_CORE13_TRANSITION_CYCLES = 0x7d, // Core 13 C State Transition Cycles
		UNC_P_CORE14_TRANSITION_CYCLES = 0x7e, // Core 14 C State Transition Cycles
		UNC_P_DELAYED_C_STATE_ABORT_CORE0 = 0x17 | (1ULL << 21), // Deep C state rejection Core 0
		UNC_P_DELAYED_C_STATE_ABORT_CORE1 = 0x18 | (1ULL << 21), // Deep C state rejection Core 1
		UNC_P_DELAYED_C_STATE_ABORT_CORE2 = 0x19 | (1ULL << 21), // Deep C state rejection Core 2
		UNC_P_DELAYED_C_STATE_ABORT_CORE3 = 0x1a | (1ULL << 21), // Deep C state rejection Core 3
		UNC_P_DELAYED_C_STATE_ABORT_CORE4 = 0x1b | (1ULL << 21), // Deep C state rejection Core 4
		UNC_P_DELAYED_C_STATE_ABORT_CORE5 = 0x1c | (1ULL << 21), // Deep C state rejection Core 5
		UNC_P_DELAYED_C_STATE_ABORT_CORE6 = 0x1d | (1ULL << 21), // Deep C state rejection Core 6
		UNC_P_DELAYED_C_STATE_ABORT_CORE7 = 0x1e | (1ULL << 21), // Deep C state rejection Core 7
		UNC_P_DELAYED_C_STATE_ABORT_CORE8 = 0x1f | (1ULL << 21), // Deep C state rejection Core 8
		UNC_P_DELAYED_C_STATE_ABORT_CORE9 = 0x20 | (1ULL << 21), // Deep C state rejection Core 9
		UNC_P_DELAYED_C_STATE_ABORT_CORE10 = 0x21 | (1ULL << 21), // Deep C state rejection Core 10
		UNC_P_DELAYED_C_STATE_ABORT_CORE11 = 0x22 | (1ULL << 21), // Deep C state rejection Core 11
		UNC_P_DELAYED_C_STATE_ABORT_CORE12 = 0x23 | (1ULL << 21), // Deep C state rejection Core 12
		UNC_P_DELAYED_C_STATE_ABORT_CORE13 = 0x24 | (1ULL << 21), // Deep C state rejection Core 13
		UNC_P_DELAYED_C_STATE_ABORT_CORE14 = 0x25 | (1ULL << 21), // Deep C state rejection Core 14
		UNC_P_DEMOTIONS_CORE0 = 0x1e, // Core 0 C State Demotions
		UNC_P_DEMOTIONS_CORE1 = 0x1f, // Core 1 C State Demotions
		UNC_P_DEMOTIONS_CORE2 = 0x20, // Core 2 C State Demotions
		UNC_P_DEMOTIONS_CORE3 = 0x21, // Core 3 C State Demotions
		UNC_P_DEMOTIONS_CORE4 = 0x22, // Core 4 C State Demotions
		UNC_P_DEMOTIONS_CORE5 = 0x23, // Core 5 C State Demotions
		UNC_P_DEMOTIONS_CORE6 = 0x24, // Core 6 C State Demotions
		UNC_P_DEMOTIONS_CORE7 = 0x25, // Core 7 C State Demotions
		UNC_P_DEMOTIONS_CORE8 = 0x40, // Core 8 C State Demotions
		UNC_P_DEMOTIONS_CORE9 = 0x41, // Core 9 C State Demotions
		UNC_P_DEMOTIONS_CORE10 = 0x42, // Core 10 C State Demotions
		UNC_P_DEMOTIONS_CORE11 = 0x43, // Core 11 C State Demotions
		UNC_P_DEMOTIONS_CORE12 = 0x44, // Core 12 C State Demotions
		UNC_P_DEMOTIONS_CORE13 = 0x45, // Core 13 C State Demotions
		UNC_P_DEMOTIONS_CORE14 = 0x46, // Core 14 C State Demotions
		UNC_P_FREQ_BAND0_CYCLES = 0xb, // Frequency Residency
		UNC_P_FREQ_BAND1_CYCLES = 0xc, // Frequency Residency
		UNC_P_FREQ_BAND2_CYCLES = 0xd, // Frequency Residency
		UNC_P_FREQ_BAND3_CYCLES = 0xe, // Frequency Residency
		UNC_P_FREQ_MAX_CURRENT_CYCLES = 0x7, // Current Strongest Upper Limit Cycles
		UNC_P_FREQ_MAX_LIMIT_THERMAL_CYCLES = 0x4, // Thermal Strongest Upper Limit Cycles
		UNC_P_FREQ_MAX_OS_CYCLES = 0x6, // OS Strongest Upper Limit Cycles
		UNC_P_FREQ_MAX_POWER_CYCLES = 0x5, // Power Strongest Upper Limit Cycles
		UNC_P_FREQ_MIN_PERF_P_CYCLES = 0x02 | (1ULL << 21), // Perf P Limit Strongest Lower Limit Cycles
		UNC_P_FREQ_MIN_IO_P_CYCLES = 0x61, // IO P Limit Strongest Lower Limit Cycles
		UNC_P_FREQ_TRANS_CYCLES = 0x60, // Cycles spent changing Frequency
		UNC_P_MEMORY_PHASE_SHEDDING_CYCLES = 0x2f, // Memory Phase Shedding Cycles
		UNC_P_PKG_C_EXIT_LATENCY = 0x26 | (1ULL << 21), // Package C state exit latency. Counts cycles the package is transitioning from C2 to C3
		UNC_P_POWER_STATE_OCCUPANCY = 0x80, // Number of cores in C0
		UNC_P_POWER_STATE_OCCUPANCY__MASK__IVBEP_UNC_P_POWER_STATE_OCCUPANCY__CORES_C0 = 0x4000, // Counts number of cores in C0
		UNC_P_POWER_STATE_OCCUPANCY__MASK__IVBEP_UNC_P_POWER_STATE_OCCUPANCY__CORES_C3 = 0x8000, // Counts number of cores in C3
		UNC_P_POWER_STATE_OCCUPANCY__MASK__IVBEP_UNC_P_POWER_STATE_OCCUPANCY__CORES_C6 = 0xc000, // Counts number of cores in C6
		UNC_P_PROCHOT_EXTERNAL_CYCLES = 0xa, // External Prochot
		UNC_P_PROCHOT_INTERNAL_CYCLES = 0x9, // Internal Prochot
		UNC_P_TOTAL_TRANSITION_CYCLES = 0x63, // Total Core C State Transition Cycles
		UNC_P_VOLT_TRANS_CYCLES_CHANGE = 0x3, // Cycles Changing Voltage
		UNC_P_VOLT_TRANS_CYCLES_DECREASE = 0x2, // Cycles Decreasing Voltage
		UNC_P_VOLT_TRANS_CYCLES_INCREASE = 0x1, // Cycles Increasing Voltage
		UNC_P_VR_HOT_CYCLES = 0x32, // VR Hot
		
	};
};

namespace ivbep_unc_p = optkit::intel::ivbep_unc_p;