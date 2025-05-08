#pragma once
#include <cstdint>
#include "intel_priv.hh"
namespace optkit::intel::snbep_unc_h{
	enum snbep_unc_h : uint64_t {
		UNC_H_CLOCKTICKS = 0x00, // HA Uncore clockticks
		UNC_H_CONFLICT_CYCLES = 0xb, // Conflict Checks
		UNC_H_CONFLICT_CYCLES__MASK__SNBEP_UNC_H_CONFLICT_CYCLES__CONFLICT = 0x200, // Number of cycles that we are handling conflicts
		UNC_H_CONFLICT_CYCLES__MASK__SNBEP_UNC_H_CONFLICT_CYCLES__NO_CONFLICT = 0x100, // Number of cycles that we are not handling conflicts
		UNC_H_DIRECT2CORE_COUNT = 0x11, // Direct2Core Messages Sent
		UNC_H_DIRECT2CORE_CYCLES_DISABLED = 0x12, // Cycles when Direct2Core was Disabled
		UNC_H_DIRECT2CORE_TXN_OVERRIDE = 0x13, // Number of Reads that had Direct2Core Overridden
		UNC_H_DIRECTORY_LOOKUP = 0xc, // Directory Lookups
		UNC_H_DIRECTORY_LOOKUP__MASK__SNBEP_UNC_H_DIRECTORY_LOOKUP__NO_SNP = 0x200, // Snoop not needed
		UNC_H_DIRECTORY_LOOKUP__MASK__SNBEP_UNC_H_DIRECTORY_LOOKUP__SNP = 0x100, // Snoop needed
		UNC_H_DIRECTORY_UPDATE = 0xd, // Directory Updates
		UNC_H_DIRECTORY_UPDATE__MASK__SNBEP_UNC_H_DIRECTORY_UPDATE__ANY = 0x300, // Counts any directory update
		UNC_H_DIRECTORY_UPDATE__MASK__SNBEP_UNC_H_DIRECTORY_UPDATE__CLEAR = 0x200, // Directory clears
		UNC_H_DIRECTORY_UPDATE__MASK__SNBEP_UNC_H_DIRECTORY_UPDATE__SET = 0x100, // Directory set
		UNC_H_IGR_NO_CREDIT_CYCLES = 0x22, // Cycles without QPI Ingress Credits
		UNC_H_IGR_NO_CREDIT_CYCLES__MASK__SNBEP_UNC_H_IGR_NO_CREDIT_CYCLES__AD_QPI0 = 0x100, // AD to QPI link 0
		UNC_H_IGR_NO_CREDIT_CYCLES__MASK__SNBEP_UNC_H_IGR_NO_CREDIT_CYCLES__AD_QPI1 = 0x200, // AD to QPI link 1
		UNC_H_IGR_NO_CREDIT_CYCLES__MASK__SNBEP_UNC_H_IGR_NO_CREDIT_CYCLES__BL_QPI0 = 0x400, // BL to QPI link 0
		UNC_H_IGR_NO_CREDIT_CYCLES__MASK__SNBEP_UNC_H_IGR_NO_CREDIT_CYCLES__BL_QPI1 = 0x800, // BL to QPI link 1
		UNC_H_IMC_RETRY = 0x1e, // Retry Events
		UNC_H_IMC_WRITES = 0x1a, // HA to iMC Full Line Writes Issued
		UNC_H_IMC_WRITES__MASK__SNBEP_UNC_H_IMC_WRITES__ALL = 0xf00, // Counts all writes
		UNC_H_IMC_WRITES__MASK__SNBEP_UNC_H_IMC_WRITES__FULL = 0x100, // Counts full line non ISOCH
		UNC_H_IMC_WRITES__MASK__SNBEP_UNC_H_IMC_WRITES__FULL_ISOCH = 0x400, // Counts ISOCH full line
		UNC_H_IMC_WRITES__MASK__SNBEP_UNC_H_IMC_WRITES__PARTIAL = 0x200, // Counts partial non-ISOCH
		UNC_H_IMC_WRITES__MASK__SNBEP_UNC_H_IMC_WRITES__PARTIAL_ISOCH = 0x800, // Counts ISOCH partial
		UNC_H_REQUESTS = 0x1, // Read and Write Requests
		UNC_H_REQUESTS__MASK__SNBEP_UNC_H_REQUESTS__READS = 0x300, // Counts incoming read requests. Good proxy for LLC read misses
		UNC_H_REQUESTS__MASK__SNBEP_UNC_H_REQUESTS__WRITES = 0xc00, // Counts incoming writes
		UNC_H_RPQ_CYCLES_NO_REG_CREDITS = 0x15, // iMC RPQ Credits Empty - Regular
		UNC_H_RPQ_CYCLES_NO_REG_CREDITS__MASK__SNBEP_UNC_H_RPQ_CYCLES_NO_REG_CREDITS__CHN0 = 0x100, // Channel 0
		UNC_H_RPQ_CYCLES_NO_REG_CREDITS__MASK__SNBEP_UNC_H_RPQ_CYCLES_NO_REG_CREDITS__CHN1 = 0x200, // Channel 1
		UNC_H_RPQ_CYCLES_NO_REG_CREDITS__MASK__SNBEP_UNC_H_RPQ_CYCLES_NO_REG_CREDITS__CHN2 = 0x400, // channel 2
		UNC_H_RPQ_CYCLES_NO_REG_CREDITS__MASK__SNBEP_UNC_H_RPQ_CYCLES_NO_REG_CREDITS__CHN3 = 0x800, // Chanell 3
		UNC_H_TAD_REQUESTS_G0 = 0x1b, // HA Requests to a TAD Region - Group 0
		UNC_H_TAD_REQUESTS_G0__MASK__SNBEP_UNC_H_TAD_REQUESTS_G0__REGION0 = 0x100, // Counts for TAD Region 0
		UNC_H_TAD_REQUESTS_G0__MASK__SNBEP_UNC_H_TAD_REQUESTS_G0__REGION1 = 0x200, // Counts for TAD Region 1
		UNC_H_TAD_REQUESTS_G0__MASK__SNBEP_UNC_H_TAD_REQUESTS_G0__REGION2 = 0x400, // Counts for TAD Region 2
		UNC_H_TAD_REQUESTS_G0__MASK__SNBEP_UNC_H_TAD_REQUESTS_G0__REGION3 = 0x800, // Counts for TAD Region 3
		UNC_H_TAD_REQUESTS_G0__MASK__SNBEP_UNC_H_TAD_REQUESTS_G0__REGION4 = 0x1000, // Counts for TAD Region 4
		UNC_H_TAD_REQUESTS_G0__MASK__SNBEP_UNC_H_TAD_REQUESTS_G0__REGION5 = 0x2000, // Counts for TAD Region 5
		UNC_H_TAD_REQUESTS_G0__MASK__SNBEP_UNC_H_TAD_REQUESTS_G0__REGION6 = 0x4000, // Counts for TAD Region 6
		UNC_H_TAD_REQUESTS_G0__MASK__SNBEP_UNC_H_TAD_REQUESTS_G0__REGION7 = 0x8000, // Counts for TAD Region 7
		UNC_H_TAD_REQUESTS_G1 = 0x1c, // HA Requests to a TAD Region - Group 1
		UNC_H_TAD_REQUESTS_G1__MASK__SNBEP_UNC_H_TAD_REQUESTS_G1__REGION8 = 0x100, // Counts for TAD Region 8
		UNC_H_TAD_REQUESTS_G1__MASK__SNBEP_UNC_H_TAD_REQUESTS_G1__REGION9 = 0x200, // Counts for TAD Region 9
		UNC_H_TAD_REQUESTS_G1__MASK__SNBEP_UNC_H_TAD_REQUESTS_G1__REGION10 = 0x400, // Counts for TAD Region 10
		UNC_H_TAD_REQUESTS_G1__MASK__SNBEP_UNC_H_TAD_REQUESTS_G1__REGION11 = 0x800, // Counts for TAD Region 11
		UNC_H_TRACKER_INSERTS = 0x6, // Tracker Allocations
		UNC_H_TRACKER_INSERTS__MASK__SNBEP_UNC_H_TRACKER_INSERTS__ALL = 0x300, // Counts all requests
		UNC_H_TXR_AD = 0xf, // Outbound NDR Ring Transactions
		UNC_H_TXR_AD__MASK__SNBEP_UNC_H_TXR_AD__NDR = 0x100, // Counts non-data responses
		UNC_H_TXR_AD__MASK__SNBEP_UNC_H_TXR_AD__SNP = 0x200, // Counts outbound snoops send on the ring
		UNC_H_TXR_AD_CYCLES_FULL = 0x2a, // AD Egress Full
		UNC_H_TXR_AD_CYCLES_FULL__MASK__SNBEP_UNC_H_TXR_AD_CYCLES_FULL__ALL = 0x300, // Counts cycles full from both schedulers
		UNC_H_TXR_AD_CYCLES_FULL__MASK__SNBEP_UNC_H_TXR_AD_CYCLES_FULL__SCHED0 = 0x100, // Counts cycles full from scheduler bank 0
		UNC_H_TXR_AD_CYCLES_FULL__MASK__SNBEP_UNC_H_TXR_AD_CYCLES_FULL__SCHED1 = 0x200, // Counts cycles full from scheduler bank 1
		UNC_H_TXR_AK_CYCLES_FULL = 0x32, // AK Egress Full
		UNC_H_TXR_AK_CYCLES_FULL__MASK__SNBEP_UNC_H_TXR_AK_CYCLES_FULL__ALL = 0x300, // Counts cycles from both schedulers
		UNC_H_TXR_AK_CYCLES_FULL__MASK__SNBEP_UNC_H_TXR_AK_CYCLES_FULL__SCHED0 = 0x100, // Counts cycles from scheduler bank 0
		UNC_H_TXR_AK_CYCLES_FULL__MASK__SNBEP_UNC_H_TXR_AK_CYCLES_FULL__SCHED1 = 0x200, // Counts cycles from scheduler bank 1
		UNC_H_TXR_AK_NDR = 0xe, // Outbound NDR Ring Transactions
		UNC_H_TXR_BL = 0x10, // Outbound DRS Ring Transactions to Cache
		UNC_H_TXR_BL__MASK__SNBEP_UNC_H_TXR_BL__DRS_CACHE = 0x100, // Counts data being sent to the cache
		UNC_H_TXR_BL__MASK__SNBEP_UNC_H_TXR_BL__DRS_CORE = 0x200, // Counts data being sent directly to the requesting core
		UNC_H_TXR_BL__MASK__SNBEP_UNC_H_TXR_BL__DRS_QPI = 0x400, // Counts data being sent to a remote socket over QPI
		UNC_H_TXR_BL_CYCLES_FULL = 0x36, // BL Egress Full
		UNC_H_TXR_BL_CYCLES_FULL__MASK__SNBEP_UNC_H_TXR_AK_CYCLES_FULL__ALL = 0x300, // Counts cycles from both schedulers
		UNC_H_TXR_BL_CYCLES_FULL__MASK__SNBEP_UNC_H_TXR_AK_CYCLES_FULL__SCHED0 = 0x100, // Counts cycles from scheduler bank 0
		UNC_H_TXR_BL_CYCLES_FULL__MASK__SNBEP_UNC_H_TXR_AK_CYCLES_FULL__SCHED1 = 0x200, // Counts cycles from scheduler bank 1
		UNC_H_WPQ_CYCLES_NO_REG_CREDITS = 0x18, // HA iMC CHN0 WPQ Credits Empty - Regular
		UNC_H_WPQ_CYCLES_NO_REG_CREDITS__MASK__SNBEP_UNC_H_RPQ_CYCLES_NO_REG_CREDITS__CHN0 = 0x100, // Channel 0
		UNC_H_WPQ_CYCLES_NO_REG_CREDITS__MASK__SNBEP_UNC_H_RPQ_CYCLES_NO_REG_CREDITS__CHN1 = 0x200, // Channel 1
		UNC_H_WPQ_CYCLES_NO_REG_CREDITS__MASK__SNBEP_UNC_H_RPQ_CYCLES_NO_REG_CREDITS__CHN2 = 0x400, // channel 2
		UNC_H_WPQ_CYCLES_NO_REG_CREDITS__MASK__SNBEP_UNC_H_RPQ_CYCLES_NO_REG_CREDITS__CHN3 = 0x800, // Chanell 3
		UNC_H_ADDR_OPC_MATCH = 0x20, // QPI address/opcode match
		UNC_H_ADDR_OPC_MATCH__MASK__SNBEP_UNC_H_ADDR_OPC_MATCH__FILT = 0x300, // modifiers)
		
	};
};

namespace snbep_unc_h = optkit::intel::snbep_unc_h;