#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <cstdint>
#include <unordered_map>
#include <thread>
#include <atomic>
#include "utils/utils.hh"
#include "utils/base_profiler.hh"
#include "utils/metric_builder.hh"

namespace optkit::disk
{
    class IoDiskProfiler : public BaseProfiler<std::vector<uint64_t>, uint64_t>
    {
    public:
        IoDiskProfiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<uint64_t> &mb);
        virtual ~IoDiskProfiler();

        virtual void enable() override { this->is_enabled = true; }
        virtual void disable() override { this->is_enabled = false; }
        virtual void reset() override {}

        virtual std::vector<uint64_t> read() override;
        virtual std::string to_json() override;

        virtual std::unordered_map<std::string, uint64_t> aggregate() override;

        /**
         * @brief Reads selected IO counters from /proc/self/io.
         *
         * @return std::unordered_map<std::string, uint64_t>
         */
        virtual std::unordered_map<std::string, uint64_t> read_selected_io_counters();

    private:
        uint64_t last_read = 0;
        uint64_t last_write = 0;

        std::unordered_map<std::string, uint64_t> last_snapshot;
        std::vector<std::pair<std::string, double>> metric_results;
        optkit::metrics::MetricBuilder<uint64_t> metric_builder;

        std::thread sampling_thread;
        std::atomic<bool> is_sampling;
    };
}
