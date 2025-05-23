#pragma once
#include <cstdint>
#define INTEL_X86_EDGE_BIT	18
#define INTEL_X86_ANY_BIT	21
#define INTEL_X86_INV_BIT	23
#define INTEL_X86_CMASK_BIT 24
#define INTEL_X86_MOD_EDGE	(1 << INTEL_X86_EDGE_BIT)
#define INTEL_X86_MOD_ANY	(1 << INTEL_X86_ANY_BIT)
#define INTEL_X86_MOD_INV	(1 << INTEL_X86_INV_BIT)
namespace optkit::intel::icx_unc_ubox{
	enum icx_unc_ubox : uint64_t {
		UNC_U_CLOCKTICKS = 0x0000, // Clockticks in the UBOX using a dedicated 48-bit Fixed Counter
		UNC_U_EVENT_MSG = 0x0042, // Message Received
		UNC_U_EVENT_MSG__MASK__ICX_UNC_U_EVENT_MSG__DOORBELL_RCVD = 0x0800ull, // Doorbell (experimental)
		UNC_U_EVENT_MSG__MASK__ICX_UNC_U_EVENT_MSG__INT_PRIO = 0x1000ull, // Interrupt (experimental)
		UNC_U_EVENT_MSG__MASK__ICX_UNC_U_EVENT_MSG__IPI_RCVD = 0x0400ull, // IPI (experimental)
		UNC_U_EVENT_MSG__MASK__ICX_UNC_U_EVENT_MSG__MSI_RCVD = 0x0200ull, // MSI (experimental)
		UNC_U_EVENT_MSG__MASK__ICX_UNC_U_EVENT_MSG__VLW_RCVD = 0x0100ull, // VLW (experimental)
		UNC_U_LOCK_CYCLES = 0x0044, // IDI Lock/SplitLock Cycles (experimental)
		UNC_U_M2U_MISC1 = 0x004d, // TBD
		UNC_U_M2U_MISC1__MASK__ICX_UNC_U_M2U_MISC1__RxC_CYCLES_NE_CBO_NCB = 0x0100ull, // TBD (experimental)
		UNC_U_M2U_MISC1__MASK__ICX_UNC_U_M2U_MISC1__RxC_CYCLES_NE_CBO_NCS = 0x0200ull, // TBD (experimental)
		UNC_U_M2U_MISC1__MASK__ICX_UNC_U_M2U_MISC1__RxC_CYCLES_NE_UPI_NCB = 0x0400ull, // TBD (experimental)
		UNC_U_M2U_MISC1__MASK__ICX_UNC_U_M2U_MISC1__RxC_CYCLES_NE_UPI_NCS = 0x0800ull, // TBD (experimental)
		UNC_U_M2U_MISC1__MASK__ICX_UNC_U_M2U_MISC1__TxC_CYCLES_CRD_OVF_CBO_NCB = 0x1000ull, // TBD (experimental)
		UNC_U_M2U_MISC1__MASK__ICX_UNC_U_M2U_MISC1__TxC_CYCLES_CRD_OVF_CBO_NCS = 0x2000ull, // TBD (experimental)
		UNC_U_M2U_MISC1__MASK__ICX_UNC_U_M2U_MISC1__TxC_CYCLES_CRD_OVF_UPI_NCB = 0x4000ull, // TBD (experimental)
		UNC_U_M2U_MISC1__MASK__ICX_UNC_U_M2U_MISC1__TxC_CYCLES_CRD_OVF_UPI_NCS = 0x8000ull, // TBD (experimental)
		UNC_U_M2U_MISC2 = 0x004e, // TBD
		UNC_U_M2U_MISC2__MASK__ICX_UNC_U_M2U_MISC2__RxC_CYCLES_EMPTY_BL = 0x0200ull, // TBD (experimental)
		UNC_U_M2U_MISC2__MASK__ICX_UNC_U_M2U_MISC2__RxC_CYCLES_FULL_BL = 0x0100ull, // TBD (experimental)
		UNC_U_M2U_MISC2__MASK__ICX_UNC_U_M2U_MISC2__TxC_CYCLES_CRD_OVF_VN0_NCB = 0x0400ull, // TBD (experimental)
		UNC_U_M2U_MISC2__MASK__ICX_UNC_U_M2U_MISC2__TxC_CYCLES_CRD_OVF_VN0_NCS = 0x0800ull, // TBD (experimental)
		UNC_U_M2U_MISC2__MASK__ICX_UNC_U_M2U_MISC2__TxC_CYCLES_EMPTY_AK = 0x2000ull, // TBD (experimental)
		UNC_U_M2U_MISC2__MASK__ICX_UNC_U_M2U_MISC2__TxC_CYCLES_EMPTY_AKC = 0x4000ull, // TBD (experimental)
		UNC_U_M2U_MISC2__MASK__ICX_UNC_U_M2U_MISC2__TxC_CYCLES_EMPTY_BL = 0x1000ull, // TBD (experimental)
		UNC_U_M2U_MISC2__MASK__ICX_UNC_U_M2U_MISC2__TxC_CYCLES_FULL_BL = 0x8000ull, // TBD (experimental)
		UNC_U_M2U_MISC3 = 0x004f, // TBD
		UNC_U_M2U_MISC3__MASK__ICX_UNC_U_M2U_MISC3__TxC_CYCLES_FULL_AK = 0x0100ull, // TBD (experimental)
		UNC_U_M2U_MISC3__MASK__ICX_UNC_U_M2U_MISC3__TxC_CYCLES_FULL_AKC = 0x0200ull, // TBD (experimental)
		UNC_U_PHOLD_CYCLES = 0x0045, // Cycles PHOLD Assert to Ack
		UNC_U_PHOLD_CYCLES__MASK__ICX_UNC_U_PHOLD_CYCLES__ASSERT_TO_ACK = 0x0100ull, // Assert to ACK (experimental)
		UNC_U_RACU_DRNG = 0x004c, // TBD
		UNC_U_RACU_DRNG__MASK__ICX_UNC_U_RACU_DRNG__PFTCH_BUF_EMPTY = 0x0400ull, // TBD (experimental)
		UNC_U_RACU_DRNG__MASK__ICX_UNC_U_RACU_DRNG__RDRAND = 0x0100ull, // TBD (experimental)
		UNC_U_RACU_DRNG__MASK__ICX_UNC_U_RACU_DRNG__RDSEED = 0x0200ull, // TBD (experimental)
		UNC_U_RACU_REQUESTS = 0x0046, // RACU Request (experimental)
		
	};
};

namespace icx_unc_ubox = optkit::intel::icx_unc_ubox;

#undef INTEL_X86_EDGE_BIT
#undef INTEL_X86_ANY_BIT
#undef INTEL_X86_INV_BIT
#undef INTEL_X86_CMASK_BIT
#undef INTEL_X86_MOD_EDGE
#undef INTEL_X86_MOD_ANY
#undef INTEL_X86_MOD_INV
