---@diagnostic disable: undefined-global, lowercase-global
---@diagnostic disable: undefined-field

-- Use global variables in the prebuildcommands

LIB_MSR_SAFE_PATH = './lib/msr-safe'
LIB_SPD_PATH = './lib/spdlog'
LIB_PFM_PATH = './lib/libpfm4'
CPU_PMU_EVENTS_DIR = './src/core/pmu/cpu/events'
UTILS_DIR = './src/utils'
LIB_GOOGLETEST_PATH = "./lib/googletest"
WHOAMI = io.popen("whoami"):read("*a"):gsub("\n", "")

OPTKIT_APP = "optkit_app"
OPTKIT_LIB_DYNAMIC = "optkit_dynamic"
OPTKIT_LIB_STATIC = "optkit_static"
OPTKIT_TEST = "optkit_test"

-- custom actions also should be registered here.
local allowed_actions = {
    clean = true,
    install = true,
    remove = true,
    generate_doc = true,
    remove_doc = true,
    gmake = true,
    gmakelegacy = true,
    codelite = true,
}

function print_help()
    print("\nAvailable premake actions:")
    print("  clean         - Clean the OptKit build.")
    print("  install       - Build & install libraries.")
    print("  remove        - Remove installed libraries from the system.")
    print("  generate_doc  - Generate documentation.")
    print("  remove_doc    - Delete documentation.")
    print("  gmake         - Generate GNU Makefiles (only supported build system).")
    print("  --help        - Show this help message.\n")
end

function print_help()
    print("\nAvailable premake actions:")
    print("  clean         - Clean the OptKit build.")
    print("  install       - Build & install libraries.")
    print("  remove        - Remove installed libraries from the system.")
    print("  generate_doc  - Generate documentation.")
    print("  remove_doc    - Delete documentation.")
    print("  gmake         - Generate GNU Makefiles (only supported build system).")
    print("  --help        - Show this help message.\n")
end

function system_checks()
    print("Current premake action: " .. tostring(_ACTION))
    print("Current platform: " .. os.target())

    -- Check if the platform is Linux
    if os.target() ~= "linux" then
        print("❌ This script is only supported on Linux platforms.")
        os.exit(1)
    end

    -- Handle --help or missing/invalid action
    if _ACTION == "--help" then
        print_help()
        os.exit(0)
    elseif not _ACTION or not allowed_actions[_ACTION] then
        print("❌ Invalid or undefined action.\n")
        print_help()
        os.exit(1)
    end
end

function system_init()
    if _ACTION == "gmake" or _ACTION == "gmakelegacy" or _ACTION == "codelite" then
        -- environment_config.hh generation for the current system
        print("🛠️ Generating environment config...")
        os.execute("./generate_environment_config.sh")

        -- SPDLOG compilation
        local spdlog_compile = ""
        spdlog_compile = spdlog_compile .. 'echo "[CHECK SPDLOG]";\n'
        spdlog_compile = spdlog_compile .. 'if [ ! -f "' .. LIB_SPD_PATH .. '/build/libspdlog.a" ]; then\n'
        spdlog_compile = spdlog_compile ..
            '    cd ' ..
            LIB_SPD_PATH ..
            ' && git checkout v1.x && mkdir -p build && cd build && cmake .. -DSPDLOG_BUILD_SHARED=OFF -DSPDLOG_BUILD_EXAMPLES=OFF -DSPDLOG_BUILD_BENCH=OFF -DSPDLOG_BUILD_TESTS=OFF && cmake --build .;\n'
        spdlog_compile = spdlog_compile .. 'fi &&\n'
        spdlog_compile = spdlog_compile .. 'echo "[✅ COMPILE SPDLOG]" || echo "[❌ COMPILE SPDLOG ERROR]"'
        os.execute(spdlog_compile)


        -- LIBPFM compilation
        local libpfm_compile = ""
        libpfm_compile = libpfm_compile .. 'echo "[CHECK LIBPFM]";\n'
        libpfm_compile = libpfm_compile .. 'if [ ! -f "' .. LIB_PFM_PATH .. '/lib/libpfm.a" ]; then\n'
        libpfm_compile = libpfm_compile .. '    cd ' .. LIB_PFM_PATH .. ' && make clean && make;\n'
        libpfm_compile = libpfm_compile .. 'fi &&\n'
        libpfm_compile = libpfm_compile .. 'echo "[✅ COMPILE LIBPFM]" || echo "[❌ COMPILE LIBPFM ERROR]"'
        os.execute(libpfm_compile)

        -- GoogleTest compilation
        local googletest_compile = ""
        googletest_compile = googletest_compile ..
            'if [ ! -f "' .. LIB_GOOGLETEST_PATH .. '/build/lib/libgtest.a" ]; then\n'
        googletest_compile = googletest_compile ..
            'cd ' ..
            LIB_GOOGLETEST_PATH .. " && git branch v1.17.0 && mkdir build && cd build && cmake .. && make -j$(nproc);\n"
        googletest_compile = googletest_compile .. 'fi && echo [✅ COMPILE SPDLOG] || echo [❌ COMPILE SPDLOG ERROR]'
        os.execute(googletest_compile)

        -- Exporting events
        local export_events = ""
        export_events = export_events .. 'echo "[CHECK EVENTS]"\n'
        export_events = export_events .. 'if [ ! -f "' .. CPU_PMU_EVENTS_DIR .. '/all_set" ]; then\n'
        export_events = export_events .. '    echo "⛏️ Exporting events from libpfm4" &&\n'
        export_events = export_events .. '    mkdir -p ' .. CPU_PMU_EVENTS_DIR .. ' &&\n'
        export_events = export_events .. '    cd ' .. UTILS_DIR .. ' &&\n'
        export_events = export_events ..
            '    python3 pmu_parser.py $(find ../../' ..
            LIB_PFM_PATH ..
            '/lib/events -type f \\( -name "intel*.h" -or -name "amd*.h" -or -name "arm*.h" -or -name "power*.h" \\) -exec echo "{}" \\;) &&\n'
        export_events = export_events .. '    touch ../../' .. CPU_PMU_EVENTS_DIR .. '/all_set\n'
        export_events = export_events .. 'fi && echo "[✅ CHECK EVENTS]" || echo "[❌ CHECK EVENTS ERROR]"'
        os.execute(export_events)
    end
end

function get_nvml_include()
    if os.isdir("/usr/local/cuda/include") then
        return "/usr/local/cuda/include"
    end
    return nil
end

function get_rocm_include()
    if os.isdir("/opt/rocm/include") then
        return "/opt/rocm/include"
    end
    return nil
end

function get_cupti_include()
    if os.isdir("/usr/local/cuda/include/") then
        return "/usr/local/cuda/include"
    end
    return nil
end

function dynamic_lib_exists(libname)
    -- First, try ldconfig (system-registered libraries)
    local pipe = io.popen("ldconfig -p 2>/dev/null | grep lib" .. libname .. ".so")
    if pipe then
        local result = pipe:read("*a")
        pipe:close()
        if result ~= nil and result ~= "" then
            return true
        end
    end

    -- If not found in ldconfig, check common ROCm/CUDA library paths
    local search_paths = {
        "/opt/rocm/lib",
        "/usr/local/cuda/lib64",
        "/usr/lib/x86_64-linux-gnu",
    }

    -- Also check LD_LIBRARY_PATH if set
    local ld_library_path = os.getenv("LD_LIBRARY_PATH")
    if ld_library_path then
        for path in string.gmatch(ld_library_path, "[^:]+") do
            table.insert(search_paths, path)
        end
    end

    -- Search for the library in these paths
    for _, path in ipairs(search_paths) do
        local lib_patterns = {
            path .. "/lib" .. libname .. ".so",
            path .. "/lib" .. libname .. ".so.*",
        }

        for _, pattern in ipairs(lib_patterns) do
            local check_cmd = "ls " .. pattern .. " 2>/dev/null"
            local check_pipe = io.popen(check_cmd)
            if check_pipe then
                local check_result = check_pipe:read("*a")
                check_pipe:close()
                if check_result ~= nil and check_result ~= "" then
                    return true
                end
            end
        end
    end

    return false
end

function static_lib_exists(libname)
    local search_paths = {
        "/usr/lib",
        "/usr/local/lib",
        "/lib",
        "/lib64",
        "/usr/lib64"
    }

    for _, path in ipairs(search_paths) do
        local full_path = path .. "/lib" .. libname .. ".a"
        local f = io.open(full_path, "r")
        if f then
            f:close()
            return true
        end
    end

    return false
end
