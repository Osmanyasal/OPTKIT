#pragma once
#include <cstdint>
#define INTEL_X86_EDGE_BIT	18
#define INTEL_X86_ANY_BIT	21
#define INTEL_X86_INV_BIT	23
#define INTEL_X86_CMASK_BIT 24
#define INTEL_X86_MOD_EDGE	(1 << INTEL_X86_EDGE_BIT)
#define INTEL_X86_MOD_ANY	(1 << INTEL_X86_ANY_BIT)
#define INTEL_X86_MOD_INV	(1 << INTEL_X86_INV_BIT)
namespace optkit::intel::bdx_unc_s{
	enum bdx_unc_s : uint64_t {
		UNC_S_BOUNCE_CONTROL = 0xa, // TBD
		UNC_S_CLOCKTICKS = 0x0, // TBD
		UNC_S_FAST_ASSERTED = 0x9, // Counts the number of cycles either the local or incoming distress signals are asserted.  Incoming distress includes up
		UNC_S_RING_AD_USED = 0x1b, // Counts the number of cycles that the AD ring is being used at this ring stop.  This includes when packets are passing by and when packets are being sent
		UNC_S_RING_AD_USED__MASK__BDX_UNC_S_RING_AD_USED__DOWN_EVEN = 0x400ull, // Down and Event
		UNC_S_RING_AD_USED__MASK__BDX_UNC_S_RING_AD_USED__DOWN_ODD = 0x800ull, // Down and Odd
		UNC_S_RING_AD_USED__MASK__BDX_UNC_S_RING_AD_USED__UP_EVEN = 0x100ull, // Up and Even
		UNC_S_RING_AD_USED__MASK__BDX_UNC_S_RING_AD_USED__UP_ODD = 0x200ull, // Up and Odd
		UNC_S_RING_AD_USED__MASK__BDX_UNC_S_RING_AD_USED__UP = 0x300ull, // Up
		UNC_S_RING_AD_USED__MASK__BDX_UNC_S_RING_AD_USED__DOWN = 0xcc00ull, // Down
		UNC_S_RING_AK_USED = 0x1c, // Counts the number of cycles that the AK ring is being used at this ring stop.  This includes when packets are passing by and when packets are being sent
		UNC_S_RING_AK_USED__MASK__BDX_UNC_S_RING_AD_USED__DOWN_EVEN = 0x400ull, // Down and Event
		UNC_S_RING_AK_USED__MASK__BDX_UNC_S_RING_AD_USED__DOWN_ODD = 0x800ull, // Down and Odd
		UNC_S_RING_AK_USED__MASK__BDX_UNC_S_RING_AD_USED__UP_EVEN = 0x100ull, // Up and Even
		UNC_S_RING_AK_USED__MASK__BDX_UNC_S_RING_AD_USED__UP_ODD = 0x200ull, // Up and Odd
		UNC_S_RING_AK_USED__MASK__BDX_UNC_S_RING_AD_USED__UP = 0x300ull, // Up
		UNC_S_RING_AK_USED__MASK__BDX_UNC_S_RING_AD_USED__DOWN = 0xcc00ull, // Down
		UNC_S_RING_BL_USED = 0x1d, // Counts the number of cycles that the BL ring is being used at this ring stop.  This includes when packets are passing by and when packets are being sent
		UNC_S_RING_BL_USED__MASK__BDX_UNC_S_RING_AD_USED__DOWN_EVEN = 0x400ull, // Down and Event
		UNC_S_RING_BL_USED__MASK__BDX_UNC_S_RING_AD_USED__DOWN_ODD = 0x800ull, // Down and Odd
		UNC_S_RING_BL_USED__MASK__BDX_UNC_S_RING_AD_USED__UP_EVEN = 0x100ull, // Up and Even
		UNC_S_RING_BL_USED__MASK__BDX_UNC_S_RING_AD_USED__UP_ODD = 0x200ull, // Up and Odd
		UNC_S_RING_BL_USED__MASK__BDX_UNC_S_RING_AD_USED__UP = 0x300ull, // Up
		UNC_S_RING_BL_USED__MASK__BDX_UNC_S_RING_AD_USED__DOWN = 0xcc00ull, // Down
		UNC_S_RING_BOUNCES = 0x5, // TBD
		UNC_S_RING_BOUNCES__MASK__BDX_UNC_S_RING_BOUNCES__AD_CACHE = 0x100ull, // Number of LLC responses that bounced on the Ring. --
		UNC_S_RING_BOUNCES__MASK__BDX_UNC_S_RING_BOUNCES__AK_CORE = 0x200ull, // Number of LLC responses that bounced on the Ring. -- Acknowledgements to core
		UNC_S_RING_BOUNCES__MASK__BDX_UNC_S_RING_BOUNCES__BL_CORE = 0x400ull, // Number of LLC responses that bounced on the Ring. -- Data Responses to core
		UNC_S_RING_BOUNCES__MASK__BDX_UNC_S_RING_BOUNCES__IV_CORE = 0x800ull, // Number of LLC responses that bounced on the Ring. -- Snoops of processors cachee.
		UNC_S_RING_IV_USED = 0x1e, // Counts the number of cycles that the BL ring is being used at this ring stop.  This includes when packets are passing by and when packets are being sent
		UNC_S_RING_IV_USED__MASK__BDX_UNC_S_RING_IV_USED__DN = 0xc00ull, // BL Ring in Use -- Any
		UNC_S_RING_IV_USED__MASK__BDX_UNC_S_RING_IV_USED__UP = 0x300ull, // BL Ring in Use -- Any
		UNC_S_RXR_BYPASS = 0x12, // Bypass the Sbo Ingress.
		UNC_S_RXR_BYPASS__MASK__BDX_UNC_S_RXR_BYPASS__AD_BNC = 0x200ull, // Bypass -- AD - Bounces
		UNC_S_RXR_BYPASS__MASK__BDX_UNC_S_RXR_BYPASS__AD_CRD = 0x100ull, // Bypass -- AD - Credits
		UNC_S_RXR_BYPASS__MASK__BDX_UNC_S_RXR_BYPASS__AK = 0x1000ull, // Bypass -- AK
		UNC_S_RXR_BYPASS__MASK__BDX_UNC_S_RXR_BYPASS__BL_BNC = 0x800ull, // Bypass -- BL - Bounces
		UNC_S_RXR_BYPASS__MASK__BDX_UNC_S_RXR_BYPASS__BL_CRD = 0x400ull, // Bypass -- BL - Credits
		UNC_S_RXR_BYPASS__MASK__BDX_UNC_S_RXR_BYPASS__IV = 0x2000ull, // Bypass -- IV
		UNC_S_RXR_INSERTS = 0x13, // Number of allocations into the Sbo Ingress  The Ingress is used to queue up requests received from the ring.
		UNC_S_RXR_INSERTS__MASK__BDX_UNC_S_RXR_INSERTS__AD_BNC = 0x200ull, // Ingress Allocations -- AD - Bounces
		UNC_S_RXR_INSERTS__MASK__BDX_UNC_S_RXR_INSERTS__AD_CRD = 0x100ull, // Ingress Allocations -- AD - Credits
		UNC_S_RXR_INSERTS__MASK__BDX_UNC_S_RXR_INSERTS__AK = 0x1000ull, // Ingress Allocations -- AK
		UNC_S_RXR_INSERTS__MASK__BDX_UNC_S_RXR_INSERTS__BL_BNC = 0x800ull, // Ingress Allocations -- BL - Bounces
		UNC_S_RXR_INSERTS__MASK__BDX_UNC_S_RXR_INSERTS__BL_CRD = 0x400ull, // Ingress Allocations -- BL - Credits
		UNC_S_RXR_INSERTS__MASK__BDX_UNC_S_RXR_INSERTS__IV = 0x2000ull, // Ingress Allocations -- IV
		UNC_S_RXR_OCCUPANCY = 0x11, // Occupancy event for the Ingress buffers in the Sbo.  The Ingress is used to queue up requests received from the ring.
		UNC_S_RXR_OCCUPANCY__MASK__BDX_UNC_S_RXR_OCCUPANCY__AD_BNC = 0x200ull, // Ingress Occupancy -- AD - Bounces
		UNC_S_RXR_OCCUPANCY__MASK__BDX_UNC_S_RXR_OCCUPANCY__AD_CRD = 0x100ull, // Ingress Occupancy -- AD - Credits
		UNC_S_RXR_OCCUPANCY__MASK__BDX_UNC_S_RXR_OCCUPANCY__AK = 0x1000ull, // Ingress Occupancy -- AK
		UNC_S_RXR_OCCUPANCY__MASK__BDX_UNC_S_RXR_OCCUPANCY__BL_BNC = 0x800ull, // Ingress Occupancy -- BL - Bounces
		UNC_S_RXR_OCCUPANCY__MASK__BDX_UNC_S_RXR_OCCUPANCY__BL_CRD = 0x400ull, // Ingress Occupancy -- BL - Credits
		UNC_S_RXR_OCCUPANCY__MASK__BDX_UNC_S_RXR_OCCUPANCY__IV = 0x2000ull, // Ingress Occupancy -- IV
		UNC_S_TXR_ADS_USED = 0x4, // TBD
		UNC_S_TXR_ADS_USED__MASK__BDX_UNC_S_TXR_ADS_USED__AD = 0x100ull, // TBD
		UNC_S_TXR_ADS_USED__MASK__BDX_UNC_S_TXR_ADS_USED__AK = 0x200ull, // TBD
		UNC_S_TXR_ADS_USED__MASK__BDX_UNC_S_TXR_ADS_USED__BL = 0x400ull, // TBD
		UNC_S_TXR_INSERTS = 0x2, // Number of allocations into the Sbo Egress.  The Egress is used to queue up requests destined for the ring.
		UNC_S_TXR_INSERTS__MASK__BDX_UNC_S_TXR_INSERTS__AD_BNC = 0x200ull, // Egress Allocations -- AD - Bounces
		UNC_S_TXR_INSERTS__MASK__BDX_UNC_S_TXR_INSERTS__AD_CRD = 0x100ull, // Egress Allocations -- AD - Credits
		UNC_S_TXR_INSERTS__MASK__BDX_UNC_S_TXR_INSERTS__AK = 0x1000ull, // Egress Allocations -- AK
		UNC_S_TXR_INSERTS__MASK__BDX_UNC_S_TXR_INSERTS__BL_BNC = 0x800ull, // Egress Allocations -- BL - Bounces
		UNC_S_TXR_INSERTS__MASK__BDX_UNC_S_TXR_INSERTS__BL_CRD = 0x400ull, // Egress Allocations -- BL - Credits
		UNC_S_TXR_INSERTS__MASK__BDX_UNC_S_TXR_INSERTS__IV = 0x2000ull, // Egress Allocations -- IV
		UNC_S_TXR_OCCUPANCY = 0x1, // Occupancy event for the Egress buffers in the Sbo.  The egress is used to queue up requests destined for the ring.
		UNC_S_TXR_OCCUPANCY__MASK__BDX_UNC_S_TXR_OCCUPANCY__AD_BNC = 0x200ull, // Egress Occupancy -- AD - Bounces
		UNC_S_TXR_OCCUPANCY__MASK__BDX_UNC_S_TXR_OCCUPANCY__AD_CRD = 0x100ull, // Egress Occupancy -- AD - Credits
		UNC_S_TXR_OCCUPANCY__MASK__BDX_UNC_S_TXR_OCCUPANCY__AK = 0x1000ull, // Egress Occupancy -- AK
		UNC_S_TXR_OCCUPANCY__MASK__BDX_UNC_S_TXR_OCCUPANCY__BL_BNC = 0x800ull, // Egress Occupancy -- BL - Bounces
		UNC_S_TXR_OCCUPANCY__MASK__BDX_UNC_S_TXR_OCCUPANCY__BL_CRD = 0x400ull, // Egress Occupancy -- BL - Credits
		UNC_S_TXR_OCCUPANCY__MASK__BDX_UNC_S_TXR_OCCUPANCY__IV = 0x2000ull, // Egress Occupancy -- IV
		UNC_S_TXR_ORDERING = 0x7, // TB
		UNC_S_TXR_ORDERING__MASK__BDX_UNC_S_TXR_ORDERING__IVSNOOPGO_UP = 0x100ull, // TBD
		UNC_S_TXR_ORDERING__MASK__BDX_UNC_S_TXR_ORDERING__IVSNOOP_DN = 0x200ull, // TBD
		UNC_S_TXR_ORDERING__MASK__BDX_UNC_S_TXR_ORDERING__AK_U2C_UP_EVEN = 0x400ull, // TBD
		UNC_S_TXR_ORDERING__MASK__BDX_UNC_S_TXR_ORDERING__AK_U2C_UP_ODD = 0x800ull, // TBD
		UNC_S_TXR_ORDERING__MASK__BDX_UNC_S_TXR_ORDERING__AK_U2C_DN_EVEN = 0x1000ull, // TBD
		UNC_S_TXR_ORDERING__MASK__BDX_UNC_S_TXR_ORDERING__AK_U2C_DN_ODD = 0x2000ull, // TBD
		
	};
};

namespace bdx_unc_s = optkit::intel::bdx_unc_s;

#undef INTEL_X86_EDGE_BIT
#undef INTEL_X86_ANY_BIT
#undef INTEL_X86_INV_BIT
#undef INTEL_X86_CMASK_BIT
#undef INTEL_X86_MOD_EDGE
#undef INTEL_X86_MOD_ANY
#undef INTEL_X86_MOD_INV
