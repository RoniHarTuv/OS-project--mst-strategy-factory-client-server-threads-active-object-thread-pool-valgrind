#include "Graph.h"
#include "MSTFactory.h"

std::atomic<Graph*> Graph::instance(nullptr);
std::mutex Graph::instance_mtx;

Graph* Graph::getInstance() {
    Graph* expected = nullptr;
    Graph* desired = new Graph();
    if (instance.compare_exchange_strong(expected, desired, std::memory_order_acq_rel)) {
        return desired;
    }
    delete desired;
    return instance.load(std::memory_order_acquire);
}

void Graph::destroyInstance() {
    std::lock_guard<std::mutex> lock(instance_mtx);
    Graph* expected = instance.load(std::memory_order_acquire);
    if (expected && instance.compare_exchange_strong(expected, nullptr, std::memory_order_acq_rel)) {
        delete expected;
    }
}

void Graph::newGraph(int v, int e) {
    std::lock_guard<std::mutex> lock(mtx);
    vertices.store(v);
    edgeList.clear();
    adj.clear();
    adj.resize(v);
}

void Graph::newEdge(int u, int v, int w) {
    std::lock_guard<std::mutex> lock(mtx);
    edgeList.emplace_back(u - 1, v - 1, w);
    adj[u - 1].emplace_back(v - 1, w);
    adj[v - 1].emplace_back(u - 1, w);
}

void Graph::removeEdge(int u, int v) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = std::remove_if(edgeList.begin(), edgeList.end(),
        [u, v](const std::tuple<int, int, int>& edge) {
            return (std::get<0>(edge) == u - 1 && std::get<1>(edge) == v - 1) ||
                   (std::get<0>(edge) == v - 1 && std::get<1>(edge) == u - 1);
        });
    if (it != edgeList.end()) {
        edgeList.erase(it, edgeList.end());
        adj[u - 1].remove_if([v](const std::pair<int, int>& neighbor) { return neighbor.first == v - 1; });
        adj[v - 1].remove_if([u](const std::pair<int, int>& neighbor) { return neighbor.first == u - 1; });
    }
}

int Graph::getVertices() const {
    return vertices.load();
}

const std::vector<std::tuple<int, int, int>>& Graph::getEdges() const {
    std::lock_guard<std::mutex> lock(mtx);
    return edgeList;
}

const std::vector<std::list<std::pair<int, int>>>& Graph::getAdjacencyList() const {
    std::lock_guard<std::mutex> lock(mtx);
    return adj;
}

void Graph::calculateMST(MSTType type) {
    auto mstSolver = MSTFactory::createMST(type, *this);
    if (mstSolver) {
        mstSolver->solve();
    } else {
        std::cout << "Invalid MST type selected!" << std::endl;
    }
}

bool Graph::isInitialized() const {
    return vertices.load() > 0;
}

Graph::~Graph() {
    std::lock_guard<std::mutex> lock(mtx);
}