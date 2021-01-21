#include <cassert>
#include <codecvt>
#include <limits>
#include <locale>

#include "tools/lexer.hpp"

#include "string.hpp"

static bool is_octal(char c) { return c >= '0' && c <= '7'; }

static bool is_hexa(char c) {
    constexpr std::string_view hexa("0123456789abcdefABCDEF");
    return hexa.find(c) != std::string::npos;
}

/**
 * https://timsong-cpp.github.io/cppwp/lex#charset-2
 */
static bool is_valid_ucs(long n) { return n >= 0 && n <= 0x10FFFF && (n < 0xD800 || n > 0xDFFF); }

/**
 * https://en.wikipedia.org/wiki/UTF-8#Encoding
 */
static std::string ucs_to_utf8(long n) {
    if (!is_valid_ucs(n)) {
        fatal("invalid unicode sequence");
    }

    if (n <= 0x7F) {
        return std::string(1, static_cast<char>(n) & 0x7F);
    }
    if (n <= 0x7FF) {
        unsigned char c[2];
        c[1] = static_cast<unsigned char>(0x80 | (n & 0x3F));
        n >>= 6;
        c[0] = static_cast<unsigned char>(0xC0 | (n & 0x1F));
        return std::string(reinterpret_cast<const char *>(c), 2);
    }
    if (n <= 0xFFFF) {
        unsigned char c[3];
        c[2] = static_cast<unsigned char>(0x80 | (n & 0x3F));
        n >>= 6;
        c[1] = static_cast<unsigned char>(0x80 | (n & 0x3F));
        n >>= 6;
        c[0] = static_cast<unsigned char>(0xE0 | (n & 0xF));
        return std::string(reinterpret_cast<const char *>(c), 3);
    }

    unsigned char c[4];
    c[3] = static_cast<unsigned char>(0x80 | (n & 0x3F));
    n >>= 6;
    c[2] = static_cast<unsigned char>(0x80 | (n & 0x3F));
    n >>= 6;
    c[1] = static_cast<unsigned char>(0x80 | (n & 0x3F));
    n >>= 6;
    c[0] = static_cast<unsigned char>(0xF0 | (n & 0x7));
    return std::string(reinterpret_cast<const char *>(c), 4);
}

using StrInt = std::tuple<std::string, size_t>;

static StrInt get_unicode(const std::string &seq, size_t i, size_t l) {
    std::string s;
    while (size(s) < l) {
        assert(is_hexa(seq[i]));
        s += seq[i];
        i++;
    }

    long n = std::stol(s, nullptr, 16);
    return {ucs_to_utf8(n), i};
}

/**
 * Convert and return a 16bits unicode sequence
 */
static StrInt get_unicode_u(const std::string &seq, size_t i) { return get_unicode(seq, i, 4); }

/**
 * Convert and return a 32bits unicode sequence
 */
static StrInt get_unicode_U(const std::string &seq, size_t i) { return get_unicode(seq, i, 8); }

static bool is_too_long_for_prefix(const std::string &prefix, size_t n) {
    return ((prefix == "" || prefix == "u8") && n > 2) || (prefix == "u" && n > 4) || ((prefix == "U" || prefix == "L") && n > 8);
}

/**
 * Convert and return an hexa sequence
 */
static StrInt get_hexa(const std::string &seq, const std::string &prefix, size_t i) {
    std::string s;
    while (is_hexa(seq[i])) {
        s += seq[i];
        i++;
    }

    if (is_too_long_for_prefix(prefix, size(s))) {
        fatal("hex escape sequence out of range");
    }
    long n = std::stol(s, nullptr, 16);
    return {ucs_to_utf8(n), i};
}

/**
 * Convert and return an octal sequence
 */
static StrInt get_octal(const std::string &seq, const std::string &prefix, size_t i) {
    std::string s;
    while (is_octal(seq[i])) {
        s += seq[i];
        i++;
    }
    if (size(s) > 3) {
        fatal("octal escape sequence out of range");
    }

    long n = std::stol(s, nullptr, 8);
    if (prefix == "u8" && n > std::numeric_limits<unsigned char>::max()) {
        fatal("octal escape sequence out of range");
    }
    return {ucs_to_utf8(n), i};
}

/**
 * Convert and return the first character found in `seq` with prefix `prefix`
 * Return the converted character and its original length
 */
static StrInt get_one_escape_sequence(const std::string &seq, const std::string &prefix) {
    size_t i(0);
    if (seq[i] != '\\') {
        return {seq.substr(0, 1), 1}; // ordinary character literal
    }

    i++; // '\\'
    char c = seq[i];
    if (c == 'x') {
        i++;
        return get_hexa(seq, prefix, i);
    }
    if (c == 'u') {
        i++;
        return get_unicode_u(seq, i);
    }
    if (c == 'U') {
        i++;
        return get_unicode_U(seq, i);
    }
    if (is_octal(c)) {
        return get_octal(seq, prefix, i);
    }

    constexpr std::string_view simple_escape_sequence_letter("'\"?\\abfnrtv");
    constexpr std::string_view simple_escaped_sequence_letter("\'\"\?\\\a\b\f\n\r\t\v");
    auto id = simple_escape_sequence_letter.find(c);
    if (id != std::string::npos) {
        return {std::string(1, simple_escaped_sequence_letter[id]), 2};
    }
    fatal("bad escape sequence");
}

/**
 * Convert escape sequence of a CharLitteral
 */
static std::string char_escape_sequence(const Token &t) {
    assert(t.is(Token::Type::CharLitteral));
    auto [c, n] = get_one_escape_sequence(t.lex(), t.prefix());
    if (size(t.lex()) != n) {
        fatal("multicharacter literal are not supported");
    }
    return c;
}

/**
 * Convert escape sequence of a StringLitteral
 */
static std::string string_escape_sequence(const Token &t) {
    assert(t.is(Token::Type::StringLitteral));
    std::string out;
    size_t i(0);

    while (i < size(t.lex())) {
        auto [c, n] = get_one_escape_sequence(t.lex().substr(i), t.prefix());
        out += c;
        i += n;
    }
    return out;
}

void convert_escape_sequence(Token &t) {
    if (t.is(Token::Type::CharLitteral)) {
        t.lex(char_escape_sequence(t));
        return;
    }
    if (t.is(Token::Type::StringLitteral)) {
        t.lex(string_escape_sequence(t));
        return;
    }
}
