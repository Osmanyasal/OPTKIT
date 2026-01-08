#pragma once

#include "utils/utils.hh"
#include "core/query.hh"
#include "core/callstack/profiler.hh"

#if OPTKIT_CONF_PMU_MACROS_ENABLED

#define OPTKIT_CALLSTACK_PROFILER(block_name)                    \
    optkit::callstack::Profiler EXPAND_AND_CONCAT(var, __LINE__) \
    {                                                            \
        {                                                        \
            block_name, true, false, ::getpid(), -1, "callstack"   \
        }                                                        \
    }
#else
#include "core/disk/clear.hh"
#endif
