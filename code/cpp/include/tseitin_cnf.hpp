#pragma once

#include "graph.hpp"

#include <string>
#include <unordered_map>
#include <vector>

struct CnfFormula {
    int variable_count = 0;
    std::vector<std::vector<int>> clauses;
};

class TseitinCnfBuilder {
public:
    TseitinCnfBuilder();

    // Build a Tseitin CNF from the given graph and vertex charges (true = 1).
    CnfFormula build(const Graph& graph, const std::vector<bool>& charges);

    // Write the CNF formula to a file in DIMACS format.
    static void writeDimacs(const CnfFormula& formula, const std::string& path);

private:
    int next_variable_;
    std::vector<std::vector<int>> clauses_;
    std::vector<int> edge_variables_;

    int newVariable();
    void addClause(const std::vector<int>& clause);
    void addXor(int x, int y, int z);
};

