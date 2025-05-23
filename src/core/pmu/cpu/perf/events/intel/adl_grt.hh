#pragma once
#include <cstdint>
#define INTEL_X86_EDGE_BIT	18
#define INTEL_X86_ANY_BIT	21
#define INTEL_X86_INV_BIT	23
#define INTEL_X86_CMASK_BIT 24
#define INTEL_X86_MOD_EDGE	(1 << INTEL_X86_EDGE_BIT)
#define INTEL_X86_MOD_ANY	(1 << INTEL_X86_ANY_BIT)
#define INTEL_X86_MOD_INV	(1 << INTEL_X86_INV_BIT)
namespace optkit::intel::adl_grt{
	enum adl_grt : uint64_t {
		BACLEARS = 0x00e6, // Counts the total number of BACLEARS due to all branch types including conditional and unconditional jumps
		BACLEARS__MASK__ADL_GRT_BACLEARS__ANY = 0x0100ull, // Counts the total number of BACLEARS due to all branch types including conditional and unconditional jumps
		BR_INST_RETIRED = 0x00c4, // Counts the total number of branch instructions retired for all branch types.
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__ALL_BRANCHES = 0x0000ull, // Counts the total number of branch instructions retired for all branch types.
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__CALL = 0xf900ull, // This event is deprecated. Refer to new event BR_INST_RETIRED.NEAR_CALL
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__COND = 0x7e00ull, // Counts the number of retired JCC (Jump on Conditional Code) branch instructions retired
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__COND_TAKEN = 0xfe00ull, // Counts the number of taken JCC (Jump on Conditional Code) branch instructions retired.
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__FAR_BRANCH = 0xbf00ull, // Counts the number of far branch instructions retired
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__INDIRECT = 0xeb00ull, // Counts the number of near indirect JMP and near indirect CALL branch instructions retired.
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__INDIRECT_CALL = 0xfb00ull, // Counts the number of near indirect CALL branch instructions retired.
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__IND_CALL = 0xfb00ull, // This event is deprecated. Refer to new event BR_INST_RETIRED.INDIRECT_CALL
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__JCC = 0x7e00ull, // This event is deprecated. Refer to new event BR_INST_RETIRED.COND
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__NEAR_CALL = 0xf900ull, // Counts the number of near CALL branch instructions retired.
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__NEAR_RETURN = 0xf700ull, // Counts the number of near RET branch instructions retired.
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__NEAR_TAKEN = 0xc000ull, // Counts the number of near taken branch instructions retired.
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__NON_RETURN_IND = 0xeb00ull, // This event is deprecated. Refer to new event BR_INST_RETIRED.INDIRECT
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__REL_CALL = 0xfd00ull, // Counts the number of near relative CALL branch instructions retired.
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__RETURN = 0xf700ull, // This event is deprecated. Refer to new event BR_INST_RETIRED.NEAR_RETURN
		BR_INST_RETIRED__MASK__ADL_GRT_BR_INST_RETIRED__TAKEN_JCC = 0xfe00ull, // This event is deprecated. Refer to new event BR_INST_RETIRED.COND_TAKEN
		BR_MISP_RETIRED = 0x00c5, // Counts the total number of mispredicted branch instructions retired for all branch types.
		BR_MISP_RETIRED__MASK__ADL_GRT_BR_MISP_RETIRED__ALL_BRANCHES = 0x0000ull, // Counts the total number of mispredicted branch instructions retired for all branch types.
		BR_MISP_RETIRED__MASK__ADL_GRT_BR_MISP_RETIRED__COND = 0x7e00ull, // Counts the number of mispredicted JCC (Jump on Conditional Code) branch instructions retired.
		BR_MISP_RETIRED__MASK__ADL_GRT_BR_MISP_RETIRED__COND_TAKEN = 0xfe00ull, // Counts the number of mispredicted taken JCC (Jump on Conditional Code) branch instructions retired.
		BR_MISP_RETIRED__MASK__ADL_GRT_BR_MISP_RETIRED__INDIRECT = 0xeb00ull, // Counts the number of mispredicted near indirect JMP and near indirect CALL branch instructions retired.
		BR_MISP_RETIRED__MASK__ADL_GRT_BR_MISP_RETIRED__INDIRECT_CALL = 0xfb00ull, // Counts the number of mispredicted near indirect CALL branch instructions retired.
		BR_MISP_RETIRED__MASK__ADL_GRT_BR_MISP_RETIRED__IND_CALL = 0xfb00ull, // This event is deprecated. Refer to new event BR_MISP_RETIRED.INDIRECT_CALL
		BR_MISP_RETIRED__MASK__ADL_GRT_BR_MISP_RETIRED__JCC = 0x7e00ull, // This event is deprecated. Refer to new event BR_MISP_RETIRED.COND
		BR_MISP_RETIRED__MASK__ADL_GRT_BR_MISP_RETIRED__NEAR_TAKEN = 0x8000ull, // Counts the number of mispredicted near taken branch instructions retired.
		BR_MISP_RETIRED__MASK__ADL_GRT_BR_MISP_RETIRED__NON_RETURN_IND = 0xeb00ull, // This event is deprecated. Refer to new event BR_MISP_RETIRED.INDIRECT
		BR_MISP_RETIRED__MASK__ADL_GRT_BR_MISP_RETIRED__RETURN = 0xf700ull, // Counts the number of mispredicted near RET branch instructions retired.
		BR_MISP_RETIRED__MASK__ADL_GRT_BR_MISP_RETIRED__TAKEN_JCC = 0xfe00ull, // This event is deprecated. Refer to new event BR_MISP_RETIRED.COND_TAKEN
		CPU_CLK_UNHALTED = 0x003c, // Counts the number of unhalted core clock cycles. (Fixed event)
		CPU_CLK_UNHALTED__MASK__ADL_GRT_CPU_CLK_UNHALTED__CORE = 0x0200ull, // Counts the number of unhalted core clock cycles. (Fixed event)
		CPU_CLK_UNHALTED__MASK__ADL_GRT_CPU_CLK_UNHALTED__CORE_P = 0x0000ull, // Counts the number of unhalted core clock cycles.
		CPU_CLK_UNHALTED__MASK__ADL_GRT_CPU_CLK_UNHALTED__REF_TSC = 0x0300ull, // Counts the number of unhalted reference clock cycles at TSC frequency. (Fixed event)
		CPU_CLK_UNHALTED__MASK__ADL_GRT_CPU_CLK_UNHALTED__REF_TSC_P = 0x0100ull, // Counts the number of unhalted reference clock cycles at TSC frequency.
		CPU_CLK_UNHALTED__MASK__ADL_GRT_CPU_CLK_UNHALTED__THREAD = 0x0200ull, // Counts the number of unhalted core clock cycles. (Fixed event)
		CPU_CLK_UNHALTED__MASK__ADL_GRT_CPU_CLK_UNHALTED__THREAD_P = 0x0000ull, // Counts the number of unhalted core clock cycles.
		DTLB_LOAD_MISSES = 0x0008, // Counts the number of page walks completed due to load DTLB misses to any page size.
		DTLB_LOAD_MISSES__MASK__ADL_GRT_DTLB_LOAD_MISSES__WALK_COMPLETED = 0x0e00ull, // Counts the number of page walks completed due to load DTLB misses to any page size.
		DTLB_STORE_MISSES = 0x0049, // Counts the number of page walks completed due to store DTLB misses to any page size.
		DTLB_STORE_MISSES__MASK__ADL_GRT_DTLB_STORE_MISSES__WALK_COMPLETED = 0x0e00ull, // Counts the number of page walks completed due to store DTLB misses to any page size.
		ICACHE = 0x0080, // Counts the number of instruction cache misses.
		ICACHE__MASK__ADL_GRT_ICACHE__ACCESSES = 0x0300ull, // Counts the number of requests to the instruction cache for one or more bytes of a cache line.
		ICACHE__MASK__ADL_GRT_ICACHE__MISSES = 0x0200ull, // Counts the number of instruction cache misses.
		INST_RETIRED = 0x00c0, // Counts the total number of instructions retired. (Fixed event)
		INST_RETIRED__MASK__ADL_GRT_INST_RETIRED__ANY = 0x0100ull, // Counts the total number of instructions retired. (Fixed event)
		INST_RETIRED__MASK__ADL_GRT_INST_RETIRED__ANY_P = 0x0000ull, // Counts the total number of instructions retired.
		ITLB_MISSES = 0x0085, // Counts the number of page walks initiated by a instruction fetch that missed the first and second level TLBs.
		ITLB_MISSES__MASK__ADL_GRT_ITLB_MISSES__MISS_CAUSED_WALK = 0x0100ull, // Counts the number of page walks initiated by a instruction fetch that missed the first and second level TLBs.
		ITLB_MISSES__MASK__ADL_GRT_ITLB_MISSES__PDE_CACHE_MISS = 0x8000ull, // Counts the number of page walks due to an instruction fetch that miss the PDE (Page Directory Entry) cache.
		ITLB_MISSES__MASK__ADL_GRT_ITLB_MISSES__WALK_COMPLETED = 0x0e00ull, // Counts the number of page walks completed due to instruction fetch misses to any page size.
		LBR_INSERTS = 0x00e4, // This event is deprecated. [This event is alias to MISC_RETIRED.LBR_INSERTS]
		LBR_INSERTS__MASK__ADL_GRT_LBR_INSERTS__ANY = 0x0100ull, // This event is deprecated. [This event is alias to MISC_RETIRED.LBR_INSERTS]
		LD_BLOCKS = 0x0003, // Counts the number of retired loads that are blocked because its address exactly matches an older store whose data is not ready.
		LD_BLOCKS__MASK__ADL_GRT_LD_BLOCKS__4K_ALIAS = 0x0400ull, // This event is deprecated. Refer to new event LD_BLOCKS.ADDRESS_ALIAS
		LD_BLOCKS__MASK__ADL_GRT_LD_BLOCKS__ADDRESS_ALIAS = 0x0400ull, // Counts the number of retired loads that are blocked because it initially appears to be store forward blocked
		LD_BLOCKS__MASK__ADL_GRT_LD_BLOCKS__DATA_UNKNOWN = 0x0100ull, // Counts the number of retired loads that are blocked because its address exactly matches an older store whose data is not ready.
		LD_HEAD = 0x0005, // Counts the number of cycles that the head (oldest load) of the load buffer and retirement are both stalled due to a DL1 miss.
		LD_HEAD__MASK__ADL_GRT_LD_HEAD__ANY_AT_RET = 0xff00ull, // Counts the number of cycles that the head (oldest load) of the load buffer is stalled due to any number of reasons
		LD_HEAD__MASK__ADL_GRT_LD_HEAD__DTLB_MISS_AT_RET = 0x9000ull, // Counts the number of cycles that the head (oldest load) of the load buffer and retirement are both stalled due to a DTLB miss.
		LD_HEAD__MASK__ADL_GRT_LD_HEAD__L1_BOUND_AT_RET = 0xf400ull, // Counts the number of cycles that the head (oldest load) of the load buffer is stalled due to a core bound stall including a store address match
		LD_HEAD__MASK__ADL_GRT_LD_HEAD__L1_MISS_AT_RET = 0x8100ull, // Counts the number of cycles that the head (oldest load) of the load buffer and retirement are both stalled due to a DL1 miss.
		LD_HEAD__MASK__ADL_GRT_LD_HEAD__OTHER_AT_RET = 0xc000ull, // Counts the number of cycles that the head (oldest load) of the load buffer and retirement are both stalled due to other block cases.
		LD_HEAD__MASK__ADL_GRT_LD_HEAD__PGWALK_AT_RET = 0xa000ull, // Counts the number of cycles that the head (oldest load) of the load buffer and retirement are both stalled due to a pagewalk.
		LD_HEAD__MASK__ADL_GRT_LD_HEAD__ST_ADDR_AT_RET = 0x8400ull, // Counts the number of cycles that the head (oldest load) of the load buffer and retirement are both stalled due to a store address match.
		LONGEST_LAT_CACHE = 0x002e, // Counts the number of cacheable memory requests that miss in the LLC. Counts on a per core basis.
		LONGEST_LAT_CACHE__MASK__ADL_GRT_LONGEST_LAT_CACHE__MISS = 0x4100ull, // Counts the number of cacheable memory requests that miss in the LLC. Counts on a per core basis.
		LONGEST_LAT_CACHE__MASK__ADL_GRT_LONGEST_LAT_CACHE__REFERENCE = 0x4f00ull, // Counts the number of cacheable memory requests that access the LLC. Counts on a per core basis.
		MACHINE_CLEARS = 0x00c3, // Counts the number of machine clears due to program modifying data (self modifying code) within 1K of a recently fetched code page.
		MACHINE_CLEARS__MASK__ADL_GRT_MACHINE_CLEARS__DISAMBIGUATION = 0x0800ull, // Counts the number of machine clears due to memory ordering in which an internal load passes an older store within the same CPU.
		MACHINE_CLEARS__MASK__ADL_GRT_MACHINE_CLEARS__FP_ASSIST = 0x0400ull, // Counts the number of floating point operations retired that required microcode assist.
		MACHINE_CLEARS__MASK__ADL_GRT_MACHINE_CLEARS__MEMORY_ORDERING = 0x0200ull, // Counts the number of machine clears due to memory ordering caused by a snoop from an external agent. Does not count internally generated machine clears such as those due to memory disambiguation.
		MACHINE_CLEARS__MASK__ADL_GRT_MACHINE_CLEARS__MRN_NUKE = 0x8000ull, // Counts the number of machines clears due to memory renaming.
		MACHINE_CLEARS__MASK__ADL_GRT_MACHINE_CLEARS__PAGE_FAULT = 0x2000ull, // Counts the number of machine clears due to a page fault.  Counts both I-Side and D-Side (Loads/Stores) page faults.  A page fault occurs when either the page is not present
		MACHINE_CLEARS__MASK__ADL_GRT_MACHINE_CLEARS__SLOW = 0x6f00ull, // Counts the number of machine clears that flush the pipeline and restart the machine with the use of microcode due to SMC
		MACHINE_CLEARS__MASK__ADL_GRT_MACHINE_CLEARS__SMC = 0x0100ull, // Counts the number of machine clears due to program modifying data (self modifying code) within 1K of a recently fetched code page.
		MEM_BOUND_STALLS = 0x0034, // Counts the number of cycles the core is stalled due to a demand load which hit in the L2 cache.
		MEM_BOUND_STALLS__MASK__ADL_GRT_MEM_BOUND_STALLS__IFETCH = 0x3800ull, // Counts the number of cycles the core is stalled due to an instruction cache or TLB miss which hit in the L2
		MEM_BOUND_STALLS__MASK__ADL_GRT_MEM_BOUND_STALLS__IFETCH_DRAM_HIT = 0x2000ull, // Counts the number of cycles the core is stalled due to an instruction cache or TLB miss which hit in DRAM or MMIO (Non-DRAM).
		MEM_BOUND_STALLS__MASK__ADL_GRT_MEM_BOUND_STALLS__IFETCH_L2_HIT = 0x0800ull, // Counts the number of cycles the core is stalled due to an instruction cache or TLB miss which hit in the L2 cache.
		MEM_BOUND_STALLS__MASK__ADL_GRT_MEM_BOUND_STALLS__IFETCH_LLC_HIT = 0x1000ull, // Counts the number of cycles the core is stalled due to an instruction cache or TLB miss which hit in the LLC or other core with HITE/F/M.
		MEM_BOUND_STALLS__MASK__ADL_GRT_MEM_BOUND_STALLS__LOAD = 0x0700ull, // Counts the number of cycles the core is stalled due to a demand load miss which hit in the L2
		MEM_BOUND_STALLS__MASK__ADL_GRT_MEM_BOUND_STALLS__LOAD_DRAM_HIT = 0x0400ull, // Counts the number of cycles the core is stalled due to a demand load miss which hit in DRAM or MMIO (Non-DRAM).
		MEM_BOUND_STALLS__MASK__ADL_GRT_MEM_BOUND_STALLS__LOAD_L2_HIT = 0x0100ull, // Counts the number of cycles the core is stalled due to a demand load which hit in the L2 cache.
		MEM_BOUND_STALLS__MASK__ADL_GRT_MEM_BOUND_STALLS__LOAD_LLC_HIT = 0x0200ull, // Counts the number of cycles the core is stalled due to a demand load which hit in the LLC or other core with HITE/F/M.
		MEM_LOAD_UOPS_RETIRED = 0x00d1, // Counts the number of load uops retired that hit in the L2 cache.
		MEM_LOAD_UOPS_RETIRED__MASK__ADL_GRT_MEM_LOAD_UOPS_RETIRED__DRAM_HIT = 0x8000ull, // Counts the number of load uops retired that hit in DRAM.
		MEM_LOAD_UOPS_RETIRED__MASK__ADL_GRT_MEM_LOAD_UOPS_RETIRED__L2_HIT = 0x0200ull, // Counts the number of load uops retired that hit in the L2 cache.
		MEM_LOAD_UOPS_RETIRED__MASK__ADL_GRT_MEM_LOAD_UOPS_RETIRED__L3_HIT = 0x0400ull, // Counts the number of load uops retired that hit in the L3 cache.
		MEM_SCHEDULER_BLOCK = 0x0004, // Counts the number of cycles that uops are blocked due to a store buffer full condition.
		MEM_SCHEDULER_BLOCK__MASK__ADL_GRT_MEM_SCHEDULER_BLOCK__ALL = 0x0700ull, // load buffer
		MEM_SCHEDULER_BLOCK__MASK__ADL_GRT_MEM_SCHEDULER_BLOCK__LD_BUF = 0x0200ull, // Counts the number of cycles that uops are blocked due to a load buffer full condition.
		MEM_SCHEDULER_BLOCK__MASK__ADL_GRT_MEM_SCHEDULER_BLOCK__RSV = 0x0400ull, // Counts the number of cycles that uops are blocked due to an RSV full condition.
		MEM_SCHEDULER_BLOCK__MASK__ADL_GRT_MEM_SCHEDULER_BLOCK__ST_BUF = 0x0100ull, // Counts the number of cycles that uops are blocked due to a store buffer full condition.
		MEM_UOPS_RETIRED = 0x00d0, // Counts the number of tagged loads with an instruction latency that exceeds or equals the threshold of 4 cycles as defined in MEC_CR_PEBS_LD_LAT_THRESHOLD (3F6H). Only counts with PEBS enabled.
		MEM_UOPS_RETIRED__MASK__ADL_GRT_MEM_UOPS_RETIRED__ALL_LOADS = 0x8100ull, // Counts the number of load uops retired.
		MEM_UOPS_RETIRED__MASK__ADL_GRT_MEM_UOPS_RETIRED__ALL_STORES = 0x8200ull, // Counts the number of store uops retired.
		MEM_UOPS_RETIRED__MASK__ADL_GRT_MEM_UOPS_RETIRED__LOAD_LATENCY = 0x500ull, // Memory load instructions retired above programmed clocks
		MEM_UOPS_RETIRED__MASK__ADL_GRT_MEM_UOPS_RETIRED__LOAD_LATENCY_GT_128 = 0x8000ull, // Counts the number of tagged loads with an instruction latency that exceeds or equals the threshold of 128 cycles as defined in MEC_CR_PEBS_LD_LAT_THRESHOLD (3F6H). Only counts with PEBS enabled.
		MEM_UOPS_RETIRED__MASK__ADL_GRT_MEM_UOPS_RETIRED__LOAD_LATENCY_GT_16 = 0x1000ull, // Counts the number of tagged loads with an instruction latency that exceeds or equals the threshold of 16 cycles as defined in MEC_CR_PEBS_LD_LAT_THRESHOLD (3F6H). Only counts with PEBS enabled.
		MEM_UOPS_RETIRED__MASK__ADL_GRT_MEM_UOPS_RETIRED__LOAD_LATENCY_GT_256 = 0x10000ull, // Counts the number of tagged loads with an instruction latency that exceeds or equals the threshold of 256 cycles as defined in MEC_CR_PEBS_LD_LAT_THRESHOLD (3F6H). Only counts with PEBS enabled.
		MEM_UOPS_RETIRED__MASK__ADL_GRT_MEM_UOPS_RETIRED__LOAD_LATENCY_GT_32 = 0x2000ull, // Counts the number of tagged loads with an instruction latency that exceeds or equals the threshold of 32 cycles as defined in MEC_CR_PEBS_LD_LAT_THRESHOLD (3F6H). Only counts with PEBS enabled.
		MEM_UOPS_RETIRED__MASK__ADL_GRT_MEM_UOPS_RETIRED__LOAD_LATENCY_GT_4 = 0x0400ull, // Counts the number of tagged loads with an instruction latency that exceeds or equals the threshold of 4 cycles as defined in MEC_CR_PEBS_LD_LAT_THRESHOLD (3F6H). Only counts with PEBS enabled.
		MEM_UOPS_RETIRED__MASK__ADL_GRT_MEM_UOPS_RETIRED__LOAD_LATENCY_GT_512 = 0x20000ull, // Counts the number of tagged loads with an instruction latency that exceeds or equals the threshold of 512 cycles as defined in MEC_CR_PEBS_LD_LAT_THRESHOLD (3F6H). Only counts with PEBS enabled.
		MEM_UOPS_RETIRED__MASK__ADL_GRT_MEM_UOPS_RETIRED__LOAD_LATENCY_GT_64 = 0x4000ull, // Counts the number of tagged loads with an instruction latency that exceeds or equals the threshold of 64 cycles as defined in MEC_CR_PEBS_LD_LAT_THRESHOLD (3F6H). Only counts with PEBS enabled.
		MEM_UOPS_RETIRED__MASK__ADL_GRT_MEM_UOPS_RETIRED__LOAD_LATENCY_GT_8 = 0x0800ull, // Counts the number of tagged loads with an instruction latency that exceeds or equals the threshold of 8 cycles as defined in MEC_CR_PEBS_LD_LAT_THRESHOLD (3F6H). Only counts with PEBS enabled.
		MEM_UOPS_RETIRED__MASK__ADL_GRT_MEM_UOPS_RETIRED__SPLIT_LOADS = 0x4100ull, // Counts the number of retired split load uops.
		MEM_UOPS_RETIRED__MASK__ADL_GRT_MEM_UOPS_RETIRED__STORE_LATENCY = 0x0600ull, // Counts the number of stores uops retired. Counts with or without PEBS enabled.
		MISC_RETIRED = 0x00e4, // Counts the number of LBR entries recorded. Requires LBRs to be enabled in IA32_LBR_CTL. [This event is alias to LBR_INSERTS.ANY]
		MISC_RETIRED__MASK__ADL_GRT_MISC_RETIRED__LBR_INSERTS = 0x0100ull, // Counts the number of LBR entries recorded. Requires LBRs to be enabled in IA32_LBR_CTL. [This event is alias to LBR_INSERTS.ANY]
		OCR0 = 0x01b7, // Counts demand data reads that have any type of response.
		OCR0__MASK__ADL_GRT_OCR__COREWB_M_ANY_RESPONSE = 0x1000800ull, // Counts modified writebacks from L1 cache and L2 cache that have any type of response.
		OCR0__MASK__ADL_GRT_OCR__DEMAND_DATA_RD_ANY_RESPONSE = 0x1000100ull, // Counts demand data reads that have any type of response.
		OCR0__MASK__ADL_GRT_OCR__DEMAND_DATA_RD_L3_HIT = 0x3f803c000100ull, // Counts demand data reads that were supplied by the L3 cache.
		OCR0__MASK__ADL_GRT_OCR__DEMAND_DATA_RD_L3_HIT_SNOOP_HITM = 0x10003c000100ull, // Counts demand data reads that were supplied by the L3 cache where a snoop was sent
		OCR0__MASK__ADL_GRT_OCR__DEMAND_DATA_RD_L3_HIT_SNOOP_HIT_NO_FWD = 0x4003c000100ull, // Counts demand data reads that were supplied by the L3 cache where a snoop was sent
		OCR0__MASK__ADL_GRT_OCR__DEMAND_DATA_RD_L3_HIT_SNOOP_HIT_WITH_FWD = 0x8003c000100ull, // Counts demand data reads that were supplied by the L3 cache where a snoop was sent
		OCR0__MASK__ADL_GRT_OCR__DEMAND_DATA_RD_L3_MISS = 0x3f8440000100ull, // Counts demand data reads that were not supplied by the L3 cache.
		OCR0__MASK__ADL_GRT_OCR__DEMAND_DATA_RD_L3_MISS_LOCAL = 0x3f8440000100ull, // Counts demand data reads that were not supplied by the L3 cache. [L3_MISS_LOCAL is alias to L3_MISS]
		OCR0__MASK__ADL_GRT_OCR__DEMAND_RFO_ANY_RESPONSE = 0x1000200ull, // Counts demand reads for ownership (RFO) and software prefetches for exclusive ownership (PREFETCHW) that have any type of response.
		OCR0__MASK__ADL_GRT_OCR__DEMAND_RFO_L3_HIT = 0x3f803c000200ull, // Counts demand reads for ownership (RFO) and software prefetches for exclusive ownership (PREFETCHW) that were supplied by the L3 cache.
		OCR0__MASK__ADL_GRT_OCR__DEMAND_RFO_L3_HIT_SNOOP_HITM = 0x10003c000200ull, // Counts demand reads for ownership (RFO) and software prefetches for exclusive ownership (PREFETCHW) that were supplied by the L3 cache where a snoop was sent
		OCR0__MASK__ADL_GRT_OCR__DEMAND_RFO_L3_MISS = 0x3f8440000200ull, // Counts demand reads for ownership (RFO) and software prefetches for exclusive ownership (PREFETCHW) that were not supplied by the L3 cache.
		OCR0__MASK__ADL_GRT_OCR__DEMAND_RFO_L3_MISS_LOCAL = 0x3f8440000200ull, // Counts demand reads for ownership (RFO) and software prefetches for exclusive ownership (PREFETCHW) that were not supplied by the L3 cache. [L3_MISS_LOCAL is alias to L3_MISS]
		OCR0__MASK__ADL_GRT_OCR__STREAMING_WR_ANY_RESPONSE = 0x1080000ull, // Counts streaming stores that have any type of response.
		OCR1 = 0x02b7, // Counts demand data reads that have any type of response.
		OCR1__MASK__ADL_GRT_OCR__COREWB_M_ANY_RESPONSE = 0x1000800ull, // Counts modified writebacks from L1 cache and L2 cache that have any type of response.
		OCR1__MASK__ADL_GRT_OCR__DEMAND_DATA_RD_ANY_RESPONSE = 0x1000100ull, // Counts demand data reads that have any type of response.
		OCR1__MASK__ADL_GRT_OCR__DEMAND_DATA_RD_L3_HIT = 0x3f803c000100ull, // Counts demand data reads that were supplied by the L3 cache.
		OCR1__MASK__ADL_GRT_OCR__DEMAND_DATA_RD_L3_HIT_SNOOP_HITM = 0x10003c000100ull, // Counts demand data reads that were supplied by the L3 cache where a snoop was sent
		OCR1__MASK__ADL_GRT_OCR__DEMAND_DATA_RD_L3_HIT_SNOOP_HIT_NO_FWD = 0x4003c000100ull, // Counts demand data reads that were supplied by the L3 cache where a snoop was sent
		OCR1__MASK__ADL_GRT_OCR__DEMAND_DATA_RD_L3_HIT_SNOOP_HIT_WITH_FWD = 0x8003c000100ull, // Counts demand data reads that were supplied by the L3 cache where a snoop was sent
		OCR1__MASK__ADL_GRT_OCR__DEMAND_DATA_RD_L3_MISS = 0x3f8440000100ull, // Counts demand data reads that were not supplied by the L3 cache.
		OCR1__MASK__ADL_GRT_OCR__DEMAND_DATA_RD_L3_MISS_LOCAL = 0x3f8440000100ull, // Counts demand data reads that were not supplied by the L3 cache. [L3_MISS_LOCAL is alias to L3_MISS]
		OCR1__MASK__ADL_GRT_OCR__DEMAND_RFO_ANY_RESPONSE = 0x1000200ull, // Counts demand reads for ownership (RFO) and software prefetches for exclusive ownership (PREFETCHW) that have any type of response.
		OCR1__MASK__ADL_GRT_OCR__DEMAND_RFO_L3_HIT = 0x3f803c000200ull, // Counts demand reads for ownership (RFO) and software prefetches for exclusive ownership (PREFETCHW) that were supplied by the L3 cache.
		OCR1__MASK__ADL_GRT_OCR__DEMAND_RFO_L3_HIT_SNOOP_HITM = 0x10003c000200ull, // Counts demand reads for ownership (RFO) and software prefetches for exclusive ownership (PREFETCHW) that were supplied by the L3 cache where a snoop was sent
		OCR1__MASK__ADL_GRT_OCR__DEMAND_RFO_L3_MISS = 0x3f8440000200ull, // Counts demand reads for ownership (RFO) and software prefetches for exclusive ownership (PREFETCHW) that were not supplied by the L3 cache.
		OCR1__MASK__ADL_GRT_OCR__DEMAND_RFO_L3_MISS_LOCAL = 0x3f8440000200ull, // Counts demand reads for ownership (RFO) and software prefetches for exclusive ownership (PREFETCHW) that were not supplied by the L3 cache. [L3_MISS_LOCAL is alias to L3_MISS]
		OCR1__MASK__ADL_GRT_OCR__STREAMING_WR_ANY_RESPONSE = 0x1080000ull, // Counts streaming stores that have any type of response.
		SERIALIZATION = 0x0075, // Counts the number of issue slots not consumed by the backend due to a micro-sequencer (MS) scoreboard
		SERIALIZATION__MASK__ADL_GRT_SERIALIZATION__NON_C01_MS_SCB = 0x0200ull, // Counts the number of issue slots not consumed by the backend due to a micro-sequencer (MS) scoreboard
		TOPDOWN_BAD_SPECULATION = 0x0073, // Counts the total number of issue slots that were not consumed by the backend because allocation is stalled due to a mispredicted jump or a machine clear.
		TOPDOWN_BAD_SPECULATION__MASK__ADL_GRT_TOPDOWN_BAD_SPECULATION__ALL = 0x0000ull, // Counts the total number of issue slots that were not consumed by the backend because allocation is stalled due to a mispredicted jump or a machine clear.
		TOPDOWN_BAD_SPECULATION__MASK__ADL_GRT_TOPDOWN_BAD_SPECULATION__FASTNUKE = 0x0200ull, // Counts the number of issue slots every cycle that were not consumed by the backend due to fast nukes such as memory ordering and memory disambiguation machine clears.
		TOPDOWN_BAD_SPECULATION__MASK__ADL_GRT_TOPDOWN_BAD_SPECULATION__MACHINE_CLEARS = 0x0300ull, // Counts the total number of issue slots that were not consumed by the backend because allocation is stalled due to a machine clear (nuke) of any kind including memory ordering and memory disambiguation.
		TOPDOWN_BAD_SPECULATION__MASK__ADL_GRT_TOPDOWN_BAD_SPECULATION__MISPREDICT = 0x0400ull, // Counts the number of issue slots every cycle that were not consumed by the backend due to branch mispredicts.
		TOPDOWN_BAD_SPECULATION__MASK__ADL_GRT_TOPDOWN_BAD_SPECULATION__NUKE = 0x0100ull, // Counts the number of issue slots every cycle that were not consumed by the backend due to a machine clear (nuke).
		TOPDOWN_BE_BOUND = 0x0074, // Counts the total number of issue slots every cycle that were not consumed by the backend due to backend stalls.
		TOPDOWN_BE_BOUND__MASK__ADL_GRT_TOPDOWN_BE_BOUND__ALL = 0x0000ull, // Counts the total number of issue slots every cycle that were not consumed by the backend due to backend stalls.
		TOPDOWN_BE_BOUND__MASK__ADL_GRT_TOPDOWN_BE_BOUND__ALLOC_RESTRICTIONS = 0x0100ull, // Counts the number of issue slots every cycle that were not consumed by the backend due to certain allocation restrictions.
		TOPDOWN_BE_BOUND__MASK__ADL_GRT_TOPDOWN_BE_BOUND__MEM_SCHEDULER = 0x0200ull, // Counts the number of issue slots every cycle that were not consumed by the backend due to memory reservation stalls in which a scheduler is not able to accept uops.
		TOPDOWN_BE_BOUND__MASK__ADL_GRT_TOPDOWN_BE_BOUND__NON_MEM_SCHEDULER = 0x0800ull, // Counts the number of issue slots every cycle that were not consumed by the backend due to IEC or FPC RAT stalls
		TOPDOWN_BE_BOUND__MASK__ADL_GRT_TOPDOWN_BE_BOUND__REGISTER = 0x2000ull, // Counts the number of issue slots every cycle that were not consumed by the backend due to the physical register file unable to accept an entry (marble stalls).
		TOPDOWN_BE_BOUND__MASK__ADL_GRT_TOPDOWN_BE_BOUND__REORDER_BUFFER = 0x4000ull, // Counts the number of issue slots every cycle that were not consumed by the backend due to the reorder buffer being full (ROB stalls).
		TOPDOWN_BE_BOUND__MASK__ADL_GRT_TOPDOWN_BE_BOUND__SERIALIZATION = 0x1000ull, // Counts the number of issue slots every cycle that were not consumed by the backend due to scoreboards from the instruction queue (IQ)
		TOPDOWN_FE_BOUND = 0x0071, // Counts the total number of issue slots every cycle that were not consumed by the backend due to frontend stalls.
		TOPDOWN_FE_BOUND__MASK__ADL_GRT_TOPDOWN_FE_BOUND__ALL = 0x0000ull, // Counts the total number of issue slots every cycle that were not consumed by the backend due to frontend stalls.
		TOPDOWN_FE_BOUND__MASK__ADL_GRT_TOPDOWN_FE_BOUND__BRANCH_DETECT = 0x0200ull, // Counts the number of issue slots every cycle that were not delivered by the frontend due to BACLEARS.
		TOPDOWN_FE_BOUND__MASK__ADL_GRT_TOPDOWN_FE_BOUND__BRANCH_RESTEER = 0x4000ull, // Counts the number of issue slots every cycle that were not delivered by the frontend due to BTCLEARS.
		TOPDOWN_FE_BOUND__MASK__ADL_GRT_TOPDOWN_FE_BOUND__CISC = 0x0100ull, // Counts the number of issue slots every cycle that were not delivered by the frontend due to the microcode sequencer (MS).
		TOPDOWN_FE_BOUND__MASK__ADL_GRT_TOPDOWN_FE_BOUND__DECODE = 0x0800ull, // Counts the number of issue slots every cycle that were not delivered by the frontend due to decode stalls.
		TOPDOWN_FE_BOUND__MASK__ADL_GRT_TOPDOWN_FE_BOUND__FRONTEND_BANDWIDTH = 0x8d00ull, // Counts the number of issue slots every cycle that were not delivered by the frontend due to frontend bandwidth restrictions due to decode
		TOPDOWN_FE_BOUND__MASK__ADL_GRT_TOPDOWN_FE_BOUND__FRONTEND_LATENCY = 0x7200ull, // Counts the number of issue slots every cycle that were not delivered by the frontend due to a latency related stalls including BACLEARs
		TOPDOWN_FE_BOUND__MASK__ADL_GRT_TOPDOWN_FE_BOUND__ICACHE = 0x2000ull, // Counts the number of issue slots every cycle that were not delivered by the frontend due to instruction cache misses.
		TOPDOWN_FE_BOUND__MASK__ADL_GRT_TOPDOWN_FE_BOUND__ITLB = 0x1000ull, // Counts the number of issue slots every cycle that were not delivered by the frontend due to ITLB misses.
		TOPDOWN_FE_BOUND__MASK__ADL_GRT_TOPDOWN_FE_BOUND__OTHER = 0x8000ull, // Counts the number of issue slots every cycle that were not delivered by the frontend due to other common frontend stalls not categorized.
		TOPDOWN_FE_BOUND__MASK__ADL_GRT_TOPDOWN_FE_BOUND__PREDECODE = 0x0400ull, // Counts the number of issue slots every cycle that were not delivered by the frontend due to wrong predecodes.
		TOPDOWN_RETIRING = 0x00c2, // Counts the total number of consumed retirement slots.
		TOPDOWN_RETIRING__MASK__ADL_GRT_TOPDOWN_RETIRING__ALL = 0x0000ull, // Counts the total number of consumed retirement slots.
		UOPS_RETIRED = 0x00c2, // Counts the total number of uops retired.
		UOPS_RETIRED__MASK__ADL_GRT_UOPS_RETIRED__ALL = 0x0000ull, // Counts the total number of uops retired.
		UOPS_RETIRED__MASK__ADL_GRT_UOPS_RETIRED__FPDIV = 0x0800ull, // Counts the number of floating point divide uops retired (x87 and SSE
		UOPS_RETIRED__MASK__ADL_GRT_UOPS_RETIRED__IDIV = 0x1000ull, // Counts the number of integer divide uops retired.
		UOPS_RETIRED__MASK__ADL_GRT_UOPS_RETIRED__MS = 0x0100ull, // Counts the number of uops that are from complex flows issued by the micro-sequencer (MS).
		UOPS_RETIRED__MASK__ADL_GRT_UOPS_RETIRED__X87 = 0x0200ull, // Counts the number of x87 uops retired
		
	};
};

namespace adl_grt = optkit::intel::adl_grt;

#undef INTEL_X86_EDGE_BIT
#undef INTEL_X86_ANY_BIT
#undef INTEL_X86_INV_BIT
#undef INTEL_X86_CMASK_BIT
#undef INTEL_X86_MOD_EDGE
#undef INTEL_X86_MOD_ANY
#undef INTEL_X86_MOD_INV
