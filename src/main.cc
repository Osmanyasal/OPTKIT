#include <omp.h>
#include "optkit.hh"

#define OPTKIT_COMPUTE_INTENSITY(var_name, block_name)                                                                   \
    optkit::core::pmu::BlockGroupProfiler var_name                                                                       \
    {                                                                                                                    \
        block_name, "compute_and_memory_intensity", optkit::core::recepies::intel::icl::Recepies::freq_governor_events() \
    }

#define VECTOR_SIZE 100000000  // 100 million elements
#define NUM_ACCESSES 100000000 // 100 million random accesses

void random_access()
{
    // OPTKIT_COMPUTE_INTENSITY(ci_random_access, "random_access");

    // Initialize the vector with random values
    std::vector<double> vec(VECTOR_SIZE);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 10.0); // Random values between 0.0 and 10.0

    for (size_t i = 0; i < VECTOR_SIZE; ++i)
    {
        vec[i] = dis(gen);
    }

    double sum = 0.0;

#pragma omp parallel reduction(+ : sum)
    {
        std::mt19937 thread_gen(rd() ^ std::hash<int>{}(omp_get_thread_num() ^ time(nullptr)));
        std::uniform_int_distribution<> thread_dis(0, vec.size() - 1);

#pragma omp for
        for (int32_t i = 0; i < NUM_ACCESSES; ++i)
        {
            int32_t idx = thread_dis(thread_gen); // Get a truly random index
            sum += vec[idx];
        }
    }

    std::cout << "Sum: " << std::fixed << sum << std::endl; // Output the sum to check correctness
}

void print_info()
{

    for (size_t i = 0; i < optkit::core::Query::num_sockets; i++)
    {
        std::cout << "Socket [" << i << "]\n";
        std::cout << "Core avail: ";
        const auto &freq_list = optkit::core::freq::QueryFreq::get_scaling_available_frequencies(optkit::core::Query::detect_packages().at(i)[0]);
        for (auto iter = freq_list.rbegin(); iter != freq_list.rend(); iter++)
            std::cout << *iter << " ";
        std::cout << "\n";
        std::cout << "Core current: " << optkit::core::freq::CPUFrequency::get_core_frequency(optkit::core::Query::detect_packages().at(i)[0]) << "\n";
        std::cout << "Uncore min-max: " << optkit::core::freq::CPUFrequency::get_uncore_min_max(i) << "\n";
        std::cout << "Uncore current: " << optkit::core::freq::CPUFrequency::get_uncore_frequency(i) << "\n";
    }
}

void triad()
{
    BLOCK_TIMER("Triad Block");
    double aa = 0;

#pragma omp parallel for
    for (int32_t i = 0; i < 900000000; i++)
        aa = aa + i * 0.052; // 2 * 900_000_000 -> 1_800_000_000

    std::cout << aa << std::endl;
    std::cout << "hello from osman yasal\n";
}

// fibonacci function
int32_t fib(int32_t n)
{
    int64_t i, a = 0;
    int32_t b = 1;
    for (i = 0; i < n; i++)
    {
        b = b + a;
        a = b - a;
    }

    return b;
}

// work function
int32_t work(int32_t n, int32_t f)
{
    int32_t i, b;
    b = 0;
    for (i = 0; i < n; i++)
    {
        b += fib(f);
    }

    return b;
}

void print_pmu()
{
    std::cout << "============== Default PMU ==============\n\n";
    std::cout << optkit::core::pmu::QueryPMU::default_pmu_info();

    std::cout << "\n";
    std::cout << "============== Avail PMUs ==============\n\n";

    optkit::core::pmu::QueryPMU::list_avail_pmus();

    std::cout << "\n";
    std::cout << "============== Avail PMU Events ==============\n\n";
    for (int i : optkit::core::pmu::QueryPMU::avail_pmu_ids())
    {
        optkit::core::pmu::QueryPMU::list_avail_events(i);
        std::cout << "\n------------------------\n";
    }
    std::cout << std::endl;
}

#include <memory>

struct Temp
{
    std::vector<std::pair<uint64_t, std::string>> get_mapping()
    {
        static std::vector<std::pair<uint64_t, std::string>> default_mapping{
            {0x3c, "CPU_CLK_UNHALTED"},                                               // 0
            {(0x9c | 0x100), "IDQ_UOPS_NOT_DELIVERED"},                               // 1
            {(0xe | 0x100), "UOPS_ISSUED_ANY"},                                       // 2
            {(0xc2 | 0x200), "UOPS_RETIRED_RETIRE_SLOTS"},                            // 3
            {(0xd | 0x300 | (1 << INTEL_X86_CMASK_BIT)), "INT_MISC_RECOVERY_CYCLES"}, // 4
            {0xc0, "INSTRUCTION_RETIRED"}};                                           // 5

        return default_mapping;
    }
    Temp()
    {
        std::cout << "ctor Temp\n";
        profiler_ref = std::unique_ptr<optkit::core::pmu::BlockGroupProfiler>(
            new optkit::core::pmu::BlockGroupProfiler("main block", "level1", get_mapping(), false, optkit::core::ProfilerConfig{false, true, true, 0, -1}));
    }

    ~Temp()
    {
        std::cout << "dtor Temp\n";

        auto pmu_record = profiler_ref->read_val();
        double SLOTS = 4 * pmu_record[0];

        std::cout << ((double)pmu_record[5] / pmu_record[0]) << "\n";
        std::cout << (pmu_record[1] / SLOTS) * 100.0 << "\n";                                       // IDQ_UOPS_NOT_DELIVERED.CORE / Slots
        std::cout << (pmu_record[3] / SLOTS) * 100.0 << "\n";                                       // UOPS_RETIRED.RETIRE_SLOTS / Slots
        std::cout << ((pmu_record[2] - pmu_record[3] + 4 * pmu_record[4]) / SLOTS) * 100.0 << "\n"; // (UOPS_ISSUED.ANY - UOPS_RETIRED.RETIRE_SLOTS + 4* INT_MISC.RECOVERY_CYCLES) / Slots
    }

    std::unique_ptr<optkit::core::BaseProfiler<std::vector<uint64_t>>> profiler_ref;
};

void normalMatrixMultiplication(const std::vector<std::vector<int>> &A, const std::vector<std::vector<int>> &B, std::vector<std::vector<int>> &C)
{
    int n = A.size();
    int m = B[0].size();
    int p = B.size();

    for (int i = 0; i < n; i++)
    { // Iterate over rows of A
        for (int j = 0; j < m; j++)
        {                // Iterate over columns of B
            for (int k = 0; k < p; k++)
            { // Sum over the common dimension
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void loopInterchangeMatrixMultiplication(const std::vector<std::vector<int>> &A, const std::vector<std::vector<int>> &B, std::vector<std::vector<int>> &C)
{
    int n = A.size();
    int m = B[0].size();
    int p = B.size();

    for (int i = 0; i < n; i++)
    { // Iterate over rows of A
        for (int k = 0; k < p; k++)
        { // Iterate over columns of A/rows of B
            for (int j = 0; j < m; j++)
            { // Iterate over columns of B
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT();
    // OPTKIT_RAPL(rapl_var, "main_block");
    {
        // Temp tmp{};
        // static std::vector<std::pair<uint64_t, std::string>> default_mapping{
        //     {0x3c, "CPU_CLK_UNHALTED"},                                               // 0
        //     {(0x9c | 0x100), "IDQ_UOPS_NOT_DELIVERED"},                               // 1
        //     {(0xe | 0x100), "UOPS_ISSUED_ANY"},                                       // 2
        //     {(0xc2 | 0x200), "UOPS_RETIRED_RETIRE_SLOTS"},                            // 3
        //     {(0xd | 0x300 | (1 << INTEL_X86_CMASK_BIT)), "INT_MISC_RECOVERY_CYCLES"}, // 4
        //     {0xc0, "INSTRUCTION_RETIRED"}};                                           // 5

        // std::unique_ptr<optkit::core::BaseProfiler<std::vector<uint64_t>>> profiler_ref = std::unique_ptr<optkit::core::pmu::BlockGroupProfiler>(
        //     new optkit::core::pmu::BlockGroupProfiler("main block", "level1", default_mapping));

        // optkit::core::pmu::BlockGroupProfiler bb{"main block", "level1", {{0x0400ull, "slot"}, {0x8000ull, "ret"}, {0x8100ull, "bs"}, {0x8200ull, "fe"}, {0x8300ull, "be"}, {optkit::intel::icl::CYCLE_ACTIVITY, "fe"}}};
        // optkit::core::pmu::BlockGroupProfiler bb{"main block", "level1", default_mapping};

        // BlockGroupProfiler bb{"main block", "level2", {{0x0400ull, "slot"}, {0x8400ull, "heavy"}, {0x8500ull, "br_mispredict"}, {0x8600ull, "fetch_lat"}, {0x8700ull, "mem_bound"}}};

        // random_access();
        // // do some work
        // std::cout << work(10, 6000000) << "\n";

        // auto pmu_record = profiler_ref->read_val();
        // double SLOTS = 4 * pmu_record[0];

        // std::cout << ((double)pmu_record[5] / pmu_record[0]) << "\n";
        // std::cout << (pmu_record[1] / SLOTS) * 100.0 << "\n";                                       // IDQ_UOPS_NOT_DELIVERED.CORE / Slots
        // std::cout << (pmu_record[3] / SLOTS) * 100.0 << "\n";                                       // UOPS_RETIRED.RETIRE_SLOTS / Slots
        // std::cout << ((pmu_record[2] - pmu_record[3] + 4 * pmu_record[4]) / SLOTS) * 100.0 << "\n"; // (UOPS_ISSUED.ANY - UOPS_RETIRED.RETIRE_SLOTS + 4* INT_MISC.RECOVERY_CYCLES) / Slots

        // auto val = bb.read_val();
        // auto ret = (double)val[1] / (double)val[0] * 100.0;
        // auto bs = (double)val[2] / (double)val[0] * 100.0;
        // auto fe = (double)val[3] / (double)val[0] * 100.0;
        // auto be = (double)val[4] / (double)val[0] * 100.0;
        // std::cout << ret << " " << bs << " " << fe << " " << be << " " << val[5] << "\n";
    }

    // {
    //     OPTKIT_PERFORMANCE_EVENTS("triad_block", "flop", var, {{icl::FP_ARITH | icl::FP_ARITH__MASK__INTEL_ICL_FP_ARITH_INST_RETIRED__SCALAR_DOUBLE, "double_operations"}});
    //     // BlockProfiler triad_block{"triad_block", "flop", {{icl::FP_ARITH | icl::FP_ARITH__MASK__INTEL_ICL_FP_ARITH_INST_RETIRED__SCALAR_DOUBLE, "double_operations"}}};
    //     triad();
    // }

    // print_pmu();

    // Example L1Metric unordered_map
    // std::unordered_map<optkit::core::recepies::L1Metric, double> L1Data = {
    //     {optkit::core::recepies::L1Metric::BackendBound, 0.5},
    //     {optkit::core::recepies::L1Metric::BadSpeculation, 0.3},
    //     {optkit::core::recepies::L1Metric::Retiring, 0.2}};

    // Example L2Metric unordered_map
    // std::unordered_map<optkit::core::recepies::L2Metric, double> L2Data = {
    //     {optkit::core::recepies::L2Metric::MemoryBound, 0.7},
    //     {optkit::core::recepies::L2Metric::CoreBound, 0.1},
    //     {optkit::core::recepies::L2Metric::BranchMisprediction, 0.05}};

    // std::cout << optkit::core::recepies::to_string(optkit::core::recepies::L1Metric::BackendBound);
    // // Print the L1 data
    // std::cout << "L1 Data: " << L1Data << std::endl;

    // // Print the L2 data
    // std::cout << "L2 Data: " << L2Data << std::endl;

    // optkit::core::recepies::TMAnalysis tma{"main", "tma analysis"};
    // tma.begin_monitoring(optkit::core::recepies::L1Metric::BackendBound);

    OPTKIT_TMA_ANALYSIS("main", temp, optkit::core::recepies::L1Metric::Default);

    const int SIZE = 2000; // 2k
    std::vector<std::vector<int>> A(SIZE, std::vector<int>(SIZE));
    std::vector<std::vector<int>> B(SIZE, std::vector<int>(SIZE));
    std::vector<std::vector<int>> C(SIZE, std::vector<int>(SIZE, 0));

    srand(42); // Fixed seed for reproducibility
    // Initialize matrices A and B with random values
    // srand(static_cast<unsigned>(time(0)));
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            A[i][j] = rand() % 100; // Random values between 0 and 99
            B[i][j] = rand() % 100;
        }
    }

    // normalMatrixMultiplication(A, B, C);
    loopInterchangeMatrixMultiplication(A, B, C);

    // std::cout << "Result of loop interchange matrix multiplication:\n";
    // for (const auto &row : C)
    // {
    //     for (int val : row)
    //     {
    //         std::cout << val << " ";
    //     }
    //     std::cout << "\n";
    // }

    // float fp = 0.0;
    // double fd = 0.0;
    // OPTKIT_PERFORMANCE_EVENTS("TEST", "FLOPS", tt, {{0x00c7 | optkit::intel::icl::FP_ARITH_INST_RETIRED__MASK__INTEL_ICL_FP_ARITH_INST_RETIRED__SCALAR_SINGLE | optkit::intel::icl::FP_ARITH_INST_RETIRED__MASK__INTEL_ICL_FP_ARITH_INST_RETIRED__SCALAR_DOUBLE, "FLOPS SINGLE"}});

    // optkit::core::pmu::BlockGroupProfiler bb{"main block", "level1", {{0x00c3 | 0x0100ull | (0x1 << INTEL_X86_CMASK_BIT) | (0x1 << INTEL_X86_EDGE_BIT), "MACHINE_CLEARS"}}};
    // optkit::core::pmu::BlockGroupProfiler bb{"main block", "level1", {{0xa3 | 0x0c00 | 0x0500 | 0x0600, "STALLED_CYCLES_MEM_ANY"}}};
    // optkit::core::pmu::BlockGroupProfiler bb{"main block", "level1", {{0xa3 | 0x0c00 | (0xc << INTEL_X86_CMASK_BIT), "STALLED_CYCLES_MEM_ANY"}, {0xa3 | 0x0500 | (0x5 << INTEL_X86_CMASK_BIT), "STALLED_CYCLES_MEM_ANY2"}, {0xa3 | 0x0600 | (0x6 << INTEL_X86_CMASK_BIT), "STALLED_CYCLES_MEM_ANY3"}}};

    // std::cout << work(10, 6000000) << "\n";
    // std::cout << work(5, 4) << "\n";
    // random_access();

    // for (int i = 0; i < 10; i++)
    // {
    //     fd++;
    //     fp += i;
    // }

    // std::cout << fp << "\n";

    std::cout << "program completed!\n";
    return 0;
}
/**
 *
 *
    {CPU UTIL = 0.902322, IPC = 1.49041, L1 Frontend Bound = 0.0101911, L1 Bad Speculation = 0.00780583, L1 Retiring = 0.310562, L1 Backend Bound = 0.671441}
    {CPU UTIL = 1.08685, IPC = 1.48424, L1 Frontend Bound = 0.0160113, L1 Bad Speculation = 0.0112863, L1 Retiring = 0.309331, L1 Backend Bound = 0.663372}
    {CPU UTIL = 0.979173, IPC = 1.49616, L1 Frontend Bound = 0.00292939, L1 Bad Speculation = 0.00139238, L1 Retiring = 0.311738, L1 Backend Bound = 0.68394}
 *
 */