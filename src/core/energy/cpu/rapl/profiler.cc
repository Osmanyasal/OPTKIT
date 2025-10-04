#include "core/energy/cpu/rapl/profiler.hh"

namespace optkit::energy::rapl
{
    Profiler::Profiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<std::map<int32_t, std::map<RaplDomain, double>>> &mb) : BaseProfiler{profiler_config}, metric_builder{mb}
    {
        auto &packages = optkit::Query::detect_cpu_packages();
        auto &avail_domains = Query::rapl_domain_info(); // Monitor for all available domains
        auto s_type = optkit::utils::read_file("/sys/bus/event_source/devices/power/type");
        auto type = std::atoi(s_type.c_str());

        struct perf_event_attr attr;

        fd_package_domain.resize(packages.size());

        for (size_t package = 0; package < packages.size(); package++)
        {
            fd_package_domain[package].resize(avail_domains.size());

            for (auto domain = 0u; domain < avail_domains.size(); domain++)
            {
                auto selected_domain = avail_domains[domain];
                fd_package_domain[package][domain] = -1;

                ::memset(&attr, 0x0, sizeof(attr));
                attr.type = type;
                attr.config = selected_domain.config;

                if (attr.config == 0)
                    continue;

                fd_package_domain[package][domain] = ::syscall(__NR_perf_event_open, &attr, -1, packages.at(package).at(0), -1, 0);

                if (fd_package_domain[package][domain] < 0)
                {
                    OPTKIT_CORE_ERROR("RAPL PERF: SOMETHING WENT WRONG!! {}", fd_package_domain[package][domain]);
                }
            }
        }
    }

    Profiler::~Profiler()
    {
        read_and_store();
        this->metric_results = this->metric_builder.calculate(aggregate());

        if (OPT_LIKELY(this->config.dump_results_to_file))
            this->save();

        if (OPT_LIKELY(this->config.verbose))
        {
            // Disable the clock.
            auto end = std::chrono::high_resolution_clock::now();

            std::cout << read();
            auto duration_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0f;
            OPTKIT_CORE_INFO("Duration: {}", duration_ms);
        }
        // Close all file descriptions!
        for (auto package = 0u; package < optkit::Query::detect_cpu_packages().size(); package++)
            for (auto domain = 0u; domain < Query::rapl_domain_info().size(); domain++)
                ::close(fd_package_domain[package][domain]);
    }

    // returns event_name - socket_id - rapl_domain - reading
    std::unordered_map<std::string, std::map<int32_t, std::map<RaplDomain, double>>> Profiler::aggregate()
    {
        double total_duration = 0.0;
        std::unordered_map<std::string, std::map<int32_t, std::map<RaplDomain, double>>> aggregated_events;
        const std::vector<std::string> &event_names = this->metric_builder.event_names();

        for (const auto &entry : read_buffer)
        {
            total_duration += entry.first;
            const std::map<int32_t, std::map<RaplDomain, double>> &values = entry.second; // socket_id - rapl_domain - reading

            for (const auto &pair : values)
            {
                int32_t socket_id = pair.first;

                int j = 0;
                for (const auto &innerpair : pair.second)
                {
                    RaplDomain domain = innerpair.first;
                    double reading = innerpair.second;

                    // Aggregate the readings
                    aggregated_events[event_names[j++]][socket_id][domain] += reading;
                }
            }
        }
        std::vector<std::pair<std::string, std::map<int32_t, std::map<RaplDomain, double>>>> event_value(
            aggregated_events.begin(), aggregated_events.end());

        this->event_results = std::move(event_value);
        this->total_duration_ms = total_duration;
        return aggregated_events;
    }

    void Profiler::disable()
    {
        OPTKIT_CORE_WARN("Rapl cannot be disabled");
    }
    void Profiler::enable()
    {
        OPTKIT_CORE_WARN("Rapl is always enabled");
    }
    std::map<int32_t, std::map<RaplDomain, double>> Profiler::read()
    {
        std::map<int32_t, std::map<RaplDomain, double>> result;
        int64_t value = 0;
        const auto &avail_domains = Query::rapl_domain_info();

        for (size_t package = 0; package < OPTKIT_ENV_CPU_NUM_SOCKETS; ++package)
        {
            for (size_t domain = 0; domain < avail_domains.size(); ++domain)
            {
                const auto &selected_domain = avail_domains[domain];
                int fd = fd_package_domain[package][domain];

                if (fd == -1)
                    continue;

                if (::read(fd, &value, sizeof(value)) == sizeof(value))
                {
                    if (OPT_LIKELY(this->config.is_reset_after_read))
                        ::ioctl(fd, PERF_EVENT_IOC_RESET, 0);

                    result[static_cast<int32_t>(package)][selected_domain.domain] = static_cast<double>(value) * selected_domain.scale;
                }
            }
        }
        return result;
    }

    std::string Profiler::to_json()
    {
        std::stringstream ss;
        ss << "[\n";
        // based on the insertion order.
        ss << optkit::energy::rapl::to_json(this->config.measurement_type, this->read_buffer);
        ss << "]\n";
        return ss.str();
    }

    std::string to_string(const std::map<optkit::energy::rapl::RaplDomain, double> &map)
    {
        std::ostringstream oss;
        oss << map;
        return oss.str();
    }
    // Overloading << for map with RaplDomain as keys
    std::ostream &operator<<(std::ostream &os, const std::map<optkit::energy::rapl::RaplDomain, double> &map)
    {
        for (const auto &item : map)
            os << item.first << ": " << item.second << "\n";
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const std::map<int32_t, std::map<optkit::energy::rapl::RaplDomain, double>> &map)
    {
        const std::vector<optkit::energy::rapl::RaplDomainInfo> &avail_domains = optkit::energy::rapl::Query::rapl_domain_info();
        for (const auto &pair : map)
        {
            os << "\tPackage " << pair.first << "\n";
            for (const auto &innerpair : pair.second)
            {
                for (const auto &info : avail_domains)
                {
                    if (info.domain == innerpair.first)
                    {
                        os << "\t\t" << info.event << ": " << innerpair.second << " " << info.units << " Consumed.\n";
                    }
                }
            }
        }
        return os;
    }
} // namespace optkit