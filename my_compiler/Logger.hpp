#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <vector>
#include <fstream>

namespace Logger {
class Logger {
    std::vector<std::string> log_list;
    std::string file_name;
    std::ofstream log_stream;
    
public: 
    Logger(std::string _file_name) : file_name(_file_name) {
        log_stream.open(file_name);
    }
    ~Logger(){
        log_stream.close();
    }
    
    void LOG(std::string message) {
        log_stream << message << '\n';
    }

};
}
#endif //LOGGER_HPP