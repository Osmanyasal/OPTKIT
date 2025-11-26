#include "config.hh"

bool is_requested_config_valid(const SystemConfig &config)
{
    return config.memory.is_valid() &&
           config.cpu.is_valid() &&
           config.disk_io.is_valid() &&
           config.kernel.is_valid() &&
           config.gpu.is_valid() &&
           config.cgroup.is_valid();
}
void apply_requested_config(const SystemConfig &config) {}