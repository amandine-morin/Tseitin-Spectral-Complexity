#include "graph.hpp"
#include "kissat_runner.hpp"
#include "tseitin_cnf.hpp"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

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

uint64_t fnv1aHashFile(const std::filesystem::path& path) {
    constexpr uint64_t kOffsetBasis = 14695981039346656037ULL;
    constexpr uint64_t kPrime = 1099511628211ULL;
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for hashing: " + path.string());
    }
    uint64_t hash = kOffsetBasis;
    char buffer[4096];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        std::streamsize count = file.gcount();
        for (std::streamsize i = 0; i < count; ++i) {
            hash ^= static_cast<unsigned char>(buffer[i]);
            hash *= kPrime;
        }
    }
    return hash;
}

std::string hashToHex(uint64_t hash) {
    std::ostringstream stream;
    stream << std::hex << hash;
    return stream.str();
}

struct RunOptions {
    int vertices = -1;
    int degree = -1;
    uint32_t seed = 0;
    std::filesystem::path out_dir = "out";
    std::string kissat_path = "/home/dinah/kissat/build/kissat";
    Graph::Mode graph_mode = Graph::Mode::Circulant;
};

RunOptions parseArgs(int argc, char** argv) {
    RunOptions options;
    bool seed_provided = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--n") {
            if (i + 1 >= argc) {
                throw std::invalid_argument("Missing value for --n");
            }
            options.vertices = std::stoi(argv[++i]);
        } else if (arg == "--d") {
            if (i + 1 >= argc) {
                throw std::invalid_argument("Missing value for --d");
            }
            options.degree = std::stoi(argv[++i]);
        } else if (arg == "--seed") {
            if (i + 1 >= argc) {
                throw std::invalid_argument("Missing value for --seed");
            }
            options.seed = static_cast<uint32_t>(std::stoul(argv[++i]));
            seed_provided = true;
        } else if (arg == "--outdir") {
            if (i + 1 >= argc) {
                throw std::invalid_argument("Missing value for --outdir");
            }
            options.out_dir = argv[++i];
        } else if (arg == "--kissat") {
            if (i + 1 >= argc) {
                throw std::invalid_argument("Missing value for --kissat");
            }
            options.kissat_path = argv[++i];
        } else if (arg == "--graph_mode") {
            if (i + 1 >= argc) {
                throw std::invalid_argument("Missing value for --graph_mode");
            }
            std::string mode = argv[++i];
            if (mode == "circulant") {
                options.graph_mode = Graph::Mode::Circulant;
            } else if (mode == "config_model") {
                options.graph_mode = Graph::Mode::ConfigModel;
            } else {
                throw std::invalid_argument("Unknown graph mode: " + mode);
            }
        } else {
            throw std::invalid_argument("Unknown argument: " + arg);
        }
    }

    if (options.vertices <= 0 || options.degree < 0) {
        throw std::invalid_argument(
            "Usage: run_kissat --n <N> --d <D> [--seed <S>] [--outdir <PATH>]"
            " [--kissat <PATH>] [--graph_mode <circulant|config_model>]");
    }

    if (!seed_provided) {
        options.seed = 0;
    }

    return options;
}

}  // namespace

int main(int argc, char** argv) {
    RunOptions options;
    try {
        options = parseArgs(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Argument error: " << e.what() << '\n';
        return 1;
    }

    std::mt19937 rng(options.seed);
    Graph graph(options.vertices, options.degree, rng, options.graph_mode);
    auto charges = buildRandomCharges(options.vertices, rng);

    TseitinCnfBuilder builder;
    auto formula = builder.build(graph, charges);

    std::string base_name = "run_kissat_n" + std::to_string(options.vertices) +
        "_d" + std::to_string(options.degree) +
        "_s" + std::to_string(options.seed);
    std::filesystem::path cnf_path = options.out_dir / (base_name + ".cnf");
    std::filesystem::path solver_output = options.out_dir / (base_name + ".out");
    std::filesystem::create_directories(options.out_dir);
    TseitinCnfBuilder::writeDimacs(formula, cnf_path.string());
    std::string cnf_hash_hex = hashToHex(fnv1aHashFile(cnf_path));

    KissatRunner runner(options.kissat_path);
    auto t0 = std::chrono::steady_clock::now();
    KissatResult result = runner.run(cnf_path.string(), solver_output.string());
    auto t1 = std::chrono::steady_clock::now();

    long long runtime_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    if (result.runtime_ms > 0) {
        runtime_ms = result.runtime_ms;
    }

    std::cout << "cnf_hash: " << cnf_hash_hex << "\n";
    std::cout << "runtime_ms: " << runtime_ms << "\n";

    std::cout << "solve_status: " << (result.timed_out ? "UNKNOWN"
                        : (result.exit_code == 10 ? "SAT"
                        : (result.exit_code == 20 ? "UNSAT" : "OK")))
          << "\n";

    if (result.timed_out) {
        return 124; // conventional timeout code
    }
    return result.exit_code;

}
