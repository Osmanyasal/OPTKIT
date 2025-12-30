#pragma once

#undef OPTKIT_SET_CPU_CORE_FREQ
#undef OPTKIT_SET_CPU_UNCORE_FREQ
#undef OPTKIT_SET_CPU_FREQ
#undef OPTKIT_RESET_CPU_CORE_FREQ
#undef OPTKIT_RESET_CPU_UNCORE_FREQ
#undef OPTKIT_SET_GPU_FREQ
#undef OPTKIT_RESET_GPU_FREQ

#define OPTKIT_SET_CPU_CORE_FREQ(frequency, socket)
#define OPTKIT_SET_CPU_UNCORE_FREQ(frequency, socket)
#define OPTKIT_SET_CPU_FREQ(frequency, socket)
#define OPTKIT_RESET_CPU_CORE_FREQ(socket)
#define OPTKIT_RESET_CPU_UNCORE_FREQ(socket)

#define OPTKIT_SET_GPU_FREQ(vendor, device_id, mem_clk_mhz, graphics_clk_mhz)
#define OPTKIT_RESET_GPU_FREQ(vendor, device_id)