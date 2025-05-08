#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::hswep_unc_u{
	enum hswep_unc_u : uint64_t {
		UNC_U_EVENT_MSG = 0x42, // VLW Received
		UNC_U_EVENT_MSG__MASK__HSWEP_UNC_U_EVENT_MSG__DOORBELL_RCVD = 0x800, // TBD
		UNC_U_PHOLD_CYCLES = 0x45, // Cycles PHOLD asserts to Ack
		UNC_U_PHOLD_CYCLES__MASK__HSWEP_UNC_U_PHOLD_CYCLES__ASSERT_TO_ACK = 0x100, // Number of cycles asserted to ACK
		UNC_U_RACU_REQUESTS = 0x46, // RACU requests
		
	};
};

namespace hswep_unc_u = optkit::intel::hswep_unc_u;