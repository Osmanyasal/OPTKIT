#include "core/energy/cpu/rapl/profiler.hh"

namespace optkit::energy::rapl
{
    Profiler::Profiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<double> &mb) : BaseProfiler{profiler_config}, metric_builder{mb}
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

        // socket_id_str - <socket_id - <rapl_domain - read value>>
        const auto aggregated = aggregate(); // Use const reference to avoid copy

        // Reserve space for metric_results to avoid reallocations
        this->metric_results.reserve(aggregated.size());

        for (const auto &aggr_item : aggregated)
        {
            // Pre-allocate event_values with estimated size
            std::vector<std::pair<std::string, double>> event_values;
            event_values.reserve(aggr_item.second.size() * 4 + 1); // Estimate + duration

            for (const auto &inner_pair : aggr_item.second)
            {
                for (const auto &domain_pair : inner_pair.second)
                {
                    // Avoid temporary string creation by moving
                    event_values.emplace_back(to_string(domain_pair.first), domain_pair.second);
                }
            }
            event_values.emplace_back("duration_microsec", this->total_duration_ms * 1000.0); // convert to microseconds

            // Use more efficient conversion and avoid intermediate unordered_map creation
            int32_t socket_id = static_cast<int32_t>(std::strtol(aggr_item.first.c_str(), nullptr, 10));

            // Create unordered_map directly with iterators to avoid copy
            std::unordered_map<std::string, double> event_map;
            event_map.reserve(event_values.size());
            for (const auto &pair : event_values)
            {
                event_map.emplace(pair.first, pair.second);
            }

            this->metric_results[socket_id] = this->metric_builder.calculate(std::move(event_map));
        }

        // call it for socket 0 and 1 and so on...
        if (OPT_LIKELY(this->config.dump_results_to_file))
            this->save();

        if (OPT_LIKELY(this->config.verbose))
        {
            if (OPT_UNLIKELY(this->metric_builder.print_events))
            {
                for (const auto &event : this->event_results)
                    std::cout << event.second << std::endl;
            }

            for (const auto &metric : this->metric_results)
            {
                std::cout << "\tPackage " << metric.first << " Metrics: \n";
                for (const auto &pair : metric.second)
                    std::cout << std::fixed << "\t\t" << pair.first << ":" << pair.second << "\n";
            }
        }

        // Close all file descriptions! (more efficient with range check)
        const auto &domain_info = Query::rapl_domain_info();
        for (size_t package = 0; package < OPTKIT_ENV_CPU_NUM_SOCKETS && package < fd_package_domain.size(); ++package)
        {
            for (size_t domain = 0; domain < domain_info.size() && domain < fd_package_domain[package].size(); ++domain)
            {
                if (fd_package_domain[package][domain] != -1)
                    ::close(fd_package_domain[package][domain]);
            }
        }
    }

    // returns socket_id_str - <socket_id - <rapl_domain - read value>>
    std::unordered_map<std::string, std::unordered_map<int32_t, std::unordered_map<RaplDomain, double>>> Profiler::aggregate()
    {
        double total_duration = 0.0;
        std::unordered_map<std::string, std::unordered_map<int32_t, std::unordered_map<RaplDomain, double>>> aggregated_events;

        // Reserve space for expected number of sockets to reduce rehashing
        aggregated_events.reserve(OPTKIT_ENV_CPU_NUM_SOCKETS);

        for (const auto &entry : read_buffer)
        {
            total_duration += entry.first;
            const auto &values = entry.second; // socket_id - rapl_domain - reading

            for (const auto &pair : values)
            {
                int32_t socket_id = pair.first;
                std::string socket_id_str = std::to_string(socket_id); // Cache the string conversion

                // Pre-allocate nested maps if they don't exist
                auto &socket_map = aggregated_events[socket_id_str];
                auto &domain_map = socket_map[socket_id];

                for (const auto &innerpair : pair.second)
                {
                    RaplDomain domain = innerpair.first;
                    double reading = innerpair.second;

                    // Aggregate the readings (more efficient access pattern)
                    domain_map[domain] += reading;

                    // Only output debug info if verbose mode is enabled
                    if (OPT_UNLIKELY(this->config.verbose))
                    {
                        std::cout << "Aggregating Event: " << to_string(domain) << " Socket: " << socket_id
                                  << " Domain: " << domain << " Reading: " << reading
                                  << " total:" << domain_map[domain] << "\n";
                    }
                }
            }
        }

        // More efficient move construction
        this->event_results.clear();
        this->event_results.reserve(aggregated_events.size());
        for (auto &item : aggregated_events)
        {
            this->event_results.emplace_back(std::move(item.first), std::move(item.second));
        }

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

    std::unordered_map<int32_t, std::unordered_map<RaplDomain, double>> Profiler::read()
    {
        std::unordered_map<int32_t, std::unordered_map<RaplDomain, double>> result;
        int64_t value = 0;
        const auto &avail_domains = Query::rapl_domain_info();

        // Reserve space for expected sockets to reduce rehashing
        result.reserve(OPTKIT_ENV_CPU_NUM_SOCKETS);

        for (size_t package = 0; package < OPTKIT_ENV_CPU_NUM_SOCKETS && package < fd_package_domain.size(); ++package)
        {
            int32_t package_id = static_cast<int32_t>(package);
            auto &package_result = result[package_id]; // Get reference to avoid repeated map lookups

            for (size_t domain = 0; domain < avail_domains.size() && domain < fd_package_domain[package].size(); ++domain)
            {
                const auto &selected_domain = avail_domains[domain];
                int fd = fd_package_domain[package][domain];

                if (OPT_UNLIKELY(fd == -1))
                    continue;

                if (OPT_LIKELY(::read(fd, &value, sizeof(value)) == sizeof(value)))
                {
                    if (OPT_LIKELY(this->config.is_reset_after_read))
                        ::ioctl(fd, PERF_EVENT_IOC_RESET, 0);

                    double scaled_value = static_cast<double>(value) * selected_domain.scale;
                    package_result[selected_domain.domain] = scaled_value;

                    // Only output debug info if verbose mode is enabled
                    if (OPT_UNLIKELY(this->config.verbose))
                    {
                        std::cout << "Read Package " << package << " Domain " << selected_domain.event
                                  << ": " << scaled_value << " " << selected_domain.units << "\n";
                    }
                }
            }
        }
        return result;
    }

    std::string Profiler::to_json()
    {
        // Pre-allocate string with estimated size to reduce reallocations
        std::string result;
        result.reserve(2048); // Estimated size for typical JSON output

        result += "[\n";
        bool first = true;

        // Create a map of all sockets and their data for efficient processing
        std::unordered_map<int32_t, std::pair<std::vector<std::pair<std::string, double>>, std::vector<std::pair<std::string, double>>>> socket_data;

        // Pre-allocate and process events
        for (const auto &event_pair : this->event_results)
        {
            int32_t socket_id = std::stoi(event_pair.first);
            auto &socket_entry = socket_data[socket_id];
            auto &events = socket_entry.first;

            // Reserve space for events to reduce reallocations
            events.reserve(event_pair.second.size() * 4); // Estimate based on typical domain count

            for (const auto &socket_pair : event_pair.second)
            {
                for (const auto &domain_pair : socket_pair.second)
                {
                    // Avoid string concatenation by pre-computing domain name
                    events.emplace_back(to_string(domain_pair.first) + "__Joules", domain_pair.second);
                }
            }
        }

        // Process metrics and add to existing socket data or create new entries
        for (const auto &metric_pair : this->metric_results)
        {
            int32_t socket_id = metric_pair.first;
            auto &socket_entry = socket_data[socket_id];
            auto &metrics = socket_entry.second;

            // Reserve space for metrics
            metrics.reserve(metric_pair.second.size());

            for (const auto &metric : metric_pair.second)
            {
                metrics.emplace_back(metric.first, metric.second);
            }
        }

        // Generate JSON for each socket in a single pass
        for (const auto &socket_entry : socket_data)
        {
            int32_t socket_id = socket_entry.first;
            const auto &events = socket_entry.second.first;
            const auto &metrics = socket_entry.second.second;

            if (!first)
                result += ",\n";
            first = false;

            // Generate JSON directly without intermediate nlohmann::json object for better performance
            nlohmann::json socket_json = utils::to_json<double>(
                this->total_duration_ms,
                this->config.measurement_type,
                events,
                metrics,
                socket_id);
            result += socket_json.dump(2);
        }

        result += "\n]\n";
        return result;
    }

    std::string to_string(const std::unordered_map<optkit::energy::rapl::RaplDomain, double> &map)
    {
        std::ostringstream oss;
        oss << map;
        return oss.str();
    }
    // Overloading << for map with RaplDomain as keys
    std::ostream &operator<<(std::ostream &os, const std::unordered_map<optkit::energy::rapl::RaplDomain, double> &map)
    {
        for (const auto &item : map)
            os << item.first << ": " << item.second << "\n";
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const std::unordered_map<int32_t, std::unordered_map<optkit::energy::rapl::RaplDomain, double>> &map)
    {
        for (const auto &pair : map)
        {
            os << "\tPackage " << pair.first << " Domains:\n";
            for (const auto &innerpair : pair.second)
            {
                os << "\t\t" << innerpair.first << ": " << innerpair.second << " Joules Consumed.\n";
            }
        }
        return os;
    }
}