#ifndef _READER_HPP_
#define _READER_HPP_

#include <vector>
#include <string>

class File
{
public:
    File();
    File(const char * file_path);
    File(const File &) = default;
    File &operator=(File &&) = default;
    File &operator=(const File &) = default;

    bool read();
    bool write();
    void print();

    std::string get_file_descriptor() { return static_cast <std::string> (m_fd); }

    std::string source;
private:
    const char * m_fd;
};

#include "lexer.hpp"

class PP_Token {
  public:
    enum class Type {
        hash_define,
        hash_,

        c_comment_start,
        c_comment_end,
        cpp_comment,
    };

    Token(Type t) noexcept : m_type{t} {}
    Token(Type t, const std::string s) noexcept : m_type{t}, m_lex(s) {}

    Type type() const noexcept { return m_type; }
    void type(Type t) noexcept { m_type = t; }

    std::string lex() const noexcept { return m_lex; }
    void lex(std::string lex) noexcept { m_lex = std::move(lex); }

    bool is(Type t) const noexcept { return m_type == t; }
    bool is_not(Type t) const noexcept { return m_type != t; }
    bool is_one_of(Type t1, Type t2) const noexcept { return is(t1) || is(t2); }

    template <typename... Ts> bool is_one_of(Type k1, Type k2, Ts... ks) const noexcept { return is(k1) || is_one_of(k2, ks...); }

  private:
    Type m_type;
    std::string m_lex;
};

class PP_Lexer : Lexer {
  public:
    PP_Lexer(std::string s) noexcept : m_s(s) {}

    Token next() noexcept;

  private:
    Token identifier() noexcept;
    Token number() noexcept;
    Token atom(Token::Type t) noexcept { return Token(t, m_s.substr(m_beg++, 1)); }

    char peek() const noexcept { return m_s[m_beg]; }
    char get() noexcept { return m_s[m_beg++]; }

    size_t m_beg = 0;
    std::string m_s;
};

#endif // !_READER_HPP_