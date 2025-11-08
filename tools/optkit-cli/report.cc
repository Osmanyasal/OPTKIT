#include "utils.hh"
#include "utils/utils.hh" // use project utils for file IO (C++11-friendly)
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <cctype>

struct RunData
{
    std::string label;                     // e.g., filename stem
    double duration_ms;                    // duration in ms
    int cores_used;                        // parsed cores_used if present
    std::map<std::string, double> topdown; // metric -> %

    RunData() : duration_ms(0.0), cores_used(0) {}
};

static const char *topdownl1_keys[] = {
    "frontend_bound", "bad_speculation", "Retiring", "backend_bound", "smt_contention"};

static const char *topdownl2_keys[] = {
    "frontend_bound_bw",
    "frontend_bound_latency",
    "backend_bound_cpu",
    "backend_bound_memory",
    "retiring_microcode",
    "retiring_fastpath",
    "bad_speculation_pipeline_restarts",
    "bad_speculation_mispredicts"};

static std::string basename_no_dir(const std::string &p)
{
    size_t s = p.find_last_of('/');
    return (s == std::string::npos) ? p : p.substr(s + 1);
}

static std::string filename_stem(const std::string &name)
{
    size_t dot = name.find_last_of('.');
    return (dot == std::string::npos) ? name : name.substr(0, dot);
}

static bool find_first_number_after(const std::string &s, size_t pos, double &out)
{
    size_t i = s.find_first_of("-0123456789", pos);
    if (i == std::string::npos)
        return false;
    size_t j = i;
    while (j < s.size())
    {
        char c = s[j];
        if ((c >= '0' && c <= '9') || c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-')
            ++j;
        else
            break;
    }
    try
    {
        out = std::stod(s.substr(i, j - i));
        return true;
    }
    catch (...)
    {
        return false;
    }
}

static bool extract_scalar_by_key(const std::string &json, const std::string &key, double &out)
{
    // looks for "key": <number> or "key": "number"
    std::string pat = "\"" + key + "\"";
    size_t k = json.find(pat);
    if (k == std::string::npos)
        return false;
    k = json.find(':', k);
    if (k == std::string::npos)
        return false;
    return find_first_number_after(json, k + 1, out);
}

static bool extract_metric_value(const std::string &json, const std::string &metric_name, double &out)
{
    // find: "name": "metric_name" or "name":"metric_name" then the next "value": <number or "number">
    std::string pat1 = "\"name\":\"" + metric_name + "\"";
    std::string pat2 = "\"name\": \"" + metric_name + "\"";
    size_t nm = json.find(pat1);
    if (nm == std::string::npos)
        nm = json.find(pat2);
    if (nm == std::string::npos)
        return false;
    size_t vpos = json.find("\"value\"", nm);
    if (vpos == std::string::npos)
        return false;
    vpos = json.find(':', vpos);
    if (vpos == std::string::npos)
        return false;
    return find_first_number_after(json, vpos + 1, out);
}

static RunData parse_run(const std::string &json_path)
{
    RunData rd;
    std::string content;
    try
    {
        content = optkit::utils::read_file(json_path);
    }
    catch (...)
    {
        content.clear();
    }
    if (content.empty())
        return rd;

    double d = 0.0;
    if (extract_scalar_by_key(content, "duration", d))
        rd.duration_ms = d;
    if (extract_scalar_by_key(content, "cores_used", d))
        rd.cores_used = static_cast<int>(d);

    rd.label = filename_stem(basename_no_dir(json_path));

    for (size_t i = 0; i < sizeof(topdownl1_keys) / sizeof(topdownl1_keys[0]); ++i)
    {
        double v = 0.0;
        if (extract_metric_value(content, topdownl1_keys[i], v))
            rd.topdown[topdownl1_keys[i]] = v;
        if (extract_metric_value(content, topdownl2_keys[i], v))
            rd.topdown[topdownl2_keys[i]] = v;
    }
    return rd;
}

static std::vector<RunData> parse_runs_from_paths(const std::vector<std::string> &paths)
{
    std::vector<std::string> json_files;
    json_files.reserve(paths.size());
    for (size_t i = 0; i < paths.size(); ++i)
    {
        const std::string &p = paths[i];
        if (!p.empty() && p.size() > 5 && p.substr(p.size() - 5) == ".json")
            json_files.push_back(p);
    }

    std::vector<RunData> runs;
    runs.reserve(json_files.size());
    for (size_t i = 0; i < json_files.size(); ++i)
    {
        RunData rd = parse_run(json_files[i]);
        if (rd.duration_ms <= 0 && rd.cores_used <= 0 && rd.topdown.empty())
            continue;
        runs.push_back(rd);
    }
    return runs;
}

static void generate_exec_time_chart(const std::vector<RunData> &runs)
{
    std::ofstream dat("exec_time_per_core.dat");
    if (!dat)
    {
        std::cerr << "Error: Cannot write exec_time_per_core.dat\n";
        return;
    }
    dat << "# cores_used duration_ms label\n";
    for (size_t i = 0; i < runs.size(); ++i)
        dat << runs[i].cores_used << "\t" << runs[i].duration_ms << "\t# " << runs[i].label << "\n";
    dat.close();

    int max_cores = 0;
    std::ostringstream xtics;
    xtics << "set xtics (";
    for (size_t i = 0; i < runs.size(); ++i)
    {
        if (i > 0)
            xtics << ", ";
        xtics << "'" << runs[i].cores_used << "' " << runs[i].cores_used;
        if (runs[i].cores_used > max_cores)
            max_cores = runs[i].cores_used;
    }
    xtics << ")\n";

    std::ofstream gp("exec_time_per_core.gp");
    gp << "set terminal pngcairo size 900,500 noenhanced\n";
    gp << "set output 'exec_time_per_core.png'\n";
    gp << "set title 'Execution time per core'\n";
    gp << "set xlabel 'Cores used'\n";
    gp << "set ylabel 'Duration (ms)'\n";
    gp << "set grid\n";
    gp << xtics.str();
    gp << "set xrange [0:" << (max_cores > 0 ? (max_cores * 1.1) : 1) << "]\n";
    gp << "plot 'exec_time_per_core.dat' using 1:2 with linespoints lw 2 title 'Duration'\n";
    gp.close();

    system("gnuplot exec_time_per_core.gp");
}

static void generate_topdownl1_chart(const std::vector<RunData> &runs)
{
    // Verify at least one run has any topdown metric
    bool any_topdown_present = false;
    for (size_t i = 0; i < runs.size() && !any_topdown_present; ++i)
    {
        for (size_t k = 0; k < sizeof(topdownl1_keys) / sizeof(topdownl1_keys[0]); ++k)
        {
            if (runs[i].topdown.find(topdownl1_keys[k]) != runs[i].topdown.end())
            {
                any_topdown_present = true;
                break;
            }
        }
    }
    if (!any_topdown_present)
    {
        std::cerr << "Info: No Topdown metrics found in any run. Skipping 'topdown_blocksl1' chart.\n";
        return;
    }

    std::ofstream dat("topdown_blocksl1.dat");
    if (!dat)
    {
        std::cerr << "Error: Cannot write topdown_blocksl1.dat\n";
        return;
    }
    dat << "# label\tfrontend_bound\tbad_speculation\tRetiring\tbackend_bound\tsmt_contention\n";
    for (size_t i = 0; i < runs.size(); ++i)
    {
        dat << runs[i].label;
        for (size_t k = 0; k < sizeof(topdownl1_keys) / sizeof(topdownl1_keys[0]); ++k)
        {
            std::map<std::string, double>::const_iterator it = runs[i].topdown.find(topdownl1_keys[k]);
            double v = (it == runs[i].topdown.end() ? 0.0 : it->second);
            dat << "\t" << v;
        }
        dat << "\n";
    }
    dat.close();

    std::ofstream gp("topdown_blocksl1.gp");
    gp << "set terminal pngcairo size 1100,600 noenhanced\n";
    gp << "set output 'topdown_blocksl1.png'\n";
    gp << "set title 'Topdown metrics per execution'\n";
    gp << "set style data histograms\n";
    gp << "set style histogram rowstacked\n";
    gp << "set style fill solid border -1\n";
    gp << "set boxwidth 0.8\n";
    gp << "set key outside right\n";
    gp << "set ylabel '%'\n";
    gp << "set yrange [0:100]\n";
    gp << "set xtics rotate by -30\n";
    gp << "plot 'topdown_blocksl1.dat' using 2:xtic(1) title 'frontend_bound', "
          "'' using 3 title 'bad_speculation', "
          "'' using 4 title 'Retiring', "
          "'' using 5 title 'backend_bound', "
          "'' using 6 title 'smt_contention'\n";
    gp.close();

    system("gnuplot topdown_blocksl1.gp");
}

static void generate_topdownl2_chart(const std::vector<RunData> &runs)
{
    // Verify at least one run has any topdown metric
    bool any_topdown_present = false;
    for (size_t i = 0; i < runs.size() && !any_topdown_present; ++i)
    {
        for (size_t k = 0; k < sizeof(topdownl2_keys) / sizeof(topdownl2_keys[0]); ++k)
        {
            if (runs[i].topdown.find(topdownl2_keys[k]) != runs[i].topdown.end())
            {
                any_topdown_present = true;
                break;
            }
        }
    }
    if (!any_topdown_present)
    {
        std::cerr << "Info: No Topdown metrics found in any run. Skipping 'topdown_blocksl2' chart.\n";
        return;
    }

    std::ofstream dat("topdown_blocksl2.dat");
    if (!dat)
    {
        std::cerr << "Error: Cannot write topdown_blocksl2.dat\n";
        return;
    }
    dat << "# label\tfrontend_bound_bw\tfrontend_bound_latency\tbackend_bound_cpu\tbackend_bound_memory\tretiring_microcode\tretiring_fastpath\tbad_speculation_pipeline_restarts\tbad_speculation_mispredicts\n";
    for (size_t i = 0; i < runs.size(); ++i)
    {
        dat << runs[i].label;
        for (size_t k = 0; k < sizeof(topdownl2_keys) / sizeof(topdownl2_keys[0]); ++k)
        {
            std::map<std::string, double>::const_iterator it = runs[i].topdown.find(topdownl2_keys[k]);
            double v = (it == runs[i].topdown.end() ? 0.0 : it->second);
            dat << "\t" << v;
        }
        dat << "\n";
    }
    dat.close();

    std::ofstream gp("topdownl2_blocks.gp");
    gp << "set terminal pngcairo size 1100,600 noenhanced\n";
    gp << "set output 'topdownl2_blocks.png'\n";
    gp << "set title 'Topdown metrics per execution'\n";
    gp << "set style data histograms\n";
    gp << "set style histogram rowstacked\n";
    gp << "set style fill solid border -1\n";
    gp << "set boxwidth 0.8\n";
    gp << "set key outside right\n";
    gp << "set ylabel '%'\n";
    gp << "set yrange [0:100]\n";
    gp << "set xtics rotate by -30\n";
    gp << "plot 'topdown_blocksl2.dat' using 2:xtic(1) title 'frontend_bound_bw', "
          "'' using 3 title 'frontend_bound_latency', "
          "'' using 4 title 'backend_bound_cpu', "
          "'' using 5 title 'backend_bound_memory', "
          "'' using 6 title 'retiring_microcode', "
          "'' using 7 title 'retiring_fastpath', "
          "'' using 8 title 'bad_speculation_pipeline_restarts', "
          "'' using 9 title 'bad_speculation_mispredicts'\n";
    gp.close();

    system("gnuplot topdownl2_blocks.gp");
}

void execute_report_command(const CommandArgs &args)
{
    // Collect input paths (assume args.program is the first path, followed by program_args)
    std::vector<std::string> file_path = args.program_args;
    file_path.insert(file_path.begin(), args.program);

    if (file_path.empty())
    {
        std::cerr << "Error: No report data folder(s) specified.\n";
        return;
    }
    if (system("which gnuplot") != 0)
    {
        std::cerr << "Error: gnuplot is not installed or not found in PATH.\n";
        return;
    }

    // Parse runs
    std::vector<RunData> runs = parse_runs_from_paths(file_path);
    if (runs.empty())
    {
        std::cerr << "Error: No JSON files found under given path(s) or parsing failed.\n";
        return;
    }

    // Sort by cores_used for line chart
    std::sort(runs.begin(), runs.end(), [](const RunData &a, const RunData &b)
              { return a.cores_used < b.cores_used; });

    // Generate charts
    generate_exec_time_chart(runs);
    generate_topdownl1_chart(runs);
    generate_topdownl2_chart(runs);
}