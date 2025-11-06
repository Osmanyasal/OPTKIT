#include "utils.hh"
#include <unordered_map>
#include <vector>
#include <string>

void execute_list_command(const CommandArgs &args)
{
    std::cout << "Listing " << to_string(args.list_type);

    if (args.target != Target::ALL)
    {
        std::cout << " for " << (args.target == Target::CPU ? "CPU" : "GPU");
    }
    std::cout << "\n";

    // Collect metrics by category
    std::unordered_map<std::string, std::vector<std::string>> metrics_by_category;
    if (args.target == Target::ALL)
    {
        metrics_by_category["cpu_performance"] = optkit::metrics::performance::cpu_metrics::get_all_metrics();
        metrics_by_category["cpu_energy"] = optkit::metrics::energy::cpu_metrics::get_all_metrics();
        metrics_by_category["gpu_energy"] = optkit::metrics::energy::gpu_metrics::get_all_metrics();
        metrics_by_category["disk"] = optkit::metrics::disk::core_metrics::get_all_metrics();
    }
    else if (args.target == Target::CPU)
    {
        metrics_by_category["cpu_performance"] = optkit::metrics::performance::cpu_metrics::get_all_metrics();
        metrics_by_category["cpu_energy"] = optkit::metrics::energy::cpu_metrics::get_all_metrics();
    }
    else if (args.target == Target::GPU)
    {
        metrics_by_category["gpu_energy"] = optkit::metrics::energy::gpu_metrics::get_all_metrics();
    }

    std::vector<int32_t> pmu_ids;
    if (args.target == Target::CPU)
        pmu_ids = optkit::pmu::cpu::Query::avail_pmu_ids();

    switch (args.list_type)
    {
    case ListType::ALL:
        std::cout << "\tAvailable PMU info\n";
        for (auto &&id : pmu_ids)
            std::cout << optkit::pmu::cpu::Query::pmu_info(id) << "\n";

        std::cout << "\tAvailable PMU events\n";
        for (const auto &pmu_id : pmu_ids)
        {
            auto events = optkit::pmu::cpu::Query::get_avail_events(pmu_id);
            for (const auto &event : events)
                std::cout << "\t\t" << event << "\n";
        }
        std::cout << "\tAvailable metrics\n";
        for (const auto &metric : metrics_by_category)
        {
            std::cout << "\t" << metric.first << "\n";
            for (auto &&i : metric.second)
                std::cout << "\t\t" << i << "\n";
        }
        break;
    case ListType::EVENTS:
        std::cout << "\tAvailable PMU events\n";
        for (const auto &pmu_id : pmu_ids)
        {
            auto events = optkit::pmu::cpu::Query::get_avail_events(pmu_id);
            for (const auto &event : events)
                std::cout << "\t\t" << event << "\n";
        }
        break;

    case ListType::METRICS:
        std::cout << "\tAvailable metrics\n";
        for (const auto &metric : metrics_by_category)
        {
            std::cout << "\t" << metric.first << "\n";
            for (auto &&i : metric.second)
                std::cout << "\t\t" << i << "\n";
        }
        break;

    case ListType::PMU:
        std::cout << "\tAvailable PMU info\n";
        for (const auto &pmu_id : pmu_ids)
        {
            std::cout << optkit::pmu::cpu::Query::pmu_info(pmu_id) << "\n";
        }
        break;
    default:
        std::cerr << "Error: Unknown list type\n";
        break;
    }
}