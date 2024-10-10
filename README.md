# OS-project : mst-strategy-factory-client-server-threads-active-object-thread-pool-valgrind
This is my final project in Operations system course, 2024. 

This project provides a server that solves the Minimal Spanning Tree (MST) problem on directed, weighted graphs. The server supports two MST algorithms, Prim's and Kruskal's, and allows clients to interact with the graph and MST operations.
Features

The server includes the following functionalities:

    Graph Management:
        Upload a new graph by specifying the number of vertices and edges.
        Add and remove edges from the graph.
        Reset and create new graphs.

    MST Algorithm Factory:
        The client can choose between Prim's or Kruskal's algorithm for MST computation.
        The chosen algorithm is used to compute the MST and relevant metrics.

    Client-Server Interaction:
        The server accepts multiple client connections and allows interaction through a set of commands.
        Each client can:
            Choose an MST algorithm (Prim or Kruskal).
            Input graph data (vertices and edges).
            Request MST-related operations (e.g., total weight, longest distance).

    Thread Management:
        The server utilizes a Thread Pool for handling multiple client requests concurrently.
        Leader-Follower Thread Pool design is used to process client requests efficiently.

    Active Object Design Pattern:
        The server implements the Active Object pattern to handle asynchronous task execution and MST computation.

    MST Metrics: After computing the MST, the server provides the following metrics:
        Total Weight: The total weight of the MST.
        Longest Distance: The longest distance between two vertices in the MST.
        Average Distance: The average distance between all pairs of vertices in the MST.
        Shortest Distance: The shortest distance between two vertices in the MST.

    Graceful Shutdown:
        The server listens for SIGTERM and SIGINT signals to allow graceful shutdown (closes connections and stops threads).

Installation

Build the project:

    make all

Run the server:

    ./server

Usage

Once the server is running, clients can connect and issue commands to interact with the graph and solve the MST problem.
Example Commands

    Choose MST Algorithm:
        Clients can select between Prim's or Kruskal's MST algorithm:


    kruskal /
    prim

Graph Input:

    Specify the number of vertices and edges:


5 7

Add edges by specifying the two vertices and the weight:


        1 2 10

    Operations: Clients can choose from the following operations:
        Total weight of the MST
        Longest distance between two vertices
        Average distance between vertices
        Shortest distance between two vertices
        Add or remove edges
        Reset and create a new graph

Example Client Interaction:

    Connect to the server and choose the MST algorithm:

    telnet 9034 127.0.0.1

Do you prefer to use Kruskal or Prim for MST computation?

    Enter kruskal or prim.

Enter the number of vertices and edges for the graph:


Please write the amount of vertices and edges that you want in the graph (format: vertices edges):

Add the edges:

Enter the edges (format: u v weight):

Choose the MST operation (e.g., total weight, longest distance):


    Please choose an operation:
    1. Total weight of the MST
    2. Longest distance between two vertices
    3. Average distance between any two vertices in the MST
    4. Shortest distance between two vertices

Thread Pool and Concurrency

    The server uses a thread pool to handle multiple client connections concurrently.
    Tasks related to MST computation are processed using the Active Object pattern to manage asynchronous execution.

Valgrind Analysis

We provide Valgrind analysis to ensure memory safety and correct thread management:

    Memcheck: Detects memory leaks.
    Helgrind: Identifies threading errors.
