#include "graph.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <string>
#include <unordered_set>

Graph::Graph(int vertices, int degree)
    : vertex_count_(vertices), degree_(degree), mode_(Mode::Circulant), incidence_(vertices) {
    if (vertices <= 0) {
        throw std::invalid_argument("Number of vertices must be positive");
    }
    if (degree < 0 || degree >= vertices) {
        throw std::invalid_argument("Degree must be non-negative and less than number of vertices");
    }
    if ((static_cast<long long>(vertices) * degree) % 2 != 0) {
        throw std::invalid_argument("n * d must be even for a d-regular graph");
    }
    if (degree_ % 2 == 1 && vertex_count_ % 2 == 1) {
        throw std::invalid_argument("For odd degree, the vertex count must be even");
    }
    buildRegularGraph();
    finalizeGraph();
}

Graph::Graph(int vertices, int degree, std::mt19937& rng)
    : Graph(vertices, degree, rng, Mode::Circulant) {}

Graph::Graph(int vertices, int degree, std::mt19937& rng, Mode mode, double p)
    : vertex_count_(vertices), degree_(degree), mode_(mode), incidence_(vertices) {
    if (vertices <= 0) {
        throw std::invalid_argument("Number of vertices must be positive");
    }
    if (degree < 0 || degree >= vertices) {
        throw std::invalid_argument("Degree must be non-negative and less than number of vertices");
    }
    if ((static_cast<long long>(vertices) * degree) % 2 != 0) {
        throw std::invalid_argument("n * d must be even for a d-regular graph");
    }
    if (degree_ % 2 == 1 && vertex_count_ % 2 == 1) {
        throw std::invalid_argument("For odd degree, the vertex count must be even");
    }
    switch (mode) {
        case Mode::Circulant: {
            buildRegularGraph();

            std::vector<int> permutation(vertex_count_);
            std::iota(permutation.begin(), permutation.end(), 0);
            std::shuffle(permutation.begin(), permutation.end(), rng);
            relabelVertices(permutation);
            finalizeGraph();
            return;
        }
        case Mode::ConfigModel:
            buildConfigurationModelGraph(rng);
            return;
        case Mode::WattsStrogatz:
            buildWattsStrogatzGraph(rng, p);
            return;
        default:
            throw std::invalid_argument("Unknown Graph::Mode");
    }
}

void Graph::addEdge(int u, int v) {
    if (u == v) {
        throw std::invalid_argument("Self-loops are not allowed");
    }
    if (u > v) {
        std::swap(u, v);
    }
    edges_.emplace_back(u, v);
    int edge_index = static_cast<int>(edges_.size()) - 1;
    incidence_[u].push_back(edge_index);
    incidence_[v].push_back(edge_index);
}

void Graph::buildRegularGraph() {
    // Circulant construction: connect each vertex to offsets 1..degree/2 and
    // optionally to n/2 for odd degree (requires even vertex_count_).
    int n = vertex_count_;
    int half = degree_ / 2;
    for (int offset = 1; offset <= half; ++offset) {
        for (int u = 0; u < n; ++u) {
            int v = (u + offset) % n;
            addEdge(u, v);
        }
    }

    if (degree_ % 2 == 1) {
        int offset = n / 2;
        for (int u = 0; u < n; ++u) {
            int v = (u + offset) % n;
            if (u < v) {
                addEdge(u, v);
            }
        }
    }
}

void Graph::buildConfigurationModelGraph(std::mt19937& rng) {
    constexpr int kMaxRetries = 1000;
    std::vector<int> stubs;
    stubs.reserve(static_cast<size_t>(vertex_count_) * degree_);

    for (int attempt = 0; attempt < kMaxRetries; ++attempt) {
        edges_.clear();
        for (auto& vec : incidence_) {
            vec.clear();
        }

        stubs.clear();
        for (int v = 0; v < vertex_count_; ++v) {
            for (int i = 0; i < degree_; ++i) {
                stubs.push_back(v);
            }
        }

        std::shuffle(stubs.begin(), stubs.end(), rng);

        bool invalid = false;
        for (size_t i = 0; i < stubs.size(); i += 2) {
            int u = stubs[i];
            int v = stubs[i + 1];
            if (u == v) {
                invalid = true;
                break;
            }
            if (u > v) {
                std::swap(u, v);
            }
            if (std::find(edges_.begin(), edges_.end(), Edge{u, v}) != edges_.end()) {
                invalid = true;
                break;
            }
            addEdge(u, v);
        }

        if (!invalid) {
            finalizeGraph();
            return;
        }
    }

    throw std::runtime_error(
        "Failed to generate simple d-regular graph using configuration model for n=" +
        std::to_string(vertex_count_) + ", d=" + std::to_string(degree_) +
        ", retries=" + std::to_string(kMaxRetries));
}

void Graph::buildWattsStrogatzGraph(std::mt19937& rng, double p) {
    if (degree_ % 2 != 0) {
        throw std::invalid_argument("Watts-Strogatz mode requires an even degree");
    }
    if (p < 0.0 || p > 1.0) {
        throw std::invalid_argument("Watts-Strogatz p must be in [0, 1]");
    }

    buildRegularGraph();
    if (edges_.empty()) {
        finalizeGraph();
        return;
    }

    std::vector<int> permutation(vertex_count_);
    std::iota(permutation.begin(), permutation.end(), 0);
    std::shuffle(permutation.begin(), permutation.end(), rng);
    relabelVertices(permutation);

    std::vector<std::unordered_set<int>> adjacency(vertex_count_);
    for (const auto& edge : edges_) {
        adjacency[edge.first].insert(edge.second);
        adjacency[edge.second].insert(edge.first);
    }

    auto removeEdgeFromAdj = [&](int u, int v) {
        adjacency[u].erase(v);
        adjacency[v].erase(u);
    };
    auto addEdgeToAdj = [&](int u, int v) {
        adjacency[u].insert(v);
        adjacency[v].insert(u);
    };
    auto edgeExists = [&](int u, int v) {
        return adjacency[u].find(v) != adjacency[u].end();
    };

    int max_swaps = static_cast<int>(edges_.size()) * 10;
    int target_swaps = static_cast<int>(std::llround(p * max_swaps));
    if (target_swaps == 0) {
        finalizeGraph();
        return;
    }

    std::uniform_int_distribution<int> edge_dist(0, static_cast<int>(edges_.size()) - 1);
    std::uniform_int_distribution<int> pattern_dist(0, 1);

    int max_attempts = std::max(1000, target_swaps * 200);
    int swaps = 0;

    for (int attempt = 0; attempt < max_attempts && swaps < target_swaps; ++attempt) {
        int e1 = edge_dist(rng);
        int e2 = edge_dist(rng);
        if (e1 == e2) {
            continue;
        }

        int a = edges_[e1].first;
        int b = edges_[e1].second;
        int c = edges_[e2].first;
        int d = edges_[e2].second;

        if (a == c || a == d || b == c || b == d) {
            continue;
        }

        removeEdgeFromAdj(a, b);
        removeEdgeFromAdj(c, d);

        int u1 = 0;
        int v1 = 0;
        int u2 = 0;
        int v2 = 0;
        if (pattern_dist(rng) == 0) {
            u1 = a;
            v1 = c;
            u2 = b;
            v2 = d;
        } else {
            u1 = a;
            v1 = d;
            u2 = b;
            v2 = c;
        }

        bool valid = true;
        if (u1 == v1 || u2 == v2) {
            valid = false;
        } else if ((u1 == u2 && v1 == v2) || (u1 == v2 && v1 == u2)) {
            valid = false;
        } else if (edgeExists(u1, v1) || edgeExists(u2, v2)) {
            valid = false;
        }

        if (!valid) {
            addEdgeToAdj(a, b);
            addEdgeToAdj(c, d);
            continue;
        }

        addEdgeToAdj(u1, v1);
        addEdgeToAdj(u2, v2);

        if (u1 > v1) {
            std::swap(u1, v1);
        }
        if (u2 > v2) {
            std::swap(u2, v2);
        }
        edges_[e1] = Edge{u1, v1};
        edges_[e2] = Edge{u2, v2};
        swaps++;
    }

    if (swaps < target_swaps) {
        throw std::runtime_error(
            "Failed to perform degree-preserving rewiring for Watts-Strogatz mode "
            "(n=" + std::to_string(vertex_count_) +
            ", d=" + std::to_string(degree_) +
            ", p=" + std::to_string(p) +
            ", edges=" + std::to_string(edges_.size()) +
            ", target_swaps=" + std::to_string(target_swaps) +
            ", performed_swaps=" + std::to_string(swaps) +
            ", max_attempts=" + std::to_string(max_attempts) + ")");
    }

    finalizeGraph();
}

void Graph::finalizeGraph() {
    std::sort(edges_.begin(), edges_.end());
    rebuildIncidence();
    validateSimpleRegularOrThrow();
}

void Graph::validateSimpleRegularOrThrow() const {
    auto modeToString = [](Mode mode) {
        switch (mode) {
            case Mode::Circulant:
                return "circulant";
            case Mode::ConfigModel:
                return "config_model";
            case Mode::WattsStrogatz:
                return "watts_strogatz";
            default:
                return "unknown";
        }
    };

    auto fail = [&](const std::string& reason) {
        throw std::runtime_error(
            "Graph validation failed (n=" + std::to_string(vertex_count_) +
            ", d=" + std::to_string(degree_) +
            ", mode=" + modeToString(mode_) +
            "): " + reason);
    };

    if (static_cast<long long>(edges_.size()) !=
        (static_cast<long long>(vertex_count_) * degree_) / 2) {
        fail("edge count mismatch");
    }

    std::vector<int> degrees(vertex_count_, 0);
    std::unordered_set<uint64_t> seen;
    seen.reserve(edges_.size() * 2 + 1);

    for (const auto& edge : edges_) {
        int u = edge.first;
        int v = edge.second;
        if (u < 0 || v < 0 || u >= vertex_count_ || v >= vertex_count_) {
            fail("vertex index out of range");
        }
        if (u >= v) {
            fail("edge not stored with u < v");
        }
        uint64_t key = (static_cast<uint64_t>(u) << 32) | static_cast<uint64_t>(v);
        if (!seen.insert(key).second) {
            fail("duplicate edge");
        }
        degrees[u]++;
        degrees[v]++;
    }

    for (int v = 0; v < vertex_count_; ++v) {
        if (degrees[v] != degree_) {
            fail("degree mismatch at v=" + std::to_string(v));
        }
    }
}

void Graph::rebuildIncidence() {
    incidence_.assign(vertex_count_, {});
    for (size_t i = 0; i < edges_.size(); ++i) {
        int u = edges_[i].first;
        int v = edges_[i].second;
        incidence_[u].push_back(static_cast<int>(i));
        incidence_[v].push_back(static_cast<int>(i));
    }
}

void Graph::relabelVertices(const std::vector<int>& permutation) {
    std::vector<Edge> relabeled_edges;
    relabeled_edges.reserve(edges_.size());

    std::vector<std::vector<int>> relabeled_incidence(vertex_count_);
    for (const auto& edge : edges_) {
        int u = permutation[edge.first];
        int v = permutation[edge.second];
        if (u == v) {
            throw std::runtime_error("Permutation produced a self-loop");
        }
        if (u > v) {
            std::swap(u, v);
        }
        int edge_index = static_cast<int>(relabeled_edges.size());
        relabeled_edges.emplace_back(u, v);
        relabeled_incidence[u].push_back(edge_index);
        relabeled_incidence[v].push_back(edge_index);
    }

    edges_ = std::move(relabeled_edges);
    incidence_ = std::move(relabeled_incidence);
}
