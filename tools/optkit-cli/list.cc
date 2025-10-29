#include "utils.hh"

void execute_list_command(const CommandArgs &args)
{
    std::cout << "Listing " << to_string(args.list_type);

    if (args.target != Target::ALL)
    {
        std::cout << " for " << (args.target == Target::CPU ? "CPU" : "GPU");
    }
    std::cout << "\n";

    auto all_metrics = optkit::metrics::performance::cpu_metrics::all_metrics();
    auto pmu_ids = optkit::pmu::cpu::Query::avail_pmu_ids();

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
        for (const auto &metric : all_metrics.metric_names())
            std::cout << "\t\t" << metric << "\n";
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
        for (const auto &metric : all_metrics.metric_names())
            std::cout << "\t\t" << metric << "\n";
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