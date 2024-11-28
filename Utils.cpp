#include "Utils.h"

using namespace std;


std::string getCurrentDirectory() {
    try {
        return std::filesystem::current_path().string();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Ошибка при получении текущей директории: " << e.what() << std::endl;
        return "";
    }
}


bool isNumber(const std::string& s)
{
    return  s.length() < 2 and !s.empty() && std::find_if(s.begin(),
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

long numberOrDefault(const string& command){
    if (isNumber(command)) {
        return strtol(command.c_str(), NULL, 10);
    }
    return -1;
}

void displayMenu(const std::vector<std::string>& options) {
    std::cout << "\nМеню:\n";
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << i << " - " << options[i] << '\n';
    }
    std::cout << "Выберите команду: ";
}
