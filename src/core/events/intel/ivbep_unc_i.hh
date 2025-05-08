#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::ivbep_unc_i{
	enum ivbep_unc_i : uint64_t {
		UNC_I_CLOCKTICKS = 0x0, // Number of uclks in domain
		UNC_I_ADDRESS_MATCH = 0x17, // Address match conflict count
		UNC_I_ADDRESS_MATCH__MASK__IVBEP_UNC_I_ADDRESS_MATCH__STALL_COUNT = 0x100, // Number of time when it is not possible to merge two conflicting requests
		UNC_I_ADDRESS_MATCH__MASK__IVBEP_UNC_I_ADDRESS_MATCH__MERGE_COUNT = 0x200, // Number of times when two requests to the same address from the same source are received back to back
		UNC_I_CACHE_ACK_PENDING_OCCUPANCY = 0x14, // Write ACK pending occupancy
		UNC_I_CACHE_ACK_PENDING_OCCUPANCY__MASK__IVBEP_UNC_I_CACHE_ACK_PENDING_OCCUPANCY__ANY = 0x100, // Any source
		UNC_I_CACHE_ACK_PENDING_OCCUPANCY__MASK__IVBEP_UNC_I_CACHE_ACK_PENDING_OCCUPANCY__SOURCE = 0x200, // Track all requests from any source port
		UNC_I_CACHE_OWN_OCCUPANCY = 0x13, // Outstanding write ownership occupancy
		UNC_I_CACHE_OWN_OCCUPANCY__MASK__IVBEP_UNC_I_CACHE_ACK_PENDING_OCCUPANCY__ANY = 0x100, // Any source
		UNC_I_CACHE_OWN_OCCUPANCY__MASK__IVBEP_UNC_I_CACHE_ACK_PENDING_OCCUPANCY__SOURCE = 0x200, // Track all requests from any source port
		UNC_I_CACHE_READ_OCCUPANCY = 0x10, // Outstanding read occupancy
		UNC_I_CACHE_READ_OCCUPANCY__MASK__IVBEP_UNC_I_CACHE_ACK_PENDING_OCCUPANCY__ANY = 0x100, // Any source
		UNC_I_CACHE_READ_OCCUPANCY__MASK__IVBEP_UNC_I_CACHE_ACK_PENDING_OCCUPANCY__SOURCE = 0x200, // Track all requests from any source port
		UNC_I_CACHE_TOTAL_OCCUPANCY = 0x12, // Total write cache occupancy
		UNC_I_CACHE_TOTAL_OCCUPANCY__MASK__IVBEP_UNC_I_CACHE_ACK_PENDING_OCCUPANCY__ANY = 0x100, // Any source
		UNC_I_CACHE_TOTAL_OCCUPANCY__MASK__IVBEP_UNC_I_CACHE_ACK_PENDING_OCCUPANCY__SOURCE = 0x200, // Track all requests from any source port
		UNC_I_CACHE_WRITE_OCCUPANCY = 0x11, // Outstanding write occupancy
		UNC_I_CACHE_WRITE_OCCUPANCY__MASK__IVBEP_UNC_I_CACHE_ACK_PENDING_OCCUPANCY__ANY = 0x100, // Any source
		UNC_I_CACHE_WRITE_OCCUPANCY__MASK__IVBEP_UNC_I_CACHE_ACK_PENDING_OCCUPANCY__SOURCE = 0x200, // Track all requests from any source port
		UNC_I_RXR_AK_CYCLES_FULL = 0xb, // TBD
		UNC_I_RXR_AK_INSERTS = 0xa, // Egress cycles full
		UNC_I_RXR_AK_OCCUPANCY = 0x0c, // TBD
		UNC_I_RXR_BL_DRS_CYCLES_FULL = 0x4, // TBD
		UNC_I_RXR_BL_DRS_INSERTS = 0x1, // BL Ingress occupancy DRS
		UNC_I_RXR_BL_DRS_OCCUPANCY = 0x7, // TBD
		UNC_I_RXR_BL_NCB_CYCLES_FULL = 0x5, // TBD
		UNC_I_RXR_BL_NCB_INSERTS = 0x2, // BL Ingress occupancy NCB
		UNC_I_RXR_BL_NCB_OCCUPANCY = 0x8, // TBD
		UNC_I_RXR_BL_NCS_CYCLES_FULL = 0x6, // TBD
		UNC_I_RXR_BL_NCS_INSERTS = 0x3, // BL Ingress Occupancy NCS
		UNC_I_RXR_BL_NCS_OCCUPANCY = 0x9, // TBD
		UNC_I_TICKLES = 0x16, // Tickle count
		UNC_I_TICKLES__MASK__IVBEP_UNC_I_TICKLES__LOST_OWNERSHIP = 0x100, // Number of request that lost ownership as a result of a tickle
		UNC_I_TICKLES__MASK__IVBEP_UNC_I_TICKLES__TOP_OF_QUEUE = 0x200, // Number of cases when a tickle was received but the request was at the head of the queue in the switch. In this case data is returned rather than releasing ownership
		UNC_I_TRANSACTIONS = 0x15, // Inbound transaction count
		UNC_I_TRANSACTIONS__MASK__IVBEP_UNC_I_TRANSACTIONS__READS = 0x100, // Number of read requests (not including read prefetches)
		UNC_I_TRANSACTIONS__MASK__IVBEP_UNC_I_TRANSACTIONS__WRITES = 0x200, // Number of write requests. Each write should have a prefetch
		UNC_I_TRANSACTIONS__MASK__IVBEP_UNC_I_TRANSACTIONS__RD_PREFETCHES = 0x400, // Number of read prefetches
		UNC_I_TXR_AD_STALL_CREDIT_CYCLES = 0x18, // No AD Egress credit stalls
		UNC_I_TXR_BL_STALL_CREDIT_CYCLES = 0x19, // No BL Egress credit stalls
		UNC_I_TXR_DATA_INSERTS_NCB = 0xe, // Outbound read requests
		UNC_I_TXR_DATA_INSERTS_NCS = 0xf, // Outbound read requests
		UNC_I_TXR_REQUEST_OCCUPANCY = 0xd, // Outbound request queue occupancy
		UNC_I_WRITE_ORDERING_STALL_CYCLES = 0x1a, // Write ordering stalls
		
	};
};

namespace ivbep_unc_i = optkit::intel::ivbep_unc_i;