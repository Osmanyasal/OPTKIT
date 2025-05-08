#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::snbep_unc_u{
	enum snbep_unc_u : uint64_t {
		UNC_U_EVENT_MSG = 0x42, // VLW Received
		UNC_U_EVENT_MSG__MASK__SNBEP_UNC_U_EVENT_MSG__DOORBELL_RCVD = 0x800, // TBD
		UNC_U_EVENT_MSG__MASK__SNBEP_UNC_U_EVENT_MSG__INT_PRIO = 0x1000, // TBD
		UNC_U_EVENT_MSG__MASK__SNBEP_UNC_U_EVENT_MSG__IPI_RCVD = 0x400, // TBD
		UNC_U_EVENT_MSG__MASK__SNBEP_UNC_U_EVENT_MSG__MSI_RCVD = 0x200, // TBD
		UNC_U_EVENT_MSG__MASK__SNBEP_UNC_U_EVENT_MSG__VLW_RCVD = 0x100, // TBD
		UNC_U_LOCK_CYCLES = 0x44, // IDI Lock/SplitLock Cycles
		
	};
};

namespace snbep_unc_u = optkit::intel::snbep_unc_u;