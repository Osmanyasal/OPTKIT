#pragma once

#include <cstdint>

/**
 * List of MSRs used in OPTKIT with official description from documentation
 **/

////////////////////////////////////////////////////////////////////////////////

/**
 * Intel 64 and IA-32 Architectures Software Developer's Manual
 * (Order Number: 325462-074US)
 *
 * IA32_ENERGY_PERF_BIAS register *********************************************
 * Performance Energy Bias Hint (R/W)
 * 0x1B0 0x000000000000000F
 * Power Policy Preference (bits 3:0):
    0 indicates preference to highest performance.
    15 indicates preference to maximize energy saving.
 * Reserved (bits 63:4)
 *
 * MSR_PLATFORM_INFO register *************************************************
 * Platform Information: Contains power management and other model specific
 * features enumeration. See http://biosbits.org.
 * 0xCE 0x0000FF003000FF00 - NHM, SDB, KNL, KNM
 * 0xCE 0x00FFFF073000FF00 - IVB, HSW, HSW-E
 * 0xCE 0x0000FF007080FF00 - IVB-E, SKL, CSL
 * 0xCE 0x00FFFF007080FF00 - BDW
 * Reserved (bits 7:0)
 * Maximum Non-Turbo Ratio (R/O) (bits 15:8):
    This is the ratio of the maximum frequency that does not require turbo.
    Frequency = ratio * Scalable Bus Frequency.
    SB Frequency at NHM 133.33 MHz, otherwise 100 MHz
 * Reserved (bits 22:16)
 * PPIN_CAP (R/O) (bit 23):
    Not available at NHM, SDB, IVB, HSW, HSW-E
    When set to 1, indicates that Protected Processor Inventory Number (PPIN)
    capability can be enabled for a privileged system inventory agent to read
    PPIN from MSR_PPIN. When set to 0, PPIN capability is not supported. An
    attempt to access MSR_PPIN_CTL or MSR_PPIN will cause #GP.
 * Reserved (bits 27:24)
 * Programmable Ratio Limit for Turbo Mode (R/O) (bit 28):
    When set to 1, indicates that Programmable Ratio Limit for Turbo mode is
    enabled. When set to 0, indicates Programmable Ratio Limit for Turbo mode is
    disabled.
 * Programmable TDP Limit for Turbo Mode (R/O) (bit 29):
    When set to 1, indicates that TDP Limit for Turbo mode is programmable. When
    set to 0, indicates TDP Limit for Turbo mode is not programmable.
 * Programmable TJ OFFSET (R/O) (bit 30):
    Not available at  NHM, SDB, IVB, HSW, HSW-E
    When set to 1, indicates that MSR_TEMPERATURE_TARGET.[27:24] is valid and
    writable to specify a temperature offset.
 * Reserved (bit 31)
 * Low Power Mode Support (LPM) (R/O) (bit 32):
    Not available at NHM, SDB, IVB, IVB-E, BDW, BDW-E, SKL, CSL
    When set to 1, indicates that LPM is supported. When set to 0, indicates LPM
    is not supported.
 * Number of ConfigTDP Levels (R/O) (bits 34:33):
    Not available at NHM, SDB, IVB-E, BDW, BDW-E, SKL, CSL
    00: Only Base TDP level available.
    01: One additional TDP level available.
    02: Two additional TDP level available.
    03: Reserved
 * Reserved (bits 39:35)
 * Maximum Efficiency Ratio (R/O) (bits 47:40):
    This is the minimum ratio (maximum efficiency) that the processor can
    operate, in units of 100MHz.
 * Minimum Operating Ratio (R/O) (bits 55:48):
    Not available at NHM, SDB, IVB-E, BDW, BDW-E, SKL, CSL
    Contains the minimum supported operating ratio in units of 100 MHz.
 * Reserved (bits 63:56)
 *
 * CPUID instruction - input EAX = 0x01 ***************************************
 * input EAX = 0x01
 * output EAX:
 * Steping ID (bits 3:0)
 * Model (bits 7:4)
 * Family ID (bits 11:8):
    Family (0FH for the Pentium 4 Processor Family)
 * Processor Type (bits 13:12):
    00 - Original OEM Processor
    01 - Intel OverDrive Processor
    10 - Dual processor (not applicable to Intel486 processors)
    11 - Intel reserved
 * Reserved (bits 15:14)
 * Extended Model ID (bits 19:16)
 * Extended Family ID (bits 27:20)
 * Reserved (bits 31:28)
 IF Family_ID =! 0x0F
    DisplayFamily = Family_ID;
 ELSE
    DisplayFamily = Extended_Family_ID + Family_ID;

 IF (Family_ID == 0x06 || Family_ID == 0x0F)
    DisplayModel = (Extended_Model_ID << 4) + Model_ID;
 ELSE
    DisplayModel = Model_ID;
 *
 * IA32_THERM_STATUS register *************************************************
 * The processor supports a digital thermal sensor if CPUID.06H.EAX[0] = 1. If
 * the processor supports digital thermal sensor, EBX[bits 3:0] determine the
 * number of thermal thresholds that are available for use.
 *
 * Software sets thermal thresholds by using the IA32_THERM_INTERRUPT MSR.
 * Software reads output of the digital thermal sensor using the
 * IA32_THERM_STATUS MSR.
 *
 * Unlike traditional analog thermal devices, the output of the digital thermal
 * sensor is a temperature relative to the maximum supported operating
 * temperature of the processor. Temperature measurements returned by digital
 * thermal sensors are always at or below Thermal Control Circuitry (TCC)
 * activation temperature. Critical temperature conditions are detected using
 * the "Critical Temperature Status" bit. When this bit is set, the processor is
 * operating at a critical temperature and immediate shutdown of the system
 * should occur. Once the "Critical Temperature Status" bit is set, reliable
 * operation is not guaranteed.
 * 0x19C 0x00000000F87F0FFF
 * Thermal Status (bit 0):
    This bit indicates whether the digital thermal sensor high-temperature
    output signal (PROCHOT#) is currently active. Bit 0 = 1 indicates the
    feature is active. This bit may not be written by software; it reflects the
    state of the digital thermal sensor.
 * Thermal Status Log (bit 1):
    This is a sticky bit that indicates the history of the thermal sensor high
    temperature output signal (PROCHOT#). Bit 1 = 1 if PROCHOT# has been
    asserted since a previous RESET or the last time software cleared the bit.
    Software may clear this bit by writing a zero.
 * PROCHOT# or FORCEPR# Event (bit 2):
    Indicates whether PROCHOT# or FORCEPR# is being asserted by another agent on
    the platform.
 * PROCHOT# or FORCEPR# Log (bit 3):
    Sticky bit that indicates whether PROCHOT# or FORCEPR# has been asserted by
    another agent on the platform since the last clearing of this bit or a
    reset. If bit 3 = 1, PROCHOT# or FORCEPR# has been externally asserted.
    Software may clear this bit by writing a zero. External PROCHOT# assertions
    are only acknowledged if the Bidirectional Prochot feature is enabled.
 * Critical Temperature Status (bit 4):
    Indicates whether the critical temperature detector output signal is
    currently active. If bit 4 = 1, the critical temperature detector output
    signal is currently active.
 * Critical Temperature Log (bit 5):
    Sticky bit that indicates whether the critical temperature detector output
    signal has been asserted since the last clearing of this bit or reset. If
    bit 5 = 1, the output signal has been asserted. Software may clear this bit
    by writing a zero.
 * Thermal Threshold #1 Status (bit 6):
    Indicates whether the actual temperature is currently higher than or equal
    to the value set in Thermal Threshold #1. If bit 6 = 0, the actual
    temperature is lower. If bit 6 = 1, the actual temperature is greater than
    or equal to TT#1. Quantitative information of actual temperature can be
    inferred from Digital Readout, bits 22:16.
 * Thermal Threshold #1 Log (bit 7):
    Sticky bit that indicates whether the Thermal Threshold #1 has been reached
    since the last clearing of this bit or a reset. If bit 7 = 1, the Threshold
    #1 has been reached. Software may clear this bit by writing a zero.
 * Thermal Threshold #2 Status (bit 8):
    Indicates whether actual temperature is currently higher than or equal to
    the value set in Thermal Threshold #2. If bit 8 = 0, the actual temperature
    is lower. If bit 8 = 1, the actual temperature is greater than or equal to
    TT#2. Quantitative information of actual temperature can be inferred from
    Digital Readout, bits 22:16.
 * Thermal Threshold #2 Log (bit 9):
    Sticky bit that indicates whether the Thermal Threshold #2 has been reached
    since the last clearing of this bit or a reset. If bit 9 = 1, the Thermal
    Threshold #2 has been reached. Software may clear this bit by writing a
    zero.
 * Power Limit Notification Status (bit 10):
    Indicates whether the processor is currently operating below OS-requested
    P-state (specified in IA32_PERF_CTL) or OS-requested clock modulation duty
    cycle (specified in IA32_CLOCK_MODULATION). This field is supported only
    if CPUID.06H:EAX[bit 4] = 1. Package level power limit notification can be
    delivered independently to IA32_PACKAGE_THERM_STATUS MSR.
 * Power Limit Notification Log (bit 11):
    Sticky bit that indicates the processor went below OS-requested P-state or
    OS-requested clock modulation duty cycle since the last clearing of this or
    RESET. This field is supported only if CPUID.06H:EAX[bit 4] = 1. Package
    level power limit notification is indicated independently in
    IA32_PACKAGE_THERM_STATUS MSR.
 * Current Limit Status (bit 12):
    If set (1), indicates an electrical current limit (e.g. Electrical Design
    Point/IccMax) is being exceeded and is adversely impacting energy efficiency
    optimizations.
 * Current Limit Log (bit 13):
    If set (1), an electrical current limit has been exceeded that has adversely
    impacted energy efficiency optimizations since the last clearing of this bit
    or a reset. This bit is sticky, software may clear this bit by writing a
    zero (0).
 * Cross-domain Limit Status (bit 14):
    If set (1), indicates another hardware domain (e.g. processor graphics) is
    currently limiting energy efficiency optimizations in the processor core
    domain.
 * Cross-domain Limit Log (bit 15):
    If set (1), indicates another hardware domain (e.g. processor graphics) has
    limited energy efficiency optimizations in the processor core domain since
    the last clearing of this bit or a reset. This bit is sticky, software may
    clear this bit by writing a zero (0).
 * Digital Readout (bits 22:16):
    Digital temperature reading in 1 degree Celsius relative to the TCC
    activation temperature.
    - 0: TCC Activation temperature,
    - 1: (TCC Activation - 1) , etc. See the processor's data sheet for details
    regarding TCC activation.
    A lower reading in the Digital Readout field (bits 22:16) indicates a higher
    actual temperature.
 * Reserved (bits 26:23)
 * Resolution in Deg. Celsius (bits 30:27):
    Specifies the resolution (or tolerance) of the digital thermal sensor. The
    value is in degrees Celsius. It is recommended that new threshold values be
    offset from the current temperature by at least the resolution + 1 in order
    to avoid hysteresis of interrupt generation.
 * Reading Valid (bit 31)
 * Reserved (bits 63:32):
    Indicates if the digital readout in bits 22:16 is valid.
    The readout is valid if bit 31 = 1.
 *
 * IA32_PACKAGE_THERM_STATUS register *****************************************
 * The thermal management facilities like IA32_THERM_INTERRUPT and
 * IA32_THERM_STATUS are often implemented with a processor core granularity. To
 * facilitate software manage thermal events from a package level granularity,
 * two architectural MSR is provided for package level thermal management. The
 * IA32_PACKAGE_THERM_STATUS and IA32_PACKAGE_THERM_INTERRUPT MSRs use similar
 * interfaces as IA32_THERM_STATUS and IA32_THERM_INTERRUPT, but are shared in
 * each physical processor package.
 * Software can enumerate the presence of the processor's support for package
 * level thermal management facility (IA32_PACKAGE_THERM_STATUS and
 * IA32_PACKAGE_THERM_INTERRUPT) by verifying CPUID.06H:EAX[bit 6] = 1.
 * 0x1B1 0x00000000007F0FFF
 * PKG Thermal Status (bit 0):
    This bit indicates whether the digital thermal sensor high-temperature
    output signal (PROCHOT#) for the package is currently active. Bit 0 = 1
    indicates the feature is active. This bit may not be written by software; it
    reflects the state of the digital thermal sensor.
 * PKG Thermal Status Log (bit 1):
    This is a sticky bit that indicates the history of the thermal sensor high
    temperature output signal (PROCHOT#) of the package. Bit 1 = 1 if package
    PROCHOT# has been asserted since a previous RESET or the last time software
    cleared the bit. Software may clear this bit by writing a zero.
 * PKG PROCHOT# or FORCEPR# Event (bit 2):
    Indicates whether package PROCHOT# is being asserted by another agent on the
    platform.
 * PKG PROCHOT# or FORCEPR# Log (bit 3):
    Sticky bit that indicates whether package PROCHOT# has been asserted by
    another agent on the platform since the last clearing of this bit or a
    reset. If bit 3 = 1, package PROCHOT# has been externally asserted. Software
    may clear this bit by writing a zero.
 * PKG Critical Temperature Status (bit 4):
    Indicates whether the package critical temperature detector output signal is
    currently active. If bit 4 = 1, the package critical temperature detector
    output signal is currently active.
 * PKG Critical Temperature Log (bit 5):
    Sticky bit that indicates whether the package critical temperature detector
    output signal has been asserted since the last clearing of this bit or
    reset. If bit 5 = 1, the output signal has been asserted. Software may clear
    this bit by writing a zero.
 * PKG Thermal Threshold #1 Status (bit 6):
    Indicates whether the actual package temperature is currently higher than or
    equal to the value set in Package Thermal Threshold #1. If bit 6 = 0, the
    actual temperature is lower. If bit 6 = 1, the actual temperature is greater
    than or equal to PTT#1. Quantitative information of actual package
    temperature can be inferred from Package Digital Readout, bits 22:16.
 * PKG Thermal Threshold #1 Log (bit 7):
    Sticky bit that indicates whether the Package Thermal Threshold #1 has been
    reached since the last clearing of this bit or a reset. If bit 7 = 1, the
    Package Threshold #1 has been reached. Software may clear this bit by
    writing a zero.
 * PKG Thermal Threshold #2 Status (bit 8):
    Indicates whether actual package temperature is currently higher than or
    equal to the value set in Package Thermal Threshold #2. If bit 8 = 0, the
    actual temperature is lower. If bit 8 = 1, the actual temperature is greater
    than or equal to PTT#2. Quantitative information of actual temperature can
    be inferred from Package Digital Readout, bits 22:16.
 * PKG Thermal Threshold #2 Log (bit 9):
    Sticky bit that indicates whether the Package Thermal Threshold #2 has been
    reached since the last clearing of this bit or a reset. If bit 9 = 1, the
    Package Thermal Threshold #2 has been reached. Software may clear this bit
    by writing a zero.
 * PKG Power Limit Notification Status (bit 10):
    Indicates package power limit is forcing one ore more processors to operate
    below OS-requested P-state. Note that package power limit violation may be
    caused by processor cores or by devices residing in the uncore. Software can
    examine IA32_THERM_STATUS to determine if the cause originates from a
    processor core.
 * PKG Power Limit Notification Log (bit 11):
    Sticky bit that indicates any processor in the package went below
    OS-requested P-state or OS-requested clock modulation duty cycle since the
    last clearing of this or RESET.
 * Reserved (bits 15:12)
 * PKG Digital Readout (bits 22:16):
    Package digital temperature reading in 1 degree Celsius relative to the
    package TCC activation temperature.
    - 0: Package TCC Activation temperature,
    - 1: (PTCC Activation - 1) , etc. See the processor's data sheet for details
    regarding PTCC activation.
    A lower reading in the Package Digital Readout field (bits 22:16) indicates
    a higher actual temperature.
 * Reserved (bits 63:23)
 ******************************************************************************
 **/

#ifdef _INTEL_
#ifndef IA32_ENERGY_PERF_BIAS
#define IA32_ENERGY_PERF_BIAS 0x1B0
#endif

#ifndef MSR_PLATFORM_INFO
#define MSR_PLATFORM_INFO 0xCE
#endif

#ifndef IA32_THERM_STATUS
#define IA32_THERM_STATUS 0x19C
#endif

#ifndef IA32_PACKAGE_THERM_STATUS
#define IA32_PACKAGE_THERM_STATUS 0x1B1
#endif
#endif

const uint64_t MSR_PLATFORM_INFO_nominal_freq_mask = 0xFF00;
const uint64_t MSR_PLATFORM_INFO_nominal_freq_shift = 8;
const uint64_t MSR_PLATFORM_INFO_prog_turbo_freq_mask = 0x10000000;
const uint64_t MSR_PLATFORM_INFO_prog_turbo_freq_shift = 28;

const uint64_t CPUID_EAX_01_stepping_id_mask = 0xF;
const uint64_t CPUID_EAX_01_model_mask = 0xF0;
const uint64_t CPUID_EAX_01_model_shift = 4;
const uint64_t CPUID_EAX_01_family_id_mask = 0xF00;
const uint64_t CPUID_EAX_01_family_id_shift = 8;
const uint64_t CPUID_EAX_01_processor_type_mask = 0x3000;
const uint64_t CPUID_EAX_01_processor_type_shift = 12;
const uint64_t CPUID_EAX_01_extended_model_id_mask = 0xF0000;
const uint64_t CPUID_EAX_01_extended_model_id_shift = 16;
const uint64_t CPUID_EAX_01_extended_family_id_mask = 0xFF00000;
const uint64_t CPUID_EAX_01_extended_family_id_shift = 20;
const uint64_t CPUID_EAX_01_display_model_shift = 4;

const uint64_t IA32_THERM_STATUS_readout_mask = 0x7F0000;
const uint64_t IA32_THERM_STATUS_readout_shift = 16;

const uint64_t IA32_PACKAGE_THERM_STATUS_readout_mask = 0x7F0000;
const uint64_t IA32_PACKAGE_THERM_STATUS_readout_shift = 16;

// CORE FREQUENCY //////////////////////////////////////////////////////////////
/**
 * Intel 64 and IA-32 Architectures Software Developer's Manual
 * (Order Number: 325462-074US)
 *
 * IA32_PERF_CTL register *****************************************************
 * Performance Control MSR (R/W) Software makes a request for a new Performance
 * state (P-State) by writing this MSR.
 * State transitions are initiated by writing a 16-bit value to the
 * IA32_PERF_CTL register, see Figure 14-2. If a transition is already in
 * progress, transition to a new value will subsequently take effect. Reads of
 * IA32_PERF_CTL determine the last targeted operating point. The current
 * operating point can be read from IA32_PERF_STATUS. IA32_PERF_STATUS is
 * updated dynamically. The 16-bit encoding that defines valid operating points
 * is model-specific. Applications and performance tools are not expected to use
 * either IA32_PERF_CTL or IA32_PERF_STATUS and should treat both as reserved.
 * Performance monitoring tools can access model-specific events and report the
 * occurrences of state transitions.
 * 0x199 0x000000010000FFFF
 * Target performance State Value (bits 15:0)
 * Reserved (bits 31:16)
 * IDA Engage (R/W) (bit 32):
    When set to 1: disengages IDA.
 * Reserved (bits 63:33)
 *
 * MSR_TURBO_RATIO_LIMIT register *********************************************
 * Before the SKL architecture:
 * Maximum Ratio Limit of Turbo Mode
 *  - RO if MSR_PLATFORM_INFO.[28] = 0,
 *  - RW if MSR_PLATFORM_INFO.[28] = 1
 * 0x1AD 0xFFFFFFFFFFFFFFFF
 * Maximum Ratio Limit for 1C (bits 7:0):
    Maximum turbo ratio limit of 1 core active.
 * Every following 8 bits correspondes to another acrtive core. If the number of
 * cores is higher than 8, the specification continues to following registers:
 * - MSR_TURBO_RATIO_LIMIT1 (0x1AE)
 * - MSR_TURBO_RATIO_LIMIT2 (0x1AF)
 * - MSR_TURBO_RATIO_LIMIT3 (0x1AC)
 * Some architectures has also:
 * Semaphore for Turbo Ratio Limit Configuration (bit 63):
    If 1, the processor uses override configuration (An override configuration
    lower than the factory-set configuration is always supported. An override
    configuration higher than the factory-set configuration is dependent on
    features specific to the processor and the platform.) specified in
    MSR_TURBO_RATIO_LIMIT*.
    If 0, the processor uses factory-set configuration (Default).
 *
 * From the SKL architecture:
 * This register defines the ratio limits. RATIO[0:7] must be populated in
 * ascending order. RATIO[i+1] must be less than or equal to RATIO[i]. Entries
 * with RATIO[i] will be ignored. If any of the rules above are broken, the
 * configuration is silently rejected. If the programmed ratio is:
 *	- Above the fused ratio for that core count, it will be clipped to the fuse
 * limits (assuming !OC).
 *  - Below the min supported ratio, it will be clipped.
 * 0xAD 0xFFFFFFFFFFFFFFFF
 * RATIO_0 (bits 7:0):
    Defines ratio limits.
 * RATIO_1 (bits 15:8):
    Defines ratio limits.
 * RATIO_2 (bits 23:16):
    Defines ratio limits.
 * RATIO_3 (bits 31:24):
    Defines ratio limits.
 * RATIO_4 (bits 39:32):
    Defines ratio limits.
 * RATIO_5 (bits 47:40):
    Defines ratio limits.
 * RATIO_6 (bits 55:41):
    Defines ratio limits.
 * RATIO_7 (bits 63:56):
    Defines ratio limits.
 *
 * MSR_TURBO_RATIO_LIMIT_CORES register ***************************************
 * From the SKL architecture - replaces MSR_TURBO_RATIO_LIMIT1+ registers
 * This register defines the active core ranges for each frequency point.
 * NUMCORE[0:7] must be populated in ascending order. NUMCORE[i+1] must be
 * greater than NUMCORE[i]. Entries with NUMCORE[i] == 0 will be ignored. The
 * last valid entry must have NUMCORE >=the number of cores in the SKU. If any
 * of the rules above are broken, the configuration is silently rejected.
 * 0x1AE 0xFFFFFFFFFFFFFFFF
 * NUMCORE_0 (bits 7:0):
    Defines the active core ranges for each frequency point.
 * NUMCORE_1 (bits 15:8):
    Defines the active core ranges for each frequency point.
 * NUMCORE_2 (bits 23:16):
    Defines the active core ranges for each frequency point.
 * NUMCORE_3 (bits 31:24):
    Defines the active core ranges for each frequency point.
 * NUMCORE_4 (bits 39:32):
    Defines the active core ranges for each frequency point.
 * NUMCORE_5 (bits 47:40):
    Defines the active core ranges for each frequency point.
 * NUMCORE_6 (bits 55:41):
    Defines the active core ranges for each frequency point.
 * NUMCORE_7 (bits 63:56):
    Defines the active core ranges for each frequency point.
 *
 * IA32_MPERF register ********************************************************
 * TSC Frequency Clock Counter (R/Write to clear)
 * 0xE7 0xFFFFFFFFFFFFFFFF
 * C0_MCNT: C0 TSC Frequency Clock Count (bits 63:0):
    Increments at fixed interval (relative to TSC freq.) when the logical
    processor is in C0. Cleared upon overflow / wrap-around of IA32_APERF.
 *
 * IA32_APERF register ********************************************************
 * Actual Performance Clock Counter (R/Write to clear)
 * 0xE8 0xFFFFFFFFFFFFFFFF
 * C0_ACNT: C0 Actual Frequency Clock Count (bits 63:0):
    Accumulates core clock counts at the coordinated clock frequency, when the
    logical processor is in C0. Cleared upon overflow / wrap-around of
    IA32_MPERF.
 *
 * IA32_PM_ENABLE register ****************************************************
 * Enable/Disable HWP, If CPUID.06H:EAX.[7] = 1, package level MSR
 * 0x770 0x0000000000000001
 * HWP_ENABLE (R/W1-Once) (bit 0):
    Software sets this bit to enable HWP with autonomous selection of processor
    P-States. When set, the processor will disregard input from the legacy
    performance control interface (IA32_PERF_CTL). Note this bit can only be
    enabled once from the default value. Once set, writes to the HWP_ENABLE bit
    are ignored. Only RESET will clear this bit. Default = zero (0).
 * Reserved (bits 63:1)
 ******************************************************************************
 **/

/**
 * Open-Source Register Reference For AMD Family 17h Processors Models 00h-2Fh
 * (56255 Rev 3.03)
 *
 * MSR_PSTATE_CUR_LIM register ************************************************
 * P-state Current Limit
 * 0xC0010061 0x0000000000000077 Core::X86::Msr::PStateCurLim
 * CurPstateLimit: current P-state limit (bits 2:0):
    Specifies the highest performance P-state (lowest value) allowed.
    CurPstateLimit is always bounded by
    Core::X86::Msr::PStateCurLim[PstateMaxVal]. Attempts to change the
    CurPstateLimit to a value greater (lower performance) than
    Core::X86::Msr::PStateCurLim[PstateMaxVal] leaves CurPstateLimit unchanged.
 * Reserved (bit 3)
 * PstateMaxVal: P-state maximum value (bits 6:4):
    Specifies the lowest performance non-boosted P-state (highest non-boosted
    value) allowed. Attempts to change Core::X86::Msr::PStateCtl[PstateCmd] to a
    lower-performance P-state (higher value) are clipped to the value of this
    field.
 * Reserved (bits 63:7)
 *
 * MSR_PSTATE_CTL register ****************************************************
 * P-state Control
 * 0xC0010062 0x0000000000000007 Core::X86::Msr::PStateCtl
 * PstateCmd: P-state change command (bits 2:0):
    Writes to this field cause the core to change to the indicated non-boosted
    P-state number, specified by Core::X86::Msr::PStateDef. 0=P0, 1=P1, etc.
    P-state limits are applied to any P-state requests made through this
    register. Reads from this field return the last written value, regardless of
    whether any limits are applied.
 * Reserved (bits 63:3)
 *
 * MSR_PSTATE_DEF_P[0-7] register *********************************************
 * P-state [7:0]
 * Each of these registers specify the frequency and voltage associated with
 * each of the core P-states. The CpuVid field in these registers is required to
 * be programmed to the same value in all cores of a processor, but are allowed
 * to be different between processors in a multi-processor system. All other
 * fields in these registers are required to be programmed to the same value in
 * each core of the coherent fabric.
 * 0xC0010064 - 0xC001006B 0x80000000FFFFFFFF Core::X86::Msr::PStateDef
 * CpuFid[7:0]: core frequency ID (bits 7:0):
    Specifies the core frequency multiplier. The core COF (Current operating
    frequency of a given clock domain) is a function of CpuFid (FID = Frequency
    Identifier; specifies the PLL frequency multiplier for a given clock domain)
    and CpuDid (DID = Divisor Identifier; specifies the post-PLL divisor used to
    reduce the COF), and defined by CoreCOF (Core current operating frequency in
    MHz. CoreCOF = (Core::X86::Msr::PStateDef[CpuFid[7:0]]/
    Core::X86::Msr::PStateDef[CpuDfsId])*200. A nominal frequency reduction can
    occur if spread spectrum clocking is enabled.).
    Valid values:
     - 0x0F - 0x00 = Reserved
     - 0xFF - 0x10 = Value * 25
 * CpuDfsId: core divisor ID (bits 13:8):
    Specifies the core frequency divisor; see Core::X86::Msr::PStateDef[CpuFid].
    For values [0x1A:0x08], 1/8th integer divide steps supported down to
    VCO/3.25 (Note, L3/L2 FIFO logic related to 4-cycle data heads-up requires
    core to be 1/3 of L3 frequency or higher). For values [0x30:0x1C], 1/4th
    integer divide steps supported down to VCO/6 (DID[0] should zero
    if DID[5:0] > 1Ah). (Note, core and L3 frequencies below 400MHz are not
    supported by the architecture). Core supports DID up to 0x30, but L3 must be
    0x2C (VCO/5.5) or less.
    Valid values:
     - 0x00 = Off
     - 0x07 - 0x01 = Reserved
     - 0x08 = VC0/1
     - 0x09 = VCO/1.125
     - 0x1A - 0x0A = VCO/(Value/8)
     - 0x1B = Reserved
     - 0x1C = VCO/(Value/8)
     - 0x1D = Reserved
     - 0x1E = VCO/(Value/8)
     - 0x1F = Reserved
     - 0x20 = VCO/(Value/8)
     - 0x21 = Reserved
     - 0x22 = VCO/(Value/8)
     - 0x23 = Reserved
     - 0x24 = VCO/(Value/8)
     - 0x25 = Reserved
     - 0x26 = VCO/(Value/8)
     - 0x27 = Reserved
     - 0x28 = VCO/(Value/8)
     - 0x29 = Reserved
     - 0x2A = VCO/(Value/8)
     - 0x2B = Reserved
     - 0x2C = VCO/(Value/8)
     - 0x3F - 0x2D - Reserved
 * CpuVid[7:0]: core VID (bits 21:14):
    See Core::X86::Msr::PStateDef register description.
    Voltage level identifier (VID).
 * IddValue: current value (bits 29:22):
    IddDiv and IddValue combine to specify the expected maximum current
    dissipation of a single core that is in the P-state corresponding to the MSR
    number. These values are intended to be used to create ACPI-defined _PSS
    objects. The values are expressed in amps; they are not intended to convey
    final product power levels; they may not match the power levels specified
    in the Power and Thermal Datasheets.
 * IddDiv: current divisor (bits 31:30):
    See Core::X86::Msr::PStateDef[IddValue]
 * Reserved (bits 62:32)
 * PstateEn: P-State enable (bit 63):
    0 = The P-state specified by this MSR is not valid.
    1 = The P-state specified by this MSR is valid. Only then the rest of the
    P-state information in the register is valid after a reset; it controls no
    hardware.
 *
 * MSR_HWCR register **********************************************************
 * Hardware configuration
 * 0xC0010015 0x000000024F366799 Core::X86::Msr::HWCR
 * SmmLock: SMM code lock (bit 0):
    1 = SMM code in the ASeg and TSeg range and the SMM registers are Read-only
    and SMI interrupts are not intercepted in SVM.
 * Reserved (bits 2:1)
 * TlbCacheDis: TLB cache dissable (bit 3):
    1 = Disable performance improvement that assumes that the PML4, PDP, PDE and
    PTE entries are in cacheable WB DRAM.
    Operating systems that maintain page tables in any other memory type must
    set the TlbCacheDis bit to insure proper operation. TlbCacheDis does not
    override the memory type specified by the SMM ASeg and TSeg memory regions
    controlled by Core::X86::Msr::SMMAddr Core::X86::Msr::SMMMask.
 * INVDWBINVD: INVD to WBINVD conversion (bit 4):
    1 = Convert INVD to WBINVD.
    This bit is required to be set for normal operation when any of the
    following are true:
     - An L2 is shared by multiple threads.
     - An L3 is shared by multiple cores.
     - CC6 is enabled.
     - Probe filter is enabled.
 * Reserved (bits 6:5)
 * Allow FERR on NE (bit 7):
    0 = Disable legacy FERR signaling and generate FERR exception directly.
    1 = Legacy FERR signaling.
 * IgnneEm: IGNNE port emulation enable (bit 8):
    1 = Enable emulation of IGNNE port.
 * MonMwaitDis: MONITOR and MWAIT disable (bit 9):
    1 = The MONITOR, MWAIT, MONITORX, and MWAITX opcodes become invalid.
    This affects what is reported back through
    Core::X86::Cpuid::FeatureIdEcx[Monitor] and
    Core::X86::Cpuid::FeatureExtIdEcx[MwaitExtended].
 * MonMwaitUserEn: MONITOR/MWAIT user mode enable (bit 10):
    0 = The MONITOR and MWAIT instructions are supported only in privilege level
    0; these instructions in privilege levels 1 to 3 cause a #UD exception.
    1 = The MONITOR and MWAIT instructions are supported in all privilege
    levels.
    The state of this bit is ignored if MonMwaitDis is set.
 * Reserved (bits 12:11)
 * SmiSpCycDis: SMI special bus cycle disable (bit 13):
    0=A link special bus cycle, SMIACK, is generated when an SMI interrupt is
    taken.
    AccessType: Core::X86::Msr::HWCR[SmmLock] ? Read-only : Read-write.
 * RsmSpCycDis: RSM special bus cycle disable (bit 14):
    0=A link special bus cycle, SMIACK, is generated on a resume from SMI.
    AccessType: Core::X86::Msr::HWCR[SmmLock] ? Read-only : Read-write.
 * Reserved (bits 16:15)
 * Wrap32Dis: 32-bit address wrap disable (bit 17):
    1 = Disable 32-bit address wrapping.
    Software can use Wrap32Dis to access physical memory above 4 Gbytes without
    switching into 64-bit mode. To do so, software should Write a greater-than
    4-Gbyte address to Core::X86::Msr::FS_BASE and Core::X86::Msr::GS_BASE. Then
    it would address +/-2 Gbytes from one of those bases using normal memory
    reference instructions with a FS or GS override prefix. However, the INVLPG,
    FST, and SSE store instructions generate 32-bit addresses in legacy mode,
    regardless of the state of Wrap32Dis.
 * McStatusWrEn: Machine check status write enable (bit 18):
    0 = MCA_STATUS registers are readable; writing a non-zero pattern to these
    registers causes a general protection fault.
    1 = MCA_STATUS registers are Read-write, including Reserved fields; do not
    cause general protection faults; such Writes update all implemented bits in
    these registers; All fields of all threshold registers are Read-write when
    accessed from MSR space, including Locked, except BlkPtr which is always
    Read-only;
    Does not change the access type for the thresholding registers accessed via
    configuration space.
    Can be used to debug machine check exception and interrupt handlers.
    Independent of the value of this bit, the processor may enforce
    Write-Ignored behavior on MCA_STATUS registers depending on platform
    settings.
 * Reserved (bit 19)
 * IoCfgGpFault: IO-space configuration causes a GP fault (bit 20):
    1=IO-space accesses to configuration space cause a GP fault. The fault is
    triggered if any part of the IO Read/Rrite address range is between CF8h and
    CFFh, inclusive. These faults only result from single IO instructions, not
    to string and REP IO instructions. This fault takes priority over the IO
    trap mechanism described by Core::X86::Msr::SMI_ON_IO_TRAP_CTL_STS.
 * LockTscToCurrentP0: Lock the TSC to the current P0 frequency (bit 21):
    0 = The TSC will count at the P0 frequency.
    1 = The TSC frequency is locked to the current P0 frequency at the time this
    bit is set and remains fixed regardless of future changes to the P0
    frequency.
 * Reserved (bits 23:22)
 * TscFreqSel: TSC frequency select (bit 24):
    1 = The TSC increments at the P0 frequency.
 * CpbDis: Core performance boost disable (bit 25):
    1 = Boost is disabled.
    If core performance boost is disabled while a core is in a boosted P-state,
    the core automatically transitions to the highest performance non-boosted
    P-state.
 * EffFreqCntMwait: Effective frequency counting during mwait (bit 26):
    0 = The registers do not increment.
    1 = The registers increment.
    Specifies whether Core::X86::Msr::MPERF and Core::X86::Msr::APERF increment
    while the core is in the monitor event pending state.
 * EffFreqReadOnlyLock: Read-only effective frequency counter lock (bit 27):
    1 = Core::X86::Msr::MPerfReadOnly, Core::X86::Msr::APerfReadOnly and
    Core::X86::Msr::IRPerfCount are Read-only.
 * Reserved (bits 29:28)
 * IRPerfEn: Instruction retired counter enable (bit 30):
    1 = enable
 * Reserved (bits 32:31)
 * SmmPgCFGLock: SMM page config lock (bit 33):
    1 = SMM page config locked. Error-on-write-1 if not in SMM mode.
    RSM unconditionally clears Core::X86::Msr::HWCR[SmmPgCfgLock].
 * Reserved (bits 63:34)
 *
 * IA32_MPERF register ********************************************************
 * Description same as Intel
 * 0xE7 0xFFFFFFFFFFFFFFFF Core::X86::Msr::MPERF
 *
 * IA32_APERF register ********************************************************
 * Description same as Intel
 * 0xE8 0xFFFFFFFFFFFFFFFF Core::X86::Msr::APERF
 *
 ******************************************************************************
 **/

const uint64_t IA32_PERF_CTL_shift = 8;
const uint64_t MSR_TURBO_RATIO_LIMIT_shift = 8;
const uint64_t MSR_TURBO_RATIO_LIMIT_mask = 0xFF;
const uint64_t IA32_PM_ENABLE_mask = 1;

const uint64_t MSR_PSTATE_CUR_LIM_curr_limit_pstate_mask = 0x7;
const uint64_t MSR_PSTATE_CUR_LIM_max_pstate_shift = 4;
const uint64_t MSR_PSTATE_CUR_LIM_max_pstate_mask = 0x70;
const uint64_t MSR_PSTATE_CTL_mask = 0x7;
const uint64_t MSR_PSTATE_DEF_Px_frequency_id_mask = 0xFF;
const uint64_t MSR_PSTATE_DEF_Px_divisor_shift = 8;
const uint64_t MSR_PSTATE_DEF_Px_divisor_mask = 0x3F00;
// const uint64_t MSR_PSTATE_DEF_Px_vid_shift = 14;
// const uint64_t MSR_PSTATE_DEF_Px_vid_mask = 0x3FC000;
// const uint64_t MSR_PSTATE_DEF_Px_idd_value_shift = 22;
// const uint64_t MSR_PSTATE_DEF_Px_idd_value_mask = 0x3FC00000;
// const uint64_t MSR_PSTATE_DEF_Px_idd_divisor_shift = 30;
// const uint64_t MSR_PSTATE_DEF_Px_idd_divisor_mask = 0xC0000000;
const uint64_t MSR_PSTATE_DEF_Px_pstate_enable_shift = 63;
const uint64_t MSR_PSTATE_DEF_Px_pstate_enable_mask = 0x8000000000000000;
const uint64_t MSR_HWCR_boost_disable_shift = 25;
const uint64_t MSR_HWCR_boost_disable_mask = 0x2000000;

#ifdef _INTEL_
#ifndef IA32_PERF_CTL
#define IA32_PERF_CTL 0x199
#endif

#ifndef MSR_TURBO_RATIO_LIMIT
#define MSR_TURBO_RATIO_LIMIT 0x1AD
#endif

#ifndef MSR_TURBO_RATIO_LIMIT_CORES
#define MSR_TURBO_RATIO_LIMIT_CORES 0x1AE
#endif

#ifndef IA32_MPERF
#define IA32_MPERF 0xE7
#endif

#ifndef IA32_APERF
#define IA32_APERF 0xE8
#endif

#ifndef IA32_PM_ENABLE
#define IA32_PM_ENABLE 0x770
#endif
#endif

#ifdef _AMD_
#ifndef MSR_PSTATE_CUR_LIM
#define MSR_PSTATE_CUR_LIM 0xC0010061
#endif

#ifndef MSR_PSTATE_CTL
#define MSR_PSTATE_CTL 0xC0010062
#endif

#ifndef MSR_PSTATE_DEF_P0
#define MSR_PSTATE_DEF_P0 0xC0010064
#endif

#ifndef MSR_PSTATE_DEF_P1
#define MSR_PSTATE_DEF_P1 0xC0010065
#endif

#ifndef MSR_PSTATE_DEF_P2
#define MSR_PSTATE_DEF_P2 0xC0010066
#endif

#ifndef MSR_PSTATE_DEF_P3
#define MSR_PSTATE_DEF_P3 0xC0010067
#endif

#ifndef MSR_PSTATE_DEF_P4
#define MSR_PSTATE_DEF_P4 0xC0010068
#endif

#ifndef MSR_PSTATE_DEF_P5
#define MSR_PSTATE_DEF_P5 0xC0010069
#endif

#ifndef MSR_PSTATE_DEF_P6
#define MSR_PSTATE_DEF_P6 0xC001006A
#endif

#ifndef MSR_PSTATE_DEF_P7
#define MSR_PSTATE_DEF_P7 0xC001006B
#endif

#ifndef MSR_HWCR
#define MSR_HWCR 0xC0010015
#endif

#ifndef IA32_MPERF
#define IA32_MPERF 0xE7
#endif

#ifndef IA32_APERF
#define IA32_APERF 0xE8
#endif
#endif

// UNCORE FREQUENCY ////////////////////////////////////////////////////////////
/**
 * Intel 64 and IA-32 Architectures Software Developer's Manual
 * Volume 4: Model-Specific Registers (Order Number: 335592-064US)
 *
 * MSR_UNCORE_RATIO_LIMIT register ********************************************
 * Uncore Ratio Limit (R/W)
 * Out of reset, the min_ratio and max_ratio fields represent the widest
 * possible range of uncore frequencies. Writing to these fields allows software
 * to control the minimum and the maximum frequency that hardware will select.
 * 0x620 0x0000000000007F7F
 * MAX_RATIO (bits 6:0):
    This field is used to limit the max ratio of the LLC/Ring.
 * Reserved (bit 7)
 * MIN_RATIO (bits 14:8):
    Writing to this field controls the minimum possible ratio of the LLC/Ring.
 * Reserved (bits 63:15)
 *
 **/
/**
 * Intel Xeon Processor E5 v2 and E7 v2 Product Families Uncore Performance
 * Monitoring Reference Manual (Reference Number: 329468-002)
 * and
 * Intel Xeon Processor E5 and E7 v3 Family Uncore Performance Monitoring
 * Reference Manual (Document Number: 331051-002)
 *
 * The following registers represent state governing all PMUs in the uncore,
 * both to exert global control and collect box-level information.
 *
 * U_MSR_PMON_GLOBAL_CTL register *********************************************
 * UBox PMON Global Control (32 bit register) Contains a bit that can freeze
 * (.frz_all) all the uncore counters. If an overflow is detected in any of the
 * uncore's PMON registers, it will be summarized in U_MSR_PMON_GLOBAL_STATUS.
 * 0x700 0xE0007FFF since HSW
 * 0xC00 0xE0007FFF IVB EP and SDB EP chips
 * PMI Core Select (bits 14:0):
    Ex:
    If counter overflow is sent to UBox...
    000000000000000 - No PMI sent
    000000000000001 - Send PMI to core 0
    000000001000000 - Send PMI to core 6
    000000001100010 - Send PMI to core 2, 5 & 6
    etc.
    NOTE: If wk_on_pmi is set to 1, a wake will be sent to any sleeping core in
    the mask prior to sending the PMI.
 * Reserved (bits 26:15)
 * Reserved (bits 28:27)
 * unfrz_all (bit 29):
    Unfreeze all uncore performance monitors.
 * wk_on_pmi (bit 30):
    If PMI event requested to send to core...
    0 - Send event to cores already woken
    1 - Wake any sleeping core and send PMI to all cores.
 * frz_all (bit 31):
    Freeze all uncore performance monitors.
 *
 * U_MSR_PMON_GLOBAL_STATUS register ******************************************
 * This register accumulates overflows sent to it from the other uncore boxes.
 * To reset these overflow bits, a user must set the corresponding bits in
 * U_MSR_PMON_GLOBAL_STATUS to 1, which will act to clear them.
 * (32 bit register)
 * 0x701 0x07FFFFFF since HSW
 * 0xC01 0x07FFFFFF IVB EP and SDB EP chips
 * ov_u_fixed (bit 0):
    Set if overflow is detected from UBox fixed PMON register.
    NOTE: Write of '1' will clear the bit.
 * ov_u (bit 1):
    Set if overflow is detected from a UBox PMON register.
    NOTE: Write of '1' will clear the bit.
 * ov_p (bit 2):
    Set if overflow is detected from a PCU PMON register.
    NOTE: Write of '1' will clear the bit.
 * ov_c[14-0] (bits 17:3):
    Set if overflow is detected from a CBo PMON register, 1 bit for each CBo
    where bit 5 corresponds CBo 0, etc.
    NOTE: Write of '1' will clear the bit.
 * ov_h0 (bit 18):
    Set if overflow is detected from an HA0 PMON register.
    NOTE: Write of '1' will clear the bit.
 * ov_h1 (bit 19):
    Set if overflow is detected from an HA1 PMON register.
    NOTE: Write of '1' will clear the bit.
 * ov_m0 (bit 20):
    Set if overflow is detected from an iMC0 PMON register.
    NOTE: Write of '1' will clear the bit.
 * ov_m1 (bit 21):
    Set if overflow is detected from an iMC1 PMON register.
    NOTE: Write of '1' will clear the bit.
 * ov_q0 (bit 22):
    Set if overflow is detected from a QPI0 PMON register.
    NOTE: Write of '1' will clear the bit.
 * ov_q1 (bit 23):
    Set if overflow is detected from a QPI1 PMON register.
    NOTE: Write of '1' will clear the bit.
 * ov_rq0 (bit 24):
    Set if overflow is detected from an R3QPI0 PMON register.
    NOTE: Write of '1' will clear the bit.
 * ov_rq1 (bit 25):
    Set if overflow is detected from an R3QPI1 PMON register.
    NOTE: Write of '1' will clear the bit.
 * ov_rp (bit 26):
    Set if overflow is detected from an R2PCIe PMON register.
    NOTE: Write of '1' will clear the bit.
 * Reserved (bits 31:27)
 *
 * U_MSR_PMON_FIXED_CTL register **********************************************
 * U-Box PMON UCLK Fixed Counter Control (32 bit register)
 * 0x703 0x00500000 since HSW
 * 0xC08 0x00500000 IVB EP and SDB EP chips
 * Reserved (bits 19:0)
 * ov_en (bit 20):
    When this bit is set to 1 and the corresponding counter overflows, a UBox
    overflow message is sent to the UBox's global logic. Once received, the
    global status register will record the overflow in
    U_MSR_PMON_GLOBAL_STATUS.ov_u_fixed.
 * Reserved (bit 21)
 * Enable (bit 22):
    Enable counter when global enable is set.
 * Reserved (bits 31:23)
 *
 * U_MSR_PMON_FIXED_CTR register **********************************************
 * U-Box PMON UCLK Fixed Counter
 * 0x704 0x0000FFFFFFFFFFFF since HSW
 * 0xC09 0x0000FFFFFFFFFFFF IVB EP and SDB EP chips
 * Event count (bits 47:0):
    48-bit performance event counter
 * Reserved (bits 63:48)
 ******************************************************************************
 **/
const uint64_t MSR_UNCORE_RATIO_LIMIT_max_mask = 0x7F;
const uint64_t MSR_UNCORE_RATIO_LIMIT_min_mask = 0x7F00;
const uint64_t MSR_UNCORE_RATIO_LIMIT_min_shift = 8;

const uint64_t MSR_UNCORE_CURRENT_RATIO_mask = 0x7F;

const uint64_t U_MSR_PMON_FIXED_CTL_shift = 22;

// #ifdef _INTEL_ // TODO: change enviroment later!!
#ifndef MSR_UNCORE_RATIO_LIMIT
#define MSR_UNCORE_RATIO_LIMIT 0x620
#endif

//	#ifndef MSR_UNCORE_CURRENT_RATIO //UNCORE_PERF_STATUS - not in use in OPTKIT
//		#define MSR_UNCORE_CURRENT_RATIO	0x621
//	#endif

#ifndef U_MSR_PMON_FIXED_CTR
#define U_MSR_PMON_FIXED_CTR 0x704 // since HSW
//	#define U_MSR_PMON_FIXED_CTR	0xC09	//IVB EP and SDB EP chips
#endif

#ifndef U_MSR_PMON_FIXED_CTL
#define U_MSR_PMON_FIXED_CTL 0x703 // since HSW
//	#define U_MSR_PMON_FIXED_CTL	0xC08	//IVB EP and SDB EP chips
#endif
// #endif

// RAPL POWER-CAP //////////////////////////////////////////////////////////////
/**
 * RAPL power domains:
    PKG      - CPU core domain (Intel)
    DRAM     - memory channels (server Intel)
    Platform - SoC (vendor specific, Intel)
 **/
/**
 * Intel 64 and IA-32 Architectures Software Developer's Manual
 * (Order Number: 325462-074US)
 *
 * MSR_PKG_POWER_LIMIT register ***********************************************
 * Register allows a software agent to define power limitation for the package
 * domain. Power limitation is defined in terms of average power usage (Watts)
 * over a time window specified in MSR_PKG_POWER_LIMIT. Two power limits can be
 * specified, corresponding to time windows of different sizes. Each power limit
 * provides independent clamping control that would permit the processor cores
 * to go below OS-requested state to meet the power limits. A lock mechanism
 * allow the software agent to enforce power limit settings. Once the lock bit
 * is set, the power limit settings are static and unmodifiable until next RESET
 * 0x610 0x00FFFFFF00FFFFFF
 * Package Power Limit #1 (bits 14:0):
    Sets the average power usage limit of the package domain corresponding
    to time window # 1. The unit of this field is specified by the "Power Units"
    field of MSR_RAPL_POWER_UNIT
 * Enable Power Limit #1 (bit 15):
    0 = disabled; 1 = enabled.
 * Package Clamping Limitation #1 (bit 16):
    0 = disabled; 1 = enabled.
    Allow going below OS-requested P/T state setting during time window
    specified by bits 23:17.
 * Time Window for Power Limit #1 (bits 23:17):
    Indicates the time window for power limit #1
    Time limit = 2^Y * (1.0 + Z/4.0) * Time_Unit
    Here "Y" is the unsigned integer value represented. by bits 21:17,
    "Z" is an unsigned integer represented by bits 23:22.
    "Time_Unit" is specified by the "Time Units" field of MSR_RAPL_POWER_UNIT.
 * Reserved (bits 31:24)
 * Package Power Limit #2 (bits 46:32):
    Sets the average power usage limit of the package domain corre-sponding
    to time window # 2. The unit of this field is specified by the "Power Units"
    field of MSR_RAPL_POWER_UNIT.
 * Enable Power Limit #2 (bit 47):
    0 = disabled; 1 = enabled.
 * Package Clamping Limitation #2 (bit 48):
    0 = disabled; 1 = enabled.
    Allow going below OS-requested P/T state setting during time window
    specified by bits 23:17.
 * Time Window for Power Limit #2 (bits 55:49):
    Indicates the time window for power limit #2
    Time limit = 2^Y * (1.0 + Z/4.0) * Time_Unit
    Here "Y" is the unsigned integer value represented. by bits 53:49,
    "Z" is an unsigned integer represented by bits 55:54.
    "Time_Unit" is specified by the "Time Units" field of MSR_RAPL_POWER_UNIT.
    This field may have a hard-coded value in hardware and ignores values
    written by software.
 * Reserved (bits 62:56)
 * Lock (bit 63):
    If set, all write attempts to this MSR are ignored until next RESET.
 *
 * MSR_DRAM_POWER_LIMIT register **********************************************
 * allows a software agent to define power limitation for the DRAM domain. Power
 * limitation is defined in terms of average power usage (Watts) over a time
 * window specified in MSR_DRAM_POWER_LIMIT. A power limit can be specified
 * along with a time window. A lock mechanism allow the software agent to
 * enforce power limit settings. Once the lock bit is set, the power limit
 * settings are static and unmodifiable until next RESET
 * 0x618 0x0000000000FFFFFF
 * DRAM Power Limit #1(bits 14:0):
    Sets the average power usage limit of the DRAM domain corresponding to time
    window # 1. The unit of this field is specified by the "Power Units" field
    of MSR_RAPL_POWER_UNIT.
 * Enable Power Limit #1(bit 15):
    0 = disabled; 1 = enabled.
 * Claping limit (bit 16):
    0 = disabled; 1 = enabled.
    Allow going below OS-requested P/T state setting during time window
    specified by bits 23:17.
 * Time Window for Power Limit (bits 23:17):
    Indicates the length of time window over which the power limit will be used
    by the processor. The numeric value encoded by bits 23:17 is represented by
    the product of 2^Y *F; where F is a single-digit decimal floating-point
    value between 1.0 and 1.3 with the fraction digit represented by bits 23:22,
    Y is an unsigned integer represented by bits 21:17. The unit of this field
    is specified by the "Time Units" field of MSR_RAPL_POWER_UNIT.
 * Reserved (bits 30:24)
 * Lock (bit 31):
    If set, all write attempts to this MSR are ignored until next RESET.
 * Reserved (bits 63:32)
 *
 * MSR_PKG_POWER_INFO/MSR_DRAM_POWER_INFO register ****************************
 * Read-only MSR. It reports the package/DRAM power range information for RAPL
 * usage. This MSR provides maximum/minimum values (derived from electrical
 * specification), thermal specification power of the package/DRAM domain. It
 * also provides the largest possible time window for software to program the
 * RAPL interface.
 * 0x614 0x003F7FFF7FFF7FFF MSR_PKG_POWER_INFO
 * 0x61C 0x003F7FFF7FFF7FFF MSR_DRAM_POWER_INFO
 * Thermal Spec Power (bits 14:0):
    The unsigned integer value is the equivalent of thermal specification power
    of the package/DRAM domain. The unit of this field is specified by the
    "Power Units" field of MSR_RAPL_POWER_UNIT.
 * Reserved (bit 15)
 * Minimum Power (bits 30:16):
    The unsigned integer value is the equivalent of minimum power derived from
    electrical spec of the package/DRAM domain. The unit of this field is
    specified by the "Power Units" field of MSR_RAPL_POWER_UNIT.
 * Reserved (bit 31)
 * Maximum Power (bits 46:32):
    The unsigned integer value is the equivalent of maximum power derived from
    the electrical spec of the package/DRAM domain. The unit of this field is
    specified by the "Power Units" field of MSR_RAPL_POWER_UNIT.
 * Reserved (bit 47)
 * Maximum Time Window (bits 53:48):
    The unsigned integer value is the equivalent of largest acceptable value to
    program the time window of MSR_PKG_POWER_LIMIT/MSR_DRAM_POWER_LIMIT. The
    unit of this field is specified by the "Time Units" field of
    MSR_RAPL_POWER_UNIT.
 * Reserved (bits 63:54)
 *
 * MSR_PKG_PERF_STATUS/MSR_DRAM_PERF_STATUS register **************************
 * Read-only MSR. It reports the total time for which the package was throttled
 * due to the RAPL power limits. Throttling in this context is defined as going
 * below the OS-requested P-state or T-state. It has a wrap-around time of many
 * hours. The availability of this MSR is platform specific (see Chapter 2,
 * "Model-Specific Registers (MSRs)" in the Intel® 64 and IA-32 Architectures
 * Software Developer's Manual, Volume 4).
 * 0x613 0x00000000FFFFFFFF MSR_PKG_PERF_STATUS
 * 0x61B 0x00000000FFFFFFFF MSR_DRAM_PERF_STATUS
 * Accumulated Package Throttled Time (bits 31:0):
    The unsigned integer value represents the cumulative time (since the last
    time this register is cleared) that the package/DRAM domain has throttled.
    The unit of this field is specified by the "Time Units" field of
    MSR_RAPL_POWER_UNIT.
 * Reserved (bits 63:32)
 *
 * MSR_PLATFORM_POWER_LIMIT register ******************************************
 * Additional MSRs Supported by 6th Generation Intel Core Processors and the
 * Intel Xeon Processor Scalable Family Based on Skylake Microarchitecture, 7th
 * Generation Intel Core Processors Based on Kaby Lake Microarchitecture, 8th
 * Generation and 9th Generation Intel Core Processors and Intel Xeon E
 * Processors Based on Coffee Lake Microarchitecture, and 8th Generation Intel
 * Core i3 Processors Based on Cannon Lake Microarchitecture
 *
 * The notation of "Platform" in the Scope column (with respect to
 * MSR_PLATFORM_ENERGY_COUNTER and MSR_PLATFORM_POWER_LIMIT) is limited to the
 * power-delivery domain and the specifics of the power delivery integration may
 * vary by platform vendor's implementation.
 *
 * Platform Power Limit Control (R/W-L) - platform scope
 * Allows platform BIOS to limit power consumption of the platform devices to
 * the specified values. The Long Duration power consumption is specified via
 * Platform_Power_Limit_1 and Platform_Power_Limit_1_Time. The Short Duration
 * power consumption limit is specified via the Platform_Power_Limit_2 with
 * duration chosen by the processor. The processor implements an
 * exponential-weighted algorithm in the placement of the time windows.
 * 0x65C 0x8001FFFF00FFFFFF
 * Platform Power Limit #1 (bits 14:0):
    Average Power limit value which the platform must not exceed over a time
    window as specified by Power_Limit_1_TIME field. The default value is the
    Thermal Design Power (TDP) and varies with product skus. The unit is
    specified in MSR_RAPL_POWER_UNIT.
 * Enable Platform Power Limit #1 (bit 15):
    When set, enables the processor to apply control policy such that the
    platform power does not exceed Platform Power limit #1 over the time window
    specified by Power Limit #1 Time Window.
 * Platform Clamping Limitation #1 (bit 16):
    When set, allows the processor to go below the OS requested P states in
    order to maintain the power below specified Platform Power Limit #1 value.
    This bit is writeable only when CPUID (EAX=6):EAX[4] is set.
 * Time Window for Platform Power Limit #1 (bits 23:17):
    Specifies the duration of the time window over which Platform Power Limit #1
    value should be maintained for sustained int64_t duration. This field is made
    up of two numbers from the following equation:
        Time Window = (float) ((1+(X/4))*(2^Y)), where:
        X = POWER_LIMIT_1_TIME[23:22]
        Y = POWER_LIMIT_1_TIME[21:17]
    The maximum allowed value in this field is defined in
    MSR_PKG_POWER_INFO[PKG_MAX_WIN]. The default value is 0xD, The unit is
    specified in MSR_RAPL_POWER_UNIT[Time Unit].
 * Reserved (bits 31:24)
 * Platform Power Limit #2 (bits 46:32):
    Average Power limit value which the platform must not exceed over the Short
    Duration time window chosen by the processor. The recommended default value
    is 1.25 times the Long Duration Power Limit (i.e., Platform Power Limit #1).
 * Enable Platform Power Limit #2 (bit 47):
    When set, enables the processor to apply control policy such that the
    platform power does not exceed Platform Power limit #2 over the Short
    Duration time window.
 * Platform Clamping Limitation #2 (bit 48):
    When set, allows the processor to go below the OS requested P states in
    order to maintain the power below specified Platform Power Limit #2 value.
 * Reserved (bits 62:49)
 * Lock (bit 63):
    Setting this bit will lock all other bits of this MSR until system RESET.
 ******************************************************************************
 **/
const uint64_t MSR_DRAM_POWER_LIMIT_mask = 0x7FFF;
const uint64_t MSR_DRAM_POWER_LIMIT_enable_mask = 0x8000;
const uint64_t MSR_DRAM_POWER_LIMIT_enable_shift = 15;
const uint64_t MSR_DRAM_POWER_LIMIT_clamping_mask = 0x10000;
const uint64_t MSR_DRAM_POWER_LIMIT_clamping_shift = 16;
const uint64_t MSR_DRAM_POWER_LIMIT_time_f_mask = 0xC00000;
const uint64_t MSR_DRAM_POWER_LIMIT_time_y_mask = 0x3E0000;
const uint64_t MSR_DRAM_POWER_LIMIT_time_f_shift = 22;
const uint64_t MSR_DRAM_POWER_LIMIT_time_y_shift = 17;

const uint64_t MSR_PKG_POWER_LIMIT_mask = 0x7FFF;
const uint64_t MSR_PKG_POWER_LIMIT_shift = 0;
const uint64_t MSR_PKG_POWER_LIMIT_enable_mask = 0x8000;
const uint64_t MSR_PKG_POWER_LIMIT_enable_shift = 15;
const uint64_t MSR_PKG_POWER_LIMIT_clamping_mask = 0x10000;
const uint64_t MSR_PKG_POWER_LIMIT_clamping_shift = 16;
const uint64_t MSR_PKG_POWER_LIMIT_time_y_mask = 0x3E0000;
const uint64_t MSR_PKG_POWER_LIMIT_time_y_shift = 17;
const uint64_t MSR_PKG_POWER_LIMIT_time_z_mask = 0xC00000;
const uint64_t MSR_PKG_POWER_LIMIT_time_z_shift = 22;

const uint64_t MSR_PKG_POWER_LIMIT_power1_shift = 0;
const uint64_t MSR_PKG_POWER_LIMIT_power2_shift = 32; // use "half register-length" shift instead of specifying all the mask and shifts for the second half of the register

const uint64_t MSR_PKG_POWER_LIMIT_lock_mask = 0x8000000000000000;
const uint64_t MSR_DRAM_POWER_LIMIT_lock_mask = 0x80000000;

const uint64_t MSR_PKG_DRAM_POWER_INFO_min_power_mask = 0x7FFF0000;
const uint64_t MSR_PKG_DRAM_POWER_INFO_min_power_shift = 16;
const uint64_t MSR_PKG_DRAM_POWER_INFO_max_power_mask = 0x7FF700000000;
const uint64_t MSR_PKG_DRAM_POWER_INFO_max_power_shift = 32;
const uint64_t MSR_PKG_DRAM_POWER_INFO_max_time_mask = 0x3F000000000000;
const uint64_t MSR_PKG_DRAM_POWER_INFO_max_time_shift = 48;

const uint64_t MSR_PLATFORM_POWER_LIMIT_mask = 0x8001FFFF00FFFFFF;
const uint64_t MSR_PLATFORM_POWER_LIMIT_shift = 0;

const uint64_t MSR_PLATFORM_POWER_LIMIT_power1_shift = 0;
const uint64_t MSR_PLATFORM_POWER_LIMIT_power2_shift = 32; // use "half register-length" shift instead of specifying all the mask and shifts for the second half of the register

const uint64_t MSR_PLATFORM_POWER_LIMIT_power_mask = 0x7FFF;
const uint64_t MSR_PLATFORM_POWER_LIMIT_enable_mask = 0x8000;
const uint64_t MSR_PLATFORM_POWER_LIMIT_enable_shift = 15;
const uint64_t MSR_PLATFORM_POWER_LIMIT_clamping_mask = 0x10000;
const uint64_t MSR_PLATFORM_POWER_LIMIT_clamping_shift = 16;

const uint64_t MSR_PLATFORM_POWER_LIMIT_time_x_mask = 0xC00000;
const uint64_t MSR_PLATFORM_POWER_LIMIT_time_x_shift = 22;
const uint64_t MSR_PLATFORM_POWER_LIMIT_time_y_mask = 0x3E0000;
const uint64_t MSR_PLATFORM_POWER_LIMIT_time_y_shift = 17;
const uint64_t MSR_PLATFORM_POWER_LIMIT_lock_mask = 0x8000000000000000;
const uint64_t MSR_PLATFORM_POWER_LIMIT_lock_shift = 63;

// #ifdef _INTEL_
#ifndef MSR_PKG_POWER_LIMIT
#define MSR_PKG_POWER_LIMIT 0x610
#endif

#ifndef MSR_PKG_POWER_INFO
#define MSR_PKG_POWER_INFO 0x614
#endif

#ifndef MSR_PKG_PERF_STATUS
#define MSR_PKG_PERF_STATUS 0x613
#endif

#ifndef MSR_DRAM_POWER_LIMIT
#define MSR_DRAM_POWER_LIMIT 0x618
#endif

#ifndef MSR_DRAM_POWER_INFO
#define MSR_DRAM_POWER_INFO 0x61C
#endif

#ifndef MSR_DRAM_PERF_STATUS
#define MSR_DRAM_PERF_STATUS 0x61B
#endif

#ifndef MSR_PLATFORM_POWER_LIMIT
#define MSR_PLATFORM_POWER_LIMIT 0x65C
#endif
// #endif

// RAPL energy measurement /////////////////////////////////////////////////////
/**
 * RAPL power domains:
    PKG      - CPU core domain (Intel, AMD)
    PP0      - cores (client Intel)
    PP1      - integrated GPU (client Intel)
    DRAM     - memory channels (server Intel)
    Platform - SoC (vendor specific, Intel)
    Core     - core (AMD)
 **/
/**
 * Intel 64 and IA-32 Architectures Software Developer's Manual
 * (Order Number: 325462-074US)
 *
 * MSR_RAPL_POWER_UNIT register ***********************************************
 * Unit Multipliers used in RAPL Interfaces (R/O)
 * 0x606 0x00000000000F1F0F
 * Power Units (bits 3:0):
    Power related information (in Watts) is based on the multiplier, 1/2^PU;
    where PU is an unsigned integer represented by bits 3:0.
    Default value is 0011b, indicating power unit is in 1/8 Watts increment.
 * Reserved (bits 7:4)
 * Energy Status Units (bits 12:8):
    Energy related information (in Joules) is based on the multiplier, 1/2^ESU;
    where ESU is an unsigned integer represented by bits 12:8.
    Default value is 10000b, indicating energy status unit is 15.3uJ increment.
 * Reserved (bits 15:13)
 * Time Units (bits 19:16):
    Time related information (in Seconds) is based on the multiplier, 1/2^TU;
    where TU is an unsigned integer represented by bits 19:16.
    Default value is 1010b, indicating time unit is in 976us increment.
 * Reserved (bits 63:20)
 *
 * MSR_PKG_ENERGY_STATUS register *********************************************
 * 0x611 0x00000000FFFFFFFF
 * Total energy consumed (bits 31:0):
    The unsigned integer value represents the total amount of energy consumed
    since that last time this register is cleared. The unit of this field
    is specified by the "Energy Status Units" field of MSR_RAPL_POWER_UNIT.
 * Reserved (bits 63:32)
 *
 * MSR_PP0_ENERGY_STATUS/MSR_PP1_ENERGY_STATUS register ***********************
 * 0x639 0x00000000FFFFFFFF MSR_PP0_ENERGY_STATUS
 * 0x641 0x00000000FFFFFFFF MSR_PP1_ENERGY_STATUS
 * Total Energy Consumed (bits 31:0):
    The unsigned integer value represents the total amount of energy consumed
    since the last time this register was cleared. The unit of this field
    is specified by the "Energy Status Units" field of MSR_RAPL_POWER_UNIT.
 * Reserved (bits 63:32)
 *
 * MSR_DRAM_ENERGY_STATUS register ********************************************
 * 0x619 0x00000000FFFFFFFF
 * Total Energy Consumed (bits 31:0):
    The unsigned integer value represents the total amount of energy consumed
    since that last time this register is cleared. The unit of this field
    is specified by the "Energy Status Units" field of MSR_RAPL_POWER_UNIT.
 * Reserved (bits 63:32)
 *
 * MSR_PLATFORM_ENERGY_COUNTER register ***************************************
 * Additional MSRs Supported by 6th Generation Intel Core Processors and the
 * Intel Xeon Processor Scalable Family Based on Skylake Microarchitecture, 7th
 * Generation Intel Core Processors Based on Kaby Lake Microarchitecture, 8th
 * Generation and 9th Generation Intel Core Processors and Intel Xeon E
 * Processors Based on Coffee Lake Microarchitecture, and 8th Generation Intel
 * Core i3 Processors Based on Cannon Lake Microarchitecture
 *
 * The notation of "Platform" in the Scope column (with respect to
 * MSR_PLATFORM_ENERGY_COUNTER and MSR_PLATFORM_POWER_LIMIT) is limited to the
 * power-delivery domain and the specifics of the power delivery integration may
 * vary by platform vendor's implementation.
 *
 * Platform Energy Counter (R/O) - platform scope
 * This MSR is valid only if both platform vendor hardware implementation and
 * BIOS enablement support it. This MSR will read 0 if not valid.
 * 0x64D 0x00000000FFFFFFFF
 * Total energy consumed (bits 31:0):
    Total energy consumed by all devices in the platform that receive power from
    integrated power delivery mechanism, included platform devices are processor
    cores, SOC, memory, add-on or peripheral devices that get powered directly
    from the platform power delivery means. The energy units are specified in
    the MSR_RAPL_POWER_UNIT.Enery_Status_Unit.
 * Reserved (bits 63:32)
 ******************************************************************************
 **/

/**
 * Open-Source Register Reference For AMD Family 17h Processors Models 00h-2Fh
 * (56255 Rev 3.03)
 *
 * MSR_RAPL_POWER_UNIT register ***********************************************
 * Description same as Intel
 * 0xC0010299 0x00000000000F1F0F Core::X86::Msr::RAPL_PWR_UNIT
 * MSR_PKG_ENERGY_STATUS register *********************************************
 * Description same as Intel
 * 0xC001029B 0x00000000FFFFFFFF Core::X86::Msr::PKG_ENERGY_STAT
 * MSR_CORE_ENERGY_STATUS register ********************************************
 * 0xC001029A 0x00000000FFFFFFFF Core::X86::Msr::CORE_ENERGY_STAT
 * Total energy consumed (bits 31:0)
 * Reserved (bits 63:32)
 **/

const uint64_t MSR_RAPL_POWER_UNIT_power_mask = 0xF;
const uint64_t MSR_RAPL_POWER_UNIT_energy_mask = 0x1F00;
const uint64_t MSR_RAPL_POWER_UNIT_energy_shift = 8;
const uint64_t MSR_RAPL_POWER_UNIT_time_mask = 0xF0000;
const uint64_t MSR_RAPL_POWER_UNIT_time_shift = 16;
// const uint64_t MSR_PKG_DRAM_PPx_ENERGY_STATUS_mask = 0xFFFFFFFF;
// const uint64_t MSR_PKG_ENERGY_STATUS_mask    = 0xFFFFFFFF;
// const uint64_t MSR_DRAM_ENERGY_STATUS_mask    = 0xFFFFFFFF;
// const uint64_t MSR_PP0_ENERGY_STATUS_mask     = 0xFFFFFFFF;
// const uint64_t MSR_PP1_ENERGY_STATUS_mask     = 0xFFFFFFFF;
// const uint64_t MSR_PLATFORM_ENERGY_COUNTER_mask = 0xFFFFFFFF;
// const uint64_t MSR_CORE_ENERGY_STATUS_mask    = 0xFFFFFFFF;
const uint64_t MSR_ANYDOMAIN_ENERGY_STATUS_mask = 0xFFFFFFFF;

#ifdef _INTEL_

#ifndef MSR_RAPL_POWER_UNIT
#define MSR_RAPL_POWER_UNIT 0x606
#endif

#ifndef MSR_PKG_ENERGY_STATUS
#define MSR_PKG_ENERGY_STATUS 0x611
#endif

//	#ifndef MSR_PP0_ENERGY_STATUS
//		#define MSR_PP0_ENERGY_STATUS 	0x639
//	#endif

//	#ifndef MSR_PP1_ENERGY_STATUS
//		#define MSR_PP1_ENERGY_STATUS 	0x641
//	#endif

#ifndef MSR_DRAM_ENERGY_STATUS
#define MSR_DRAM_ENERGY_STATUS 0x619
#endif

#ifndef MSR_PLATFORM_ENERGY_COUNTER
#define MSR_PLATFORM_ENERGY_COUNTER 0x64D
#endif
#endif

#ifdef _AMD_

#ifndef MSR_RAPL_POWER_UNIT
#define MSR_RAPL_POWER_UNIT 0xC0010299
#endif

#ifndef MSR_PKG_ENERGY_STATUS
#define MSR_PKG_ENERGY_STATUS 0xC001029B
#endif

#ifndef MSR_CORE_ENERGY_STATUS
#define MSR_CORE_ENERGY_STATUS 0xC001029A
#endif

#endif

////////////////////////////////////////////////////////////////////////////////

const std::map<uint64_t, uint64_t> msr_whitelist = {
#ifdef IA32_ENERGY_PERF_BIAS
    {IA32_ENERGY_PERF_BIAS, 0x000000000000000F},
#endif
#ifdef MSR_PLATFORM_INFO
    {MSR_PLATFORM_INFO, 0x0000FF007080FF00}, // MSR_PLATFORM_INFO/PLATFORM_INFO_ADDR
#endif
#ifdef IA32_THERM_STATUS
    {IA32_THERM_STATUS, 0x00000000F87F0FFF},
#endif
#ifdef IA32_PACKAGE_THERM_STATUS
    {IA32_PACKAGE_THERM_STATUS, 0x00000000007F0FFF},
#endif
#ifdef IA32_PERF_CTL
    {IA32_PERF_CTL, 0x000000010000FFFF},
#endif
#ifdef MSR_TURBO_RATIO_LIMIT
    {MSR_TURBO_RATIO_LIMIT, 0xFFFFFFFFFFFFFFFF},
#endif
#ifdef MSR_TURBO_RATIO_LIMIT_CORES
    {MSR_TURBO_RATIO_LIMIT_CORES, 0xFFFFFFFFFFFFFFFF}, // equals to MSR_TURBO_RATIO_LIMIT1
#endif
#ifdef IA32_MPERF
    {IA32_MPERF, 0xFFFFFFFFFFFFFFFF},
#endif
#ifdef IA32_APERF
    {IA32_APERF, 0xFFFFFFFFFFFFFFFF},
#endif
#ifdef IA32_PM_ENABLE
    {IA32_PM_ENABLE, 0x0000000000000001},
#endif
#ifdef MSR_PSTATE_CUR_LIM
    {MSR_PSTATE_CUR_LIM, 0x0000000000000077},
#endif
#ifdef MSR_PSTATE_CTL
    {MSR_PSTATE_CTL, 0x0000000000000007},
#endif
#ifdef MSR_PSTATE_DEF_P0
    {MSR_PSTATE_DEF_P0, 0x80000000FFFFFFFF},
#endif
#ifdef MSR_PSTATE_DEF_P1
    {MSR_PSTATE_DEF_P1, 0x80000000FFFFFFFF},
#endif
#ifdef MSR_PSTATE_DEF_P2
    {MSR_PSTATE_DEF_P2, 0x80000000FFFFFFFF},
#endif
#ifdef MSR_PSTATE_DEF_P3
    {MSR_PSTATE_DEF_P3, 0x80000000FFFFFFFF},
#endif
#ifdef MSR_PSTATE_DEF_P4
    {MSR_PSTATE_DEF_P4, 0x80000000FFFFFFFF},
#endif
#ifdef MSR_PSTATE_DEF_P5
    {MSR_PSTATE_DEF_P5, 0x80000000FFFFFFFF},
#endif
#ifdef MSR_PSTATE_DEF_P6
    {MSR_PSTATE_DEF_P6, 0x80000000FFFFFFFF},
#endif
#ifdef MSR_PSTATE_DEF_P7
    {MSR_PSTATE_DEF_P7, 0x80000000FFFFFFFF},
#endif
#ifdef MSR_HWCR
    {MSR_HWCR, 0x000000024F366799},
#endif
#ifdef MSR_UNCORE_RATIO_LIMIT
    {MSR_UNCORE_RATIO_LIMIT, 0x0000000000007F7F},
#endif
#ifdef MSR_UNCORE_CURRENT_RATIO
    {MSR_UNCORE_CURRENT_RATIO, 0x000000000000007F},
#endif
#ifdef U_MSR_PMON_FIXED_CTR
    {U_MSR_PMON_FIXED_CTR, 0x0000FFFFFFFFFFFF}, // U_MSR_PMON_FIXED_CTR/MSR_U_PMON_UCLK_FIXED_CTR
#endif
#ifdef U_MSR_PMON_FIXED_CTL
    {U_MSR_PMON_FIXED_CTL, 0x00500000}, // U_MSR_PMON_FIXED_CTL/MSR_U_PMON_UCLK_FIXED_CTL
#endif
#ifdef MSR_PKG_POWER_LIMIT
    {MSR_PKG_POWER_LIMIT, 0x00FFFFFF00FFFFFF},
#endif
#ifdef MSR_PKG_POWER_INFO
    {MSR_PKG_POWER_INFO, 0x003F7FFF7FFF7FFF},
#endif
#ifdef MSR_PKG_PERF_STATUS
    {MSR_PKG_PERF_STATUS, 0x00000000FFFFFFFF},
#endif
#ifdef MSR_DRAM_POWER_LIMIT
    {MSR_DRAM_POWER_LIMIT, 0x0000000000FFFFFF},
#endif
#ifdef MSR_DRAM_POWER_INFO
    {MSR_DRAM_POWER_INFO, 0x003F7FFF7FFF7FFF},
#endif
#ifdef MSR_DRAM_PERF_STATUS
    {MSR_DRAM_PERF_STATUS, 0x00000000FFFFFFFF},
#endif
#ifdef MSR_PLATFORM_POWER_LIMIT
    {MSR_PLATFORM_POWER_LIMIT, 0x8001FFFF00FFFFFF},
#endif
#ifdef MSR_RAPL_POWER_UNIT
    {MSR_RAPL_POWER_UNIT, 0x00000000000F1F0F},
#endif
#ifdef MSR_PKG_ENERGY_STATUS
    {MSR_PKG_ENERGY_STATUS, 0x00000000FFFFFFFF},
#endif
#ifdef MSR_DRAM_ENERGY_STATUS
    {MSR_DRAM_ENERGY_STATUS, 0x00000000FFFFFFFF},
#endif
#ifdef MSR_PP0_ENERGY_STATUS
    {MSR_PP0_ENERGY_STATUS, 0x00000000FFFFFFFF},
#endif
#ifdef MSR_PP1_ENERGY_STATUS
    {MSR_PP1_ENERGY_STATUS, 0x00000000FFFFFFFF},
#endif
#ifdef MSR_PLATFORM_ENERGY_COUNTER
    {MSR_PLATFORM_ENERGY_COUNTER, 0x00000000FFFFFFFF},
#endif
#ifdef MSR_CORE_ENERGY_STATUS
    {MSR_CORE_ENERGY_STATUS, 0x00000000FFFFFFFF}
#endif

};

