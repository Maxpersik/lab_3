#include "Connection.h"
#include "Pipe.h"
#include "CS.h"
#include "Utils.h"
#include "Logger.h"
#include "vector"
#include <stack>
#include <unordered_set>

int Connection::nextId = 1;
std::unordered_map<int, Connection> Connection::connections;

Connection::Connection(int id, int pipeId, int stationId1, int stationId2)
    : id(id), pipeId(pipeId), stationId1(stationId1), stationId2(stationId2) {
    logger.log("Создан новый объект Connection с ID: " + std::to_string(id));
}

std::unordered_map<int, std::vector<int>> Connection::adjListOut;
std::unordered_map<int, std::vector<int>> Connection::adjListIn;
std::unordered_map<int, int> Connection::pipeCapacity;  // Производительность трубы (ID трубы -> значение)

void Connection::addConnection() {
    logger.log("Добавление нового соединения.");

    int stationId1 = -1, stationId2 = -1;
    int desiredDiameter = 0;

    while (true) {
        std::cout << "Введите ID входной станции для соединения (или 0 для отмены): ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "0") {
            std::cout << "Отмена добавления соединения.\n";
            logger.log("Отмена добавления соединения пользователем.");
            return;
        }

        stationId1 = numberOrDefault(input);
        if (CompressorStation::stations.find(stationId1) == CompressorStation::stations.end()) {
            std::cout << "Станция с таким ID не найдена.\n";
            logger.log("Ошибка: Станция с ID " + std::to_string(stationId1) + " не найдена.");
            continue;
        }

        break;
    }

    while (true) {
        std::cout << "Введите ID выходной станции для соединения (или 0 для отмены): ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "0") {
            std::cout << "Отмена добавления соединения.\n";
            logger.log("Отмена добавления соединения пользователем.");
            return;
        }

        stationId2 = numberOrDefault(input);
        if (CompressorStation::stations.find(stationId2) == CompressorStation::stations.end()) {
            std::cout << "Станция с таким ID не найдена.\n";
            logger.log("Ошибка: Станция с ID " + std::to_string(stationId2) + " не найдена.");
            continue;
        }

        if (stationId1 == stationId2) {
            std::cout << "Нельзя соединить станцию саму с собой. Выберите другую станцию.\n";
            logger.log("Ошибка: Попытка соединить станцию с самой собой.");
            continue;
        }

        break;
    }

    while (true) {
        while (true) {
            std::vector<int> acceptableDiameters = {500, 700, 1000, 1400};

            std::cout << "Введите диаметр трубы для соединения (500, 700, 1000, 1400 мм) или 0 для отмены: ";
            desiredDiameter = inputInRange<int>("", 0, 1400);

            if (desiredDiameter == 0) {
                std::cout << "Отмена добавления соединения.\n";
                logger.log("Отмена добавления соединения пользователем.");
                return;
            }

            if (std::find(acceptableDiameters.begin(), acceptableDiameters.end(), desiredDiameter) == acceptableDiameters.end()) {
                std::cout << "Некорректный диаметр. Выберите из 500, 700, 1000, 1400 мм.\n";
                continue;
            }

            break;
        }

        while (true) {
            int pipeId = -1;
            for (const auto& [id, pipe] : Pipe::pipes) {
                bool pipeInUse = false;
                for (const auto& [connId, conn] : connections) {
                    if (conn.getPipeId() == id) {
                        pipeInUse = true;
                        break;
                    }
                }
                if (!pipeInUse && pipe.getDiameter() == desiredDiameter && !pipe.getStatus()) {
                    pipeId = id;
                    break;
                }
            }

            if (pipeId == -1) {
                std::cout << "Нет свободных труб с диаметром " << desiredDiameter << " мм.\n";
                std::cout << "Выберите действие:\n";
                std::cout << "1 - Создать новую трубу\n";
                std::cout << "2 - Выбрать другой диаметр\n";
                std::cout << "0 - Отмена\n";
                int choice = inputInRange<int>("", 0, 2);
                if (choice == 1) {
                    std::cout << "Создание новой трубы.\n";
                    Pipe::addPipeWithDiameter(desiredDiameter);
                    continue;
                } else if (choice == 2) {
                    break;
                } else {
                    std::cout << "Добавление соединения отменено.\n";
                    logger.log("Добавление соединения отменено пользователем при отсутствии подходящей трубы.");
                    return;
                }
            } else {

                std::cout << "Вы собираетесь создать соединение между станцией " << stationId1 << " и станцией " << stationId2
                          << " с использованием трубы " << pipeId << " (диаметр: " << desiredDiameter << " мм).\n";
                std::cout << "Подтвердите действие (1 - Да, 0 - Отмена): ";
                int confirm = inputInRange<int>("", 0, 1);
                if (confirm == 1) {
                    Connection conn(nextId++, pipeId, stationId1, stationId2);
                    connections[conn.getId()] = conn;
                    
                    adjListOut[stationId1].push_back(stationId2);
                    adjListIn[stationId2].push_back(stationId1);

                    std::cout << "Соединение успешно добавлено с ID: " << conn.getId() << std::endl;
                    logger.log("Добавлено новое соединение с ID: " + std::to_string(conn.getId()));

                    return;
                } else {
                    std::cout << "Добавление соединения отменено пользователем.\n";
                    logger.log("Добавление соединения отменено пользователем на этапе подтверждения.");
                    return;
                }
            }
        }
    }
}

void Connection::displayAllConnections() {
    logger.log("Вывод всех соединений.");
    if (connections.empty()) {
        std::cout << "Нет соединений для отображения.\n";
    } else {
        for (const auto& [id, conn] : connections) {
            conn.writeToConsole();
        }
    }
}

void Connection::deleteConnectionsWithPipe(int pipeId) {
    logger.log("Удаление соединений, связанных с трубой ID: " + std::to_string(pipeId));

    std::vector<int> connectionsToDelete;
    for (const auto& [id, conn] : connections) {
        if (conn.getPipeId() == pipeId) {
            connectionsToDelete.push_back(id);
        }
    }

    for (int id : connectionsToDelete) {
        connections.erase(id);
        logger.log("Удалено соединение с ID: " + std::to_string(id));
    }

    for (auto& [key, neighbors] : adjListOut) {
        neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), pipeId), neighbors.end());
    }

    for (auto& [key, neighbors] : adjListIn) {
        neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), pipeId), neighbors.end());
    }

    logger.log("Удаление трубы с ID: " + std::to_string(pipeId) + " завершено.");
    std::cout << "Труба и все связанные соединения успешно удалены.\n";
}

void Connection::deleteConnectionsWithStation(int stationId) {
    logger.log("Удаление соединений со станцией ID: " + std::to_string(stationId));

    std::vector<int> connectionsToDelete;
    for (const auto& [id, conn] : connections) {
        if (conn.getStationId1() == stationId || conn.getStationId2() == stationId) {
            connectionsToDelete.push_back(id);
        }
    }
    for (int id : connectionsToDelete) {
        connections.erase(id);
        logger.log("Удалено соединение с ID: " + std::to_string(id));
    }

    for (auto& [key, neighbors] : adjListOut) {
        neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), stationId), neighbors.end());
    }
    adjListOut.erase(stationId);

    for (auto& [key, neighbors] : adjListIn) {
        neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), stationId), neighbors.end());
    }
    adjListIn.erase(stationId);

    std::cout << "Станция и все связанные соединения успешно удалены.\n";
}

void Connection::writeToConsole() const {
    std::cout << "Соединение ID: " << id << "\n";
    std::cout << "Труба ID: " << pipeId << "\n";
    std::cout << "Станция 1 ID: " << stationId1 << "\n";
    std::cout << "Станция 2 ID: " << stationId2 << "\n";
    auto it = Pipe::pipes.find(pipeId);
       if (it != Pipe::pipes.end()) {
           const Pipe& pipe = it->second;
           std::cout << "Диаметр трубы: " << pipe.getDiameter() << " мм\n";
           std::cout << "Длина трубы: " << pipe.getLength() << " км\n\n";
       } else {
           std::cout << "Труба с ID " << pipeId << " не найдена.\n";
       }
}

void Connection::deleteConnection() {
    logger.log("Удаление соединения.");

    if (connections.empty()) {
        std::cout << "Нет соединений для удаления.\n";
        logger.log("Нет соединений для удаления.");
        return;
    }

    std::cout << "Список доступных соединений:\n";
    for (const auto& [id, conn] : connections) {
        std::cout << "ID соединения: " << id << ", Труба ID: " << conn.getPipeId()
                  << ", Станция 1 ID: " << conn.getStationId1()
                  << ", Станция 2 ID: " << conn.getStationId2() << "\n";
    }

    std::cout << "Введите ID соединения для удаления (или 0 для отмены): ";
    int connectionId = inputInRange<int>("", 0, nextId - 1);

    if (connectionId == 0) {
        std::cout << "Отмена удаления соединения.\n";
        logger.log("Отмена удаления соединения пользователем.");
        return;
    }

    auto it = connections.find(connectionId);
    if (it != connections.end()) {
        connections.erase(it);
        std::cout << "Соединение с ID " << connectionId << " успешно удалено.\n";
        logger.log("Соединение с ID " + std::to_string(connectionId) + " удалено.");
    } else {
        std::cout << "Соединение с таким ID не найдено.\n";
        logger.log("Соединение с ID " + std::to_string(connectionId) + " не найдено.");
    }
}

void Connection::dfsReachableFromStart(int station, const std::unordered_map<int, std::vector<int>>& adjListOut, std::unordered_set<int>& visited) {
    visited.insert(station);
    if (adjListOut.find(station) != adjListOut.end()) {
        for (int next : adjListOut.at(station)) {
            if (visited.find(next) == visited.end()) {
                dfsReachableFromStart(next, adjListOut, visited);
            }
        }
    }
}

void dfsReachableToEnd(int station, const std::unordered_map<int, std::vector<int>>& adjListIn, std::unordered_set<int>& visited) {
    visited.insert(station);
    if (adjListIn.find(station) != adjListIn.end()) {
        for (int prev : adjListIn.at(station)) {
            if (visited.find(prev) == visited.end()) {
                dfsReachableToEnd(prev, adjListIn, visited);
            }
        }
    }
}

bool topologicalSortUtil(int station,
                                const std::unordered_map<int, std::vector<int>>& adjList,
                                std::unordered_set<int>& visited,
                                std::unordered_set<int>& recursionStack,
                                std::vector<int>& result) {
    visited.insert(station);
    recursionStack.insert(station);

    if (adjList.find(station) != adjList.end()) {
        for (int next : adjList.at(station)) {
            if (visited.find(next) == visited.end()) {
                if (!topologicalSortUtil(next, adjList, visited, recursionStack, result)) {
                    return false; // если обнаружен цикл
                }
            } else if (recursionStack.find(next) != recursionStack.end()) {
                return false;
            }
        }
    }

    recursionStack.erase(station);
    result.push_back(station);
    return true;
}

void Connection::topologicalSortMenu() {
    logger.log("Вызвано меню топологической сортировки станций.");

    if (CompressorStation::stations.empty()) {
        std::cout << "Нет станций для топологической сортировки.\n";
        logger.log("Ошибка: Нет станций.");
        return;
    }

    int startStation, endStation;
    while (true) {
        std::cout << "Введите ID начальной станции (или 0 для отмены): ";
        std::string input;
        std::getline(std::cin, input);
        startStation = numberOrDefault(input);
        if (startStation == 0) {
            std::cout << "Отмена топологической сортировки.\n";
            logger.log("Отмена топологической сортировки пользователем.");
            return;
        }
        if (CompressorStation::stations.find(startStation) == CompressorStation::stations.end()) {
            std::cout << "Станция не найдена. Повторите ввод.\n";
            continue;
        }
        break;
    }

    while (true) {
        std::cout << "Введите ID конечной станции (или 0 для отмены): ";
        std::string input;
        std::getline(std::cin, input);
        endStation = numberOrDefault(input);
        if (endStation == 0) {
            std::cout << "Отмена топологической сортировки.\n";
            logger.log("Отмена топологической сортировки пользователем.");
            return;
        }
        if (CompressorStation::stations.find(endStation) == CompressorStation::stations.end()) {
            std::cout << "Станция не найдена. Повторите ввод.\n";
            continue;
        }
        break;
    }

    topologicalSort(startStation, endStation);
}

void Connection::topologicalSort(int startStation, int endStation) {
    logger.log("Запуск топологической сортировки со станцией начала: " + std::to_string(startStation) +
               " и станцией конца: " + std::to_string(endStation));

    std::unordered_set<int> reachableFromStart;
    dfsReachableFromStart(startStation, adjListOut, reachableFromStart);

    std::unordered_set<int> reachableToEnd;
    dfsReachableToEnd(endStation, adjListIn, reachableToEnd);

    std::unordered_set<int> subgraphStations;
    for (int st : reachableFromStart) {
        if (reachableToEnd.find(st) != reachableToEnd.end()) {
            subgraphStations.insert(st);
        }
    }

    if (subgraphStations.empty()) {
        std::cout << "Не найдено путей от станции " << startStation << " к станции " << endStation << ".\n";
        logger.log("Топологическая сортировка: Пути не найдены.");
        return;
    }

    std::unordered_map<int, std::vector<int>> subgraph;
    for (int st : subgraphStations) {
        if (adjListOut.find(st) != adjListOut.end()) {
            std::vector<int> filtered;
            for (int nxt : adjListOut.at(st)) {
                if (subgraphStations.find(nxt) != subgraphStations.end()) {
                    filtered.push_back(nxt);
                }
            }
            if (!filtered.empty()) {
                subgraph[st] = filtered;
            }
        }
    }

    std::unordered_set<int> visited;
    std::unordered_set<int> recursionStack;
    std::vector<int> result;

    for (int st : subgraphStations) {
        if (visited.find(st) == visited.end()) {
            if (!topologicalSortUtil(st, subgraph, visited, recursionStack, result)) {
                std::cout << "Топологическая сортировка невозможна, обнаружен цикл в выбранном подграфе.\n";
                logger.log("Топологическая сортировка: обнаружен цикл.");
                return;
            }
        }
    }

    std::reverse(result.begin(), result.end());

    std::cout << "Топологически отсортированный порядок станций (подграф от " << startStation << " до " << endStation << "):\n";
    for (int st : result) {
        std::cout << st << " ";
    }
    std::cout << "\n";

    logger.log("Топологическая сортировка успешно выполнена.");
}

double calculatePipePerformance(const Pipe& pipe) {
    if (pipe.getStatus()) {
        return 0.0;
    }

    double diameter = pipe.getDiameter();
    double length = pipe.getLength();

    if (length == 0) {
        throw std::runtime_error("Длина трубы не может быть равна 0.");
    }

    return std::sqrt(std::pow(diameter, 5) / length) * 1000;
}

double Connection::fordFulkerson(int source, int sink) {
    std::unordered_map<int, std::unordered_map<int, double>> residualCapacity;

    for (const auto& [connectionId, conn] : connections) {
        int pipeId = conn.getPipeId();
        int startStation = conn.getStationId1();
        int endStation = conn.getStationId2();

        auto pipeIt = Pipe::pipes.find(pipeId);
        if (pipeIt != Pipe::pipes.end() && !pipeIt->second.getStatus()) {
            double diameterInMeters = pipeIt->second.getDiameter() / 1000.0;
            double lengthInMeters = pipeIt->second.getLength() * 1000.0; // Преобразуем длину в метры

            double capacity = (sqrt(pow(diameterInMeters, 5)) / lengthInMeters) * 10.0;

            if (capacity > 0) {
                residualCapacity[startStation][endStation] = capacity;
                std::cout << "Added capacity from " << startStation
                          << " to " << endStation << ": " << capacity << std::endl;
            }
        }
    }

    double maxFlow = 0.0;
    std::unordered_map<int, int> parent;

    auto bfs = [&](int s, int t) -> bool {
        std::unordered_set<int> visited;
        std::queue<int> queue;
        queue.push(s);
        visited.insert(s);
        parent.clear();

        while (!queue.empty()) {
            int u = queue.front();
            queue.pop();

            for (const auto& [v, cap] : residualCapacity[u]) {
                if (cap > 0 && visited.find(v) == visited.end()) {
                    queue.push(v);
                    visited.insert(v);
                    parent[v] = u;

                    if (v == t) return true;
                }
            }
        }
        return false;
    };

    while (bfs(source, sink)) {
        double pathFlow = std::numeric_limits<double>::max();

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            pathFlow = std::min(pathFlow, residualCapacity[u][v]);
        }

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            residualCapacity[u][v] -= pathFlow;
            residualCapacity[v][u] += pathFlow;
        }

        maxFlow += pathFlow;
    }

    std::cout << "Max Flow: " << std::fixed << std::setprecision(6) << maxFlow << std::endl;
    return maxFlow;
}

int Connection::findPipeByStations(int station1, int station2) {
    for (const auto& [connectionId, conn] : connections) {
        if ((conn.getStationId1() == station1 && conn.getStationId2() == station2) ||
            (conn.getStationId1() == station2 && conn.getStationId2() == station1)) {
            return conn.getPipeId();
        }
    }
    throw std::runtime_error("Труба между указанными станциями не найдена");
}

std::vector<int> Connection::dijkstra(int start, int end) {
    std::unordered_map<int, double> dist;
    std::unordered_map<int, int> prev;
    std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<>> pq;

    for (const auto& station : CompressorStation::stations) {
        dist[station.first] = std::numeric_limits<double>::infinity();
    }
    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (u == end) break;

        for (int v : adjListOut[u]) {
            int pipeId = findPipeByStations(u, v);
            const auto& pipe = Pipe::pipes.at(pipeId);

            double weight = pipe.getStatus() ? std::numeric_limits<double>::infinity() : pipe.getLength();

            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                prev[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    std::vector<int> path;
    if (dist[end] == std::numeric_limits<double>::infinity()) {
        std::cout << "Путь от станции " << start << " к станции " << end << " недоступен.\n";
        return path;
    }

    for (int at = end; at != start; at = prev[at]) {
        path.push_back(at);
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());

    std::cout << "Общая длина пути: " << dist[end] << " км\n";

    return path;
}

void Connection::handleMaxFlow() {
    int minStation = 1;
    int maxStation = CompressorStation::stations.size();

    int source = inputInRange<int>("Введите начальную станцию: ", minStation, maxStation);
    int sink = inputInRange<int>("Введите конечную станцию: ", minStation, maxStation);


    try {
        double maxFlow = fordFulkerson(source, sink);
        std::cout << "Максимальный поток: " << maxFlow << "\n";
    } catch (const std::exception& e) {
        std::cout << "Ошибка при расчете максимального потока: " << e.what() << "\n";
    }
}

void Connection::handleShortestPath() {
    //int start, end;
    int minStation = 1;
    int maxStation = CompressorStation::stations.size();

    int start = inputInRange<int>("Введите начальную станцию: ", minStation, maxStation);
    int end = inputInRange<int>("Введите конечную станцию: ", minStation, maxStation);


    try {
        auto path = dijkstra(start, end);
        if (path.empty()) {
            std::cout << "Путь не найден.\n";
        } else {
            std::cout << "Кратчайший путь: ";
            for (int station : path) std::cout << station << " ";
            std::cout << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "Ошибка при расчете кратчайшего пути: " << e.what() << "\n";
    }
}



void Connection::connectionSubMenu() {
    std::vector<std::string> getMenuOptionsConnection = {
           "Выход",
           "Показать все соединения",
           "Добавить соединение",
           "Удалить соединение",
           "Топологическая сортировка станций",
           "Рассчитать максимальный поток",
           "Рассчитать кратчайший путь"
       };

    std::string command;
    long value;
    logger.log("Вход в подменю соединений.");
    while (true) {
        displayMenu(getMenuOptionsConnection);
        std::getline(std::cin, command);
        value = numberOrDefault(command);

        switch (value) {
            case 1:
                displayAllConnections();
                break;
            case 2:
                addConnection();
                break;
            case 3:
                deleteConnection();
                break;
            case 4:
                logger.log("Запуск топологической сортировки станций.");
                topologicalSortMenu();
               break;
            case 5:
                handleMaxFlow();
                break;
            case 6:
                handleShortestPath();
                break;
            case 0:
                logger.log("Выход из подменю соединений.");
                std::cout << "Выход из меню работы с соединениями.\n";
                return;
            default:
                logger.log("Ошибка: Неверный выбор в меню соединений.");
                std::cout << "Неверный выбор. Попробуйте снова.\n";
                break;
        }
    }
}
