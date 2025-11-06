#include "utils.hh"
#include "utils/utils.hh" // use project utils for file IO (C++11-friendly)
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <cctype>

void execute_report_command(const CommandArgs &args)
{
    std::vector<std::string> file_path = args.program_args;
    file_path.insert(file_path.begin(), args.program);

    if (file_path.empty())
    {
        std::cerr << "Error: No report data folder(s) specified.\n";
        return;
    }
    if (system("which gnuplot") != 0) // check if gnuplot exists
    {
        std::cerr << "Error: gnuplot is not installed or not found in PATH.\n";
        return;
    }

    // Helper lambdas
    auto basename = [](const std::string &p) -> std::string
    {
        size_t s = p.find_last_of('/');
        if (s == std::string::npos)
            return p;
        return p.substr(s + 1);
    };
    auto stem = [](const std::string &name) -> std::string
    {
        size_t dot = name.find_last_of('.');
        return (dot == std::string::npos) ? name : name.substr(0, dot);
    };
    auto find_first_number_after = [](const std::string &s, size_t pos, double &out) -> bool
    {
        size_t i = s.find_first_of("-0123456789", pos);
        if (i == std::string::npos)
            return false;
        size_t j = i;
        while (j < s.size())
        {
            char c = s[j];
            if ((c >= '0' && c <= '9') || c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-')
            {
                ++j;
            }
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
    };
    auto extract_scalar_by_key = [&](const std::string &json, const std::string &key, double &out) -> bool
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
    };
    auto extract_metric_value = [&](const std::string &json, const std::string &metric_name, double &out) -> bool
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
    };

    struct RunData
    {
        std::string label;      // e.g., "1 cores" or directory basename
        double duration_ms = 0; // duration in ms
        int cores_used = 0;
        std::map<std::string, double> topdown; // metric -> %
    };

    auto parse_run = [&](const std::string &json_path) -> RunData
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
            rd.duration_ms = d; // ms per samples in provided files
        if (extract_scalar_by_key(content, "cores_used", d))
            rd.cores_used = static_cast<int>(d);
        // default label from filename
        rd.label = stem(basename(json_path));

        // Topdown metrics of interest
        const char *td_keys[] = {"frontend_bound", "bad_speculation", "Retiring", "backend_bound", "smt_contention"};
        for (auto k : td_keys)
        {
            double v = 0.0;
            if (extract_metric_value(content, k, v))
                rd.topdown[k] = v;
        }
        return rd;
    };

    // Build run data for each input (assume given paths are JSON files)
    std::vector<std::string> json_files;
    json_files.reserve(file_path.size());
    for (size_t i = 0; i < file_path.size(); ++i)
    {
        const std::string &p = file_path[i];
        if (!p.empty() && p.size() > 5 && p.substr(p.size() - 5) == ".json")
            json_files.push_back(p);
    }
    if (json_files.empty())
    {
        std::cerr << "Error: No JSON files found under given path(s).\n";
        return;
    }

    std::vector<RunData> runs;
    runs.reserve(json_files.size());
    for (const auto &jf : json_files)
    {
        RunData rd = parse_run(jf);
        if (rd.duration_ms <= 0 && rd.cores_used <= 0 && rd.topdown.empty())
            continue;
        runs.push_back(rd);
    }
    if (runs.empty())
    {
        std::cerr << "Error: Could not parse any runs from JSON files.\n";
        return;
    }

    // Sort by cores_used for line chart
    std::sort(runs.begin(), runs.end(), [](const RunData &a, const RunData &b)
              { return a.cores_used < b.cores_used; });

    // 1) Generate execution time per core line chart
    {
        std::ofstream dat("exec_time_per_core.dat");
        if (!dat)
        {
            std::cerr << "Error: Cannot write exec_time_per_core.dat\n";
            return;
        }
        dat << "# cores_used duration_ms label\n";
        for (const auto &r : runs)
            dat << r.cores_used << "\t" << r.duration_ms << "\t# " << r.label << "\n";
        dat.close();

        // Prepare xtics explicitly from cores_used and set xrange to cover all points
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

    // 2) Generate topdown stacked block chart per execution
    {
        const char *td_keys[] = {"frontend_bound", "bad_speculation", "Retiring", "backend_bound", "smt_contention"};
        std::ofstream dat("topdown_blocks.dat");
        if (!dat)
        {
            std::cerr << "Error: Cannot write topdown_blocks.dat\n";
            return;
        }
        dat << "# label\tfrontend_bound\tbad_speculation\tRetiring\tbackend_bound\tsmt_contention\n";
        for (const auto &r : runs)
        {
            dat << r.label;
            for (auto k : td_keys)
            {
                auto it = r.topdown.find(k);
                double v = (it == r.topdown.end() ? 0.0 : it->second);
                dat << "\t" << v;
            }
            dat << "\n";
        }
        dat.close();

        std::ofstream gp("topdown_blocks.gp");
        gp << "set terminal pngcairo size 1100,600 noenhanced\n";
        gp << "set output 'topdown_blocks.png'\n";
        gp << "set title 'Topdown metrics per execution'\n";
        gp << "set style data histograms\n";
        gp << "set style histogram rowstacked\n";
        gp << "set style fill solid border -1\n";
        gp << "set boxwidth 0.8\n";
        gp << "set key outside right\n";
        gp << "set ylabel '%'\n";
        gp << "set yrange [0:100]\n";
        gp << "set xtics rotate by -30\n";
        gp << "plot 'topdown_blocks.dat' using 2:xtic(1) title 'frontend_bound', "
              "'' using 3 title 'bad_speculation', "
              "'' using 4 title 'Retiring', "
              "'' using 5 title 'backend_bound', "
              "'' using 6 title 'smt_contention'\n";
        gp.close();
        system("gnuplot topdown_blocks.gp");
    }
}