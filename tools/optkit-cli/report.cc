#include "utils.hh"
#include "utils/utils.hh"              // use project utils for file IO (C++11-friendly)
#include "utils/environment_config.hh" // for CARM bandwidth macros
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>
#include <iomanip>
#include <cctype>
#include <vector>
#include <cstdlib>
#include <limits.h>
#include <unistd.h>

struct RunData
{
    std::string label;                     // e.g., filename stem
    double duration_ms;                    // duration in ms
    int cores_used;                        // parsed cores_used if present
    long long core_freq_khz;               // core frequency in kHz (from JSON)
    long long uncore_freq_khz;             // uncore frequency in kHz (from JSON)
    double energy_pkg;                     // Joules, from measurements name: "energy_pkg"
    double kilo_edp_pkg;                   // unitless (kilo edp pkg), from measurements name: "kilo_edp_pkg"
    double ai;                             // arithmetic intensity
    double gflops;                         // gigaflops
    std::map<std::string, double> topdown; // metric -> %

    RunData() : duration_ms(0.0), cores_used(-1), core_freq_khz(0), uncore_freq_khz(0), energy_pkg(0.0), kilo_edp_pkg(0.0), ai(0.0), gflops(0.0) {}
};

static const std::vector<std::string> topdownl1_keys = {
    "frontend_bound", "bad_speculation", "Retiring", "backend_bound", "smt_contention"};

static const std::vector<std::string> topdownl2_keys = {
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
    std::ifstream f(path.c_str());
    return f.good();
}

static std::string detect_optkit_root_from_exe()
{
    // tools/optkit-cli/optkit -> repo root is ../..
    std::string dir = executable_dir();
    if (dir.empty())
        return std::string();

    char resolved[PATH_MAX];
    std::string candidate = join_path(dir, "../..");
    if (::realpath(candidate.c_str(), resolved) != nullptr)
        return std::string(resolved);
    return candidate;
}

// Parse callstack JSON and generate FlameGraph SVG
static void handle_callstack_json(const std::string &json_path)
{
    std::string content;
    try
    {
        std::ifstream file(json_path);
        content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }
    catch (...)
    {
        return;
    }

    if (content.empty())
        return;

    // Extract folded stacks from JSON by finding all "stack" and "count" pairs
    std::ofstream folded_file(filename_stem(json_path) + ".folded");
    if (!folded_file)
        return;

    // Find the samples array - structure is: "readings": [ { "samples": [...] } ]
    // So we need to find the first "samples" array
    size_t samples_pos = content.find("\"samples\"");
    if (samples_pos == std::string::npos)
        return;

    // Find the opening bracket [
    size_t open_bracket = content.find('[', samples_pos);
    if (open_bracket == std::string::npos)
        return;

    // Find the closing bracket ] by counting brackets
    int bracket_depth = 0;
    size_t max_pos = open_bracket;
    for (size_t i = open_bracket; i < content.size(); ++i)
    {
        if (content[i] == '[')
            bracket_depth++;
        else if (content[i] == ']')
        {
            bracket_depth--;
            if (bracket_depth == 0)
            {
                max_pos = i;
                break;
            }
        }
    }

    size_t search_start = open_bracket + 1;

    while (search_start < max_pos)
    {
        // Find next object containing "stack" or "count"
        // Look for opening brace {
        size_t obj_start = content.find('{', search_start);
        if (obj_start == std::string::npos || obj_start >= max_pos)
            break;

        // Find closing brace }
        size_t obj_end = content.find('}', obj_start);
        if (obj_end == std::string::npos || obj_end > max_pos)
            break;

        // Extract the object content
        std::string obj_content = content.substr(obj_start, obj_end - obj_start + 1);

        // Find "stack" and "count" within this object
        size_t stack_key = obj_content.find("\"stack\"");
        size_t count_key = obj_content.find("\"count\"");

        if (stack_key == std::string::npos || count_key == std::string::npos)
        {
            search_start = obj_end + 1;
            continue;
        }

        // Extract stack value
        size_t stack_colon = obj_content.find(':', stack_key);
        size_t stack_quote = obj_content.find('"', stack_colon);
        size_t stack_close = obj_content.find('"', stack_quote + 1);

        if (stack_quote == std::string::npos || stack_close == std::string::npos)
        {
            search_start = obj_end + 1;
            continue;
        }

        std::string stack = obj_content.substr(stack_quote + 1, stack_close - stack_quote - 1);

        // Extract count value
        size_t count_colon = obj_content.find(':', count_key);
        size_t num_start = count_colon + 1;
        while (num_start < obj_content.size() && std::isspace(obj_content[num_start]))
            num_start++;

        size_t num_end = num_start;
        while (num_end < obj_content.size() && std::isdigit(obj_content[num_end]))
            num_end++;

        std::string count_str = obj_content.substr(num_start, num_end - num_start);
        if (!count_str.empty())
            folded_file << stack << " " << count_str << "\n";

        // Move past this object
        search_start = obj_end + 1;
    }
    folded_file.close();

    // Execute flamegraph.pl to generate SVG
    std::string folded_path = filename_stem(json_path) + ".folded";
    std::string svg_path = filename_stem(json_path) + ".svg";

    // Try to find flamegraph.pl under OPTKIT's lib/FlameGraph.
    // Important: do NOT rely on current working directory; aliases may run from anywhere.
    std::string fg_script;

    const std::string optkit_root = detect_optkit_root_from_exe();
    if (!optkit_root.empty())
    {
        const std::string candidate = join_path(optkit_root, "lib/FlameGraph/flamegraph.pl");
        if (file_exists(candidate))
            fg_script = candidate;
    }

    if (fg_script.empty())
    {
        // Fallbacks (best effort): relative paths from CWD, or system install location.
        const char *search_paths[] = {
            "lib/FlameGraph/flamegraph.pl",
            "../../../lib/FlameGraph/flamegraph.pl",
            "../../lib/FlameGraph/flamegraph.pl",
            "/usr/local/FlameGraph/flamegraph.pl",
            nullptr};

        for (int i = 0; search_paths[i] != nullptr; i++)
        {
            if (file_exists(search_paths[i]))
            {
                fg_script = search_paths[i];
                break;
            }
        }
    }

    if (!fg_script.empty())
    {
        std::string cmd = "perl " + fg_script + " " + folded_path + " > " + svg_path + " 2>/dev/null";
        if (run_system_checked(cmd, "FlameGraph (perl)"))
        {
            std::cout << "Generated: " << svg_path << "\n";
        }
    }
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
    out = 0.0;
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
    {
        rd.cores_used = static_cast<int>(d);
        if (rd.cores_used == 0)
            rd.cores_used = -1; // Assume it is executed by any available cores.
    }

    // parse core/uncore frequencies (kHz)
    if (extract_scalar_by_key(content, "core_frequency_khz", d))
        rd.core_freq_khz = static_cast<long long>(d);
    if (extract_scalar_by_key(content, "uncore_frequency_khz", d))
        rd.uncore_freq_khz = static_cast<long long>(d);

    // parse energy and EDP metrics if present
    if (extract_metric_value(content, "energy-pkg", d))
        rd.energy_pkg = d;
    if (extract_metric_value(content, "kilo_edp_pkg", d))
        rd.kilo_edp_pkg = d;

    if (extract_scalar_by_key(content, "ai", d))
        rd.ai = d;
    if (extract_scalar_by_key(content, "gflops", d))
        rd.gflops = d;

    rd.label = filename_stem(basename_no_dir(json_path));

    for (size_t i = 0; i < topdownl1_keys.size(); ++i)
    {
        double v = 0.0;
        if (extract_metric_value(content, topdownl1_keys[i], v))
            rd.topdown[topdownl1_keys[i]] = v;
    }

    for (size_t i = 0; i < topdownl2_keys.size(); ++i)
    {
        double v = 0.0;
        if (extract_metric_value(content, topdownl2_keys[i], v))
            rd.topdown[topdownl2_keys[i]] = v;
    }
    return rd;
}
// Utility: make a safe file basename from a metric id (replace non-alnum with '_')
static std::string safe_name(const std::string &s)
{
    std::string r;
    r.reserve(s.size());
    for (char c : s)
    {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
            r.push_back(c);
        else
            r.push_back('_');
    }
    return r;
}

// Generate a heatmap (core vs uncore) for the given metric key across runs
static void generate_heatmap(const std::vector<RunData> &runs, const std::string &metric_key)
{
    // Aggregate by (core_khz, uncore_khz)
    struct Agg
    {
        double sum{0.0};
        int count{0};
    };

    std::map<std::pair<long long, long long>, Agg> grid; // ((core,uncore)->agg)
    std::set<long long> cores_khz;
    std::set<long long> uncores_khz;

    auto get_metric_value = [&](const RunData &rd) -> double
    {
        if (metric_key == "energy-pkg")
            return rd.energy_pkg;
        if (metric_key == "kilo_edp_pkg")
            return rd.kilo_edp_pkg;
        if (metric_key == "duration_ms")
            return rd.duration_ms;
        return 0.0;
    };

    for (const auto &rd : runs)
    {
        if (rd.core_freq_khz <= 0 || rd.uncore_freq_khz < 0)
            continue; // require core; allow uncore=0 if present
        double val = get_metric_value(rd);
        auto key = std::make_pair(rd.core_freq_khz, rd.uncore_freq_khz);
        grid[key].sum += val;
        grid[key].count += 1;
        cores_khz.insert(rd.core_freq_khz);
        uncores_khz.insert(rd.uncore_freq_khz);
    }

    if (grid.empty())
    {
        std::cerr << "Info: No data found to generate heatmap for metric '" << metric_key << "'.\n";
        return;
    }

    // Prepare filenames
    std::string base = safe_name(metric_key);
    std::string dat_name = "heatmap_" + base + ".dat";
    std::string gp_name = "heatmap_" + base + ".gp";
    std::string png_name = "heatmap_" + base + ".png";

    // Detect rectangular grid coverage (all combinations present)
    bool dense = (grid.size() == static_cast<size_t>(cores_khz.size() * uncores_khz.size()));

    // Compute minimal step (spacing) for cores & uncores to size boxes nicely.
    auto min_step = [](const std::set<long long> &vals) -> double
    {
        if (vals.size() < 2)
            return 0.1; // fallback arbitrary width if only one value
        double prev = -1.0;
        double ms = 1e12;
        for (auto v : vals)
        {
            double cur = v / 1e6; // convert to GHz
            if (prev >= 0)
                ms = std::min(ms, cur - prev);
            prev = cur;
        }
        if (ms <= 0)
            ms = 0.1; // safety fallback
        return ms;
    };
    double core_step_ghz = min_step(cores_khz);
    double uncore_step_ghz = min_step(uncores_khz);

    // Write data file: three columns core(GHz) uncore(GHz) value
    double min_val = std::numeric_limits<double>::infinity();
    double min_core_ghz = 0.0, min_uncore_ghz = 0.0;
    {
        std::ofstream dat(dat_name.c_str());
        if (!dat)
        {
            std::cerr << "Error: Cannot write " << dat_name << "\n";
            return;
        }
        dat << std::fixed << std::setprecision(6);
        dat << "# core_GHz\tuncore_GHz\t" << base << "\n";
        for (const auto &kv : grid)
        {
            long long c = kv.first.first;
            long long u = kv.first.second;
            const Agg &a = kv.second;
            double avg = (a.count > 0) ? (a.sum / a.count) : 0.0;
            double cg = (c / 1e6);
            double ug = (u / 1e6);
            dat << cg << "\t" << ug << "\t" << avg << "\n";
            if (avg < min_val)
            {
                min_val = avg;
                min_core_ghz = cg;
                min_uncore_ghz = ug;
            }
        }
    }

    // Write min point file for overlay
    std::string min_name = "heatmap_" + base + "_min.dat";
    {
        std::ofstream m(min_name.c_str());
        if (m)
        {
            m << std::fixed << std::setprecision(6) << min_core_ghz << "\t" << min_uncore_ghz << "\t" << min_val << "\n";
            m.close();
        }
    }

    // Build gnuplot script
    std::ofstream gp(gp_name.c_str());
    if (!gp)
    {
        std::cerr << "Error: Cannot write " << gp_name << "\n";
        return;
    }
    gp << "set terminal pngcairo size 1400,420 noenhanced font 'Arial,11'\n";
    gp << "set output '" << png_name << "'\n";
    gp << "set title '" << metric_key << " vs Core/Uncore Frequency'\n";
    gp << "set xlabel 'Core Frequency (GHz)'\n";
    gp << "set ylabel 'Uncore Frequency (GHz)'\n";
    gp << "set cblabel '" << base << "' offset 2,0\n";
    gp << "set key off\n";
    gp << "set grid xtics\n";
    gp << "set palette rgbformulae 22,13,-31\n"; // blue->red gradient

    // Generate explicit xtics with exact frequency values
    std::ostringstream xtics;
    xtics << "set xtics (";
    bool first_x = true;
    for (auto c : cores_khz)
    {
        if (!first_x)
            xtics << ", ";
        xtics << "'" << std::fixed << std::setprecision(3) << (c / 1e6) << "' " << (c / 1e6);
        first_x = false;
    }
    xtics << ") rotate by -45\n";
    gp << xtics.str();

    // Generate explicit ytics with exact frequency values
    std::ostringstream ytics;
    ytics << "set ytics (";
    bool first_y = true;
    for (auto u : uncores_khz)
    {
        if (!first_y)
            ytics << ", ";
        ytics << "'" << std::fixed << std::setprecision(3) << (u / 1e6) << "' " << (u / 1e6);
        first_y = false;
    }
    ytics << ")\n";
    gp << ytics.str();

    // Compute box half-dimensions (for low/high coordinates)
    // Use 95% of step or minimum visible size
    double box_half_width = (core_step_ghz > 0 ? std::max((core_step_ghz * 0.95) / 2.0, 0.045) : 0.045);
    double box_half_height = (uncores_khz.size() > 1
                                  ? std::max((uncore_step_ghz > 0 ? (uncore_step_ghz * 0.95) / 2.0 : 0.09), 0.09)
                                  : 0.09);

    // Set ranges to show all data
    if (uncores_khz.size() > 1)
    {
        double y_min = (*uncores_khz.begin()) / 1e6;
        double y_max = (*uncores_khz.rbegin()) / 1e6;
        gp << "set yrange [" << (y_min - box_half_height * 1.2) << ":" << (y_max + box_half_height * 1.2) << "]\n";
    }
    else
    {
        // Single uncore value - create narrow band
        double y_center = (*uncores_khz.begin()) / 1e6;
        gp << "set yrange [" << (y_center - box_half_height * 1.2) << ":" << (y_center + box_half_height * 1.2) << "]\n";
    }

    // Let gnuplot autoscale color range (robust across versions)
    gp << "unset cbrange\n";
    gp << "set autoscale cbfix\n";
    // Define half sizes as variables for clarity
    gp << "w = " << std::fixed << std::setprecision(6) << box_half_width << "\n";
    gp << "h = " << std::fixed << std::setprecision(6) << box_half_height << "\n";

    // Draw colored rectangles using low/high coordinates, then overlay minimum marker
    gp << "plot '" << dat_name << "' using 1:2:($1-w):($1+w):($2-h):($2+h):3 with boxxyerrorbars palette fs solid 1.0 border lc rgb 'black' notitle, \\\n";
    gp << "     '" << min_name << "' using 1:2 with points pt 7 ps 2.5 lc rgb 'white' lw 3 notitle, \\\n";
    gp << "     '" << min_name << "' using 1:2 with points pt 6 ps 2.0 lc rgb 'black' lw 2 notitle, \\\n";
    gp << "     '" << min_name << "' using 1:2:(sprintf('MIN=%.3f', $3)) with labels offset 0,1.5 tc rgb 'white' font ',12' notitle\n";
    gp.close();
    // Execute the generated gnuplot script to produce the PNG
    std::string cmd = std::string("gnuplot ") + gp_name;
    run_system_checked(cmd, "gnuplot heatmap");
}

static std::vector<RunData> parse_runs_from_paths(const std::vector<std::string> &paths)
{
    std::vector<std::string> json_files;
    json_files.reserve(paths.size());
    for (size_t i = 0; i < paths.size(); ++i)
    {
        const std::string &p = paths[i];
        if (!p.empty() && p.size() > 5 && p.substr(p.size() - 5) == ".json")
        {
            // Check if this is a callstack JSON (filename contains "callstack")
            if (p.find("callstack") != std::string::npos)
            {
                handle_callstack_json(p);
                continue; // Don't parse as RunData
            }
            json_files.push_back(p);
        }
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
    std::vector<RunData> valid_runs;
    for (size_t i = 0; i < runs.size(); ++i)
    {
        if (runs[i].duration_ms > 0)
            valid_runs.push_back(runs[i]);
    }
    if (valid_runs.empty())
    {
        std::cerr << "Info: No valid runs with execution time found. Skipping exec_time_per_core chart.\n";
        return;
    }

    std::ofstream dat("exec_time_per_core.dat");
    if (!dat)
    {
        std::cerr << "Error: Cannot write exec_time_per_core.dat\n";
        return;
    }
    dat << "# cores_used duration_ms label\n";
    for (size_t i = 0; i < valid_runs.size(); ++i)
        dat << valid_runs[i].cores_used << "\t" << valid_runs[i].duration_ms << "\t# " << valid_runs[i].label << "\n";
    dat.close();

    int max_cores = 0;
    std::ostringstream xtics;
    xtics << "set xtics (";
    for (size_t i = 0; i < valid_runs.size(); ++i)
    {
        if (i > 0)
            xtics << ", ";
        xtics << "'" << valid_runs[i].cores_used << "' " << valid_runs[i].cores_used;
        if (valid_runs[i].cores_used > max_cores)
            max_cores = valid_runs[i].cores_used;
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
    gp << "set xrange [-1:" << (max_cores > 0 ? (max_cores * 1.1) : 1) << "]\n";
    gp << "plot 'exec_time_per_core.dat' using 1:2 with linespoints lw 2 title 'Duration'\n";
    gp.close();

    run_system_checked("gnuplot exec_time_per_core.gp", "gnuplot exec_time_per_core.gp");
}

static void generate_topdownl1_chart(const std::vector<RunData> &runs)
{
    // Verify at least one run has any topdown metric
    std::vector<RunData> valid_runs;
    for (size_t i = 0; i < runs.size(); ++i)
        for (size_t k = 0; k < topdownl1_keys.size(); ++k)
            if (runs[i].topdown.find(topdownl1_keys[k]) != runs[i].topdown.end())
            {
                valid_runs.push_back(runs[i]);
                break;
            }

    if (valid_runs.empty())
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
    dat << "# cores_used\tfrontend_bound\tbad_speculation\tRetiring\tbackend_bound\tsmt_contention\n";
    for (size_t i = 0; i < valid_runs.size(); ++i)
    {
        dat << valid_runs[i].cores_used;
        for (size_t k = 0; k < topdownl1_keys.size(); ++k)
        {
            std::map<std::string, double>::const_iterator it = valid_runs[i].topdown.find(topdownl1_keys[k]);
            double v = (it == valid_runs[i].topdown.end() ? 0.0 : it->second);
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
    gp << "set xlabel 'Core Count'\n";
    gp << "set ylabel '%'\n";
    gp << "set yrange [0:100]\n";
    gp << "set xtics rotate by -30\n";
    gp << "plot 'topdown_blocksl1.dat' using 2:xtic(1) title 'frontend_bound', "
          "'' using 3 title 'bad_speculation', "
          "'' using 4 title 'Retiring', "
          "'' using 5 title 'backend_bound', "
          "'' using 6 title 'smt_contention', "
          "'' using 0:($2/2):($2 > 3 ? sprintf('%.1f%%',$2) : '') with labels tc rgb 'black' font ',9' notitle, "
          "'' using 0:($2+$3/2):($3 > 3 ? sprintf('%.1f%%',$3) : '') with labels tc rgb 'black' font ',9' notitle, "
          "'' using 0:($2+$3+$4/2):($4 > 3 ? sprintf('%.1f%%',$4) : '') with labels tc rgb 'black' font ',9' notitle, "
          "'' using 0:($2+$3+$4+$5/2):($5 > 3 ? sprintf('%.1f%%',$5) : '') with labels tc rgb 'black' font ',9' notitle, "
          "'' using 0:($2+$3+$4+$5+$6/2):($6 > 3 ? sprintf('%.1f%%',$6) : '') with labels tc rgb 'black' font ',9' notitle\n";
    gp.close();

    run_system_checked("gnuplot topdown_blocksl1.gp", "gnuplot topdown_blocksl1.gp");
}

static void generate_topdownl2_chart(const std::vector<RunData> &runs)
{
    // Verify at least one run has any topdown metric
    std::vector<RunData> valid_runs;
    for (size_t i = 0; i < runs.size(); ++i)
        for (size_t k = 0; k < topdownl2_keys.size(); ++k)
            if (runs[i].topdown.find(topdownl2_keys[k]) != runs[i].topdown.end())
            {
                valid_runs.push_back(runs[i]);
                break;
            }

    if (valid_runs.empty())
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
    dat << "# cores_used\tfrontend_bound_bw\tfrontend_bound_latency\tbackend_bound_cpu\tbackend_bound_memory\tretiring_microcode\tretiring_fastpath\tbad_speculation_pipeline_restarts\tbad_speculation_mispredicts\n";
    for (size_t i = 0; i < valid_runs.size(); ++i)
    {
        dat << valid_runs[i].cores_used;
        for (size_t k = 0; k < topdownl2_keys.size(); ++k)
        {
            std::map<std::string, double>::const_iterator it = valid_runs[i].topdown.find(topdownl2_keys[k]);
            double v = (it == valid_runs[i].topdown.end() ? 0.0 : it->second);
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
    gp << "set xlabel 'Core Count'\n";
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
          "'' using 9 title 'bad_speculation_mispredicts', "
          "'' using 0:($2/2):($2 > 3 ? sprintf('%.1f%%',$2) : '') with labels tc rgb 'black' font ',9' notitle, "
          "'' using 0:($2+$3/2):($3 > 3 ? sprintf('%.1f%%',$3) : '') with labels tc rgb 'black' font ',9' notitle, "
          "'' using 0:($2+$3+$4/2):($4 > 3 ? sprintf('%.1f%%',$4) : '') with labels tc rgb 'black' font ',9' notitle, "
          "'' using 0:($2+$3+$4+$5/2):($5 > 3 ? sprintf('%.1f%%',$5) : '') with labels tc rgb 'black' font ',9' notitle, "
          "'' using 0:($2+$3+$4+$5+$6/2):($6 > 3 ? sprintf('%.1f%%',$6) : '') with labels tc rgb 'black' font ',9' notitle, "
          "'' using 0:($2+$3+$4+$5+$6+$7/2):($7 > 3 ? sprintf('%.1f%%',$7) : '') with labels tc rgb 'black' font ',9' notitle, "
          "'' using 0:($2+$3+$4+$5+$6+$7+$8/2):($8 > 3 ? sprintf('%.1f%%',$8) : '') with labels tc rgb 'black' font ',9' notitle, "
          "'' using 0:($2+$3+$4+$5+$6+$7+$8+$9/2):($9 > 3 ? sprintf('%.1f%%',$9) : '') with labels tc rgb 'black' font ',9' notitle\n";
    gp.close();

    run_system_checked("gnuplot topdownl2_blocks.gp", "gnuplot topdownl2_blocks.gp");
}

static void generate_carm_roofline_for_isa(const std::vector<RunData> &runs,
                                           const std::string &isa_name,
                                           double l1_bw, double l2_bw, double l3_bw,
                                           double mem_bw, double compute_peak)
{
    std::string output_name = "carm_roofline_" + isa_name + ".png";
    std::string gp_name = "carm_roofline_" + isa_name + ".gp";

    // Calculate AI knees
    double ai_knee_l1 = compute_peak / l1_bw;
    double ai_knee_l2 = compute_peak / l2_bw;
    double ai_knee_l3 = compute_peak / l3_bw;
    double ai_knee_mem = compute_peak / mem_bw;

    // Write gnuplot script
    std::ofstream gp(gp_name.c_str());
    if (!gp)
    {
        std::cerr << "Error: Cannot write " << gp_name << "\n";
        return;
    }

    gp << "# Cache-Aware Roofline Model (CARM) - " << isa_name << "\n";
    gp << "set terminal pngcairo size 1000,640 enhanced font 'Arial,12'\n";
    gp << "set output '" << output_name << "'\n\n";

    gp << "set title \"CARM - " << isa_name << " ISA\"\n";
    gp << "set xlabel \"Arithmetic Intensity [FLOPs/Byte]\"\n";
    gp << "set ylabel \"Performance [GFLOPs/s]\"\n";
    gp << "set logscale xy\n";
    gp << "set grid\n\n";

    gp << "# Margins and layout\n";
    gp << "set tmargin 3\n";
    gp << "set bmargin 3\n";
    gp << "set lmargin 8\n";
    gp << "set rmargin 4\n";
    gp << "set key left top spacing 1.2 box samplen 3 width 0\n";
    gp << "set samples 400\n\n";

    gp << "# Hardware parameters from CARM measurements\n";
    gp << "compute_peak = " << compute_peak << "  # GFLOPs/s\n";
    gp << "l1_bw = " << l1_bw << "         # GB/s\n";
    gp << "l2_bw = " << l2_bw << "         # GB/s\n";
    gp << "l3_bw = " << l3_bw << "         # GB/s\n";
    gp << "mem_bw = " << mem_bw << "       # GB/s\n\n";

    gp << "# Roofline functions\n";
    gp << "roof_bw(bw, x) = (bw * x <= compute_peak) ? (bw * x) : compute_peak\n";
    gp << "compute_line(x) = compute_peak\n\n";

    gp << "# AI knees (where bandwidth roof meets compute roof)\n";
    gp << "ai_knee_l1  = " << std::fixed << std::setprecision(6) << ai_knee_l1 << "\n";
    gp << "ai_knee_l2  = " << std::fixed << std::setprecision(6) << ai_knee_l2 << "\n";
    gp << "ai_knee_l3  = " << std::fixed << std::setprecision(6) << ai_knee_l3 << "\n";
    gp << "ai_knee_mem = " << std::fixed << std::setprecision(6) << ai_knee_mem << "\n\n";

    gp << "format_knee(x) = sprintf(\"AI=%.3g\", x)\n\n";

    gp << "set xrange [1e-3:5e1]\n";
    gp << "set yrange [0.1:compute_peak*6]\n\n";

    gp << "# Plot rooflines and measured data\n";
    gp << "plot \\\n";
    gp << "    roof_bw(l1_bw, x)  w l lw 2 lc rgb \"#00BB00\" title sprintf(\"L1 (%.0f GB/s)\", l1_bw), \\\n";
    gp << "    roof_bw(l2_bw, x)  w l lw 2 lc rgb \"#0000FF\" title sprintf(\"L2 (%.0f GB/s)\", l2_bw), \\\n";
    gp << "    roof_bw(l3_bw, x)  w l lw 2 lc rgb \"#9900CC\" title sprintf(\"L3 (%.0f GB/s)\", l3_bw), \\\n";
    gp << "    roof_bw(mem_bw, x) w l lw 2 lc rgb \"#FF9900\" title sprintf(\"DRAM (%.0f GB/s)\", mem_bw), \\\n";
    gp << "    'roofline.dat' using 1:2 w p pt 7 ps 2 lw 2 lc rgb \"#FF0000\" title \"Measured\", \\\n";
    gp << "    'roofline.dat' using 1:2:(sprintf('Cores=%d, AI=%.3f, GFlops=%.2f', column(3), column(4), column(5))) with labels offset 0,-1.0 tc rgb \"#000000\" font ',9' notitle\n\n";

    gp << "# Knee markers and labels\n";
    gp << "set arrow from ai_knee_l1, graph 0 to ai_knee_l1, compute_peak nohead dt 3 lc rgb \"#00BB00\"\n";
    gp << "set label 1 format_knee(ai_knee_l1) at ai_knee_l1, compute_peak*0.45 center tc rgb \"#00BB00\"\n\n";

    gp << "set arrow from ai_knee_l2, graph 0 to ai_knee_l2, compute_peak nohead dt 3 lc rgb \"#0000FF\"\n";
    gp << "set label 2 format_knee(ai_knee_l2) at ai_knee_l2, compute_peak*0.6 center tc rgb \"#0000FF\"\n\n";

    gp << "set arrow from ai_knee_l3, graph 0 to ai_knee_l3, compute_peak nohead dt 3 lc rgb \"#9900CC\"\n";
    gp << "set label 3 format_knee(ai_knee_l3) at ai_knee_l3, compute_peak*0.75 center tc rgb \"#9900CC\"\n\n";

    gp << "set arrow from ai_knee_mem, graph 0 to ai_knee_mem, compute_peak nohead dt 3 lc rgb \"#FF9900\"\n";
    gp << "set label 4 format_knee(ai_knee_mem) at ai_knee_mem, compute_peak*0.25 center tc rgb \"#FF9900\"\n\n";

    gp << "# Replot to apply labels\n";
    gp << "replot\n";
    gp.close();

    std::string cmd = "gnuplot " + gp_name;
    run_system_checked(cmd, "CARM roofline generation");

    std::cout << "  " << isa_name << ": " << output_name << " (Peak: " << compute_peak
              << " GFlops/s, DRAM: " << mem_bw << " GB/s)\n";
}

static void generate_carm_roofline_chart(const std::vector<RunData> &runs)
{
#ifdef OPTKIT_ENV_CARM_AVX512_L1_BW || OPTKIT_ENV_CARM_AVX2_L1_BW || OPTKIT_ENV_CARM_SSE_L1_BW || OPTKIT_ENV_CARM_SCALAR_L1_BW

    // Check if we have any AI/GFlops data
    std::vector<RunData> valid_runs;
    for (size_t i = 0; i < runs.size(); ++i)
        if (runs[i].ai > 0.0 && runs[i].gflops > 0.0)
            valid_runs.push_back(runs[i]);

    if (valid_runs.empty())
    {
        std::cerr << "Info: No AI/GFlops data found in runs. Skipping CARM roofline chart.\n";
        return;
    }

    // Write shared data file with measured kernels (AI vs GFlops with metadata)
    std::ofstream dat("roofline.dat");
    if (!dat)
    {
        std::cerr << "Error: Cannot write roofline.dat\n";
        return;
    }
    dat << "# AI(FLOPs/Byte)\tGFlops/s\tCores\tAI\tGFlops\n";
    for (size_t i = 0; i < valid_runs.size(); ++i)
    {
        if (valid_runs[i].ai > 0.0 && valid_runs[i].gflops > 0.0)
        {
            dat << std::fixed << std::setprecision(6) << valid_runs[i].ai << "\t" << valid_runs[i].gflops << "\t"
                << valid_runs[i].cores_used << "\t" << valid_runs[i].ai << "\t" << valid_runs[i].gflops << "\n";
        }
    }
    dat.close();

    std::cout << "CARM Roofline charts generated:\n";

#ifdef OPTKIT_ENV_CARM_AVX512_L1_BW
    // Generate roofline for each ISA
    generate_carm_roofline_for_isa(valid_runs, "AVX512",
                                   OPTKIT_ENV_CARM_AVX512_L1_BW, OPTKIT_ENV_CARM_AVX512_L2_BW,
                                   OPTKIT_ENV_CARM_AVX512_L3_BW, OPTKIT_ENV_CARM_AVX512_DRAM_BW,
                                   OPTKIT_ENV_CARM_AVX512_FP_FMA_GFLOPS);
#endif

#ifdef OPTKIT_ENV_CARM_AVX2_L1_BW
    generate_carm_roofline_for_isa(valid_runs, "AVX2",
                                   OPTKIT_ENV_CARM_AVX2_L1_BW, OPTKIT_ENV_CARM_AVX2_L2_BW,
                                   OPTKIT_ENV_CARM_AVX2_L3_BW, OPTKIT_ENV_CARM_AVX2_DRAM_BW,
                                   OPTKIT_ENV_CARM_AVX2_FP_FMA_GFLOPS);
#endif

#ifdef OPTKIT_ENV_CARM_SSE_L1_BW
    generate_carm_roofline_for_isa(valid_runs, "SSE",
                                   OPTKIT_ENV_CARM_SSE_L1_BW, OPTKIT_ENV_CARM_SSE_L2_BW,
                                   OPTKIT_ENV_CARM_SSE_L3_BW, OPTKIT_ENV_CARM_SSE_DRAM_BW,
                                   OPTKIT_ENV_CARM_SSE_FP_FMA_GFLOPS);
#endif
#ifdef OPTKIT_ENV_CARM_SCALAR_L1_BW
    generate_carm_roofline_for_isa(valid_runs, "SCALAR",
                                   OPTKIT_ENV_CARM_SCALAR_L1_BW, OPTKIT_ENV_CARM_SCALAR_L2_BW,
                                   OPTKIT_ENV_CARM_SCALAR_L3_BW, OPTKIT_ENV_CARM_SCALAR_DRAM_BW,
                                   OPTKIT_ENV_CARM_SCALAR_FP_FMA_GFLOPS);
#endif
#endif
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
    generate_carm_roofline_chart(runs);

    // Generate heatmaps for requested metrics
    generate_heatmap(runs, "duration_ms");
    generate_heatmap(runs, "energy-pkg");
    generate_heatmap(runs, "kilo_edp_pkg");
}
