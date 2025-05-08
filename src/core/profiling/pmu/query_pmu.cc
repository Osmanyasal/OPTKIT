#include "core/profiling/pmu/query_pmu.hh"

namespace optkit::core::pmu
{
    const int64_t QueryPMU::num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    bool QueryPMU::is_active = false;
    pfm_pmu_info_t QueryPMU::default_architectural_pmu;

    void QueryPMU::init()
    {
        if (pfm_initialize() == PFM_SUCCESS)
        {
            is_active = true;
            memset(&QueryPMU::default_architectural_pmu, 0, sizeof(pfm_pmu_info_t));
            OPTKIT_CORE_INFO("pfm initialized successfully!");
        }
        else
        {
            is_active = false;
            OPTKIT_CORE_ERROR("pfm initialize failed!");
        }
    }

    void QueryPMU::destroy()
    {
        pfm_terminate();
        is_active = false;
    }

    pfm_pmu_info_t QueryPMU::pmu_info(int32_t pmu_id)
    {
        if (OPT_UNLIKELY(QueryPMU::is_active == false))
            init();

        pfm_pmu_info_t pmu_info;
        memset(&pmu_info, 0, sizeof(pfm_pmu_info_t));
        pfm_get_pmu_info((pfm_pmu_t)pmu_id, &pmu_info);

        return pmu_info;
    }

    pfm_pmu_info_t QueryPMU::default_pmu_info()
    {

        if (OPT_UNLIKELY(QueryPMU::is_active == false))
            init();

        if (OPT_LIKELY(QueryPMU::default_architectural_pmu.is_dfl))
            return QueryPMU::default_architectural_pmu;

        std::vector<int32_t> pmu_ids = QueryPMU::avail_pmu_ids();
        for (auto pmu_id : pmu_ids)
        {
            pfm_pmu_info_t info = QueryPMU::pmu_info(pmu_id);
            if (info.is_dfl || info.type == PFM_PMU_TYPE_CORE)
            {
                info.is_dfl = true; // make it default!
                QueryPMU::default_architectural_pmu = info;
                return QueryPMU::default_architectural_pmu;
            }
        }
        // If not found return defaul pmu_info
        OPTKIT_CORE_WARN("Default PMU architecture is not detected!");
        pfm_pmu_info_t pmu_info;
        memset(&pmu_info, 0, sizeof(pfm_pmu_info_t));
        return pmu_info;
    }

    void QueryPMU::list_avail_events(int32_t pmu_id)
    {

        if (OPT_UNLIKELY(QueryPMU::is_active == false))
            init();

        pfm_event_info_t info;
        pfm_pmu_info_t pinfo;
        int32_t i, ret;

        memset(&info, 0, sizeof(info));
        memset(&pinfo, 0, sizeof(pinfo));

        info.size = sizeof(info);
        pinfo.size = sizeof(pinfo);

        ret = pfm_get_pmu_info((pfm_pmu_t)pmu_id, &pinfo);
        if (ret != PFM_SUCCESS)
        {
            OPTKIT_CORE_ERROR("Cannot get PMU info");
        }

        for (i = pinfo.first_event; i != -1; i = pfm_get_event_next(i))
        {
            ret = pfm_get_event_info(i, pfm_os_t::PFM_OS_NONE, &info);
            if (ret != PFM_SUCCESS)
            {
                OPTKIT_CORE_ERROR("Cannot get event info");
            }
            else
                std::cout << (pinfo.is_present ? "Active" : "Supported") << " Event: " << pinfo.name << "::" << info.name << std::endl;
        }
    }
    pfm_event_info_t QueryPMU::event_detail(int32_t pmu_id, uint32_t event_code)
    {
        if (OPT_UNLIKELY(QueryPMU::is_active == false))
            init();

        pfm_event_info_t info;
        pfm_pmu_info_t pinfo;
        int32_t i, ret;

        memset(&info, 0, sizeof(info));
        memset(&pinfo, 0, sizeof(pinfo));

        info.size = sizeof(info);
        pinfo.size = sizeof(pinfo);

        ret = pfm_get_pmu_info((pfm_pmu_t)pmu_id, &pinfo);
        if (ret != PFM_SUCCESS)
        {
            OPTKIT_CORE_ERROR("Cannot get PMU info");
        }

        for (i = pinfo.first_event; i != -1; i = pfm_get_event_next(i))
        {
            ret = pfm_get_event_info(i, pfm_os_t::PFM_OS_NONE, &info);
            if (ret != PFM_SUCCESS)
            {
                OPTKIT_CORE_ERROR("Cannot get event info");
            }
            else
            {
                if (info.code != event_code)
                    continue;
                return info;
            }
        }
        return info;
    }

    void QueryPMU::list_avail_pmus()
    {
        if (OPT_UNLIKELY(QueryPMU::is_active == false))
            init();

        std::cout << "[INFO] Listing Available PMUs...." << std::endl;
        int32_t i = 0;
        pfm_for_all_pmus(i)
        {
            pfm_pmu_info_t pmu_info = QueryPMU::pmu_info(i);
            if (pmu_info.is_present)
                std::cout << pmu_info << std::endl;
        }
    }

    std::vector<int32_t> QueryPMU::avail_pmu_ids()
    {
        if (OPT_UNLIKELY(QueryPMU::is_active == false))
            init();

        std::vector<int32_t> avail_pmu_ids;

        int32_t i = 0;
        pfm_for_all_pmus(i)
        {
            if (QueryPMU::pmu_info(i).is_present)
                avail_pmu_ids.push_back(i);
        }

        return avail_pmu_ids;
    }

    std::ostream &operator<<(std::ostream &out, const pfm_event_info_t &event_info)
    {
        if (OPT_UNLIKELY(event_info.idx == 0))
            return out;

        // Calculate the number of spaces needed for "Information" label and vertical bars
        int32_t _10_plus_event_info_desc_size = 10 + std::string(event_info.desc).size();
        int32_t horizontalLineWidth = 16 + _10_plus_event_info_desc_size + 4; // Width of the horizontal line 4 for (| and blank) x 2
        int32_t informationWidth = horizontalLineWidth - 2;                   // 2 is for the two vertical bars
        int32_t paddingWidth = (informationWidth - 22) / 2;                   // 16 is for the "PMU Event Information" label

        // Output the top horizontal line
        out << std::string(horizontalLineWidth, '-') << "\n";

        out << "| ";

        // Print padding before "Information" label
        out << std::setw(paddingWidth) << " ";
        out << "PMU Event Information";

        // Print padding after "Information" label
        out << std::setw(paddingWidth) << " ";

        out << "|\n";

        // Output the middle horizontal line
        out << std::string(horizontalLineWidth, '-') << "\n";

        out << "| " << std::setw(16) << "name:" << std::setw(_10_plus_event_info_desc_size) << event_info.name << " |\n";
        out << "| " << std::setw(16) << "desc:" << std::setw(_10_plus_event_info_desc_size) << event_info.desc << " |\n";
        out << "| " << std::setw(16) << "size:" << std::setw(_10_plus_event_info_desc_size) << event_info.size << " |\n";
        out << "| " << std::setw(16) << "code:" << std::setw(_10_plus_event_info_desc_size) << event_info.code << " |\n";
        // Print the rest of the members with appropriate setw values
        out << "| " << std::setw(16) << "pmu:" << std::setw(_10_plus_event_info_desc_size - 6) << optkit::core::pmu::pmu_names[(std::size_t)event_info.pmu] << std::setw(6) << "(" + std::to_string(event_info.pmu) + ")"
            << " |\n";
        out << "| " << std::setw(16) << "dtype:" << std::setw(_10_plus_event_info_desc_size - 6) << optkit::core::pmu::pmu_types[(std::size_t)event_info.dtype] << std::setw(6) << "(" + std::to_string(event_info.pmu) + ")"
            << " |\n";

        out << "| " << std::setw(16) << "idx:" << std::setw(_10_plus_event_info_desc_size) << event_info.idx << " |\n";
        out << "| " << std::setw(16) << "nattrs:" << std::setw(_10_plus_event_info_desc_size) << event_info.nattrs << " |\n";
        out << "| " << std::setw(16) << "reserved:" << std::setw(_10_plus_event_info_desc_size) << event_info.reserved << " |\n";
        out << "| " << std::setw(16) << "is_precise:" << std::setw(_10_plus_event_info_desc_size) << event_info.is_precise << " |\n";
        out << "| " << std::setw(16) << "is_speculative:" << std::setw(_10_plus_event_info_desc_size) << event_info.is_speculative << " |\n";
        out << "| " << std::setw(16) << "support_hw_smpl:" << std::setw(_10_plus_event_info_desc_size) << event_info.support_hw_smpl << " |\n";
        out << "| " << std::setw(16) << "reserved_bits:" << std::setw(_10_plus_event_info_desc_size) << event_info.reserved_bits << " |\n";
        out << std::string(horizontalLineWidth, '-') << "\n";

        return out;
    }

    std::ostream &operator<<(std::ostream &out, const pfm_pmu_info_t &pmu_info)
    {

        // Calculate the number of spaces needed for "Information" label and vertical bars
        int32_t horizontalLineWidth = 16 + 30 + 4;          // Width of the horizontal line 4 for (| and blank) x 2
        int32_t informationWidth = horizontalLineWidth - 2; // 2 is for the two vertical bars
        int32_t paddingWidth = (informationWidth - 16) / 2; // 16 is for the "PMU Information" label

        // Output the top horizontal line
        out << std::string(horizontalLineWidth, '-') << "\n";

        out << "| ";

        // Print padding before "Information" label
        out << std::setw(paddingWidth) << " ";
        out << "PMU Information";

        // Print padding after "Information" label
        out << std::setw(paddingWidth) << " ";

        out << "|\n";

        // Output the middle horizontal line
        out << std::string(horizontalLineWidth, '-') << "\n";

        out << "| " << std::setw(16) << "name:" << std::setw(30) << pmu_info.name << " |\n";
        out << "| " << std::setw(16) << "desc:" << std::setw(30) << pmu_info.desc << " |\n";
        out << "| " << std::setw(16) << "size:" << std::setw(30) << pmu_info.size << " |\n";
        // Print the rest of the members with appropriate setw values
        out << "| " << std::setw(16) << "pmu:" << std::setw(24) << optkit::core::pmu::pmu_names[(std::size_t)pmu_info.pmu] << std::setw(6) << "(" + std::to_string(pmu_info.pmu) + ")"
            << " |\n";
        out << "| " << std::setw(16) << "type:" << std::setw(24) << optkit::core::pmu::pmu_types[(std::size_t)pmu_info.type] << std::setw(6) << "(" + std::to_string(pmu_info.pmu) + ")"
            << " |\n";
        out << "| " << std::setw(16) << "nevents:" << std::setw(30) << pmu_info.nevents << " |\n";
        out << "| " << std::setw(16) << "first_event:" << std::setw(30) << pmu_info.first_event << " |\n";
        out << "| " << std::setw(16) << "max_encoding:" << std::setw(30) << pmu_info.max_encoding << " |\n";
        out << "| " << std::setw(16) << "num_cntrs:" << std::setw(30) << pmu_info.num_cntrs << " |\n";
        out << "| " << std::setw(16) << "num_fixed_cntrs:" << std::setw(30) << pmu_info.num_fixed_cntrs << " |\n";
        out << "| " << std::setw(16) << "is_present:" << std::setw(30) << pmu_info.is_present << " |\n";
        out << "| " << std::setw(16) << "is_dfl:" << std::setw(30) << pmu_info.is_dfl << " |\n";
        out << "| " << std::setw(16) << "reserved_bits:" << std::setw(30) << pmu_info.reserved_bits << " |\n";
        out << std::string(horizontalLineWidth, '-') << "\n";

        return out;
    }

} // namespace optkit::core::pmu