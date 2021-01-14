#include <iostream>

#include "file_manager.hpp"
#include "preprocessor.hpp"

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cout << "bad usage" << std::endl;
    }
    File source(argv[1]);
    source.read();
    Preprocessor pp(source);
    pp.run();
    //source.print();
    return 0;
}