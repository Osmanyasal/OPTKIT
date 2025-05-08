#pragma once
#include <cstdint>
namespace optkit::arm::cortex_a7{
	enum cortex_a7 : uint64_t {
		SW_INCR = 0x00, // Incremented on writes to the Software Increment Register
		L1I_CACHE_REFILL = 0x01, // Level 1 instruction cache refill
		L1I_TLB_REFILL = 0x02, // Level 1 instruction TLB refill
		L1D_CACHE_REFILL = 0x03, // Level 1 data cache refill
		L1D_CACHE_ACCESS = 0x04, // Level 1 data cache access
		L1D_TLB_REFILL = 0x05, // Level 1 data TLB refill
		DATA_READS = 0x06, // Data reads architecturally executed
		DATA_WRITES = 0x07, // Data writes architecturally executed
		INST_RETIRED = 0x08, // Instruction architecturally executed
		EXCEPTION_TAKEN = 0x09, // Exception taken
		EXCEPTION_RETURN = 0x0a, // Instruction architecturally executed
		CID_WRITE_RETIRED = 0x0b, // Change to ContextID retired
		SW_CHANGE_PC = 0x0c, // Software change of PC
		IMMEDIATE_BRANCHES = 0x0d, // Immediate branch architecturally executed
		PROCEDURE_RETURNS = 0x0e, // Procedure returns architecturally executed
		UNALIGNED_LOAD_STORE = 0x0f, // Unaligned load-store
		BRANCH_MISPRED = 0x10, // Branches mispredicted/not predicted
		CPU_CYCLES = 0x11, // Cycles
		BRANCH_PRED = 0x12, // Predictable branch speculatively executed
		DATA_MEM_ACCESS = 0x13, // Data memory access
		L1I_CACHE_ACCESS = 0x14, // Level 1 instruction cache access
		L1D_CACHE_EVICTION = 0x15, // Level 1 data cache eviction
		L2D_CACHE_ACCESS = 0x16, // Level 2 data cache access
		L2D_CACHE_REFILL = 0x17, // Level 2 data cache refill
		L2D_CACHE_WB = 0x18, // Level 2 data cache WriteBack
		BUS_ACCESS = 0x19, // Bus accesses
		BUS_CYCLES = 0x1d, // Bus cycle
		BUS_READ_ACCESS = 0x60, // Bus read access
		BUS_WRITE_ACCESS = 0x61, // Bus write access
		IRQ_EXCEPTION_TAKEN = 0x86, // IRQ Exception Taken
		FIQ_EXCEPTION_TAKEN = 0x87, // FIQ Exception Taken
		EXTERNAL_MEMORY_REQUEST = 0xc0, // External memory request
		NONCACHE_EXTERNAL_MEMORY_REQUEST = 0xc1, // Non-cacheable xternal memory request
		PREFETCH_LINEFILL = 0xc2, // Linefill due to prefetch
		PREFETCH_LINEFILL_DROPPED = 0xc3, // Prefetch linefill dropped
		ENTERING_READ_ALLOC = 0xc4, // Entering read allocate mode
		READ_ALLOC = 0xc5, // Read allocate mode
		ETM_EXT_OUT_0 = 0xc7, // ETM Ext Out[0]
		ETM_EXT_OUT_1 = 0xc8, // ETM Ext Out[1]
		DATA_WRITE_STALL = 0xc9, // Data write operation that stalls pipeline due to full store buffer
		DATA_SNOOPED = 0xca, // Data snooped from other processor
		
	};
};

namespace cortex_a7 = optkit::arm::cortex_a7;