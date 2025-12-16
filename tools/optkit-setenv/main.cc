#include <iostream>
#include "sysconfig.hh"
#include "config_loader.hh"
#include "config_writer.hh"

int help(int argc, char **argv)
{
    std::cerr << "Usage: " << argv[0] << " [OPTIONS] <config.json>\n"
              << "\nOptions:\n"
              << "  --help              Display this help message\n"
              << "  --init              Create empty config template (default_config.json)\n"
              << "  --restore           Restore environment from backup (/tmp/optkit_env_backup.json)\n"
              << "  --backup            Backup current environment to /tmp/optkit_env_backup.json\n"
              << "  <config.json>       Load and display configuration from file\n"
              << "\nExamples:\n"
              << "  " << argv[0] << " --init\n"
              << "  " << argv[0] << " --restore\n"
              << "  " << argv[0] << " my_config.json\n"
              << std::endl;
    return 1;
}
int main(int argc, char **argv)
{
    OPTKIT_INIT(false);

    if (argc == 2 && std::string(argv[1]) == "--backup")
    {
        EXEC_IF_ROOT_RETURN(1);
        SysConfig &config = SysConfig::instance();

        config.load_current_settings(getpid());
        save_system_config(config, "/tmp/optkit_env_backup.json");
        OPTKIT_INFO("✓ Created backup of current environment at /tmp/optkit_env_backup.json");
    }
    else if (argc == 2 && std::string(argv[1]) == "--init")
    {
        EXEC_IF_ROOT_RETURN(1);
        SysConfig &config = SysConfig::instance();
        config.load_current_settings(getpid());
        save_system_config(config, "current_config.json");
        save_system_config(config.possible_values(), "possible_config.txt");
        OPTKIT_INFO("✓ Created current_config.json (current system state)");
        OPTKIT_INFO("✓ Created possible_config.txt (possible values reference)");
    }
    else if (argc == 2 && std::string(argv[1]) == "--restore")
    {
        EXEC_IF_ROOT_RETURN(1);
        SysConfig &config = load_system_config("/tmp/optkit_env_backup.json");

        if (config.is_valid())
        {
            if (config.apply(getpid()))
            {
                OPTKIT_INFO("✓ Restored environment from /tmp/optkit_env_backup.json");
            }
            else
            {
                OPTKIT_ERROR("✗ Failed to apply some settings during restore");
                return 1;
            }
        }
        else
        {
            OPTKIT_ERROR("✗ Backup configuration is invalid");
            return 1;
        }
    }
    else if (argc == 2 && std::string(argv[1]).find(".json") != std::string::npos)
    {
        EXEC_IF_ROOT_RETURN(1);
        SysConfig &config = load_system_config(argv[1]);
        CGroup::instance().create_cgroup();
        CGroup::instance().add_process(getpid());

        OPTKIT_INFO("Loaded configuration from " + std::string(argv[1]) + ":");
        if (config.is_valid())
        {
            if (config.apply(getpid()))
                std::cout << "\n✓ Configuration applied successfully\n";
            else
            {
                std::cerr << "\n✗ Failed to apply some settings\n";
                return 1;
            }
        }
        else
        {
            OPTKIT_ERROR("✗ Configuration is invalid");
            return 1;
        }

        // CGroup::instance().destroy_cgroup();
    }
    else
    {
        return help(argc, argv);
    }

    return 0;
}