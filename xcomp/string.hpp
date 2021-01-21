#ifndef STRING_HPP
#define STRING_HPP

#include "tools/lexer.hpp"

/**
 * Convert escape sequence
 * All unicode sequence are stored internally as utf-8
 * https://timsong-cpp.github.io/cppwp/lex#phases-1.5
 */
void convert_escape_sequence(Token &t);

#endif
