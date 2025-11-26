#pragma once

#include "helper.hh"
struct DiskIO
{
    std::string io_scheduler;  // none, mq-deadline, cfq, bfq
    std::string mount_path;    // mount point path
    std::string mount_options; // e.g., noatime,nobarrier
    int64_t aio_max_nr;        // max aio requests
    bool sync_disk;            // true or false
    bool use_direct_io;        // true or false

    std::string to_string() const
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
    bool is_valid();
};

inline std::ostream &operator<<(std::ostream &os, const DiskIO &disk)
{
    return os << disk.to_string();
}