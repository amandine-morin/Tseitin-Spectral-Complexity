#include "graph.hpp"
#include "kissat_runner.hpp"
#include "tseitin_cnf.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct ExperimentConfig {
    int vertices;
    int degree;
};

std::vector<bool> buildCharges(int vertices) {
    std::vector<bool> charges(vertices, false);
    // Set a single charge to make the overall parity odd, producing an unsatisfiable instance.
    if (!charges.empty()) {
        charges[0] = true;
    }
    return charges;
}

int main() {
    std::vector<ExperimentConfig> experiments = {
        {6, 2},
        {8, 3},
        {10, 4}
    };

    std::filesystem::create_directories("out");

    std::ofstream csv("out/results.csv", std::ios::trunc);
    csv << "vertices,degree,variables,clauses,runtime_ms,exit_code\n";

    KissatRunner runner;

    for (const auto& exp : experiments) {
        try {
            Graph graph(exp.vertices, exp.degree);
            auto charges = buildCharges(exp.vertices);
            TseitinCnfBuilder builder;
            auto formula = builder.build(graph, charges);

            std::string cnf_path = "out/graph_" + std::to_string(exp.vertices) + "_" + std::to_string(exp.degree) + ".cnf";
            std::string solver_output = "out/kissat_" + std::to_string(exp.vertices) + "_" + std::to_string(exp.degree) + ".txt";

            TseitinCnfBuilder::writeDimacs(formula, cnf_path);

            KissatResult result = runner.run(cnf_path, solver_output);

            csv << exp.vertices << ',' << exp.degree << ','
                << formula.variable_count << ',' << formula.clauses.size() << ','
                << result.runtime_ms << ',' << result.exit_code << '\n';

            std::cout << "Experiment n=" << exp.vertices << " d=" << exp.degree
                      << " -> vars=" << formula.variable_count
                      << ", clauses=" << formula.clauses.size()
                      << ", runtime_ms=" << result.runtime_ms
                      << ", exit=" << result.exit_code << "\n";
        } catch (const std::exception& ex) {
            std::cerr << "Failed experiment n=" << exp.vertices << " d=" << exp.degree
                      << ": " << ex.what() << "\n";
        }
    }

    return 0;
}

