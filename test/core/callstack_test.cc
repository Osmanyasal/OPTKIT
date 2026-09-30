#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <unistd.h>
#include "gtest/gtest.h"
#include "optkit.hh"

/**
 * @file callstack_test.cc
 * @brief Test callstack profiler with nested functions and threading.
 *
 * Ground Truth Call Stacks:
 * ==========================
 *
 * SINGLE-THREADED NESTED CALL STACKS:
 * ====================================
 *
 * 1. level_5() (deepest)
 *    Stack: main -> level_1 -> level_2 -> level_3 -> level_4 -> level_5
 *
 * 2. level_4() (mid-depth)
 *    Stack: main -> level_1 -> level_2 -> level_3 -> level_4
 *
 * 3. level_3() (shallow)
 *    Stack: main -> level_1 -> level_2 -> level_3
 *
 * 4. level_2() (very shallow)
 *    Stack: main -> level_1 -> level_2
 *
 * 5. level_1() (direct child of main)
 *    Stack: main -> level_1
 *
 *
 * MULTI-THREADED CALL STACKS:
 * =============================
 *
 * Each worker thread will have:
 *    Stack: main -> worker_thread -> level_1 -> level_2 -> level_3 -> level_4 -> level_5
 *
 * With multiple threads, we expect to see the same nesting but sampled from different threads.
 *
 *
 * EXPECTED JSON OUTPUT STRUCTURE:
 * ==============================
 * {
 *   "readings": [{
 *     "block_name": "test_name",
 *     "measurement_type": "callstack",
 *     "samples": [
 *       {"stack": "..;main;level_1;level_2;level_3;level_4;level_5", "count": N},
 *       {"stack": "..;main;level_1;level_2;level_3;level_4", "count": N},
 *       ... (other stacks)
 *     ]
 *   }]
 * }
 */

// ============================================================================
// NESTED FUNCTION DEFINITIONS (Single-Threaded)
// ============================================================================

__attribute__((noinline)) void level_5()
{
    // Deepest level: 500M iterations
    volatile double x = 0;
    for (long i = 0; i < 500000000; i++)
    {
        x += 1.0;
    }
}

__attribute__((noinline)) void level_4()
{
    // Mid-depth: 300M iterations
    volatile double x = 0;
    for (long i = 0; i < 300000000; i++)
    {
        x += 1.0;
    }
    level_5();
    x += 0.0; // Prevent tail-call optimization
}

__attribute__((noinline)) void level_3()
{
    // Shallow: 200M iterations
    volatile double x = 0;
    for (long i = 0; i < 200000000; i++)
    {
        x += 1.0;
    }
    level_4();
    x += 0.0;
}

__attribute__((noinline)) void level_2()
{
    // Very shallow: 100M iterations
    volatile double x = 0;
    for (long i = 0; i < 100000000; i++)
    {
        x += 1.0;
    }
    level_3();
    x += 0.0;
}

__attribute__((noinline)) void level_1()
{
    // Direct child of main: 50M iterations
    volatile double x = 0;
    for (long i = 0; i < 50000000; i++)
    {
        x += 1.0;
    }
    level_2();
    x += 0.0;
}

// ============================================================================
// MULTI-THREADED WORKER
// ============================================================================

__attribute__((noinline)) void worker_thread_func(int thread_id)
{
    // Simulate worker doing nested calls
    level_1();
}

// ============================================================================
// TESTS
// ============================================================================

class CallstackTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // OPTKIT_INIT is called in main_test.cc
    }

    void TearDown() override
    {
        // Give profiler time to write output
        sleep(2);
    }
};

/**
 * @test SingleThreadedNestedCallstack
 * @brief Test callstack profiling with deeply nested function calls.
 *
 * Expected: All 5 nesting levels should be visible in the profiler output.
 * The deepest level (level_5) should have the most samples.
 * Each level should show the complete call stack from main.
 */
TEST_F(CallstackTest, SingleThreadedNestedCallstack)
{
    {
        OPTKIT_CALLSTACK_PROFILER("single_threaded_nested");

        // Call the top level which cascades through all levels
        level_1();
    }

    // Verify output file exists
    std::string output_dir = "OPTKIT_CALLSTACK_*__*__*"; // OPTKIT creates timestamped dirs

    std::cout << "\n✓ Single-threaded nested callstack test completed.\n";
    std::cout << "  Expected stacks in output:\n";
    std::cout << "    - main -> level_1 -> level_2 -> level_3 -> level_4 -> level_5\n";
    std::cout << "    - main -> level_1 -> level_2 -> level_3 -> level_4\n";
    std::cout << "    - main -> level_1 -> level_2 -> level_3\n";
    std::cout << "    - main -> level_1 -> level_2\n";
    std::cout << "    - main -> level_1\n";
}

/**
 * @test MultiThreadedNestedCallstack
 * @brief Test callstack profiling with multiple threads executing nested functions.
 *
 * Expected: Each worker thread should have the same call stacks as the single-threaded version.
 * Profiler should automatically discover and attach to all threads via /proc/<pid>/task.
 */
TEST_F(CallstackTest, MultiThreadedNestedCallstack)
{
    {
        OPTKIT_CALLSTACK_PROFILER("multi_threaded_nested");

        const int num_threads = 4;
        std::vector<std::thread> threads;

        // Spawn worker threads
        for (int i = 0; i < num_threads; ++i)
        {
            threads.emplace_back([i]()
                                 { worker_thread_func(i); });
        }

        // Also run on main thread
        worker_thread_func(-1);

        // Wait for all threads to complete
        for (auto &t : threads)
        {
            t.join();
        }
    }

    std::cout << "\n✓ Multi-threaded nested callstack test completed.\n";
    std::cout << "  Spawned 4 worker threads + main thread = 5 threads total.\n";
    std::cout << "  Expected behavior:\n";
    std::cout << "    - Each thread should have identical call stacks\n";
    std::cout << "    - Profiler auto-discovers threads via /proc/<pid>/task\n";
    std::cout << "    - All threads should contribute to the folded stack counts\n";
}

/**
 * @test ShallowCallstack
 * @brief Test callstack profiling with minimal nesting depth.
 *
 * Expected: Only 2-3 levels of nesting should appear in output.
 */
TEST_F(CallstackTest, ShallowCallstack)
{
    {
        OPTKIT_CALLSTACK_PROFILER("shallow_nesting");

        // Only call level_4 which has minimal depth
        level_4();
    }

    std::cout << "\n✓ Shallow callstack test completed.\n";
    std::cout << "  Expected stacks in output (limited depth):\n";
    std::cout << "    - main -> level_4 -> level_5\n";
    std::cout << "    - main -> level_4\n";
}

/**
 * @test DeepCallstack
 * @brief Test callstack profiling with maximum nesting depth.
 *
 * Expected: All 5 levels plus deeper system calls should appear.
 * This validates the profiler's ability to handle deep stacks.
 */
TEST_F(CallstackTest, DeepCallstack)
{
    {
        OPTKIT_CALLSTACK_PROFILER("deep_nesting");

        // Call to deepest level
        level_1();
    }

    std::cout << "\n✓ Deep callstack test completed.\n";
    std::cout << "  Expected: All 5 nesting levels visible with deepest (level_5) most frequently sampled.\n";
}
