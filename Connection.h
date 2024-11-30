#ifndef CONNECTION_H
#define CONNECTION_H

#include <iostream>
#include <unordered_map>

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

    static void deleteConnectionsWithPipe(int pipeId);
    static void deleteConnectionsWithStation(int stationId);
    static void topologicalSort();

    int getId() const { return id; }
    int getPipeId() const { return pipeId; }
    int getStationId1() const { return stationId1; }
    int getStationId2() const { return stationId2; }

    void writeToConsole() const;
    static void connectionSubMenu();
};

#endif // CONNECTION_H
