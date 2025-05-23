#pragma once
#include <cstdint>
namespace optkit::amd64::fam1ah_zen5_l3{
	enum fam1ah_zen5_l3 : uint64_t {
		UNC_L3_REQUESTS = 0x04, // Number of requests to L3 cache
		UNC_L3_REQUESTS__MASK__AMD64_FAM1AH_ZEN5_L3_REQUESTS__L3_MISS = 0x1, // L3 miss
		UNC_L3_REQUESTS__MASK__AMD64_FAM1AH_ZEN5_L3_REQUESTS__L3_HIT = 0xfe, // L3 hit
		UNC_L3_REQUESTS__MASK__AMD64_FAM1AH_ZEN5_L3_REQUESTS__ALL = 0xff, // All types of requests
		
	};
};

namespace fam1ah_zen5_l3 = optkit::amd64::fam1ah_zen5_l3;

