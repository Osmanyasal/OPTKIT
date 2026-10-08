#pragma once
#include <cstdint>
namespace optkit::::{
	enum  : uint64_t {
		 = 0x00, // Incremented by writes to the Software Increment Register
		__REPEAT__1 = 0x01, // Instruction fetches that cause lowest-level cache miss
		__REPEAT__2 = 0x02, // Instruction fetches that cause lowest-level TLB miss
		__REPEAT__3 = 0x03, // Data read or writes that cause lowest-level cache miss
		__REPEAT__4 = 0x04, // Data read or writes that cause lowest-level cache access
		__REPEAT__5 = 0x05, // Data read or writes that cause lowest-level TLB refill
		__REPEAT__6 = 0x06, // Data read architecturally executed
		__REPEAT__7 = 0x07, // Data write architecturally executed
		__REPEAT__8 = 0x08, // Instructions architecturally executed
		__REPEAT__9 = 0x09, // Counts each exception taken
		__REPEAT__10 = 0x0a, // Exception returns architecturally executed
		__REPEAT__11 = 0x0b, // Instruction writes to Context ID Register
		__REPEAT__12 = 0x0c, // Software change of PC.  Equivalent to branches
		__REPEAT__13 = 0x0d, // Immediate branches architecturally executed
		__REPEAT__14 = 0x0e, // Procedure returns architecturally executed
		__REPEAT__15 = 0x0f, // Unaligned accesses architecturally executed
		__REPEAT__16 = 0x10, // Branches mispredicted or not predicted
		__REPEAT__17 = 0x11, // Clock cycles
		__REPEAT__18 = 0x12, // Branches that could have been predicted
		__REPEAT__19 = 0x40, // Cycles Write buffer full
		__REPEAT__20 = 0x41, // Stores merged in L2
		__REPEAT__21 = 0x42, // Bufferable store transactions to L2
		__REPEAT__22 = 0x43, // Accesses to L2 cache
		__REPEAT__23 = 0x44, // L2 cache misses
		__REPEAT__24 = 0x45, // Cycles with active AXI read channel transactions
		__REPEAT__25 = 0x46, // Cycles with Active AXI write channel transactions
		__REPEAT__26 = 0x47, // Memory replay events
		__REPEAT__27 = 0x48, // Unaligned accesses causing replays
		__REPEAT__28 = 0x49, // L1 data misses due to hashing algorithm
		__REPEAT__29 = 0x4a, // L1 instruction misses due to hashing algorithm
		__REPEAT__30 = 0x4b, // L1 data access where page color alias occurs
		__REPEAT__31 = 0x4c, // NEON accesses that hit in L1 cache
		__REPEAT__32 = 0x4d, // NEON cache accesses for L1 cache
		__REPEAT__33 = 0x4e, // L2 accesses caused by NEON
		__REPEAT__34 = 0x4f, // L2 hits caused by NEON
		__REPEAT__35 = 0x50, // L1 instruction cache accesses
		__REPEAT__36 = 0x51, // Return stack mispredictions
		__REPEAT__37 = 0x52, // Branch prediction failures
		__REPEAT__38 = 0x53, // Branches predicted taken
		__REPEAT__39 = 0x54, // Taken branches executed
		__REPEAT__40 = 0x55, // Operations executed (includes sub-ops in multi-cycle instructions)
		__REPEAT__41 = 0x56, // Cycles no instruction is available for issue
		__REPEAT__42 = 0x57, // Number of instructions issued in cycle
		__REPEAT__43 = 0x58, // Cycles stalled waiting on NEON MRC data
		__REPEAT__44 = 0x59, // Cycles stalled due to full NEON queues
		__REPEAT__45 = 0x5a, // Cycles NEON and integer processors both not idle
		__REPEAT__46 = 0x70, // External PMUEXTIN[0] event
		__REPEAT__47 = 0x71, // External PMUEXTIN[1] event
		__REPEAT__48 = 0x72, // External PMUEXTIN[0] or PMUEXTIN[1] event
		__REPEAT__49 = 0xff, // CPU cycles
		
	};
};

namespace  = optkit::::;

