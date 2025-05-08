#include "core/profiling/pmu/libpfm4_wrapper.hh"
namespace optkit::core::pmu
{
    const char *pmu_names[] = {
        "NONE",             /* no PMU */
        "GEN_IA64",         /* Intel IA-64 architected PMU */
        "ITANIUM",          /* Intel Itanium   */
        "ITANIUM2",         /* Intel Itanium 2 */
        "MONTECITO",        /* Intel Dual-Core Itanium 2 9000 */
        "AMD64",            /* AMD AMD64 (obsolete) */
        "I386_P6",          /* Intel PIII (P6 core) */
        "INTEL_NETBURST",   /* Intel Netburst (Pentium 4) */
        "INTEL_NETBURST_P", /* Intel Netburst Prescott (Pentium 4) */
        "COREDUO",          /* Intel Core Duo/Core Solo */
        "I386_PM",          /* Intel Pentium M */
        "INTEL_CORE",       /* Intel Core */
        "INTEL_PPRO",       /* Intel Pentium Pro */
        "INTEL_PII",        /* Intel Pentium II */
        "INTEL_ATOM",       /* Intel Atom */
        "INTEL_NHM",        /* Intel Nehalem core PMU */
        "INTEL_NHM_EX",     /* Intel Nehalem-EX core PMU */
        "INTEL_NHM_UNC",    /* Intel Nehalem uncore PMU */
        "INTEL_X86_ARCH",   /* Intel X86 architectural PMU */

        "MIPS_20KC",   /* MIPS 20KC */
        "MIPS_24K",    /* MIPS 24K */
        "MIPS_25KF",   /* MIPS 25KF */
        "MIPS_34K",    /* MIPS 34K */
        "MIPS_5KC",    /* MIPS 5KC */
        "MIPS_74K",    /* MIPS 74K */
        "MIPS_R10000", /* MIPS R10000 */
        "MIPS_R12000", /* MIPS R12000 */
        "MIPS_RM7000", /* MIPS RM7000 */
        "MIPS_RM9000", /* MIPS RM9000 */
        "MIPS_SB1",    /* MIPS SB1/SB1A */
        "MIPS_VR5432", /* MIPS VR5432 */
        "MIPS_VR5500", /* MIPS VR5500 */
        "MIPS_ICE9A",  /* SiCortex ICE9A */
        "MIPS_ICE9B",  /* SiCortex ICE9B */
        "POWERPC",     /* POWERPC */
        "CELL",        /* IBM CELL */

        "SPARC_ULTRA12",    /* UltraSPARC I, II, IIi, and IIe */
        "SPARC_ULTRA3",     /* UltraSPARC III */
        "SPARC_ULTRA3I",    /* UltraSPARC IIIi and IIIi+ */
        "SPARC_ULTRA3PLUS", /* UltraSPARC III+ and IV */
        "SPARC_ULTRA4PLUS", /* UltraSPARC IV+ */
        "SPARC_NIAGARA1",   /* Niagara-1 */
        "SPARC_NIAGARA2",   /* Niagara-2 */

        "PPC970",   /* IBM PowerPC 970(FX",GX) */
        "PPC970MP", /* IBM PowerPC 970MP */
        "POWER3",   /* IBM POWER3 */
        "POWER4",   /* IBM POWER4 */
        "POWER5",   /* IBM POWER5 */
        "POWER5p",  /* IBM POWER5+ */
        "POWER6",   /* IBM POWER6 */
        "POWER7",   /* IBM POWER7 */

        "PERF_EVENT",    /* perf_event PMU */
        "INTEL_WSM",     /* Intel Westmere single-socket (Clarkdale) */
        "INTEL_WSM_DP",  /* Intel Westmere dual-socket (Westmere-EP", Gulftwon) */
        "INTEL_WSM_UNC", /* Intel Westmere uncore PMU */

        "AMD64_K7",               /* AMD AMD64 K7 */
        "AMD64_K8_REVB",          /* AMD AMD64 K8 RevB */
        "AMD64_K8_REVC",          /* AMD AMD64 K8 RevC */
        "AMD64_K8_REVD",          /* AMD AMD64 K8 RevD */
        "AMD64_K8_REVE",          /* AMD AMD64 K8 RevE */
        "AMD64_K8_REVF",          /* AMD AMD64 K8 RevF */
        "AMD64_K8_REVG",          /* AMD AMD64 K8 RevG */
        "AMD64_FAM10H_BARCELONA", /* AMD AMD64 Fam10h Barcelona RevB */
        "AMD64_FAM10H_SHANGHAI",  /* AMD AMD64 Fam10h Shanghai RevC  */
        "AMD64_FAM10H_ISTANBUL",  /* AMD AMD64 Fam10h Istanbul RevD  */

        "ARM_CORTEX_A8", /* ARM Cortex A8 */
        "ARM_CORTEX_A9", /* ARM Cortex A9 */

        "TORRENT", /* IBM Torrent hub chip */

        "INTEL_SNB",               /* Intel Sandy Bridge (single socket) */
        "AMD64_FAM14H_BOBCAT",     /* AMD AMD64 Fam14h Bobcat */
        "AMD64_FAM15H_INTERLAGOS", /* AMD AMD64 Fam15h Interlagos */

        "INTEL_SNB_EP",        /* Intel SandyBridge EP */
        "AMD64_FAM12H_LLANO",  /* AMD AMD64 Fam12h Llano */
        "AMD64_FAM11H_TURION", /* AMD AMD64 Fam11h Turion */
        "INTEL_IVB",           /* Intel IvyBridge */
        "ARM_CORTEX_A15",      /* ARM Cortex A15 */

        "INTEL_SNB_UNC_CB0", /* Intel SandyBridge C-box 0 uncore PMU */
        "INTEL_SNB_UNC_CB1", /* Intel SandyBridge C-box 1 uncore PMU */
        "INTEL_SNB_UNC_CB2", /* Intel SandyBridge C-box 2 uncore PMU */
        "INTEL_SNB_UNC_CB3", /* Intel SandyBridge C-box 3 uncore PMU */

        "INTEL_SNBEP_UNC_CB0",    /* Intel SandyBridge-EP C-Box core 0 uncore */
        "INTEL_SNBEP_UNC_CB1",    /* Intel SandyBridge-EP C-Box core 1 uncore */
        "INTEL_SNBEP_UNC_CB2",    /* Intel SandyBridge-EP C-Box core 2 uncore */
        "INTEL_SNBEP_UNC_CB3",    /* Intel SandyBridge-EP C-Box core 3 uncore */
        "INTEL_SNBEP_UNC_CB4",    /* Intel SandyBridge-EP C-Box core 4 uncore */
        "INTEL_SNBEP_UNC_CB5",    /* Intel SandyBridge-EP C-Box core 5 uncore */
        "INTEL_SNBEP_UNC_CB6",    /* Intel SandyBridge-EP C-Box core 6 uncore */
        "INTEL_SNBEP_UNC_CB7",    /* Intel SandyBridge-EP C-Box core 7 uncore */
        "INTEL_SNBEP_UNC_HA",     /* Intel SandyBridge-EP HA uncore */
        "INTEL_SNBEP_UNC_IMC0",   /* Intel SandyBridge-EP IMC socket 0 uncore */
        "INTEL_SNBEP_UNC_IMC1",   /* Intel SandyBridge-EP IMC socket 1 uncore */
        "INTEL_SNBEP_UNC_IMC2",   /* Intel SandyBridge-EP IMC socket 2 uncore */
        "INTEL_SNBEP_UNC_IMC3",   /* Intel SandyBridge-EP IMC socket 3 uncore */
        "INTEL_SNBEP_UNC_PCU",    /* Intel SandyBridge-EP PCU uncore */
        "INTEL_SNBEP_UNC_QPI0",   /* Intel SandyBridge-EP QPI link 0 uncore */
        "INTEL_SNBEP_UNC_QPI1",   /* Intel SandyBridge-EP QPI link 1 uncore */
        "INTEL_SNBEP_UNC_UBOX",   /* Intel SandyBridge-EP U-Box uncore */
        "INTEL_SNBEP_UNC_R2PCIE", /* Intel SandyBridge-EP R2PCIe uncore */
        "INTEL_SNBEP_UNC_R3QPI0", /* Intel SandyBridge-EP R3QPI 0 uncore */
        "INTEL_SNBEP_UNC_R3QPI1", /* Intel SandyBridge-EP R3QPI 1 uncore */
        "INTEL_KNC",              /* Intel Knights Corner (Xeon Phi) */

        "S390X_CPUM_CF", /* s390x: CPU-M counter facility */

        "ARM_1176", /* ARM 1176 */

        "INTEL_IVB_EP", /* Intel IvyBridge EP */
        "INTEL_HSW",    /* Intel Haswell */

        "INTEL_IVB_UNC_CB0", /* Intel IvyBridge C-box 0 uncore PMU */
        "INTEL_IVB_UNC_CB1", /* Intel IvyBridge C-box 1 uncore PMU */
        "INTEL_IVB_UNC_CB2", /* Intel IvyBridge C-box 2 uncore PMU */
        "INTEL_IVB_UNC_CB3", /* Intel IvyBridge C-box 3 uncore PMU */

        "POWER8",     /* IBM POWER8 */
        "INTEL_RAPL", /* Intel RAPL */

        "INTEL_SLM",       /* Intel Silvermont */
        "AMD64_FAM15H_NB", /* AMD AMD64 Fam15h NorthBridge */

        "ARM_QCOM_KRAIT", /* Qualcomm Krait */
        "PERF_EVENT_RAW", /* perf_events RAW event syntax */

        "INTEL_IVBEP_UNC_CB0",    /* Intel IvyBridge-EP C-Box core 0 uncore */
        "INTEL_IVBEP_UNC_CB1",    /* Intel IvyBridge-EP C-Box core 1 uncore */
        "INTEL_IVBEP_UNC_CB2",    /* Intel IvyBridge-EP C-Box core 2 uncore */
        "INTEL_IVBEP_UNC_CB3",    /* Intel IvyBridge-EP C-Box core 3 uncore */
        "INTEL_IVBEP_UNC_CB4",    /* Intel IvyBridge-EP C-Box core 4 uncore */
        "INTEL_IVBEP_UNC_CB5",    /* Intel IvyBridge-EP C-Box core 5 uncore */
        "INTEL_IVBEP_UNC_CB6",    /* Intel IvyBridge-EP C-Box core 6 uncore */
        "INTEL_IVBEP_UNC_CB7",    /* Intel IvyBridge-EP C-Box core 7 uncore */
        "INTEL_IVBEP_UNC_CB8",    /* Intel IvyBridge-EP C-Box core 8 uncore */
        "INTEL_IVBEP_UNC_CB9",    /* Intel IvyBridge-EP C-Box core 9 uncore */
        "INTEL_IVBEP_UNC_CB10",   /* Intel IvyBridge-EP C-Box core 10 uncore */
        "INTEL_IVBEP_UNC_CB11",   /* Intel IvyBridge-EP C-Box core 11 uncore */
        "INTEL_IVBEP_UNC_CB12",   /* Intel IvyBridge-EP C-Box core 12 uncore */
        "INTEL_IVBEP_UNC_CB13",   /* Intel IvyBridge-EP C-Box core 13 uncore */
        "INTEL_IVBEP_UNC_CB14",   /* Intel IvyBridge-EP C-Box core 14 uncore */
        "INTEL_IVBEP_UNC_HA0",    /* Intel IvyBridge-EP HA 0 uncore */
        "INTEL_IVBEP_UNC_HA1",    /* Intel IvyBridge-EP HA 1 uncore */
        "INTEL_IVBEP_UNC_IMC0",   /* Intel IvyBridge-EP IMC socket 0 uncore */
        "INTEL_IVBEP_UNC_IMC1",   /* Intel IvyBridge-EP IMC socket 1 uncore */
        "INTEL_IVBEP_UNC_IMC2",   /* Intel IvyBridge-EP IMC socket 2 uncore */
        "INTEL_IVBEP_UNC_IMC3",   /* Intel IvyBridge-EP IMC socket 3 uncore */
        "INTEL_IVBEP_UNC_IMC4",   /* Intel IvyBridge-EP IMC socket 4 uncore */
        "INTEL_IVBEP_UNC_IMC5",   /* Intel IvyBridge-EP IMC socket 5 uncore */
        "INTEL_IVBEP_UNC_IMC6",   /* Intel IvyBridge-EP IMC socket 6 uncore */
        "INTEL_IVBEP_UNC_IMC7",   /* Intel IvyBridge-EP IMC socket 7 uncore */
        "INTEL_IVBEP_UNC_PCU",    /* Intel IvyBridge-EP PCU uncore */
        "INTEL_IVBEP_UNC_QPI0",   /* Intel IvyBridge-EP QPI link 0 uncore */
        "INTEL_IVBEP_UNC_QPI1",   /* Intel IvyBridge-EP QPI link 1 uncore */
        "INTEL_IVBEP_UNC_QPI2",   /* Intel IvyBridge-EP QPI link 2 uncore */
        "INTEL_IVBEP_UNC_UBOX",   /* Intel IvyBridge-EP U-Box uncore */
        "INTEL_IVBEP_UNC_R2PCIE", /* Intel IvyBridge-EP R2PCIe uncore */
        "INTEL_IVBEP_UNC_R3QPI0", /* Intel IvyBridge-EP R3QPI 0 uncore */
        "INTEL_IVBEP_UNC_R3QPI1", /* Intel IvyBridge-EP R3QPI 1 uncore */
        "INTEL_IVBEP_UNC_R3QPI2", /* Intel IvyBridge-EP R3QPI 2 uncore */
        "INTEL_IVBEP_UNC_IRP",    /* Intel IvyBridge-EP IRP uncore */

        "S390X_CPUM_SF", /* s390x: CPU-M sampling facility */

        "ARM_CORTEX_A57", /* ARM Cortex A57 (ARMv8) */
        "ARM_CORTEX_A53", /* ARM Cortex A53 (ARMv8) */

        "ARM_CORTEX_A7", /* ARM Cortex A7 */

        "INTEL_HSW_EP", /* Intel Haswell EP */
        "INTEL_BDW",    /* Intel Broadwell */

        "ARM_XGENE", /* Applied Micro X-Gene (ARMv8) */

        "INTEL_HSWEP_UNC_CB0",    /* Intel Haswell-EP C-Box core 0 uncore */
        "INTEL_HSWEP_UNC_CB1",    /* Intel Haswell-EP C-Box core 1 uncore */
        "INTEL_HSWEP_UNC_CB2",    /* Intel Haswell-EP C-Box core 2 uncore */
        "INTEL_HSWEP_UNC_CB3",    /* Intel Haswell-EP C-Box core 3 uncore */
        "INTEL_HSWEP_UNC_CB4",    /* Intel Haswell-EP C-Box core 4 uncore */
        "INTEL_HSWEP_UNC_CB5",    /* Intel Haswell-EP C-Box core 5 uncore */
        "INTEL_HSWEP_UNC_CB6",    /* Intel Haswell-EP C-Box core 6 uncore */
        "INTEL_HSWEP_UNC_CB7",    /* Intel Haswell-EP C-Box core 7 uncore */
        "INTEL_HSWEP_UNC_CB8",    /* Intel Haswell-EP C-Box core 8 uncore */
        "INTEL_HSWEP_UNC_CB9",    /* Intel Haswell-EP C-Box core 9 uncore */
        "INTEL_HSWEP_UNC_CB10",   /* Intel Haswell-EP C-Box core 10 uncore */
        "INTEL_HSWEP_UNC_CB11",   /* Intel Haswell-EP C-Box core 11 uncore */
        "INTEL_HSWEP_UNC_CB12",   /* Intel Haswell-EP C-Box core 12 uncore */
        "INTEL_HSWEP_UNC_CB13",   /* Intel Haswell-EP C-Box core 13 uncore */
        "INTEL_HSWEP_UNC_CB14",   /* Intel Haswell-EP C-Box core 14 uncore */
        "INTEL_HSWEP_UNC_CB15",   /* Intel Haswell-EP C-Box core 15 uncore */
        "INTEL_HSWEP_UNC_CB16",   /* Intel Haswell-EP C-Box core 16 uncore */
        "INTEL_HSWEP_UNC_CB17",   /* Intel Haswell-EP C-Box core 17 uncore */
        "INTEL_HSWEP_UNC_HA0",    /* Intel Haswell-EP HA 0 uncore */
        "INTEL_HSWEP_UNC_HA1",    /* Intel Haswell-EP HA 1 uncore */
        "INTEL_HSWEP_UNC_IMC0",   /* Intel Haswell-EP IMC socket 0 uncore */
        "INTEL_HSWEP_UNC_IMC1",   /* Intel Haswell-EP IMC socket 1 uncore */
        "INTEL_HSWEP_UNC_IMC2",   /* Intel Haswell-EP IMC socket 2 uncore */
        "INTEL_HSWEP_UNC_IMC3",   /* Intel Haswell-EP IMC socket 3 uncore */
        "INTEL_HSWEP_UNC_IMC4",   /* Intel Haswell-EP IMC socket 4 uncore */
        "INTEL_HSWEP_UNC_IMC5",   /* Intel Haswell-EP IMC socket 5 uncore */
        "INTEL_HSWEP_UNC_IMC6",   /* Intel Haswell-EP IMC socket 6 uncore */
        "INTEL_HSWEP_UNC_IMC7",   /* Intel Haswell-EP IMC socket 7 uncore */
        "INTEL_HSWEP_UNC_PCU",    /* Intel Haswell-EP PCU uncore */
        "INTEL_HSWEP_UNC_QPI0",   /* Intel Haswell-EP QPI link 0 uncore */
        "INTEL_HSWEP_UNC_QPI1",   /* Intel Haswell-EP QPI link 1 uncore */
        "INTEL_HSWEP_UNC_UBOX",   /* Intel Haswell-EP U-Box uncore */
        "INTEL_HSWEP_UNC_R2PCIE", /* Intel Haswell-EP R2PCIe uncore */
        "INTEL_HSWEP_UNC_R3QPI0", /* Intel Haswell-EP R3QPI 0 uncore */
        "INTEL_HSWEP_UNC_R3QPI1", /* Intel Haswell-EP R3QPI 1 uncore */
        "INTEL_HSWEP_UNC_R3QPI2", /* Intel Haswell-EP R3QPI 2 uncore */
        "INTEL_HSWEP_UNC_IRP",    /* Intel Haswell-EP IRP uncore */
        "INTEL_HSWEP_UNC_SB0",    /* Intel Haswell-EP S-Box 0 uncore */
        "INTEL_HSWEP_UNC_SB1",    /* Intel Haswell-EP S-Box 1 uncore */
        "INTEL_HSWEP_UNC_SB2",    /* Intel Haswell-EP S-Box 2 uncore */
        "INTEL_HSWEP_UNC_SB3",    /* Intel Haswell-EP S-Box 3 uncore */

        "POWERPC_NEST_MCS_READ_BW",  /* POWERPC Nest Memory Read bandwidth */
        "POWERPC_NEST_MCS_WRITE_BW", /* POWERPC Nest Memory Write bandwidth */

        "INTEL_SKL", /* Intel Skylake */

        "INTEL_BDW_EP", /* Intel Broadwell EP */

        "INTEL_GLM", /* Intel Goldmont */

        "INTEL_KNL",               /* Intel Knights Landing */
        "INTEL_KNL_UNC_IMC0",      /* Intel KnightLanding IMC channel 0 uncore */
        "INTEL_KNL_UNC_IMC1",      /* Intel KnightLanding IMC channel 1 uncore */
        "INTEL_KNL_UNC_IMC2",      /* Intel KnightLanding IMC channel 2 uncore */
        "INTEL_KNL_UNC_IMC3",      /* Intel KnightLanding IMC channel 3 uncore */
        "INTEL_KNL_UNC_IMC4",      /* Intel KnightLanding IMC channel 4 uncore */
        "INTEL_KNL_UNC_IMC5",      /* Intel KnightLanding IMC channel 5 uncore */
        "INTEL_KNL_UNC_IMC_UCLK0", /* Intel KnightLanding IMC UCLK unit 0 uncore */
        "INTEL_KNL_UNC_IMC_UCLK1", /* Intel KnightLanding IMC UCLK unit 1 uncore */
        "INTEL_KNL_UNC_EDC_ECLK0", /* Intel KnightLanding EDC ECLK unit 0 uncore */
        "INTEL_KNL_UNC_EDC_ECLK1", /* Intel KnightLanding EDC ECLK unit 1 uncore */
        "INTEL_KNL_UNC_EDC_ECLK2", /* Intel KnightLanding EDC ECLK unit 2 uncore */
        "INTEL_KNL_UNC_EDC_ECLK3", /* Intel KnightLanding EDC ECLK unit 3 uncore */
        "INTEL_KNL_UNC_EDC_ECLK4", /* Intel KnightLanding EDC ECLK unit 4 uncore */
        "INTEL_KNL_UNC_EDC_ECLK5", /* Intel KnightLanding EDC ECLK unit 5 uncore */
        "INTEL_KNL_UNC_EDC_ECLK6", /* Intel KnightLanding EDC ECLK unit 6 uncore */
        "INTEL_KNL_UNC_EDC_ECLK7", /* Intel KnightLanding EDC ECLK unit 7 uncore */
        "INTEL_KNL_UNC_EDC_UCLK0", /* Intel KnightLanding EDC UCLK unit 0 uncore */
        "INTEL_KNL_UNC_EDC_UCLK1", /* Intel KnightLanding EDC UCLK unit 1 uncore */
        "INTEL_KNL_UNC_EDC_UCLK2", /* Intel KnightLanding EDC UCLK unit 2 uncore */
        "INTEL_KNL_UNC_EDC_UCLK3", /* Intel KnightLanding EDC UCLK unit 3 uncore */
        "INTEL_KNL_UNC_EDC_UCLK4", /* Intel KnightLanding EDC UCLK unit 4 uncore */
        "INTEL_KNL_UNC_EDC_UCLK5", /* Intel KnightLanding EDC UCLK unit 5 uncore */
        "INTEL_KNL_UNC_EDC_UCLK6", /* Intel KnightLanding EDC UCLK unit 6 uncore */
        "INTEL_KNL_UNC_EDC_UCLK7", /* Intel KnightLanding EDC UCLK unit 7 uncore */

        "INTEL_KNL_UNC_CHA0",  /* Intel KnightLanding CHA unit 0 uncore */
        "INTEL_KNL_UNC_CHA1",  /* Intel KnightLanding CHA unit 1 uncore */
        "INTEL_KNL_UNC_CHA2",  /* Intel KnightLanding CHA unit 2 uncore */
        "INTEL_KNL_UNC_CHA3",  /* Intel KnightLanding CHA unit 3 uncore */
        "INTEL_KNL_UNC_CHA4",  /* Intel KnightLanding CHA unit 4 uncore */
        "INTEL_KNL_UNC_CHA5",  /* Intel KnightLanding CHA unit 5 uncore */
        "INTEL_KNL_UNC_CHA6",  /* Intel KnightLanding CHA unit 6 uncore */
        "INTEL_KNL_UNC_CHA7",  /* Intel KnightLanding CHA unit 7 uncore */
        "INTEL_KNL_UNC_CHA8",  /* Intel KnightLanding CHA unit 8 uncore */
        "INTEL_KNL_UNC_CHA9",  /* Intel KnightLanding CHA unit 9 uncore */
        "INTEL_KNL_UNC_CHA10", /* Intel KnightLanding CHA unit 10 uncore */
        "INTEL_KNL_UNC_CHA11", /* Intel KnightLanding CHA unit 11 uncore */
        "INTEL_KNL_UNC_CHA12", /* Intel KnightLanding CHA unit 12 uncore */
        "INTEL_KNL_UNC_CHA13", /* Intel KnightLanding CHA unit 13 uncore */
        "INTEL_KNL_UNC_CHA14", /* Intel KnightLanding CHA unit 14 uncore */
        "INTEL_KNL_UNC_CHA15", /* Intel KnightLanding CHA unit 15 uncore */
        "INTEL_KNL_UNC_CHA16", /* Intel KnightLanding CHA unit 16 uncore */
        "INTEL_KNL_UNC_CHA17", /* Intel KnightLanding CHA unit 17 uncore */
        "INTEL_KNL_UNC_CHA18", /* Intel KnightLanding CHA unit 18 uncore */
        "INTEL_KNL_UNC_CHA19", /* Intel KnightLanding CHA unit 19 uncore */
        "INTEL_KNL_UNC_CHA20", /* Intel KnightLanding CHA unit 20 uncore */
        "INTEL_KNL_UNC_CHA21", /* Intel KnightLanding CHA unit 21 uncore */
        "INTEL_KNL_UNC_CHA22", /* Intel KnightLanding CHA unit 22 uncore */
        "INTEL_KNL_UNC_CHA23", /* Intel KnightLanding CHA unit 23 uncore */
        "INTEL_KNL_UNC_CHA24", /* Intel KnightLanding CHA unit 24 uncore */
        "INTEL_KNL_UNC_CHA25", /* Intel KnightLanding CHA unit 25 uncore */
        "INTEL_KNL_UNC_CHA26", /* Intel KnightLanding CHA unit 26 uncore */
        "INTEL_KNL_UNC_CHA27", /* Intel KnightLanding CHA unit 27 uncore */
        "INTEL_KNL_UNC_CHA28", /* Intel KnightLanding CHA unit 28 uncore */
        "INTEL_KNL_UNC_CHA29", /* Intel KnightLanding CHA unit 29 uncore */
        "INTEL_KNL_UNC_CHA30", /* Intel KnightLanding CHA unit 30 uncore */
        "INTEL_KNL_UNC_CHA31", /* Intel KnightLanding CHA unit 31 uncore */
        "INTEL_KNL_UNC_CHA32", /* Intel KnightLanding CHA unit 32 uncore */
        "INTEL_KNL_UNC_CHA33", /* Intel KnightLanding CHA unit 33 uncore */
        "INTEL_KNL_UNC_CHA34", /* Intel KnightLanding CHA unit 34 uncore */
        "INTEL_KNL_UNC_CHA35", /* Intel KnightLanding CHA unit 35 uncore */
        "INTEL_KNL_UNC_CHA36", /* Intel KnightLanding CHA unit 36 uncore */
        "INTEL_KNL_UNC_CHA37", /* Intel KnightLanding CHA unit 37 uncore */

        "INTEL_KNL_UNC_UBOX",   /* Intel KnightLanding Ubox uncore */
        "INTEL_KNL_UNC_M2PCIE", /* Intel KnightLanding M2PCIe uncore */

        "POWER9", /* IBM POWER9 */

        "INTEL_BDX_UNC_CB0",    /* Intel Broadwell-X C-Box core 0 uncore */
        "INTEL_BDX_UNC_CB1",    /* Intel Broadwell-X C-Box core 1 uncore */
        "INTEL_BDX_UNC_CB2",    /* Intel Broadwell-X C-Box core 2 uncore */
        "INTEL_BDX_UNC_CB3",    /* Intel Broadwell-X C-Box core 3 uncore */
        "INTEL_BDX_UNC_CB4",    /* Intel Broadwell-X C-Box core 4 uncore */
        "INTEL_BDX_UNC_CB5",    /* Intel Broadwell-X C-Box core 5 uncore */
        "INTEL_BDX_UNC_CB6",    /* Intel Broadwell-X C-Box core 6 uncore */
        "INTEL_BDX_UNC_CB7",    /* Intel Broadwell-X C-Box core 7 uncore */
        "INTEL_BDX_UNC_CB8",    /* Intel Broadwell-X C-Box core 8 uncore */
        "INTEL_BDX_UNC_CB9",    /* Intel Broadwell-X C-Box core 9 uncore */
        "INTEL_BDX_UNC_CB10",   /* Intel Broadwell-X C-Box core 10 uncore */
        "INTEL_BDX_UNC_CB11",   /* Intel Broadwell-X C-Box core 11 uncore */
        "INTEL_BDX_UNC_CB12",   /* Intel Broadwell-X C-Box core 12 uncore */
        "INTEL_BDX_UNC_CB13",   /* Intel Broadwell-X C-Box core 13 uncore */
        "INTEL_BDX_UNC_CB14",   /* Intel Broadwell-X C-Box core 14 uncore */
        "INTEL_BDX_UNC_CB15",   /* Intel Broadwell-X C-Box core 15 uncore */
        "INTEL_BDX_UNC_CB16",   /* Intel Broadwell-X C-Box core 16 uncore */
        "INTEL_BDX_UNC_CB17",   /* Intel Broadwell-X C-Box core 17 uncore */
        "INTEL_BDX_UNC_CB18",   /* Intel Broadwell-X C-Box core 18 uncore */
        "INTEL_BDX_UNC_CB19",   /* Intel Broadwell-X C-Box core 19 uncore */
        "INTEL_BDX_UNC_CB20",   /* Intel Broadwell-X C-Box core 20 uncore */
        "INTEL_BDX_UNC_CB21",   /* Intel Broadwell-X C-Box core 21 uncore */
        "INTEL_BDX_UNC_CB22",   /* Intel Broadwell-X C-Box core 22 uncore */
        "INTEL_BDX_UNC_CB23",   /* Intel Broadwell-X C-Box core 23 uncore */
        "INTEL_BDX_UNC_HA0",    /* Intel Broadwell-X HA 0 uncore */
        "INTEL_BDX_UNC_HA1",    /* Intel Broadwell-X HA 1 uncore */
        "INTEL_BDX_UNC_IMC0",   /* Intel Broadwell-X IMC socket 0 uncore */
        "INTEL_BDX_UNC_IMC1",   /* Intel Broadwell-X IMC socket 1 uncore */
        "INTEL_BDX_UNC_IMC2",   /* Intel Broadwell-X IMC socket 2 uncore */
        "INTEL_BDX_UNC_IMC3",   /* Intel Broadwell-X IMC socket 3 uncore */
        "INTEL_BDX_UNC_IMC4",   /* Intel Broadwell-X IMC socket 4 uncore */
        "INTEL_BDX_UNC_IMC5",   /* Intel Broadwell-X IMC socket 5 uncore */
        "INTEL_BDX_UNC_IMC6",   /* Intel Broadwell-X IMC socket 6 uncore */
        "INTEL_BDX_UNC_IMC7",   /* Intel Broadwell-X IMC socket 7 uncore */
        "INTEL_BDX_UNC_PCU",    /* Intel Broadwell-X PCU uncore */
        "INTEL_BDX_UNC_QPI0",   /* Intel Broadwell-X QPI link 0 uncore */
        "INTEL_BDX_UNC_QPI1",   /* Intel Broadwell-X QPI link 1 uncore */
        "INTEL_BDX_UNC_QPI2",   /* Intel Broadwell-X QPI link 2 uncore */
        "INTEL_BDX_UNC_UBOX",   /* Intel Broadwell-X U-Box uncore */
        "INTEL_BDX_UNC_R2PCIE", /* Intel Broadwell-X R2PCIe uncore */
        "INTEL_BDX_UNC_R3QPI0", /* Intel Broadwell-X R3QPI 0 uncore */
        "INTEL_BDX_UNC_R3QPI1", /* Intel Broadwell-X R3QPI 1 uncore */
        "INTEL_BDX_UNC_R3QPI2", /* Intel Broadwell-X R3QPI 2 uncore */
        "INTEL_BDX_UNC_IRP",    /* Intel Broadwell-X IRP uncore */
        "INTEL_BDX_UNC_SB0",    /* Intel Broadwell-X S-Box 0 uncore */
        "INTEL_BDX_UNC_SB1",    /* Intel Broadwell-X S-Box 1 uncore */
        "INTEL_BDX_UNC_SB2",    /* Intel Broadwell-X S-Box 2 uncore */
        "INTEL_BDX_UNC_SB3",    /* Intel Broadwell-X S-Box 3 uncore */

        "AMD64_FAM17H", /* AMD AMD64 Fam17h Zen1 (deprecated) */
        "AMD64_FAM16H", /* AMD AMD64 Fam16h Jaguar */

        "INTEL_SKX", /* Intel Skylake-X */

        "INTEL_SKX_UNC_CHA0",   /* Intel Skylake-X CHA core 0 uncore */
        "INTEL_SKX_UNC_CHA1",   /* Intel Skylake-X CHA core 1 uncore */
        "INTEL_SKX_UNC_CHA2",   /* Intel Skylake-X CHA core 2 uncore */
        "INTEL_SKX_UNC_CHA3",   /* Intel Skylake-X CHA core 3 uncore */
        "INTEL_SKX_UNC_CHA4",   /* Intel Skylake-X CHA core 4 uncore */
        "INTEL_SKX_UNC_CHA5",   /* Intel Skylake-X CHA core 5 uncore */
        "INTEL_SKX_UNC_CHA6",   /* Intel Skylake-X CHA core 6 uncore */
        "INTEL_SKX_UNC_CHA7",   /* Intel Skylake-X CHA core 7 uncore */
        "INTEL_SKX_UNC_CHA8",   /* Intel Skylake-X CHA core 8 uncore */
        "INTEL_SKX_UNC_CHA9",   /* Intel Skylake-X CHA core 9 uncore */
        "INTEL_SKX_UNC_CHA10",  /* Intel Skylake-X CHA core 10 uncore */
        "INTEL_SKX_UNC_CHA11",  /* Intel Skylake-X CHA core 11 uncore */
        "INTEL_SKX_UNC_CHA12",  /* Intel Skylake-X CHA core 12 uncore */
        "INTEL_SKX_UNC_CHA13",  /* Intel Skylake-X CHA core 13 uncore */
        "INTEL_SKX_UNC_CHA14",  /* Intel Skylake-X CHA core 14 uncore */
        "INTEL_SKX_UNC_CHA15",  /* Intel Skylake-X CHA core 15 uncore */
        "INTEL_SKX_UNC_CHA16",  /* Intel Skylake-X CHA core 16 uncore */
        "INTEL_SKX_UNC_CHA17",  /* Intel Skylake-X CHA core 17 uncore */
        "INTEL_SKX_UNC_CHA18",  /* Intel Skylake-X CHA core 18 uncore */
        "INTEL_SKX_UNC_CHA19",  /* Intel Skylake-X CHA core 19 uncore */
        "INTEL_SKX_UNC_CHA20",  /* Intel Skylake-X CHA core 20 uncore */
        "INTEL_SKX_UNC_CHA21",  /* Intel Skylake-X CHA core 21 uncore */
        "INTEL_SKX_UNC_CHA22",  /* Intel Skylake-X CHA core 22 uncore */
        "INTEL_SKX_UNC_CHA23",  /* Intel Skylake-X CHA core 23 uncore */
        "INTEL_SKX_UNC_CHA24",  /* Intel Skylake-X CHA core 24 uncore */
        "INTEL_SKX_UNC_CHA25",  /* Intel Skylake-X CHA core 25 uncore */
        "INTEL_SKX_UNC_CHA26",  /* Intel Skylake-X CHA core 26 uncore */
        "INTEL_SKX_UNC_CHA27",  /* Intel Skylake-X CHA core 27 uncore */
        "INTEL_SKX_UNC_IIO0",   /* Intel Skylake-X IIO0 uncore */
        "INTEL_SKX_UNC_IIO1",   /* Intel Skylake-X IIO1 uncore */
        "INTEL_SKX_UNC_IIO2",   /* Intel Skylake-X IIO2 uncore */
        "INTEL_SKX_UNC_IIO3",   /* Intel Skylake-X IIO3 uncore */
        "INTEL_SKX_UNC_IIO4",   /* Intel Skylake-X IIO4 uncore */
        "INTEL_SKX_UNC_IIO5",   /* Intel Skylake-X IIO5 uncore */
        "INTEL_SKX_UNC_IMC0",   /* Intel Skylake-X IMC0 uncore */
        "INTEL_SKX_UNC_IMC1",   /* Intel Skylake-X IMC1 uncore */
        "INTEL_SKX_UNC_IMC2",   /* Intel Skylake-X IMC2 uncore */
        "INTEL_SKX_UNC_IMC3",   /* Intel Skylake-X IMC3 uncore */
        "INTEL_SKX_UNC_IMC4",   /* Intel Skylake-X IMC4 uncore */
        "INTEL_SKX_UNC_IMC5",   /* Intel Skylake-X IMC5 uncore */
        "INTEL_SKX_UNC_IRP",    /* Intel Skylake-X IRP  uncore */
        "INTEL_SKX_UNC_M2M0",   /* Intel Skylake-X M2M0 uncore */
        "INTEL_SKX_UNC_M2M1",   /* Intel Skylake-X M2M1 uncore */
        "INTEL_SKX_UNC_M3UPI0", /* Intel Skylake-X M3UPI0 uncore */
        "INTEL_SKX_UNC_M3UPI1", /* Intel Skylake-X M3UPI1 uncore */
        "INTEL_SKX_UNC_M3UPI2", /* Intel Skylake-X M3UPI2 uncore */
        "INTEL_SKX_UNC_PCU",    /* Intel Skylake-X PCU uncore */
        "INTEL_SKX_UNC_UBOX",   /* Intel Skylake-X U-Box uncore */
        "INTEL_SKX_UNC_UPI0",   /* Intel Skylake-X UPI link 0 uncore */
        "INTEL_SKX_UNC_UPI1",   /* Intel Skylake-X UPI link 1 uncore */
        "INTEL_SKX_UNC_UPI2",   /* Intel Skylake-X UPI link 2 uncore */

        "INTEL_KNM",               /* Intel Knights Mill */
        "INTEL_KNM_UNC_IMC0",      /* Intel Knights Mill IMC channel 0 uncore */
        "INTEL_KNM_UNC_IMC1",      /* Intel Knights Mill IMC channel 1 uncore */
        "INTEL_KNM_UNC_IMC2",      /* Intel Knights Mill IMC channel 2 uncore */
        "INTEL_KNM_UNC_IMC3",      /* Intel Knights Mill IMC channel 3 uncore */
        "INTEL_KNM_UNC_IMC4",      /* Intel Knights Mill IMC channel 4 uncore */
        "INTEL_KNM_UNC_IMC5",      /* Intel Knights Mill IMC channel 5 uncore */
        "INTEL_KNM_UNC_IMC_UCLK0", /* Intel Knights Mill IMC UCLK unit 0 uncore */
        "INTEL_KNM_UNC_IMC_UCLK1", /* Intel Knights Mill IMC UCLK unit 1 uncore */
        "INTEL_KNM_UNC_EDC_ECLK0", /* Intel Knights Mill EDC ECLK unit 0 uncore */
        "INTEL_KNM_UNC_EDC_ECLK1", /* Intel Knights Mill EDC ECLK unit 1 uncore */
        "INTEL_KNM_UNC_EDC_ECLK2", /* Intel Knights Mill EDC ECLK unit 2 uncore */
        "INTEL_KNM_UNC_EDC_ECLK3", /* Intel Knights Mill EDC ECLK unit 3 uncore */
        "INTEL_KNM_UNC_EDC_ECLK4", /* Intel Knights Mill EDC ECLK unit 4 uncore */
        "INTEL_KNM_UNC_EDC_ECLK5", /* Intel Knights Mill EDC ECLK unit 5 uncore */
        "INTEL_KNM_UNC_EDC_ECLK6", /* Intel Knights Mill EDC ECLK unit 6 uncore */
        "INTEL_KNM_UNC_EDC_ECLK7", /* Intel Knights Mill EDC ECLK unit 7 uncore */
        "INTEL_KNM_UNC_EDC_UCLK0", /* Intel Knights Mill EDC UCLK unit 0 uncore */
        "INTEL_KNM_UNC_EDC_UCLK1", /* Intel Knights Mill EDC UCLK unit 1 uncore */
        "INTEL_KNM_UNC_EDC_UCLK2", /* Intel Knights Mill EDC UCLK unit 2 uncore */
        "INTEL_KNM_UNC_EDC_UCLK3", /* Intel Knights Mill EDC UCLK unit 3 uncore */
        "INTEL_KNM_UNC_EDC_UCLK4", /* Intel Knights Mill EDC UCLK unit 4 uncore */
        "INTEL_KNM_UNC_EDC_UCLK5", /* Intel Knights Mill EDC UCLK unit 5 uncore */
        "INTEL_KNM_UNC_EDC_UCLK6", /* Intel Knights Mill EDC UCLK unit 6 uncore */
        "INTEL_KNM_UNC_EDC_UCLK7", /* Intel Knights Mill EDC UCLK unit 7 uncore */

        "INTEL_KNM_UNC_CHA0",  /* Intel Knights Mill CHA unit 0 uncore */
        "INTEL_KNM_UNC_CHA1",  /* Intel Knights Mill CHA unit 1 uncore */
        "INTEL_KNM_UNC_CHA2",  /* Intel Knights Mill CHA unit 2 uncore */
        "INTEL_KNM_UNC_CHA3",  /* Intel Knights Mill CHA unit 3 uncore */
        "INTEL_KNM_UNC_CHA4",  /* Intel Knights Mill CHA unit 4 uncore */
        "INTEL_KNM_UNC_CHA5",  /* Intel Knights Mill CHA unit 5 uncore */
        "INTEL_KNM_UNC_CHA6",  /* Intel Knights Mill CHA unit 6 uncore */
        "INTEL_KNM_UNC_CHA7",  /* Intel Knights Mill CHA unit 7 uncore */
        "INTEL_KNM_UNC_CHA8",  /* Intel Knights Mill CHA unit 8 uncore */
        "INTEL_KNM_UNC_CHA9",  /* Intel Knights Mill CHA unit 9 uncore */
        "INTEL_KNM_UNC_CHA10", /* Intel Knights Mill CHA unit 10 uncore */
        "INTEL_KNM_UNC_CHA11", /* Intel Knights Mill CHA unit 11 uncore */
        "INTEL_KNM_UNC_CHA12", /* Intel Knights Mill CHA unit 12 uncore */
        "INTEL_KNM_UNC_CHA13", /* Intel Knights Mill CHA unit 13 uncore */
        "INTEL_KNM_UNC_CHA14", /* Intel Knights Mill CHA unit 14 uncore */
        "INTEL_KNM_UNC_CHA15", /* Intel Knights Mill CHA unit 15 uncore */
        "INTEL_KNM_UNC_CHA16", /* Intel Knights Mill CHA unit 16 uncore */
        "INTEL_KNM_UNC_CHA17", /* Intel Knights Mill CHA unit 17 uncore */
        "INTEL_KNM_UNC_CHA18", /* Intel Knights Mill CHA unit 18 uncore */
        "INTEL_KNM_UNC_CHA19", /* Intel Knights Mill CHA unit 19 uncore */
        "INTEL_KNM_UNC_CHA20", /* Intel Knights Mill CHA unit 20 uncore */
        "INTEL_KNM_UNC_CHA21", /* Intel Knights Mill CHA unit 21 uncore */
        "INTEL_KNM_UNC_CHA22", /* Intel Knights Mill CHA unit 22 uncore */
        "INTEL_KNM_UNC_CHA23", /* Intel Knights Mill CHA unit 23 uncore */
        "INTEL_KNM_UNC_CHA24", /* Intel Knights Mill CHA unit 24 uncore */
        "INTEL_KNM_UNC_CHA25", /* Intel Knights Mill CHA unit 25 uncore */
        "INTEL_KNM_UNC_CHA26", /* Intel Knights Mill CHA unit 26 uncore */
        "INTEL_KNM_UNC_CHA27", /* Intel Knights Mill CHA unit 27 uncore */
        "INTEL_KNM_UNC_CHA28", /* Intel Knights Mill CHA unit 28 uncore */
        "INTEL_KNM_UNC_CHA29", /* Intel Knights Mill CHA unit 29 uncore */
        "INTEL_KNM_UNC_CHA30", /* Intel Knights Mill CHA unit 30 uncore */
        "INTEL_KNM_UNC_CHA31", /* Intel Knights Mill CHA unit 31 uncore */
        "INTEL_KNM_UNC_CHA32", /* Intel Knights Mill CHA unit 32 uncore */
        "INTEL_KNM_UNC_CHA33", /* Intel Knights Mill CHA unit 33 uncore */
        "INTEL_KNM_UNC_CHA34", /* Intel Knights Mill CHA unit 34 uncore */
        "INTEL_KNM_UNC_CHA35", /* Intel Knights Mill CHA unit 35 uncore */
        "INTEL_KNM_UNC_CHA36", /* Intel Knights Mill CHA unit 36 uncore */
        "INTEL_KNM_UNC_CHA37", /* Intel Knights Mill CHA unit 37 uncore */

        "INTEL_KNM_UNC_UBOX",   /* Intel Knights Mill Ubox uncore */
        "INTEL_KNM_UNC_M2PCIE", /* Intel Knights Mill M2PCIe uncore */
        "ARM_THUNDERX2",        /* Marvell ThunderX2 */

        "INTEL_CLX", /* Intel CascadeLake X */

        "ARM_THUNDERX2_DMC0",  /* Marvell ThunderX2 DMC unit 0 uncore */
        "ARM_THUNDERX2_DMC1",  /* Marvell ThunderX2 DMC unit 1 uncore */
        "ARM_THUNDERX2_LLC0",  /* Marvell ThunderX2 LLC unit 0 uncore */
        "ARM_THUNDERX2_LLC1",  /* Marvell ThunderX2 LLC unit 1 uncore */
        "ARM_THUNDERX2_CCPI0", /* Marvell ThunderX2 Cross-Socket Interconnect unit 0 uncore */
        "ARM_THUNDERX2_CCPI1", /* Marvell ThunderX2 Cross-Socket Interconnect unit 1 uncore */

        "AMD64_FAM17H_ZEN1", /* AMD AMD64 Fam17h Zen1 */
        "AMD64_FAM17H_ZEN2", /* AMD AMD64 Fam17h Zen2 */

        "INTEL_TMT", /* Intel Tremont */
        "INTEL_ICL", /* Intel IceLake */

        "ARM_A64FX", /* Fujitsu A64FX processor */
        "ARM_N1",    /* Arm Neoverse N1 */

        "AMD64_FAM19H_ZEN3",    /* AMD AMD64 Fam19h Zen3 */
        "AMD64_RAPL",           /* AMD64 RAPL */
        "AMD64_FAM19H_ZEN3_L3", /* AMD64 Fam17h Zen3 L3 */

        "INTEL_ICX", /* Intel IceLakeX */

        "ARM_N2", /* Arm Neoverse N2 */

        "ARM_KUNPENG",                 /* HiSilicon Kunpeng processor */
        "ARM_KUNPENG_UNC_SCCL1_DDRC0", /* Hisilicon Kunpeng SCCL unit 1 DDRC 0 uncore */
        "ARM_KUNPENG_UNC_SCCL1_DDRC1", /* Hisilicon Kunpeng SCCL unit 1 DDRC 1 uncore */
        "ARM_KUNPENG_UNC_SCCL1_DDRC2", /* Hisilicon Kunpeng SCCL unit 1 DDRC 2 uncore */
        "ARM_KUNPENG_UNC_SCCL1_DDRC3", /* Hisilicon Kunpeng SCCL unit 1 DDRC 3 uncore */
        "ARM_KUNPENG_UNC_SCCL3_DDRC0", /* Hisilicon Kunpeng SCCL unit 3 DDRC 0 uncore */
        "ARM_KUNPENG_UNC_SCCL3_DDRC1", /* Hisilicon Kunpeng SCCL unit 3 DDRC 1 uncore */
        "ARM_KUNPENG_UNC_SCCL3_DDRC2", /* Hisilicon Kunpeng SCCL unit 3 DDRC 2 uncore */
        "ARM_KUNPENG_UNC_SCCL3_DDRC3", /* Hisilicon Kunpeng SCCL unit 3 DDRC 3 uncore */
        "ARM_KUNPENG_UNC_SCCL5_DDRC0", /* Hisilicon Kunpeng SCCL unit 5 DDRC 0 uncore */
        "ARM_KUNPENG_UNC_SCCL5_DDRC1", /* Hisilicon Kunpeng SCCL unit 5 DDRC 1 uncore */
        "ARM_KUNPENG_UNC_SCCL5_DDRC2", /* Hisilicon Kunpeng SCCL unit 5 DDRC 2 uncore */
        "ARM_KUNPENG_UNC_SCCL5_DDRC3", /* Hisilicon Kunpeng SCCL unit 5 DDRC 3 uncore */
        "ARM_KUNPENG_UNC_SCCL7_DDRC0", /* Hisilicon Kunpeng SCCL unit 7 DDRC 0 uncore */
        "ARM_KUNPENG_UNC_SCCL7_DDRC1", /* Hisilicon Kunpeng SCCL unit 7 DDRC 1 uncore */
        "ARM_KUNPENG_UNC_SCCL7_DDRC2", /* Hisilicon Kunpeng SCCL unit 7 DDRC 2 uncore */
        "ARM_KUNPENG_UNC_SCCL7_DDRC3", /* Hisilicon Kunpeng SCCL unit 7 DDRC 3 uncore */
        "ARM_KUNPENG_UNC_SCCL1_HHA2",  /* Hisilicon Kunpeng SCCL unit 1 HHA 2 uncore */
        "ARM_KUNPENG_UNC_SCCL1_HHA3",  /* Hisilicon Kunpeng SCCL unit 1 HHA 3 uncore */
        "ARM_KUNPENG_UNC_SCCL3_HHA0",  /* Hisilicon Kunpeng SCCL unit 3 HHA 0 uncore */
        "ARM_KUNPENG_UNC_SCCL3_HHA1",  /* Hisilicon Kunpeng SCCL unit 3 HHA 1 uncore */
        "ARM_KUNPENG_UNC_SCCL5_HHA6",  /* Hisilicon Kunpeng SCCL unit 5 HHA 6 uncore */
        "ARM_KUNPENG_UNC_SCCL5_HHA7",  /* Hisilicon Kunpeng SCCL unit 5 HHA 7 uncore */
        "ARM_KUNPENG_UNC_SCCL7_HHA4",  /* Hisilicon Kunpeng SCCL unit 7 HHA 4 uncore */
        "ARM_KUNPENG_UNC_SCCL7_HHA5",  /* Hisilicon Kunpeng SCCL unit 7 HHA 5 uncore */
        "ARM_KUNPENG_UNC_SCCL1_L3C10", /* Hisilicon Kunpeng SCCL unit 1 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL1_L3C11", /* Hisilicon Kunpeng SCCL unit 1 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL1_L3C12", /* Hisilicon Kunpeng SCCL unit 1 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL1_L3C13", /* Hisilicon Kunpeng SCCL unit 1 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL1_L3C14", /* Hisilicon Kunpeng SCCL unit 1 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL1_L3C15", /* Hisilicon Kunpeng SCCL unit 1 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL1_L3C8",  /* Hisilicon Kunpeng SCCL unit 1 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL1_L3C9",  /* Hisilicon Kunpeng SCCL unit 1 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL3_L3C0",  /* Hisilicon Kunpeng SCCL unit 3 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL3_L3C1",  /* Hisilicon Kunpeng SCCL unit 3 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL3_L3C2",  /* Hisilicon Kunpeng SCCL unit 3 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL3_L3C3",  /* Hisilicon Kunpeng SCCL unit 3 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL3_L3C4",  /* Hisilicon Kunpeng SCCL unit 3 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL3_L3C5",  /* Hisilicon Kunpeng SCCL unit 3 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL3_L3C6",  /* Hisilicon Kunpeng SCCL unit 3 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL3_L3C7",  /* Hisilicon Kunpeng SCCL unit 3 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL5_L3C24", /* Hisilicon Kunpeng SCCL unit 5 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL5_L3C25", /* Hisilicon Kunpeng SCCL unit 5 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL5_L3C26", /* Hisilicon Kunpeng SCCL unit 5 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL5_L3C27", /* Hisilicon Kunpeng SCCL unit 5 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL5_L3C28", /* Hisilicon Kunpeng SCCL unit 5 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL5_L3C29", /* Hisilicon Kunpeng SCCL unit 5 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL5_L3C30", /* Hisilicon Kunpeng SCCL unit 5 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL5_L3C31", /* Hisilicon Kunpeng SCCL unit 5 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL7_L3C16", /* Hisilicon Kunpeng SCCL unit 7 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL7_L3C17", /* Hisilicon Kunpeng SCCL unit 7 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL7_L3C18", /* Hisilicon Kunpeng SCCL unit 7 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL7_L3C19", /* Hisilicon Kunpeng SCCL unit 7 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL7_L3C20", /* Hisilicon Kunpeng SCCL unit 7 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL7_L3C21", /* Hisilicon Kunpeng SCCL unit 7 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL7_L3C22", /* Hisilicon Kunpeng SCCL unit 7 L3C uncore */
        "ARM_KUNPENG_UNC_SCCL7_L3C23", /* Hisilicon Kunpeng SCCL unit 7 L3C uncore */

        "INTEL_SPR", /* Intel SapphireRapid */

        "POWER10",           /* IBM POWER10 */
        "AMD64_FAM19H_ZEN4", /* AMD AMD64 Fam19h Zen4 */
        "ARM_V1",            /* ARM Neoverse V1 */
        "ARM_V2",            /* Arm Neoverse V2 */
        "INTEL_EMR",         /* Intel EmeraldRapid */

        /* MUST ADD NEW PMU MODELS HERE */

        "MAX" /* end marker */
    };

    const char *pmu_types[] = {
        "UNKNOWN",    /* unknown PMU type */
        "CORE",       /* processor core PMU */
        "UNCORE",     /* processor socket-level PMU */
        "OS_GENERIC", /* generic OS-provided PMU */
        "MAX"};
}