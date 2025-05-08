#include "core/profiling/rapl/rapl_profiler.hh"

namespace optkit::core::rapl
{
    RaplProfiler::RaplProfiler(const char *block_name, const char *event_name, const RaplConfig &config) : BaseProfiler{block_name, event_name, true}, rapl_config{config}
    {
        const std::map<int32_t, std::vector<int32_t>> &packages = Query::detect_packages();
        const std::vector<RaplDomainInfo> &avail_domains = QueryRapl::rapl_domain_info(); // Monitor for all available domains

        // std::cout << rapl_config << std::endl;
        switch (rapl_config.read_method)
        {
        case RaplReadMethods::PERF:
            rapl_reader.reset(new RaplPerfReader{{packages, avail_domains, rapl_config}});
            break;

        case RaplReadMethods::MSR:
            // TODO:[FUTURE_WORK] Implement MSR reader
            OPTKIT_CORE_WARN("MSR not implemented in this version! Switching to PERF.");
            rapl_reader.reset(new RaplPerfReader{{packages, avail_domains, rapl_config}});

            break;

        case RaplReadMethods::POWERCAP:
            // TODO:[FUTURE_WORK] Implement POWERCAP reader
            OPTKIT_CORE_WARN("POWERCAP not implemented in this version! Switching to PERF.");
            rapl_reader.reset(new RaplPerfReader{{packages, avail_domains, rapl_config}});
            break;
        default:
            OPTKIT_CORE_WARN("unknown read method!");
            break;
        }
    }

    RaplProfiler::~RaplProfiler()
    {
        if (OPT_LIKELY(this->rapl_config.dump_results_to_file))
        {
            read();
            this->save();
        }
        else if(OPT_LIKELY(this->verbose))
        {
            // Disable the clock.
            auto end = std::chrono::high_resolution_clock::now();

            std::cout << read_val();
            auto duration_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0f;
            OPTKIT_CORE_INFO("Duration: {}", duration_ms);
        }

        delete rapl_reader.release();
    }

    void RaplProfiler::disable()
    {
        OPTKIT_CORE_WARN("Rapl cannot be disabled");
    }
    void RaplProfiler::enable()
    {
        OPTKIT_CORE_WARN("Rapl is always enabled");
    }

    std::map<int32_t, std::map<RaplDomain, double>> RaplProfiler::read_val()
    {
        return rapl_reader->read_val();
    }

    std::string RaplProfiler::convert_buffer_to_json()
    {
        std::stringstream ss;
        ss << "[\n";
        // based on the insertion order.
        ss << core::rapl::to_json(event_name, this->read_buffer);
        ss << "]\n";
        return ss.str();
    }
 

// Overloading << for map with RaplDomain as keys
std::ostream &operator<<(std::ostream &os, const std::map<optkit::core::rapl::RaplDomain, double> &map)
{

    for (const auto &item : map)
    {
        os << item.first << ": " << item.second << "\n";
    }

    return os;
}

} // namespace optkit::core