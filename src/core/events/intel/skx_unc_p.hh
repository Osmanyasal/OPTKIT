#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::skx_unc_p{
	enum skx_unc_p : uint64_t {
		UNC_P_CLOCKTICKS = 0x0, // The PCU runs off a fixed 1 GHz clock.  This event counts the number of pclk cycles measured while the counter was enabled.  The pclk
		UNC_P_CORE_TRANSITION_CYCLES = 0x60, // TBD
		UNC_P_CTS_EVENT0 = 0x11, // TBD
		UNC_P_CTS_EVENT1 = 0x12, // TBD
		UNC_P_DEMOTIONS = 0x30, // TBD
		UNC_P_FIVR_PS_PS0_CYCLES = 0x75, // Cycles spent in phase-shedding power state 0
		UNC_P_FIVR_PS_PS1_CYCLES = 0x76, // Cycles spent in phase-shedding power state 1
		UNC_P_FIVR_PS_PS2_CYCLES = 0x77, // Cycles spent in phase-shedding power state 2
		UNC_P_FIVR_PS_PS3_CYCLES = 0x78, // Cycles spent in phase-shedding power state 3
		UNC_P_FREQ_MAX_LIMIT_THERMAL_CYCLES = 0x4, // Counts the number of cycles when thermal conditions are the upper limit on frequency.  This is related to the THERMAL_THROTTLE CYCLES_ABOVE_TEMP event
		UNC_P_FREQ_MAX_POWER_CYCLES = 0x5, // Counts the number of cycles when power is the upper limit on frequency.
		UNC_P_FREQ_MIN_IO_P_CYCLES = 0x73, // Counts the number of cycles when IO P Limit is preventing us from dropping the frequency lower.  This algorithm monitors the needs to the IO subsystem on both local and remote sockets and will maintain a frequency high enough to maintain good IO BW.  This is necessary for when all the IA cores on a socket are idle but a user still would like to maintain high IO Bandwidth.
		UNC_P_FREQ_TRANS_CYCLES = 0x74, // Counts the number of cycles when the system is changing frequency.  This can not be filtered by thread ID.  One can also use it with the occupancy counter that monitors number of threads in C0 to estimate the performance impact that frequency transitions had on the system.
		UNC_P_MCP_PROCHOT_CYCLES = 0x6, // TBD
		UNC_P_MEMORY_PHASE_SHEDDING_CYCLES = 0x2f, // Counts the number of cycles that the PCU has triggered memory phase shedding.  This is a mode that can be run in the iMC physicals that saves power at the expense of additional latency.
		UNC_P_PKG_RESIDENCY_C0_CYCLES = 0x2a, // Counts the number of cycles when the package was in C0.  This event can be used in conjunction with edge detect to count C0 entrances (or exits using invert).  Residency events do not include transition times.
		UNC_P_PKG_RESIDENCY_C2E_CYCLES = 0x2b, // Counts the number of cycles when the package was in C2E.  This event can be used in conjunction with edge detect to count C2E entrances (or exits using invert).  Residency events do not include transition times.
		UNC_P_PKG_RESIDENCY_C3_CYCLES = 0x2c, // Counts the number of cycles when the package was in C3.  This event can be used in conjunction with edge detect to count C3 entrances (or exits using invert).  Residency events do not include transition times.
		UNC_P_PKG_RESIDENCY_C6_CYCLES = 0x2d, // Counts the number of cycles when the package was in C6.  This event can be used in conjunction with edge detect to count C6 entrances (or exits using invert).  Residency events do not include transition times.
		UNC_P_PMAX_THROTTLED_CYCLES = 0x7, // TBD
		UNC_P_PROCHOT_EXTERNAL_CYCLES = 0xa, // Counts the number of cycles that we are in external PROCHOT mode.  This mode is triggered when a sensor off the die determines that something off-die (like DRAM) is too hot and must throttle to avoid damaging the chip.
		UNC_P_PROCHOT_INTERNAL_CYCLES = 0x9, // Counts the number of cycles that we are in internal PROCHOT mode.  This mode is triggered when a sensor on the die determines that we are too hot and must throttle to avoid damaging the chip.
		UNC_P_TOTAL_TRANSITION_CYCLES = 0x72, // Number of cycles spent performing core C state transitions across all cores.
		UNC_P_FREQ_BAND0_CYCLES = 0xb, // Frequency Residency
		UNC_P_FREQ_BAND1_CYCLES = 0xc, // Frequency Residency
		UNC_P_FREQ_BAND2_CYCLES = 0xd, // Frequency Residency
		UNC_P_FREQ_BAND3_CYCLES = 0xe, // Frequency Residency
		UNC_P_VR_HOT_CYCLES = 0x42, // TBD
		UNC_P_POWER_STATE_OCCUPANCY = 0x80, // This is an occupancy event that tracks the number of cores that are in the chosen C-State.  It can be used by itself to get the average number of cores in that C-state with threshholding to generate histograms
		UNC_P_POWER_STATE_OCCUPANCY__MASK__SKX_UNC_P_POWER_STATE_OCCUPANCY__CORES_C0 = 0x4000, // Number of cores in C-State -- C0 and C1
		UNC_P_POWER_STATE_OCCUPANCY__MASK__SKX_UNC_P_POWER_STATE_OCCUPANCY__CORES_C3 = 0x8000, // Number of cores in C-State -- C3
		UNC_P_POWER_STATE_OCCUPANCY__MASK__SKX_UNC_P_POWER_STATE_OCCUPANCY__CORES_C6 = 0xc000, // Number of cores in C-State -- C6 and C7
		
	};
};

namespace skx_unc_p = optkit::intel::skx_unc_p;