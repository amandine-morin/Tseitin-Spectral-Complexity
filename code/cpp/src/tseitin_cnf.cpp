#include "tseitin_cnf.hpp"

#include <fstream>
#include <stdexcept>

TseitinCnfBuilder::TseitinCnfBuilder() : next_variable_(1) {}

int TseitinCnfBuilder::newVariable() {
    return next_variable_++;
}

void TseitinCnfBuilder::addClause(const std::vector<int>& clause) {
    clauses_.push_back(clause);
}

void TseitinCnfBuilder::addXor(int x, int y, int z) {
    // Standard 4-clause CNF encoding for z = x XOR y
    addClause({-x, -y, -z});
    addClause({-x, y, z});
    addClause({x, -y, z});
    addClause({x, y, -z});
}

CnfFormula TseitinCnfBuilder::build(const Graph& graph, const std::vector<bool>& charges) {
    if (static_cast<int>(charges.size()) != graph.vertexCount()) {
        throw std::invalid_argument("Charges vector must match vertex count");
    }

    clauses_.clear();
    edge_variables_.clear();
    next_variable_ = 1;

    // Assign variables to edges (1-based for DIMACS).
    edge_variables_.resize(graph.edges().size());
    for (size_t i = 0; i < graph.edges().size(); ++i) {
        edge_variables_[i] = newVariable();
    }

    // Build XOR constraint per vertex.
    for (int v = 0; v < graph.vertexCount(); ++v) {
        const auto& incident = graph.incidentEdges(v);
        if (incident.empty()) {
            continue;
        }

        std::vector<int> vars;
        vars.reserve(incident.size());
        for (int edge_index : incident) {
            vars.push_back(edge_variables_[edge_index]);
        }

        // Build XOR chain: (((e1 XOR e2) XOR e3) ... ) = charge
        int current = vars[0];
        for (size_t i = 1; i < vars.size(); ++i) {
            int aux = newVariable();
            addXor(current, vars[i], aux);
            current = aux;
        }

        // Enforce final parity.
        if (charges[v]) {
            addClause({current});
        } else {
            addClause({-current});
        }
    }

    CnfFormula result;
    result.variable_count = next_variable_ - 1;
    result.clauses = clauses_;
    return result;
}

void TseitinCnfBuilder::writeDimacs(const CnfFormula& formula, const std::string& path) {
    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("Failed to open output file: " + path);
    }

    out << "p cnf " << formula.variable_count << " " << formula.clauses.size() << "\n";
    for (const auto& clause : formula.clauses) {
        for (int lit : clause) {
            out << lit << " ";
        }
        out << "0\n";
    }
}

