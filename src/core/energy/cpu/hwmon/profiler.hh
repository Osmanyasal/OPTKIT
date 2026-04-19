#pragma once

#include <memory>
#include <ostream>
#include <map>
#include <set>
#include <fstream>
#include <thread>
#include <atomic>

#include "utils/utils.hh"
#include "utils/base_profiler.hh"
#include "core/query.hh"
#include "core/energy/cpu/hwmon/query.hh"
#include "core/energy/cpu/hwmon/utils.hh"
#include "utils/metric_builder.hh"

namespace optkit::energy::hwmon
{
    /**
     * @brief HWMON Profiler for Grace and ARM systems
     *
     * This class provides a profiler for reading power consumption data from HWMON sensors.
     * It inherits from the BaseProfiler class and implements methods to enable, disable, read, 
     * and aggregate power data from /sys/class/hwmon interfaces.
     *
     * The profiler can be configured using the ProfilerConfig structure, which allows specifying 
     * the block name, measurement type, and other options.
     *
     * The read method returns a map where the keys are socket IDs and the values are maps of 
     * HWMON domains to their corresponding power readings in Watts.
     *
     * @see BaseProfiler
     * @see ProfilerConfig
     */
    class Profiler : public BaseProfiler<std::unordered_map<int32_t, std::unordered_map<HwmonDomain, double>>, 
                                         std::unordered_map<int32_t, std::unordered_map<HwmonDomain, double>>>
    {
    public:
        Profiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<double> &mb);
        virtual ~Profiler();

        /**
         * @brief Disables this hwmon profiler (not applicable for sysfs reads).
         */
        virtual void disable() override;

        /**
         * @brief Enables this hwmon profiler (not applicable for sysfs reads).
         */
        virtual void enable() override;

        virtual void reset() override {}

        virtual std::string to_json() override;

        /**
         * @brief Reads the values of all HWMON power sensors.
         *
         * @return std::unordered_map<int32_t, std::unordered_map<HwmonDomain, double>> 
         *         SocketID -> HwmonDomain -> power reading in Watts
         */
        virtual std::unordered_map<int32_t, std::unordered_map<HwmonDomain, double>> read() override;

        /**
         * @brief Aggregates all power readings over time.
         * 
         * @return Map of event names to socket->domain->energy readings (Joules)
         */
        virtual std::unordered_map<std::string, std::unordered_map<int32_t, std::unordered_map<HwmonDomain, double>>> aggregate() override;

    private:
        struct SensorInfo
        {
            std::string path;
            HwmonDomain domain;
            int socket_id;
            double scale;
            std::ifstream file_stream;
        };

        std::vector<SensorInfo> sensors; // All available sensors
        
        optkit::metrics::MetricBuilder<double> metric_builder;
        std::unordered_map<uint32_t, std::vector<std::pair<std::string, double>>> metric_results;

        std::thread sampling_thread;
        std::atomic<bool> is_sampling{false};

        // For energy calculation from power samples
        std::unordered_map<int32_t, std::unordered_map<HwmonDomain, double>> last_power_reading;
        std::chrono::high_resolution_clock::time_point last_read_time;
    };

    // Overloading << for map with HwmonDomain as keys
    std::string to_string(const std::unordered_map<optkit::energy::hwmon::HwmonDomain, double> &map);
    std::ostream &operator<<(std::ostream &os, const std::unordered_map<optkit::energy::hwmon::HwmonDomain, double> &map);
    std::ostream &operator<<(std::ostream &os, const std::unordered_map<int32_t, std::unordered_map<optkit::energy::hwmon::HwmonDomain, double>> &map);

} // namespace optkit::energy::hwmon

using optkit::energy::hwmon::operator<<; // make available to global namespace
