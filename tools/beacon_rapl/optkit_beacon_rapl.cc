#include <iostream>
#include <iomanip>
#include <csignal>
#include <atomic>
#include <optkit.hh>
#include <thread>

std::atomic<bool> terminate_flag{false};

void signal_handler(int)
{
    terminate_flag.store(true);
}

int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT({false});

    // Parse optional duration argument
    int duration_sec = argc > 1 ? std::atoi(argv[1]) : -1;

    // Register signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    OPTKIT_INFO("Monitoring energy ({} sec). Ctrl+C to stop.", duration_sec);

    // Initialize RAPL
    optkit::core::energy::rapl::RaplProfiler rapl {"energy_monitor", "rapl", optkit::core::energy::rapl::RaplConfig{optkit::core::energy::rapl::RaplReadMethods::PERF, (int32_t)optkit::core::energy::rapl::RaplDomain::ALL, true, false}};

    double total_energy = 0.0;

    try
    {
        auto start = std::chrono::steady_clock::now();

        while (!terminate_flag &&
               (duration_sec == -1 || std::chrono::duration_cast<std::chrono::seconds>(
                                          std::chrono::steady_clock::now() - start)
                                              .count() <= duration_sec))
        {

            auto energy_data = rapl.read_and_store(); // map<int32_t (socket), map<RaplDomain, double>>

            double energy_this_second = 0.0;

            for (const auto &[socket, domain_energy_map] : energy_data)
            {
                for (const auto &[domain, energy] : domain_energy_map)
                {
                    energy_this_second += energy;
                }
            }

            total_energy += energy_this_second;

            std::cout << "Second "
                      << std::setw(3) << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count()
                      << ": " << std::fixed << std::setprecision(3)
                      << energy_this_second << " J\n";

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch (...)
    {
        OPTKIT_ERROR("Monitoring interrupted");
    }

    std::cout << "\n=== TOTAL ENERGY CONSUMPTION ===\n"
              << std::fixed << std::setprecision(3) << total_energy << " J\n";

    return 0;
}
