#pragma once
#include <cstdint>
namespace optkit::arm::thunderx2_unc_llc{
	enum thunderx2_unc_llc : uint64_t {
		UNC_CCPI_GIC = CCPI2_EVENT_GIC_PKT_SENT, // UNC_LLC_READ
		UNC_LLC_EVICT = L3_EVENT_EVICT_REQ, // Evict requests to LLC
		UNC_LLC_READ_HIT = L3_EVENT_READ_HIT, // Read requests to LLC which hit
		
	};
};

namespace thunderx2_unc_llc = optkit::arm::thunderx2_unc_llc;