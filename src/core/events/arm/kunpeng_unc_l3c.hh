#pragma once
#include <cstdint>
namespace optkit::arm::kunpeng_unc_l3c{
	enum kunpeng_unc_l3c : uint64_t {
		spill_success = 0x21, // rd_cpipe
		wr_cpipe = 0x01, // Total write accesses.
		rd_hit_cpipe = 0x02, // Total read hits.
		wr_hit_cpipe = 0x03, // Total write hits.
		victim_num = 0x04, // l3c precharge commands.
		rd_spipe = 0x20, // Count of the number of read lines that come from this cluster of CPU core in spipe.
		wr_spipe = 0x21, // Count of the number of write lines that come from this cluster of CPU core in spipe.
		rd_hit_spipe = 0x22, // Count of the number of read lines that hits in spipe of this L3C.
		wr_hit_spipe = 0x23, // Count of the number of write lines that hits in spipe of this L3C.
		back_invalid = 0x29, // Count of the number of L3C back invalid operations.
		retry_cpu = 0x40, // Count of the number of retry that L3C suppresses the CPU operations.
		retry_ring = 0x41, // Count of the number of retry that L3C suppresses the ring operations.
		
	};
};

namespace kunpeng_unc_l3c = optkit::arm::kunpeng_unc_l3c;