#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <string>

#include "error.hpp"

class Token {
  public:
    enum class Type {
        Number,
        Identifier,
        LeftParen,
        RightParen,
        LeftSquare,
        RightSquare,
        LeftCurly,
        RightCurly,
        LessThan,
        GreaterThan,
        Equal,
        Plus,
        Minus,
        Asterisk,
        Slash,
        BackSlash,
        Hash,
        Dot,
        Comma,
        Colon,
        Semicolon,
        SingleQuote,
        DoubleQuote,
        Comment,
        Pipe,
        End,
        Space,
        Tab,
        Newline,
        Unexpected,
        CharLitteral,
        DoubleHash,
        OpOrPunctuator,
        StringLitteral
    };

    explicit Token(Type t) noexcept : m_type{t} {}
    Token(Type t, const std::string s) noexcept : m_type{t}, m_lex(s) {}

    Type type() const noexcept { return m_type; }
    void type(Type t) noexcept { m_type = t; }

    std::string lex() const noexcept { return m_lex; }
    void lex(std::string lex) noexcept { m_lex = std::move(lex); }

    std::string prefix() const noexcept { return m_prefix; }
    void prefix(std::string prefix) noexcept { m_prefix = std::move(prefix); }

    bool is(Type t) const noexcept { return m_type == t; }

    template <typename... T> bool is_one_of(T... t) const noexcept { return (is(t) || ...); }

  private:
    Type m_type;
    std::string m_lex;
    std::string m_prefix = "";
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
     *  Read and return a CharLitteral with it content
     */
    Token charLitteral();
    Token identifier() noexcept;
    Token number() noexcept;
    Token atom(Token::Type t) noexcept { return Token(t, m_s.substr(m_beg++, 1)); }

    char peek(size_t i = 0) const noexcept {
        if (m_beg + i > size(m_s)) {
            error("Unexpected end of file");
        }
        return m_s[m_beg + i];
    }
    char get() noexcept { return m_s[m_beg++]; }

    size_t m_beg = 0;
    std::string m_s;
};

#endif // !LEXER_HPP
