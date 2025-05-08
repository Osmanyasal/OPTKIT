#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::snbep_unc_m{
	enum snbep_unc_m : uint64_t {
		UNC_M_CLOCKTICKS = 0xff, // IMC Uncore clockticks
		UNC_M_ACT_COUNT = 0x1, // DRAM Activate Count
		UNC_M_CAS_COUNT = 0x4, // DRAM RD_CAS and WR_CAS Commands.
		UNC_M_CAS_COUNT__MASK__SNBEP_UNC_M_CAS_COUNT__ALL = 0xf00, // Counts total number of DRAM CAS commands issued on this channel
		UNC_M_CAS_COUNT__MASK__SNBEP_UNC_M_CAS_COUNT__RD = 0x300, // Counts all DRAM reads on this channel
		UNC_M_CAS_COUNT__MASK__SNBEP_UNC_M_CAS_COUNT__RD_REG = 0x100, // Counts number of DRAM read CAS commands issued on this channel
		UNC_M_CAS_COUNT__MASK__SNBEP_UNC_M_CAS_COUNT__RD_UNDERFILL = 0x200, // Counts number of underfill reads issued by the memory controller
		UNC_M_CAS_COUNT__MASK__SNBEP_UNC_M_CAS_COUNT__WR = 0xc00, // Counts number of DRAM write CAS commands on this channel
		UNC_M_CAS_COUNT__MASK__SNBEP_UNC_M_CAS_COUNT__WR_RMM = 0x800, // Counts Number of opportunistic DRAM write CAS commands issued on this channel
		UNC_M_CAS_COUNT__MASK__SNBEP_UNC_M_CAS_COUNT__WR_WMM = 0x400, // Counts number of DRAM write CAS commands issued on this channel while in Write-Major mode
		UNC_M_DRAM_PRE_ALL = 0x6, // DRAM Precharge All Commands
		UNC_M_DRAM_REFRESH = 0x5, // Number of DRAM Refreshes Issued
		UNC_M_DRAM_REFRESH__MASK__SNBEP_UNC_M_DRAM_REFRESH__HIGH = 0x400, // TBD
		UNC_M_DRAM_REFRESH__MASK__SNBEP_UNC_M_DRAM_REFRESH__PANIC = 0x200, // TBD
		UNC_M_ECC_CORRECTABLE_ERRORS = 0x9, // ECC Correctable Errors
		UNC_M_MAJOR_MODES = 0x7, // Cycles in a Major Mode
		UNC_M_MAJOR_MODES__MASK__SNBEP_UNC_M_MAJOR_MODES__ISOCH = 0x800, // Counts cycles in ISOCH Major mode
		UNC_M_MAJOR_MODES__MASK__SNBEP_UNC_M_MAJOR_MODES__PARTIAL = 0x400, // Counts cycles in Partial Major mode
		UNC_M_MAJOR_MODES__MASK__SNBEP_UNC_M_MAJOR_MODES__READ = 0x100, // Counts cycles in Read Major mode
		UNC_M_MAJOR_MODES__MASK__SNBEP_UNC_M_MAJOR_MODES__WRITE = 0x200, // Counts cycles in Write Major mode
		UNC_M_POWER_CHANNEL_DLLOFF = 0x84, // Channel DLLOFF Cycles
		UNC_M_POWER_CHANNEL_PPD = 0x85, // Channel PPD Cycles
		UNC_M_POWER_CKE_CYCLES = 0x83, // CKE_ON_CYCLES by Rank
		UNC_M_POWER_CKE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK0 = 0x100, // Count cycles for rank 0
		UNC_M_POWER_CKE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK1 = 0x200, // Count cycles for rank 1
		UNC_M_POWER_CKE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK2 = 0x400, // Count cycles for rank 2
		UNC_M_POWER_CKE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK3 = 0x800, // Count cycles for rank 3
		UNC_M_POWER_CKE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK4 = 0x1000, // Count cycles for rank 4
		UNC_M_POWER_CKE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK5 = 0x2000, // Count cycles for rank 5
		UNC_M_POWER_CKE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK6 = 0x4000, // Count cycles for rank 6
		UNC_M_POWER_CKE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK7 = 0x8000, // Count cycles for rank 7
		UNC_M_POWER_CRITICAL_THROTTLE_CYCLES = 0x86, // Critical Throttle Cycles
		UNC_M_POWER_SELF_REFRESH = 0x43, // Clock-Enabled Self-Refresh
		UNC_M_POWER_THROTTLE_CYCLES = 0x41, // Throttle Cycles for Rank 0
		UNC_M_POWER_THROTTLE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK0 = 0x100, // Count cycles for rank 0
		UNC_M_POWER_THROTTLE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK1 = 0x200, // Count cycles for rank 1
		UNC_M_POWER_THROTTLE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK2 = 0x400, // Count cycles for rank 2
		UNC_M_POWER_THROTTLE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK3 = 0x800, // Count cycles for rank 3
		UNC_M_POWER_THROTTLE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK4 = 0x1000, // Count cycles for rank 4
		UNC_M_POWER_THROTTLE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK5 = 0x2000, // Count cycles for rank 5
		UNC_M_POWER_THROTTLE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK6 = 0x4000, // Count cycles for rank 6
		UNC_M_POWER_THROTTLE_CYCLES__MASK__SNBEP_UNC_M_POWER_CKE_CYCLES__RANK7 = 0x8000, // Count cycles for rank 7
		UNC_M_PREEMPTION = 0x8, // Read Preemption Count
		UNC_M_PREEMPTION__MASK__SNBEP_UNC_M_PREEMPTION__RD_PREEMPT_RD = 0x100, // Counts read over read preemptions
		UNC_M_PREEMPTION__MASK__SNBEP_UNC_M_PREEMPTION__RD_PREEMPT_WR = 0x200, // Counts read over write preemptions
		UNC_M_PRE_COUNT = 0x2, // DRAM Precharge commands.
		UNC_M_PRE_COUNT__MASK__SNBEP_UNC_M_PRE_COUNT__PAGE_CLOSE = 0x200, // Counts number of DRAM precharge commands sent on this channel as a result of the page close counter expiring
		UNC_M_PRE_COUNT__MASK__SNBEP_UNC_M_PRE_COUNT__PAGE_MISS = 0x100, // Counts number of DRAM precharge commands sent on this channel as a result of page misses
		UNC_M_RPQ_CYCLES_FULL = 0x12, // Read Pending Queue Full Cycles
		UNC_M_RPQ_CYCLES_NE = 0x11, // Read Pending Queue Not Empty
		UNC_M_RPQ_INSERTS = 0x10, // Read Pending Queue Allocations
		UNC_M_RPQ_OCCUPANCY = 0x80, // Read Pending Queue Occupancy
		UNC_M_WPQ_CYCLES_FULL = 0x22, // Write Pending Queue Full Cycles
		UNC_M_WPQ_CYCLES_NE = 0x21, // Write Pending Queue Not Empty
		UNC_M_WPQ_INSERTS = 0x20, // Write Pending Queue Allocations
		UNC_M_WPQ_OCCUPANCY = 0x81, // Write Pending Queue Occupancy
		UNC_M_WPQ_READ_HIT = 0x23, // Write Pending Queue CAM Match
		UNC_M_WPQ_WRITE_HIT = 0x24, // Write Pending Queue CAM Match
		
	};
};

namespace snbep_unc_m = optkit::intel::snbep_unc_m;