#pragma once

#include <string>

#include "core/energy/pdu/pdu.hh"

namespace optkit::energy::pdu
{
    class Query final
    {
    public:
        static int32_t avail_pdu_read_methods();
        static bool is_pdu_snmp_avail();
        static bool is_configured();
        static const PduTargetInfo &target_info();

    private:
        Query() {}
        ~Query() {}
    };
}