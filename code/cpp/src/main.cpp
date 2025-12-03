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

// -------------------------------------------------------------
// Build random charges with enforced odd parity (UNSAT instance)
// -------------------------------------------------------------
std::vector<bool> buildRandomCharges(int vertices, std::mt19937& rng) {
    std::vector<bool> charges(vertices, false);
    std::bernoulli_distribution dist(0.5);

    for (int i = 0; i < vertices; ++i) {
        charges[i] = dist(rng);
    }

    int sum = std::accumulate(charges.begin(), charges.end(), 0);
    if (sum % 2 == 0 && vertices > 0) {
        charges[0] = !charges[0];
    }

    return charges;
}

// -------------------------------------------------------------
// MAIN: Heavy-mode experimental sweep
// -------------------------------------------------------------
int main() {
    // HEAVY GRID — scales well with Kissat
    std::vector<int> vertex_sizes = {40, 80, 160, 320, 640, 1000};
    std::vector<int> degrees = {3, 4, 5};
    const int trials_per_config = 5;

    // Work directory
    std::filesystem::path work_dir = std::filesystem::path("..") / ".." / "work_heavy";
    std::filesystem::create_directories(work_dir);

    // CSV log file
    std::filesystem::path csv_path = work_dir / "results_kissat_heavy.csv";
    std::ofstream csv(csv_path, std::ios::trunc);
    if (!csv.is_open()) {
        std::cerr << "Failed to open CSV log at: " << csv_path << '\n';
        return 1;
    }
    csv << "n,d,trial,vars,clauses,runtime_ms,exit_code\n";

    KissatRunner runner;
    std::mt19937 rng(42);  // reproducible RNG

    // --------------------------------------
    // Warm-up Kissat ONCE to remove 4s delay
    // --------------------------------------
    try {
        Graph warm_g(20, 3);
        auto warm_charges = buildRandomCharges(20, rng);
        TseitinCnfBuilder warm_builder;
        auto warm_formula = warm_builder.build(warm_g, warm_charges);

        std::filesystem::path warm_cnf = work_dir / "warmup.cnf";
        std::filesystem::path warm_out = work_dir / "warmup.txt";

        TseitinCnfBuilder::writeDimacs(warm_formula, warm_cnf.string());
        runner.run(warm_cnf.string(), warm_out.string());

        std::cout << "[Warmup completed]\n";
    }
    catch (...) {
        std::cout << "[Warmup failed, continuing anyway]\n";
    }

    // --------------------------------------
    // REAL HEAVY EXPERIMENT LOOP
    // --------------------------------------
    for (int vertices : vertex_sizes) {
        for (int degree : degrees) {
            for (int trial = 1; trial <= trials_per_config; ++trial) {
                try {
                    Graph graph(vertices, degree);
                    auto charges = buildRandomCharges(vertices, rng);

                    TseitinCnfBuilder builder;
                    auto formula = builder.build(graph, charges);

                    std::filesystem::path cnf_path =
                        work_dir / ("graph_" + std::to_string(vertices) + "_" +
                                    std::to_string(degree) + "_t" +
                                    std::to_string(trial) + ".cnf");

                    std::filesystem::path solver_output =
                        work_dir / ("kissat_" + std::to_string(vertices) + "_" +
                                    std::to_string(degree) + "_t" +
                                    std::to_string(trial) + ".txt");

                    TseitinCnfBuilder::writeDimacs(formula, cnf_path.string());

                    auto t0 = std::chrono::steady_clock::now();
                    KissatResult result = runner.run(cnf_path.string(), solver_output.string());
                    auto t1 = std::chrono::steady_clock::now();

                    long long runtime_ms =
                        std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

                    if (result.runtime_ms > 0)
                        runtime_ms = result.runtime_ms;

                    // Write CSV
                    csv << vertices << ',' << degree << ',' << trial << ','
                        << formula.variable_count << ',' << formula.clauses.size() << ','
                        << runtime_ms << ',' << result.exit_code << '\n';

                    // Console log
                    std::cout << "HEAVY n=" << vertices
                              << " d=" << degree
                              << " trial=" << trial
                              << " vars=" << formula.variable_count
                              << " clauses=" << formula.clauses.size()
                              << " runtime_ms=" << runtime_ms
                              << " exit=" << result.exit_code << "\n";

                }
                catch (const std::exception& e) {
                    std::cerr << "[ERROR] n=" << vertices
                              << " d=" << degree
                              << " trial=" << trial
                              << " : " << e.what() << "\n";
                }
            }
        }
    }

    std::cout << "\n[HEAVY RUN COMPLETED]\n";
    std::cout << "Results written to: " << csv_path << "\n";

    return 0;
}
