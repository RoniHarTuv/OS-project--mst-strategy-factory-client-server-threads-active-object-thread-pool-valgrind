#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <list>
#include <queue>
#include <functional>
#include <algorithm>
#include <chrono>
#include <signal.h> // Include signal handling
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Graph.h"
#include "MSTFactory.h"
#include "ActiveObject.h"
#include "ThreadPool.h"

#define PORT 9034
#define MAX_CLIENTS 100

std::mutex cout_mutex;

// Declare listener_fd globally
int listener_fd = -1;

// Signal handler function
void signal_handler(int signal)
{
    if (signal == SIGTERM || signal == SIGINT)
    { // Include SIGINT
        if (listener_fd >= 0)
        {
            close(listener_fd); // Close the listener socket
            listener_fd = -1;   // Set to -1 to indicate it's closed
        }
    }
}

// Helper function to read a line from the socket
std::string recv_line(int sock)
{
    std::string line;
    char c;
    while (true)
    {
        int bytes_received = recv(sock, &c, 1, 0);
        if (bytes_received <= 0)
        {
            // Connection closed or error
            return "";
        }
        if (c == '\n')
        {
            break;
        }
        else if (c == '\r')
        {
            // Peek ahead to see if next char is '\n'
            int peek_bytes = recv(sock, &c, 1, MSG_PEEK);
            if (peek_bytes > 0 && c == '\n')
            {
                // Consume the '\n'
                recv(sock, &c, 1, 0);
            }
            break;
        }
        line += c;
    }
    return line;
}

// Function to handle each client connection
void handle_client(int client_sock, ActiveObject &ao)
{
    std::string response;
    Graph *graph = Graph::getInstance();
    MSTType mstType = MSTType::KRUSKAL; // Default MST algorithm

    auto send_response = [client_sock](const std::string &msg)
    {
        send(client_sock, msg.c_str(), msg.length(), 0);
    };

    send_response("Do you prefer to use Kruskal or Prim for MST computation?\n");

    std::string cmd = recv_line(client_sock);
    if (cmd.empty())
    {
        close(client_sock);
        return;
    }

    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    if (cmd == "kruskal")
    {
        mstType = MSTType::KRUSKAL;
        send_response("MST algorithm set to Kruskal.\n");
    }
    else if (cmd == "prim")
    {
        mstType = MSTType::PRIM;
        send_response("MST algorithm set to Prim.\n");
    }
    else
    {
        send_response("Unknown MST algorithm. Defaulting to Kruskal.\n");
    }

    while (true)
    {
        bool graph_exists = graph->isInitialized();

        if (!graph_exists)
        {
            send_response("Please write the amount of vertices and edges that you want in the graph (format: vertices edges):\n");

            cmd = recv_line(client_sock);
            if (cmd.empty())
            {
                close(client_sock);
                return;
            }

            std::istringstream iss(cmd);
            int v, e;
            if (!(iss >> v >> e))
            {
                send_response("Invalid input. Please try again.\n");
                continue;
            }

            send_response("Enter the edges (format: u v weight):\n");

            std::vector<std::tuple<int, int, int>> edges;
            for (int i = 0; i < e; ++i)
            {
                std::string edge_cmd = recv_line(client_sock);
                if (edge_cmd.empty())
                {
                    close(client_sock);
                    return;
                }

                std::istringstream edge_iss(edge_cmd);
                int u, v_edge, w;
                if (!(edge_iss >> u >> v_edge >> w))
                {
                    send_response("Invalid edge input. Please try again.\n");
                    --i;
                    continue;
                }
                edges.emplace_back(u, v_edge, w);
            }

            ao.send([v, e, edges, &response, graph]()
                    {
                graph->newGraph(v, e);
                for (const auto& edge : edges) {
                    int u = std::get<0>(edge);
                    int v_edge = std::get<1>(edge);
                    int w = std::get<2>(edge);
                    graph->newEdge(u, v_edge, w);
                }
                response = "Graph created successfully.\n"; });

            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            ao.send([&response, graph, mstType]()
                    {
                auto mstSolver = MSTFactory::createMST(mstType, *graph);
                if (mstSolver) {
                    mstSolver->solve();
                    response = "MST calculated successfully.\n";
                } else {
                    response = "Failed to calculate MST.\n";
                } });

            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            send_response("Graph and MST are ready.\n");
        }

        std::string menu = "Please choose an operation:\n"
                           "1. Total weight of the MST\n"
                           "2. Longest distance between two vertices\n"
                           "3. Average distance between any two vertices in the MST\n"
                           "4. Shortest distance between two vertices\n"
                           "5. Add edge\n"
                           "6. Remove edge\n"
                           "7. New graph\n"
                           "Enter the number of the operation:\n";
        send_response(menu);

        cmd = recv_line(client_sock);
        if (cmd.empty())
        {
            close(client_sock);
            return;
        }

        int operation;
        try
        {
            operation = std::stoi(cmd);
        }
        catch (const std::exception &)
        {
            send_response("Invalid input. Please enter a number.\n");
            continue;
        }

        switch (operation)
        {
        case 1: // Total weight of MST
            ao.send([client_sock, &response, graph, mstType, &send_response]()
                    {
                    auto mstSolver = MSTFactory::createMST(mstType, *graph);
                    if (mstSolver) {
                        mstSolver->solve();
                        int weight = mstSolver->getMSTWeight();
                        response = "Total weight of MST: " + std::to_string(weight) + "\n";
                    } else {
                        response = "MST algorithm not set or invalid.\n";
                    }
                    send_response(response); });
            break;

        case 2: // Longest distance between two vertices
            ao.send([client_sock, &response, graph, mstType, &send_response]()
                    {
                    auto mstSolver = MSTFactory::createMST(mstType, *graph);
                    if (mstSolver) {
                        mstSolver->solve();
                        int diameter = mstSolver->getDiameter();
                        response = "Longest distance in MST: " + std::to_string(diameter) + "\n";
                    } else {
                        response = "MST algorithm not set or invalid.\n";
                    }
                    send_response(response); });
            break;

        case 3: // Average distance between any two vertices in the MST
            ao.send([client_sock, &response, graph, mstType, &send_response]()
                    {
                    auto mstSolver = MSTFactory::createMST(mstType, *graph);
                    if (mstSolver) {
                        mstSolver->solve();
                        double avg_distance = mstSolver->getAverageDistance();
                        response = "Average distance in MST: " + std::to_string(avg_distance) + "\n";
                    } else {
                        response = "MST algorithm not set or invalid.\n";
                    }
                    send_response(response); });
            break;

        case 4: // Shortest distance between two vertices Xi, Xj
            send_response("Enter two vertices Xi and Xj:\n");
            cmd = recv_line(client_sock);
            if (cmd.empty())
            {
                close(client_sock);
                return;
            }

            {
                std::istringstream iss(cmd);
                int xi, xj;
                if (!(iss >> xi >> xj))
                {
                    send_response("Invalid input. Please enter two integers.\n");
                    break;
                }

                ao.send([xi, xj, client_sock, &response, graph, mstType, &send_response]()
                        {
                        auto mstSolver = MSTFactory::createMST(mstType, *graph);
                        if (mstSolver) {
                            mstSolver->solve();
                            int shortest_distance = mstSolver->getShortestDistance(xi - 1, xj - 1);
                            response = "Shortest distance between " + std::to_string(xi) + " and " + std::to_string(xj) + " in MST: " + std::to_string(shortest_distance) + "\n";
                        } else {
                            response = "MST algorithm not set or invalid.\n";
                        }
                        send_response(response); });
            }
            break;

        case 5: // Add edge
            send_response("Enter the edge to add (format: u v weight):\n");
            cmd = recv_line(client_sock);
            if (cmd.empty())
            {
                close(client_sock);
                return;
            }

            {
                std::istringstream iss(cmd);
                int u, v_edge, w;
                if (!(iss >> u >> v_edge >> w))
                {
                    send_response("Invalid input. Please enter three integers.\n");
                    break;
                }

                ao.send([u, v_edge, w, client_sock, &response, graph, mstType, &send_response]()
                        {
                        graph->newEdge(u, v_edge, w);
                        auto mstSolver = MSTFactory::createMST(mstType, *graph);
                        if (mstSolver) {
                            mstSolver->solve();
                            response = "Edge added and MST updated successfully.\n";
                        } else {
                            response = "Failed to update MST.\n";
                        }
                        send_response(response); });
            }
            break;

        case 6: // Remove edge
            send_response("Enter the edge to remove (format: u v):\n");
            cmd = recv_line(client_sock);
            if (cmd.empty())
            {
                close(client_sock);
                return;
            }

            {
                std::istringstream iss(cmd);
                int u, v_edge;
                if (!(iss >> u >> v_edge))
                {
                    send_response("Invalid input. Please enter two integers.\n");
                    break;
                }

                ao.send([u, v_edge, client_sock, &response, graph, mstType, &send_response]()
                        {
                        graph->removeEdge(u, v_edge);
                        auto mstSolver = MSTFactory::createMST(mstType, *graph);
                        if (mstSolver) {
                            mstSolver->solve();
                            response = "Edge removed and MST updated successfully.\n";
                        } else {
                            response = "Failed to update MST.\n";
                        }
                        send_response(response); });
            }
            break;

        case 7: // New graph
            ao.send([&graph]()
                    { graph->newGraph(0, 0); });
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            send_response("Graph has been reset. Please create a new graph.\n");
            break;

        default:
            send_response("Invalid operation selected.\n");
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main()
{
    int client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    // Register signal handler for SIGTERM and SIGINT
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler); // Handle Ctrl+C gracefully

    listener_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_fd < 0)
    {
        perror("Socket error");
        exit(1);
    }

    // Set socket options to reuse address and port
    int opt = 1;
    setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    // Bind
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listener_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind error");
        close(listener_fd);
        exit(1);
    }

    // Listen
    if (listen(listener_fd, 10) < 0)
    {
        perror("Listen error");
        close(listener_fd);
        exit(1);
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    // ThreadPool and ActiveObject
    ThreadPool threadPool(4);
    ActiveObject ao;

    while (true)
    {
        addr_size = sizeof(client_addr);
        client_fd = accept(listener_fd, (struct sockaddr *)&client_addr, &addr_size);
        if (client_fd < 0)
        {
            if (listener_fd == -1)
            {
                // Listener socket was closed by signal handler, exit the loop
                break;
            }
            perror("Accept error");
            continue;
        }
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Accepted connection from " << inet_ntoa(client_addr.sin_addr) << std::endl;
        }
        threadPool.enqueue([client_fd, &ao]()
                           { handle_client(client_fd, ao); });
    }

    // Server is shutting down
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Server is shutting down..." << std::endl;
    }
    threadPool.stop();
    ao.stop();
    Graph::destroyInstance();
    return 0;
}
