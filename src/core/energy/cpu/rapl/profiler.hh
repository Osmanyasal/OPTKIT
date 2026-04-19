#pragma once

#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <memory>
#include <ostream>
#include <map>
#include <set>

#include "utils/utils.hh"
#include "utils/base_profiler.hh"
#include "core/query.hh"
#include "core/energy/cpu/rapl/query.hh"
#include "core/energy/cpu/rapl/utils.hh"
#include "utils/metric_builder.hh"

namespace optkit::energy::rapl
{
    /**
     * @brief Rapl Profiler (using perf_event_open)
     *
     * This class provides a profiler for reading energy consumption data from RAPL (Running Average Power Limit) domains.
     * It inherits from the BaseProfiler class and implements methods to enable, disable, read, and aggregate energy data.
     *
     * The profiler can be configured using the ProfilerConfig structure, which allows specifying the block name,
     * measurement type, and other options.
     *
     * The read method returns a map where the keys are socket IDs and the values are maps of RAPL domains to their corresponding energy readings in Joules.
     *
     * @see BaseProfiler
     * @see ProfilerConfig
     */
    class Profiler : public BaseProfiler<std::unordered_map<int32_t, std::unordered_map<RaplDomain, double>>, std::unordered_map<int32_t, std::unordered_map<RaplDomain, double>>>
    {
    public:
        Profiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<double> &mb);
        virtual ~Profiler();

        /**
         * @brief Disables this rapl profiler.
         *
         */
        virtual void disable() override;

        /**
         * @brief Enables this rapl profiler.
         *
         */
        virtual void enable() override;

        virtual void reset() override {}

        virtual std::string to_json() override;

        /**
         * @brief Reads the values of all raw_events.
         *
         * @return std::unordered_map<int32_t,std::unordered_map<RaplDomain, int32_t>> SocketID - RaplDomain - reading
         */
        virtual std::unordered_map<int32_t, std::unordered_map<RaplDomain, double>> read() override;

        virtual std::unordered_map<std::string, std::unordered_map<int32_t, std::unordered_map<RaplDomain, double>>> aggregate() override; // event_name - reading_val

    private:
        std::vector<std::vector<int32_t>> fd_package_domain; // file descriptors [package(socket)][domain]

        optkit::metrics::MetricBuilder<double> metric_builder;                                    // metric_data
        std::unordered_map<uint32_t, std::vector<std::pair<std::string, double>>> metric_results; // metric - value

        std::thread sampling_thread;
        std::atomic<bool> is_sampling{false};
    };

    // Overloading << for map with RaplDomain as keys
    std::string to_string(const std::unordered_map<optkit::energy::rapl::RaplDomain, double> &map);
    std::ostream &operator<<(std::ostream &os, const std::unordered_map<optkit::energy::rapl::RaplDomain, double> &map);
    std::ostream &operator<<(std::ostream &os, const std::unordered_map<int32_t, std::unordered_map<optkit::energy::rapl::RaplDomain, double>> &map);

} // namespace optkit::energy::rapl

using optkit::energy::rapl::operator<<; // make available to global namespace
