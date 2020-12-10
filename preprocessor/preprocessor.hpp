#ifndef _PREPROCESSOR_HPP_
#define _PREPROCESSOR_HPP_
#include <string>
#include <vector>
#include <map>
#include "reader.hpp"

typedef struct def {
    std::string s;
    size_t line;
}def;

class preprocessor {
public:
    preprocessor(file f);

    void run();
private:
    std::vector<std::string> m_in;
    std::vector<std::string> m_out;

    std::map<std::string, def> m_define;
};

#endif // !_PREPROCESSOR_HPP_



/*
#include <fstream>
#include <iostream>
#include <sstream> //std::stringstream
int main() {
    std::ifstream inFile;
    inFile.open("inFileName"); //open the input file

    std::stringstream strStream;
    strStream << inFile.rdbuf(); //read the file
    std::string str = strStream.str(); //str holds the content of the file

    std::cout << str << "\n"; //you can do anything with the string!!!
}
*/