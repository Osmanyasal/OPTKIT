#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::snb_unc_cbo{
	enum snb_unc_cbo : uint64_t {
		UNC_CBO_XSNP_RESPONSE = 0x22, // Snoop responses (must provide a snoop type and filter)
		UNC_CBO_XSNP_RESPONSE__MASK__SNB_UNC_CBO_XSNP_RESPONSE__MISS = 0x100, // Number of snoop misses
		UNC_CBO_XSNP_RESPONSE__MASK__SNB_UNC_CBO_XSNP_RESPONSE__INVAL = 0x200, // Number of snoop invalidates of a non-modified line
		UNC_CBO_XSNP_RESPONSE__MASK__SNB_UNC_CBO_XSNP_RESPONSE__HIT = 0x400, // Number of snoop hits of a non-modified line
		UNC_CBO_XSNP_RESPONSE__MASK__SNB_UNC_CBO_XSNP_RESPONSE__HITM = 0x800, // Number of snoop hits of a modified line
		UNC_CBO_XSNP_RESPONSE__MASK__SNB_UNC_CBO_XSNP_RESPONSE__INVAL_M = 0x1000, // Number of snoop invalidates of a modified line
		UNC_CBO_XSNP_RESPONSE__MASK__SNB_UNC_CBO_XSNP_RESPONSE__ANY_SNP = 0x1f00, // Number of snoops
		UNC_CBO_XSNP_RESPONSE__MASK__SNB_UNC_CBO_XSNP_RESPONSE__EXTERNAL_FILTER = 0x2000, // Filter on cross-core snoops initiated by this Cbox due to external snoop request
		UNC_CBO_XSNP_RESPONSE__MASK__SNB_UNC_CBO_XSNP_RESPONSE__XCORE_FILTER = 0x4000, // Filter on cross-core snoops initiated by this Cbox due to processor core memory request
		UNC_CBO_XSNP_RESPONSE__MASK__SNB_UNC_CBO_XSNP_RESPONSE__EVICTION_FILTER = 0x8000, // Filter on cross-core snoops initiated by this Cbox due to LLC eviction
		UNC_CBO_CACHE_LOOKUP = 0x34, // LLC cache lookups
		UNC_CBO_CACHE_LOOKUP__MASK__SNB_UNC_CBO_CACHE_LOOKUP__STATE_M = 0x100, // Number of LLC lookup requests for a line in modified state
		UNC_CBO_CACHE_LOOKUP__MASK__SNB_UNC_CBO_CACHE_LOOKUP__STATE_E = 0x200, // Number of LLC lookup requests for a line in exclusive state
		UNC_CBO_CACHE_LOOKUP__MASK__SNB_UNC_CBO_CACHE_LOOKUP__STATE_S = 0x400, // Number of LLC lookup requests for a line in shared state
		UNC_CBO_CACHE_LOOKUP__MASK__SNB_UNC_CBO_CACHE_LOOKUP__STATE_I = 0x800, // Number of LLC lookup requests for a line in invalid state
		UNC_CBO_CACHE_LOOKUP__MASK__SNB_UNC_CBO_CACHE_LOOKUP__STATE_MESI = 0xf00, // Number of LLC lookup requests for a line
		UNC_CBO_CACHE_LOOKUP__MASK__SNB_UNC_CBO_CACHE_LOOKUP__READ_FILTER = 0x1000, // Filter on processor core initiated cacheable read requests
		UNC_CBO_CACHE_LOOKUP__MASK__SNB_UNC_CBO_CACHE_LOOKUP__WRITE_FILTER = 0x2000, // Filter on processor core initiated cacheable write requests
		UNC_CBO_CACHE_LOOKUP__MASK__SNB_UNC_CBO_CACHE_LOOKUP__EXTSNP_FILTER = 0x4000, // Filter on external snoop requests
		UNC_CBO_CACHE_LOOKUP__MASK__SNB_UNC_CBO_CACHE_LOOKUP__ANY_FILTER = 0x8000, // Filter on any IRQ or IPQ initiated requests including uncacheable
		
	};
};

namespace snb_unc_cbo = optkit::intel::snb_unc_cbo;