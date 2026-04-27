#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include "utils/utils.hh"
#include "utils/base_profiler.hh"
#include "utils/metric_builder.hh"

namespace optkit::temperature::hwmon
{
    /**
     * @brief HWMON temperature profiler.
     * This profiler reads temperature sensors from the hwmon interface.
     * It is designed to work with the hwmon interface, which is typically found in Linux
     * systems under `/sys/class/hwmon/`.
     *
     * double is chosen as the read type since, as opposed to pmu and io, next reading can be smaller than the previous one,
     * e.g., when the related device cools down. This is why we use double instead of uint64_t.
     *
     * Discovery of sensors is done in the static `init()` method, which is called in OPTKIT ctor.
     * The sensors are stored in a static map `sensor_paths` which maps sensor names to their respective hwmon paths to obtain the temperature readings.
     *
     * @note: grouping feature is not fully implemented. meaning, what ever exists in hwmon, is reported as is.
     *
     */
    class Profiler : public BaseProfiler<std::vector<double>, double>
    {
    public:
        static void init(); // Initialize static sensor paths

    private:
        static std::unordered_map<std::string, std::string> discover_hwmon_sensors();
        static std::string build_sensor_name(const std::string &hwmon_name, const std::string &temp_num_str);
        static std::unordered_map<std::string, std::string> sensor_paths; // sensor_name -> hwmon_path

    public:
        Profiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<double> &mb = {});
        virtual ~Profiler();

        virtual void enable() override { this->is_enabled = true; }
        virtual void disable() override { this->is_enabled = false; }
        virtual void reset() override {}

        // This read saves deltas, meaning, the change from the previous read. (current_val - prev_val)
        // if you need current values, use the read_temperature_sensors() method directly.
        virtual std::vector<double> read() override;
        virtual std::string to_json() override;
        virtual std::unordered_map<std::string, double> aggregate() override;

        /**
         * Reads selected temperature sensors from hwmon and GPU APIs.
         * @param sensor_names vector of sensor names to read
         * @return unordered_map from sensor_name -> temperature (millidegrees C)
         */
        std::unordered_map<std::string, double> read_temperature_sensors();

    private:
        virtual void on_sample_stored(const std::pair<double, std::vector<double>> &sample) override;
        void flush_compacted_samples();

        /**
         * Read temperature from hwmon path
         */
        double read_hwmon_temperature(const std::string &hwmon_path);

    private:
        optkit::metrics::MetricBuilder<double> metric_builder;
        std::unordered_map<std::string, double> last_snapshot;
        std::vector<std::pair<std::string, double>> metric_results;
        std::unordered_map<std::string, double> compacted_event_counts;
        double compacted_duration_ms{0.0};
        double buffered_duration_ms{0.0};
    };
} // namespace optkit::temperature::hwmon