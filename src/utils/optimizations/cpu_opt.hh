#pragma once

#include "../../environment_config.hh"
// all macros start with OPT_ prefix.


#if defined(__GNUC__) || defined(__clang__)
    #define OPT_LIKELY(EXPR) __builtin_expect((bool)(EXPR), true)
    #define OPT_UNLIKELY(EXPR) __builtin_expect((bool)(EXPR), false)
    #define OPT_PREFETCH(address, rw, locality) __builtin_prefetch((address), (rw), (locality))
    #define OPT_FORCE_INLINE inline __attribute__((always_inline)) 

#else // if compiler is not GNU
    #define OPT_LIKELY(EXPR) EXPR
    #define OPT_UNLIKELY(EXPR) EXPR
    #define OPT_PREFETCH(address, rw, locality)
    #define OPT_FORCE_INLINE 
#endif

#ifdef OPTKIT_ENV_LLC_CACHE_LINESIZE
    #define OPT_NUM_ELEMENTS_PER_CACHELINE(dtype) OPTKIT_ENV_LLC_CACHE_LINESIZE / sizeof(dtype)
#else 
    #define OPT_NUM_ELEMENTS_PER_CACHELINE(dtype) 64 / sizeof(dtype)
#endif
