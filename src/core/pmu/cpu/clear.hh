#pragma once

#undef OPTKIT_CPU_EVENTS
#undef OPTKIT_CPU_BLOCK_EVENTS
#undef OPTKIT_CPU_GROUP_EVENTS
#undef OPTKIT_CPU_GROUP_EVENTS_REPEAT

#undef OPTKIT_CPU_EVENTS_SAMPLING
#undef OPTKIT_CPU_BLOCK_EVENTS_SAMPLING
#undef OPTKIT_CPU_GROUP_EVENTS_SAMPLING
#undef OPTKIT_CPU_GROUP_EVENTS_REPEAT_SAMPLING

#define OPTKIT_CPU_EVENTS(block_name, metric_builder, ...)
#define OPTKIT_CPU_BLOCK_EVENTS(block_name, metric_builder, ...)
#define OPTKIT_CPU_GROUP_EVENTS(block_name, metric_builder, ...)
#define OPTKIT_CPU_GROUP_EVENTS_REPEAT(block_name, metric_builder, count, ...)

#define OPTKIT_CPU_EVENTS_SAMPLING(block_name, metric_builder, ...)
#define OPTKIT_CPU_BLOCK_EVENTS_SAMPLING(block_name, metric_builder, ...)
#define OPTKIT_CPU_GROUP_EVENTS_SAMPLING(block_name, metric_builder, ...)
#define OPTKIT_CPU_GROUP_EVENTS_REPEAT_SAMPLING(block_name, metric_builder, count, ...)