#pragma once

#include <cstdint>
#include <cstring>
#include <vector>
#include "utils/utils.hh"

namespace optkit
{
    /**
     * @brief Base profiler config.
     *
     */
    struct ProfilerConfig
    {
        ProfilerConfig(const char *block_name,
                       const char *measurement_type,
                       bool is_reset_after_read,
                       bool dump_results_to_file,
                       bool verbose) : block_name{block_name},
                                       measurement_type{measurement_type},
                                       is_reset_after_read{is_reset_after_read},
                                       dump_results_to_file{dump_results_to_file},
                                       verbose{verbose}
        {
        }
        virtual ~ProfilerConfig() {}

        const char *block_name;
        const char *measurement_type;
        bool is_reset_after_read;
        bool dump_results_to_file;
        bool verbose;
    };

    /**
     * @brief Base class for profiling various metrics.
     *
     * This class provides a framework for measuring and storing performance metrics
     * in a structured manner. It supports enabling/disabling profiling, reading values,
     * and aggregating results. The class is designed to be extended for specific profiling
     * implementations.
     *
     * @tparam readT return type of the read method, tipically it is vector<uint64_t> or uint64_t. or can be anything else.
     * @tparam readvalT Type of the single value of the element. for vector<uint64_t> it is uint64_t.
     *
     * @note readValT must be big enough to store the value of accumulation of read_buffer.
     * ie. your values are int32_t, but it is makesense to use int64_t as readvalT since acummulating many int32_t values can overflow int32_t.
     *
     * @note All PMU classes should use, and is using, uint64_t as readValT since it is the standard. https://www.man7.org/linux/man-pages/man2/perf_event_open.2.html#EXAMPLES
     */
    template <typename readT, typename readvalT>
    class BaseProfiler
    {
    public:
        BaseProfiler(const ProfilerConfig &config) : config{config}, total_duration_ms{0}, start{std::chrono::high_resolution_clock::now()}
        {
        }
        virtual ~BaseProfiler() {}

        virtual void disable() = 0;
        virtual void enable() = 0;
        virtual void reset() = 0;

        /**
         * @brief Reads the value and STORES it in a buffer for subsequent saving to a file.
         *        Read also store the duration time between start-end and re-set the start to end value afterwards
         * @return  std::pair<double, readT> where first is duration and second is the value.
         */
        virtual std::pair<double, readT> read_and_store() final
        {
            // stop timer
            auto end = std::chrono::high_resolution_clock::now();

            // read value
            const readT &val = read();

            // calculate duration in ms
            auto duration_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0f;

            // write to buffer
            read_buffer.push_back({duration_ms, val});

            // re-set start time to now
            this->start = end;

            return read_buffer.back();
        }

        /**
         * @brief read_val method that refrains from storing the value in a buffer for future writing.
         *        read_val should NOT! mess with time durations, only should read data and retrun it.
         * @return readT
         */
        virtual readT read() = 0;

        /**
         * @brief Convert buffer to json string as you wish.
         *
         * @return std::string
         */
        virtual std::string to_json() = 0;

        // virtual const std::vector<std::pair<double, readT>> &get_read_buffer() final
        // {
        //     return read_buffer;
        // }

        /**
         * @brief Aggregates all collected data from the read buffer.
         *
         * If multiple `read_and_save()` calls are made while monitoring events (e.g., x, y, z),
         * the `read_buffer` may contain several entries, each holding a duration and a list of event values.
         *
         * This method processes the entire buffer, summing up the total duration and combining
         * all recorded event-value pairs into a single list.
         *
         * You should call MetricBuffer.calculate(...) to see the results of your metrics.
         *
         * @return unique event-value map.
         */
        virtual std::unordered_map<std::string, readvalT> aggregate() = 0;

        /**
         * @brief Returns the historic data of power samples taken.
         *
         * @return std::vector<std::pair<double, readT>> where first is duration and second is the value.
         */
        std::vector<std::pair<double, readT>> get_read_buffer()
        {
            return this->read_buffer;
        }

    protected:
        /**
         * @brief Converts the buffer to JSON format and writes it to a file.
         *
         */
        virtual void save() final
        {
            std::string json_data = to_json();
            std::string block_name = this->config.block_name;
            std::replace(block_name.begin(), block_name.end(), ' ', '_');
            utils::write_file(utils::EXECUTION_FOLDER_NAME + "/" + block_name + "__" + this->config.measurement_type + ".json", json_data, this->config.verbose);
        }

    public:
        const ProfilerConfig config;

    protected:
        double total_duration_ms;
        std::chrono::high_resolution_clock::time_point start;

        /**
         * @brief Vector of timestamp–measurement pairs.
         *
         * Note: Timestamps may vary across different workloads or basically user takes samples at random places.
         * Each timestamp represents the elapsed time at which the corresponding measurement was taken.
         *
         */
        std::vector<std::pair<double, readT>> read_buffer;

        /**
         * @brief Aggregated view of the read_buffer, where each element
         *        represents an event name and its accumulated value. it is filled by aggregate() method.
         */
        std::vector<std::pair<std::string, readvalT>> event_results;
    };

} // namespace optkit
