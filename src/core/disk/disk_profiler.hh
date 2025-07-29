#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <cstdint>
#include <unordered_map>
#include "utils/utils.hh"
#include "utils/base_profiler.hh"
#include "utils/metric_builder.hh"

namespace optkit::core::disk
{
    class IoDiskProfiler : public BaseProfiler<std::vector<uint64_t>>
    {
    public:
        IoDiskProfiler(const char *block_name, const core::metrics::MetricBuilder &mb, bool verbose = !Query::create_folder);
        virtual ~IoDiskProfiler();

        virtual void enable() override {}  // Already handled by constructor
        virtual void disable() override {} // No-op
        virtual void reset() override {}   // No-op

        virtual std::vector<uint64_t> read() override;
        virtual std::string to_json() override;

        virtual std::unordered_map<std::string, uint64_t> aggregate() override;

    private:
        /**
         * Reads selected io counters from /proc/self/io.
         * @param keys vector of counter names (e.g. "rchar", "wchar", "syscr", ...)
         * @return unordered_map from key -> value (uint64_t)
         * Throws on file open error.
         */
        virtual std::unordered_map<std::string, uint64_t> read_selected_io_counters(const std::vector<std::string> &keys);

    private:
        uint64_t last_read = 0;
        uint64_t last_write = 0;

        std::unordered_map<std::string, uint64_t> last_snapshot;

        std::vector<std::pair<std::string, double>> metric_results;
        core::metrics::MetricBuilder metric_builder;
    };
}
