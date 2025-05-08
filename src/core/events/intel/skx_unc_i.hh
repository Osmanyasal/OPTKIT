#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::skx_unc_i{
	enum skx_unc_i : uint64_t {
		UNC_I_CACHE_TOTAL_OCCUPANCY = 0xf, // Accumulates the number of reads and writes that are outstanding in the uncore in each cycle.  This is effectively the sum of the READ_OCCUPANCY and WRITE_OCCUPANCY events.
		UNC_I_CACHE_TOTAL_OCCUPANCY__MASK__SKX_UNC_I_CACHE_TOTAL_OCCUPANCY__ANY = 0x100, // Total Write Cache Occupancy -- Any Source
		UNC_I_CACHE_TOTAL_OCCUPANCY__MASK__SKX_UNC_I_CACHE_TOTAL_OCCUPANCY__IV_Q = 0x200, // Total Write Cache Occupancy -- Snoops
		UNC_I_CACHE_TOTAL_OCCUPANCY__MASK__SKX_UNC_I_CACHE_TOTAL_OCCUPANCY__MEM = 0x400, // Total Write Cache Occupancy -- Mem
		UNC_I_CLOCKTICKS = 0x1, // IRP Clocks
		UNC_I_COHERENT_OPS = 0x10, // Counts the number of coherency related operations servied by the IRP
		UNC_I_COHERENT_OPS__MASK__SKX_UNC_I_COHERENT_OPS__CLFLUSH = 0x8000, // Coherent Ops -- CLFlush
		UNC_I_COHERENT_OPS__MASK__SKX_UNC_I_COHERENT_OPS__CRD = 0x200, // Coherent Ops -- CRd
		UNC_I_COHERENT_OPS__MASK__SKX_UNC_I_COHERENT_OPS__DRD = 0x400, // Coherent Ops -- DRd
		UNC_I_COHERENT_OPS__MASK__SKX_UNC_I_COHERENT_OPS__PCIDCAHINT = 0x2000, // Coherent Ops -- PCIDCAHin5t
		UNC_I_COHERENT_OPS__MASK__SKX_UNC_I_COHERENT_OPS__PCIRDCUR = 0x100, // Coherent Ops -- PCIRdCur
		UNC_I_COHERENT_OPS__MASK__SKX_UNC_I_COHERENT_OPS__PCITOM = 0x1000, // Coherent Ops -- PCIItoM
		UNC_I_COHERENT_OPS__MASK__SKX_UNC_I_COHERENT_OPS__RFO = 0x800, // Coherent Ops -- RFO
		UNC_I_COHERENT_OPS__MASK__SKX_UNC_I_COHERENT_OPS__WBMTOI = 0x4000, // Coherent Ops -- WbMtoI
		UNC_I_FAF_FULL = 0x17, // TBD
		UNC_I_FAF_INSERTS = 0x18, // TBD
		UNC_I_FAF_OCCUPANCY = 0x19, // TBD
		UNC_I_FAF_TRANSACTIONS = 0x16, // TBD
		UNC_I_IRP_ALL = 0x1e, // TBD
		UNC_I_IRP_ALL__MASK__SKX_UNC_I_IRP_ALL__INBOUND_INSERTS = 0x100, // -- All Inserts Inbound (p2p + faf + cset)
		UNC_I_IRP_ALL__MASK__SKX_UNC_I_IRP_ALL__OUTBOUND_INSERTS = 0x200, // -- All Inserts Outbound (BL
		UNC_I_MISC0 = 0x1c, // TBD
		UNC_I_MISC0__MASK__SKX_UNC_I_MISC0__2ND_ATOMIC_INSERT = 0x1000, // Misc Events - Set 0 -- Cache Inserts of Atomic Transactions as Secondary
		UNC_I_MISC0__MASK__SKX_UNC_I_MISC0__2ND_RD_INSERT = 0x400, // Misc Events - Set 0 -- Cache Inserts of Read Transactions as Secondary
		UNC_I_MISC0__MASK__SKX_UNC_I_MISC0__2ND_WR_INSERT = 0x800, // Misc Events - Set 0 -- Cache Inserts of Write Transactions as Secondary
		UNC_I_MISC0__MASK__SKX_UNC_I_MISC0__FAST_REJ = 0x200, // Misc Events - Set 0 -- Fastpath Rejects
		UNC_I_MISC0__MASK__SKX_UNC_I_MISC0__FAST_REQ = 0x100, // Misc Events - Set 0 -- Fastpath Requests
		UNC_I_MISC0__MASK__SKX_UNC_I_MISC0__FAST_XFER = 0x2000, // Misc Events - Set 0 -- Fastpath Transfers From Primary to Secondary
		UNC_I_MISC0__MASK__SKX_UNC_I_MISC0__PF_ACK_HINT = 0x4000, // Misc Events - Set 0 -- Prefetch Ack Hints From Primary to Secondary
		UNC_I_MISC0__MASK__SKX_UNC_I_MISC0__UNKNOWN = 0x8000, // Misc Events - Set 0 --
		UNC_I_MISC1 = 0x1d, // TBD
		UNC_I_MISC1__MASK__SKX_UNC_I_MISC1__LOST_FWD = 0x1000, // Misc Events - Set 1 -- Lost Forward
		UNC_I_MISC1__MASK__SKX_UNC_I_MISC1__SEC_RCVD_INVLD = 0x2000, // Misc Events - Set 1 -- Received Invalid
		UNC_I_MISC1__MASK__SKX_UNC_I_MISC1__SEC_RCVD_VLD = 0x4000, // Misc Events - Set 1 -- Received Valid
		UNC_I_MISC1__MASK__SKX_UNC_I_MISC1__SLOW_E = 0x400, // Misc Events - Set 1 -- Slow Transfer of E Line
		UNC_I_MISC1__MASK__SKX_UNC_I_MISC1__SLOW_I = 0x100, // Misc Events - Set 1 -- Slow Transfer of I Line
		UNC_I_MISC1__MASK__SKX_UNC_I_MISC1__SLOW_M = 0x800, // Misc Events - Set 1 -- Slow Transfer of M Line
		UNC_I_MISC1__MASK__SKX_UNC_I_MISC1__SLOW_S = 0x200, // Misc Events - Set 1 -- Slow Transfer of S Line
		UNC_I_P2P_INSERTS = 0x14, // P2P requests from the ITC
		UNC_I_P2P_OCCUPANCY = 0x15, // P2P B & S Queue Occupancy
		UNC_I_P2P_TRANSACTIONS = 0x13, // TBD
		UNC_I_P2P_TRANSACTIONS__MASK__SKX_UNC_I_P2P_TRANSACTIONS__CMPL = 0x800, // P2P Transactions -- P2P completions
		UNC_I_P2P_TRANSACTIONS__MASK__SKX_UNC_I_P2P_TRANSACTIONS__LOC = 0x4000, // P2P Transactions -- match if local only
		UNC_I_P2P_TRANSACTIONS__MASK__SKX_UNC_I_P2P_TRANSACTIONS__LOC_AND_TGT_MATCH = 0x8000, // P2P Transactions -- match if local and target matches
		UNC_I_P2P_TRANSACTIONS__MASK__SKX_UNC_I_P2P_TRANSACTIONS__MSG = 0x400, // P2P Transactions -- P2P Message
		UNC_I_P2P_TRANSACTIONS__MASK__SKX_UNC_I_P2P_TRANSACTIONS__RD = 0x100, // P2P Transactions -- P2P reads
		UNC_I_P2P_TRANSACTIONS__MASK__SKX_UNC_I_P2P_TRANSACTIONS__REM = 0x1000, // P2P Transactions -- Match if remote only
		UNC_I_P2P_TRANSACTIONS__MASK__SKX_UNC_I_P2P_TRANSACTIONS__REM_AND_TGT_MATCH = 0x2000, // P2P Transactions -- match if remote and target matches
		UNC_I_P2P_TRANSACTIONS__MASK__SKX_UNC_I_P2P_TRANSACTIONS__WR = 0x200, // P2P Transactions -- P2P Writes
		UNC_I_SNOOP_RESP = 0x12, // TBD
		UNC_I_SNOOP_RESP__MASK__SKX_UNC_I_SNOOP_RESP__HIT_ES = 0x400, // Snoop Responses -- Hit E or S
		UNC_I_SNOOP_RESP__MASK__SKX_UNC_I_SNOOP_RESP__HIT_I = 0x200, // Snoop Responses -- Hit I
		UNC_I_SNOOP_RESP__MASK__SKX_UNC_I_SNOOP_RESP__HIT_M = 0x800, // Snoop Responses -- Hit M
		UNC_I_SNOOP_RESP__MASK__SKX_UNC_I_SNOOP_RESP__MISS = 0x100, // Snoop Responses -- Miss
		UNC_I_SNOOP_RESP__MASK__SKX_UNC_I_SNOOP_RESP__SNPCODE = 0x1000, // Snoop Responses -- SnpCode
		UNC_I_SNOOP_RESP__MASK__SKX_UNC_I_SNOOP_RESP__SNPDATA = 0x2000, // Snoop Responses -- SnpData
		UNC_I_SNOOP_RESP__MASK__SKX_UNC_I_SNOOP_RESP__SNPINV = 0x4000, // Snoop Responses -- SnpInv
		UNC_I_TRANSACTIONS = 0x11, // Counts the number of Inbound transactions from the IRP to the Uncore.  This can be filtered based on request type in addition to the source queue.  Note the special filtering equation.  We do OR-reduction on the request type.  If the SOURCE bit is set
		UNC_I_TRANSACTIONS__MASK__SKX_UNC_I_TRANSACTIONS__ATOMIC = 0x1000, // Inbound Transaction Count -- Atomic
		UNC_I_TRANSACTIONS__MASK__SKX_UNC_I_TRANSACTIONS__OTHER = 0x2000, // Inbound Transaction Count -- Other
		UNC_I_TRANSACTIONS__MASK__SKX_UNC_I_TRANSACTIONS__RD_PREF = 0x400, // Inbound Transaction Count -- Read Prefetches
		UNC_I_TRANSACTIONS__MASK__SKX_UNC_I_TRANSACTIONS__READS = 0x100, // Inbound Transaction Count -- Reads
		UNC_I_TRANSACTIONS__MASK__SKX_UNC_I_TRANSACTIONS__WRITES = 0x200, // Inbound Transaction Count -- Writes
		UNC_I_TRANSACTIONS__MASK__SKX_UNC_I_TRANSACTIONS__WR_PREF = 0x800, // Inbound Transaction Count -- Write Prefetches
		UNC_I_TXC_AK_INSERTS = 0xb, // TBD
		UNC_I_TXC_BL_DRS_CYCLES_FULL = 0x5, // TBD
		UNC_I_TXC_BL_DRS_INSERTS = 0x2, // TBD
		UNC_I_TXC_BL_DRS_OCCUPANCY = 0x8, // TBD
		UNC_I_TXC_BL_NCB_CYCLES_FULL = 0x6, // TBD
		UNC_I_TXC_BL_NCB_INSERTS = 0x3, // TBD
		UNC_I_TXC_BL_NCB_OCCUPANCY = 0x9, // TBD
		UNC_I_TXC_BL_NCS_CYCLES_FULL = 0x7, // TBD
		UNC_I_TXC_BL_NCS_INSERTS = 0x4, // TBD
		UNC_I_TXC_BL_NCS_OCCUPANCY = 0xa, // TBD
		UNC_I_TXR2_AD_STALL_CREDIT_CYCLES = 0x1a, // Counts the number times when it is not possible to issue a request to the R2PCIe because there are no AD Egress Credits available.
		UNC_I_TXR2_BL_STALL_CREDIT_CYCLES = 0x1b, // Counts the number times when it is not possible to issue data to the R2PCIe because there are no BL Egress Credits available.
		UNC_I_TXS_DATA_INSERTS_NCB = 0xd, // Counts the number of requests issued to the switch (towards the devices).
		UNC_I_TXS_DATA_INSERTS_NCS = 0xe, // Counts the number of requests issued to the switch (towards the devices).
		UNC_I_TXS_REQUEST_OCCUPANCY = 0xc, // Accumultes the number of outstanding outbound requests from the IRP to the switch (towards the devices).  This can be used in conjuection with the allocations event in order to calculate average latency of outbound requests.
		
	};
};

namespace skx_unc_i = optkit::intel::skx_unc_i;