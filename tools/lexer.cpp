#include <cassert>
#include <limits>
#include <set>

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
    case 'u':
        if (peek(1) == '\'') {
            return prefix();
        } else if (peek(1) == '8' && peek(2) == '\'') {
            return prefix();
        } else {
            return identifier();
        }
    case 'U':
        if (peek(1) == '\'') {
            return prefix();
        } else {
            return identifier();
        }
    case 'L':
        if (peek(1) == '\'') {
            return prefix();
        } else {
            return identifier();
        }
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
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
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
    case '\\':
        return atom(Token::Type::BackSlash);
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
        return charLitteral();
    case '"':
        return atom(Token::Type::DoubleQuote);
    case '|':
        return atom(Token::Type::Pipe);
    }
}

static constexpr std::string_view basic_source_character("abcdefghijklmnopqrstuvwxyz"
                                                         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                                         "0123456789"
                                                         "_{}[]#()<>%:;.?*+-/^&|~!=,\\\"'"
                                                         " \t\v\f\n");

static bool is_octal(char c) { return c >= '0' && c <= '7'; }

static bool is_hexa(char c) {
    constexpr std::string_view hexa("0123456789abcdefABCDEF");
    return hexa.find(c) != std::string::npos;
}

std::string Lexer::escape_sequence() {
    std::string s(1, get());
    assert(s == "\\");

    char c = get();
    if (c == 'x') {
        s += 'x';
        while (is_hexa(peek())) {
            s += get();
        }
        return s;
    }

    if (c == 'u') {
        s += 'u';
        while (size(s) < 4 + 2 && is_hexa(peek())) {
            s += get();
        }

        if (size(s) != 4 + 2) {
            error("incomplete universal character name ", s);
        }
        return s;
    }

    if (c == 'U') {
        s += 'U';
        while (size(s) < 8 + 2 && is_hexa(peek())) {
            s += get();
        }

        if (size(s) != 8 + 2) {
            error("incomplete universal character name ", s);
        }
        return s;
    }

    if (is_octal(c)) {
        s += c;
        while (size(s) < 3 + 1 && is_octal(peek())) {
            s += get();
        }
        return s;
    }

    constexpr std::string_view simple_escape_sequence_letter("'\"?\\abfnrtv");
    auto i = simple_escape_sequence_letter.find(c);
    if (i != std::string::npos) {
        s += c;
        return s;
    }
    error("bad escape sequence");
}

Token Lexer::prefix() {
    char c = get();
    if (c == 'u' && peek() == '8') {
        if (peek(1) != '\'') {
            assert(0); // bad prefix
        }
        get();
        Token t(charLitteral());
        t.prefix("u8");
        return t;
    }

    std::set<char> prefix{'u', 'U', 'L'};
    if (prefix.count(c) != 0 && peek() == '\'') {
        Token t(charLitteral());
        t.prefix(std::string(1, c));
        return t;
    }
    assert(0); // bad prefix
}

Token Lexer::charLitteral() {
    assert(get() == '\'');
    std::string ch;

    char c = peek();
    while (c != '\'' && c != '\n' && basic_source_character.find(c) != std::string::npos) {
        if (c == '\\') {
            ch += escape_sequence();
        } else {
            ch += get();
        }
        c = peek();
    }

    if (get() != '\'') {
        std::cerr << "Unexpected char in char litteral, c=`" << c << "'=0x" << std::hex << static_cast<int>(c) << std::endl;
        assert(0);
    }
    return Token(Token::Type::CharLitteral, ch);
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
        "Number",      "Identifier", "LeftParen",  "RightParen",   "LeftSquare",  "RightSquare",    "LeftCurly",     "RightCurly", "LessThan",
        "GreaterThan", "Equal",      "Plus",       "Minus",        "Asterisk",    "Slash",          "backslash",     "Hash",       "Dot",
        "Comma",       "Colon",      "Semicolon",  "SingleQuote",  "DoubleQuote", "Comment",        "Pipe",          "End",        "Space",
        "tab",         "newLine",    "Unexpected", "CharLitteral", "DoubleHash",  "OpOrPunctuator", "StringLitteral"};
    return os << names[static_cast<int>(kind)];
}
