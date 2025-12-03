#pragma once

#include <string>

struct KissatResult {
    int exit_code = -1;
    long long runtime_ms = 0;
    std::string output_path;
};

class KissatRunner {
public:
    // Convert Windows-style paths (e.g., C:\\tmp\\file.cnf) to WSL paths (/mnt/c/tmp/file.cnf).
    static std::string windowsToWslPath(const std::string& windows_path);

    // Run Kissat through WSL and return runtime information.
    KissatResult run(const std::string& input_path, const std::string& output_path) const;
};

