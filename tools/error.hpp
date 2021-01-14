#ifndef ERROR_HPP
#define ERROR_HPP

#include <iostream>

constexpr std::string_view normal = "\x1b[0m";
constexpr std::string_view purple = "\x1b[1;35m";
constexpr std::string_view red = "\x1b[1;31m";

/**
 * Print args to stderr, then exit
 */
template <typename... T> [[noreturn]] void fatal(T... t) {
    std::cerr << red << "error: " << normal;
    (std::cerr << ... << t);
    std::cerr << std::endl;
    exit(EXIT_FAILURE);
}

/**
 * Print args to stderr
 */
template <typename... T> void error(T... t) {
    std::cerr << red << "error: " << normal;
    (std::cerr << ... << t);
    std::cerr << std::endl;
}

/**
 * Print args to stderr
 */
template <typename... T> void warning(T... t) {
    std::cerr << purple << "warning: " << normal;
    (std::cerr << ... << t);
    std::cerr << std::endl;
}

#endif
