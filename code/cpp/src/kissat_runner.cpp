#include "kissat_runner.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#ifndef _WIN32
#include <signal.h>
#include <sys/wait.h>
#endif

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

    int effective_timeout = timeout_seconds;
    if (effective_timeout <= 0) {
        effective_timeout = 60;  // default: 60 seconds
    }

    std::string kissat = kissat_path_.empty() ? "kissat" : kissat_path_;
    std::string command;

#ifdef _WIN32
    std::string wsl_input = windowsToWslPath(input_path);
    std::string wsl_output = windowsToWslPath(output_path);

    command = "wsl \"" + kissat + "\" ";
    command += "--time=" + std::to_string(effective_timeout) + " ";
    command += "\"" + wsl_input + "\" > \"" + wsl_output + "\"";
#else
    command = "\"" + kissat + "\" ";
    command += "--time=" + std::to_string(effective_timeout) + " ";
    command += "\"" + input_path + "\" > \"" + output_path + "\"";
#endif

    auto start = std::chrono::steady_clock::now();
    int code = std::system(command.c_str());
    auto end = std::chrono::steady_clock::now();

    result.runtime_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Normalize system() return code:
    // - On POSIX, system() returns a wait status (exit_code << 8).
    // - On Windows, it is typically the process return code already.
    int exit_status = code;

#ifndef _WIN32
    #include <sys/wait.h>
    if (WIFEXITED(code)) {
        exit_status = WEXITSTATUS(code);
    } else if (WIFSIGNALED(code)) {
        exit_status = 128 + WTERMSIG(code);
    }
#endif

    result.exit_code = exit_status;

    // Kissat uses:
    //   10 = SAT
    //   20 = UNSAT
    //   0  = sometimes used as "ok" depending on build/wrapper
    if (exit_status != 0 && exit_status != 10 && exit_status != 20) {
        throw std::runtime_error(
            "Kissat command failed (exit status " + std::to_string(exit_status) +
            ", raw system()=" + std::to_string(code) + "): " + command);
    }

    // Determine result status from the DIMACS result line.
    // Kissat prints one of: "s SATISFIABLE", "s UNSATISFIABLE", or "s UNKNOWN".
    result.timed_out = false;
    {
        std::ifstream in(output_path);
        if (in) {
            std::string line;
            while (std::getline(in, line)) {
                if (line.rfind("s ", 0) == 0) { // starts with "s "
                    if (line.find("UNKNOWN") != std::string::npos) {
                        result.timed_out = true; // treat UNKNOWN as censored/timeout
                    }
                    break;
                }
            }
        }
    }

    return result;
}
