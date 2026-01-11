#pragma once

#include <iostream>
#include <vector>
#include <chrono>
#include "utils/base_profiler.hh"
#include "utils/json.hh"
#include "utils/environment_config.hh"
#include "core/pmu/cpu/perf/profiler_config.hh"
#include "core/callstack/postman.hh"

namespace optkit::callstack
{

    /**
     * @brief perf_event_open based Profiler for callstack profiling
     *
     * This code implements a Distributed, Lock-Free Sampling Profiler. It is designed to scale to hundreds of threads without slowing down your application.
     *
     *  Here is the breakdown of how it works, using the "Mailbox and Postman" analogy.
     *  1. The Architecture: "Private Mailboxes"
     *
     * Instead of forcing all threads to fight over a single output buffer (which causes locking and slowness), this code gives every single thread its own private Ring Buffer (Mailbox).
     *
     *    Worker Threads: They just write to their own local buffer. They never wait for anyone.
     *
     *    Sweeper Thread: This is the "Postman." It walks around, checks everyone's mailbox, and prints the letters.
     *
     * @note User must use "-g -rdynamic -fno-omit-frame-pointer" compiler flags to enable callstack unwinding with frame pointers.
     *
     */
    class Profiler : public BaseProfiler<std::unordered_map<std::string, uint64_t>, uint64_t>
    {
    public:
        Profiler(const optkit::pmu::cpu::perf::PerfProfilerConfig &profiler_config);
        virtual ~Profiler();
        /**
         * @brief Disables this block profiler
         *
         */
        virtual void disable() override;

        /**
         * @brief Enables this block profiler
         *
         */
        virtual void enable() override;

        /**
         * @brief Reset this block profiler
         *
         */
        virtual void reset() override;

        /**
         * @brief converts buffer to json
         *
         */
        virtual std::string to_json() override;

        /**
         * @brief Reads the values of all raw_events.
         *
         * @return std::vector<uint64_t> contains each raw_events' pmu data.
         */
        virtual std::unordered_map<std::string, uint64_t> read() override;

        virtual std::unordered_map<std::string, uint64_t> aggregate() override;

#if !OPTKIT_TESTING // if not testing (in prod) then make those private, in testin make those public
    private:
#endif
        /**
         * @brief fd_list holds pmu events being monitor by this Profiler Object.
         * when created the same file description must be registered global fd_stack
         */
        const optkit::pmu::cpu::perf::PerfProfilerConfig profiler_config;
        uint32_t sample_freq_hz;
    };

} // namespace optkit::callstack
