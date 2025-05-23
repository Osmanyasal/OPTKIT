#pragma once
#include <cstdint>
#define INTEL_X86_EDGE_BIT	18
#define INTEL_X86_ANY_BIT	21
#define INTEL_X86_INV_BIT	23
#define INTEL_X86_CMASK_BIT 24
#define INTEL_X86_MOD_EDGE	(1 << INTEL_X86_EDGE_BIT)
#define INTEL_X86_MOD_ANY	(1 << INTEL_X86_ANY_BIT)
#define INTEL_X86_MOD_INV	(1 << INTEL_X86_INV_BIT)
namespace optkit::intel::spr_unc_imc{
	enum spr_unc_imc : uint64_t {
		UNC_M_ACT_COUNT = 0x0002, // Count Activation
		UNC_M_ACT_COUNT__MASK__SPR_UNC_M_ACT_COUNT__ALL = 0xff00ull, // Activate due to read
		UNC_M_CAS_COUNT = 0x0005, // DRAM CAS commands issued
		UNC_M_CAS_COUNT__MASK__SPR_UNC_M_CAS_COUNT__ALL = 0xff00ull, // All DRAM CAS commands issued
		UNC_M_CAS_COUNT__MASK__SPR_UNC_M_CAS_COUNT__PCH0 = 0x4000ull, // Pseudo Channel 0 (experimental)
		UNC_M_CAS_COUNT__MASK__SPR_UNC_M_CAS_COUNT__PCH1 = 0x8000ull, // Pseudo Channel 1 (experimental)
		UNC_M_CAS_COUNT__MASK__SPR_UNC_M_CAS_COUNT__RD = 0xcf00ull, // All DRAM read CAS commands issued (including underfills)
		UNC_M_CAS_COUNT__MASK__SPR_UNC_M_CAS_COUNT__RD_PRE_REG = 0xc200ull, // DRAM RD_CAS and WR_CAS Commands. (experimental)
		UNC_M_CAS_COUNT__MASK__SPR_UNC_M_CAS_COUNT__RD_PRE_UNDERFILL = 0xc800ull, // DRAM RD_CAS and WR_CAS Commands. (experimental)
		UNC_M_CAS_COUNT__MASK__SPR_UNC_M_CAS_COUNT__RD_REG = 0xc100ull, // All DRAM read CAS commands issued (does not include underfills) (experimental)
		UNC_M_CAS_COUNT__MASK__SPR_UNC_M_CAS_COUNT__RD_UNDERFILL = 0xc400ull, // DRAM underfill read CAS commands issued (experimental)
		UNC_M_CAS_COUNT__MASK__SPR_UNC_M_CAS_COUNT__WR = 0xf000ull, // All DRAM write CAS commands issued
		UNC_M_CAS_COUNT__MASK__SPR_UNC_M_CAS_COUNT__WR_NONPRE = 0xd000ull, // DRAM WR_CAS commands w/o auto-pre (experimental)
		UNC_M_CAS_COUNT__MASK__SPR_UNC_M_CAS_COUNT__WR_PRE = 0xe000ull, // DRAM RD_CAS and WR_CAS Commands. (experimental)
		UNC_M_CAS_ISSUED_REQ_LEN = 0x0006, // CAS Command in Regular Mode issued
		UNC_M_CAS_ISSUED_REQ_LEN__MASK__SPR_UNC_M_CAS_ISSUED_REQ_LEN__PCH0 = 0x4000ull, // Pseudo Channel 0 (experimental)
		UNC_M_CAS_ISSUED_REQ_LEN__MASK__SPR_UNC_M_CAS_ISSUED_REQ_LEN__PCH1 = 0x8000ull, // Pseudo Channel 1 (experimental)
		UNC_M_CAS_ISSUED_REQ_LEN__MASK__SPR_UNC_M_CAS_ISSUED_REQ_LEN__RD_32B = 0xc800ull, // Read CAS Command in Interleaved Mode (32B) (experimental)
		UNC_M_CAS_ISSUED_REQ_LEN__MASK__SPR_UNC_M_CAS_ISSUED_REQ_LEN__RD_64B = 0xc100ull, // Read CAS Command in Regular Mode (64B) in Pseudochannel 0 (experimental)
		UNC_M_CAS_ISSUED_REQ_LEN__MASK__SPR_UNC_M_CAS_ISSUED_REQ_LEN__RD_UFILL_32B = 0xd000ull, // Underfill Read CAS Command in Interleaved Mode (32B) (experimental)
		UNC_M_CAS_ISSUED_REQ_LEN__MASK__SPR_UNC_M_CAS_ISSUED_REQ_LEN__RD_UFILL_64B = 0xc200ull, // Underfill Read CAS Command in Regular Mode (64B) in Pseudochannel 1 (experimental)
		UNC_M_CAS_ISSUED_REQ_LEN__MASK__SPR_UNC_M_CAS_ISSUED_REQ_LEN__WR_32B = 0xe000ull, // Write CAS Command in Interleaved Mode (32B) (experimental)
		UNC_M_CAS_ISSUED_REQ_LEN__MASK__SPR_UNC_M_CAS_ISSUED_REQ_LEN__WR_64B = 0xc400ull, // Write CAS Command in Regular Mode (64B) in Pseudochannel 0 (experimental)
		UNC_M_CLOCKTICKS = 0x0101, // IMC Clockticks at DCLK frequency
		UNC_M_DRAM_PRE_ALL = 0x0044, // DRAM Precharge All Commands (experimental)
		UNC_M_HCLOCKTICKS = 0x0001, // IMC Clockticks at HCLK frequency
		UNC_M_PCLS = 0x00a0, // TBD
		UNC_M_PCLS__MASK__SPR_UNC_M_PCLS__RD = 0x0500ull, // TBD (experimental)
		UNC_M_PCLS__MASK__SPR_UNC_M_PCLS__TOTAL = 0x0f00ull, // TBD (experimental)
		UNC_M_PCLS__MASK__SPR_UNC_M_PCLS__WR = 0x0a00ull, // TBD (experimental)
		UNC_M_PMM_RPQ_INSERTS = 0x00e3, // PMM Read Pending Queue inserts
		UNC_M_PMM_RPQ_OCCUPANCY = 0x00e0, // PMM Read Pending Queue occupancy
		UNC_M_PMM_RPQ_OCCUPANCY__MASK__SPR_UNC_M_PMM_RPQ_OCCUPANCY__ALL_SCH0 = 0x0100ull, // PMM Read Pending Queue occupancy
		UNC_M_PMM_RPQ_OCCUPANCY__MASK__SPR_UNC_M_PMM_RPQ_OCCUPANCY__ALL_SCH1 = 0x0200ull, // PMM Read Pending Queue occupancy
		UNC_M_PMM_RPQ_OCCUPANCY__MASK__SPR_UNC_M_PMM_RPQ_OCCUPANCY__GNT_WAIT_SCH0 = 0x1000ull, // PMM Read Pending Queue Occupancy (experimental)
		UNC_M_PMM_RPQ_OCCUPANCY__MASK__SPR_UNC_M_PMM_RPQ_OCCUPANCY__GNT_WAIT_SCH1 = 0x2000ull, // PMM Read Pending Queue Occupancy (experimental)
		UNC_M_PMM_RPQ_OCCUPANCY__MASK__SPR_UNC_M_PMM_RPQ_OCCUPANCY__NO_GNT_SCH0 = 0x0400ull, // PMM Read Pending Queue Occupancy (experimental)
		UNC_M_PMM_RPQ_OCCUPANCY__MASK__SPR_UNC_M_PMM_RPQ_OCCUPANCY__NO_GNT_SCH1 = 0x0800ull, // PMM Read Pending Queue Occupancy (experimental)
		UNC_M_PMM_WPQ_CYCLES_NE = 0x00e5, // PMM (for IXP) Write Queue Cycles Not Empty (experimental)
		UNC_M_PMM_WPQ_INSERTS = 0x00e7, // PMM Write Pending Queue inserts
		UNC_M_PMM_WPQ_OCCUPANCY = 0x00e4, // PMM Write Pending Queue Occupancy
		UNC_M_PMM_WPQ_OCCUPANCY__MASK__SPR_UNC_M_PMM_WPQ_OCCUPANCY__ALL = 0x0300ull, // PMM Write Pending Queue Occupancy
		UNC_M_PMM_WPQ_OCCUPANCY__MASK__SPR_UNC_M_PMM_WPQ_OCCUPANCY__ALL_SCH0 = 0x0100ull, // PMM Write Pending Queue Occupancy
		UNC_M_PMM_WPQ_OCCUPANCY__MASK__SPR_UNC_M_PMM_WPQ_OCCUPANCY__ALL_SCH1 = 0x0200ull, // PMM Write Pending Queue Occupancy
		UNC_M_PMM_WPQ_OCCUPANCY__MASK__SPR_UNC_M_PMM_WPQ_OCCUPANCY__CAS = 0x0c00ull, // PMM (for IXP) Write Pending Queue Occupancy (experimental)
		UNC_M_PMM_WPQ_OCCUPANCY__MASK__SPR_UNC_M_PMM_WPQ_OCCUPANCY__PWR = 0x3000ull, // PMM (for IXP) Write Pending Queue Occupancy (experimental)
		UNC_M_POWER_CHANNEL_PPD = 0x0085, // Channel PPD Cycles (experimental)
		UNC_M_POWER_CKE_CYCLES = 0x0047, // Cycles in CKE mode
		UNC_M_POWER_CKE_CYCLES__MASK__SPR_UNC_M_POWER_CKE_CYCLES__LOW_0 = 0x0100ull, // DIMM ID (experimental)
		UNC_M_POWER_CKE_CYCLES__MASK__SPR_UNC_M_POWER_CKE_CYCLES__LOW_1 = 0x0200ull, // DIMM ID (experimental)
		UNC_M_POWER_CKE_CYCLES__MASK__SPR_UNC_M_POWER_CKE_CYCLES__LOW_2 = 0x0400ull, // DIMM ID (experimental)
		UNC_M_POWER_CKE_CYCLES__MASK__SPR_UNC_M_POWER_CKE_CYCLES__LOW_3 = 0x0800ull, // DIMM ID (experimental)
		UNC_M_POWER_CRIT_THROTTLE_CYCLES = 0x0086, // Throttled Cycles
		UNC_M_POWER_CRIT_THROTTLE_CYCLES__MASK__SPR_UNC_M_POWER_CRIT_THROTTLE_CYCLES__SLOT0 = 0x0100ull, // Throttle Cycles for Rank 0 (experimental)
		UNC_M_POWER_CRIT_THROTTLE_CYCLES__MASK__SPR_UNC_M_POWER_CRIT_THROTTLE_CYCLES__SLOT1 = 0x0200ull, // Throttle Cycles for Rank 0 (experimental)
		UNC_M_POWER_SELF_REFRESH = 0x0043, // Clock-Enabled Self-Refresh (experimental)
		UNC_M_PRE_COUNT = 0x0003, // Count number of DRAM Precharge
		UNC_M_PRE_COUNT__MASK__SPR_UNC_M_PRE_COUNT__ALL = 0xff00ull, // Precharge due to read
		UNC_M_PRE_COUNT__MASK__SPR_UNC_M_PRE_COUNT__PGT = 0x8800ull, // DRAM Precharge commands
		UNC_M_PRE_COUNT__MASK__SPR_UNC_M_PRE_COUNT__PGT_PCH0 = 0x0800ull, // Precharges from Page Table (experimental)
		UNC_M_PRE_COUNT__MASK__SPR_UNC_M_PRE_COUNT__PGT_PCH1 = 0x8000ull, // DRAM Precharge commands. (experimental)
		UNC_M_PRE_COUNT__MASK__SPR_UNC_M_PRE_COUNT__RD = 0x1100ull, // Precharge due to read on page miss
		UNC_M_PRE_COUNT__MASK__SPR_UNC_M_PRE_COUNT__RD_PCH0 = 0x0100ull, // Precharge due to read (experimental)
		UNC_M_PRE_COUNT__MASK__SPR_UNC_M_PRE_COUNT__RD_PCH1 = 0x1000ull, // DRAM Precharge commands. (experimental)
		UNC_M_PRE_COUNT__MASK__SPR_UNC_M_PRE_COUNT__UFILL = 0x4400ull, // DRAM Precharge commands. (experimental)
		UNC_M_PRE_COUNT__MASK__SPR_UNC_M_PRE_COUNT__UFILL_PCH0 = 0x0400ull, // DRAM Precharge commands. (experimental)
		UNC_M_PRE_COUNT__MASK__SPR_UNC_M_PRE_COUNT__UFILL_PCH1 = 0x4000ull, // DRAM Precharge commands. (experimental)
		UNC_M_PRE_COUNT__MASK__SPR_UNC_M_PRE_COUNT__WR = 0x2200ull, // Precharge due to write on page miss
		UNC_M_PRE_COUNT__MASK__SPR_UNC_M_PRE_COUNT__WR_PCH0 = 0x0200ull, // Precharge due to write (experimental)
		UNC_M_PRE_COUNT__MASK__SPR_UNC_M_PRE_COUNT__WR_PCH1 = 0x2000ull, // DRAM Precharge commands. (experimental)
		UNC_M_RDB_FULL = 0x0019, // Counts the number of cycles where the read buffer has greater than UMASK elements.  This includes reads to both DDR and PMEM.  NOTE: Umask must be set to the maximum number of elements in the queue (24 entries for SPR). (experimental)
		UNC_M_RDB_INSERTS = 0x0017, // Read Data Buffer Inserts
		UNC_M_RDB_INSERTS__MASK__SPR_UNC_M_RDB_INSERTS__PCH0 = 0x0100ull, // Pseudo-channel 0
		UNC_M_RDB_INSERTS__MASK__SPR_UNC_M_RDB_INSERTS__PCH1 = 0x0200ull, // Pseudo-channel 1
		UNC_M_RDB_NE = 0x0018, // Counts the number of cycles where there is at least one element in the read buffer.  This includes reads to both DDR and PMEM.
		UNC_M_RDB_NE__MASK__SPR_UNC_M_RDB_NE__PCH0 = 0x0100ull, // Pseudo-channel 0 (experimental)
		UNC_M_RDB_NE__MASK__SPR_UNC_M_RDB_NE__PCH1 = 0x0200ull, // Pseudo-channel 1 (experimental)
		UNC_M_RDB_NE__MASK__SPR_UNC_M_RDB_NE__ANY = 0x0300ull, // Read Data Buffer Not Empty any channel (experimental)
		UNC_M_RDB_OCCUPANCY = 0x001a, // Counts the number of elements in the read buffer
		UNC_M_RPQ_INSERTS = 0x0010, // Read Pending Queue Allocations
		UNC_M_RPQ_INSERTS__MASK__SPR_UNC_M_RPQ_INSERTS__PCH0 = 0x0100ull, // Pseudo-channel 0
		UNC_M_RPQ_INSERTS__MASK__SPR_UNC_M_RPQ_INSERTS__PCH1 = 0x0200ull, // Pseudo-channel 1
		UNC_M_RPQ_OCCUPANCY_PCH0 = 0x0080, // Read Pending Queue Occupancy pseudo-channel 0
		UNC_M_RPQ_OCCUPANCY_PCH1 = 0x0081, // Read Pending Queue Occupancy pseudo-channel 0
		UNC_M_SB_ACCESSES = 0x00d2, // Scoreboard accesses
		UNC_M_SB_ACCESSES__MASK__SPR_UNC_M_SB_ACCESSES__ACCEPTS = 0x0500ull, // Scoreboard accepts (experimental)
		UNC_M_SB_ACCESSES__MASK__SPR_UNC_M_SB_ACCESSES__FM_RD_CMPS = 0x4000ull, // Write Accepts (experimental)
		UNC_M_SB_ACCESSES__MASK__SPR_UNC_M_SB_ACCESSES__FM_WR_CMPS = 0x8000ull, // Write Rejects (experimental)
		UNC_M_SB_ACCESSES__MASK__SPR_UNC_M_SB_ACCESSES__NM_RD_CMPS = 0x1000ull, // FM read completions (experimental)
		UNC_M_SB_ACCESSES__MASK__SPR_UNC_M_SB_ACCESSES__NM_WR_CMPS = 0x2000ull, // FM write completions (experimental)
		UNC_M_SB_ACCESSES__MASK__SPR_UNC_M_SB_ACCESSES__RD_ACCEPTS = 0x0100ull, // Read Accepts (experimental)
		UNC_M_SB_ACCESSES__MASK__SPR_UNC_M_SB_ACCESSES__RD_REJECTS = 0x0200ull, // Read Rejects (experimental)
		UNC_M_SB_ACCESSES__MASK__SPR_UNC_M_SB_ACCESSES__REJECTS = 0x0a00ull, // Scoreboard rejects (experimental)
		UNC_M_SB_ACCESSES__MASK__SPR_UNC_M_SB_ACCESSES__WR_ACCEPTS = 0x0400ull, // NM read completions (experimental)
		UNC_M_SB_ACCESSES__MASK__SPR_UNC_M_SB_ACCESSES__WR_REJECTS = 0x0800ull, // NM write completions (experimental)
		UNC_M_SB_CANARY = 0x00d9, // TBD
		UNC_M_SB_CANARY__MASK__SPR_UNC_M_SB_CANARY__ALLOC = 0x0100ull, // Alloc (experimental)
		UNC_M_SB_CANARY__MASK__SPR_UNC_M_SB_CANARY__DEALLOC = 0x0200ull, // Dealloc (experimental)
		UNC_M_SB_CANARY__MASK__SPR_UNC_M_SB_CANARY__FM_RD_STARVED = 0x2000ull, // Near Mem Write Starved (experimental)
		UNC_M_SB_CANARY__MASK__SPR_UNC_M_SB_CANARY__FM_TGR_WR_STARVED = 0x8000ull, // Far Mem Write Starved (experimental)
		UNC_M_SB_CANARY__MASK__SPR_UNC_M_SB_CANARY__FM_WR_STARVED = 0x4000ull, // Far Mem Read Starved (experimental)
		UNC_M_SB_CANARY__MASK__SPR_UNC_M_SB_CANARY__NM_RD_STARVED = 0x0800ull, // Valid (experimental)
		UNC_M_SB_CANARY__MASK__SPR_UNC_M_SB_CANARY__NM_WR_STARVED = 0x1000ull, // Near Mem Read Starved (experimental)
		UNC_M_SB_CANARY__MASK__SPR_UNC_M_SB_CANARY__VLD = 0x0400ull, // Reject (experimental)
		UNC_M_SB_CYCLES_FULL = 0x00d1, // Scoreboard Cycles Full (experimental)
		UNC_M_SB_CYCLES_NE = 0x00d0, // Scoreboard Cycles Not-Empty (experimental)
		UNC_M_SB_INSERTS = 0x00d6, // Scoreboard Inserts
		UNC_M_SB_INSERTS__MASK__SPR_UNC_M_SB_INSERTS__BLOCK_RDS = 0x1000ull, // Block region reads (experimental)
		UNC_M_SB_INSERTS__MASK__SPR_UNC_M_SB_INSERTS__BLOCK_WRS = 0x2000ull, // Block region writes (experimental)
		UNC_M_SB_INSERTS__MASK__SPR_UNC_M_SB_INSERTS__PMM_RDS = 0x0400ull, // Persistent Mem reads (experimental)
		UNC_M_SB_INSERTS__MASK__SPR_UNC_M_SB_INSERTS__PMM_WRS = 0x0800ull, // Persistent Mem writes (experimental)
		UNC_M_SB_INSERTS__MASK__SPR_UNC_M_SB_INSERTS__RDS = 0x0100ull, // Reads (experimental)
		UNC_M_SB_INSERTS__MASK__SPR_UNC_M_SB_INSERTS__WRS = 0x0200ull, // Writes (experimental)
		UNC_M_SB_OCCUPANCY = 0x00d5, // Scoreboard Occupancy
		UNC_M_SB_OCCUPANCY__MASK__SPR_UNC_M_SB_OCCUPANCY__BLOCK_RDS = 0x2000ull, // Block region reads (experimental)
		UNC_M_SB_OCCUPANCY__MASK__SPR_UNC_M_SB_OCCUPANCY__BLOCK_WRS = 0x4000ull, // Block region writes (experimental)
		UNC_M_SB_OCCUPANCY__MASK__SPR_UNC_M_SB_OCCUPANCY__PMM_RDS = 0x0400ull, // Persistent Mem reads (experimental)
		UNC_M_SB_OCCUPANCY__MASK__SPR_UNC_M_SB_OCCUPANCY__PMM_WRS = 0x0800ull, // Persistent Mem writes (experimental)
		UNC_M_SB_OCCUPANCY__MASK__SPR_UNC_M_SB_OCCUPANCY__RDS = 0x0100ull, // Reads (experimental)
		UNC_M_SB_PREF_INSERTS = 0x00da, // Scoreboard Prefetch Inserts
		UNC_M_SB_PREF_INSERTS__MASK__SPR_UNC_M_SB_PREF_INSERTS__ALL = 0x0100ull, // All (experimental)
		UNC_M_SB_PREF_INSERTS__MASK__SPR_UNC_M_SB_PREF_INSERTS__DDR = 0x0200ull, // DDR4 (experimental)
		UNC_M_SB_PREF_INSERTS__MASK__SPR_UNC_M_SB_PREF_INSERTS__PMM = 0x0400ull, // PMM (experimental)
		UNC_M_SB_PREF_OCCUPANCY = 0x00db, // Scoreboard Prefetch Occupancy
		UNC_M_SB_PREF_OCCUPANCY__MASK__SPR_UNC_M_SB_PREF_OCCUPANCY__ALL = 0x0100ull, // All (experimental)
		UNC_M_SB_PREF_OCCUPANCY__MASK__SPR_UNC_M_SB_PREF_OCCUPANCY__DDR = 0x0200ull, // DDR4 (experimental)
		UNC_M_SB_PREF_OCCUPANCY__MASK__SPR_UNC_M_SB_PREF_OCCUPANCY__PMM = 0x0400ull, // Persistent Mem (experimental)
		UNC_M_SB_REJECT = 0x00d4, // Number of Scoreboard Requests Rejected
		UNC_M_SB_REJECT__MASK__SPR_UNC_M_SB_REJECT__CANARY = 0x0800ull, // Number of Scoreboard Requests Rejected (experimental)
		UNC_M_SB_REJECT__MASK__SPR_UNC_M_SB_REJECT__DDR_EARLY_CMP = 0x2000ull, // Number of Scoreboard Requests Rejected (experimental)
		UNC_M_SB_REJECT__MASK__SPR_UNC_M_SB_REJECT__FM_ADDR_CNFLT = 0x0200ull, // FM requests rejected due to full address conflict (experimental)
		UNC_M_SB_REJECT__MASK__SPR_UNC_M_SB_REJECT__NM_SET_CNFLT = 0x0100ull, // NM requests rejected due to set conflict (experimental)
		UNC_M_SB_REJECT__MASK__SPR_UNC_M_SB_REJECT__PATROL_SET_CNFLT = 0x0400ull, // Patrol requests rejected due to set conflict (experimental)
		UNC_M_SB_STRV_ALLOC = 0x00d7, // TBD
		UNC_M_SB_STRV_ALLOC__MASK__SPR_UNC_M_SB_STRV_DEALLOC__FM_RD = 0x0200ull, // Far Mem Read - Set (experimental)
		UNC_M_SB_STRV_ALLOC__MASK__SPR_UNC_M_SB_STRV_DEALLOC__FM_TGR = 0x1000ull, // Near Mem Read - Clear (experimental)
		UNC_M_SB_STRV_ALLOC__MASK__SPR_UNC_M_SB_STRV_DEALLOC__FM_WR = 0x0800ull, // Far Mem Write - Set (experimental)
		UNC_M_SB_STRV_ALLOC__MASK__SPR_UNC_M_SB_STRV_DEALLOC__NM_RD = 0x0100ull, // Near Mem Read - Set (experimental)
		UNC_M_SB_STRV_ALLOC__MASK__SPR_UNC_M_SB_STRV_DEALLOC__NM_WR = 0x0400ull, // Near Mem Write - Set (experimental)
		UNC_M_SB_STRV_DEALLOC = 0x00de, // TBD
		UNC_M_SB_STRV_DEALLOC__MASK__SPR_UNC_M_SB_STRV_DEALLOC__FM_RD = 0x0200ull, // Far Mem Read - Set (experimental)
		UNC_M_SB_STRV_DEALLOC__MASK__SPR_UNC_M_SB_STRV_DEALLOC__FM_TGR = 0x1000ull, // Near Mem Read - Clear (experimental)
		UNC_M_SB_STRV_DEALLOC__MASK__SPR_UNC_M_SB_STRV_DEALLOC__FM_WR = 0x0800ull, // Far Mem Write - Set (experimental)
		UNC_M_SB_STRV_DEALLOC__MASK__SPR_UNC_M_SB_STRV_DEALLOC__NM_RD = 0x0100ull, // Near Mem Read - Set (experimental)
		UNC_M_SB_STRV_DEALLOC__MASK__SPR_UNC_M_SB_STRV_DEALLOC__NM_WR = 0x0400ull, // Near Mem Write - Set (experimental)
		UNC_M_SB_STRV_OCC = 0x00d8, // TBD
		UNC_M_SB_STRV_OCC__MASK__SPR_UNC_M_SB_STRV_OCC__FM_RD = 0x0200ull, // Far Mem Read (experimental)
		UNC_M_SB_STRV_OCC__MASK__SPR_UNC_M_SB_STRV_OCC__FM_TGR = 0x1000ull, // Near Mem Read - Clear (experimental)
		UNC_M_SB_STRV_OCC__MASK__SPR_UNC_M_SB_STRV_OCC__FM_WR = 0x0800ull, // Far Mem Write (experimental)
		UNC_M_SB_STRV_OCC__MASK__SPR_UNC_M_SB_STRV_OCC__NM_RD = 0x0100ull, // Near Mem Read (experimental)
		UNC_M_SB_STRV_OCC__MASK__SPR_UNC_M_SB_STRV_OCC__NM_WR = 0x0400ull, // Near Mem Write (experimental)
		UNC_M_SB_TAGGED = 0x00dd, // TBD
		UNC_M_SB_TAGGED__MASK__SPR_UNC_M_SB_TAGGED__DDR4_CMP = 0x0800ull, // TBD (experimental)
		UNC_M_SB_TAGGED__MASK__SPR_UNC_M_SB_TAGGED__NEW = 0x0100ull, // TBD (experimental)
		UNC_M_SB_TAGGED__MASK__SPR_UNC_M_SB_TAGGED__OCC = 0x8000ull, // TBD (experimental)
		UNC_M_SB_TAGGED__MASK__SPR_UNC_M_SB_TAGGED__PMM0_CMP = 0x1000ull, // TBD (experimental)
		UNC_M_SB_TAGGED__MASK__SPR_UNC_M_SB_TAGGED__PMM1_CMP = 0x2000ull, // TBD (experimental)
		UNC_M_SB_TAGGED__MASK__SPR_UNC_M_SB_TAGGED__PMM2_CMP = 0x4000ull, // TBD (experimental)
		UNC_M_SB_TAGGED__MASK__SPR_UNC_M_SB_TAGGED__RD_HIT = 0x0200ull, // TBD (experimental)
		UNC_M_SB_TAGGED__MASK__SPR_UNC_M_SB_TAGGED__RD_MISS = 0x0400ull, // TBD (experimental)
		UNC_M_TAGCHK = 0x00d3, // 2LM Tag check
		UNC_M_TAGCHK__MASK__SPR_UNC_M_TAGCHK__HIT = 0x0100ull, // 2LM Tag check hit in near memory cache (DDR4)
		UNC_M_TAGCHK__MASK__SPR_UNC_M_TAGCHK__MISS_CLEAN = 0x0200ull, // 2LM Tag check miss
		UNC_M_TAGCHK__MASK__SPR_UNC_M_TAGCHK__MISS_DIRTY = 0x0400ull, // 2LM Tag check miss
		UNC_M_TAGCHK__MASK__SPR_UNC_M_TAGCHK__NM_RD_HIT = 0x0800ull, // 2LM Tag check hit due to memory read
		UNC_M_TAGCHK__MASK__SPR_UNC_M_TAGCHK__NM_WR_HIT = 0x1000ull, // 2LM Tag check hit due to memory write
		UNC_M_WPQ_INSERTS = 0x0020, // Write Pending Queue Allocations
		UNC_M_WPQ_INSERTS__MASK__SPR_UNC_M_WPQ_INSERTS__PCH0 = 0x0100ull, // Pseudo-channel 0
		UNC_M_WPQ_INSERTS__MASK__SPR_UNC_M_WPQ_INSERTS__PCH1 = 0x0200ull, // Pseudo-channel 1
		UNC_M_WPQ_OCCUPANCY_PCH0 = 0x0082, // Write Pending Queue Occupancy pseudo channel 0
		UNC_M_WPQ_OCCUPANCY_PCH1 = 0x0083, // Write Pending Queue Occupancy pseudo channel 1
		UNC_M_WPQ_READ_HIT = 0x0023, // Write Pending Queue CAM Match (experimental)
		UNC_M_WPQ_WRITE_HIT = 0x0024, // Write Pending Queue CAM Match (experimental)
		
	};
};

namespace spr_unc_imc = optkit::intel::spr_unc_imc;

#undef INTEL_X86_EDGE_BIT
#undef INTEL_X86_ANY_BIT
#undef INTEL_X86_INV_BIT
#undef INTEL_X86_CMASK_BIT
#undef INTEL_X86_MOD_EDGE
#undef INTEL_X86_MOD_ANY
#undef INTEL_X86_MOD_INV
