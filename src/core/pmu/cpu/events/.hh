#pragma once
#include <cstdint>
namespace optkit::::{
	enum  : uint64_t {
		PMNC_SW_INCR = 0x00, // Incremented by writes to the Software Increment Register
		IFETCH_MISS = 0x01, // Instruction fetches that cause lowest-level cache miss
		ITLB_MISS = 0x02, // Instruction fetches that cause lowest-level TLB miss
		DCACHE_REFILL = 0x03, // Data read or writes that cause lowest-level cache miss
		DCACHE_ACCESS = 0x04, // Data read or writes that cause lowest-level cache access
		DTLB_REFILL = 0x05, // Data read or writes that cause lowest-level TLB refill
		DREAD = 0x06, // Data read architecturally executed
		DWRITE = 0x07, // Data write architecturally executed
		EXC_TAKEN = 0x09, // Counts each exception taken
		EXC_EXECUTED = 0x0a, // Exception returns architecturally executed
		CID_WRITE = 0x0b, // Instruction writes to Context ID Register
		PC_WRITE = 0x0c, // Software change of PC.  Equivalent to branches
		PC_IMM_BRANCH = 0x0d, // Immediate branches architecturally executed
		UNALIGNED_ACCESS = 0x0f, // Unaligned accesses architecturally executed
		PC_BRANCH_MIS_PRED = 0x10, // Branches mispredicted or not predicted
		CLOCK_CYCLES = 0x11, // Clock cycles
		PC_BRANCH_MIS_USED = 0x12, // Branches that could have been predicted
		JAVA_HW_BYTECODE_EXEC = 0x40, // Java bytecodes decoded
		JAVA_SW_BYTECODE_EXEC = 0x41, // Software Java bytecodes decoded
		JAZELLE_BRANCH_EXEC = 0x42, // Jazelle backward branches executed. Includes branches that are flushed because of previous load/store which abort late (approximate)
		COHERENT_LINE_MISS = 0x50, // Coherent linefill misses which also miss on other processors
		COHERENT_LINE_HIT = 0x51, // Coherent linefill requests that hit on another processor
		ICACHE_DEP_STALL_CYCLES = 0x60, // Cycles processor is stalled waiting for instruction cache and the instruction cache is performing at least one linefill (approximate)
		DCACHE_DEP_STALL_CYCLES = 0x61, // Cycles processor is stalled waiting for data cache
		TLB_MISS_DEP_STALL_CYCLES = 0x62, // Cycles processor is stalled waiting for completion of TLB walk (approximate)
		STREX_EXECUTED_PASSED = 0x63, // Number of STREX instructions executed and passed
		STREX_EXECUTED_FAILED = 0x64, // Number of STREX instructions executed and failed
		DATA_EVICTION = 0x65, // Data eviction requests due to linefill in data cache
		ISSUE_STAGE_NO_INST = 0x66, // Cycles the issue stage does not dispatch any instructions
		ISSUE_STAGE_EMPTY = 0x67, // Cycles where issue stage is empty
		INST_OUT_OF_RENAME_STAGE = 0x68, // Number of instructions going through register renaming stage (approximate)
		PREDICTABLE_FUNCT_RETURNS = 0x6e, // Number of predictable function returns whose condition codes do not fail (approximate)
		MAIN_UNIT_EXECUTED_INST = 0x70, // Instructions executed in the main execution
		SECOND_UNIT_EXECUTED_INST = 0x71, // Instructions executed in the second execution pipeline
		LD_ST_UNIT_EXECUTED_INST = 0x72, // Instructions executed in the Load/Store unit
		FP_EXECUTED_INST = 0x73, // Floating point instructions going through register renaming stage
		NEON_EXECUTED_INST = 0x74, // NEON instructions going through register renaming stage (approximate)
		PLD_FULL_DEP_STALL_CYCLES = 0x80, // Cycles processor is stalled because PLD slots are full (approximate)
		DATA_WR_DEP_STALL_CYCLES = 0x81, // Cycles processor is stalled due to writes to external memory (approximate)
		ITLB_MISS_DEP_STALL_CYCLES = 0x82, // Cycles stalled due to main instruction TLB miss (approximate)
		DTLB_MISS_DEP_STALL_CYCLES = 0x83, // Cycles stalled due to main data TLB miss (approximate)
		MICRO_ITLB_MISS_DEP_STALL_CYCLES = 0x84, // Cycles stalled due to micro instruction TLB miss (approximate)
		MICRO_DTLB_MISS_DEP_STALL_CYCLES = 0x85, // Cycles stalled due to micro data TLB miss (approximate)
		DMB_DEP_STALL_CYCLES = 0x86, // Cycles stalled due to DMB memory barrier (approximate)
		INTGR_CLK_ENABLED_CYCLES = 0x8a, // Cycles during which integer core clock is enabled (approximate)
		DATA_ENGINE_CLK_EN_CYCLES = 0x8b, // Cycles during which Data Engine clock is enabled (approximate)
		ISB_INST = 0x90, // Number of ISB instructions architecturally executed
		DSB_INST = 0x91, // Number of DSB instructions architecturally executed
		DMB_INST = 0x92, // Number of DMB instructions architecturally executed (approximate)
		EXT_INTERRUPTS = 0x93, // Number of External interrupts (approximate)
		PLE_CACHE_LINE_RQST_COMPLETED = 0xa0, // PLE cache line requests completed
		PLE_CACHE_LINE_RQST_SKIPPED = 0xa1, // PLE cache line requests skipped
		PLE_FIFO_FLUSH = 0xa2, // PLE FIFO flushes
		PLE_RQST_COMPLETED = 0xa3, // PLE requests completed
		PLE_FIFO_OVERFLOW = 0xa4, // PLE FIFO overflows
		PLE_RQST_PROG = 0xa5, // PLE requests programmed
		CPU_CYCLES = 0xff, // CPU cycles
		
	};
};

namespace  = optkit::::;

