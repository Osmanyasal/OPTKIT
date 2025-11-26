#include "memory.hh"

bool Memory::set_thp_mode(THPMode mode)
{
    std::string mode_str = to_string_thp_mode(mode);
    optkit::utils::write_file("/sys/kernel/mm/transparent_hugepage/enabled", mode_str);
    this->thp_mode = mode;
    return true;
}

bool Memory::set_malloc_backend(Backend backend)
{
    // Set LD_PRELOAD environment variable before launching app

    switch (backend)
    {
    case Backend::GLIBC:
        unsetenv("LD_PRELOAD");
        break;
    case Backend::JEMALLOC:
        setenv("LD_PRELOAD", "/usr/lib/x86_64-linux-gnu/libjemalloc.so.2", 1);
        break;
    case Backend::TCMALLOC:
        setenv("LD_PRELOAD", "/usr/lib/x86_64-linux-gnu/libtcmalloc.so.4", 1);
        break;
    }
    this->malloc_backend = backend;
    return true;
}

bool Memory::set_hugepages_count(int64_t count)
{
    optkit::utils::write_file(
        "/sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages",
        std::to_string(count));

    this->hugepages_count = count;
    return true;
}

bool Memory::set_arena_max(int64_t max)
{
    setenv("MALLOC_ARENA_MAX", std::to_string(max).c_str(), 1);
    this->arena_max = max;
    return true;
}

bool Memory::set_swappiness(int64_t value)
{
    optkit::utils::write_file("/proc/sys/vm/swappiness", std::to_string(value));
    this->swappiness = value;
    return true;
}

bool Memory::set_oom_kill_task(int64_t score, pid_t pid)
{
    optkit::utils::write_file("/proc/" + std::to_string(pid) + "/oom_score_adj", std::to_string(score));
    this->oom_kill_task = score;
    return true;
}

bool Memory::drop_caches_now()
{
    // Flush pending writes first
    sync();

    // 1 = pagecache, 2 = dentries/inodes, 3 = both
    optkit::utils::write_file("/proc/sys/vm/drop_caches", "3");
    this->drop_caches = true;
    return true;
}

bool Memory::set_mlock_all(bool enable)
{
    if (enable)
    {
        if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0)
        {
            std::cerr << "mlockall failed: " << strerror(errno) << "\n";
            return false;
        }
    }
    else
    {
        munlockall();
    }

    this->mlock_all = enable;
    return true;
}