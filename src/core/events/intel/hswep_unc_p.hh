#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::hswep_unc_p{
	enum hswep_unc_p : uint64_t {
		UNC_P_CLOCKTICKS = 0x00, // PCU Uncore clockticks
		UNC_P_CORE0_TRANSITION_CYCLES = 0x60, // Core 0 C State Transition Cycles
		UNC_P_CORE1_TRANSITION_CYCLES = 0x61, // Core 1 C State Transition Cycles
		UNC_P_CORE2_TRANSITION_CYCLES = 0x62, // Core 2 C State Transition Cycles
		UNC_P_CORE3_TRANSITION_CYCLES = 0x63, // Core 3 C State Transition Cycles
		UNC_P_CORE4_TRANSITION_CYCLES = 0x64, // Core 4 C State Transition Cycles
		UNC_P_CORE5_TRANSITION_CYCLES = 0x65, // Core 5 C State Transition Cycles
		UNC_P_CORE6_TRANSITION_CYCLES = 0x66, // Core 6 C State Transition Cycles
		UNC_P_CORE7_TRANSITION_CYCLES = 0x67, // Core 7 C State Transition Cycles
		UNC_P_CORE8_TRANSITION_CYCLES = 0x68, // Core 8 C State Transition Cycles
		UNC_P_CORE9_TRANSITION_CYCLES = 0x69, // Core 9 C State Transition Cycles
		UNC_P_CORE10_TRANSITION_CYCLES = 0x6a, // Core 10 C State Transition Cycles
		UNC_P_CORE11_TRANSITION_CYCLES = 0x6b, // Core 11 C State Transition Cycles
		UNC_P_CORE12_TRANSITION_CYCLES = 0x6c, // Core 12 C State Transition Cycles
		UNC_P_CORE13_TRANSITION_CYCLES = 0x6d, // Core 13 C State Transition Cycles
		UNC_P_CORE14_TRANSITION_CYCLES = 0x6e, // Core 14 C State Transition Cycles
		UNC_P_CORE15_TRANSITION_CYCLES = 0x6f, // Core 15 C State Transition Cycles
		UNC_P_CORE16_TRANSITION_CYCLES = 0x70, // Core 16 C State Transition Cycles
		UNC_P_CORE17_TRANSITION_CYCLES = 0x71, // Core 17 C State Transition Cycles
		UNC_P_DEMOTIONS_CORE0 = 0x30, // Core 0 C State Demotions
		UNC_P_DEMOTIONS_CORE1 = 0x31, // Core 1 C State Demotions
		UNC_P_DEMOTIONS_CORE2 = 0x32, // Core 2 C State Demotions
		UNC_P_DEMOTIONS_CORE3 = 0x33, // Core 3 C State Demotions
		UNC_P_DEMOTIONS_CORE4 = 0x34, // Core 4 C State Demotions
		UNC_P_DEMOTIONS_CORE5 = 0x35, // Core 5 C State Demotions
		UNC_P_DEMOTIONS_CORE6 = 0x36, // Core 6 C State Demotions
		UNC_P_DEMOTIONS_CORE7 = 0x37, // Core 7 C State Demotions
		UNC_P_DEMOTIONS_CORE8 = 0x38, // Core 8 C State Demotions
		UNC_P_DEMOTIONS_CORE9 = 0x39, // Core 9 C State Demotions
		UNC_P_DEMOTIONS_CORE10 = 0x3a, // Core 10 C State Demotions
		UNC_P_DEMOTIONS_CORE11 = 0x3b, // Core 11 C State Demotions
		UNC_P_DEMOTIONS_CORE12 = 0x3c, // Core 12 C State Demotions
		UNC_P_DEMOTIONS_CORE13 = 0x3d, // Core 13 C State Demotions
		UNC_P_DEMOTIONS_CORE14 = 0x3e, // Core 14 C State Demotions
		UNC_P_DEMOTIONS_CORE15 = 0x3f, // Core 15 C State Demotions
		UNC_P_DEMOTIONS_CORE16 = 0x40, // Core 16 C State Demotions
		UNC_P_DEMOTIONS_CORE17 = 0x41, // Core 17 C State Demotions
		UNC_P_FREQ_BAND0_CYCLES = 0xb, // Frequency Residency
		UNC_P_FREQ_BAND1_CYCLES = 0xc, // Frequency Residency
		UNC_P_FREQ_BAND2_CYCLES = 0xd, // Frequency Residency
		UNC_P_FREQ_BAND3_CYCLES = 0xe, // Frequency Residency
		UNC_P_FREQ_MAX_LIMIT_THERMAL_CYCLES = 0x4, // Thermal Strongest Upper Limit Cycles
		UNC_P_FREQ_MAX_OS_CYCLES = 0x6, // OS Strongest Upper Limit Cycles
		UNC_P_FREQ_MAX_POWER_CYCLES = 0x5, // Power Strongest Upper Limit Cycles
		UNC_P_FREQ_MIN_IO_P_CYCLES = 0x73, // IO P Limit Strongest Lower Limit Cycles
		UNC_P_FREQ_TRANS_CYCLES = 0x74, // Cycles spent changing Frequency
		UNC_P_PKG_RESIDENCY_C0_CYCLES = 0x2a, // Package C State residency - C0
		UNC_P_PKG_RESIDENCY_C1E_CYCLES = 0x4e, // Package C State residency - C1E
		UNC_P_PKG_RESIDENCY_C2E_CYCLES = 0x2b, // Package C State residency - C2E
		UNC_P_PKG_RESIDENCY_C3_CYCLES = 0x2c, // Package C State residency - C3
		UNC_P_PKG_RESIDENCY_C6_CYCLES = 0x2d, // Package C State residency - C6
		UNC_P_PKG_RESIDENCY_C7_CYCLES = 0x2e, // Package C State residency - C7
		UNC_P_MEMORY_PHASE_SHEDDING_CYCLES = 0x2f, // Memory Phase Shedding Cycles
		UNC_P_POWER_STATE_OCCUPANCY = 0x80, // Number of cores in C0
		UNC_P_POWER_STATE_OCCUPANCY__MASK__HSWEP_UNC_P_POWER_STATE_OCCUPANCY__CORES_C0 = 0x4000, // Counts number of cores in C0
		UNC_P_POWER_STATE_OCCUPANCY__MASK__HSWEP_UNC_P_POWER_STATE_OCCUPANCY__CORES_C3 = 0x8000, // Counts number of cores in C3
		UNC_P_POWER_STATE_OCCUPANCY__MASK__HSWEP_UNC_P_POWER_STATE_OCCUPANCY__CORES_C6 = 0xc000, // Counts number of cores in C6
		UNC_P_PROCHOT_EXTERNAL_CYCLES = 0xa, // External Prochot
		UNC_P_PROCHOT_INTERNAL_CYCLES = 0x9, // Internal Prochot
		UNC_P_TOTAL_TRANSITION_CYCLES = 0x72, // Total Core C State Transition Cycles
		UNC_P_VR_HOT_CYCLES = 0x42, // VR Hot
		
	};
};

namespace hswep_unc_p = optkit::intel::hswep_unc_p;