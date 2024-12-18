# Shortset-Path-Server-Client in C++ : 

This project consists of two programs, a server and a client, that communicate over TCP/IP to compute the shortest path between two nodes in a graph. The server stores previously computed paths in a cache to speed up future requests.

Server (server.cpp)
Graph Representation:

The server reads a graph from a file where each line represents an undirected edge between two nodes. This graph is stored as an adjacency list in a vector of vectors.
Shortest Path Calculation (Breadth-First Search):

The server implements the BFS algorithm to calculate the shortest path between two nodes in the graph. It uses a queue to explore nodes and tracks the path using a vector<vector<int>> to store paths for each node.
Caching:

To optimize performance, the server caches the last 10 computed shortest paths. This is done using a map where each key is a pair of nodes (start, end), and the value is the corresponding shortest path.
Multithreading:

Each client request is handled in a separate thread to allow for simultaneous connections. The server listens for incoming connections, creates a new thread for each connection, and processes the request by calculating the shortest path and returning it to the client.
Thread Safety:

A mutex is used to ensure thread safety when accessing and modifying the cache of previously computed paths.
Client (client.cpp)
Connecting to Server:

The client connects to the server using a provided IP address and port number. It sends the start and end nodes to the server as a string.
Request and Response:

The client sends a request to the server containing the start and end nodes for the path calculation. The server then calculates the shortest path using BFS (or retrieves it from the cache) and sends the result back to the client.
Displaying Results:

Once the client receives the response from the server, it prints the shortest path.
