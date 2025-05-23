#pragma once
#include <cstdint>
#define INTEL_X86_EDGE_BIT	18
#define INTEL_X86_ANY_BIT	21
#define INTEL_X86_INV_BIT	23
#define INTEL_X86_CMASK_BIT 24
#define INTEL_X86_MOD_EDGE	(1 << INTEL_X86_EDGE_BIT)
#define INTEL_X86_MOD_ANY	(1 << INTEL_X86_ANY_BIT)
#define INTEL_X86_MOD_INV	(1 << INTEL_X86_INV_BIT)
namespace optkit::intel::ivbep_unc_u{
	enum ivbep_unc_u : uint64_t {
		UNC_U_EVENT_MSG = 0x42, // VLW Received
		UNC_U_EVENT_MSG__MASK__IVBEP_UNC_U_EVENT_MSG__DOORBELL_RCVD = 0x800, // TBD
		UNC_U_EVENT_MSG__MASK__IVBEP_UNC_U_EVENT_MSG__INT_PRIO = 0x1000, // TBD
		UNC_U_EVENT_MSG__MASK__IVBEP_UNC_U_EVENT_MSG__IPI_RCVD = 0x400, // TBD
		UNC_U_EVENT_MSG__MASK__IVBEP_UNC_U_EVENT_MSG__MSI_RCVD = 0x200, // TBD
		UNC_U_EVENT_MSG__MASK__IVBEP_UNC_U_EVENT_MSG__VLW_RCVD = 0x100, // TBD
		UNC_U_LOCK_CYCLES = 0x44, // IDI Lock/SplitLock Cycles
		UNC_U_PHOLD_CYCLES = 0x45, // Cycles PHOLD asserts to Ack
		UNC_U_PHOLD_CYCLES__MASK__IVBEP_UNC_U_PHOLD_CYCLES__ASSERT_TO_ACK = 0x100, // Number of cycles asserted to ACK
		UNC_U_PHOLD_CYCLES__MASK__IVBEP_UNC_U_PHOLD_CYCLES__ACK_TO_DEASSERT = 0x100, // Number of cycles ACK to deassert
		UNC_U_RACU_REQUESTS = 0x46, // RACU requests
		
	};
};

namespace ivbep_unc_u = optkit::intel::ivbep_unc_u;

#undef INTEL_X86_EDGE_BIT
#undef INTEL_X86_ANY_BIT
#undef INTEL_X86_INV_BIT
#undef INTEL_X86_CMASK_BIT
#undef INTEL_X86_MOD_EDGE
#undef INTEL_X86_MOD_ANY
#undef INTEL_X86_MOD_INV
