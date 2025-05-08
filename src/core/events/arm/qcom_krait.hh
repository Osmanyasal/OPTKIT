#pragma once
#include <cstdint>
namespace optkit::arm::qcom_krait{
	enum qcom_krait : uint64_t {
		L1D_CACHE_REFILL = 0x03, // Level 1 data cache refill
		L1D_CACHE_ACCESS = 0x04, // Level 1 data cache access
		INSTR_EXECUTED = 0x08, // Instructions architecturally executed
		PC_WRITE = 0x0c, // Software change of PC.  Equivalent to branches
		PC_BRANCH_MIS_PRED = 0x10, // Branches mispredicted or not predicted
		CLOCK_CYCLES = 0x11, // Cycles
		BRANCH_PRED = 0x12, // Predictable branch speculatively executed
		CPU_CYCLES = 0xff, // Cycles
		
	};
};

namespace qcom_krait = optkit::arm::qcom_krait;