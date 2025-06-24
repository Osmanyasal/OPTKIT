#pragma once
#include <cstdint>
#define INTEL_X86_EDGE_BIT	18
#define INTEL_X86_ANY_BIT	21
#define INTEL_X86_INV_BIT	23
#define INTEL_X86_CMASK_BIT 24
#define INTEL_X86_MOD_EDGE	(1 << INTEL_X86_EDGE_BIT)
#define INTEL_X86_MOD_ANY	(1 << INTEL_X86_ANY_BIT)
#define INTEL_X86_MOD_INV	(1 << INTEL_X86_INV_BIT)
namespace optkit::intel::snbep_unc_c{
	enum snbep_unc_c : uint64_t {
		UNC_C_CLOCKTICKS = 0x00, // C-box Uncore clockticks
		UNC_C_COUNTER0_OCCUPANCY = 0x1f, // Counter 0 occupancy. Counts the occupancy related information by filtering CB0 occupancy count captured in counter 0.
		UNC_C_ISMQ_DRD_MISS_OCC = 0x21, // TBD
		UNC_C_LLC_LOOKUP = 0x34, // Cache lookups. Counts number of times the LLC is accessed from L2 for code
		UNC_C_LLC_LOOKUP__MASK__SNBEP_UNC_C_LLC_LOOKUP__ANY = 0x1f00ull, // Any request
		UNC_C_LLC_LOOKUP__MASK__SNBEP_UNC_C_LLC_LOOKUP__DATA_READ = 0x300ull, // Data read requests
		UNC_C_LLC_LOOKUP__MASK__SNBEP_UNC_C_LLC_LOOKUP__WRITE = 0x500ull, // Write requests. Includes all write transactions (cached
		UNC_C_LLC_LOOKUP__MASK__SNBEP_UNC_C_LLC_LOOKUP__REMOTE_SNOOP = 0x900ull, // External snoop request
		UNC_C_LLC_LOOKUP__MASK__SNBEP_UNC_C_LLC_LOOKUP__NID = 0x4100ull, // X modifier)
		UNC_C_LLC_VICTIMS = 0x37, // Lines victimized
		UNC_C_LLC_VICTIMS__MASK__SNBEP_UNC_C_LLC_VICTIMS__M_STATE = 0x100ull, // Lines in M state
		UNC_C_LLC_VICTIMS__MASK__SNBEP_UNC_C_LLC_VICTIMS__E_STATE = 0x200ull, // Lines in E state
		UNC_C_LLC_VICTIMS__MASK__SNBEP_UNC_C_LLC_VICTIMS__S_STATE = 0x400ull, // Lines in S state
		UNC_C_LLC_VICTIMS__MASK__SNBEP_UNC_C_LLC_VICTIMS__MISS = 0x800ull, // TBD
		UNC_C_LLC_VICTIMS__MASK__SNBEP_UNC_C_LLC_VICTIMS__NID = 0x4000ull, // X modifier)
		UNC_C_MISC = 0x39, // Miscellaneous C-Box events
		UNC_C_MISC__MASK__SNBEP_UNC_C_MISC__RSPI_WAS_FSE = 0x100ull, // Silent snoop eviction
		UNC_C_MISC__MASK__SNBEP_UNC_C_MISC__WC_ALIASING = 0x200ull, // Write combining aliasing
		UNC_C_MISC__MASK__SNBEP_UNC_C_MISC__STARTED = 0x400ull, // TBD
		UNC_C_MISC__MASK__SNBEP_UNC_C_MISC__RFO_HIT_S = 0x800ull, // RFO hits in S state
		UNC_C_RING_AD_USED = 0x1b, // Address ring in use. Counts number of cycles ring is being used at this ring stop
		UNC_C_RING_AD_USED__MASK__SNBEP_UNC_C_RING_AD_USED__UP_EVEN = 0x100ull, // Up and Even ring polarity filter
		UNC_C_RING_AD_USED__MASK__SNBEP_UNC_C_RING_AD_USED__UP_ODD = 0x200ull, // Up and odd ring polarity filter
		UNC_C_RING_AD_USED__MASK__SNBEP_UNC_C_RING_AD_USED__DOWN_EVEN = 0x400ull, // Down and even ring polarity filter
		UNC_C_RING_AD_USED__MASK__SNBEP_UNC_C_RING_AD_USED__DOWN_ODD = 0x800ull, // Down and odd ring polarity filter
		UNC_C_RING_AK_USED = 0x1c, // Acknowledgment ring in use. Counts number of cycles ring is being used at this ring stop
		UNC_C_RING_AK_USED__MASK__SNBEP_UNC_C_RING_AD_USED__UP_EVEN = 0x100ull, // Up and Even ring polarity filter
		UNC_C_RING_AK_USED__MASK__SNBEP_UNC_C_RING_AD_USED__UP_ODD = 0x200ull, // Up and odd ring polarity filter
		UNC_C_RING_AK_USED__MASK__SNBEP_UNC_C_RING_AD_USED__DOWN_EVEN = 0x400ull, // Down and even ring polarity filter
		UNC_C_RING_AK_USED__MASK__SNBEP_UNC_C_RING_AD_USED__DOWN_ODD = 0x800ull, // Down and odd ring polarity filter
		UNC_C_RING_BL_USED = 0x1d, // Bus or Data ring in use. Counts number of cycles ring is being used at this ring stop
		UNC_C_RING_BL_USED__MASK__SNBEP_UNC_C_RING_AD_USED__UP_EVEN = 0x100ull, // Up and Even ring polarity filter
		UNC_C_RING_BL_USED__MASK__SNBEP_UNC_C_RING_AD_USED__UP_ODD = 0x200ull, // Up and odd ring polarity filter
		UNC_C_RING_BL_USED__MASK__SNBEP_UNC_C_RING_AD_USED__DOWN_EVEN = 0x400ull, // Down and even ring polarity filter
		UNC_C_RING_BL_USED__MASK__SNBEP_UNC_C_RING_AD_USED__DOWN_ODD = 0x800ull, // Down and odd ring polarity filter
		UNC_C_RING_BOUNCES = 0x05, // Number of LLC responses that bounced in the ring
		UNC_C_RING_BOUNCES__MASK__SNBEP_UNC_C_RING_BOUNCES__AK_CORE = 0x200ull, // Acknowledgment to core
		UNC_C_RING_BOUNCES__MASK__SNBEP_UNC_C_RING_BOUNCES__BL_CORE = 0x400ull, // Data response to core
		UNC_C_RING_BOUNCES__MASK__SNBEP_UNC_C_RING_BOUNCES__IV_CORE = 0x800ull, // Snoops of processor cache
		UNC_C_RING_IV_USED = 0x1e, // Invalidate ring in use. Counts number of cycles ring is being used at this ring stop
		UNC_C_RING_IV_USED__MASK__SNBEP_UNC_C_RING_IV_USED__ANY = 0xf00ull, // Any filter
		UNC_C_RING_SRC_THRTL = 0x07, // TDB
		UNC_C_RXR_EXT_STARVED = 0x12, // Ingress arbiter blocking cycles
		UNC_C_RXR_EXT_STARVED__MASK__SNBEP_UNC_C_RXR_EXT_STARVED__IRQ = 0x100ull, // Irq externally starved
		UNC_C_RXR_EXT_STARVED__MASK__SNBEP_UNC_C_RXR_EXT_STARVED__IPQ = 0x200ull, // IPQ externally starved
		UNC_C_RXR_EXT_STARVED__MASK__SNBEP_UNC_C_RXR_EXT_STARVED__ISMQ = 0x400ull, // ISMQ externally starved
		UNC_C_RXR_EXT_STARVED__MASK__SNBEP_UNC_C_RXR_EXT_STARVED__ISMQ_BIDS = 0x800ull, // Number of time the ISMQ bids
		UNC_C_RXR_INSERTS = 0x13, // Ingress Allocations
		UNC_C_RXR_INSERTS__MASK__SNBEP_UNC_C_RXR_INSERTS__IPQ = 0x400ull, // IPQ
		UNC_C_RXR_INSERTS__MASK__SNBEP_UNC_C_RXR_INSERTS__IRQ = 0x100ull, // IRQ
		UNC_C_RXR_INSERTS__MASK__SNBEP_UNC_C_RXR_INSERTS__IRQ_REJECTED = 0x200ull, // IRQ rejected
		UNC_C_RXR_INSERTS__MASK__SNBEP_UNC_C_RXR_INSERTS__VFIFO = 0x1000ull, // Counts the number of allocated into the IRQ ordering FIFO
		UNC_C_RXR_IPQ_RETRY = 0x31, // Probe Queue Retries
		UNC_C_RXR_IPQ_RETRY__MASK__SNBEP_UNC_C_RXR_IPQ_RETRY__ADDR_CONFLICT = 0x400ull, // Address conflict
		UNC_C_RXR_IPQ_RETRY__MASK__SNBEP_UNC_C_RXR_IPQ_RETRY__ANY = 0x100ull, // Any Reject
		UNC_C_RXR_IPQ_RETRY__MASK__SNBEP_UNC_C_RXR_IPQ_RETRY__FULL = 0x200ull, // No Egress credits
		UNC_C_RXR_IPQ_RETRY__MASK__SNBEP_UNC_C_RXR_IPQ_RETRY__QPI_CREDITS = 0x1000ull, // No QPI credits
		UNC_C_RXR_IRQ_RETRY = 0x32, // Ingress Request Queue Rejects
		UNC_C_RXR_IRQ_RETRY__MASK__SNBEP_UNC_C_RXR_IRQ_RETRY__ADDR_CONFLICT = 0x400ull, // Address conflict
		UNC_C_RXR_IRQ_RETRY__MASK__SNBEP_UNC_C_RXR_IRQ_RETRY__ANY = 0x100ull, // Any reject
		UNC_C_RXR_IRQ_RETRY__MASK__SNBEP_UNC_C_RXR_IRQ_RETRY__FULL = 0x200ull, // No Egress credits
		UNC_C_RXR_IRQ_RETRY__MASK__SNBEP_UNC_C_RXR_IRQ_RETRY__QPI_CREDITS = 0x1000ull, // No QPI credits
		UNC_C_RXR_IRQ_RETRY__MASK__SNBEP_UNC_C_RXR_IRQ_RETRY__RTID = 0x800ull, // No RTIDs
		UNC_C_RXR_ISMQ_RETRY = 0x33, // ISMQ Retries
		UNC_C_RXR_ISMQ_RETRY__MASK__SNBEP_UNC_C_RXR_ISMQ_RETRY__ANY = 0x100ull, // Any reject
		UNC_C_RXR_ISMQ_RETRY__MASK__SNBEP_UNC_C_RXR_ISMQ_RETRY__FULL = 0x200ull, // No Egress credits
		UNC_C_RXR_ISMQ_RETRY__MASK__SNBEP_UNC_C_RXR_ISMQ_RETRY__IIO_CREDITS = 0x2000ull, // No IIO credits
		UNC_C_RXR_ISMQ_RETRY__MASK__SNBEP_UNC_C_RXR_ISMQ_RETRY__QPI_CREDITS = 0x1000ull, // NO QPI credits
		UNC_C_RXR_ISMQ_RETRY__MASK__SNBEP_UNC_C_RXR_ISMQ_RETRY__RTID = 0x800ull, // No RTIDs
		UNC_C_RXR_OCCUPANCY = 0x11, // Ingress Occupancy
		UNC_C_RXR_OCCUPANCY__MASK__SNBEP_UNC_C_RXR_INSERTS__IPQ = 0x400ull, // IPQ
		UNC_C_RXR_OCCUPANCY__MASK__SNBEP_UNC_C_RXR_INSERTS__IRQ = 0x100ull, // IRQ
		UNC_C_RXR_OCCUPANCY__MASK__SNBEP_UNC_C_RXR_INSERTS__IRQ_REJECTED = 0x200ull, // IRQ rejected
		UNC_C_RXR_OCCUPANCY__MASK__SNBEP_UNC_C_RXR_INSERTS__VFIFO = 0x1000ull, // Counts the number of allocated into the IRQ ordering FIFO
		UNC_C_TOR_INSERTS = 0x35, // TOR Inserts
		UNC_C_TOR_INSERTS__MASK__SNBEP_UNC_C_TOR_INSERTS__EVICTION = 0x400ull, // Number of Evictions transactions inserted into TOR
		UNC_C_TOR_INSERTS__MASK__SNBEP_UNC_C_TOR_INSERTS__MISS_ALL = 0xa00ull, // Number of miss requests inserted into the TOR
		UNC_C_TOR_INSERTS__MASK__SNBEP_UNC_C_TOR_INSERTS__MISS_OPCODE = 0x300ull, // Number of miss transactions inserted into the TOR that match an opcode (must provide opc_* umask)
		UNC_C_TOR_INSERTS__MASK__SNBEP_UNC_C_TOR_INSERTS__NID_ALL = 0x4800ull, // X modifier)
		UNC_C_TOR_INSERTS__MASK__SNBEP_UNC_C_TOR_INSERTS__NID_EVICTION = 0x4400ull, // X modifier)
		UNC_C_TOR_INSERTS__MASK__SNBEP_UNC_C_TOR_INSERTS__NID_MISS_ALL = 0x4a00ull, // X modifier)
		UNC_C_TOR_INSERTS__MASK__SNBEP_UNC_C_TOR_INSERTS__NID_MISS_OPCODE = 0x4300ull, // X modifier)
		UNC_C_TOR_INSERTS__MASK__SNBEP_UNC_C_TOR_INSERTS__NID_OPCODE = 0x4100ull, // X modifier)
		UNC_C_TOR_INSERTS__MASK__SNBEP_UNC_C_TOR_INSERTS__NID_WB = 0x5000ull, // X modifier)
		UNC_C_TOR_INSERTS__MASK__SNBEP_UNC_C_TOR_INSERTS__OPCODE = 0x100ull, // Number of transactions inserted into the TOR that match an opcode (must provide opc_* umask)
		UNC_C_TOR_INSERTS__MASK__SNBEP_UNC_C_TOR_INSERTS__WB = 0x1000ull, // Number of write transactions inserted into the TOR
		UNC_C_TOR_OCCUPANCY = 0x36, // TOR Occupancy
		UNC_C_TOR_OCCUPANCY__MASK__SNBEP_UNC_C_TOR_OCCUPANCY__ALL = 0x800ull, // All valid TOR entries
		UNC_C_TOR_OCCUPANCY__MASK__SNBEP_UNC_C_TOR_OCCUPANCY__EVICTION = 0x400ull, // Number of outstanding eviction transactions in the TOR
		UNC_C_TOR_OCCUPANCY__MASK__SNBEP_UNC_C_TOR_OCCUPANCY__MISS_ALL = 0xa00ull, // Number of outstanding miss requests in the TOR
		UNC_C_TOR_OCCUPANCY__MASK__SNBEP_UNC_C_TOR_OCCUPANCY__MISS_OPCODE = 0x300ull, // Number of TOR entries that match a NID and an opcode (must provide opc_* umask)
		UNC_C_TOR_OCCUPANCY__MASK__SNBEP_UNC_C_TOR_OCCUPANCY__NID_ALL = 0x4800ull, // X modifier)
		UNC_C_TOR_OCCUPANCY__MASK__SNBEP_UNC_C_TOR_OCCUPANCY__NID_EVICTION = 0x4400ull, // X modifier)
		UNC_C_TOR_OCCUPANCY__MASK__SNBEP_UNC_C_TOR_OCCUPANCY__NID_MISS_ALL = 0x4a00ull, // X modifier)
		UNC_C_TOR_OCCUPANCY__MASK__SNBEP_UNC_C_TOR_OCCUPANCY__NID_MISS_OPCODE = 0x4300ull, // X modifier and opc_* umask)
		UNC_C_TOR_OCCUPANCY__MASK__SNBEP_UNC_C_TOR_OCCUPANCY__NID_OPCODE = 0x4100ull, // X modifier and opc_* umask)
		UNC_C_TOR_OCCUPANCY__MASK__SNBEP_UNC_C_TOR_OCCUPANCY__OPCODE = 0x100ull, // Number of TOR entries that match an opcode (must provide opc_* umask)
		UNC_C_TXR_ADS_USED = 0x04, // Egress events
		UNC_C_TXR_INSERTS = 0x02, // Egress allocations
		UNC_C_TXR_INSERTS__MASK__SNBEP_UNC_C_TXR_INSERTS__AD_CACHE = 0x100ull, // Counts the number of ring transactions from Cachebo to AD ring
		UNC_C_TXR_INSERTS__MASK__SNBEP_UNC_C_TXR_INSERTS__AK_CACHE = 0x200ull, // Counts the number of ring transactions from Cachebo to AK ring
		UNC_C_TXR_INSERTS__MASK__SNBEP_UNC_C_TXR_INSERTS__BL_CACHE = 0x400ull, // Counts the number of ring transactions from Cachebo to BL ring
		UNC_C_TXR_INSERTS__MASK__SNBEP_UNC_C_TXR_INSERTS__IV_CACHE = 0x800ull, // Counts the number of ring transactions from Cachebo to IV ring
		UNC_C_TXR_INSERTS__MASK__SNBEP_UNC_C_TXR_INSERTS__AD_CORE = 0x1000ull, // Counts the number of ring transactions from Corebo to AD ring
		UNC_C_TXR_INSERTS__MASK__SNBEP_UNC_C_TXR_INSERTS__AK_CORE = 0x2000ull, // Counts the number of ring transactions from Corebo to AK ring
		UNC_C_TXR_INSERTS__MASK__SNBEP_UNC_C_TXR_INSERTS__BL_CORE = 0x4000ull, // Counts the number of ring transactions from Corebo to BL ring
		
	};
};

namespace snbep_unc_c = optkit::intel::snbep_unc_c;

#undef INTEL_X86_EDGE_BIT
#undef INTEL_X86_ANY_BIT
#undef INTEL_X86_INV_BIT
#undef INTEL_X86_CMASK_BIT
#undef INTEL_X86_MOD_EDGE
#undef INTEL_X86_MOD_ANY
#undef INTEL_X86_MOD_INV
