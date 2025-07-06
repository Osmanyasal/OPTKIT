#pragma once
#include <cstdint>
#define INTEL_X86_EDGE_BIT	18
#define INTEL_X86_ANY_BIT	21
#define INTEL_X86_INV_BIT	23
#define INTEL_X86_CMASK_BIT 24
#define INTEL_X86_MOD_EDGE	(1 << INTEL_X86_EDGE_BIT)
#define INTEL_X86_MOD_ANY	(1 << INTEL_X86_ANY_BIT)
#define INTEL_X86_MOD_INV	(1 << INTEL_X86_INV_BIT)
namespace optkit::intel::knl_unc_edc_eclk{
	enum knl_unc_edc_eclk : uint64_t {
		UNC_E_E_CLOCKTICKS = 0x00, // EDC ECLK clockticks (generic counters)
		UNC_E_RPQ_INSERTS = 0x0101, // Counts total number of EDC RPQ insers
		UNC_E_WPQ_INSERTS = 0x0102, // Counts total number of EDC WPQ insers
		
	};
};

namespace knl_unc_edc_eclk = optkit::intel::knl_unc_edc_eclk;

#undef INTEL_X86_EDGE_BIT
#undef INTEL_X86_ANY_BIT
#undef INTEL_X86_INV_BIT
#undef INTEL_X86_CMASK_BIT
#undef INTEL_X86_MOD_EDGE
#undef INTEL_X86_MOD_ANY
#undef INTEL_X86_MOD_INV
