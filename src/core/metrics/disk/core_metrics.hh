#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <cmath>
#include "utils/metric_builder.hh"
#include "core/metrics/disk/core_events.hh"

namespace optkit::core::metrics::disk
{
    /**
     * @brief Disk-level performance metrics derived from /proc/self/io.
     *
     * Focus on actionable, interpretable metrics that provide insights into:
     * - I/O efficiency and patterns
     * - Cache behavior
     * - Syscall characteristics
     * - Disk vs memory operations
     */
    template <typename T>
    class CoreMetrics
    {
    public:
        /**
         * @brief Average bytes per read syscall.
         *
         * Formula: rchar / syscw
         *
         * Indicates read efficiency - larger values suggest better batching.
         * Values < 4KB may indicate inefficient small reads.
         */
        static optkit::core::metrics::MetricBuilder ReadBatchSize()
        {
            std::string rchar = to_string(CoreEvents::RCHAR);
            std::string syscr = to_string(CoreEvents::SYSCR);

            return optkit::core::metrics::MetricBuilder{}
                .add(rchar, {0x0})
                .add(syscr, {0x0})
                .build("read_batch_size",
                       [rchar, syscr](const std::unordered_map<std::string, uint64_t> &m)
                       {
                           uint64_t val_rchar = m.at(rchar);
                           uint64_t val_syscr = m.at(syscr);

                           if (val_syscr == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(val_rchar) / val_syscr;
                       });
        }

        /**
         * @brief Average bytes per write syscall.
         *
         * Formula: wchar / syscw
         *
         * Indicates write efficiency - larger values suggest better batching.
         * Values < 4KB may indicate inefficient small writes.
         */
        static optkit::core::metrics::MetricBuilder WriteBatchSize()
        {
            std::string wchar = to_string(CoreEvents::WCHAR);
            std::string syscw = to_string(CoreEvents::SYSCW);

            return optkit::core::metrics::MetricBuilder{}
                .add(wchar, {0x0})
                .add(syscw, {0x0})
                .build("write_batch_size",
                       [wchar, syscw](const std::unordered_map<std::string, uint64_t> &m)
                       {
                           uint64_t val_wchar = m.at(wchar);
                           uint64_t val_syscw = m.at(syscw);

                           if (val_syscw == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(val_wchar) / val_syscw;
                       });
        }

        /**
         * @brief Percentage of read operations served from cache.
         *
         * Formula: 100 * (1 - read_bytes/rchar)
         *
         * 100% = all reads from cache, 0% = all reads from disk
         * Higher values indicate better cache utilization.
         */
        static optkit::core::metrics::MetricBuilder ReadCacheHitRate()
        {
            std::string rchar = to_string(CoreEvents::RCHAR);
            std::string read_bytes = to_string(CoreEvents::READ_BYTES);

            return optkit::core::metrics::MetricBuilder{}
                .add(rchar, {0x0})
                .add(read_bytes, {0x0})
                .build("read_cache_hit_rate_%",
                       [rchar, read_bytes](const std::unordered_map<std::string, uint64_t> &m)
                       {
                           uint64_t val_rchar = m.at(rchar);
                           uint64_t val_read_bytes = m.at(read_bytes);

                           if (val_rchar == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100 * (1.0 - static_cast<double>(val_read_bytes) / val_rchar);
                       });
        }

        /**
         * @brief Percentage of write operations that bypassed cache.
         *
         * Formula: 100 * (write_bytes/wchar)
         *
         * High values indicate sync writes or cache bypassing.
         * Low values indicate good write caching/buffering.
         */
        static optkit::core::metrics::MetricBuilder WriteCacheBypassRate()
        {
            std::string wchar = to_string(CoreEvents::WCHAR);
            std::string write_bytes = to_string(CoreEvents::WRITE_BYTES);

            return optkit::core::metrics::MetricBuilder{}
                .add(wchar, {0x0})
                .add(write_bytes, {0x0})
                .build("write_cache_bypass_rate_%",
                       [wchar, write_bytes](const std::unordered_map<std::string, uint64_t> &m)
                       {
                           uint64_t val_wchar = m.at(wchar);
                           uint64_t val_write_bytes = m.at(write_bytes);

                           if (val_wchar == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100.0 * (static_cast<double>(val_write_bytes) / val_wchar);
                       });
        }

        /**
         * @brief Disk I/O amplification factor.
         *
         * Formula: (read_bytes + write_bytes) / (rchar + wchar)
         *
         * Values > 1.0 indicate I/O amplification (e.g., from compression, encryption)
         * Values < 1.0 indicate good caching (common case)
         * Values near 0 indicate mostly cached operations
         */
        static optkit::core::metrics::MetricBuilder IOAmplificationFactor()
        {
            std::string rchar = to_string(CoreEvents::RCHAR);
            std::string wchar = to_string(CoreEvents::WCHAR);
            std::string read_bytes = to_string(CoreEvents::READ_BYTES);
            std::string write_bytes = to_string(CoreEvents::WRITE_BYTES);

            return optkit::core::metrics::MetricBuilder{}
                .add(rchar, {0x0})
                .add(wchar, {0x0})
                .add(read_bytes, {0x0})
                .add(write_bytes, {0x0})
                .build("io_amplification_factor",
                       [rchar, wchar, read_bytes, write_bytes](const std::unordered_map<std::string, uint64_t> &m)
                       {
                           uint64_t val_rchar = m.at(rchar);
                           uint64_t val_wchar = m.at(wchar);
                           uint64_t val_read_bytes = m.at(read_bytes);
                           uint64_t val_write_bytes = m.at(write_bytes);

                           uint64_t total_logical = val_rchar + val_wchar;
                           uint64_t total_physical = val_read_bytes + val_write_bytes;

                           if (total_logical == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(total_physical) / total_logical;
                       });
        }

        /**
         * @brief Read/Write operation ratio.
         *
         * Formula: rchar / wchar
         *
         * > 1.0 = Read-heavy workload
         * < 1.0 = Write-heavy workload
         * Helps characterize workload patterns
         */
        static optkit::core::metrics::MetricBuilder ReadWriteRatio()
        {
            std::string rchar = to_string(CoreEvents::RCHAR);
            std::string wchar = to_string(CoreEvents::WCHAR);

            return optkit::core::metrics::MetricBuilder{}
                .add(rchar, {0x0})
                .add(wchar, {0x0})
                .build("read_write_ratio",
                       [rchar, wchar](const std::unordered_map<std::string, uint64_t> &m)
                       {
                           uint64_t val_rchar = m.at(rchar);
                           uint64_t val_wchar = m.at(wchar);

                           if (val_wchar == 0)
                           {
                               return val_rchar > 0 ? std::numeric_limits<double>::infinity()
                                                    : std::numeric_limits<double>::quiet_NaN();
                           }
                           return static_cast<double>(val_rchar) / val_wchar;
                       });
        }

        /**
         * @brief Syscall efficiency indicator.
         *
         * Formula: (rchar + wchar) / (syscr + syscw)
         *
         * Higher values indicate fewer syscalls for the same I/O volume.
         * Values < 1KB suggest very inefficient syscall patterns.
         * Values > 64KB suggest good I/O batching.
         */
        static optkit::core::metrics::MetricBuilder SyscallEfficiency()
        {
            std::string rchar = to_string(CoreEvents::RCHAR);
            std::string wchar = to_string(CoreEvents::WCHAR);
            std::string syscr = to_string(CoreEvents::SYSCR);
            std::string syscw = to_string(CoreEvents::SYSCW);

            return optkit::core::metrics::MetricBuilder{}
                .add(rchar, {0x0})
                .add(wchar, {0x0})
                .add(syscr, {0x0})
                .add(syscw, {0x0})
                .build("syscall_efficiency",
                       [rchar, wchar, syscr, syscw](const std::unordered_map<std::string, uint64_t> &m)
                       {
                           uint64_t val_rchar = m.at(rchar);
                           uint64_t val_wchar = m.at(wchar);
                           uint64_t val_syscr = m.at(syscr);
                           uint64_t val_syscw = m.at(syscw);

                           uint64_t total_bytes = val_rchar + val_wchar;
                           uint64_t total_syscalls = val_syscr + val_syscw;

                           if (total_syscalls == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(total_bytes) / total_syscalls;
                       });
        }

        /**
         * @brief Disk utilization ratio.
         *
         * Formula: 100 * (read_bytes + write_bytes) / (rchar + wchar)
         *
         * Percentage of logical I/O that actually hit the disk.
         * Lower values indicate better caching.
         * Values near 100% suggest poor cache utilization or sync I/O.
         */
        static optkit::core::metrics::MetricBuilder DiskUtilizationRate()
        {
            std::string rchar = to_string(CoreEvents::RCHAR);
            std::string wchar = to_string(CoreEvents::WCHAR);
            std::string read_bytes = to_string(CoreEvents::READ_BYTES);
            std::string write_bytes = to_string(CoreEvents::WRITE_BYTES);

            return optkit::core::metrics::MetricBuilder{}
                .add(rchar, {0x0})
                .add(wchar, {0x0})
                .add(read_bytes, {0x0})
                .add(write_bytes, {0x0})
                .build("disk_utilization_rate_%",
                       [rchar, wchar, read_bytes, write_bytes](const std::unordered_map<std::string, uint64_t> &m)
                       {
                           uint64_t val_rchar = m.at(rchar);
                           uint64_t val_wchar = m.at(wchar);
                           uint64_t val_read_bytes = m.at(read_bytes);
                           uint64_t val_write_bytes = m.at(write_bytes);

                           uint64_t total_logical = val_rchar + val_wchar;
                           uint64_t total_physical = val_read_bytes + val_write_bytes;

                           if (total_logical == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100.0 * (static_cast<double>(total_physical) / total_logical);
                       });
        }

        static optkit::core::metrics::MetricBuilder AllMetrics()
        {
            return optkit::core::metrics::MetricBuilder{}
                .add(ReadBatchSize())
                .add(WriteBatchSize())
                .add(ReadCacheHitRate())
                .add(WriteCacheBypassRate())
                .add(IOAmplificationFactor())
                .add(ReadWriteRatio())
                .add(SyscallEfficiency())
                .add(DiskUtilizationRate());
        }

    private:
        CoreMetrics() {}
        ~CoreMetrics() {}
    };
} // namespace optkit::core::metrics::disk