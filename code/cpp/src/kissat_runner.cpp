#include "kissat_runner.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <stdexcept>

std::string KissatRunner::windowsToWslPath(const std::string& windows_path) {
    if (windows_path.size() < 2 || windows_path[1] != ':') {
        // Already looks like a Unix path.
        std::string converted = windows_path;
        std::replace(converted.begin(), converted.end(), '\\', '/');
        return converted;
    }

    char drive = static_cast<char>(std::tolower(windows_path[0]));
    std::string path_rest = windows_path.substr(2);

    std::string converted = "/mnt/";
    converted.push_back(drive);
    converted.push_back('/');
    for (char c : path_rest) {
        converted.push_back(c == '\\' ? '/' : c);
    }
    if (!converted.empty() && converted.back() == '/') {
        converted.pop_back();
    }
    return converted;
}

KissatResult KissatRunner::run(const std::string& input_path, const std::string& output_path) const {
    KissatResult result;
    result.output_path = output_path;

    std::string wsl_input = windowsToWslPath(input_path);
    std::string wsl_output = windowsToWslPath(output_path);

    std::string command = "wsl /home/dinah/kissat/build/kissat \"" + wsl_input + "\" > \"" + wsl_output + "\"";

    auto start = std::chrono::steady_clock::now();
    int code = std::system(command.c_str());
    auto end = std::chrono::steady_clock::now();

    result.exit_code = code;
    result.runtime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    return result;
}

