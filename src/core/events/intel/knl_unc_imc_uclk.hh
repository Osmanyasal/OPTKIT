#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::knl_unc_imc_uclk{
	enum knl_unc_imc_uclk : uint64_t {
		UNC_M_U_CLOCKTICKS = 0x00, // IMC UCLK counts
		
	};
};

namespace knl_unc_imc_uclk = optkit::intel::knl_unc_imc_uclk;