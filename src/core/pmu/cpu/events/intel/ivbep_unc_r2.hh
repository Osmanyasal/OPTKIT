#pragma once
#include <cstdint>
#define INTEL_X86_EDGE_BIT	18
#define INTEL_X86_ANY_BIT	21
#define INTEL_X86_INV_BIT	23
#define INTEL_X86_CMASK_BIT 24
#define INTEL_X86_MOD_EDGE	(1 << INTEL_X86_EDGE_BIT)
#define INTEL_X86_MOD_ANY	(1 << INTEL_X86_ANY_BIT)
#define INTEL_X86_MOD_INV	(1 << INTEL_X86_INV_BIT)
namespace optkit::intel::ivbep_unc_r2{
	enum ivbep_unc_r2 : uint64_t {
		UNC_R2_CLOCKTICKS = 0x1, // Number of uclks in domain
		UNC_R2_RING_AD_USED = 0x7, // R2 AD Ring in Use
		UNC_R2_RING_AD_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CCW_VR0_EVEN = 0x400ull, // Counter-clockwise and even ring polarity on virtual ring 0
		UNC_R2_RING_AD_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CCW_VR0_ODD = 0x800ull, // Counter-clockwise and odd ring polarity on virtual ring 0
		UNC_R2_RING_AD_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CW_VR0_EVEN = 0x100ull, // Clockwise and even ring polarity on virtual ring 0
		UNC_R2_RING_AD_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CW_VR0_ODD = 0x200ull, // Clockwise and odd ring polarity on virtual ring 0
		UNC_R2_RING_AD_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CCW_VR1_EVEN = 0x400ull, // Counter-clockwise and even ring polarity on virtual ring 1
		UNC_R2_RING_AD_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CCW_VR1_ODD = 0x800ull, // Counter-clockwise and odd ring polarity on virtual ring 1
		UNC_R2_RING_AD_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CW_VR1_EVEN = 0x100ull, // Clockwise and even ring polarity on virtual ring 1
		UNC_R2_RING_AD_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CW_VR1_ODD = 0x200ull, // Clockwise and odd ring polarity on virtual ring 1
		UNC_R2_RING_AD_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CW = 0x3300ull, // Clockwise with any polarity on either virtual rings
		UNC_R2_RING_AD_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CCW = 0xcc00ull, // Counter-clockwise with any polarity on either virtual rings
		UNC_R2_RING_AK_USED = 0x8, // R2 AK Ring in Use
		UNC_R2_RING_AK_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CCW_VR0_EVEN = 0x400ull, // Counter-clockwise and even ring polarity on virtual ring 0
		UNC_R2_RING_AK_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CCW_VR0_ODD = 0x800ull, // Counter-clockwise and odd ring polarity on virtual ring 0
		UNC_R2_RING_AK_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CW_VR0_EVEN = 0x100ull, // Clockwise and even ring polarity on virtual ring 0
		UNC_R2_RING_AK_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CW_VR0_ODD = 0x200ull, // Clockwise and odd ring polarity on virtual ring 0
		UNC_R2_RING_AK_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CCW_VR1_EVEN = 0x400ull, // Counter-clockwise and even ring polarity on virtual ring 1
		UNC_R2_RING_AK_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CCW_VR1_ODD = 0x800ull, // Counter-clockwise and odd ring polarity on virtual ring 1
		UNC_R2_RING_AK_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CW_VR1_EVEN = 0x100ull, // Clockwise and even ring polarity on virtual ring 1
		UNC_R2_RING_AK_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CW_VR1_ODD = 0x200ull, // Clockwise and odd ring polarity on virtual ring 1
		UNC_R2_RING_AK_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CW = 0x3300ull, // Clockwise with any polarity on either virtual rings
		UNC_R2_RING_AK_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CCW = 0xcc00ull, // Counter-clockwise with any polarity on either virtual rings
		UNC_R2_RING_BL_USED = 0x9, // R2 BL Ring in Use
		UNC_R2_RING_BL_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CCW_VR0_EVEN = 0x400ull, // Counter-clockwise and even ring polarity on virtual ring 0
		UNC_R2_RING_BL_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CCW_VR0_ODD = 0x800ull, // Counter-clockwise and odd ring polarity on virtual ring 0
		UNC_R2_RING_BL_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CW_VR0_EVEN = 0x100ull, // Clockwise and even ring polarity on virtual ring 0
		UNC_R2_RING_BL_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CW_VR0_ODD = 0x200ull, // Clockwise and odd ring polarity on virtual ring 0
		UNC_R2_RING_BL_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CCW_VR1_EVEN = 0x400ull, // Counter-clockwise and even ring polarity on virtual ring 1
		UNC_R2_RING_BL_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CCW_VR1_ODD = 0x800ull, // Counter-clockwise and odd ring polarity on virtual ring 1
		UNC_R2_RING_BL_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CW_VR1_EVEN = 0x100ull, // Clockwise and even ring polarity on virtual ring 1
		UNC_R2_RING_BL_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CW_VR1_ODD = 0x200ull, // Clockwise and odd ring polarity on virtual ring 1
		UNC_R2_RING_BL_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CW = 0x3300ull, // Clockwise with any polarity on either virtual rings
		UNC_R2_RING_BL_USED__MASK__IVBEP_UNC_R2_RING_AD_USED__CCW = 0xcc00ull, // Counter-clockwise with any polarity on either virtual rings
		UNC_R2_RING_IV_USED = 0xa, // R2 IV Ring in Use
		UNC_R2_RING_IV_USED__MASK__IVBEP_UNC_R2_RING_IV_USED__CW = 0x3300ull, // Clockwise with any polarity on either virtual rings
		UNC_R2_RING_IV_USED__MASK__IVBEP_UNC_R2_RING_IV_USED__CCW = 0xcc00ull, // Counter-clockwise with any polarity on either virtual rings
		UNC_R2_RING_IV_USED__MASK__IVBEP_UNC_R2_RING_IV_USED__ANY = 0xff00ull, // any direction and any polarity on any virtual ring
		UNC_R2_RXR_AK_BOUNCES = 0x12, // AK Ingress Bounced
		UNC_R2_RXR_AK_BOUNCES__MASK__IVBEP_UNC_R2_RXR_AK_BOUNCES__CW = 0x100ull, // Clockwise
		UNC_R2_RXR_AK_BOUNCES__MASK__IVBEP_UNC_R2_RXR_AK_BOUNCES__CCW = 0x200ull, // Counter-clockwise
		UNC_R2_RXR_OCCUPANCY = 0x13, // Ingress occupancy accumulator
		UNC_R2_RXR_OCCUPANCY__MASK__IVBEP_UNC_R2_RXR_OCCUPANCY__DRS = 0x800ull, // DRS Ingress queue
		UNC_R2_RXR_CYCLES_NE = 0x10, // Ingress Cycles Not Empty
		UNC_R2_RXR_CYCLES_NE__MASK__IVBEP_UNC_R2_RXR_CYCLES_NE__NCB = 0x1000ull, // NCB Ingress queue
		UNC_R2_RXR_CYCLES_NE__MASK__IVBEP_UNC_R2_RXR_CYCLES_NE__NCS = 0x2000ull, // NCS Ingress queue
		UNC_R2_RXR_INSERTS = 0x11, // Ingress inserts
		UNC_R2_RXR_INSERTS__MASK__IVBEP_UNC_R2_RXR_CYCLES_NE__NCB = 0x1000ull, // NCB Ingress queue
		UNC_R2_RXR_INSERTS__MASK__IVBEP_UNC_R2_RXR_CYCLES_NE__NCS = 0x2000ull, // NCS Ingress queue
		UNC_R2_TXR_CYCLES_FULL = 0x25, // Egress Cycles Full
		UNC_R2_TXR_CYCLES_FULL__MASK__IVBEP_UNC_R2_TXR_CYCLES_FULL__AD = 0x100ull, // AD Egress queue
		UNC_R2_TXR_CYCLES_FULL__MASK__IVBEP_UNC_R2_TXR_CYCLES_FULL__AK = 0x200ull, // AK Egress queue
		UNC_R2_TXR_CYCLES_FULL__MASK__IVBEP_UNC_R2_TXR_CYCLES_FULL__BL = 0x400ull, // BL Egress queue
		UNC_R2_TXR_CYCLES_NE = 0x23, // Egress Cycles Not Empty
		UNC_R2_TXR_CYCLES_NE__MASK__IVBEP_UNC_R2_TXR_CYCLES_FULL__AD = 0x100ull, // AD Egress queue
		UNC_R2_TXR_CYCLES_NE__MASK__IVBEP_UNC_R2_TXR_CYCLES_FULL__AK = 0x200ull, // AK Egress queue
		UNC_R2_TXR_CYCLES_NE__MASK__IVBEP_UNC_R2_TXR_CYCLES_FULL__BL = 0x400ull, // BL Egress queue
		UNC_R2_TXR_NACK_CCW = 0x28, // Egress counter-clockwise BACK
		UNC_R2_TXR_NACK_CCW__MASK__IVBEP_UNC_R2_TXR_CYCLES_FULL__AD = 0x100ull, // AD Egress queue
		UNC_R2_TXR_NACK_CCW__MASK__IVBEP_UNC_R2_TXR_CYCLES_FULL__AK = 0x200ull, // AK Egress queue
		UNC_R2_TXR_NACK_CCW__MASK__IVBEP_UNC_R2_TXR_CYCLES_FULL__BL = 0x400ull, // BL Egress queue
		UNC_R2_TXR_NACK_CW = 0x26, // Egress clockwise BACK
		UNC_R2_TXR_NACK_CW__MASK__IVBEP_UNC_R2_TXR_CYCLES_FULL__AD = 0x100ull, // AD Egress queue
		UNC_R2_TXR_NACK_CW__MASK__IVBEP_UNC_R2_TXR_CYCLES_FULL__AK = 0x200ull, // AK Egress queue
		UNC_R2_TXR_NACK_CW__MASK__IVBEP_UNC_R2_TXR_CYCLES_FULL__BL = 0x400ull, // BL Egress queue
		
	};
};

namespace ivbep_unc_r2 = optkit::intel::ivbep_unc_r2;

#undef INTEL_X86_EDGE_BIT
#undef INTEL_X86_ANY_BIT
#undef INTEL_X86_INV_BIT
#undef INTEL_X86_CMASK_BIT
#undef INTEL_X86_MOD_EDGE
#undef INTEL_X86_MOD_ANY
#undef INTEL_X86_MOD_INV
