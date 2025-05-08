#pragma once

#include <memory>
#include <ostream>
#include <map>

#include "utils/utils.hh"
#include "core/query.hh"
#include "core/profiling/rapl/query_rapl.hh"
#include "core/profiling/base_profiler.hh"
#include "core/profiling/profiler_config.hh"
#include "core/profiling/rapl/rapl_perf_reader.hh"
#include "core/profiling/rapl/rapl_utils.hh"
namespace optkit::core::rapl
{ 

    class RaplProfiler : public BaseProfiler<std::map<int32_t, std::map<RaplDomain, double>>>
    {
    public:
        RaplProfiler(const char *block_name, const char *event_name = "rapl", const RaplConfig &config = RaplConfig{});
        virtual ~RaplProfiler();

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

        virtual std::string convert_buffer_to_json() override;

        /**
         * @brief Reads the values of all raw_events.
         *
         * @return std::map<int32_t,std::map<RaplDomain, int32_t>> SocketID - RaplDomain - reading
         */
        virtual std::map<int32_t, std::map<RaplDomain, double>> read_val() override;

    private:
        std::unique_ptr<optkit::core::rapl::RaplPerfReader> rapl_reader;
        RaplConfig rapl_config;
    };
 
    // Overloading << for map with RaplDomain as keys
    std::ostream &operator<<(std::ostream &os, const std::map<optkit::core::rapl::RaplDomain, double> &map);

} // namespace optkit::core::rapl

using optkit::core::rapl::operator<<; // make available to global namespace