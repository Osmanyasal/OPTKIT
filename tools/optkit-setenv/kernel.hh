#pragma once
#include "module.hh"
#include "helper.hh"

struct Kernel : public Module
{
    std::string irqbalance;           // on, off
    std::string isolate_irqs;         // CPU list or range
    std::string isolate_cpus;         // CPU list or range
    std::string mitigations;          // on, off
    std::string clocksource;          // tsc, hpet, acpi_pm
    int64_t sched_min_granularity_ms; // in milliseconds
    int64_t ulimit_n;                 // max number of open files
    bool disable_watchdogs;           // true or false

    std::string to_string() const override;
    bool is_valid() const override;
    bool apply() override;
};

inline std::ostream &operator<<(std::ostream &os, const Kernel &kern)
{
    return os << kern.to_string();
}
