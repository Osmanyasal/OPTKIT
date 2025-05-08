#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::tmt{
	enum tmt : uint64_t {
		UNHALTED_CORE_CYCLES = 0x3c, // Count core clock cycles whenever the clock signal on the specific core is running (not halted)
		UNHALTED_REFERENCE_CYCLES = 0x0300, // Unhalted reference cycles
		INSTRUCTION_RETIRED = 0xc0, // Number of instructions at retirement
		INSTRUCTIONS_RETIRED = 0xc0, // This is an alias for INSTRUCTION_RETIRED
		OCR = 0x01b7, // Counts demand data reads that have any response type.
		OCR__MASK__INTEL_TMT_OCR__DEMAND_RFO_L3_MISS = 0x3f0400000200ull, // Counts all demand reads for ownership (RFO) requests and software based prefetches for exclusive ownership (PREFETCHW) that was not supplied by the L3 cache.
		OCR__MASK__INTEL_TMT_OCR__DEMAND_RFO_ANY_RESPONSE = 0x1000200ull, // Counts all demand reads for ownership (RFO) requests and software based prefetches for exclusive ownership (PREFETCHW) that have any response type.
		OCR__MASK__INTEL_TMT_OCR__DEMAND_DATA_RD_L3_MISS = 0x3f0400000100ull, // Counts demand data reads that was not supplied by the L3 cache.
		OCR__MASK__INTEL_TMT_OCR__DEMAND_DATA_RD_ANY_RESPONSE = 0x1000100ull, // Counts demand data reads that have any response type.
		OFFCORE_RESPONSE_0 = 0x01b7, // Counts demand data reads that have any response type.
		OFFCORE_RESPONSE_0__MASK__INTEL_TMT_OCR__DEMAND_RFO_L3_MISS = 0x3f0400000200ull, // Counts all demand reads for ownership (RFO) requests and software based prefetches for exclusive ownership (PREFETCHW) that was not supplied by the L3 cache.
		OFFCORE_RESPONSE_0__MASK__INTEL_TMT_OCR__DEMAND_RFO_ANY_RESPONSE = 0x1000200ull, // Counts all demand reads for ownership (RFO) requests and software based prefetches for exclusive ownership (PREFETCHW) that have any response type.
		OFFCORE_RESPONSE_0__MASK__INTEL_TMT_OCR__DEMAND_DATA_RD_L3_MISS = 0x3f0400000100ull, // Counts demand data reads that was not supplied by the L3 cache.
		OFFCORE_RESPONSE_0__MASK__INTEL_TMT_OCR__DEMAND_DATA_RD_ANY_RESPONSE = 0x1000100ull, // Counts demand data reads that have any response type.
		OFFCORE_RESPONSE_1 = 0x02b7, // Counts demand data reads that have any response type.
		OFFCORE_RESPONSE_1__MASK__INTEL_TMT_OCR__DEMAND_RFO_L3_MISS = 0x3f0400000200ull, // Counts all demand reads for ownership (RFO) requests and software based prefetches for exclusive ownership (PREFETCHW) that was not supplied by the L3 cache.
		OFFCORE_RESPONSE_1__MASK__INTEL_TMT_OCR__DEMAND_RFO_ANY_RESPONSE = 0x1000200ull, // Counts all demand reads for ownership (RFO) requests and software based prefetches for exclusive ownership (PREFETCHW) that have any response type.
		OFFCORE_RESPONSE_1__MASK__INTEL_TMT_OCR__DEMAND_DATA_RD_L3_MISS = 0x3f0400000100ull, // Counts demand data reads that was not supplied by the L3 cache.
		OFFCORE_RESPONSE_1__MASK__INTEL_TMT_OCR__DEMAND_DATA_RD_ANY_RESPONSE = 0x1000100ull, // Counts demand data reads that have any response type.
		MEM_LOAD_UOPS_RETIRED = 0x00d1, // Counts the number of load uops retired that hit the level 1 data cache
		MEM_LOAD_UOPS_RETIRED__MASK__INTEL_TMT_MEM_LOAD_UOPS_RETIRED__L2_MISS = 0x1000ull, // Counts the number of load uops retired that miss in the level 2 cache
		MEM_LOAD_UOPS_RETIRED__MASK__INTEL_TMT_MEM_LOAD_UOPS_RETIRED__L1_MISS = 0x0800ull, // Counts the number of load uops retired that miss in the level 1 data cache
		MEM_LOAD_UOPS_RETIRED__MASK__INTEL_TMT_MEM_LOAD_UOPS_RETIRED__L3_HIT = 0x0400ull, // Counts the number of load uops retired that miss in the level 3 cache
		MEM_LOAD_UOPS_RETIRED__MASK__INTEL_TMT_MEM_LOAD_UOPS_RETIRED__L2_HIT = 0x0200ull, // Counts the number of load uops retired that hit in the level 2 cache
		MEM_LOAD_UOPS_RETIRED__MASK__INTEL_TMT_MEM_LOAD_UOPS_RETIRED__L1_HIT = 0x0100ull, // Counts the number of load uops retired that hit the level 1 data cache
		MEM_UOPS_RETIRED = 0x00d0, // Counts the number of load uops retired.
		MEM_UOPS_RETIRED__MASK__INTEL_TMT_MEM_UOPS_RETIRED__ALL_STORES = 0x8200ull, // Counts the number of store uops retired.
		MEM_UOPS_RETIRED__MASK__INTEL_TMT_MEM_UOPS_RETIRED__ALL_LOADS = 0x8100ull, // Counts the number of load uops retired.
		CYCLES_DIV_BUSY = 0x00cd, // Counts cycles the floating point divider or integer divider or both are busy.  Does not imply a stall waiting for either divider.
		CYCLES_DIV_BUSY__MASK__INTEL_TMT_CYCLES_DIV_BUSY__ANY = 0x0000ull, // Counts cycles the floating point divider or integer divider or both are busy.  Does not imply a stall waiting for either divider.
		BR_MISP_RETIRED = 0x00c5, // Counts the number of mispredicted branch instructions retired.
		BR_MISP_RETIRED__MASK__INTEL_TMT_BR_MISP_RETIRED__ALL_BRANCHES = 0x0000ull, // Counts the number of mispredicted branch instructions retired.
		BR_INST_RETIRED = 0x00c4, // Counts the number of branch instructions retired for all branch types.
		BR_INST_RETIRED__MASK__INTEL_TMT_BR_INST_RETIRED__ALL_BRANCHES = 0x0000ull, // Counts the number of branch instructions retired for all branch types.
		MACHINE_CLEARS = 0x00c3, // Counts all machine clears due to
		MACHINE_CLEARS__MASK__INTEL_TMT_MACHINE_CLEARS__ANY = 0x0000ull, // Counts all machine clears due to
		ITLB_MISSES = 0x0085, // Page walk completed due to an instruction fetch in a 4K page.
		ITLB_MISSES__MASK__INTEL_TMT_ITLB_MISSES__WALK_COMPLETED_2M_4M = 0x0400ull, // Page walk completed due to an instruction fetch in a 2M or 4M page.
		ITLB_MISSES__MASK__INTEL_TMT_ITLB_MISSES__WALK_COMPLETED_4K = 0x0200ull, // Page walk completed due to an instruction fetch in a 4K page.
		ITLB = 0x0081, // Counts the number of times there was an ITLB miss and a new translation was filled into the ITLB.
		ITLB__MASK__INTEL_TMT_ITLB__FILLS = 0x0400ull, // Counts the number of times there was an ITLB miss and a new translation was filled into the ITLB.
		ICACHE = 0x0080, // Counts requests to the Instruction Cache (ICache) for one or more bytes in a cache line and they do not hit in the ICache (miss).
		ICACHE__MASK__INTEL_TMT_ICACHE__ACCESSES = 0x0300ull, // Counts requests to the Instruction Cache (ICache) for one or more bytes cache Line.
		ICACHE__MASK__INTEL_TMT_ICACHE__MISSES = 0x0200ull, // Counts requests to the Instruction Cache (ICache) for one or more bytes in a cache line and they do not hit in the ICache (miss).
		DTLB_STORE_MISSES = 0x0049, // Page walk completed due to a demand data store to a 4K page.
		DTLB_STORE_MISSES__MASK__INTEL_TMT_DTLB_STORE_MISSES__WALK_COMPLETED_2M_4M = 0x0400ull, // Page walk completed due to a demand data store to a 2M or 4M page.
		DTLB_STORE_MISSES__MASK__INTEL_TMT_DTLB_STORE_MISSES__WALK_COMPLETED_4K = 0x0200ull, // Page walk completed due to a demand data store to a 4K page.
		LONGEST_LAT_CACHE = 0x002e, // Counts memory requests originating from the core that miss in the last level cache. If the platform has an L3 cache
		LONGEST_LAT_CACHE__MASK__INTEL_TMT_LONGEST_LAT_CACHE__REFERENCE = 0x4f00ull, // Counts memory requests originating from the core that reference a cache line in the last level cache. If the platform has an L3 cache
		LONGEST_LAT_CACHE__MASK__INTEL_TMT_LONGEST_LAT_CACHE__MISS = 0x4100ull, // Counts memory requests originating from the core that miss in the last level cache. If the platform has an L3 cache
		DTLB_LOAD_MISSES = 0x0008, // Page walk completed due to a demand load to a 4K page.
		DTLB_LOAD_MISSES__MASK__INTEL_TMT_DTLB_LOAD_MISSES__WALK_COMPLETED_2M_4M = 0x0400ull, // Page walk completed due to a demand load to a 2M or 4M page.
		DTLB_LOAD_MISSES__MASK__INTEL_TMT_DTLB_LOAD_MISSES__WALK_COMPLETED_4K = 0x0200ull, // Page walk completed due to a demand load to a 4K page.
		CPU_CLK_UNHALTED = 0x003c, // Counts the number of unhalted core clock cycles. (Fixed event)
		CPU_CLK_UNHALTED__MASK__INTEL_TMT_CPU_CLK_UNHALTED__REF = 0x0100ull, // Counts the number of unhalted reference clock cycles at TSC frequency.
		CPU_CLK_UNHALTED__MASK__INTEL_TMT_CPU_CLK_UNHALTED__CORE_P = 0x0000ull, // Counts the number of unhalted core clock cycles.
		CPU_CLK_UNHALTED__MASK__INTEL_TMT_CPU_CLK_UNHALTED__CORE = 0x0000ull, // Counts the number of unhalted core clock cycles.
		CPU_CLK_UNHALTED__MASK__INTEL_TMT_CPU_CLK_UNHALTED__REF_TSC = 0x0300ull, // Counts the number of unhalted reference clock cycles at TSC frequency. (Fixed event)
		
	};
};

namespace tmt = optkit::intel::tmt;