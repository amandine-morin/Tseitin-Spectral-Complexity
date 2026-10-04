#include "graph.hpp"
#include "kissat_runner.hpp"
#include "tseitin_cnf.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>
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

namespace {

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

uint64_t deriveSeed(uint64_t base_seed, int vertices, int degree, int trial) {
    uint64_t seed = base_seed;
    seed ^= 0x9e3779b97f4a7c15ULL + static_cast<uint64_t>(vertices) + (seed << 6) + (seed >> 2);
    seed ^= 0x9e3779b97f4a7c15ULL + static_cast<uint64_t>(degree) + (seed << 6) + (seed >> 2);
    seed ^= 0x9e3779b97f4a7c15ULL + static_cast<uint64_t>(trial) + (seed << 6) + (seed >> 2);
    return seed;
}

struct RunOptions {
    uint64_t base_seed = 0;
    bool seed_provided = false;
    bool fixed_seed = false;
    int warmup_jobs = 0;
};

RunOptions parseArgs(int argc, char** argv) {
    RunOptions options;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--seed") {
            if (i + 1 >= argc) {
                throw std::invalid_argument("Missing value for --seed");
            }
            options.base_seed = static_cast<uint64_t>(std::stoull(argv[++i]));
            options.seed_provided = true;
        } else if (arg == "--warmup") {
            if (i + 1 >= argc) {
                throw std::invalid_argument("Missing value for --warmup");
            }
            options.warmup_jobs = std::stoi(argv[++i]);
            if (options.warmup_jobs < 0) {
                throw std::invalid_argument("--warmup must be non-negative");
            }
        } else if (arg == "--fixed-seed") {
            options.fixed_seed = true;
        } else {
            throw std::invalid_argument("Unknown argument: " + arg);
        }
    }
    if (!options.seed_provided) {
        std::random_device rd;
        options.base_seed = (static_cast<uint64_t>(rd()) << 32) ^ rd();
    }
    return options;
}

}  // namespace

// -------------------------------------------------------------
// MAIN: Heavy-mode experimental sweep
// -------------------------------------------------------------
int main(int argc, char** argv) {
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
    csv << "n,d,trial,base_seed,seed,cnf_hash,vars,clauses,runtime_ms,exit_code\n";

    KissatRunner runner;
    RunOptions options;
    try {
        options = parseArgs(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Argument error: " << e.what() << '\n';
        return 1;
    }

    // --------------------------------------
    // Optional warm-up jobs (not recorded)
    // --------------------------------------
    if (options.warmup_jobs > 0) {
        try {
            std::filesystem::path warm_cnf = work_dir / "warmup.cnf";
            std::filesystem::path warm_out = work_dir / "warmup.txt";
            for (int i = 1; i <= options.warmup_jobs; ++i) {
                uint64_t warm_seed = options.fixed_seed
                    ? options.base_seed
                    : deriveSeed(options.base_seed, 20, 3, i);
                std::mt19937 warm_rng(static_cast<uint32_t>(warm_seed));
                Graph warm_g(20, 3, warm_rng);
                auto warm_charges = buildRandomCharges(20, warm_rng);
                TseitinCnfBuilder warm_builder;
                auto warm_formula = warm_builder.build(warm_g, warm_charges);
                TseitinCnfBuilder::writeDimacs(warm_formula, warm_cnf.string());
                runner.run(warm_cnf.string(), warm_out.string());
            }
            std::cout << "[Warmup completed: " << options.warmup_jobs << " jobs]\n";
        }
        catch (...) {
            std::cout << "[Warmup failed, continuing anyway]\n";
        }
    }

    // --------------------------------------
    // REAL HEAVY EXPERIMENT LOOP
    // --------------------------------------
    struct Job {
        int vertices;
        int degree;
        int trial;
    };

    std::vector<Job> jobs;
    jobs.reserve(vertex_sizes.size() * degrees.size() * trials_per_config);
    for (int vertices : vertex_sizes) {
        for (int degree : degrees) {
            for (int trial = 1; trial <= trials_per_config; ++trial) {
                jobs.push_back(Job{vertices, degree, trial});
            }
        }
    }

    std::mt19937 job_rng(static_cast<uint32_t>(options.base_seed));
    std::shuffle(jobs.begin(), jobs.end(), job_rng);

    for (const auto& job : jobs) {
        const int vertices = job.vertices;
        const int degree = job.degree;
        const int trial = job.trial;
        try {
            // Derive a per-job seed from (base, n, d, trial) so instance identity
            // is deterministic and independent of shuffled run order.
            uint64_t trial_seed = options.fixed_seed
                ? options.base_seed
                : deriveSeed(options.base_seed, vertices, degree, trial);
            std::mt19937 trial_rng(static_cast<uint32_t>(trial_seed));

            Graph graph(vertices, degree, trial_rng);
            auto charges = buildRandomCharges(vertices, trial_rng);

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
            std::string cnf_hash_hex = hashToHex(fnv1aHashFile(cnf_path));

            auto t0 = std::chrono::steady_clock::now();
            KissatResult result = runner.run(cnf_path.string(), solver_output.string());
            auto t1 = std::chrono::steady_clock::now();

            long long runtime_ms =
                std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

            if (result.runtime_ms > 0)
                runtime_ms = result.runtime_ms;

            // Write CSV
            csv << vertices << ',' << degree << ',' << trial << ','
                << options.base_seed << ',' << trial_seed << ',' << cnf_hash_hex << ','
                << formula.variable_count << ',' << formula.clauses.size() << ','
                << runtime_ms << ',' << result.exit_code << '\n';

            // Console log
            std::cout << "HEAVY n=" << vertices
                      << " d=" << degree
                      << " trial=" << trial
                      << " base_seed=" << options.base_seed
                      << " seed=" << trial_seed
                      << " cnf_hash=" << cnf_hash_hex
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

    std::cout << "\n[HEAVY RUN COMPLETED]\n";
    std::cout << "Results written to: " << csv_path << "\n";

    return 0;
}
