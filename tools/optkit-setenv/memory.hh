#pragma once
#include "helper.hh"
struct Memory
{
    enum class THPMode
    {
        NEVER,
        ALWAYS,
        MADVISE
    };
    enum class Backend
    {
        GLIBC,
        JEMALLOC,
        TCMALLOC
    };
    THPMode thp_mode;       // Transparent HugePages: never, always, madvise
    Backend malloc_backend; // glibc, jemalloc, tcmalloc
    int64_t hugepages_count;
    int64_t arena_max;
    int64_t swappiness;    // 0-100
    int64_t oom_kill_task; // 0 or 1
    bool drop_caches;
    bool mlock_all;

    // Setter methods
    bool set_thp_mode(THPMode mode);
    bool set_malloc_backend(Backend backend);
    bool set_hugepages_count(int64_t count);
    bool set_arena_max(int64_t max);
    bool set_swappiness(int64_t value);
    bool set_oom_kill_task(int64_t score);
    bool drop_caches_now();
    bool set_mlock_all(bool enable);

    bool is_valid();
    std::string to_string() const
    {
        std::ostringstream oss;
        oss << "Memory{thp=" << to_string_thp_mode(thp_mode)
            << ", malloc=" << malloc_backend
            << ", hugepages=" << hugepages_count
            << ", arena_max=" << arena_max
            << ", swappiness=" << swappiness
            << ", oom_kill=" << oom_kill_task
            << ", drop_caches=" << (drop_caches ? "yes" : "no")
            << ", mlock_all=" << (mlock_all ? "yes" : "no")
            << "}";
        return oss.str();
    }
    std::string to_string_thp_mode(THPMode mode) const
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
};

inline std::ostream &operator<<(std::ostream &os, const Memory &mem)
{
    return os << mem.to_string();
}
