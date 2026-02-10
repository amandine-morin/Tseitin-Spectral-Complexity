#include "kissat_runner.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#ifndef _WIN32
#include <signal.h>
#include <sys/wait.h>
#endif

namespace {

std::string normalizeText(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return text;
}

std::string trim(const std::string& text) {
    const std::string ws = " \t\r\n";
    const std::size_t start = text.find_first_not_of(ws);
    if (start == std::string::npos) {
        return "";
    }
    const std::size_t end = text.find_last_not_of(ws);
    return text.substr(start, end - start + 1);
}

std::string shellQuote(const std::string& text) {
    std::string out = "'";
    for (char ch : text) {
        if (ch == '\'') {
            out += "'\\''";
        } else {
            out.push_back(ch);
        }
    }
    out += "'";
    return out;
}

bool isTimeoutExitCode(int exit_status) {
    return exit_status == 124 || exit_status == 137 || exit_status == 143;
}

bool commandExists(const std::string& command_name) {
    const std::string probe = "command -v " + command_name + " >/dev/null 2>&1";
    return std::system(probe.c_str()) == 0;
}

int normalizeSystemExitCode(int code) {
#ifndef _WIN32
    if (code == -1) {
        return -1;
    }
    if (WIFEXITED(code)) {
        return WEXITSTATUS(code);
    }
    if (WIFSIGNALED(code)) {
        return 128 + WTERMSIG(code);
    }
#endif
    return code;
}

}  // namespace

KissatRunner::KissatRunner(SatSolver solver,
                           std::string solver_path,
                           int timeout_seconds)
    : solver_(solver),
      solver_path_(std::move(solver_path)),
      timeout_seconds_(timeout_seconds > 0 ? timeout_seconds : 60) {}

std::string KissatRunner::windowsToWslPath(const std::string& windows_path) {
    if (windows_path.size() < 2 || windows_path[1] != ':') {
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

SatSolver KissatRunner::solverFromString(const std::string& solver_name) {
    const std::string normalized = normalizeText(trim(solver_name));
    if (normalized == "kissat") {
        return SatSolver::Kissat;
    }
    if (normalized == "minisat") {
        return SatSolver::Minisat;
    }
    throw std::invalid_argument("Unknown solver: " + solver_name + " (expected kissat|minisat)");
}

std::string KissatRunner::solverToString(SatSolver solver) {
    return solver == SatSolver::Kissat ? "kissat" : "minisat";
}

KissatResult KissatRunner::run(const std::string& input_path,
                               const std::string& output_path) const {
    KissatResult result;
    result.output_path = output_path;

    const std::string fallback_solver = solver_ == SatSolver::Kissat ? "kissat" : "minisat";
    const std::string solver_binary = solver_path_.empty() ? fallback_solver : solver_path_;

#ifdef _WIN32
    const std::string solver_cmd = "\"" + solver_binary + "\"";
    const std::string wsl_input = windowsToWslPath(input_path);
    const std::string wsl_output = windowsToWslPath(output_path);
#else
    const std::string solver_cmd = shellQuote(solver_binary);
    const std::string wsl_input = input_path;
    const std::string wsl_output = output_path;
#endif

    const std::string minisat_result_path = wsl_output + ".res";

    std::string solver_invocation;
    if (solver_ == SatSolver::Kissat) {
        solver_invocation = solver_cmd + " --time=" + std::to_string(timeout_seconds_) + " " +
                            shellQuote(wsl_input) + " > " + shellQuote(wsl_output) + " 2>&1";
    } else {
        solver_invocation = solver_cmd + " " + shellQuote(wsl_input) + " " +
                            shellQuote(minisat_result_path) + " > " +
                            shellQuote(wsl_output) + " 2>&1";
    }

    std::string command;
#ifdef _WIN32
    const bool timeout_available = std::system("wsl command -v timeout >/dev/null 2>&1") == 0;
    std::string wsl_payload = solver_invocation;
    if (timeout_available) {
        wsl_payload = "timeout --preserve-status -k 1s " + std::to_string(timeout_seconds_) +
                      "s " + wsl_payload;
    } else {
        std::cerr << "Warning: 'timeout' not available in WSL. Running solver without external timeout.\n";
    }
    command = "wsl sh -lc " + shellQuote(wsl_payload);
#else
    const bool timeout_available = commandExists("timeout");
    if (timeout_available) {
        command = "timeout --preserve-status -k 1s " + std::to_string(timeout_seconds_) +
                  "s " + solver_invocation;
    } else {
        std::cerr << "Warning: 'timeout' command not available. Running solver without external timeout.\n";
        command = solver_invocation;
    }
#endif

    auto start = std::chrono::steady_clock::now();
    const int raw_code = std::system(command.c_str());
    auto end = std::chrono::steady_clock::now();

    result.runtime_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    const int exit_status = normalizeSystemExitCode(raw_code);
    if (exit_status == -1) {
        throw std::runtime_error("system() failed for command: " + command);
    }

    if (isTimeoutExitCode(exit_status)) {
        result.timed_out = true;
        result.exit_code = 124;
        result.status_string = "UNKNOWN";
        return result;
    }

    result.timed_out = false;
    result.exit_code = exit_status;

    if (solver_ == SatSolver::Kissat) {
        std::ifstream in(output_path);
        if (in) {
            std::string line;
            while (std::getline(in, line)) {
                if (line.rfind("s ", 0) == 0) {
                    const std::string normalized = normalizeText(line);
                    if (normalized.find("unknown") != std::string::npos) {
                        result.timed_out = true;
                        result.exit_code = 124;
                        result.status_string = "UNKNOWN";
                        return result;
                    }
                    if (normalized.find("unsat") != std::string::npos) {
                        result.status_string = "UNSAT";
                    } else if (normalized.find("sat") != std::string::npos) {
                        result.status_string = "SAT";
                    }
                    break;
                }
            }
        }

        if (result.status_string == "OK") {
            if (exit_status == 10) {
                result.status_string = "SAT";
            } else if (exit_status == 20) {
                result.status_string = "UNSAT";
            }
        }

        if (exit_status != 0 && exit_status != 10 && exit_status != 20) {
            throw std::runtime_error(
                "Solver command failed (kissat, exit status " + std::to_string(exit_status) +
                "): " + command);
        }

        return result;
    }

    std::ifstream minisat_result(minisat_result_path);
    if (minisat_result) {
        std::string first_line;
        if (std::getline(minisat_result, first_line)) {
            const std::string normalized = normalizeText(trim(first_line));
            if (normalized == "sat" || normalized == "satisfiable") {
                result.status_string = "SAT";
            } else if (normalized == "unsat" || normalized == "unsatisfiable") {
                result.status_string = "UNSAT";
            }
        }
    }

    if (result.status_string == "OK") {
        if (exit_status == 10) {
            result.status_string = "SAT";
        } else if (exit_status == 20) {
            result.status_string = "UNSAT";
        } else if (exit_status != 0) {
            throw std::runtime_error(
                "Solver command failed (minisat, exit status " + std::to_string(exit_status) +
                "): " + command);
        }
    }

    return result;
}
