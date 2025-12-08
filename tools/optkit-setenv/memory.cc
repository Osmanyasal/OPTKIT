#include "memory.hh"

const std::string Memory::name = "memory";

bool Memory::set_thp_mode(THPMode mode)
{
    std::string mode_str = to_string_thp_mode(mode);
    optkit::utils::write_file("/sys/kernel/mm/transparent_hugepage/enabled", mode_str);
    this->thp_mode = mode;
    return true;
}
bool Memory::set_malloc_backend(Backend backend)
{
    const char *current_preload = getenv("LD_PRELOAD");
    std::string new_preload;

    // Parse existing LD_PRELOAD and remove any malloc libraries
    if (current_preload != nullptr)
    {
        std::string preload_str(current_preload);
        std::vector<std::string> libs;

        // Split by ':' delimiter
        size_t pos = 0;
        while ((pos = preload_str.find(':')) != std::string::npos)
        {
            std::string lib = preload_str.substr(0, pos);
            // Keep libraries that aren't malloc backends
            if (lib.find("jemalloc") == std::string::npos &&
                lib.find("tcmalloc") == std::string::npos)
            {
                libs.push_back(lib);
            }
            preload_str.erase(0, pos + 1);
        }
        // Handle last library (or only library if no ':')
        if (!preload_str.empty() &&
            preload_str.find("jemalloc") == std::string::npos &&
            preload_str.find("tcmalloc") == std::string::npos)
        {
            libs.push_back(preload_str);
        }

        // Reconstruct LD_PRELOAD without malloc libraries
        for (size_t i = 0; i < libs.size(); ++i)
        {
            new_preload += libs[i];
            if (i < libs.size() - 1)
            {
                new_preload += ":";
            }
        }
    }

    // Add the requested malloc backend
    switch (backend)
    {
    case Backend::GLIBC:
        // Just use the cleaned preload (no malloc library added)
        break;
    case Backend::JEMALLOC:
        if (!new_preload.empty())
            new_preload += ":";
        new_preload += "/usr/lib/x86_64-linux-gnu/libjemalloc.so.2";
        break;
    case Backend::TCMALLOC:
        if (!new_preload.empty())
            new_preload += ":";
        new_preload += "/usr/lib/x86_64-linux-gnu/libtcmalloc.so.4";
        break;
    }

    // Set or unset LD_PRELOAD
    if (new_preload.empty())
    {
        unsetenv("LD_PRELOAD");
    }
    else
    {
        setenv("LD_PRELOAD", new_preload.c_str(), 1);
    }

    this->malloc_backend = backend;
    return true;
}

Memory::Backend Memory::get_malloc_backend() const
{
    const char *preload = getenv("LD_PRELOAD");

    if (preload == nullptr || strlen(preload) == 0)
    {
        return Backend::GLIBC; // No LD_PRELOAD means default glibc
    }

    std::string preload_str(preload);

    // Check for jemalloc
    if (preload_str.find("jemalloc") != std::string::npos)
    {
        return Backend::JEMALLOC;
    }

    // Check for tcmalloc
    if (preload_str.find("tcmalloc") != std::string::npos)
    {
        return Backend::TCMALLOC;
    }

    // Default to glibc if no recognized malloc library found
    return Backend::GLIBC;
}

bool Memory::set_hugepages_count(int64_t count)
{
    const std::string path = "/sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages";
    optkit::utils::write_file(path, std::to_string(count));
    auto read = std::stoll(optkit::utils::str_trim(optkit::utils::read_file(path)));
    if (read == 0)
    {
        return false;
    }

    this->hugepages_count = read;
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

void Memory::load_current_settings(pid_t pid)
{
    std::string huge_pages = optkit::utils::read_file("/sys/kernel/mm/transparent_hugepage/enabled");
    auto tokens = optkit::utils::str_split(huge_pages, " ");
    for (const auto &token : tokens)
    {
        if (token.front() == '[' && token.back() == ']')
        {
            std::string mode_str = token.substr(1, token.size() - 2);
            this->thp_mode = from_string_thp_mode(mode_str);
            break;
        }
    }

    this->malloc_backend = get_malloc_backend();

    // Load hugepages count
    std::string hugepages_str = optkit::utils::read_file("/sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages");
    this->hugepages_count = std::stoll(hugepages_str);

    // Load arena_max from environment
    const char *arena_env = getenv("MALLOC_ARENA_MAX");
    this->arena_max = arena_env ? std::stoll(arena_env) : 0;

    // Load swappiness
    std::string swappiness_str = optkit::utils::read_file("/proc/sys/vm/swappiness");
    this->swappiness = std::stoll(swappiness_str);

    // Load OOM score for current process
    std::string oom_str = optkit::utils::read_file("/proc/" + std::to_string(pid) + "/oom_score_adj");
    this->oom_kill_task = std::stoll(oom_str);

    // drop_caches and mlock_all are write-only, can't read current state
    this->drop_caches = false;
    this->mlock_all = false;
}

nlohmann::json Memory::to_json() const
{
    nlohmann::json j;
    j["thp_mode"] = to_string_thp_mode(thp_mode);
    j["malloc_backend"] = to_string_malloc_backend(malloc_backend);
    j["hugepages_count"] = hugepages_count;
    j["arena_max"] = arena_max;
    j["swappiness"] = swappiness;
    j["oom_kill_task"] = oom_kill_task;
    j["drop_caches"] = drop_caches;
    j["mlock_all"] = mlock_all;
    return j;
}
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
std::string Memory::to_string_thp_mode(THPMode mode)
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
Memory::THPMode Memory::from_string_thp_mode(const std::string &mode_str)
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
std::string Memory::to_string_malloc_backend(Backend backend)
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
Memory::Backend Memory::from_string_malloc_backend(const std::string &backend_str)
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
std::string Memory::possible_values() const
{
    std::ostringstream oss;
    oss << "\tTHP Modes: never, always, madvise\n"
        << "\tMalloc Backends: glibc, jemalloc, tcmalloc\n"
        << "\tHugepages Count: non-negative integer\n"
        << "\tArena Max: non-negative integer\n"
        << "\tSwappiness: 0-100\n"
        << "\tOOM Kill Task Score: -1000 - 1000\n"
        << "\tDrop Caches: true, false\n"
        << "\tMlock All: true, false";
    return oss.str();
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

bool Memory::apply(pid_t pid)
{
    // Apply THP mode
    set_thp_mode(thp_mode);

    // Apply malloc backend
    set_malloc_backend(malloc_backend);

    // Apply hugepages count
    set_hugepages_count(hugepages_count);

    // Apply arena max
    set_arena_max(arena_max);

    // Apply swappiness
    set_swappiness(swappiness);

    // Apply OOM kill task score
    set_oom_kill_task(oom_kill_task, pid);

    // Apply drop caches if requested
    if (drop_caches)
        drop_caches_now();

    // Apply mlock_all
    set_mlock_all(mlock_all);

    return true;
}