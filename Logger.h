#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>
#include <mutex>

class Logger {
public:
    explicit Logger(const std::string &filename);
    
    ~Logger();

    void log(const std::string &action);

private:
    std::ofstream logFile;
    std::mutex logMutex;

    std::string getCurrentTime();
};

extern Logger logger;

#endif // LOGGER_H
