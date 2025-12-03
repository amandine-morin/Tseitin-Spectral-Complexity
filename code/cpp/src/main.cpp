#include "graph.hpp"
#include "kissat_runner.hpp"
#include "tseitin_cnf.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

std::vector<bool> buildRandomCharges(int vertices, std::mt19937& rng) {
    std::vector<bool> charges(vertices, false);
    std::bernoulli_distribution dist(0.5);

    for (int i = 0; i < vertices; ++i) {
        charges[i] = dist(rng);
    }

    // Ensure an odd number of charges to produce an unsatisfiable instance.
    int charge_count = std::accumulate(charges.begin(), charges.end(), 0);
    if (charge_count % 2 == 0 && !charges.empty()) {
        charges[0] = !charges[0];
    }

    return charges;
}

int main() {
    const std::vector<int> vertex_sizes = {6, 8, 10, 12, 16, 20};
    const std::vector<int> degrees = {2, 3, 4};
    const int trials_per_config = 3;

    std::filesystem::path work_dir = std::filesystem::path("..") / ".." / "work";
    std::filesystem::create_directories(work_dir);

    std::filesystem::path csv_path = work_dir / "results_kissat.csv";
    std::ofstream csv(csv_path, std::ios::trunc);
    if (!csv.is_open()) {
        std::cerr << "Failed to open CSV log at: " << csv_path << '\n';
        return 1;
    }
    csv << "n,d,vars,clauses,runtime_ms,exit_code\n";

    KissatRunner runner;
    std::mt19937 rng(std::random_device{}());

    for (int vertices : vertex_sizes) {
        for (int degree : degrees) {
            for (int trial = 1; trial <= trials_per_config; ++trial) {
                try {
                    Graph graph(vertices, degree);
                    auto charges = buildRandomCharges(vertices, rng);

                    TseitinCnfBuilder builder;
                    auto formula = builder.build(graph, charges);

                    std::filesystem::path cnf_path = work_dir / ("graph_" + std::to_string(vertices) + "_" + std::to_string(degree) + "_" + std::to_string(trial) + ".cnf");
                    std::filesystem::path solver_output = work_dir / ("kissat_" + std::to_string(vertices) + "_" + std::to_string(degree) + "_" + std::to_string(trial) + ".txt");

                    TseitinCnfBuilder::writeDimacs(formula, cnf_path.string());

                    auto start = std::chrono::steady_clock::now();
                    KissatResult result = runner.run(cnf_path.string(), solver_output.string());
                    auto end = std::chrono::steady_clock::now();

                    long long runtime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                    // Prefer runtime returned by KissatRunner if it is positive; otherwise, fall back to local timer.
                    if (result.runtime_ms <= 0) {
                        result.runtime_ms = runtime_ms;
                    }

                    csv << vertices << ',' << degree << ','
                        << formula.variable_count << ',' << formula.clauses.size() << ','
                        << result.runtime_ms << ',' << result.exit_code << '\n';

                    if (result.exit_code != 0) {
                        std::cerr << "Warning: Kissat exit code " << result.exit_code
                                  << " for n=" << vertices << " d=" << degree
                                  << " trial=" << trial << '\n';
                    }

                    std::cout << "Experiment n=" << vertices << " d=" << degree
                              << " trial=" << trial
                              << " -> vars=" << formula.variable_count
                              << ", clauses=" << formula.clauses.size()
                              << ", runtime_ms=" << result.runtime_ms
                              << ", exit=" << result.exit_code << "\n";
                } catch (const std::exception& ex) {
                    std::cerr << "Failed experiment n=" << vertices << " d=" << degree
                              << " trial=" << trial << ": " << ex.what() << "\n";
                }
            }
        }
    }

    return 0;
}

