#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <string>

#include "error.hpp"

class Token {
  public:
    enum class Type {
        CharLitteral,
        End,
        Identifier,
        Newline,
        Number,
        OpOrPunctuator,
        PreprocessingOperator,
        Space,
        StringLitteral,
        Unexpected,
    };

    explicit Token(Type t) noexcept : m_type{t} {}
    Token(Type t, const std::string s) noexcept : m_type{t}, m_lex(s) {}

    Type type() const noexcept { return m_type; }
    void type(Type t) noexcept { m_type = t; }

    std::string lex() const noexcept { return m_lex; }
    void lex(std::string lex) noexcept { m_lex = std::move(lex); }

    std::string prefix() const noexcept { return m_prefix; }
    void prefix(std::string prefix) noexcept { m_prefix = std::move(prefix); }

    bool raw() const noexcept { return m_raw; }
    void raw(bool raw) noexcept { m_raw = raw; }

    bool is(Type t) const noexcept { return m_type == t; }

    template <typename... T> bool is_one_of(T... t) const noexcept { return (is(t) || ...); }

  private:
    Type m_type;
    std::string m_lex;
    std::string m_prefix = "";
    bool m_raw = false;
};

std::ostream &operator<<(std::ostream &os, const Token::Type &kind);

class Lexer {
  public:
    explicit Lexer(std::string s) noexcept : m_s(s) {}

    Token next() noexcept;

  private:
    /**
     * Read and return an escape sequence
     */
    std::string escape_sequence();

    /**
     * Read and return a string or char with its prefix
     */
    Token prefix();

    /**
     *  Read and return a CharLitteral or a StringLitteral (not raw) with it content
     */
    Token get_litteral();

    /**
     * https://timsong-cpp.github.io/cppwp/lex#nt:r-char
     */
    bool is_r_char(char c, const std::string &d) const;

    /**
     * Read and return a StringLitteral which is a raw string
     */
    Token raw_string();
    Token identifier() noexcept;
    Token number() noexcept;

    /**
     * If it's an alterative token, convert it
     * https://timsong-cpp.github.io/cppwp/lex#digraph-2
     */
    Token get_operator(Token::Type t, const std::string &lex);

    /**
     * Read and return an OpOrPunctuator
     */
    Token handle_special() noexcept;

    Token atom(Token::Type t) noexcept { return atom(t, std::string(1, peek())); }
    Token atom(Token::Type t, const std::string &lex) noexcept {
        Token tok(t, lex);
        m_beg += size(lex);
        return tok;
    }

    char peek(size_t i = 0) const noexcept {
        if (m_beg + i > size(m_s)) {
            fatal("Unexpected end of file");
        }
        return m_s[m_beg + i];
    }
    char get() noexcept { return m_s[m_beg++]; }

    size_t m_beg = 0;
    std::string m_s;
};

#endif // !LEXER_HPP
