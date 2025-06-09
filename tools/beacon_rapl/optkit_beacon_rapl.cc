#include <iostream>
#include <iomanip>
#include <csignal>
#include <atomic>
#include <thread>
#include <string>
#include <optkit.hh>

std::atomic<bool> terminate_flag{false};

void signal_handler(int);
void print_usage(const char *program_name);
void argument_parsing(int32_t argc, char **argv);

int duration_sec = -1;

int32_t main(int32_t argc, char **argv)
{
    argument_parsing(argc, argv);

    OPTKIT_INIT({false});

    // Register signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    OPTKIT_INFO("Monitoring energy ({} sec). Ctrl+C to stop.", duration_sec == -1 ? "unlimited" : std::to_string(duration_sec));

    OPTKIT_CPU_ENERGY(energy, "energy_monitoring");

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
            auto energy_data = energy.read(); // map<int32_t (socket), map<RaplDomain, double>>

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

void signal_handler(int)
{
    terminate_flag.store(true);
}

void print_usage(const char *program_name)
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

void argument_parsing(int32_t argc, char **argv)
{
    // Argument parsing loop
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h")
        {
            print_usage(argv[0]);
            std::exit(EXIT_SUCCESS);
        }
        else if (arg == "--duration" || arg == "-d")
        {
            if (i + 1 >= argc)
            {
                print_usage(argv[0]);
                std::cerr << "Error: --duration requires a value.\n";
                std::exit(EXIT_FAILURE);
            }
            try
            {
                duration_sec = std::stoi(argv[++i]);
                if (duration_sec < 0){
                    std::cerr << "Negative value\n";
                    std::exit(EXIT_FAILURE);
                }
            }
            catch (...)
            {
                std::cerr << "Error: Invalid value for --duration.\n";
                std::exit(EXIT_FAILURE);
            }
        }
        else
        {
            print_usage(argv[0]);
            std::cerr << "Unknown option: " << arg << "\n";
            std::exit(EXIT_FAILURE);
        }
    }
}