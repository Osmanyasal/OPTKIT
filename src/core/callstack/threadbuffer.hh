#pragma once

#include "utils/environment_config.hh"

#if OPTKIT_ENV_LIB_PERF_EVENT

#include "core/callstack/utils.hh"
#include <iostream>

namespace optkit::callstack
{
    class ThreadBuffer;
    class Registry // phone book for all mailboxes, Since every thread has its own private buffer, the Postman needs a way to find them
    {
    public:
        void register_buffer(ThreadBuffer *buf)
        {
            std::lock_guard<std::mutex> l(lock);
            active_buffers.insert(buf);
        }

        void unregister_buffer(ThreadBuffer *buf)
        {
            std::lock_guard<std::mutex> l(lock);
            active_buffers.erase(buf);
        }

        std::unordered_set<ThreadBuffer *> get_all_buffers()
        {
            std::lock_guard<std::mutex> l(lock);
            return active_buffers;
        }

    private:
        std::mutex lock;
        std::unordered_set<ThreadBuffer *> active_buffers;
    };

    inline Registry &global_registry()
    {
        static Registry reg;
        return reg;
    }

    class ThreadBuffer // MailBox per software thread.
    {
    public:
        ThreadBuffer() = default;
        ThreadBuffer(const ThreadBuffer &) = delete;
        ThreadBuffer &operator=(const ThreadBuffer &) = delete;

        ~ThreadBuffer()
        {
            shutdown();
        }

        bool init_for_tid(int32_t tid, uint32_t sample_freq)
        {
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
            // pe.exclude_kernel = 1;
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

        void refresh(uint32_t sample_freq, int32_t exclude_tid, int32_t target_pid)
        {
            // if you switch target process, clear all owned buffers
            if (target_pid != ThreadAttachManager::current_pid)
            {
                for (auto &kv : owned)
                    global_registry().unregister_buffer(kv.second.get());
                owned.clear();
                ThreadAttachManager::current_pid = target_pid;
            }

            // List all TIDs in the target process.
            const auto tids = ThreadAttachManager::list_tids(target_pid);

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
                if (tid == exclude_tid)
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
        int32_t current_pid = -1;
        std::unordered_map<int32_t, std::unique_ptr<ThreadBuffer>> owned;
    };
} // namespace optkit::callstack

#endif
