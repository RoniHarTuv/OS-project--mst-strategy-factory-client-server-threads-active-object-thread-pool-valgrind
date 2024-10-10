#ifndef KRUSKAL_MST_H
#define KRUSKAL_MST_H

#include "Graph.h"
#include "IMSTSolver.h"
#include <vector>
#include <tuple>
#include <algorithm>
#include <list>
#include <queue>
#include <climits>
#include <mutex>

class KruskalMST : public IMSTSolver {
public:
    KruskalMST(const Graph& graph);

    void solve() override;
    int getMSTWeight() const override;
    const std::vector<std::tuple<int, int, int>>& getMSTEdges() const override;

    // New methods for additional operations
    int getDiameter() const override;
    double getAverageDistance() const override;
    int getShortestDistance(int xi, int xj) const override;

private:
    int find(std::vector<int>& parent, int i);
    void Union(std::vector<int>& parent, std::vector<int>& rank, int x, int y);

    void buildMSTAdjacencyList() const;

    const Graph& graph;
    int mst_weight;
    std::vector<std::tuple<int, int, int>> mst_edges;

    // MST adjacency list for calculations
    mutable std::vector<std::list<std::pair<int, int>>> mst_adj;
    mutable std::mutex mst_adj_mutex;
};

#endif // KRUSKAL_MST_H