#include<iostream>

#include "reader.hpp"
#include "preprocessor.hpp"

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cout << "bad usage" << std::endl;
    }
    file source(argv[1]);
    source.read();
    preprocessor pp(source);
    return 0;
}