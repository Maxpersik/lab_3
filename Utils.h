//#ifndef UTILS_H
//#define UTILS_H
//
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <limits>
//#include <ctime>
//#include <sstream>
//#include <algorithm>
//#include <cctype>
//#include <vector>
//
//
//void log(const std::string &action);
//long numberOrDefault(const std::string& command);
//template <typename T>
//T inputInRange(const std::string &prompt, T minValue, T maxValue);
//bool isNumber(const std::string& s);
//long numberInRange(const std::string& prompt, long minValue, long maxValue);
//void displayMenu(const std::vector<std::string>& options);
//template <typename T>
//T inputInRange(const std::string& prompt, T minValue, T maxValue) {
//    T value;
//    while (true) {
//        std::cout << prompt;
//        std::string input;
//        getline(std::cin, input);
//        std::stringstream ss(input);
//
//        if (ss >> value && ss.eof()) {
//            if (value >= minValue && value <= maxValue) {
//                return value;
//            } else {
//                std::cout << "Ошибка: введите число от " << minValue << " до " << maxValue << "." << std::endl;
//            }
//        } else {
//            std::cout << "Ошибка: введите корректное число." << std::endl;
//        }
//    }
//}
//
//template int inputInRange<int>(const std::string& prompt, int minValue, int maxValue);
//template double inputInRange<double>(const std::string& prompt, double minValue, double maxValue);
//
//#endif

#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <vector>

std::string getCurrentDirectory();
long numberOrDefault(const std::string& input);
bool isNumber(const std::string& s);
void displayMenu(const std::vector<std::string>& options);

template <typename T>
T inputInRange(const std::string& prompt, T minValue, T maxValue) {
    T value;
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        std::stringstream ss(input);

        if (ss >> value && ss.eof()) {
            if (value >= minValue && value <= maxValue) {
                return value;
            } else {
                std::cout << "Ошибка: введите число от " << minValue << " до " << maxValue << "." << std::endl;
            }
        } else {
            std::cout << "Ошибка: введите корректное число." << std::endl;
        }
    }
}

#endif // UTILS_H
