#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

Logger logger("log.txt");

Logger::Logger(const std::string &filename)
    : logFile(filename, std::ios::app | std::ios::out) {
    if (!logFile.is_open()) {
        std::cerr << "Не удалось открыть файл логирования: " << filename << std::endl;
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::log(const std::string &action) {
    std::lock_guard<std::mutex> guard((logMutex));  
    if (logFile.is_open()) {
        logFile << getCurrentTime() << " -- " << action << std::endl;
    } else {
        std::cerr << "Ошибка: файл логирования не открыт!" << std::endl;
    }
}

std::string Logger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime;
    localtime_r(&nowTime, &localTime);

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");  // Формат ISO 8601
    return oss.str();
}
