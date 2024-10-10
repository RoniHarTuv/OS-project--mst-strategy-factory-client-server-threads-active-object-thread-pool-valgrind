#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <list>
#include <tuple>
#include <memory>
#include <mutex>
#include <atomic>

enum class MSTType;

class Graph {
private:
    std::atomic<int> vertices; // Number of vertices in the graph
    std::vector<std::tuple<int, int, int>> edgeList; // List of edges (u, v, weight)
    std::vector<std::list<std::pair<int, int>>> adj; // Adjacency list (vertex, weight)
    mutable std::mutex mtx; // Mutex for thread safety

    // Singleton instance
    static std::atomic<Graph*> instance;
    static std::mutex instance_mtx; // Mutex to protect instance creation/destruction

    // Private constructor
    Graph() : vertices(0) {}

public:
    // Get singleton instance
    static Graph* getInstance();

    // Delete copy constructor and assignment operator
    Graph(const Graph&) = delete;
    void operator=(const Graph&) = delete;

    // Create a new graph
    void newGraph(int v, int e);

    // Add a new edge
    void newEdge(int u, int v, int w);

    // Remove an edge
    void removeEdge(int u, int v);

    // Getters
    int getVertices() const;
    const std::vector<std::tuple<int, int, int>>& getEdges() const;
    const std::vector<std::list<std::pair<int, int>>>& getAdjacencyList() const;

    // Function to calculate the MST using the factory pattern
    void calculateMST(MSTType type);

    // Destroy the singleton instance
    static void destroyInstance();

    // Method to check if the graph is initialized
    bool isInitialized() const;

    // Virtual destructor
    virtual ~Graph();
};

#endif // GRAPH_H