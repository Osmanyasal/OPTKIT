#pragma once

#undef OPTKIT_GPU_ENERGY
#undef OPTKIT_GPU_ENERGY_EVENTS_WITH_METRICS

#define OPTKIT_GPU_ENERGY(block_name)
#define OPTKIT_GPU_ENERGY_EVENTS_WITH_METRICS(block_name, metric_builder, ...)