#include <iostream>
#include "Pipe.h"
#include "CS.h"
#include "DataManager.h"
#include "Utils.h"
#include "Logger.h"
#include "Connection.h"


std::vector<std::string> getMenuOptions = {
    "Выход",
    "Меню трубы",
    "Меню станции",
    "Меню соединений",
    "Показать все трубы и станции",
    "Сохранить в файл",
    "Загрузить из файла"
};

int main() {
    std::string command;
    long value;
    std::string filename;
    Logger logger("/Users/maximgvozdev/Gubkin_main/Gubkin_3_term/Algoritmic languages/lab_3/lab_3/log.txt");

    while (true) {
        displayMenu(getMenuOptions);
        getline(std::cin, command);

        value = numberOrDefault(command);

        switch (value) {
            case 1:
                logger.log("Вход в меню трубы.");
                Pipe::pipeSubMenu();
                break;
            case 2:
                logger.log("Вход в меню станции.");
                CompressorStation::stationSubMenu();
                break;
            case 3:
                logger.log("Вход в меню соединений.");
                Connection::connectionSubMenu();
                break;
            case 4:
                logger.log("Показ всех труб и станций.");
                CompressorStation::displayAll();
                Pipe::displayAll();
                break;
            case 5:
                filename = DataManager::getFullPathFromCurrentDirectory();
                logger.log("Сохранение данных в файл: " + filename);
                DataManager::saveToFile(Pipe::pipes, CompressorStation::stations, Connection::connections, filename);
                break;
            case 6:
                filename = DataManager::getFullPathFromCurrentDirectory();
                logger.log("Загрузка данных из файла: " + filename);
                DataManager::loadFromFile(Pipe::pipes, CompressorStation::stations, Connection::connections,filename);
                break;
            case 0:
                logger.log("Программа завершена.");
                std::cout << "Выход из программы.\n";
                return 0;
            default:
                logger.log("Неверный выбор: " + command);
                std::cout << "Неверный выбор. Попробуйте снова.\n";
                break;
        }
    }
}
