#pragma once

#include "core/callstack/utils.hh"
namespace optkit::callstack
{
    class Symbolizer
    {
    public:
        void set_target_pid(int32_t pid)
        {
            target_pid = pid;
            cache.clear();
            maps_cache.clear();
            last_maps_load = std::chrono::steady_clock::time_point{};

            // Snapshot mappings early while the target is likely still alive.
            // This is important for short-lived targets (e.g., optkit-cli stat) where
            // symbolization happens after the process exits and /proc/<pid>/maps is gone.
            load_maps();
            last_maps_load = std::chrono::steady_clock::now();
        }

        // Best-effort: refresh (or snapshot) /proc/<pid>/maps while the target is alive.
        // Safe to call frequently; internally throttled.
        void prefetch_remote_maps()
        {
            const int32_t self_pid = static_cast<int32_t>(::getpid());
            if (target_pid != 0 && target_pid != self_pid)
                ensure_maps_loaded();
        }

        std::string symbolize(uint64_t addr)
        {
            const int32_t self_pid = static_cast<int32_t>(::getpid());
            const bool is_remote = (target_pid != 0 && target_pid != self_pid);

            // A tiny cache helps a lot (dladdr + demangle are expensive).
            {
                auto it = cache.find(addr);
                if (it != cache.end())
                    return it->second;
            }

            const std::string result = is_remote ? symbolize_remote(addr) : symbolize_self(addr);
            cache.emplace(addr, result);
            return result;
        }

    private:
        static std::string shell_single_quote(const std::string &s)
        {
            // Safe single-quoting for POSIX shells: ' -> '\''
            std::string out;
            out.reserve(s.size() + 2);
            out.push_back('\'');
            for (char c : s)
            {
                if (c == '\'')
                    out.append("'\\''");
                else
                    out.push_back(c);
            }
            out.push_back('\'');
            return out;
        }

        static std::string addr2line_function(const std::string &module_path, uint64_t module_addr)
        {
            // Best-effort: return a demangled function name, or empty on failure.
            // Note: Requires addr2line to be installed, and may require debuginfo packages
            // for system libraries.
            static int addr2line_usable = -1; // -1 unknown, 0 no, 1 yes
            if (addr2line_usable == 0)
                return {};

            char addr_buf[32];
            std::snprintf(addr_buf, sizeof(addr_buf), "0x%llx", static_cast<unsigned long long>(module_addr));

            // addr2line prints:
            //   <function name>\n
            //   <file:line>\n
            const std::string cmd = "addr2line -f -C -e " + shell_single_quote(module_path) + " " + addr_buf + " 2>/dev/null";
            FILE *fp = ::popen(cmd.c_str(), "r");
            if (!fp)
            {
                addr2line_usable = 0;
                return {};
            }

            char line[512];
            std::string func;
            if (std::fgets(line, sizeof(line), fp))
                func = line;

            ::pclose(fp);

            // Trim trailing whitespace/newlines.
            while (!func.empty() && std::isspace(static_cast<unsigned char>(func.back())))
                func.pop_back();

            addr2line_usable = 1;
            if (func.empty() || func == "??")
                return {};
            return func;
        }

        struct MapEntry
        {
            uint64_t start{0};
            uint64_t end{0};
            uint64_t file_offset{0};
            std::string path;
        };

        void load_maps()
        {
            const std::string maps_path = proc::maps_path(target_pid);

            std::ifstream in(maps_path);
            if (!in)
                return;

            std::vector<MapEntry> new_cache;
            new_cache.reserve(256);

            std::string line;
            while (std::getline(in, line))
            {
                // Format: start-end perms offset dev inode pathname
                // pathname is optional.
                std::istringstream iss(line);
                std::string range;
                std::string perms;
                std::string offset_hex;
                std::string dev;
                std::string inode;
                if (!(iss >> range >> perms >> offset_hex >> dev >> inode))
                    continue;

                std::string path;
                std::getline(iss, path);
                // trim leading spaces
                while (!path.empty() && std::isspace(static_cast<unsigned char>(path.front())))
                    path.erase(path.begin());

                const size_t dash = range.find('-');
                if (dash == std::string::npos)
                    continue;

                const std::string start_hex = range.substr(0, dash);
                const std::string end_hex = range.substr(dash + 1);

                MapEntry e;
                e.start = std::strtoull(start_hex.c_str(), nullptr, 16);
                e.end = std::strtoull(end_hex.c_str(), nullptr, 16);
                e.file_offset = std::strtoull(offset_hex.c_str(), nullptr, 16);
                e.path = std::move(path);
                if (e.start < e.end)
                    new_cache.push_back(std::move(e));
            }

            // Only replace the cache after successfully parsing the maps file.
            // This preserves an earlier snapshot if the target exits later.
            if (!new_cache.empty())
                maps_cache = std::move(new_cache);
        }

        const MapEntry *find_map(uint64_t addr) const
        {
            for (const auto &m : maps_cache)
            {
                if (addr >= m.start && addr < m.end)
                    return &m;
            }
            return nullptr;
        }

        void ensure_maps_loaded()
        {
            const auto now = std::chrono::steady_clock::now();
            if (!maps_cache.empty() && (now - last_maps_load) <= std::chrono::seconds(1))
                return;

            // Reload periodically while the target is alive, but keep the last good snapshot
            // if reloading fails (e.g., after the process has exited).
            const auto before_size = maps_cache.size();
            load_maps();
            if (!maps_cache.empty() || before_size != 0)
                last_maps_load = now;
        }

        std::string symbolize_remote(uint64_t addr)
        {
            ensure_maps_loaded();

            const MapEntry *m = find_map(addr);
            if (m && !m->path.empty())
            {
                const char *slash = std::strrchr(m->path.c_str(), '/');
                const char *base = slash ? slash + 1 : m->path.c_str();
                const uint64_t file_off = m->file_offset + (addr - m->start);

                const std::string fn = addr2line_function(m->path, file_off);
                if (!fn.empty())
                    return std::string(base) + "!" + fn;

                char buf[256];
                std::snprintf(buf, sizeof(buf), "%s+0x%llx", base, static_cast<unsigned long long>(file_off));
                return buf;
            }

            char buf[64];
            std::snprintf(buf, sizeof(buf), "0x%llx", static_cast<unsigned long long>(addr));
            return buf;
        }

        std::string symbolize_self(uint64_t addr)
        {
            Dl_info info;
            std::memset(&info, 0, sizeof(info));
            std::string result = "?";

            if (!dladdr(reinterpret_cast<void *>(addr), &info))
                return result;

            if (info.dli_sname)
            {
                int status = -1;
                std::unique_ptr<char, void (*)(void *)> nice_name{
                    abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status),
                    std::free};
                result = (status == 0 && nice_name) ? nice_name.get() : info.dli_sname;

                const size_t paren = result.find('(');
                if (paren != std::string::npos)
                    result = result.substr(0, paren);
                return result;
            }

            if (info.dli_fname && info.dli_fbase)
            {
                const char *slash = std::strrchr(info.dli_fname, '/');
                const char *base = slash ? slash + 1 : info.dli_fname;
                const uint64_t off = addr - reinterpret_cast<uint64_t>(info.dli_fbase);

                const std::string fn = addr2line_function(info.dli_fname, off);
                if (!fn.empty())
                    return std::string(base) + "!" + fn;

                char buf[256];
                std::snprintf(buf, sizeof(buf), "%s+0x%llx", base, static_cast<unsigned long long>(off));
                result = buf;
            }

            return result;
        }

        std::unordered_map<uint64_t, std::string> cache;
        int32_t target_pid{0};
        std::vector<MapEntry> maps_cache;
        std::chrono::steady_clock::time_point last_maps_load{};
    };
}