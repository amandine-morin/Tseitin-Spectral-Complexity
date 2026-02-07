#include "kissat_runner.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <stdexcept>

KissatRunner::KissatRunner(std::string kissat_path)
    : kissat_path_(std::move(kissat_path)) {}

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

KissatResult KissatRunner::run(const std::string& input_path,
                               const std::string& output_path,
                               int timeout_seconds) const {
    KissatResult result;
    result.output_path = output_path;

    std::string wsl_input = windowsToWslPath(input_path);
    std::string wsl_output = windowsToWslPath(output_path);

    // Use a default timeout if none is provided or if a non-positive value is given.
    int effective_timeout = timeout_seconds;
    if (effective_timeout <= 0) {
        effective_timeout = 60;  // default: 60 seconds
    }

    // Build the Kissat command with timeout.
    // Kissat supports: --time=<seconds>
    std::string command = "wsl \"" + kissat_path_ + "\" ";
    command += "--time=" + std::to_string(effective_timeout) + " ";
    command += "\"" + wsl_input + "\" > \"" + wsl_output + "\"";

    auto start = std::chrono::steady_clock::now();
    int code = std::system(command.c_str());
    auto end = std::chrono::steady_clock::now();

    result.exit_code = code;
    result.runtime_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Detect timeout by scanning Kissat's output for the word "timeout".
    // This is a simple heuristic but works well in practice.
    result.timed_out = false;
    {
        std::ifstream in(output_path);
        if (in) {
            std::string line;
            while (std::getline(in, line)) {
                if (line.find("timeout") != std::string::npos ||
                    line.find("time limit") != std::string::npos) {
                    result.timed_out = true;
                    break;
                }
            }
        }
    }

    return result;
}
