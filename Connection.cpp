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
    std::cout << "Станция 2 ID: " << stationId2 << "\n\n";
    auto it = Pipe::pipes.find(pipeId);
       if (it != Pipe::pipes.end()) {
           const Pipe& pipe = it->second;
           std::cout << "Диаметр трубы: " << pipe.getDiameter() << " мм\n";
           std::cout << "Длина трубы: " << pipe.getLength() << " м\n";
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
                // Если обнаружен цикл
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


void Connection::connectionSubMenu() {
    std::vector<std::string> getMenuOptionsConnection = {
        "Выход",
        "Показать все соединения",
        "Добавить соединение",
        "Удаление соединение ",
        "Топологическая сортировка станций"
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
