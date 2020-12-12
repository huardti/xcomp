#include "lexer.hpp"

bool is_digit(char c) noexcept {
    switch (c) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return true;
    default:
        return false;
    }
}

bool is_identifier_char(char c) noexcept {
    switch (c) {
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '_':
        return true;
    default:
        return false;
    }
}

Token Lexer::next() noexcept {
    char c = peek();
    switch (c) {
    case '\0':
        return Token(Token::Type::End, m_s.substr(m_beg, 1));
    default:
        std::cout << "unknow char" << c << std::endl;
        return atom(Token::Type::Unexpected);
    case ' ':
        return atom(Token::Type::Space);
    case '\t':
        return atom(Token::Type::Tab);
    case '\n':
        return atom(Token::Type::Newline);
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
        return identifier();
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return number();
    case '(':
        return atom(Token::Type::LeftParen);
    case ')':
        return atom(Token::Type::RightParen);
    case '[':
        return atom(Token::Type::LeftSquare);
    case ']':
        return atom(Token::Type::RightSquare);
    case '{':
        return atom(Token::Type::LeftCurly);
    case '}':
        return atom(Token::Type::RightCurly);
    case '<':
        return atom(Token::Type::LessThan);
    case '>':
        return atom(Token::Type::GreaterThan);
    case '=':
        return atom(Token::Type::Equal);
    case '+':
        return atom(Token::Type::Plus);
    case '-':
        return atom(Token::Type::Minus);
    case '*':
        return atom(Token::Type::Asterisk);
    case '/':
        return atom(Token::Type::Slash);
    case '#':
        return atom(Token::Type::Hash);
    case '.':
        return atom(Token::Type::Dot);
    case ',':
        return atom(Token::Type::Comma);
    case ':':
        return atom(Token::Type::Colon);
    case ';':
        return atom(Token::Type::Semicolon);
    case '\'':
        return atom(Token::Type::SingleQuote);
    case '"':
        return atom(Token::Type::DoubleQuote);
    case '|':
        return atom(Token::Type::Pipe);
    }
}

Token Lexer::identifier() noexcept {
    int start = m_beg;
    std::string r;
    get();
    while (is_identifier_char(peek())) {
        get();
    }
    return Token(Token::Type::Identifier, m_s.substr(start, m_beg - start));
}

Token Lexer::number() noexcept {
    int start = m_beg;
    get();
    while (is_digit(peek()))
        get();
    return Token(Token::Type::Number, m_s.substr(start, m_beg - start));
}

std::ostream &operator<<(std::ostream &os, const Token::Type &kind) {
    static const char *const names[]{
        "Number",      "Identifier",  "LeftParen", "RightParen", "LeftSquare", "RightSquare", "LeftCurly", "RightCurly", "LessThan",   "GreaterThan",
        "Equal",       "Plus",        "Minus",     "Asterisk",   "Slash",      "Hash",        "Dot",       "Comma",      "Colon",      "Semicolon",
        "SingleQuote", "DoubleQuote", "Comment",   "Pipe",       "End",        "Space",       "tab",       "newLine",    "Unexpected",
    };
    return os << names[static_cast<int>(kind)];
}
