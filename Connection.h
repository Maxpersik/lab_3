#ifndef CONNECTION_H
#define CONNECTION_H

#include <iostream>
#include <unordered_map>
#include <stack>
#include <unordered_set>

class Connection {
private:
    int id;
    int pipeId;
    int stationId1;
    int stationId2;

public:
    static int nextId;
    static std::unordered_map<int, Connection> connections;

    Connection() = default;
    Connection(int id, int pipeId, int stationId1, int stationId2);

    static void addConnection();
    static void deleteConnection();
    static void displayAllConnections();
    
    static void dfsReachableFromStart(int station, const std::unordered_map<int, std::vector<int>>& adjListOut, std::unordered_set<int>& visited);
    

    static void deleteConnectionsWithPipe(int pipeId);
    static void deleteConnectionsWithStation(int stationId);
    static void topologicalSort(int startStation, int endStation);

    int getId() const { return id; }
    int getPipeId() const { return pipeId; }
    int getStationId1() const { return stationId1; }
    int getStationId2() const { return stationId2; }
    
    static std::unordered_map<int, std::vector<int>> adjListOut;
    static std::unordered_map<int, std::vector<int>> adjListIn; 
    static std::unordered_map<int, int> pipeCapacity;
    
    static int findPipeByStations(int station1, int station2);
    static void handleMaxFlow();
    static void handleShortestPath();


    void writeToConsole() const;
    static void topologicalSortMenu();
    static double fordFulkerson(int source, int sink);
    static std::vector<int> dijkstra(int start, int end);

    static void connectionSubMenu();
};

static void dfsReachableToEnd(int station, const std::unordered_map<int, std::vector<int>>& adjListIn, std::unordered_set<int>& visited);
bool topologicalSortUtil(int station,
                                const std::unordered_map<int, std::vector<int>>& adjList,
                                std::unordered_set<int>& visited,
                                std::unordered_set<int>& recursionStack,
                         std::vector<int>& result);

#endif // CONNECTION_H
