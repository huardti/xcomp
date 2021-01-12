#include <algorithm>
#include <cassert>
#include <limits>
#include <set>
#include <vector>

#include "lexer.hpp"

static bool is_digit(char c) noexcept { return c >= '0' && c <= '9'; }

static bool is_non_digit(char c) noexcept {
    constexpr std::string_view id("abcdefghijklmnopqrstuvwxyz"
                                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ_");
    return id.find(c) != std::string::npos;
}

static bool is_identifier_char(char c) noexcept { return is_digit(c) || is_non_digit(c); }

static bool is_quote(char c) { return c == '\'' || c == '"'; }

Token Lexer::next() noexcept {
    char c = peek();
    switch (c) {
    case '\0':
        return atom(Token::Type::End);
    case '\\':
        error("stray '\\' in program");
    case 'u':
        if (is_quote(peek(1)) || (peek(1) == 'R' && peek(2) == '"') || (peek(1) == '8' && is_quote(peek(2))) ||
            (peek(1) == '8' && peek(2) == 'R' && peek(3) == '"')) {
            return prefix();
        }
        return identifier();
    case 'U':
    case 'L':
        if (is_quote(peek(1)) || (peek(1) == 'R' && peek(2) == '"')) {
            return prefix();
        }
        return identifier();
    case 'R':
        if (peek(1) == '"') {
            return raw_string();
        }
        return identifier();
    case ' ':
    case '\f':
    case '\v':
    case '\t':
        return atom(Token::Type::Space);
    case '\n':
        return atom(Token::Type::Newline);
    default:
        break; // continue the function
    }

    if (is_quote(c)) {
        return get_litteral();
    }

    if (is_non_digit(c)) {
        return identifier();
    }

    if (is_digit(c)) {
        return number();
    }

    constexpr std::string_view special("{}[]#()<>%:;.?*+-/^&|~!=,\\\"'");
    if (special.find(c) != std::string::npos) {
        return handle_special();
    }

    warning("unknown char ", c);
    return atom(Token::Type::Unexpected);
}

Token Lexer::get_operator(Token::Type t, const std::string &lex) {
    const std::vector<std::string> alterative{"<%",  "%>",    "<:",     ":>",     "%:",    "%:%:",   "and", "bitor", "or",
                                              "xor", "compl", "bitand", "and_eq", "or_eq", "xor_eq", "not", "not_eq"};
    const std::vector<std::string> real{"{", "}", "[", "]", "#", "##", "&&", "|", "||", "^", "~", "&", "&=", "|=", "^=", "!", "!="};
    Token tok(atom(t, lex));

    auto it = std::find(alterative.begin(), alterative.end(), lex);
    if (it != alterative.end()) {
        size_t i = static_cast<size_t>(std::distance(alterative.begin(), it));
        tok.lex(real[i]);
    }
    return tok;
}

static bool in_vector(const std::string &s, const std::vector<std::string> &v) { return std::find(v.begin(), v.end(), s) != v.end(); }

Token Lexer::handle_special() noexcept {
    std::string s(1, peek());
    if (peek(1) != '\0') {
        s += peek(1);
        if (peek(2) != '\0') {
            s += peek(2);
            s += peek(3);
        }
    }

    if (peek() == '<' && peek(1) == ':' && peek(2) == ':' && peek(3) != ':' && peek(3) != '>') {
        return get_operator(Token::Type::OpOrPunctuator, s.substr(0, 1));
    }

    if (s.substr(0, 4) == "%:%:") {
        return get_operator(Token::Type::PreprocessingOperator, s.substr(0, 4));
    }

    const std::vector<std::string> t3{"...", "->*", "<=>", "<<=", ">>="};
    if (in_vector(s.substr(0, 3), t3)) {
        return get_operator(Token::Type::OpOrPunctuator, s.substr(0, 3));
    }

    if (s.substr(0, 2) == "##" || s.substr(0, 2) == "%:") {
        return get_operator(Token::Type::PreprocessingOperator, s.substr(0, 2));
    }
    const std::vector<std::string> t2{"<:", ":>", "<%", "%>", "::", ".*", "->", "+=", "-=", "*=", "/=", "%=", "^=",
                                      "&=", "|=", "==", "!=", "<=", ">=", "&&", "||", "<<", ">>", "++", "--"};
    if (in_vector(s.substr(0, 2), t2)) {
        return get_operator(Token::Type::OpOrPunctuator, s.substr(0, 2));
    }

    if (s[0] == '#') {
        return get_operator(Token::Type::PreprocessingOperator, s.substr(0, 1));
    }
    return get_operator(Token::Type::OpOrPunctuator, s.substr(0, 1));
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
        get(); // '8'
        assert(is_quote(peek()) || (peek() == 'R' && peek(1) == '"'));

        Token t(Token::Type::Unexpected);
        if (peek() == 'R') {
            t = raw_string();
        } else {
            t = get_litteral();
        }
        t.prefix("u8");
        return t;
    }

    std::set<char> prefix{'u', 'U', 'L'};
    assert(prefix.count(c) != 0);

    if (peek() == 'R') {
        assert(peek(1) == '"');
        Token t(raw_string());
        t.prefix(std::string(1, c));
        return t;
    }
    assert(is_quote(peek()));

    Token t(get_litteral());
    t.prefix(std::string(1, c));
    return t;
}

Token Lexer::get_litteral() {
    char q = get();
    assert(is_quote(q));
    std::string ch;

    char c = peek();
    while (c != q && c != '\n' && basic_source_character.find(c) != std::string::npos) {
        if (c == '\\') {
            ch += escape_sequence();
        } else {
            ch += get();
        }
        c = peek();
    }

    if (get() != q) {
        error("Unexpected char in char|string litteral, c=`", c, "'=0x", std::hex, static_cast<int>(c));
    }

    if (q == '\'') {
        return Token(Token::Type::CharLitteral, ch);
    } else {
        return Token(Token::Type::StringLitteral, ch);
    }
}

/**
 * https://timsong-cpp.github.io/cppwp/lex#nt:d-char
 */
static bool is_d_char(char c) {
    constexpr std::string_view except(" ()\\\t\v\f\n");
    return basic_source_character.find(c) != std::string::npos && except.find(c) == std::string::npos;
}

bool Lexer::is_r_char(char c, const std::string &d) const {
    if (basic_source_character.find(c) == std::string::npos) {
        return false;
    }
    if (c != ')') {
        return true;
    }

    std::string d2;
    size_t i(1);
    while (size(d2) < size(d)) {
        d2 += peek(i);
        ++i;
    }
    if (d2 != d) {
        return true;
    }
    return peek(i) != '"';
}

#define D_CHAR_SIZE_MAX 16

Token Lexer::raw_string() {
    char tmp = get();
    assert(tmp == 'R');
    tmp = get();
    assert(tmp == '"');

    std::string d;
    while (size(d) < D_CHAR_SIZE_MAX && is_d_char(peek())) {
        d += get();
    }

    if (size(d) >= D_CHAR_SIZE_MAX) {
        error("raw string delimiter longer than ", D_CHAR_SIZE_MAX, " characters");
    }
    if (size(d) > 0 && peek() != '(') {
        error("invalid '", peek(), "' in raw string delimiter");
    }
    get(); // '('

    std::string r;
    while (is_r_char(peek(), d)) {
        r += get();
    }

    if (get() != ')') {
        error("raw string missing terminating parenthese or bad delimiter");
    }

    std::string d2;
    while (size(d2) < size(d)) {
        d2 += get();
    }
    assert(d2 == d);
    tmp = get();
    assert(tmp == '"');

    Token t(Token::Type::StringLitteral, r);
    t.raw(true);
    return t;
}

Token Lexer::identifier() noexcept {
    std::string s;
    while (is_identifier_char(peek())) {
        s += get();
    }
    return Token(Token::Type::Identifier, s);
}

Token Lexer::number() noexcept {
    std::string s;
    while (is_digit(peek())) {
        s += get();
    }
    return Token(Token::Type::Number, s);
}

std::ostream &operator<<(std::ostream &os, const Token::Type &kind) {
    const std::vector<std::string> names{"CharLitteral",          "End",   "Identifier",     "Newline",   "Number", "OpOrPunctuator",
                                         "PreprocessingOperator", "Space", "StringLitteral", "Unexpected"};
    return os << names[static_cast<size_t>(kind)];
}
