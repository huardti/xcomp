#include "file_manager.hpp"

#include <fstream>
#include <iostream>
#include <sstream> 

#include "error.hpp"

File::File(){
    m_fd = nullptr;
    source.clear();
}

File::File(const char * file_path) : m_fd(file_path) {
    read();
}

bool File::read(){
    source.clear();
    std::ifstream f;

    f.open(m_fd);
    if (!f.is_open()){
        ERROR(m_fd, 0,"can't open file")
        return false;
    }
    std::stringstream buffer;
    buffer << f.rdbuf();
    source = buffer.str();
    return true;
}

bool File::write(){
    std::ofstream f;
    f.open(m_fd);
    if (!f.is_open()){
        ERROR(m_fd, 0, "can't open file")
        return false;
    }
    f.write(source.c_str(), source.size()); //need test
    return true;
}

void File::print(){
    std::cout << source << std::endl;
    
}