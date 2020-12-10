#include "preprocessor.hpp"
#include <string>
#include <vector>
#include <iostream>

#include <sstream>

std::vector<std::string> split(const std::string &chaine, char delimiteur) {
 std::vector<std::string> elements;
std::stringstream ss(chaine);
 std::string sousChaine;
 while (getline(ss, sousChaine, delimiteur)) {
    elements.push_back(sousChaine);
 }
 return elements;
}

preprocessor::preprocessor(file f): m_in(f.source) {
    std::cout << " nb line of inputfile " << m_in.size() << std::endl;
    for (size_t i = 0; i < m_in.size(); i++) {
        
        std::string s = m_in[i]; // iteration ligne par ligne
        std::vector<std::string> splited = split(s, ' ');
        for (size_t j = 0; j < splited.size(); j++) {
            std::cout << splited[j] << std::endl;
            if (splited[j][0] == '#') {
                std::cout << "find hash command" << std::endl;
                
            }
        }
    }
}