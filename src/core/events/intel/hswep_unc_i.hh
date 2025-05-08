#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::hswep_unc_i{
	enum hswep_unc_i : uint64_t {
		UNC_I_CLOCKTICKS = 0x0, // Number of uclks in domain
		UNC_I_SNOOP_RESP = 0x17, // Snoop responses
		UNC_I_SNOOP_RESP__MASK__HSWEP_UNC_I_SNOOP_RESP__MISS = 0x100, // Miss
		UNC_I_SNOOP_RESP__MASK__HSWEP_UNC_I_SNOOP_RESP__HIT_I = 0x200, // Hit in Invalid state
		UNC_I_SNOOP_RESP__MASK__HSWEP_UNC_I_SNOOP_RESP__HIT_ES = 0x400, // Hit in Exclusive or Shared state
		UNC_I_SNOOP_RESP__MASK__HSWEP_UNC_I_SNOOP_RESP__HIT_M = 0x800, // Hit in Modified state
		UNC_I_SNOOP_RESP__MASK__HSWEP_UNC_I_SNOOP_RESP__SNPCODE = 0x1000, // Snoop Code
		UNC_I_SNOOP_RESP__MASK__HSWEP_UNC_I_SNOOP_RESP__SNPDATA = 0x2000, // Snoop Data
		UNC_I_SNOOP_RESP__MASK__HSWEP_UNC_I_SNOOP_RESP__SNPINV = 0x4000, // Snoop Invalid
		UNC_I_MISC0 = 0x14, // Miscellaneous events
		UNC_I_MISC0__MASK__HSWEP_UNC_I_MISC0__FAST_REQ = 0x100, // Fastpath requests
		UNC_I_MISC0__MASK__HSWEP_UNC_I_MISC0__FAST_REJ = 0x200, // Fastpath rejects
		UNC_I_MISC0__MASK__HSWEP_UNC_I_MISC0__2ND_RD_INSERT = 0x400, // Cache insert of read transaction as secondary
		UNC_I_MISC0__MASK__HSWEP_UNC_I_MISC0__2ND_WR_INSERT = 0x800, // Cache insert of write transaction as secondary
		UNC_I_MISC0__MASK__HSWEP_UNC_I_MISC0__2ND_ATOMIC_INSERT = 0x1000, // Cache insert of atomic transaction as secondary
		UNC_I_MISC0__MASK__HSWEP_UNC_I_MISC0__FAST_XFER = 0x2000, // Fastpath trasnfers from primary to secondary
		UNC_I_MISC0__MASK__HSWEP_UNC_I_MISC0__PF_ACK_HINT = 0x4000, // Prefetch ack hints from primary to secondary
		UNC_I_MISC0__MASK__HSWEP_UNC_I_MISC0__PF_TIMEOUT = 0x8000, // Prefetch timeout
		UNC_I_COHERENT_OPS = 0x13, // Coherent operations
		UNC_I_COHERENT_OPS__MASK__HSWEP_UNC_I_COHERENT_OPS__PCIRDCUR = 0x100, // PCI read current
		UNC_I_COHERENT_OPS__MASK__HSWEP_UNC_I_COHERENT_OPS__CRD = 0x200, // CRD
		UNC_I_COHERENT_OPS__MASK__HSWEP_UNC_I_COHERENT_OPS__DRD = 0x400, // DRD
		UNC_I_COHERENT_OPS__MASK__HSWEP_UNC_I_COHERENT_OPS__RFO = 0x800, // RFO
		UNC_I_COHERENT_OPS__MASK__HSWEP_UNC_I_COHERENT_OPS__PCITOM = 0x1000, // DRITOM
		UNC_I_COHERENT_OPS__MASK__HSWEP_UNC_I_COHERENT_OPS__PCIDCAHINT = 0x2000, // PCIDCAHINT
		UNC_I_COHERENT_OPS__MASK__HSWEP_UNC_I_COHERENT_OPS__WBMTOI = 0x4000, // WBMTOI
		UNC_I_COHERENT_OPS__MASK__HSWEP_UNC_I_COHERENT_OPS__CFLUSH = 0x8000, // CFLUSH
		UNC_I_CACHE_TOTAL_OCCUPANCY = 0x12, // Total write cache occupancy
		UNC_I_CACHE_TOTAL_OCCUPANCY__MASK__HSWEP_UNC_I_CACHE_ACK_PENDING_OCCUPANCY__ANY = 0x100, // Any source
		UNC_I_CACHE_TOTAL_OCCUPANCY__MASK__HSWEP_UNC_I_CACHE_ACK_PENDING_OCCUPANCY__SOURCE = 0x200, // Track all requests from any source port
		UNC_I_RXR_AK_INSERTS = 0xa, // Egress cycles full
		UNC_I_RXR_BL_DRS_CYCLES_FULL = 0x4, // TBD
		UNC_I_RXR_BL_DRS_INSERTS = 0x1, // BL Ingress occupancy DRS
		UNC_I_RXR_BL_DRS_OCCUPANCY = 0x7, // TBD
		UNC_I_RXR_BL_NCB_CYCLES_FULL = 0x5, // TBD
		UNC_I_RXR_BL_NCB_INSERTS = 0x2, // BL Ingress occupancy NCB
		UNC_I_RXR_BL_NCB_OCCUPANCY = 0x8, // TBD
		UNC_I_RXR_BL_NCS_CYCLES_FULL = 0x6, // TBD
		UNC_I_RXR_BL_NCS_INSERTS = 0x3, // BL Ingress Occupancy NCS
		UNC_I_RXR_BL_NCS_OCCUPANCY = 0x9, // TBD
		UNC_I_TRANSACTIONS = 0x16, // Inbound transactions
		UNC_I_TRANSACTIONS__MASK__HSWEP_UNC_I_TRANSACTIONS__READS = 0x100, // Reads (not including prefetches)
		UNC_I_TRANSACTIONS__MASK__HSWEP_UNC_I_TRANSACTIONS__WRITES = 0x200, // Writes
		UNC_I_TRANSACTIONS__MASK__HSWEP_UNC_I_TRANSACTIONS__RD_PREF = 0x400, // Read prefetches
		UNC_I_TRANSACTIONS__MASK__HSWEP_UNC_I_TRANSACTIONS__WR_PREF = 0x800, // Write prefetches
		UNC_I_TRANSACTIONS__MASK__HSWEP_UNC_I_TRANSACTIONS__ATOMIC = 0x1000, // Atomic transactions
		UNC_I_TRANSACTIONS__MASK__HSWEP_UNC_I_TRANSACTIONS__OTHER = 0x2000, // Other kinds of transactions
		UNC_I_TRANSACTIONS__MASK__HSWEP_UNC_I_TRANSACTIONS__ORDERINGQ = 0x4000, // Track request coming from port designated in IRP OrderingQ filter
		UNC_I_MISC1 = 0x15, // Misc events
		UNC_I_MISC1__MASK__HSWEP_UNC_I_MISC1__SLOW_I = 0x100, // Slow transfer of I-state cacheline
		UNC_I_MISC1__MASK__HSWEP_UNC_I_MISC1__SLOW_S = 0x200, // Slow transfer of S-state cacheline
		UNC_I_MISC1__MASK__HSWEP_UNC_I_MISC1__SLOW_E = 0x400, // Slow transfer of e-state cacheline
		UNC_I_MISC1__MASK__HSWEP_UNC_I_MISC1__SLOW_M = 0x800, // Slow transfer of M-state cacheline
		UNC_I_MISC1__MASK__HSWEP_UNC_I_MISC1__LOST_FWD = 0x1000, // LOST forwards
		UNC_I_MISC1__MASK__HSWEP_UNC_I_MISC1__SEC_RCVD_INVLD = 0x2000, // Received Invalid
		UNC_I_MISC1__MASK__HSWEP_UNC_I_MISC1__SEC_RCVD_VLD = 0x4000, // Received Valid
		UNC_I_MISC1__MASK__HSWEP_UNC_I_MISC1__DATA_THROTTLE = 0x8000, // Data throttled
		UNC_I_TXR_AD_STALL_CREDIT_CYCLES = 0x18, // No AD Egress credit stalls
		UNC_I_TXR_BL_STALL_CREDIT_CYCLES = 0x19, // No BL Egress credit stalls
		UNC_I_TXR_DATA_INSERTS_NCB = 0xe, // Outbound read requests
		UNC_I_TXR_DATA_INSERTS_NCS = 0xf, // Outbound read requests
		UNC_I_TXR_REQUEST_OCCUPANCY = 0xd, // Outbound request queue occupancy
		
	};
};

namespace hswep_unc_i = optkit::intel::hswep_unc_i;