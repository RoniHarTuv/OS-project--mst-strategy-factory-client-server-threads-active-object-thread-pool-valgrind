#include "PrimMST.h"

PrimMST::PrimMST(const Graph& graph) : graph(graph), mst_weight(0) {}

void PrimMST::solve() {
    int V = graph.getVertices();
    mst_edges.clear();
    mst_weight = 0;

    std::vector<bool> inMST(V, false);
    std::vector<int> key(V, INT_MAX);
    std::vector<int> parent(V, -1);

    // Min-heap priority queue
    using pii = std::pair<int, int>;
    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;

    // Start from vertex 0
    key[0] = 0;
    pq.push({0, 0});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (inMST[u])
            continue;

        inMST[u] = true;

        if (parent[u] != -1) {
            // Add edge to MST
            mst_edges.emplace_back(parent[u], u, key[u]);
            mst_weight += key[u];
        }

        for (const auto& neighbor : graph.getAdjacencyList()[u]) {
            int v = neighbor.first;
            int weight = neighbor.second;

            if (!inMST[v] && key[v] > weight) {
                key[v] = weight;
                pq.push({key[v], v});
                parent[v] = u;
            }
        }
    }

    // Build MST adjacency list for further calculations
    buildMSTAdjacencyList();
}

int PrimMST::getMSTWeight() const {
    return mst_weight;
}

const std::vector<std::tuple<int, int, int>>& PrimMST::getMSTEdges() const {
    return mst_edges;
}

void PrimMST::buildMSTAdjacencyList() const {
    std::lock_guard<std::mutex> lock(mst_adj_mutex);
    if (!mst_adj.empty()) return;  // Already built
    
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

int PrimMST::getDiameter() const {
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

double PrimMST::getAverageDistance() const {
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

int PrimMST::getShortestDistance(int xi, int xj) const {
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
