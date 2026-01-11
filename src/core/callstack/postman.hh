#pragma once

#include "core/callstack/threadbuffer.hh"
#include "core/callstack/symbolizer.hh"
namespace optkit::callstack
{
    class Postman // The Postman
    {
    private:
        static inline uint64_t fnv1a_update(uint64_t h, const void *data, size_t len)
        {
            constexpr uint64_t FNV_PRIME = 1099511628211ULL;
            const auto *p = static_cast<const unsigned char *>(data);
            for (size_t i = 0; i < len; ++i)
            {
                h ^= static_cast<uint64_t>(p[i]);
                h *= FNV_PRIME;
            }
            return h;
        }

        static inline uint64_t splitmix64(uint64_t x)
        {
            x += 0x9e3779b97f4a7c15ULL;
            x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
            x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
            return x ^ (x >> 31);
        }

    public:
        Postman() = default;
        Postman(const Postman &) = delete;
        Postman &operator=(const Postman &) = delete;
        Postman(const Postman &&) = delete;
        Postman &operator=(const Postman &&) = delete;

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
            key_to_stack.clear();
        }

        std::unordered_map<std::string, uint64_t> consume_counts(bool reset_after)
        {
            std::lock_guard<std::mutex> l(counts_lock);
            std::unordered_map<std::string, uint64_t> out;
            out.reserve(counts.size());
            for (const auto &kv : counts)
            {
                const auto it = key_to_stack.find(kv.first);
                if (it != key_to_stack.end())
                    out[it->second] += kv.second;
            }
            if (reset_after)
            {
                counts.clear();
                key_to_stack.clear();
            }
            return out;
        }

        std::string symbolize(uint64_t addr)
        {
            return symbolizer.symbolize(addr);
        }

    private:
        void run()
        {
            const int32_t postman_tid = static_cast<int32_t>(syscall(SYS_gettid));
            thread_attach_manager.refresh(sample_freq_hz.load(), postman_tid, target_pid.load());

            // Warm up remote maps early; fork/exec targets may not have stable mappings immediately.
            symbolizer.prefetch_remote_maps();

            // std::cout << "Postman started in TID: " << postman_tid << "\n";
            auto last_refresh = std::chrono::steady_clock::now();

            // init for first time
            while (!stop_flag.load())
            {
                // Periodically auto-attach to new threads (e.g., OpenMP workers).
                const auto now = std::chrono::steady_clock::now();
                if (now - last_refresh > std::chrono::milliseconds(25))
                {
                    thread_attach_manager.refresh(sample_freq_hz.load(), postman_tid, target_pid.load());
                    symbolizer.prefetch_remote_maps();
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

                                // Store raw addresses as semicolon-separated hex string, symbolization deferred to aggregate()
                                std::string address_chain;
                                address_chain.reserve(256);
                                uint64_t key_hash = 14695981039346656037ULL; // FNV-1a offset basis
                                for (size_t i = 0; i < chain.size(); i++)
                                {
                                    if (i)
                                    {
                                        address_chain.push_back(';');
                                        const char sep = ';';
                                        key_hash = fnv1a_update(key_hash, &sep, 1);
                                    }
                                    char hex_buf[2 + 16 + 1];
                                    const int n = std::snprintf(hex_buf, sizeof(hex_buf), "0x%016llx",
                                                                static_cast<unsigned long long>(chain[i]));
                                    if (n > 0)
                                    {
                                        address_chain.append(hex_buf, static_cast<size_t>(n));
                                        key_hash = fnv1a_update(key_hash, hex_buf, static_cast<size_t>(n));
                                    }
                                }

                                if (!address_chain.empty())
                                {
                                    std::lock_guard<std::mutex> l(counts_lock);
                                    uint64_t key = key_hash;
                                    auto it = key_to_stack.find(key);
                                    if (it == key_to_stack.end())
                                    {
                                        key_to_stack.emplace(key, address_chain);
                                    }
                                    else if (it->second != address_chain)
                                    {
                                        // Extremely unlikely collision; probe to preserve correctness.
                                        while (it != key_to_stack.end() && it->second != address_chain)
                                        {
                                            key = splitmix64(key);
                                            it = key_to_stack.find(key);
                                        }
                                        if (it == key_to_stack.end())
                                            key_to_stack.emplace(key, address_chain);
                                    }

                                    counts[key] += 1;
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
        std::unordered_map<uint64_t, uint64_t> counts;
        std::unordered_map<uint64_t, std::string> key_to_stack;
        Symbolizer symbolizer;
        ThreadAttachManager thread_attach_manager;
    };
}