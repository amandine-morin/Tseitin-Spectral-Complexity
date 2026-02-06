#include "graph.hpp"

#include <algorithm>
#include <numeric>
#include <stdexcept>

Graph::Graph(int vertices, int degree)
    : vertex_count_(vertices), degree_(degree), incidence_(vertices) {
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
}

Graph::Graph(int vertices, int degree, std::mt19937& rng)
    : vertex_count_(vertices), degree_(degree), incidence_(vertices) {
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

    std::vector<int> permutation(vertex_count_);
    std::iota(permutation.begin(), permutation.end(), 0);
    std::shuffle(permutation.begin(), permutation.end(), rng);
    relabelVertices(permutation);
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
            if (u < v) {
                addEdge(u, v);
            }
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
