#pragma once

/**
 * @brief Porting enviroment
 * when is enabled, it removes trace logs.
 *
 */

#ifndef CONF__PORTING__IS_PRODUCTION
#define CONF__PORTING__IS_PRODUCTION 1
#endif

#ifndef CONF__OPTKIT__ENABLED
#define CONF__OPTKIT__ENABLED 1
#endif

#ifndef CONF__PMU__MACROS__ENABLED
#define CONF__PMU__MACROS__ENABLED (1 & CONF__OPTKIT__ENABLED)
#endif

#ifndef CONF__RAPL__MACROS__ENABLED
#define CONF__RAPL__MACROS__ENABLED (1 & CONF__OPTKIT__ENABLED)
#endif

#ifndef CONF__FREQ__MACROS__ENABLED
#define CONF__FREQ__MACROS__ENABLED (1 & CONF__OPTKIT__ENABLED)
#endif
