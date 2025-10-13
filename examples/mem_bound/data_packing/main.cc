#include <algorithm>
#include <iostream>

#include "solution.hh"
#include "solution_patch.hh"
#include "constants.hh"
#include "optkit.hh"

// Forward declarations
template <typename T>
T create_entry(int first_value, int second_value);

template <typename T>
void init(std::vector<T> &arr);

void solution(std::vector<S> &arr);
void solution(std::vector<S_patch> &arr);

template <typename Received_t, typename Expected_t>
static void reportError(const char *var_name, Received_t received,
                        Expected_t expected, int first_value, int second_value)
{
    std::cerr << "Validation Failed. Value " << var_name << " is " << received
              << ". Expected is " << expected << " for intialization values "
              << first_value << " and " << second_value << std::endl;
}

bool check_entry(int first, int second)
{
    S entry = create_entry<S>(first, second);

    bool isValid = true;

    if (entry.i != first)
    {
        reportError("i", entry.i, first, first, second);
        isValid = false;
    }

    if (entry.s != second)
    {
        reportError("s", entry.s, second, first, second);
        isValid = false;
    }

    const auto expected_l = static_cast<short>(first * second);
    if (entry.l != expected_l)
    {
        reportError("l", entry.l, expected_l, first, second);
        isValid = false;
    }

    const auto expected_d = static_cast<double>(first) / DATA_PACKING_MAX_RANDOM;
    if (std::abs(float(entry.d - expected_d)) > 0.001)
    {
        reportError("d", entry.d, expected_d, first, second);
        isValid = false;
    }

    const auto expected_b = (first < second);
    if (entry.b != expected_b)
    {
        reportError("b", entry.b, expected_b, first, second);
        isValid = false;
    }

    return isValid;
}

std::ostream &operator<<(std::ostream &os, const S &s)
{
    os << "{ i: " << s.i << ", s: " << s.s << ", l: " << s.l << ", d: " << s.d << ", b: " << s.b << " }";
    return os;
}

int main()
{
    OPTKIT_INIT(false);
    std::vector<S> arr(DATA_PACKING_N);
    init<S>(arr);

    auto expected = arr;
    solution(arr);
    if (!std::is_sorted(arr.begin(), arr.end()))
    {
        std::cerr << "Validation Failed. Array is not properly sorted." << std::endl;
        return 1;
    }
    auto cmp_eq = [](const S a, const S b)
    {
        return std::tie(a.i, a.s, a.l, a.d, a.b) == std::tie(b.i, b.s, b.l, b.d, b.b);
    };
    auto cmp_less = [](const S a, const S b)
    {
        return std::tie(a.i, a.s, a.l, a.d, a.b) < std::tie(b.i, b.s, b.l, b.d, b.b);
    };
    std::sort(expected.begin(), expected.end(), cmp_less);
    std::sort(arr.begin(), arr.end(), cmp_less);

    for (int i = 0; i < DATA_PACKING_N; i++)
    {
        if (!cmp_eq(arr[i], expected[i]))
        {
            std::cerr << "Validation Failed. Result[" << i << "] = " << arr[i]
                      << ". Expected[" << i << "] = " << expected[i] << std::endl;
            return 1;
        }
    }

    bool checks_passed = check_entry(DATA_PACKING_MIN_RANDOM, DATA_PACKING_MIN_RANDOM);
    checks_passed = check_entry(DATA_PACKING_MIN_RANDOM, DATA_PACKING_MAX_RANDOM) && checks_passed;
    checks_passed = check_entry(DATA_PACKING_MIN_RANDOM + 1, DATA_PACKING_MAX_RANDOM - 1) && checks_passed;
    checks_passed = check_entry(DATA_PACKING_MAX_RANDOM, DATA_PACKING_MIN_RANDOM) && checks_passed;
    checks_passed = check_entry(DATA_PACKING_MAX_RANDOM, DATA_PACKING_MAX_RANDOM) && checks_passed;

    if (!checks_passed)
    {
        return 2;
    }

    std::cout << "Validation Successful" << std::endl;

    double first_duration_ms = 0.0;
    double second_duration_ms = 0.0;
    // START BENCHMARKING
    {
        std::cout << "Size of S: " << sizeof(S) << " bytes" << std::endl;

        optkit::utils::BlockTimer block_timer("baseline", first_duration_ms);
        // OPTKIT_CPU_EVENTS("solution", optkit::metrics::performance::cpu_metrics::topdown_l1());
        // OPTKIT_CPU_EVENTS("solution", optkit::metrics::performance::cpu_metrics::topdown_l2());
        OPTKIT_CPU_EVENTS("solution", optkit::metrics::performance::cpu_metrics::all_mpki());
        std::vector<S> arr(DATA_PACKING_N);
        init<S>(arr);
        for (int i = 0; i < BENCHMARK_ITERATIONS; i++)
        {
            solution(arr);
        }
    }
    {
        std::cout << "Size of S: " << sizeof(S_patch) << " bytes" << std::endl;

        optkit::utils::BlockTimer block_timer("fixed solution", second_duration_ms);
        // OPTKIT_CPU_EVENTS("patch solution", optkit::metrics::performance::cpu_metrics::topdown_l1());
        // OPTKIT_CPU_EVENTS("patch solution", optkit::metrics::performance::cpu_metrics::topdown_l2());
        OPTKIT_CPU_EVENTS("patch solution", optkit::metrics::performance::cpu_metrics::all_mpki());
        std::vector<S_patch> arr(DATA_PACKING_N);
        init<S_patch>(arr);
        for (int i = 0; i < BENCHMARK_ITERATIONS; i++)
        {
            solution(arr);
        }
    }
    std::cout << "First Duration (ms): " << first_duration_ms << std::endl;
    std::cout << "Second Duration (ms): " << second_duration_ms << std::endl;
    std::cout << "Speedup: " << first_duration_ms / second_duration_ms << "x" << std::endl;
    return 0;
}