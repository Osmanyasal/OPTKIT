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
        static optkit::core::metrics::MetricBuilder<uint64_t> LogicalReadPerSyscall()
        {
            std::string rchar = to_string(CoreEvents::RCHAR);
            std::string syscr = to_string(CoreEvents::SYSCR);

            return optkit::core::metrics::MetricBuilder<uint64_t>{}
                .add(rchar, {0x0})
                .add(syscr, {0x0})
                .build("LogicalReadPerSyscall",
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
        static optkit::core::metrics::MetricBuilder<uint64_t> LogicalWritePerSyscall()
        {
            std::string wchar = to_string(CoreEvents::WCHAR);
            std::string syscw = to_string(CoreEvents::SYSCW);

            return optkit::core::metrics::MetricBuilder<uint64_t>{}
                .add(wchar, {0x0})
                .add(syscw, {0x0})
                .build("LogicalWritePerSyscall",
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
        static optkit::core::metrics::MetricBuilder<uint64_t> PhysicalReadCacheHitRate()
        {
            std::string rchar = to_string(CoreEvents::RCHAR);
            std::string read_bytes = to_string(CoreEvents::READ_BYTES);

            return optkit::core::metrics::MetricBuilder<uint64_t>{}
                .add(rchar, {0x0})
                .add(read_bytes, {0x0})
                .build("PhysicalReadCacheHitRate__%",
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
         * @brief Percentage of write operations served by cache/buffering.
         *
         * Formula: 100 * (1 - write_bytes/wchar)
         *
         * 100% = all writes cached/buffered, 0% = all writes go directly to disk
         * Higher values indicate better write caching/buffering.
         */
        static optkit::core::metrics::MetricBuilder<uint64_t> PhysicalWriteCacheHitRate()
        {
            std::string wchar = to_string(CoreEvents::WCHAR);
            std::string write_bytes = to_string(CoreEvents::WRITE_BYTES);
            return optkit::core::metrics::MetricBuilder<uint64_t>{}
                .add(wchar, {0x0})
                .add(write_bytes, {0x0})
                .build("PhysicalWriteCacheHitRate__%",
                       [wchar, write_bytes](const std::unordered_map<std::string, uint64_t> &m)
                       {
                           uint64_t val_wchar = m.at(wchar);
                           uint64_t val_write_bytes = m.at(write_bytes);
                           if (val_wchar == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return 100.0 * (1.0 - static_cast<double>(val_write_bytes) / val_wchar);
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
        static optkit::core::metrics::MetricBuilder<uint64_t> IOAmplificationFactor()
        {
            std::string rchar = to_string(CoreEvents::RCHAR);
            std::string wchar = to_string(CoreEvents::WCHAR);
            std::string read_bytes = to_string(CoreEvents::READ_BYTES);
            std::string write_bytes = to_string(CoreEvents::WRITE_BYTES);

            return optkit::core::metrics::MetricBuilder<uint64_t>{}
                .add(rchar, {0x0})
                .add(wchar, {0x0})
                .add(read_bytes, {0x0})
                .add(write_bytes, {0x0})
                .build("IOAmplificationFactor",
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
         * @brief Read amplification factor.
         *
         * Formula: read_bytes / rchar
         *
         * Values > 1.0 are unusual and may indicate measurement inconsistencies.
         * Values < 1.0 indicate caching effectiveness (most reads served from cache).
         * Values near 0 mean almost all reads are cached.
         */
        static optkit::core::metrics::MetricBuilder<uint64_t> ReadAmplificationFactor()
        {
            std::string rchar = to_string(CoreEvents::RCHAR);
            std::string read_bytes = to_string(CoreEvents::READ_BYTES);

            return optkit::core::metrics::MetricBuilder<uint64_t>{}
                .add(rchar, {0x0})
                .add(read_bytes, {0x0})
                .build("ReadAmplificationFactor",
                       [rchar, read_bytes](const std::unordered_map<std::string, uint64_t> &m)
                       {
                           uint64_t val_rchar = m.at(rchar);
                           uint64_t val_read_bytes = m.at(read_bytes);

                           if (val_rchar == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(val_read_bytes) / val_rchar;
                       });
        }

        /**
         * @brief Write amplification factor.
         *
         * Formula: write_bytes / wchar
         *
         * Values >= 1.0 may indicate extra internal writes (e.g., journaling, SSD overhead).
         * Values close to 1.0 indicate little amplification.
         * Values < 1.0 suggest write buffering or caching.
         */
        static optkit::core::metrics::MetricBuilder<uint64_t> WriteAmplificationFactor()
        {
            std::string wchar = to_string(CoreEvents::WCHAR);
            std::string write_bytes = to_string(CoreEvents::WRITE_BYTES);

            return optkit::core::metrics::MetricBuilder<uint64_t>{}
                .add(wchar, {0x0})
                .add(write_bytes, {0x0})
                .build("WriteAmplificationFactor",
                       [wchar, write_bytes](const std::unordered_map<std::string, uint64_t> &m)
                       {
                           uint64_t val_wchar = m.at(wchar);
                           uint64_t val_write_bytes = m.at(write_bytes);

                           if (val_wchar == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(val_write_bytes) / val_wchar;
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
        static optkit::core::metrics::MetricBuilder<uint64_t> LogicalReadPerWrite()
        {
            std::string rchar = to_string(CoreEvents::RCHAR);
            std::string wchar = to_string(CoreEvents::WCHAR);

            return optkit::core::metrics::MetricBuilder<uint64_t>{}
                .add(rchar, {0x0})
                .add(wchar, {0x0})
                .build("LogicalReadPerWrite",
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
         * @brief Logical io per syscall indicator.
         *
         * Formula: (rchar + wchar) / (syscr + syscw)
         *
         * Higher values indicate fewer syscalls for the same I/O volume.
         * Values < 1KB suggest very inefficient syscall patterns.
         * Values > 64KB suggest good I/O batching.
         */
        static optkit::core::metrics::MetricBuilder<uint64_t> LogicalIOPerSyscall()
        {
            std::string rchar = to_string(CoreEvents::RCHAR);
            std::string wchar = to_string(CoreEvents::WCHAR);
            std::string syscr = to_string(CoreEvents::SYSCR);
            std::string syscw = to_string(CoreEvents::SYSCW);

            return optkit::core::metrics::MetricBuilder<uint64_t>{}
                .add(rchar, {0x0})
                .add(wchar, {0x0})
                .add(syscr, {0x0})
                .add(syscw, {0x0})
                .build("LogicalIOPerSyscall",
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
        static optkit::core::metrics::MetricBuilder<uint64_t> DiskUtilizationRate()
        {
            std::string rchar = to_string(CoreEvents::RCHAR);
            std::string wchar = to_string(CoreEvents::WCHAR);
            std::string read_bytes = to_string(CoreEvents::READ_BYTES);
            std::string write_bytes = to_string(CoreEvents::WRITE_BYTES);

            return optkit::core::metrics::MetricBuilder<uint64_t>{}
                .add(rchar, {0x0})
                .add(wchar, {0x0})
                .add(read_bytes, {0x0})
                .add(write_bytes, {0x0})
                .build("DiskUtilizationRate__%",
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

        /**
         * @brief I/O Operations Per Second (IOPS)
         *
         * @return optkit::core::metrics::MetricBuilder<uint64_t>
         */
        static optkit::core::metrics::MetricBuilder<uint64_t> IOPS()
        {
            std::string syscr = to_string(CoreEvents::SYSCR);
            std::string syscw = to_string(CoreEvents::SYSCW);
            return optkit::core::metrics::MetricBuilder<uint64_t>{}
                .add(syscr, {0x0})
                .add(syscw, {0x0})
                .build("IOPS",
                       [syscr, syscw](const std::unordered_map<std::string, uint64_t> &m)
                       {
                           uint64_t val_syscr = m.at(syscr);
                           uint64_t val_syscw = m.at(syscw);

                           uint64_t total_logical = val_syscr + val_syscw;
                           double duration_sec = get_event_count(m, "duration_microsec") / 1.0e6;

                           if (duration_sec == 0)
                               return std::numeric_limits<double>::quiet_NaN();
                           return static_cast<double>(total_logical) / duration_sec;
                       });
        }

        static optkit::core::metrics::MetricBuilder<uint64_t> AllMetrics()
        {
            return optkit::core::metrics::MetricBuilder<uint64_t>{}
                .add(IOPS())
                .add(LogicalReadPerSyscall())
                .add(LogicalWritePerSyscall())
                .add(PhysicalReadCacheHitRate())
                .add(PhysicalWriteCacheHitRate())
                .add(IOAmplificationFactor())
                .add(ReadAmplificationFactor())
                .add(WriteAmplificationFactor())
                .add(LogicalReadPerWrite())
                .add(LogicalIOPerSyscall())
                .add(DiskUtilizationRate());
        }

    private:
        CoreMetrics() {}
        ~CoreMetrics() {}
    };
} // namespace optkit::core::metrics::disk