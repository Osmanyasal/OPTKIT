#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <cstdint>
#include <unordered_map>
#include <filesystem>
#include "utils/utils.hh"
#include "utils/base_profiler.hh"
#include "utils/metric_builder.hh"

namespace optkit::core::temperature
{
    /**
     * @brief CPU temperature profiler.
     * This profiler reads CPU temperature sensors from the hwmon interface.
     * It is designed to work with the hwmon interface, which is typically found in Linux
     * systems under `/sys/class/hwmon/`.
     *
     * int64_t is chosen as the read type since, as opposed to pmu and io, next reading can be smaller than the previous one,
     * e.g., when the CPU cools down. This is why we use int64_t instead of uint64_t.
     *
     * Discovery of sensors is done in the static `init()` method, which is called in OPTKIT ctor.
     * The sensors are stored in a static map `sensor_paths` which maps sensor names to their respective hwmon paths to obtain the temperature readings.
     *
     *
     */
    class CPUTemperatureProfiler : public BaseProfiler<std::vector<int64_t>, int64_t>
    {
    public:
        static void init(); // Initialize static sensor paths

    private:
        static std::unordered_map<std::string, std::string> discover_hwmon_sensors();
        static std::string build_sensor_name(const std::string &hwmon_name, const std::string &temp_num_str);
        static std::unordered_map<std::string, std::string> sensor_paths; // sensor_name -> hwmon_path

    public:
        CPUTemperatureProfiler(const char *block_name, const core::metrics::MetricBuilder<int64_t> &mb, bool verbose = !Query::create_folder);
        virtual ~CPUTemperatureProfiler();

        virtual void enable() override {}  // Already handled by constructor
        virtual void disable() override {} // No-op
        virtual void reset() override {}   // No-op

        virtual std::vector<int64_t> read() override;
        virtual std::string to_json() override;
        virtual std::unordered_map<std::string, int64_t> aggregate() override;

    private:
        /**
         * Reads selected temperature sensors from hwmon and GPU APIs.
         * @param sensor_names vector of sensor names to read
         * @return unordered_map from sensor_name -> temperature (millidegrees C)
         */
        std::unordered_map<std::string, int64_t> read_selected_temperature_sensors(const std::vector<std::string> &sensor_names);

        /**
         * Read temperature from hwmon path
         */
        int64_t read_hwmon_temperature(const std::string &hwmon_path);

    private:
        core::metrics::MetricBuilder<int64_t> metric_builder;
        std::unordered_map<std::string, int64_t> last_snapshot;
        std::vector<std::pair<std::string, double>> metric_results;
    };
}