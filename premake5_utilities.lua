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
    if _ACTION ~= "clean" then
        print("🛠️ Generating environment config...")
        os.execute("./generate_environment_config.sh")
    end
end

function ld_has_library(libname)
    local pipe = io.popen("ldconfig -p 2>/dev/null | grep lib" .. libname .. ".so")
    if not pipe then return false end

    local result = pipe:read("*a")
    pipe:close()

    return result ~= nil and result ~= ""
end

-- Use global variables in the prebuildcommands

LIB_MSR_SAFE_PATH = './lib/msr-safe'
LIB_SPD_PATH = './lib/spdlog'
LIB_PFM_PATH = './lib/libpfm4'
CORE_EVENTS_DIR = './src/core/pmu/cpu/perf/events'
UTILS_DIR = './src/utils'
LIB_GOOGLETEST_PATH = "./lib/googletest"
LIB_X86_ADAPT_PATH = './lib/x86-adapt'
WHOAMI = io.popen("whoami"):read("*a"):gsub("\n", "")

OPTKIT_APP = "optkit_app"
OPTKIT_LIB_DYNAMIC = "optkit_dynamic"
OPTKIT_LIB_STATIC = "optkit_static"
OPTKIT_TEST = "optkit_test"
