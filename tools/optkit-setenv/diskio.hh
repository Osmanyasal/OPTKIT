#pragma once

#include "module.hh"

struct DiskIO : public Module
{
    std::string io_scheduler;  // none, mq-deadline, cfq, bfq
    std::string mount_path;    // mount point path
    std::string mount_options; // e.g., noatime,nobarrier
    int64_t aio_max_nr;        // max aio requests
    bool sync_disk;            // true or false
    bool use_direct_io;        // true or false

    std::string to_string() const override;
    bool is_valid() const override;
    bool apply() override;
    void load_current_settings(pid_t pid) override;
};

inline std::ostream &operator<<(std::ostream &os, const DiskIO &disk)
{
    return os << disk.to_string();
}