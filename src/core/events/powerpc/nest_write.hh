#pragma once
#include <cstdint>
namespace optkit::ibm::powerpc_nest_write{
	enum powerpc_nest_write : uint64_t {
		MCS_00 = 0x198, // Total Write Bandwidth seen on both MCS of MC0
		MCS_01 = 0x1a0, // Total Write Bandwidth seen on both MCS of MC1
		MCS_02 = 0x1a8, // Total Write Bandwidth seen on both MCS of MC2
		MCS_03 = 0x1b0, // Total Write Bandwidth seen on both MCS of MC3
		
	};
};

namespace powerpc_nest_write = optkit::ibm::powerpc_nest_write;