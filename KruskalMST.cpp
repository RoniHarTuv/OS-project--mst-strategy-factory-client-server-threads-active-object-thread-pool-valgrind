#include "KruskalMST.h"

KruskalMST::KruskalMST(const Graph& graph) : graph(graph), mst_weight(0) {}

void KruskalMST::solve() {
    int V = graph.getVertices();
    auto edges = graph.getEdges();

    // Sort edges by weight
    std::sort(edges.begin(), edges.end(), [](const auto& a, const auto& b) {
        return std::get<2>(a) < std::get<2>(b);
    });

    std::vector<int> parent(V);
    std::vector<int> rank(V, 0);
    for (int i = 0; i < V; i++)
        parent[i] = i;

    mst_edges.clear();
    mst_weight = 0;

    for (const auto& edge : edges) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        int weight = std::get<2>(edge);

        int root_u = find(parent, u);
        int root_v = find(parent, v);

        if (root_u != root_v) {
            mst_edges.push_back(edge);
            mst_weight += weight;
            Union(parent, rank, root_u, root_v);
        }
    }

    // Build MST adjacency list for further calculations
    buildMSTAdjacencyList();
}

int KruskalMST::getMSTWeight() const {
    return mst_weight;
}

const std::vector<std::tuple<int, int, int>>& KruskalMST::getMSTEdges() const {
    return mst_edges;
}

int KruskalMST::find(std::vector<int>& parent, int i) {
    if (parent[i] != i)
        parent[i] = find(parent, parent[i]);
    return parent[i];
}

void KruskalMST::Union(std::vector<int>& parent, std::vector<int>& rank, int x, int y) {
    if (rank[x] < rank[y])
        parent[x] = y;
    else if (rank[x] > rank[y])
        parent[y] = x;
    else {
        parent[y] = x;
        rank[x]++;
    }
}

void KruskalMST::buildMSTAdjacencyList() const {
    std::lock_guard<std::mutex> lock(mst_adj_mutex);
    if (!mst_adj.empty()) return;
    
    int V = graph.getVertices();
    mst_adj.clear();
    mst_adj.resize(V);
    for (const auto& edge : mst_edges) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        int w = std::get<2>(edge);
        mst_adj[u].emplace_back(v, w);
        mst_adj[v].emplace_back(u, w);
    }
}

int KruskalMST::getDiameter() const {
    if (mst_adj.empty())
        buildMSTAdjacencyList();

    int V = graph.getVertices();

    // BFS function to find farthest node and its distance
    auto bfs = [&](int start) {
        std::vector<bool> visited(V, false);
        std::vector<int> dist(V, 0);
        std::queue<int> q;
        q.push(start);
        visited[start] = true;
        int farthest_node = start;

        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (const auto& neighbor : mst_adj[u]) {
                int v = neighbor.first;
                int w = neighbor.second;
                if (!visited[v]) {
                    visited[v] = true;
                    dist[v] = dist[u] + w;
                    if (dist[v] > dist[farthest_node])
                        farthest_node = v;
                    q.push(v);
                }
            }
        }
        return std::make_pair(farthest_node, dist[farthest_node]);
    };

    auto [node, _] = bfs(0);
    auto [farthest_node, max_dist] = bfs(node);

    return max_dist;
}

double KruskalMST::getAverageDistance() const {
    if (mst_adj.empty())
        buildMSTAdjacencyList();

    int V = graph.getVertices();
    long long total_distance = 0;
    int pair_count = 0;

    // For all pairs (i, j) where i <= j
    for (int i = 0; i < V; ++i) {
        // BFS to compute distances from node i
        std::vector<int> dist(V, INT_MAX);
        std::queue<int> q;
        q.push(i);
        dist[i] = 0;

        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (const auto& neighbor : mst_adj[u]) {
                int v = neighbor.first;
                int w = neighbor.second;
                if (dist[v] == INT_MAX) {
                    dist[v] = dist[u] + w;
                    q.push(v);
                }
            }
        }

        for (int j = i; j < V; ++j) {
            if (dist[j] != INT_MAX) {
                total_distance += dist[j];
                pair_count++;
            }
        }
    }

    return static_cast<double>(total_distance) / pair_count;
}

int KruskalMST::getShortestDistance(int xi, int xj) const {
    if (mst_adj.empty())
        buildMSTAdjacencyList();

    int V = graph.getVertices();
    std::vector<bool> visited(V, false);
    std::vector<int> dist(V, INT_MAX);
    std::queue<int> q;
    q.push(xi);
    dist[xi] = 0;
    visited[xi] = true;

    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (u == xj)
            break;
        for (const auto& neighbor : mst_adj[u]) {
            int v = neighbor.first;
            int w = neighbor.second;
            if (!visited[v]) {
                visited[v] = true;
                dist[v] = dist[u] + w;
                q.push(v);
            }
        }
    }

    return dist[xj];
}
