#include "diskio.hh"

std::string DiskIO::to_string() const
{
    std::ostringstream oss;
    oss << "DiskIO{scheduler=" << io_scheduler
        << ", mount=" << mount_path
        << ", options=" << mount_options
        << ", aio_max=" << aio_max_nr
        << ", sync=" << (sync_disk ? "yes" : "no")
        << ", direct_io=" << (use_direct_io ? "yes" : "no")
        << "}";
    return oss.str();
}
bool DiskIO::is_valid() const
{
    return true;
}

bool DiskIO::apply()
{
    // Currently no-op
    return true;
}

void DiskIO::load_current_settings()
{
    // Implementation to load current diskio settings would go here
}