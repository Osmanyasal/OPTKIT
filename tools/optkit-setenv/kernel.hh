#pragma once
#include "helper.hh"

struct Kernel
{
    std::string irqbalance;           // on, off
    std::string isolate_irqs;         // CPU list or range
    std::string isolate_cpus;         // CPU list or range
    std::string mitigations;          // on, off
    std::string clocksource;          // tsc, hpet, acpi_pm
    int64_t sched_min_granularity_ms; // in milliseconds
    int64_t ulimit_n;                 // max number of open files
    bool disable_watchdogs;           // true or false

    std::string to_string() const
    {
        std::ostringstream oss;
        oss << "Kernel{irqbalance=" << irqbalance
            << ", isolate_irqs=" << isolate_irqs
            << ", isolate_cpus=" << isolate_cpus
            << ", mitigations=" << mitigations
            << ", clocksource=" << clocksource
            << ", sched_granularity=" << sched_min_granularity_ms << "ms"
            << ", ulimit_n=" << ulimit_n
            << ", watchdogs=" << (disable_watchdogs ? "disabled" : "enabled")
            << "}";
        return oss.str();
    }
    bool is_valid();
};

inline std::ostream &operator<<(std::ostream &os, const Kernel &kern)
{
    return os << kern.to_string();
}
