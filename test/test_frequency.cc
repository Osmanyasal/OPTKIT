#include <test_frequency.hh>
namespace optkit::test::freq
{

    int run()
    {
        std::cout << "Running frequency test suite...\n\n";
        
        test_set_core_frequency(2300000, 0);
        std::cout << "Sleeping for 15\n";
        sleep(1);
        test_get_core_frequencies(0);
        test_get_core_frequencies(1);
        sleep(10);

        test_set_core_frequency(1600000, 0);
        std::cout << "Sleeping for 15\n";
        sleep(1);
        test_get_core_frequencies(0);
        test_get_core_frequencies(1);
        sleep(10);
 
        test_set_core_frequency(2200000, 0);
        std::cout << "Sleeping for 15\n";
        sleep(1);
        test_get_core_frequencies(0);
        test_get_core_frequencies(1);
        sleep(10); 

        std::cout << "\nFrequency test suite completed.\n";
        return 0;
    }

    void test_set_core_frequency(long freq, short socket)
    {
        CPUFrequency::set_core_frequency(freq, socket);
        sleep(1);

        for (long _freq : CPUFrequency::get_core_frequencies(socket))
            if (freq - freq > THREASHOLD_FREQ)  // bigger than 1MHz
            {
                OPTKIT_CORE_ERROR("test_set_core_frequency[FAILED]: freq={} | current_freq={}", freq, _freq);
                exit(1);
            }

        OPTKIT_CORE_INFO("test_set_core_frequency[SUCCESS]");
    }

    void test_get_core_frequency(short cpu)
    {
        // Test get_core_frequency methods
        long freq = CPUFrequency::get_core_frequency(cpu);
        std::cout << "Core(" << cpu << ") Frequency: " << freq << " Hz\n";
    }

    void test_get_core_frequencies(short socket)
    {
        // Test get_core_frequency methods
        std::vector<long> freq = CPUFrequency::get_core_frequencies(socket);
        std::cout << "Socket-" << socket << "\n";
        for (size_t i = 0; i < freq.size(); i++)
        {
            std::cout << freq[i] << "\n";
        }
    }

    void test_get_uncore_frequency()
    {
        // Test get_uncore_frequency method
        long uncoreFreq = CPUFrequency::get_uncore_frequency(0);
        std::cout << "Uncore Frequency: " << uncoreFreq << " Hz\n";
    }

    void test_set_uncore_frequency()
    {
        // Test set_uncore_frequency method
        CPUFrequency::set_uncore_frequency(3000000,0);
    }

}