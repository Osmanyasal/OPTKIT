#include "memory.hh"

std::string Memory::to_string() const
{
    std::ostringstream oss;
    oss << "Memory{thp=" << to_string_thp_mode(thp_mode)
        << ", malloc=" << to_string_malloc_backend(malloc_backend)
        << ", hugepages=" << hugepages_count
        << ", arena_max=" << arena_max
        << ", swappiness=" << swappiness
        << ", oom_kill=" << oom_kill_task
        << ", drop_caches=" << (drop_caches ? "yes" : "no")
        << ", mlock_all=" << (mlock_all ? "yes" : "no")
        << "}";
    return oss.str();
}
std::string Memory::to_string_thp_mode(THPMode mode) const
{
    switch (mode)
    {
    case THPMode::NEVER:
        return "never";
    case THPMode::ALWAYS:
        return "always";
    case THPMode::MADVISE:
        return "madvise";
    default:
        return "unknown";
    }
}
Memory::THPMode Memory::from_string_thp_mode(const std::string &mode_str) const
{
    if (mode_str == "never")
        return THPMode::NEVER;
    else if (mode_str == "always")
        return THPMode::ALWAYS;
    else if (mode_str == "madvise")
        return THPMode::MADVISE;
    else
        throw std::invalid_argument("Invalid THP mode string: " + mode_str);
}
std::string Memory::to_string_malloc_backend(Backend backend) const
{
    switch (backend)
    {
    case Backend::GLIBC:
        return "glibc";
    case Backend::JEMALLOC:
        return "jemalloc";
    case Backend::TCMALLOC:
        return "tcmalloc";
    default:
        return "unknown";
    }
}
Memory::Backend Memory::from_string_malloc_backend(const std::string &backend_str) const
{
    if (backend_str == "glibc")
        return Backend::GLIBC;
    else if (backend_str == "jemalloc")
        return Backend::JEMALLOC;
    else if (backend_str == "tcmalloc")
        return Backend::TCMALLOC;
    else
        throw std::invalid_argument("Invalid malloc backend string: " + backend_str);
}

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

bool Memory::is_valid() const
{
    bool result = true;

    if (hugepages_count < 0)
    {
        OPTKIT_WARN("hugepages_count cannot be negative: {}", hugepages_count);
        result = false;
    }

    if (arena_max < 0)
    {
        OPTKIT_WARN("arena_max cannot be negative: {}", arena_max);
        result = false;
    }

    if (swappiness < 0 || swappiness > 100)
    {
        OPTKIT_WARN("swappiness must be between 0 and 100: {}", swappiness);
        result = false;
    }

    if (oom_kill_task < -1000 || oom_kill_task > 1000)
    {
        OPTKIT_WARN("oom_kill_task must be between -1000 and 1000: {}", oom_kill_task);
        result = false;
    }

    return result;
}

bool Memory::apply()
{
    // Apply THP mode
    set_thp_mode(thp_mode);

    // Apply hugepages count
    set_hugepages_count(hugepages_count);

    // Apply arena max
    set_arena_max(arena_max);

    // Apply swappiness
    set_swappiness(swappiness);

    // Apply drop caches if requested
    if (drop_caches)
        drop_caches_now();

    // Apply mlock_all
    set_mlock_all(mlock_all);

    return true;
}
void Memory::load_current_settings()
{
    // Implementation to load current memory settings would go here
}