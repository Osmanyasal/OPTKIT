#pragma once
#include "module.hh"
#include "helper.hh"

struct Memory : public Module
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
    bool set_oom_kill_task(int64_t score, pid_t pid);
    bool drop_caches_now();
    bool set_mlock_all(bool enable);

    std::string to_string() const override;
    std::string to_string_thp_mode(THPMode mode) const;
    THPMode from_string_thp_mode(const std::string &mode_str) const;
    std::string to_string_malloc_backend(Backend backend) const;
    Backend from_string_malloc_backend(const std::string &backend_str) const;
    bool is_valid() const override;
    bool apply() override;
};

inline std::ostream &operator<<(std::ostream &os, const Memory &mem)
{
    return os << mem.to_string();
}
