---@diagnostic disable: undefined-global, lowercase-global
---@diagnostic disable: undefined-field

function define_custom_actions()
    local action = _ACTION
    if action ~= "clean" then
        print("🛠️ Generating environment config...")
        os.execute("./generate_environment_config.sh")
    end

    local actions = {
        clean = "clean",          -- clean the optkit build.
        install = "install",      -- build & install libs
        remove = "remove",        -- remove installed libs from the system
        gen_doc = "gen-doc",      -- generate documentaiton
        remove_doc = "remove-doc" -- delete documentaiton
    }

    -- Tasks for clean, install, and generate docs
    newaction {
        trigger = actions.clean,
        description = "clean bin and build directory",
        execute = function()
            print("[REMOVE]: ./bin")
            os.rmdir("./bin")

            print("[REMOVE]: ./build")
            os.rmdir("./build")

            print("[REMOVE]: ./Makefile")
            os.remove("./Makefile")

            print("[REMOVE]: ./src/utils/environment_config.hh")
            os.remove("./src/utils/environment_config.hh")

            print("[REMOVE]: ./test/utils/environment_config.hh")
            os.remove("./test/utils/environment_config.hh")

            print("[REMOVE]: " .. CORE_EVENTS_DIR)
            os.rmdir(CORE_EVENTS_DIR)

            print("[REMOVE]: " .. OPTKIT_APP .. ".make")
            os.remove(OPTKIT_APP .. ".make")

            print("[REMOVE]: " .. OPTKIT_LIB_DYNAMIC .. ".make")
            os.remove(OPTKIT_LIB_DYNAMIC .. ".make")

            print("[REMOVE]: " .. OPTKIT_LIB_STATIC .. ".make")
            os.remove(OPTKIT_LIB_STATIC .. ".make")

            print("[REMOVE]: " .. OPTKIT_TEST .. ".make")
            os.remove(OPTKIT_TEST .. ".make")

            print("🧹 Cleaned build directories!")
        end
    }

    newaction {
        trigger = actions.install,
        description = "Install OPTKIT headers and libs + dependencies to system directories",
        execute = function()
            -- Check if the Release directory exists
            if not os.isdir("./bin/Release") then
                print("❌ Release directory not found! Only config=release builds can be installed to the system.")
                return
            end

            print("[Installing]: SPDLOG headers")
            os.execute(
                "cd " ..
                LIB_SPD_PATH ..
                "/ && ./compile.sh && sudo cp -R ./include/spdlog /usr/local/include/ && sudo cp ./build/libspdlog.a /usr/local/lib")

            print("[Installing]: PFM4 headers")
            os.execute("cd " .. LIB_PFM_PATH .. "/ && ./compile.sh && sudo make install && sudo make install-all")

            -- print("[Installing]: MSR-SAFE")
            -- os.execute("cd ./lib/msr-safe/ && ./compile.sh && ./sudo cp -R ./include/spdlog /usr/local/include/")

            print("[Installing]: headers and libraries!")
            os.execute("sudo rm -rf /usr/local/include/optkit/ && sudo mkdir -p /usr/local/include/optkit")               -- create optkit directory for headers
            os.execute(
                "cd ./src; sudo find ./ -type f -name \"*.hh\" -exec cp --parents {} \"/usr/local/include/optkit/\" \\;") -- copy all header files by keeping the file structure as-is
            os.execute("sudo cp -R ./bin/Release/lib" .. OPTKIT_LIB_STATIC .. ".a /usr/local/lib")                        -- copy static library
            os.execute("sudo cp -R ./bin/Release/lib" .. OPTKIT_LIB_DYNAMIC .. ".so /usr/local/lib")                      -- copy dynamic library
            print("[✅ Installed]: headers and libraries!")

            -- BUILD TOOLS AND ALSO INSTALL THEM, TOOLS WILL BE USING THE STATIC-DYNAMIC LIBRARY THAT'S INSTALLED.
            print("[Installing]: utility tools!")
            os.execute("cd ./tools && ./install.sh")
            print("[✅ Installed]: utility tools!")
        end
    }

    newaction {
        trigger = actions.remove,
        description = "Remove OPTKIT from the system. (deletes all OPTKIT-cli and libraries from the system)",
        execute = function()
            print("[Removing]: OPTKIT from the system")
            os.execute("sudo rm -rf /usr/local/include/optkit") -- removes optkit headers
            os.execute("sudo rm -f /usr/local/bin/optkit*")     -- removes optkit binaries
            os.execute("sudo rm -f /usr/local/lib/liboptkit.a") -- removes optkit library
            print("[Removed 🧹]: OPTKIT from the system")
        end
    }

    newaction {
        trigger = actions.gen_doc,
        description = "Generate documentation",
        execute = function()
            os.execute("doxygen ./doxyfile") -- create doxygen file
            print("📄 Documentation generated!")
        end
    }

    newaction {
        trigger = actions.remove_doc,
        description = "Remove documentation",
        execute = function()
            os.execute("rm -rf ./doc") -- create doxygen file
            print("📄 Documentation Removed!")
        end
    }
end