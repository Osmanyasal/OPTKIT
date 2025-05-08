#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::snbep_unc_p{
	enum snbep_unc_p : uint64_t {
		UNC_P_CLOCKTICKS = 0x00, // PCU Uncore clockticks
		UNC_P_CORE0_TRANSITION_CYCLES = 0x3 | (1ULL << 21), // Core C State Transition Cycles
		UNC_P_CORE1_TRANSITION_CYCLES = 0x4 | (1ULL << 21), // Core C State Transition Cycles
		UNC_P_CORE2_TRANSITION_CYCLES = 0x5 | (1ULL << 21), // Core C State Transition Cycles
		UNC_P_CORE3_TRANSITION_CYCLES = 0x6 | (1ULL << 21), // Core C State Transition Cycles
		UNC_P_CORE4_TRANSITION_CYCLES = 0x7 | (1ULL << 21), // Core C State Transition Cycles
		UNC_P_CORE5_TRANSITION_CYCLES = 0x8 | (1ULL << 21), // Core C State Transition Cycles
		UNC_P_CORE6_TRANSITION_CYCLES = 0x9 | (1ULL << 21), // Core C State Transition Cycles
		UNC_P_CORE7_TRANSITION_CYCLES = 0xa | (1ULL << 21), // Core C State Transition Cycles
		UNC_P_DEMOTIONS_CORE0 = 0x1e, // Core C State Demotions
		UNC_P_DEMOTIONS_CORE1 = 0x1f, // Core C State Demotions
		UNC_P_DEMOTIONS_CORE2 = 0x20, // Core C State Demotions
		UNC_P_DEMOTIONS_CORE3 = 0x21, // Core C State Demotions
		UNC_P_DEMOTIONS_CORE4 = 0x22, // Core C State Demotions
		UNC_P_DEMOTIONS_CORE5 = 0x23, // Core C State Demotions
		UNC_P_DEMOTIONS_CORE6 = 0x24, // Core C State Demotions
		UNC_P_DEMOTIONS_CORE7 = 0x25, // Core C State Demotions
		UNC_P_FREQ_BAND0_CYCLES = 0xb, // Frequency Residency
		UNC_P_FREQ_BAND1_CYCLES = 0xc, // Frequency Residency
		UNC_P_FREQ_BAND2_CYCLES = 0xd, // Frequency Residency
		UNC_P_FREQ_BAND3_CYCLES = 0xe, // Frequency Residency
		UNC_P_FREQ_MAX_CURRENT_CYCLES = 0x7, // Current Strongest Upper Limit Cycles
		UNC_P_FREQ_MAX_LIMIT_THERMAL_CYCLES = 0x4, // Thermal Strongest Upper Limit Cycles
		UNC_P_FREQ_MAX_OS_CYCLES = 0x6, // OS Strongest Upper Limit Cycles
		UNC_P_FREQ_MAX_POWER_CYCLES = 0x5, // Power Strongest Upper Limit Cycles
		UNC_P_FREQ_MIN_IO_P_CYCLES = 0x1 | (1ULL << 21), // IO P Limit Strongest Lower Limit Cycles
		UNC_P_FREQ_MIN_IO_P_CYCLES__MASK__SNBEP_UNC_P_OCCUPANCY_COUNTERS__C0 = 0x0100, // Counts number of cores in C0
		UNC_P_FREQ_MIN_IO_P_CYCLES__MASK__SNBEP_UNC_P_OCCUPANCY_COUNTERS__C3 = 0x0200, // Counts number of cores in C3
		UNC_P_FREQ_MIN_IO_P_CYCLES__MASK__SNBEP_UNC_P_OCCUPANCY_COUNTERS__C6 = 0x0300, // Counts number of cores in C6
		UNC_P_FREQ_MIN_PERF_P_CYCLES = 0x2 | (1ULL << 21), // Perf P Limit Strongest Lower Limit Cycles
		UNC_P_FREQ_MIN_PERF_P_CYCLES__MASK__SNBEP_UNC_P_OCCUPANCY_COUNTERS__C0 = 0x0100, // Counts number of cores in C0
		UNC_P_FREQ_MIN_PERF_P_CYCLES__MASK__SNBEP_UNC_P_OCCUPANCY_COUNTERS__C3 = 0x0200, // Counts number of cores in C3
		UNC_P_FREQ_MIN_PERF_P_CYCLES__MASK__SNBEP_UNC_P_OCCUPANCY_COUNTERS__C6 = 0x0300, // Counts number of cores in C6
		UNC_P_FREQ_TRANS_CYCLES = 0x0 | (1ULL << 21), // Cycles spent changing Frequency
		UNC_P_FREQ_TRANS_CYCLES__MASK__SNBEP_UNC_P_OCCUPANCY_COUNTERS__C0 = 0x0100, // Counts number of cores in C0
		UNC_P_FREQ_TRANS_CYCLES__MASK__SNBEP_UNC_P_OCCUPANCY_COUNTERS__C3 = 0x0200, // Counts number of cores in C3
		UNC_P_FREQ_TRANS_CYCLES__MASK__SNBEP_UNC_P_OCCUPANCY_COUNTERS__C6 = 0x0300, // Counts number of cores in C6
		UNC_P_MEMORY_PHASE_SHEDDING_CYCLES = 0x2f, // Memory Phase Shedding Cycles
		UNC_P_POWER_STATE_OCCUPANCY = 0x80, // Number of cores in C0
		UNC_P_POWER_STATE_OCCUPANCY__MASK__SNBEP_UNC_P_POWER_STATE_OCCUPANCY__CORES_C0 = 0x4000, // Counts number of cores in C0
		UNC_P_POWER_STATE_OCCUPANCY__MASK__SNBEP_UNC_P_POWER_STATE_OCCUPANCY__CORES_C3 = 0x8000, // Counts number of cores in C3
		UNC_P_POWER_STATE_OCCUPANCY__MASK__SNBEP_UNC_P_POWER_STATE_OCCUPANCY__CORES_C6 = 0xc000, // Counts number of cores in C6
		UNC_P_PROCHOT_EXTERNAL_CYCLES = 0xa, // External Prochot
		UNC_P_PROCHOT_INTERNAL_CYCLES = 0x9, // Internal Prochot
		UNC_P_TOTAL_TRANSITION_CYCLES = 0xb | (1ULL << 21), // Total Core C State Transition Cycles
		UNC_P_VOLT_TRANS_CYCLES_CHANGE = 0x3, // Cycles Changing Voltage
		UNC_P_VOLT_TRANS_CYCLES_DECREASE = 0x2, // Cycles Decreasing Voltage
		UNC_P_VOLT_TRANS_CYCLES_INCREASE = 0x1, // Cycles Increasing Voltage
		UNC_P_VR_HOT_CYCLES = 0x32, // VR Hot
		
	};
};

namespace snbep_unc_p = optkit::intel::snbep_unc_p;