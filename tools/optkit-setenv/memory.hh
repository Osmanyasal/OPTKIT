#pragma once
#include "module.hh"

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
    static std::string to_string_thp_mode(THPMode mode);
    static THPMode from_string_thp_mode(const std::string &mode_str);
    static std::string to_string_malloc_backend(Backend backend);
    static Backend from_string_malloc_backend(const std::string &backend_str);

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
    Backend get_malloc_backend() const;
    bool set_hugepages_count(int64_t count);
    bool set_arena_max(int64_t max);
    bool set_swappiness(int64_t value);
    bool set_oom_kill_task(int64_t score, pid_t pid);
    bool drop_caches_now();
    bool set_mlock_all(bool enable);

    std::string to_string() const override;
    bool is_valid() const override;
    bool apply() override;
    void load_current_settings(pid_t pid) override;
};

inline std::ostream &operator<<(std::ostream &os, const Memory &mem)
{
    return os << mem.to_string();
}
