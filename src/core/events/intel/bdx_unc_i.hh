#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::bdx_unc_i{
	enum bdx_unc_i : uint64_t {
		UNC_I_CACHE_TOTAL_OCCUPANCY = 0x12, // Accumulates the number of reads and writes that are outstanding in the uncore in each cycle.  This is effectively the sum of the READ_OCCUPANCY and WRITE_OCCUPANCY events.
		UNC_I_CACHE_TOTAL_OCCUPANCY__MASK__BDX_UNC_I_CACHE_TOTAL_OCCUPANCY__ANY = 0x100, // Total Write Cache Occupancy -- Any Source
		UNC_I_CACHE_TOTAL_OCCUPANCY__MASK__BDX_UNC_I_CACHE_TOTAL_OCCUPANCY__SOURCE = 0x200, // Total Write Cache Occupancy -- Select Source
		UNC_I_CLOCKTICKS = 0x0, // Number of clocks in the IRP.
		UNC_I_COHERENT_OPS = 0x13, // Counts the number of coherency related operations servied by the IRP
		UNC_I_COHERENT_OPS__MASK__BDX_UNC_I_COHERENT_OPS__CLFLUSH = 0x8000, // Coherent Ops -- CLFlush
		UNC_I_COHERENT_OPS__MASK__BDX_UNC_I_COHERENT_OPS__CRD = 0x200, // Coherent Ops -- CRd
		UNC_I_COHERENT_OPS__MASK__BDX_UNC_I_COHERENT_OPS__DRD = 0x400, // Coherent Ops -- DRd
		UNC_I_COHERENT_OPS__MASK__BDX_UNC_I_COHERENT_OPS__PCIDCAHINT = 0x2000, // Coherent Ops -- PCIDCAHin5t
		UNC_I_COHERENT_OPS__MASK__BDX_UNC_I_COHERENT_OPS__PCIRDCUR = 0x100, // Coherent Ops -- PCIRdCur
		UNC_I_COHERENT_OPS__MASK__BDX_UNC_I_COHERENT_OPS__PCITOM = 0x1000, // Coherent Ops -- PCIItoM
		UNC_I_COHERENT_OPS__MASK__BDX_UNC_I_COHERENT_OPS__RFO = 0x800, // Coherent Ops -- RFO
		UNC_I_COHERENT_OPS__MASK__BDX_UNC_I_COHERENT_OPS__WBMTOI = 0x4000, // Coherent Ops -- WbMtoI
		UNC_I_MISC0 = 0x14, // TBD
		UNC_I_MISC0__MASK__BDX_UNC_I_MISC0__2ND_ATOMIC_INSERT = 0x1000, // Misc Events - Set 0 -- Cache Inserts of Atomic Transactions as Secondary
		UNC_I_MISC0__MASK__BDX_UNC_I_MISC0__2ND_RD_INSERT = 0x400, // Misc Events - Set 0 -- Cache Inserts of Read Transactions as Secondary
		UNC_I_MISC0__MASK__BDX_UNC_I_MISC0__2ND_WR_INSERT = 0x800, // Misc Events - Set 0 -- Cache Inserts of Write Transactions as Secondary
		UNC_I_MISC0__MASK__BDX_UNC_I_MISC0__FAST_REJ = 0x200, // Misc Events - Set 0 -- Fastpath Rejects
		UNC_I_MISC0__MASK__BDX_UNC_I_MISC0__FAST_REQ = 0x100, // Misc Events - Set 0 -- Fastpath Requests
		UNC_I_MISC0__MASK__BDX_UNC_I_MISC0__FAST_XFER = 0x2000, // Misc Events - Set 0 -- Fastpath Transfers From Primary to Secondary
		UNC_I_MISC0__MASK__BDX_UNC_I_MISC0__PF_ACK_HINT = 0x4000, // Misc Events - Set 0 -- Prefetch Ack Hints From Primary to Secondary
		UNC_I_MISC0__MASK__BDX_UNC_I_MISC0__PF_TIMEOUT = 0x8000, // Misc Events - Set 0 -- Prefetch TimeOut
		UNC_I_MISC1 = 0x15, // TBD
		UNC_I_MISC1__MASK__BDX_UNC_I_MISC1__DATA_THROTTLE = 0x8000, // Misc Events - Set 1 -- Data Throttled
		UNC_I_MISC1__MASK__BDX_UNC_I_MISC1__LOST_FWD = 0x1000, // Misc Events - Set 1 --
		UNC_I_MISC1__MASK__BDX_UNC_I_MISC1__SEC_RCVD_INVLD = 0x2000, // Misc Events - Set 1 -- Received Invalid
		UNC_I_MISC1__MASK__BDX_UNC_I_MISC1__SEC_RCVD_VLD = 0x4000, // Misc Events - Set 1 -- Received Valid
		UNC_I_MISC1__MASK__BDX_UNC_I_MISC1__SLOW_I = 0x100, // Misc Events - Set 1 -- Slow Transfer of I Line
		UNC_I_MISC1__MASK__BDX_UNC_I_MISC1__SLOW_S = 0x200, // Misc Events - Set 1 -- Slow Transfer of S Line
		UNC_I_MISC1__MASK__BDX_UNC_I_MISC1__SLOW_E = 0x400, // Misc Events - Set 1 -- Slow Transfer of E Line
		UNC_I_MISC1__MASK__BDX_UNC_I_MISC1__SLOW_M = 0x800, // Misc Events - Set 1 -- Slow Transfer of M Line
		UNC_I_RXR_AK_INSERTS = 0xa, // Counts the number of allocations into the AK Ingress.  This queue is where the IRP receives responses from R2PCIe (the ring).
		UNC_I_RXR_BL_DRS_CYCLES_FULL = 0x4, // Counts the number of cycles when the BL Ingress is full.  This queue is where the IRP receives data from R2PCIe (the ring).  It is used for data returns from read requets as well as outbound MMIO writes.
		UNC_I_RXR_BL_DRS_INSERTS = 0x1, // Counts the number of allocations into the BL Ingress.  This queue is where the IRP receives data from R2PCIe (the ring).  It is used for data returns from read requets as well as outbound MMIO writes.
		UNC_I_RXR_BL_DRS_OCCUPANCY = 0x7, // Accumulates the occupancy of the BL Ingress in each cycles.  This queue is where the IRP receives data from R2PCIe (the ring).  It is used for data returns from read requets as well as outbound MMIO writes.
		UNC_I_RXR_BL_NCB_CYCLES_FULL = 0x5, // Counts the number of cycles when the BL Ingress is full.  This queue is where the IRP receives data from R2PCIe (the ring).  It is used for data returns from read requets as well as outbound MMIO writes.
		UNC_I_RXR_BL_NCB_INSERTS = 0x2, // Counts the number of allocations into the BL Ingress.  This queue is where the IRP receives data from R2PCIe (the ring).  It is used for data returns from read requets as well as outbound MMIO writes.
		UNC_I_RXR_BL_NCB_OCCUPANCY = 0x8, // Accumulates the occupancy of the BL Ingress in each cycles.  This queue is where the IRP receives data from R2PCIe (the ring).  It is used for data returns from read requets as well as outbound MMIO writes.
		UNC_I_RXR_BL_NCS_CYCLES_FULL = 0x6, // Counts the number of cycles when the BL Ingress is full.  This queue is where the IRP receives data from R2PCIe (the ring).  It is used for data returns from read requets as well as outbound MMIO writes.
		UNC_I_RXR_BL_NCS_INSERTS = 0x3, // Counts the number of allocations into the BL Ingress.  This queue is where the IRP receives data from R2PCIe (the ring).  It is used for data returns from read requets as well as outbound MMIO writes.
		UNC_I_RXR_BL_NCS_OCCUPANCY = 0x9, // Accumulates the occupancy of the BL Ingress in each cycles.  This queue is where the IRP receives data from R2PCIe (the ring).  It is used for data returns from read requets as well as outbound MMIO writes.
		UNC_I_SNOOP_RESP = 0x17, // TBD
		UNC_I_SNOOP_RESP__MASK__BDX_UNC_I_SNOOP_RESP__HIT_ES = 0x400, // Snoop Responses -- Hit E or S
		UNC_I_SNOOP_RESP__MASK__BDX_UNC_I_SNOOP_RESP__HIT_I = 0x200, // Snoop Responses -- Hit I
		UNC_I_SNOOP_RESP__MASK__BDX_UNC_I_SNOOP_RESP__HIT_M = 0x800, // Snoop Responses -- Hit M
		UNC_I_SNOOP_RESP__MASK__BDX_UNC_I_SNOOP_RESP__MISS = 0x100, // Snoop Responses -- Miss
		UNC_I_SNOOP_RESP__MASK__BDX_UNC_I_SNOOP_RESP__SNPCODE = 0x1000, // Snoop Responses -- SnpCode
		UNC_I_SNOOP_RESP__MASK__BDX_UNC_I_SNOOP_RESP__SNPDATA = 0x2000, // Snoop Responses -- SnpData
		UNC_I_SNOOP_RESP__MASK__BDX_UNC_I_SNOOP_RESP__SNPINV = 0x4000, // Snoop Responses -- SnpInv
		UNC_I_TRANSACTIONS = 0x16, // Counts the number of Inbound transactions from the IRP to the Uncore.  This can be filtered based on request type in addition to the source queue.  Note the special filtering equation.  We do OR-reduction on the request type.  If the SOURCE bit is set
		UNC_I_TRANSACTIONS__MASK__BDX_UNC_I_TRANSACTIONS__ATOMIC = 0x1000, // Inbound Transaction Count -- Atomic
		UNC_I_TRANSACTIONS__MASK__BDX_UNC_I_TRANSACTIONS__ORDERINGQ = 0x4000, // Inbound Transaction Count -- Select Source via IRP orderingQ register
		UNC_I_TRANSACTIONS__MASK__BDX_UNC_I_TRANSACTIONS__OTHER = 0x2000, // Inbound Transaction Count -- Other
		UNC_I_TRANSACTIONS__MASK__BDX_UNC_I_TRANSACTIONS__RD_PREF = 0x400, // Inbound Transaction Count -- Read Prefetches
		UNC_I_TRANSACTIONS__MASK__BDX_UNC_I_TRANSACTIONS__READS = 0x100, // Inbound Transaction Count -- Reads
		UNC_I_TRANSACTIONS__MASK__BDX_UNC_I_TRANSACTIONS__WRITES = 0x200, // Inbound Transaction Count -- Writes
		UNC_I_TRANSACTIONS__MASK__BDX_UNC_I_TRANSACTIONS__WR_PREF = 0x800, // Inbound Transaction Count -- Write Prefetches
		UNC_I_TXR_AD_STALL_CREDIT_CYCLES = 0x18, // Counts the number times when it is not possible to issue a request to the R2PCIe because there are no AD Egress Credits available.
		UNC_I_TXR_BL_STALL_CREDIT_CYCLES = 0x19, // Counts the number times when it is not possible to issue data to the R2PCIe because there are no BL Egress Credits available.
		UNC_I_TXR_DATA_INSERTS_NCB = 0xe, // Counts the number of requests issued to the switch (towards the devices).
		UNC_I_TXR_DATA_INSERTS_NCS = 0xf, // Counts the number of requests issued to the switch (towards the devices).
		UNC_I_TXR_REQUEST_OCCUPANCY = 0xd, // Accumultes the number of outstanding outbound requests from the IRP to the switch (towards the devices).  This can be used in conjuection with the allocations event in order to calculate average latency of outbound requests.
		
	};
};

namespace bdx_unc_i = optkit::intel::bdx_unc_i;