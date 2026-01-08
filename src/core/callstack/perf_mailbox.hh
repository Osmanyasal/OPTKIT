#pragma once

#include "utils/environment_config.hh"

#if OPTKIT_ENV_LIB_PERF_EVENT

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <dirent.h>

#include <dlfcn.h>
#include <cxxabi.h>

#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace optkit::callstack
{
    namespace proc
    {
        inline int32_t effective_pid(int32_t pid)
        {
            return (pid != 0) ? pid : static_cast<int32_t>(::getpid());
        }

        inline std::string task_dir(int32_t pid)
        {
            return "/proc/" + std::to_string(effective_pid(pid)) + "/task";
        }

        inline std::string maps_path(int32_t pid)
        {
            return "/proc/" + std::to_string(effective_pid(pid)) + "/maps";
        }
    } // namespace proc

    class ThreadBuffer // MailBox
    {
    public:
        ThreadBuffer() = default;
        ThreadBuffer(const ThreadBuffer &) = delete;
        ThreadBuffer &operator=(const ThreadBuffer &) = delete;

        ~ThreadBuffer()
        {
            shutdown();
        }

        bool init(uint32_t sample_freq)
        {
            const int32_t tid = static_cast<int32_t>(syscall(SYS_gettid));
            // std::cout << "Initializing ThreadBuffer for TID: " << tid << "\n";
            return init_for_tid(tid, sample_freq);
        }

        bool init_for_tid(int32_t tid, uint32_t sample_freq)
        {
            // std::cout << "init for tid: " << tid << "\n";
            this->thread_id = tid;

            const size_t page_size = static_cast<size_t>(sysconf(_SC_PAGESIZE));
            const size_t mmap_pages = 1; // keep it small; one buffer per thread
            data_size = page_size * mmap_pages;
            mmap_size = page_size * (1 + mmap_pages);

            perf_event_attr pe;
            std::memset(&pe, 0, sizeof(pe));

            pe.type = PERF_TYPE_SOFTWARE;
            pe.config = PERF_COUNT_SW_CPU_CLOCK;
            pe.sample_freq = sample_freq;
            pe.freq = 1;
            pe.disabled = 1;
            pe.exclude_kernel = 1;
            pe.inherit = 0; // The most important part, do not inherit to child threads let each thread have its own buffer !!
            pe.sample_type = PERF_SAMPLE_IP | PERF_SAMPLE_CALLCHAIN;
            pe.wakeup_events = 1;

            fd = static_cast<int32_t>(syscall(__NR_perf_event_open, &pe, this->thread_id, -1, -1, 0));
            if (fd < 0)
                return false;

            void *mmap_result = mmap(nullptr, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (mmap_result == MAP_FAILED)
            {
                ::close(fd);
                fd = -1;
                return false;
            }

            metadata = reinterpret_cast<perf_event_mmap_page *>(mmap_result);
            data_buffer = reinterpret_cast<char *>(mmap_result) + page_size;

            ioctl(fd, PERF_EVENT_IOC_RESET, 0);
            ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
            return true;
        }

        void shutdown()
        {
            if (fd != -1)
            {
                ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
                ::close(fd);
                fd = -1;
            }
            if (metadata != nullptr)
            {
                munmap(metadata, mmap_size);
                metadata = nullptr;
                data_buffer = nullptr;
                mmap_size = 0;
                data_size = 0;
            }
        }

        void read_data(uint64_t offset, void *dest, size_t len) const
        {
            const uint64_t real_offset = offset % data_size;
            if (real_offset + len <= data_size)
            {
                std::memcpy(dest, data_buffer + real_offset, len);
            }
            else
            {
                const size_t first_chunk = static_cast<size_t>(data_size - real_offset);
                std::memcpy(dest, data_buffer + real_offset, first_chunk);
                std::memcpy(reinterpret_cast<char *>(dest) + first_chunk, data_buffer, len - first_chunk);
            }
        }

        int32_t fd{-1};
        perf_event_mmap_page *metadata{nullptr};
        char *data_buffer{nullptr};
        size_t mmap_size{0};
        size_t data_size{0};
        int32_t thread_id{0};
    };

    class Registry // phone book for all mailboxes, Since every thread has its own private buffer, the Sweeper needs a way to find them
    {
    public:
        void register_buffer(ThreadBuffer *buf)
        {
            std::lock_guard<std::mutex> l(lock);
            active_buffers.push_back(buf);
        }

        void unregister_buffer(ThreadBuffer *buf)
        {
            std::lock_guard<std::mutex> l(lock);
            active_buffers.erase(
                std::remove(active_buffers.begin(), active_buffers.end(), buf),
                active_buffers.end());
        }

        std::vector<ThreadBuffer *> get_all_buffers()
        {
            std::lock_guard<std::mutex> l(lock);
            return active_buffers;
        }

    private:
        std::mutex lock;
        std::vector<ThreadBuffer *> active_buffers;
    };

    inline Registry &global_registry()
    {
        static Registry reg;
        return reg;
    }

    class ThreadAttachManager
    {
    public:
        static std::vector<int32_t> list_tids(int32_t target_pid)
        {
            std::vector<int32_t> tids;

            const std::string dir_path = proc::task_dir(target_pid);
            DIR *dir = opendir(dir_path.c_str());
            if (!dir)
                return tids;

            while (dirent *ent = readdir(dir))
            {
                if (!std::isdigit(static_cast<unsigned char>(ent->d_name[0])))
                    continue;
                const int32_t tid = static_cast<int32_t>(std::strtol(ent->d_name, nullptr, 10));
                if (tid > 0)
                    tids.push_back(tid);
            }

            closedir(dir);
            return tids;
        }

        static void refresh(uint32_t sample_freq, int32_t exclude_tid, int32_t target_pid)
        {
            // if you switch target process, clear all owned buffers
            if (target_pid != current_pid)
            {
                for (auto &kv : owned)
                    global_registry().unregister_buffer(kv.second.get());
                owned.clear();
                current_pid = target_pid;
            }

            // List all TIDs in the target process.
            const auto tids = list_tids(target_pid);

            std::unordered_set<int32_t> alive;
            alive.reserve(tids.size());
            for (int32_t tid : tids)
                alive.insert(tid);

            // Build a set of already-registered TIDs (TLS samplers + attached ones).
            std::unordered_set<int32_t> registered;
            auto buffers = global_registry().get_all_buffers();
            registered.reserve(buffers.size());
            for (auto *buf : buffers)
            {
                if (buf)
                    registered.insert(buf->thread_id);
            }

            // Remove stale buffers (threads that exited).
            for (auto it = owned.begin(); it != owned.end();)
            {
                if (alive.find(it->first) == alive.end())
                {
                    global_registry().unregister_buffer(it->second.get());
                    it = owned.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            // Attach buffers for new threads we haven't seen.
            for (int32_t tid : tids)
            {
                if (target_pid == 0 && tid == exclude_tid)
                    continue;
                if (registered.find(tid) != registered.end())
                    continue;

                std::unique_ptr<ThreadBuffer> buf(new ThreadBuffer());
                if (!buf->init_for_tid(tid, sample_freq))
                    continue;

                ThreadBuffer *raw = buf.get();
                if (owned.find(tid) != owned.end())
                    continue;
                owned.emplace(tid, std::move(buf));
                global_registry().register_buffer(raw);
            }
        }

    private:
        static int32_t current_pid;
        static std::unordered_map<int32_t, std::unique_ptr<ThreadBuffer>> owned;
    };
    int32_t ThreadAttachManager::current_pid = 0;
    std::unordered_map<int32_t, std::unique_ptr<ThreadBuffer>> ThreadAttachManager::owned;

    class ThreadLocalSampler
    {
    public:
        explicit ThreadLocalSampler(uint32_t sample_freq)
        {
            if (buffer.init(sample_freq))
                global_registry().register_buffer(&buffer);
        }

        ~ThreadLocalSampler()
        {
            global_registry().unregister_buffer(&buffer);
        }

        ThreadBuffer buffer;
    };

    inline ThreadLocalSampler &tls_sampler(uint32_t sample_freq)
    {
        // Initialized on first use in the thread. This matches the sample's "touch_profiler" idea.
        thread_local ThreadLocalSampler sampler(sample_freq);
        return sampler;
    }

    inline void touch_thread(uint32_t sample_freq)
    {
        (void)tls_sampler(sample_freq);
    }

    class Symbolizer
    {
    public:
        void set_target_pid(int32_t pid)
        {
            target_pid = pid;
            cache.clear();
            maps_cache.clear();
            last_maps_load = std::chrono::steady_clock::time_point{};
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
        struct MapEntry
        {
            uint64_t start{0};
            uint64_t end{0};
            uint64_t file_offset{0};
            std::string path;
        };

        void load_maps()
        {
            maps_cache.clear();

            const std::string maps_path = proc::maps_path(target_pid);

            std::ifstream in(maps_path);
            if (!in)
                return;

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
                    maps_cache.push_back(std::move(e));
            }
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
            load_maps();
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

    class Sweeper // The Postman
    {
    public:
        Sweeper() = default;
        Sweeper(const Sweeper &) = delete;
        Sweeper &operator=(const Sweeper &) = delete;

        void set_sample_freq(uint32_t hz)
        {
            sample_freq_hz.store(hz);
        }

        void set_target_pid(int32_t pid)
        {
            target_pid.store(pid);
            symbolizer.set_target_pid(pid);
        }

        void start()
        {
            bool expected = false;
            if (!running.compare_exchange_strong(expected, true))
                return;

            stop_flag = false;
            worker = std::thread([this]()
                                 { this->run(); });
        }

        void stop()
        {
            if (!running.load())
                return;

            stop_flag = true;
            if (worker.joinable())
                worker.join();
            running = false;
        }

        void reset_counts()
        {
            std::lock_guard<std::mutex> l(counts_lock);
            counts.clear();
        }

        std::unordered_map<std::string, uint64_t> consume_counts(bool reset_after)
        {
            std::lock_guard<std::mutex> l(counts_lock);
            auto out = counts;
            if (reset_after)
                counts.clear();
            return out;
        }

    private:
        void run()
        {
            const int32_t sweeper_tid = static_cast<int32_t>(syscall(SYS_gettid));
            // std::cout << "Sweeper started in TID: " << sweeper_tid << "\n";
            auto last_refresh = std::chrono::steady_clock::now();

            while (!stop_flag.load())
            {
                // Periodically auto-attach to new threads (e.g., OpenMP workers).
                const auto now = std::chrono::steady_clock::now();
                if (now - last_refresh > std::chrono::milliseconds(25))
                {
                    ThreadAttachManager::refresh(sample_freq_hz.load(), sweeper_tid, target_pid.load());
                    last_refresh = now;
                }

                auto buffers = global_registry().get_all_buffers();

                for (auto *buf : buffers)
                {
                    if (buf == nullptr || buf->metadata == nullptr)
                        continue;

                    uint64_t head = buf->metadata->data_head;
                    uint64_t tail = buf->metadata->data_tail;
                    __sync_synchronize();

                    if (tail >= head)
                        continue;

                    while (tail < head)
                    {
                        perf_event_header header;
                        buf->read_data(tail, &header, sizeof(header));

                        if (header.size == 0)
                            break;

                        if (header.type == PERF_RECORD_SAMPLE)
                        {
                            size_t offset = sizeof(header);

                            uint64_t ip = 0;
                            buf->read_data(tail + offset, &ip, sizeof(ip));
                            offset += sizeof(ip);

                            uint64_t nr = 0;
                            buf->read_data(tail + offset, &nr, sizeof(nr));
                            offset += sizeof(nr);

                            // Filter obviously invalid sizes.
                            if (nr > 1024)
                                nr = 1024;

                            std::vector<uint64_t> chain;
                            chain.reserve(static_cast<size_t>(nr));

                            for (uint64_t i = 0; i < nr; i++)
                            {
                                uint64_t chain_ip = 0;
                                buf->read_data(tail + offset, &chain_ip, sizeof(chain_ip));
                                offset += sizeof(chain_ip);

                                // Similar heuristic to your sample.
                                if (chain_ip != 0 && chain_ip < 0x7fffffffffffULL)
                                    chain.push_back(chain_ip);
                            }

                            // If callchain is missing (or got filtered), fall back to the sample IP.
                            if (chain.empty() && ip != 0)
                                chain.push_back(ip);

                            if (!chain.empty())
                            {
                                // perf callchain is usually leaf->root; FlameGraph expects root->leaf.
                                std::reverse(chain.begin(), chain.end());

                                std::string folded;
                                folded.reserve(256);
                                for (size_t i = 0; i < chain.size(); i++)
                                {
                                    if (i)
                                        folded.push_back(';');
                                    folded += symbolizer.symbolize(chain[i]);
                                }

                                if (!folded.empty())
                                {
                                    std::lock_guard<std::mutex> l(counts_lock);
                                    counts[folded] += 1;
                                }
                            }
                        }

                        tail += header.size;
                    }

                    buf->metadata->data_tail = tail;
                    __sync_synchronize();
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

    private:
        std::atomic<bool> running{false};
        std::atomic<bool> stop_flag{false};
        std::atomic<uint32_t> sample_freq_hz{100};
        std::atomic<int32_t> target_pid{0};
        std::thread worker;

        std::mutex counts_lock;
        std::unordered_map<std::string, uint64_t> counts;
        Symbolizer symbolizer;
    };

} // namespace optkit::callstack

#endif
