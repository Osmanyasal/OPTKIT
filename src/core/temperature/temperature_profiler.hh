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
    class TemperatureProfiler : public BaseProfiler<std::vector<uint64_t>>
    {
    public:
        static void init(); // Initialize static sensor paths

    private:
        static std::unordered_map<std::string, std::string> discover_hwmon_sensors();
        static std::string build_sensor_name(const std::string &hwmon_name, const std::string &temp_num_str);
        static std::unordered_map<std::string, std::string> sensor_paths; // sensor_name -> hwmon_path

    public:
        TemperatureProfiler(const char *block_name, const core::metrics::MetricBuilder &mb, bool verbose = !Query::create_folder);
        virtual ~TemperatureProfiler();

        virtual void enable() override {}  // Already handled by constructor
        virtual void disable() override {} // No-op
        virtual void reset() override {}   // No-op

        virtual std::vector<uint64_t> read() override;
        virtual std::string to_json() override;
        virtual std::unordered_map<std::string, uint64_t> aggregate() override;

    private:
        /**
         * Reads selected temperature sensors from hwmon and GPU APIs.
         * @param sensor_names vector of sensor names to read
         * @return unordered_map from sensor_name -> temperature (millidegrees C)
         */
        std::unordered_map<std::string, uint64_t> read_selected_temperature_sensors(const std::vector<std::string> &sensor_names);

        /**
         * Read temperature from hwmon path
         */
        uint64_t read_hwmon_temperature(const std::string &hwmon_path);

        /**
         * Read GPU temperature using vendor APIs
         */
        uint64_t read_gpu_temperature();

    private:
        core::metrics::MetricBuilder metric_builder;
        std::unordered_map<std::string, uint64_t> last_snapshot;
        std::vector<std::pair<std::string, double>> metric_results;
    };
}