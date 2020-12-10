#include "reader.hpp"

#include <fstream>
#include <iostream>
#include <string_view>

file::file(const char * file_path) : m_fd(file_path) {
    read();
}

void file::read(){
    source.clear();
    std::string line;
        std::ifstream f;

    f.open (m_fd);
    if (!f.is_open()){
        std::cout << "can't open file" << std::endl;
        
    }
    while (std::getline(f, line)) {
        source.push_back(line);
    }
}

void file::print(){
    for (size_t i = 0; i < source.size(); i++) {
        std::cout << source[i] << std::endl;
    }
}