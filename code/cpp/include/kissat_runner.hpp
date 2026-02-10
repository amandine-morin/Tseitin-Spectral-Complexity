#pragma once

#include <string>

enum class SatSolver {
    Kissat,
    Minisat,
};

struct KissatResult {
    int exit_code = -1;
    long long runtime_ms = 0;
    std::string output_path;
    bool timed_out = false;
    std::string status_string = "OK";
};

class KissatRunner {
public:
    explicit KissatRunner(SatSolver solver = SatSolver::Kissat,
                          std::string solver_path = "",
                          int timeout_seconds = 60);

    static std::string windowsToWslPath(const std::string& windows_path);
    static SatSolver solverFromString(const std::string& solver_name);
    static std::string solverToString(SatSolver solver);

    KissatResult run(const std::string& input_path,
                     const std::string& output_path) const;

private:
    SatSolver solver_;
    std::string solver_path_;
    int timeout_seconds_;
};
