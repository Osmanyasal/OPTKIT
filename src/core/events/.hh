#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::::{
	enum  : uint64_t {
		ICACHE_MISS = 0x00, // Instruction cache miss (includes speculative accesses)
		IBUF_STALL = 0x01, // Stall because instruction buffer cannot deliver an instruction
		DDEP_STALL = 0x02, // Stall because of data dependency
		ITLB_MISS = 0x03, // Instruction MicroTLB miss
		DTLB_MISS = 0x04, // Data MicroTLB miss
		BR_EXEC = 0x05, // Branch instruction executed
		BR_MISPREDICT = 0x06, // Branch mispredicted
		INSTR_EXEC = 0x07, // Instruction executed
		DCACHE_HIT = 0x09, // Data cache hit
		DCACHE_ACCESS = 0x0a, // Data cache access
		DCACHE_MISS = 0x0b, // Data cache miss
		DCACHE_WBACK = 0x0c, // Data cache writeback
		SW_PC_CHANGE = 0x0d, // Software changed the PC.
		MAIN_TLB_MISS = 0x0f, // Main TLB miss
		EXPL_D_ACCESS = 0x10, // Explicit external data cache access
		LSU_FULL_STALL = 0x11, // Stall because of a full Load Store Unit request queue.
		WBUF_DRAINED = 0x12, // Write buffer drained due to data synchronization barrier or strongly ordered operation
		ETMEXTOUT_0 = 0x20, // ETMEXTOUT[0] was asserted
		ETMEXTOUT_1 = 0x21, // ETMEXTOUT[1] was asserted
		ETMEXTOUT = 0x22, // Increment once for each of ETMEXTOUT[0] or ETMEXTOUT[1]
		PROC_CALL_EXEC = 0x23, // Procedure call instruction executed
		PROC_RET_EXEC = 0x24, // Procedure return instruction executed
		PROC_RET_EXEC_PRED = 0x25, // Procedure return instruction executed and address predicted
		PROC_RET_EXEC_PRED_INCORRECT = 0x26, // Procedure return instruction executed and address predicted incorrectly
		CPU_CYCLES = 0xff, // CPU cycles
		
	};
};

namespace  = optkit::::;