#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::bdx_unc_u{
	enum bdx_unc_u : uint64_t {
		UNC_U_EVENT_MSG = 0x42, // Virtual Logical Wire (legacy) message were received from uncore
		UNC_U_EVENT_MSG__MASK__BDX_UNC_U_EVENT_MSG__DOORBELL_RCVD = 0x800, // VLW Received
		UNC_U_PHOLD_CYCLES = 0x45, // PHOLD cycles.  Filter from source CoreID.
		UNC_U_PHOLD_CYCLES__MASK__BDX_UNC_U_PHOLD_CYCLES__ASSERT_TO_ACK = 0x100, // Cycles PHOLD Assert to Ack. Assert to ACK
		UNC_U_RACU_REQUESTS = 0x46, // Number outstanding register requests within message channel tracker
		
	};
};

namespace bdx_unc_u = optkit::intel::bdx_unc_u;