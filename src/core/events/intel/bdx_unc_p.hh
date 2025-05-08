#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::bdx_unc_p{
	enum bdx_unc_p : uint64_t {
		UNC_P_CLOCKTICKS = 0x0, // The PCU runs off a fixed 1 GHz clock.  This event counts the number of pclk cycles measured while the counter was enabled.  The pclk
		UNC_P_CORE0_TRANSITION_CYCLES = 0x60, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE10_TRANSITION_CYCLES = 0x6a, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE11_TRANSITION_CYCLES = 0x6b, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE12_TRANSITION_CYCLES = 0x6c, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE13_TRANSITION_CYCLES = 0x6d, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE14_TRANSITION_CYCLES = 0x6e, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE15_TRANSITION_CYCLES = 0x6f, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE16_TRANSITION_CYCLES = 0x70, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE17_TRANSITION_CYCLES = 0x71, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE1_TRANSITION_CYCLES = 0x61, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE2_TRANSITION_CYCLES = 0x62, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE3_TRANSITION_CYCLES = 0x63, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE4_TRANSITION_CYCLES = 0x64, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE5_TRANSITION_CYCLES = 0x65, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE6_TRANSITION_CYCLES = 0x66, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE7_TRANSITION_CYCLES = 0x67, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE8_TRANSITION_CYCLES = 0x68, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_CORE9_TRANSITION_CYCLES = 0x69, // Number of cycles spent performing core C state transitions.  There is one event per core.
		UNC_P_DEMOTIONS_CORE0 = 0x30, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE1 = 0x31, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE10 = 0x3a, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE11 = 0x3b, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE12 = 0x3c, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE13 = 0x3d, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE14 = 0x3e, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE15 = 0x3f, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE16 = 0x40, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE17 = 0x41, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE2 = 0x32, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE3 = 0x33, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE4 = 0x34, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE5 = 0x35, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE6 = 0x36, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE7 = 0x37, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE8 = 0x38, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_DEMOTIONS_CORE9 = 0x39, // Counts the number of times when a configurable cores had a C-state demotion
		UNC_P_FREQ_MAX_LIMIT_THERMAL_CYCLES = 0x4, // Counts the number of cycles when thermal conditions are the upper limit on frequency.  This is related to the THERMAL_THROTTLE CYCLES_ABOVE_TEMP event
		UNC_P_FREQ_MAX_OS_CYCLES = 0x6, // Counts the number of cycles when the OS is the upper limit on frequency.
		UNC_P_FREQ_MAX_POWER_CYCLES = 0x5, // Counts the number of cycles when power is the upper limit on frequency.
		UNC_P_FREQ_MIN_IO_P_CYCLES = 0x73, // Counts the number of cycles when IO P Limit is preventing us from dropping the frequency lower.  This algorithm monitors the needs to the IO subsystem on both local and remote sockets and will maintain a frequency high enough to maintain good IO BW.  This is necessary for when all the IA cores on a socket are idle but a user still would like to maintain high IO Bandwidth.
		UNC_P_FREQ_TRANS_CYCLES = 0x74, // Counts the number of cycles when the system is changing frequency.  This can not be filtered by thread ID.  One can also use it with the occupancy counter that monitors number of threads in C0 to estimate the performance impact that frequency transitions had on the system.
		UNC_P_MEMORY_PHASE_SHEDDING_CYCLES = 0x2f, // Counts the number of cycles that the PCU has triggered memory phase shedding.  This is a mode that can be run in the iMC physicals that saves power at the expense of additional latency.
		UNC_P_POWER_STATE_OCCUPANCY = 0x80, // This is an occupancy event that tracks the number of cores that are in the chosen C-State.  It can be used by itself to get the average number of cores in that C-state with threshholding to generate histograms
		UNC_P_POWER_STATE_OCCUPANCY__MASK__BDX_UNC_P_POWER_STATE_OCCUPANCY__CORES_C0 = 0x4000, // Number of cores in C-State -- C0 and C1
		UNC_P_POWER_STATE_OCCUPANCY__MASK__BDX_UNC_P_POWER_STATE_OCCUPANCY__CORES_C3 = 0x8000, // Number of cores in C-State -- C3
		UNC_P_POWER_STATE_OCCUPANCY__MASK__BDX_UNC_P_POWER_STATE_OCCUPANCY__CORES_C6 = 0xc000, // Number of cores in C-State -- C6 and C7
		UNC_P_PROCHOT_EXTERNAL_CYCLES = 0xa, // Counts the number of cycles that we are in external PROCHOT mode.  This mode is triggered when a sensor off the die determines that something off-die (like DRAM) is too hot and must throttle to avoid damaging the chip.
		UNC_P_PROCHOT_INTERNAL_CYCLES = 0x9, // Counts the number of cycles that we are in internal PROCHOT mode.  This mode is triggered when a sensor on the die determines that we are too hot and must throttle to avoid damaging the chip.
		UNC_P_TOTAL_TRANSITION_CYCLES = 0x72, // Number of cycles spent performing core C state transitions across all cores.
		UNC_P_UFS_BANDWIDTH_MAX_RANGE = 0x7e, // TBD
		UNC_P_UFS_TRANSITIONS_DOWN = 0x7c, // Ring GV down due to low traffic
		UNC_P_UFS_TRANSITIONS_IO_P_LIMIT = 0x7d, // TBD
		UNC_P_UFS_TRANSITIONS_NO_CHANGE = 0x79, // Ring GV with same final and initial frequency
		UNC_P_UFS_TRANSITIONS_UP_RING = 0x7a, // Ring GV up due to high ring traffic
		UNC_P_UFS_TRANSITIONS_UP_STALL = 0x7b, // Ring GV up due to high core stalls
		UNC_P_VR_HOT_CYCLES = 0x42, // TBD
		UNC_P_FREQ_BAND0_CYCLES = 0xb, // Frequency Residency
		UNC_P_FREQ_BAND1_CYCLES = 0xc, // Frequency Residency
		UNC_P_FREQ_BAND2_CYCLES = 0xd, // Frequency Residency
		UNC_P_FREQ_BAND3_CYCLES = 0xe, // Frequency Residency
		UNC_P_FIVR_PS_PS0_CYCLES = 0x75, // Cycles spent in phase-shedding power state 0
		UNC_P_FIVR_PS_PS1_CYCLES = 0x76, // Cycles spent in phase-shedding power state 1
		UNC_P_FIVR_PS_PS2_CYCLES = 0x77, // Cycles spent in phase-shedding power state 2
		UNC_P_FIVR_PS_PS3_CYCLES = 0x78, // Cycles spent in phase-shedding power state 3
		
	};
};

namespace bdx_unc_p = optkit::intel::bdx_unc_p;