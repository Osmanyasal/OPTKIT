#pragma once

#undef OPTKIT_CPU_EVENTS
#undef OPTKIT_CPU_BLOCK_EVENTS
#undef OPTKIT_CPU_GROUP_EVENTS
#undef OPTKIT_CPU_GROUP_EVENTS_REPEAT

#define OPTKIT_CPU_EVENTS(block_name, metric_builder, ...)
#define OPTKIT_CPU_BLOCK_EVENTS(block_name, metric_builder, ...)
#define OPTKIT_CPU_GROUP_EVENTS(block_name, metric_builder, ...)
#define OPTKIT_CPU_GROUP_EVENTS_REPEAT(block_name, metric_builder, count, ...)