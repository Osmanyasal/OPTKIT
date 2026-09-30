#pragma once

#include <linux/perf_event.h>
#include <string>

#include "utils/deployment/deployment_config.hh"
#include "core/metrics/performance/cpu/intel/native_events.hh"

#if OPTKIT_ENV_CPU_INTEL
namespace optkit::pmu::cpu::perf::intel
{
    inline void apply_event_attr(perf_event_attr &attr, const std::string &event_name)
    {
#if OPTKIT_INTEL_HAS_RFO_HITM_METRIC
        if (event_name == "DEMAND_RFO_HITM")
        {
#if OPTKIT_ENV_CPU_MICROARCH_NHM || OPTKIT_ENV_CPU_MICROARCH_WSM
            attr.config1 = 0x0c02ull;
#elif OPTKIT_ENV_CPU_MICROARCH_SNB || OPTKIT_ENV_CPU_MICROARCH_IVB || \
      OPTKIT_ENV_CPU_MICROARCH_HSW || OPTKIT_ENV_CPU_MICROARCH_BDW || \
      OPTKIT_ENV_CPU_MICROARCH_SKL
            attr.config1 = 0x1000000002ull;
#elif OPTKIT_ENV_CPU_MICROARCH_ICL || OPTKIT_ENV_CPU_MICROARCH_SPR
            attr.config1 = 0x10003c0002ull;
#endif
        }
        else if (event_name == "DEMAND_RFO_ANY_RESPONSE")
        {
#if OPTKIT_ENV_CPU_MICROARCH_NHM || OPTKIT_ENV_CPU_MICROARCH_WSM
            attr.config1 = 0xff02ull;
#elif OPTKIT_ENV_CPU_MICROARCH_SNB || OPTKIT_ENV_CPU_MICROARCH_IVB || \
      OPTKIT_ENV_CPU_MICROARCH_HSW || OPTKIT_ENV_CPU_MICROARCH_BDW || \
      OPTKIT_ENV_CPU_MICROARCH_SKL
            attr.config1 = 0x10002ull;
#elif OPTKIT_ENV_CPU_MICROARCH_ICL
#if OPTKIT_ENV_CPU_MODEL == 0x6A || OPTKIT_ENV_CPU_MODEL == 0x6C
            attr.config1 = 0x3f3ffc0002ull;
#else
            attr.config1 = 0x10002ull;
#endif
#elif OPTKIT_ENV_CPU_MICROARCH_SPR
            attr.config1 = 0x3f3ffc0002ull;
#endif
        }
#else
        (void)attr;
        (void)event_name;
#endif
    }
}
#endif