#pragma once
#include <cstdint>
namespace optkit::arm::cortex_a53{
	enum cortex_a53 : uint64_t {
		SW_INCR = 0x00, // Instruction architecturally executed (condition check pass) Software increment
		L1I_CACHE_REFILL = 0x01, // Level 1 instruction cache refill
		L1I_TLB_REFILL = 0x02, // Level 1 instruction TLB refill
		L1D_CACHE_REFILL = 0x03, // Level 1 data cache refill
		L1D_CACHE_ACCESS = 0x04, // Level 1 data cache access
		L1D_TLB_REFILL = 0x05, // Level 1 data TLB refill
		LD_RETIRED = 0x06, // Load Instruction architecturally executed
		ST_RETIRED = 0x07, // Store Instruction architecturally executed
		INST_RETIRED = 0x08, // Instruction architecturally executed
		EXCEPTION_TAKEN = 0x09, // Exception taken
		EXCEPTION_RETURN = 0x0a, // Instruction architecturally executed (condition check pass) Exception return
		CID_WRITE_RETIRED = 0x0b, // Change to Context ID retired
		PC_WRITE_RETIRED = 0x0c, // Write to CONTEXTIDR
		BR_IMMED_RETIRED = 0x0d, // Software change of the PC
		UNALIGNED_LDST_RETIRED = 0x0f, // Procedure return
		BRANCH_MISPRED = 0x10, // Mispredicted or not predicted branch speculatively executed
		CPU_CYCLES = 0x11, // Cycles
		BRANCH_PRED = 0x12, // Predictable branch speculatively executed
		DATA_MEM_ACCESS = 0x13, // Data memory access
		L1I_CACHE_ACCESS = 0x14, // Level 1 instruction cache access
		L1D_CACHE_WB = 0x15, // Level 1 data cache WriteBack
		L2D_CACHE_ACCESS = 0x16, // Level 2 data cache access
		L2D_CACHE_REFILL = 0x17, // Level 2 data cache refill
		L2D_CACHE_WB = 0x18, // Level 2 data cache WriteBack
		BUS_ACCESS = 0x19, // Bus access
		LOCAL_MEMORY_ERROR = 0x1a, // Local memory error
		BUS_CYCLES = 0x1d, // Bus cycle
		BUS_READ_ACCESS = 0x60, // Bus read access
		BUS_WRITE_ACCESS = 0x61, // Bus write access
		BRANCH_SPEC_EXEC_IND = 0x7a, // Indirect branch speculatively executed
		EXCEPTION_IRQ = 0x86, // Exception taken
		EXCEPTION_FIQ = 0x87, // Exception taken
		
	};
};

namespace cortex_a53 = optkit::arm::cortex_a53;