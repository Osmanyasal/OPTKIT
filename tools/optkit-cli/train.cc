#include "utils.hh"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits.h>
#include <sstream>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static std::string executable_dir()
{
    char buf[PATH_MAX];
    ssize_t n = ::readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (n <= 0)
        return std::string();
    buf[n] = '\0';
    std::string full(buf);
    size_t s = full.find_last_of('/');
    if (s == std::string::npos)
        return std::string();
    return full.substr(0, s);
}

static bool file_exists(const std::string &path)
{
    return (::access(path.c_str(), F_OK) == 0);
}

static std::string join_path(const std::string &a, const std::string &b)
{
    if (a.empty())
        return b;
    if (!b.empty() && b[0] == '/')
        return b;
    if (a[a.size() - 1] == '/')
        return a + b;
    return a + "/" + b;
}

static std::string choose_python_exe(const std::string &exe_dir)
{
    // 1) Explicit override
    const char *override = ::getenv("OPTKIT_TRAIN_PYTHON");
    if (override && *override)
        return std::string(override);

    // 2) Colocated venv (recommended on PEP-668 systems)
    const std::string venv_py = join_path(exe_dir, ".venv-train/bin/python");
    if (file_exists(venv_py))
        return venv_py;

    // 3) Fallback to python3 in PATH
    return "python3";
}

static int run_process(const std::vector<std::string> &argv)
{
    pid_t pid = ::fork();
    if (pid == 0)
    {
        std::vector<char *> c_args;
        c_args.reserve(argv.size() + 1);
        for (const auto &a : argv)
            c_args.push_back(const_cast<char *>(a.c_str()));
        c_args.push_back(nullptr);

        if (argv.empty())
            _exit(1);

        ::execvp(argv[0].c_str(), c_args.data());
        std::cerr << "execvp(" << argv[0] << ") failed: " << std::strerror(errno) << "\n";
        _exit(1);
    }
    else if (pid < 0)
    {
        std::cerr << "fork failed: " << std::strerror(errno) << "\n";
        return 1;
    }

    int status = 0;
    if (::waitpid(pid, &status, 0) < 0)
    {
        std::cerr << "waitpid failed: " << std::strerror(errno) << "\n";
        return 1;
    }

    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    if (WIFSIGNALED(status))
        return 128 + WTERMSIG(status);
    return 1;
}

static int run_python_trainer(const std::string &python_exe,
                              const std::string &script,
                              const std::string &folder,
                              const std::vector<std::string> &extra_args)
{
    std::vector<std::string> argv;
    argv.reserve(3 + extra_args.size());
    argv.push_back(python_exe);
    argv.push_back(script);
    argv.push_back(folder);
    for (const auto &a : extra_args)
        argv.push_back(a);
    return run_process(argv);
}

static int ensure_train_venv_and_deps(const std::string &exe_dir)
{
    // If user overrides python, do not attempt venv management.
    const char *override = ::getenv("OPTKIT_TRAIN_PYTHON");
    if (override && *override)
        return 0;

    const std::string venv_dir = join_path(exe_dir, ".venv-train");
    const std::string venv_py = join_path(exe_dir, ".venv-train/bin/python");
    const std::string venv_pip = join_path(exe_dir, ".venv-train/bin/pip");
    const std::string req = join_path(exe_dir, "requirements-train.txt");

    if (!file_exists(req))
    {
        std::cerr << "Error: missing training requirements file: " << req << "\n";
        return 2;
    }

    if (!file_exists(venv_py))
    {
        std::cout << "[optkit train] Creating venv at " << venv_dir << "\n";
        // Use system python3 to create the venv.
        int rc = run_process({"python3", "-m", "venv", venv_dir});
        if (rc != 0)
        {
            std::cerr << "[optkit train] Failed to create venv (python3 -m venv). Exit code " << rc << "\n";
            return rc;
        }
    }

    // Check deps inside venv.
    int dep_rc = run_process({venv_py, "-c", "import torch, onnx, onnxruntime"});
    if (dep_rc != 0)
    {
        std::cout << "[optkit train] Installing training dependencies into venv (may take a while)...\n";
        // Best-effort: avoid version check noise.
        ::setenv("PIP_DISABLE_PIP_VERSION_CHECK", "1", 1);
        int rc = run_process({venv_pip, "install", "-r", req});
        if (rc != 0)
        {
            std::cerr << "[optkit train] Dependency install failed. Exit code " << rc << "\n";
            return rc;
        }
        // Re-check
        dep_rc = run_process({venv_py, "-c", "import torch, onnx, onnxruntime"});
        if (dep_rc != 0)
        {
            std::cerr << "[optkit train] Dependencies still not importable after install.\n";
            return dep_rc;
        }
    }

    return 0;
}

void execute_train_command(const CommandArgs &args)
{
    if (args.train_folder.empty())
    {
        std::cerr << "Error: train requires a folder argument: optkit train <folder>\n";
        return;
    }

    const std::string exe_dir = executable_dir();
    if (exe_dir.empty())
    {
        std::cerr << "Error: unable to resolve executable directory (/proc/self/exe)\n";
        return;
    }

    const std::string script = exe_dir + "/train_model.py";
    if (!file_exists(script))
    {
        std::cerr << "Error: training script not found next to executable: " << script << "\n";
        std::cerr << "Hint: ensure train_model.py exists in the same folder as the optkit binary (tools/optkit-cli/).\n";
        return;
    }

    if (::access(args.train_folder.c_str(), R_OK) != 0)
    {
        std::cerr << "Error: cannot read folder: " << args.train_folder << " (" << std::strerror(errno) << ")\n";
        return;
    }

    // Ensure venv + deps are present (unless OPTKIT_TRAIN_PYTHON is set).
    int env_rc = ensure_train_venv_and_deps(exe_dir);
    if (env_rc != 0)
        return;

    const std::string python_exe = choose_python_exe(exe_dir);
    std::cout << "[optkit train] Running: " << python_exe << " " << script << " " << args.train_folder << "\n";
    if (python_exe.find(".venv-train") != std::string::npos)
        std::cout << "[optkit train] Using venv python at " << python_exe << "\n";
    int rc = run_python_trainer(python_exe, script, args.train_folder, args.train_args);
    if (rc != 0)
        std::cerr << "[optkit train] Trainer exited with code " << rc << "\n";
}
