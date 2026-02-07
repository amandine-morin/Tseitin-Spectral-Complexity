#pragma once

#include <string>

struct KissatResult {
    int exit_code = -1;
    long long runtime_ms = 0;
    std::string output_path;
    bool timed_out = false;  // true if Kissat hit its internal time limit
};

class KissatRunner {
public:
    explicit KissatRunner(std::string kissat_path = "/home/dinah/kissat/build/kissat");
    // Convert Windows-style paths (e.g., C:\\tmp\\file.cnf) to WSL paths (/mnt/c/tmp/file.cnf).
    static std::string windowsToWslPath(const std::string& windows_path);

    // Run Kissat through WSL and return runtime information.
    // If timeout_seconds <= 0, a default timeout is used (e.g. 60 seconds).
    KissatResult run(const std::string& input_path,
                     const std::string& output_path,
                     int timeout_seconds = -1) const;

private:
    std::string kissat_path_;
};
