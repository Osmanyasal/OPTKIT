#include <iostream>
#include "optkit.hh"

void print_pmu();
void print_rapl();
void print_cpu();

int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT(false);

    print_cpu();
    print_rapl();
    print_pmu();
    return 0;
}

void print_pmu()
{
    std::cout << "============== Default PMU ==============\n\n";
    std::cout << optkit::core::pmu::cpu::perf::QueryPMU::default_pmu_info();

    std::cout << "\n";
    std::cout << "============== Avail PMUs ==============\n\n";

    optkit::core::pmu::cpu::perf::QueryPMU::list_avail_pmus();

    std::cout << "\n";
    std::cout << "============== Avail PMU Events ==============\n\n";
    for (int i : optkit::core::pmu::cpu::perf::QueryPMU::avail_pmu_ids())
    {
        optkit::core::pmu::cpu::perf::QueryPMU::list_avail_events(i);
        std::cout << "\n------------------------\n";
    }
    std::cout << std::endl;
}
void print_rapl()
{

    std::cout << "============== Rapl Domain Info ==============\n\n";
    for (auto &item : optkit::core::energy::rapl::QueryRapl::rapl_domain_info())
        std::cout << item << std::endl;

    std::cout << "\n";
    std::cout << "============== Rapl Read Methods ==============\n\n";
    std::cout << "RAPL PERF read avail: [" << optkit::core::energy::rapl::QueryRapl::is_rapl_perf_avail() << "]\n";
    std::cout << "RAPL powercap read avail: [" << optkit::core::energy::rapl::QueryRapl::is_rapl_powercap_avail() << "]\n";
    std::cout << "RAPL MSR read avail: [" << optkit::core::energy::rapl::QueryRapl::is_rapl_msr_avail() << "]\n";
    std::cout << std::endl;
}
void print_cpu()
{
    std::cout << "============== Detect Packages ==============\n";
    const auto &packages = optkit::core::Query::detect_cpu_packages();
    std::cout << packages << "\n";
    std::cout << "TOTAL # OF SOCKETS: " << optkit::core::Query::num_sockets << "\n";
    std::cout << "TOTAL # OF CORES: " << optkit::core::Query::num_cores << "\n\n";

    for (int socket = 0; socket < optkit::core::Query::num_sockets; socket++)
    {
        std::cout << "========== Socket[" << socket << "] ==========\n";
        try
        {
            const auto &core_ids = packages.at(socket);
            const auto &avail_freqs = optkit::core::frequency::QueryCPUFrequency::get_scaling_available_frequencies(core_ids[0]);

            std::cout << "  Core[0] Available Freqs (Hz): ";
            for (auto it = avail_freqs.rbegin(); it != avail_freqs.rend(); ++it)
                std::cout << *it << " ";
            std::cout << "\n";

            auto uncore_min_max = optkit::core::frequency::CPUFrequency::get_uncore_min_max(socket);
            auto uncore_freq = optkit::core::frequency::CPUFrequency::get_uncore_frequency(socket);

            std::cout << "  Uncore Frequency Range (Min-Max Hz): " << uncore_min_max << "\n";
            std::cout << "  Current Uncore Frequency (Hz): " << uncore_freq << "\n";
        }
        catch (const std::exception &e)
        {
            std::cerr << "  [Error] Failed to query socket[" << socket << "]: " << e.what() << "\n";
        }
        std::cout << "\n";
    }

    for (int i = 0; i < optkit::core::Query::num_cores; i++)
    {
        std::cout << "========== CPU Core[" << i << "] ==========\n";
        try
        {
            std::cout << "  BIOS Limit (Hz): " << optkit::core::frequency::QueryCPUFrequency::get_bios_limit(i) << "\n";
            std::cout << "  Min Core Freq (cpuinfo_min_freq): " << optkit::core::frequency::QueryCPUFrequency::get_cpuinfo_min_freq(i) << "\n";
            std::cout << "  Max Core Freq (cpuinfo_max_freq): " << optkit::core::frequency::QueryCPUFrequency::get_cpuinfo_max_freq(i) << "\n";
            std::cout << "  Scaling Min Limit (Hz): " << optkit::core::frequency::QueryCPUFrequency::get_scaling_min_limit(i) << "\n";
            std::cout << "  Scaling Max Limit (Hz): " << optkit::core::frequency::QueryCPUFrequency::get_scaling_max_limit(i) << "\n";
            std::cout << "  Scaling Driver: " << optkit::core::frequency::QueryCPUFrequency::get_scaling_driver(i) << "\n";

            const auto &freq_list = optkit::core::frequency::QueryCPUFrequency::get_scaling_available_frequencies(i);
            std::cout << "  Available Core Freqs (Hz): ";
            for (auto it = freq_list.rbegin(); it != freq_list.rend(); ++it)
                std::cout << *it << " ";
            std::cout << "\n";
        }
        catch (const std::exception &e)
        {
            std::cerr << "  [Error] Failed to query core[" << i << "]: " << e.what() << "\n";
        }
        std::cout << "\n";
    }
}