---@diagnostic disable: undefined-global, lowercase-global
---@diagnostic disable: undefined-field

-- custom actions also should be registered here.
local allowed_actions = {
    clean = true,
    install = true,
    remove = true,
    generate_doc = true,
    remove_doc = true,
    gmake= true,
    gmakelegacy= true,
    codelite=true,
}

function system_checks()
    print("Current premake action: " .. tostring(_ACTION))
    print("Current platform: " .. os.target())
    -- Check if the platform is Linux
    if os.target() ~= "linux" then
        print("❌ This script is only supported on Linux platforms.")
        os.exit(1) -- Exit with a non-zero status to terminate the script
    end
    if not _ACTION or not allowed_actions[_ACTION] then
        print("❌ Invalid or undefined action. Please use a valid premake action.")        
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
        spdlog_compile = spdlog_compile .. '    cd ' .. LIB_SPD_PATH .. ' && ./compile.sh;\n'
        spdlog_compile = spdlog_compile .. 'fi &&\n'
        spdlog_compile = spdlog_compile .. 'echo "[✅ COMPILE SPDLOG]" || echo "[❌ COMPILE SPDLOG ERROR]"'
        os.execute(spdlog_compile)


        -- LIBPFM compilation
        local libpfm_compile = ""
        libpfm_compile = libpfm_compile .. 'echo "[CHECK LIBPFM]";\n'
        libpfm_compile = libpfm_compile .. 'if [ ! -f "' .. LIB_PFM_PATH .. '/lib/libpfm.a" ]; then\n'
        libpfm_compile = libpfm_compile .. '    cd ' .. LIB_PFM_PATH .. ' && ./compile.sh;\n'
        libpfm_compile = libpfm_compile .. 'fi &&\n'
        libpfm_compile = libpfm_compile .. 'echo "[✅ COMPILE LIBPFM]" || echo "[❌ COMPILE LIBPFM ERROR]"'
        os.execute(libpfm_compile)

        -- Exporting events
        local export_events = ""
        export_events = export_events .. 'echo "[CHECK EVENTS]"\n'
        export_events = export_events .. 'if [ ! -f "' .. CORE_EVENTS_DIR .. '/all_set" ]; then\n'
        export_events = export_events .. '    echo "⛏️ Exporting events from libpfm4" &&\n'
        export_events = export_events .. '    mkdir -p ' .. CORE_EVENTS_DIR .. ' &&\n'
        export_events = export_events .. '    cd ' .. UTILS_DIR .. ' &&\n'
        export_events = export_events .. '    python3 pmu_parser.py $(find ../../' .. LIB_PFM_PATH .. '/lib/events -type f \\( -name "intel*.h" -or -name "amd*.h" -or -name "arm*.h" -or -name "power*.h" \\) -exec echo "{}" \\;) &&\n'
        export_events = export_events .. '    touch ../../' .. CORE_EVENTS_DIR .. '/all_set\n'
        export_events = export_events .. 'fi && echo "[✅ CHECK EVENTS]" || echo "[❌ CHECK EVENTS ERROR]"'
        os.execute(export_events)

    end
end

function dynamic_lib_exists(libname)
    local pipe = io.popen("ldconfig -p 2>/dev/null | grep lib" .. libname .. ".so")
    if not pipe then return false end

    local result = pipe:read("*a")
    pipe:close()

    return result ~= nil and result ~= ""
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


-- Use global variables in the prebuildcommands

LIB_MSR_SAFE_PATH = './lib/msr-safe'
LIB_SPD_PATH = './lib/spdlog'
LIB_PFM_PATH = './lib/libpfm4'
CORE_EVENTS_DIR = './src/core/pmu/cpu/perf/events'
UTILS_DIR = './src/utils'
LIB_GOOGLETEST_PATH = "./lib/googletest"
WHOAMI = io.popen("whoami"):read("*a"):gsub("\n", "")

OPTKIT_APP = "optkit_app"
OPTKIT_LIB_DYNAMIC = "optkit_dynamic"
OPTKIT_LIB_STATIC = "optkit_static"
OPTKIT_TEST = "optkit_test"
