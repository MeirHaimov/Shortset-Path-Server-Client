// Authors:  Meir Haimov, Adi Vaizman.
// ID's: 207910787, 212555726
#include <arpa/inet.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <thread>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <mutex>
#include <queue>
#include <sstream>
#include <fstream>
#include <cstring>
using namespace std;

int const MAX_SIZE = 1024;

vector<int> shortestPath(vector<vector<int>>& graph, int start, int end) {
    vector<int> path;
    if (start < 0 || start >= graph.size() || end < 0 || end >= graph.size()) { 
        cerr << "Error: Invalid start or end node , check your numbers and try again.\n";
        return path;
    }
    vector<bool> ifvisited(graph.size(), false);
    vector<vector<int>> paths(graph.size());
    queue<int> q;
    q.push(start);
    ifvisited[start] = true;
    while (!q.empty()) {
        int node = q.front();
        q.pop();
        if (node < 0 || node >= graph.size()) {
            cerr << "Error: Invalid node in graph, try another numbers\n";
            continue;
        }
        for (int neighbor : graph[node]) {
            if (!ifvisited[neighbor]) {
                ifvisited[neighbor] = true;
                paths[neighbor] = paths[node];
                paths[neighbor].push_back(neighbor);
                q.push(neighbor);
            }
        }
    }
    return paths[end];
}

vector<vector<int>> cgraph(const string& filename) {
    vector<vector<int>> graph;
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            long long int node1, node2;
            if (ss >> node1 >> node2) {
                while (graph.size() <= max(node1, node2)) {
                    graph.push_back({});
                }
                graph[node1].push_back(node2);
                graph[node2].push_back(node1);
            } else {
                cerr << "Invalid line in graph file, please insert different numbers and try argain! \n";
            }
        }
        file.close();
    } else {
        cerr << "Unable to open graph file, please try again!\n";
    }
    return graph;
}

mutex m;
map<pair<int, int>, vector<int>> tenLast;

void receiveClient(int fd, vector<vector<int>>& graph) {
    char buffer[MAX_SIZE];
    int reader = recv(fd, buffer, MAX_SIZE, 0);
    if (reader > 0) {
        buffer[reader] = '\0';
        int start = -1, end = -1; 
        stringstream ss(buffer);
        ss >> start >> end;

        vector<int> result;
        {
            unique_lock<mutex> lock(m);
            auto it = tenLast.find({start, end});
            if (it != tenLast.end()) {
                result = it->second;
            }
        }
        vector<int> path;
        if (result.empty()) {
            path = shortestPath(graph, start, end);
            {
                unique_lock<mutex> lock(m);
                if (tenLast.size() >= 10) {
                    tenLast.erase(tenLast.begin());
                }
                tenLast[{start, end}] = path;
            }
        } else {
            path = result;
        }

        stringstream response;
        response << start << " ";
        for (int node : path) {
            response << node << " ";
        }
        response << "\n";
        string answer = response.str();
        write(fd, answer.c_str(), answer.size());
    }
    close(fd);
}
int main(int argc, char** argv) {
    vector<vector<int>> graph = cgraph(argv[1]);
    int port = atoi(argv[2]);
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr = {0};
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    bind(fd, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(fd, 2);
    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientSize = sizeof(clientAddr);
        int clientSocket = accept(fd, (sockaddr*)&clientAddr, &clientSize);
        thread clientThread(receiveClient, clientSocket, ref(graph));
        clientThread.detach();
    }
    close(fd);
    return 0;
}
