#include <iostream>
#include <fstream>
#include <unordered_map>
#include "Pipe.h"
#include "CS.h"
#include "DataManager.h"

std::string DataManager::getFullPathFromCurrentDirectory() {
    try {
        std::string currentPath ="/Users/maximgvozdev/Gubkin_main/Gubkin_3_term/Algoritmic languages/lab_3/lab_3";
        
        std::string filename;
        std::cout << "Введите название файла: ";
        std::getline(std::cin, filename);
        
        std::string fullPath = currentPath + "/" + filename;
        
        return fullPath;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Ошибка при получении текущей директории: " << e.what() << std::endl;
        return "";
    }
}


void DataManager::saveToFile(const std::unordered_map<int, Pipe>& pipes, const std::unordered_map<int, CompressorStation>& stations, const std::string& filename) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        for (const auto& [id, pipe] : pipes) {
            if (!pipe.name.empty()) {
                outFile << "PIPE" << std::endl;
                outFile << pipe.id << std::endl;
                outFile << pipe.name << std::endl;
                outFile << pipe.length << std::endl;
                outFile << pipe.diameter << std::endl;
                outFile << (pipe.repairStatus ? "1" : "0") << std::endl;
            }
        }

        for (const auto& [id, station] : stations) {
            if (!station.name.empty()) {
                outFile << "CS" << std::endl;
                outFile << station.id << std::endl;
                outFile << station.name << std::endl;
                outFile << station.workshopNumber << std::endl;
                outFile << station.workshopNumberInWork << std::endl;
                outFile << station.efficiency << std::endl;
            }
        }

        outFile.close();
        std::cout << "Данные сохранены в файл " << filename << std::endl;
    } else {
        std::cerr << "Не удалось открыть файл для записи: " << filename << std::endl;
    }
}

void DataManager::loadFromFile(std::unordered_map<int, Pipe>& pipes, std::unordered_map<int, CompressorStation>& stations, const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "Ошибка при открытии файла для чтения: " << filename << "\n";
        return;
    }

    pipes.clear();
    stations.clear();

    std::string line;
    while (std::getline(inFile, line)) {
        if (line == "PIPE") {
            Pipe pipe;
            int id;

            inFile >> id;
            inFile.ignore();

            std::getline(inFile, pipe.name);
            inFile >> pipe.length;
            inFile >> pipe.diameter;
            inFile >> pipe.repairStatus;
            inFile.ignore();

            pipe.id = id;
            pipes[id] = pipe;
        } else if (line == "CS") {
            CompressorStation station;
            int id;

            inFile >> id;
            inFile.ignore();

            std::getline(inFile, station.name);
            inFile >> station.workshopNumber;
            inFile >> station.workshopNumberInWork;
            inFile >> station.efficiency;
            inFile.ignore();

            station.id = id;
            stations[id] = station;
        }
    }

    int maxPipeId = 0;
        for (const auto& [id, pipe] : pipes) {
            if (id > maxPipeId) {
                maxPipeId = id;
            }
        }
        Pipe::nextId = maxPipeId + 1;

        int maxStationId = 0;
        for (const auto& [id, station] : stations) {
            if (id > maxStationId) {
                maxStationId = id;
            }
        }
        CompressorStation::nextId = maxStationId + 1;

        inFile.close();
        std::cout << "Данные успешно загружены из файла: " << filename << "\n";
    
}
