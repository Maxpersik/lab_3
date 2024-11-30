#include "Connection.h"
#include "Pipe.h"
#include "CS.h"
#include "Utils.h"
#include "Logger.h"
#include "vector"

int Connection::nextId = 1;
std::unordered_map<int, Connection> Connection::connections;

Connection::Connection(int id, int pipeId, int stationId1, int stationId2)
    : id(id), pipeId(pipeId), stationId1(stationId1), stationId2(stationId2) {
    logger.log("Создан новый объект Connection с ID: " + std::to_string(id));
}

void Connection::addConnection() {
    logger.log("Добавление нового соединения.");

    int stationId1 = -1, stationId2 = -1;
    int desiredDiameter = 0;

    // Выбор входной станции
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

        // Первая станция успешно выбрана
        break;
    }

    // Выбор выходной станции
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

        // Вторая станция успешно выбрана
        break;
    }

    while (true) {
        // Выбор диаметра трубы
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

            // Диаметр успешно выбран
            break;
        }

        while (true) {
            // Поиск свободной трубы с указанным диаметром
            int pipeId = -1;
            for (const auto& [id, pipe] : Pipe::pipes) {
                // Проверяем, что труба имеет нужный диаметр и не используется в соединении
                bool pipeInUse = false;
                for (const auto& [connId, conn] : connections) {
                    if (conn.getPipeId() == id) {
                        pipeInUse = true;
                        break;
                    }
                }
                if (!pipeInUse && pipe.getDiameter() == desiredDiameter) {
                    pipeId = id;
                    break;
                }
            }

            if (pipeId == -1) {
                // Если не нашли подходящую трубу, предлагаем варианты
                std::cout << "Нет свободных труб с диаметром " << desiredDiameter << " мм.\n";
                std::cout << "Выберите действие:\n";
                std::cout << "1 - Создать новую трубу\n";
                std::cout << "2 - Выбрать другой диаметр\n";
                std::cout << "0 - Отмена\n";
                int choice = inputInRange<int>("", 0, 2);
                if (choice == 1) {
                    // Создание новой трубы
                    std::cout << "Создание новой трубы.\n";
                    Pipe::addPipe();
                    // После создания новой трубы, проверяем наличие трубы с нужным диаметром
                    continue; // Повторяем поиск трубы
                } else if (choice == 2) {
                    // Возвращаемся к выбору диаметра
                    break; // Выходим из внутреннего цикла, возвращаемся к выбору диаметра
                } else {
                    // Отмена операции
                    std::cout << "Добавление соединения отменено.\n";
                    logger.log("Добавление соединения отменено пользователем при отсутствии подходящей трубы.");
                    return;
                }
            } else {
                // Труба найдена, переходим к подтверждению
                // Подтверждение создания соединения
                std::cout << "Вы собираетесь создать соединение между станцией " << stationId1 << " и станцией " << stationId2
                          << " с использованием трубы " << pipeId << " (диаметр: " << desiredDiameter << " мм).\n";
                std::cout << "Подтвердите действие (1 - Да, 0 - Отмена): ";
                int confirm = inputInRange<int>("", 0, 1);
                if (confirm == 1) {
                    // Создание соединения
                    Connection conn(nextId++, pipeId, stationId1, stationId2);
                    connections[conn.getId()] = conn;

                    std::cout << "Соединение успешно добавлено с ID: " << conn.getId() << std::endl;
                    logger.log("Добавлено новое соединение с ID: " + std::to_string(conn.getId()));

                    // Выход из функции после успешного добавления
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
    logger.log("Удаление соединений с трубой ID: " + std::to_string(pipeId));
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
}

void Connection::writeToConsole() const {
    std::cout << "Соединение ID: " << id << "\n";
    std::cout << "Труба ID: " << pipeId << "\n";
    std::cout << "Станция 1 ID: " << stationId1 << "\n";
    std::cout << "Станция 2 ID: " << stationId2 << "\n\n";
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

void Connection::topologicalSort() {
    // Построение списка смежности из соединений
    std::unordered_map<int, std::vector<int>> adjList; // Ключ: ID станции, значение: список смежных станций

    // Инициализация списка смежности для всех станций
    for (const auto& [id, station] : CompressorStation::stations) {
        adjList[id] = std::vector<int>();
    }

    // Заполнение списка смежности на основе соединений
    for (const auto& [connId, conn] : connections) {
        int fromStation = conn.getStationId1(); // Исходная станция
        int toStation = conn.getStationId2();   // Конечная станция
        adjList[fromStation].push_back(toStation);
    }

    // Переменные для отслеживания посещенных станций и стека рекурсии
    std::unordered_map<int, bool> visited;
    std::unordered_map<int, bool> onStack;
    std::stack<int> sortedStations;
    bool hasCycle = false;

    // Определение рекурсивной функции DFS
    std::function<void(int)> dfs = [&](int stationId) {
        if (hasCycle) return;
        visited[stationId] = true;
        onStack[stationId] = true;

        for (int neighbor : adjList[stationId]) {
            if (!visited[neighbor]) {
                dfs(neighbor);
            } else if (onStack[neighbor]) {
                hasCycle = true;
                return;
            }
        }

        onStack[stationId] = false;
        sortedStations.push(stationId);
    };

    // Выполнение DFS из каждой непосещенной станции
    for (const auto& [id, station] : CompressorStation::stations) {
        if (!visited[id]) {
            dfs(id);
            if (hasCycle) break;
        }
    }

    // Вывод результатов
    if (hasCycle) {
        std::cout << "Граф содержит цикл. Топологическая сортировка невозможна.\n";
        logger.log("Топологическая сортировка не выполнена из-за наличия цикла в графе.");
    } else {
        std::cout << "Топологическая сортировка станций:\n";
        logger.log("Выполнена топологическая сортировка станций.");
        while (!sortedStations.empty()) {
            int stationId = sortedStations.top();
            sortedStations.pop();
            std::cout << "Станция ID: " << stationId << ", Название: " << CompressorStation::stations[stationId].getName() << "\n";
        }
    }
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
               topologicalSort();
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
