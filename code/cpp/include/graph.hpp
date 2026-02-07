#pragma once

#include <random>
#include <utility>
#include <vector>

// Simple undirected graph data structure for d-regular graphs.
class Graph {
public:
    using Edge = std::pair<int, int>;

    enum class Mode {
        Circulant,
        ConfigModel
    };

    Graph(int vertices, int degree);
    Graph(int vertices, int degree, std::mt19937& rng);
    Graph(int vertices, int degree, std::mt19937& rng, Mode mode);

    int vertexCount() const { return vertex_count_; }
    int degree() const { return degree_; }

    // Return all edges as pairs with u < v.
    const std::vector<Edge>& edges() const { return edges_; }

    // Return edges incident to a vertex.
    const std::vector<int>& incidentEdges(int vertex) const { return incidence_[vertex]; }

private:
    int vertex_count_;
    int degree_;
    std::vector<Edge> edges_;
    std::vector<std::vector<int>> incidence_;

    void buildRegularGraph();
    void buildConfigurationModelGraph(std::mt19937& rng);
    void addEdge(int u, int v);
    void relabelVertices(const std::vector<int>& permutation);
};