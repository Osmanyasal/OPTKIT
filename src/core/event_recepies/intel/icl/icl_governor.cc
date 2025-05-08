#include "core/event_recepies/intel/icl/icl_governor.hh"

namespace optkit::core::governors::intel::icl
{
    Governor::Governor(bool data_collector_mode) : BaseGovernor{data_collector_mode}, core_cycle_watcher{"core_cycles", "core_cycles", {{optkit::intel::icl::UNHALTED_CORE_CYCLES, "unhalted_core_cycles"}}, true, this->config}, interested_events{"computational_intensity", "computational_intensity", optkit::core::recepies::intel::icl::Recepies::freq_governor_events(), true, {false, true, true, 0, -1}}
    {
        Governor::current_governor = this;
        int32_t cycle_pmc_fd = core_cycle_watcher.fd_list.at(0);

        mmap(NULL, (1 + OPTKIT_BASE_GOVERNOR_MMAP_PAGES) * getpagesize(),
             PROT_READ | PROT_WRITE, MAP_SHARED, cycle_pmc_fd, 0);

        fcntl(cycle_pmc_fd, F_SETFL, O_RDWR | O_NONBLOCK | O_ASYNC);
        fcntl(cycle_pmc_fd, F_SETSIG, SIGUSR2);
        fcntl(cycle_pmc_fd, F_SETOWN, getpid());
        ioctl(cycle_pmc_fd, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
    }
}
