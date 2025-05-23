#pragma once
#include <cstdint>
#define INTEL_X86_EDGE_BIT	18
#define INTEL_X86_ANY_BIT	21
#define INTEL_X86_INV_BIT	23
#define INTEL_X86_CMASK_BIT 24
#define INTEL_X86_MOD_EDGE	(1 << INTEL_X86_EDGE_BIT)
#define INTEL_X86_MOD_ANY	(1 << INTEL_X86_ANY_BIT)
#define INTEL_X86_MOD_INV	(1 << INTEL_X86_INV_BIT)
namespace optkit::intel::icx_unc_irp{
	enum icx_unc_irp : uint64_t {
		UNC_I_CACHE_TOTAL_OCCUPANCY = 0x000f, // Total IRP occupancy of inbound read and write requests to coherent memory.
		UNC_I_CACHE_TOTAL_OCCUPANCY__MASK__ICX_UNC_I_CACHE_TOTAL_OCCUPANCY__ANY = 0x0100ull, // Any Source (experimental)
		UNC_I_CACHE_TOTAL_OCCUPANCY__MASK__ICX_UNC_I_CACHE_TOTAL_OCCUPANCY__IV_Q = 0x0200ull, // Snoops (experimental)
		UNC_I_CACHE_TOTAL_OCCUPANCY__MASK__ICX_UNC_I_CACHE_TOTAL_OCCUPANCY__MEM = 0x0400ull, // TBD
		UNC_I_CLOCKTICKS = 0x0001, // Clockticks of the IO coherency tracker (IRP)
		UNC_I_COHERENT_OPS = 0x0010, // PCIITOM request issued by the IRP unit to the mesh with the intention of writing a full cacheline.
		UNC_I_COHERENT_OPS__MASK__ICX_UNC_I_COHERENT_OPS__CLFLUSH = 0x8000ull, // CLFlush (experimental)
		UNC_I_COHERENT_OPS__MASK__ICX_UNC_I_COHERENT_OPS__PCITOM = 0x1000ull, // TBD
		UNC_I_COHERENT_OPS__MASK__ICX_UNC_I_COHERENT_OPS__RFO = 0x0800ull, // TBD (experimental)
		UNC_I_COHERENT_OPS__MASK__ICX_UNC_I_COHERENT_OPS__WBMTOI = 0x4000ull, // WbMtoI
		UNC_I_FAF_FULL = 0x0017, // FAF RF full
		UNC_I_FAF_INSERTS = 0x0018, // Inbound read requests received by the IRP and inserted into the FAF queue.
		UNC_I_FAF_OCCUPANCY = 0x0019, // Occupancy of the IRP FAF queue.
		UNC_I_FAF_TRANSACTIONS = 0x0016, // FAF allocation -- sent to ADQ
		UNC_I_IRP_ALL = 0x0020, // TBD
		UNC_I_IRP_ALL__MASK__ICX_UNC_I_IRP_ALL__EVICTS = 0x0400ull, // All Inserts Outbound (BL
		UNC_I_IRP_ALL__MASK__ICX_UNC_I_IRP_ALL__INBOUND_INSERTS = 0x0100ull, // All Inserts Inbound (p2p + faf + cset)
		UNC_I_IRP_ALL__MASK__ICX_UNC_I_IRP_ALL__OUTBOUND_INSERTS = 0x0200ull, // All Inserts Outbound (BL
		UNC_I_MISC0 = 0x001e, // Counts Timeouts - Set 0
		UNC_I_MISC0__MASK__ICX_UNC_I_MISC0__2ND_ATOMIC_INSERT = 0x1000ull, // Cache Inserts of Atomic Transactions as Secondary (experimental)
		UNC_I_MISC0__MASK__ICX_UNC_I_MISC0__2ND_RD_INSERT = 0x0400ull, // Cache Inserts of Read Transactions as Secondary (experimental)
		UNC_I_MISC0__MASK__ICX_UNC_I_MISC0__2ND_WR_INSERT = 0x0800ull, // Cache Inserts of Write Transactions as Secondary (experimental)
		UNC_I_MISC0__MASK__ICX_UNC_I_MISC0__FAST_REJ = 0x0200ull, // Fastpath Rejects (experimental)
		UNC_I_MISC0__MASK__ICX_UNC_I_MISC0__FAST_REQ = 0x0100ull, // Fastpath Requests (experimental)
		UNC_I_MISC0__MASK__ICX_UNC_I_MISC0__FAST_XFER = 0x2000ull, // Fastpath Transfers From Primary to Secondary (experimental)
		UNC_I_MISC0__MASK__ICX_UNC_I_MISC0__PF_ACK_HINT = 0x4000ull, // Prefetch Ack Hints From Primary to Secondary (experimental)
		UNC_I_MISC0__MASK__ICX_UNC_I_MISC0__SLOWPATH_FWPF_NO_PRF = 0x8000ull, // Slow path fwpf didn't find prefetch (experimental)
		UNC_I_MISC1 = 0x001f, // Misc Events - Set 1
		UNC_I_MISC1__MASK__ICX_UNC_I_MISC1__LOST_FWD = 0x1000ull, // Lost Forward
		UNC_I_MISC1__MASK__ICX_UNC_I_MISC1__SEC_RCVD_INVLD = 0x2000ull, // Received Invalid (experimental)
		UNC_I_MISC1__MASK__ICX_UNC_I_MISC1__SEC_RCVD_VLD = 0x4000ull, // Received Valid (experimental)
		UNC_I_MISC1__MASK__ICX_UNC_I_MISC1__SLOW_E = 0x0400ull, // Slow Transfer of E Line (experimental)
		UNC_I_MISC1__MASK__ICX_UNC_I_MISC1__SLOW_I = 0x0100ull, // Slow Transfer of I Line (experimental)
		UNC_I_MISC1__MASK__ICX_UNC_I_MISC1__SLOW_M = 0x0800ull, // Slow Transfer of M Line (experimental)
		UNC_I_MISC1__MASK__ICX_UNC_I_MISC1__SLOW_S = 0x0200ull, // Slow Transfer of S Line (experimental)
		UNC_I_P2P_INSERTS = 0x0014, // P2P Requests (experimental)
		UNC_I_P2P_OCCUPANCY = 0x0015, // P2P Occupancy (experimental)
		UNC_I_P2P_TRANSACTIONS = 0x0013, // P2P Transactions
		UNC_I_P2P_TRANSACTIONS__MASK__ICX_UNC_I_P2P_TRANSACTIONS__CMPL = 0x0800ull, // P2P completions (experimental)
		UNC_I_P2P_TRANSACTIONS__MASK__ICX_UNC_I_P2P_TRANSACTIONS__LOC = 0x4000ull, // match if local only (experimental)
		UNC_I_P2P_TRANSACTIONS__MASK__ICX_UNC_I_P2P_TRANSACTIONS__LOC_AND_TGT_MATCH = 0x8000ull, // match if local and target matches (experimental)
		UNC_I_P2P_TRANSACTIONS__MASK__ICX_UNC_I_P2P_TRANSACTIONS__MSG = 0x0400ull, // P2P Message (experimental)
		UNC_I_P2P_TRANSACTIONS__MASK__ICX_UNC_I_P2P_TRANSACTIONS__RD = 0x0100ull, // P2P reads (experimental)
		UNC_I_P2P_TRANSACTIONS__MASK__ICX_UNC_I_P2P_TRANSACTIONS__REM = 0x1000ull, // Match if remote only (experimental)
		UNC_I_P2P_TRANSACTIONS__MASK__ICX_UNC_I_P2P_TRANSACTIONS__REM_AND_TGT_MATCH = 0x2000ull, // match if remote and target matches (experimental)
		UNC_I_P2P_TRANSACTIONS__MASK__ICX_UNC_I_P2P_TRANSACTIONS__WR = 0x0200ull, // P2P Writes (experimental)
		UNC_I_SNOOP_RESP = 0x0012, // Responses to snoops of any type that hit M line in the IIO cache
		UNC_I_SNOOP_RESP__MASK__ICX_UNC_I_SNOOP_RESP__ALL_HIT = 0x7e00ull, // TBD (experimental)
		UNC_I_SNOOP_RESP__MASK__ICX_UNC_I_SNOOP_RESP__ALL_HIT_ES = 0x7400ull, // TBD (experimental)
		UNC_I_SNOOP_RESP__MASK__ICX_UNC_I_SNOOP_RESP__ALL_HIT_I = 0x7200ull, // TBD (experimental)
		UNC_I_SNOOP_RESP__MASK__ICX_UNC_I_SNOOP_RESP__ALL_HIT_M = 0x7800ull, // TBD
		UNC_I_SNOOP_RESP__MASK__ICX_UNC_I_SNOOP_RESP__ALL_MISS = 0x7100ull, // TBD (experimental)
		UNC_I_SNOOP_RESP__MASK__ICX_UNC_I_SNOOP_RESP__HIT_ES = 0x0400ull, // Hit E or S (experimental)
		UNC_I_SNOOP_RESP__MASK__ICX_UNC_I_SNOOP_RESP__HIT_I = 0x0200ull, // Hit I (experimental)
		UNC_I_SNOOP_RESP__MASK__ICX_UNC_I_SNOOP_RESP__HIT_M = 0x0800ull, // Hit M (experimental)
		UNC_I_SNOOP_RESP__MASK__ICX_UNC_I_SNOOP_RESP__MISS = 0x0100ull, // Miss (experimental)
		UNC_I_SNOOP_RESP__MASK__ICX_UNC_I_SNOOP_RESP__SNPCODE = 0x1000ull, // SnpCode (experimental)
		UNC_I_SNOOP_RESP__MASK__ICX_UNC_I_SNOOP_RESP__SNPDATA = 0x2000ull, // SnpData (experimental)
		UNC_I_SNOOP_RESP__MASK__ICX_UNC_I_SNOOP_RESP__SNPINV = 0x4000ull, // SnpInv (experimental)
		UNC_I_TRANSACTIONS = 0x0011, // Inbound write (fast path) requests received by the IRP.
		UNC_I_TRANSACTIONS__MASK__ICX_UNC_I_TRANSACTIONS__ATOMIC = 0x1000ull, // Atomic (experimental)
		UNC_I_TRANSACTIONS__MASK__ICX_UNC_I_TRANSACTIONS__ORDERINGQ = 0x4000ull, // Select Source (experimental)
		UNC_I_TRANSACTIONS__MASK__ICX_UNC_I_TRANSACTIONS__OTHER = 0x2000ull, // Other (experimental)
		UNC_I_TRANSACTIONS__MASK__ICX_UNC_I_TRANSACTIONS__WRITES = 0x0200ull, // Writes (experimental)
		UNC_I_TRANSACTIONS__MASK__ICX_UNC_I_TRANSACTIONS__WR_PREF = 0x0800ull, // TBD
		UNC_I_TxC_AK_INSERTS = 0x000b, // AK Egress Allocations (experimental)
		UNC_I_TxC_BL_DRS_CYCLES_FULL = 0x0005, // BL DRS Egress Cycles Full (experimental)
		UNC_I_TxC_BL_DRS_INSERTS = 0x0002, // BL DRS Egress Inserts (experimental)
		UNC_I_TxC_BL_DRS_OCCUPANCY = 0x0008, // BL DRS Egress Occupancy (experimental)
		UNC_I_TxC_BL_NCB_CYCLES_FULL = 0x0006, // BL NCB Egress Cycles Full (experimental)
		UNC_I_TxC_BL_NCB_INSERTS = 0x0003, // BL NCB Egress Inserts (experimental)
		UNC_I_TxC_BL_NCB_OCCUPANCY = 0x0009, // BL NCB Egress Occupancy (experimental)
		UNC_I_TxC_BL_NCS_CYCLES_FULL = 0x0007, // BL NCS Egress Cycles Full (experimental)
		UNC_I_TxC_BL_NCS_INSERTS = 0x0004, // BL NCS Egress Inserts (experimental)
		UNC_I_TxC_BL_NCS_OCCUPANCY = 0x000a, // BL NCS Egress Occupancy (experimental)
		UNC_I_TxR2_AD01_STALL_CREDIT_CYCLES = 0x001c, // UNC_I_TxR2_AD01_STALL_CREDIT_CYCLES (experimental)
		UNC_I_TxR2_AD0_STALL_CREDIT_CYCLES = 0x001a, // No AD0 Egress Credits Stalls (experimental)
		UNC_I_TxR2_AD1_STALL_CREDIT_CYCLES = 0x001b, // No AD1 Egress Credits Stalls (experimental)
		UNC_I_TxR2_BL_STALL_CREDIT_CYCLES = 0x001d, // No BL Egress Credit Stalls (experimental)
		UNC_I_TxS_DATA_INSERTS_NCB = 0x000d, // Outbound Read Requests (experimental)
		UNC_I_TxS_DATA_INSERTS_NCS = 0x000e, // Outbound Read Requests (experimental)
		UNC_I_TxS_REQUEST_OCCUPANCY = 0x000c, // Outbound Request Queue Occupancy (experimental)
		
	};
};

namespace icx_unc_irp = optkit::intel::icx_unc_irp;

#undef INTEL_X86_EDGE_BIT
#undef INTEL_X86_ANY_BIT
#undef INTEL_X86_INV_BIT
#undef INTEL_X86_CMASK_BIT
#undef INTEL_X86_MOD_EDGE
#undef INTEL_X86_MOD_ANY
#undef INTEL_X86_MOD_INV
