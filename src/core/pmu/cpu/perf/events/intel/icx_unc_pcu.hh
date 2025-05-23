#pragma once
#include <cstdint>
#define INTEL_X86_EDGE_BIT	18
#define INTEL_X86_ANY_BIT	21
#define INTEL_X86_INV_BIT	23
#define INTEL_X86_CMASK_BIT 24
#define INTEL_X86_MOD_EDGE	(1 << INTEL_X86_EDGE_BIT)
#define INTEL_X86_MOD_ANY	(1 << INTEL_X86_ANY_BIT)
#define INTEL_X86_MOD_INV	(1 << INTEL_X86_INV_BIT)
namespace optkit::intel::icx_unc_pcu{
	enum icx_unc_pcu : uint64_t {
		UNC_P_CLOCKTICKS = 0x0000, // Clockticks of the power control unit (PCU)
		UNC_P_CORE_TRANSITION_CYCLES = 0x0060, // UNC_P_CORE_TRANSITION_CYCLES (experimental)
		UNC_P_DEMOTIONS = 0x0030, // UNC_P_DEMOTIONS (experimental)
		UNC_P_FIVR_PS_PS0_CYCLES = 0x0075, // Phase Shed 0 Cycles (experimental)
		UNC_P_FIVR_PS_PS1_CYCLES = 0x0076, // Phase Shed 1 Cycles (experimental)
		UNC_P_FIVR_PS_PS2_CYCLES = 0x0077, // Phase Shed 2 Cycles (experimental)
		UNC_P_FIVR_PS_PS3_CYCLES = 0x0078, // Phase Shed 3 Cycles (experimental)
		UNC_P_FREQ_CLIP_AVX256 = 0x0049, // AVX256 Frequency Clipping (experimental)
		UNC_P_FREQ_CLIP_AVX512 = 0x004a, // AVX512 Frequency Clipping (experimental)
		UNC_P_FREQ_MAX_LIMIT_THERMAL_CYCLES = 0x0004, // Thermal Strongest Upper Limit Cycles (experimental)
		UNC_P_FREQ_MAX_POWER_CYCLES = 0x0005, // Power Strongest Upper Limit Cycles (experimental)
		UNC_P_FREQ_MIN_IO_P_CYCLES = 0x0073, // IO P Limit Strongest Lower Limit Cycles (experimental)
		UNC_P_FREQ_TRANS_CYCLES = 0x0074, // Cycles spent changing Frequency (experimental)
		UNC_P_MEMORY_PHASE_SHEDDING_CYCLES = 0x002f, // Memory Phase Shedding Cycles (experimental)
		UNC_P_PKG_RESIDENCY_C0_CYCLES = 0x002a, // Package C State Residency - C0 (experimental)
		UNC_P_PKG_RESIDENCY_C2E_CYCLES = 0x002b, // Package C State Residency - C2E (experimental)
		UNC_P_PKG_RESIDENCY_C3_CYCLES = 0x002c, // Package C State Residency - C3 (experimental)
		UNC_P_PKG_RESIDENCY_C6_CYCLES = 0x002d, // Package C State Residency - C6 (experimental)
		UNC_P_PMAX_THROTTLED_CYCLES = 0x0006, // UNC_P_PMAX_THROTTLED_CYCLES (experimental)
		UNC_P_POWER_STATE_OCCUPANCY = 0x0080, // Number of cores in C-State
		UNC_P_POWER_STATE_OCCUPANCY__MASK__ICX_UNC_P_POWER_STATE_OCCUPANCY__CORES_C0 = 0x4000ull, // C0 and C1 (experimental)
		UNC_P_POWER_STATE_OCCUPANCY__MASK__ICX_UNC_P_POWER_STATE_OCCUPANCY__CORES_C3 = 0x8000ull, // C3 (experimental)
		UNC_P_POWER_STATE_OCCUPANCY__MASK__ICX_UNC_P_POWER_STATE_OCCUPANCY__CORES_C6 = 0xc000ull, // C6 and C7 (experimental)
		UNC_P_PROCHOT_EXTERNAL_CYCLES = 0x000a, // External Prochot (experimental)
		UNC_P_PROCHOT_INTERNAL_CYCLES = 0x0009, // Internal Prochot (experimental)
		UNC_P_TOTAL_TRANSITION_CYCLES = 0x0072, // Total Core C State Transition Cycles (experimental)
		UNC_P_VR_HOT_CYCLES = 0x0042, // VR Hot (experimental)
		
	};
};

namespace icx_unc_pcu = optkit::intel::icx_unc_pcu;

#undef INTEL_X86_EDGE_BIT
#undef INTEL_X86_ANY_BIT
#undef INTEL_X86_INV_BIT
#undef INTEL_X86_CMASK_BIT
#undef INTEL_X86_MOD_EDGE
#undef INTEL_X86_MOD_ANY
#undef INTEL_X86_MOD_INV
