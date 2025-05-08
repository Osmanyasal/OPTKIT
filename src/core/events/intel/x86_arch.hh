#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::x86_arch{
	enum x86_arch : uint64_t {
		UNHALTED_CORE_CYCLES = 0x003c, // count core clock cycles whenever the clock signal on the specific core is running (not halted)
		INSTRUCTION_RETIRED = 0x00c0, // count the number of instructions at retirement. For instructions that consists of multiple micro-ops
		UNHALTED_REFERENCE_CYCLES = 0x013c, // count reference clock cycles while the clock signal on the specific core is running. The reference clock operates at a fixed frequency
		LLC_REFERENCES = 0x4f2e, // count each request originating from the core to reference a cache line in the last level cache. The count may include speculation
		LLC_MISSES = 0x412e, // count each cache miss condition for references to the last level cache. The event count may include speculation
		BRANCH_INSTRUCTIONS_RETIRED = 0x00c4, // count branch instructions at retirement. Specifically
		
	};
};

namespace x86_arch = optkit::intel::x86_arch;