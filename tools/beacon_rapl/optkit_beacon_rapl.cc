#include <iostream>
#include <iomanip>
#include <csignal>
#include <atomic>
#include <optkit.hh>
#include <thread>
#include <string>

std::atomic<bool> terminate_flag{false};

void signal_handler(int)
{
    terminate_flag.store(true);
}


void print_usage(const char* program_name)
{
    std::cout << "Usage:\n"
              << "  " << program_name << " [OPTIONS]\n\n"
              << "Options:\n\t"
              << "If no duration is specified, the tool reports energy data once per second until interrupted" 
              << "  -d, --duration <seconds>    \n\tSet duration for monitoring in seconds (default: run until Ctrl+C)\n"
              << "  -h, --help                  \n\tShow this help message and exit\n\n"
              << "Examples:\n"
              << "  " << program_name << " --duration 60\n"
              << "  " << program_name << "\n";
}


int32_t main(int32_t argc, char **argv)
{
    // Argument parsing
    int duration_sec = -1;

    // Argument parsing loop
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        }
        else if (arg == "--duration" || arg == "-d") {
            if (i + 1 >= argc) {
                std::cerr << "Error: --duration requires a value.\n";
                print_usage(argv[0]);
                return 1;
            }
            try {
                duration_sec = std::stoi(argv[++i]);
                if (duration_sec < 0) throw std::invalid_argument("Negative value");
            } catch (...) {
                std::cerr << "Error: Invalid value for --duration.\n";
                return 1;
            }
        }
        else {
            std::cerr << "Unknown option: " << arg << "\n";
            print_usage(argv[0]);
            return 1;
        }
    }

    OPTKIT_INIT({false});

    // Register signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    OPTKIT_INFO("Monitoring energy ({} sec). Ctrl+C to stop.", duration_sec == -1 ? "unlimited" : std::to_string(duration_sec));

    // Initialize RAPL
    optkit::core::energy::rapl::RaplProfiler rapl{"energy_monitor", "rapl",
        optkit::core::energy::rapl::RaplConfig{
            optkit::core::energy::rapl::RaplReadMethods::PERF,
            (int32_t)optkit::core::energy::rapl::RaplDomain::ALL,
            true, false
        }
    };

    double total_energy = 0.0;

    try
    {
        auto start = std::chrono::steady_clock::now();

        while (!terminate_flag &&
               (duration_sec == -1 || std::chrono::duration_cast<std::chrono::seconds>(
                                          std::chrono::steady_clock::now() - start)
                                              .count() < duration_sec))
        {
            auto energy_data = rapl.read(); // map<int32_t (socket), map<RaplDomain, double>>

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
                      << std::setw(3) << std::chrono::duration_cast<std::chrono::seconds>(
                                             std::chrono::steady_clock::now() - start)
                                             .count()
                      << ": " << std::fixed << std::setprecision(3)
                      << energy_this_second << " [Joules]\n";

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch (...)
    {
        OPTKIT_ERROR("Monitoring interrupted");
    }

    std::cout << "\n=== TOTAL ENERGY CONSUMPTION ===\n"
              << std::fixed << std::setprecision(3) << total_energy << " [Joules]\n";

    return 0;
}
