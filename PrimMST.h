// PrimMST.h
#ifndef PRIM_MST_H
#define PRIM_MST_H

#include "Graph.h"
#include "IMSTSolver.h"
#include <vector>
#include <tuple>
#include <queue>
#include <functional>
#include <climits>
#include <list>
#include <mutex>

class PrimMST : public IMSTSolver {
public:
    PrimMST(const Graph& graph);

    void solve() override;
    int getMSTWeight() const override;
    const std::vector<std::tuple<int, int, int>>& getMSTEdges() const override;

    // New methods for additional operations
    int getDiameter() const override;
    double getAverageDistance() const override;
    int getShortestDistance(int xi, int xj) const override;

private:
    void buildMSTAdjacencyList() const;

    const Graph& graph;
    int mst_weight;
    std::vector<std::tuple<int, int, int>> mst_edges;

    // MST adjacency list for calculations
    mutable std::vector<std::list<std::pair<int, int>>> mst_adj;
    mutable std::mutex mst_adj_mutex;
};

#endif // PRIM_MST_H