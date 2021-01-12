#include <gtest/gtest.h>

#include "tools/lexer.hpp"

class TokenTest : public ::testing::Test {};

class FakeBuffer : public std::streambuf {
  public:
    std::string get() { return s; }
    int overflow(int c) override {
        s += static_cast<char>(c);
        return c;
    }

  private:
    std::string s = "";
};

TEST_F(TokenTest, stream_operator) {
    FakeBuffer fake_buffer;
    std::ostream fake_stream(&fake_buffer);

    fake_stream << Token::Type::OpOrPunctuator;
    EXPECT_EQ(fake_buffer.get(), "OpOrPunctuator");
}

TEST_F(TokenTest, is_true) {
    Token t(Token::Type::Number);
    EXPECT_TRUE(t.is(Token::Type::Number));
}

TEST_F(TokenTest, is_false) {
    Token t(Token::Type::Number);
    EXPECT_FALSE(t.is(Token::Type::Space));
}

TEST_F(TokenTest, is_one_of_true) {
    Token t(Token::Type::Number);
    EXPECT_TRUE(t.is_one_of(Token::Type::Space, Token::Type::Number));
}

TEST_F(TokenTest, is_one_of_false) {
    Token t(Token::Type::Number);
    EXPECT_FALSE(t.is_one_of(Token::Type::Space, Token::Type::CharLitteral));
}

TEST_F(TokenTest, is_one_of_multiple) {
    Token t(Token::Type::Number);
    EXPECT_TRUE(t.is_one_of(Token::Type::Space, Token::Type::CharLitteral, Token::Type::Number, Token::Type::OpOrPunctuator));
}

TEST_F(TokenTest, is_one_of_multiple_first) {
    Token t(Token::Type::Number);
    EXPECT_TRUE(t.is_one_of(Token::Type::Number, Token::Type::Space, Token::Type::CharLitteral, Token::Type::OpOrPunctuator));
}

TEST_F(TokenTest, is_one_of_multiple_false) {
    Token t(Token::Type::Number);
    EXPECT_FALSE(t.is_one_of(Token::Type::Space, Token::Type::CharLitteral, Token::Type::OpOrPunctuator));
}

TEST_F(TokenTest, type) {
    Token t(Token::Type::Number);
    EXPECT_EQ(t.type(), Token::Type::Number);
}

TEST_F(TokenTest, type_setter) {
    Token t(Token::Type::Number);
    t.type(Token::Type::CharLitteral);
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
}

TEST_F(TokenTest, lex) {
    Token t(Token::Type::Number, "yo");
    EXPECT_EQ(t.lex(), "yo");
}

TEST_F(TokenTest, lex_setter) {
    Token t(Token::Type::Number, "yo");
    t.lex("hello");
    EXPECT_EQ(t.lex(), "hello");
}

class LexerTest : public ::testing::Test {};

using LexerDeathTest = LexerTest;

TEST_F(LexerDeathTest, end_of_file) {
    Lexer l("");
    EXPECT_EQ(l.next().type(), Token::Type::End);
    EXPECT_DEATH(l.next(), "Unexpected end of file");
}

TEST_F(LexerDeathTest, backslash) {
    Lexer l("\\");
    EXPECT_DEATH(l.next(), "stray");
}

class GenerateDeathTest : public testing::TestWithParam<std::string> {};

const std::vector<std::string> bad_sequence{
    "'\\s'",          "'\\u123'",       "'\\U12345'",   "'\\$'",    "'e\n'", "'e\xff'", "R\"ccccccccccccccccc(b)a\"",
    "R\"c\xff(b)a\"", "R\"c(b\xff)a\"", "R\"cc(b)ca\"", "R\"c(b)ce"};

TEST_P(GenerateDeathTest, bad_sequence) {
    Lexer l(GetParam());
    EXPECT_DEATH(l.next(), "error");
}

INSTANTIATE_TEST_SUITE_P(bad_sequence, GenerateDeathTest, testing::ValuesIn(bad_sequence));

TEST_F(LexerTest, next) {
    Lexer l("a");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
}

TEST_F(LexerTest, unknown) {
    Lexer l("\xf4");
    EXPECT_EQ(l.next().type(), Token::Type::Unexpected);
}

TEST_F(LexerTest, tab) {
    Lexer l("\ta");
    EXPECT_EQ(l.next().type(), Token::Type::Space);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
}

TEST_F(LexerTest, newline) {
    Lexer l("a\na");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Newline);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
}

TEST_F(LexerTest, include) {
    Lexer l("#include <string>");
    EXPECT_EQ(l.next().type(), Token::Type::PreprocessingOperator);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Space);
    EXPECT_EQ(l.next().type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, include_quotes) {
    Lexer l("#include \"file.hpp\"");
    EXPECT_EQ(l.next().type(), Token::Type::PreprocessingOperator);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Space);
    EXPECT_EQ(l.next().type(), Token::Type::StringLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, array_declaration) {
    Lexer l("int a[6];");

    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Space);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::Number);
    EXPECT_EQ(l.next().type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, plus_plus) {
    Lexer l("x+++++y");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    Token t(l.next());
    EXPECT_EQ(t.lex(), "++");
    EXPECT_EQ(t.type(), Token::Type::OpOrPunctuator);
    t = l.next();
    EXPECT_EQ(t.lex(), "++");
    EXPECT_EQ(t.type(), Token::Type::OpOrPunctuator);
    t = l.next();
    EXPECT_EQ(t.lex(), "+");
    EXPECT_EQ(t.type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, dot_dot) {
    Lexer l("a..y");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    Token t(l.next());
    EXPECT_EQ(t.lex(), ".");
    EXPECT_EQ(t.type(), Token::Type::OpOrPunctuator);
    t = l.next();
    EXPECT_EQ(t.lex(), ".");
    EXPECT_EQ(t.type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, modulo) {
    Lexer l("a%:%b");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    Token t(l.next());
    EXPECT_EQ(t.type(), Token::Type::PreprocessingOperator);
    t = l.next();
    EXPECT_EQ(t.lex(), "%");
    EXPECT_EQ(t.type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, particular_case1) {
    Lexer l("a<:::b");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    Token t(l.next());
    EXPECT_EQ(t.lex(), "[");
    EXPECT_EQ(t.type(), Token::Type::OpOrPunctuator);
    t = l.next();
    EXPECT_EQ(t.lex(), "::");
    EXPECT_EQ(t.type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, particular_case2) {
    Lexer l("a<::>b");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    Token t(l.next());
    EXPECT_EQ(t.lex(), "[");
    EXPECT_EQ(t.type(), Token::Type::OpOrPunctuator);
    t = l.next();
    EXPECT_EQ(t.lex(), "]");
    EXPECT_EQ(t.type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, particular_case3) {
    Lexer l("a<::b");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    Token t(l.next());
    EXPECT_EQ(t.lex(), "<");
    EXPECT_EQ(t.type(), Token::Type::OpOrPunctuator);
    t = l.next();
    EXPECT_EQ(t.lex(), "::");
    EXPECT_EQ(t.type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, particular_case4) {
    Lexer l("a<:b");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    Token t(l.next());
    EXPECT_EQ(t.lex(), "[");
    EXPECT_EQ(t.type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

class GenerateTest5 : public testing::TestWithParam<std::string> {};

const std::vector<std::string> CharLitteral{"a", "ab", "\\n", "\\x7", "\\x41", "\\7", "\\100", "\\u0436", "\\U0001F996"};

TEST_P(GenerateTest5, CharLitteral) {
    std::string s("'" + GetParam() + "m'");
    Lexer l(s);
    Token t(l.next());
    EXPECT_EQ(t.lex(), GetParam() + "m");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

INSTANTIATE_TEST_SUITE_P(CharLitteral, GenerateTest5, testing::ValuesIn(CharLitteral));

class GenerateTest2 : public testing::TestWithParam<std::string> {};

const std::vector<std::string> prefix{"u", "u8", "U", "L"};

TEST_P(GenerateTest2, CharLitteral_prefix) {
    std::string s(GetParam());
    s += "'a'";
    Lexer l(s);
    Token t(l.next());
    EXPECT_EQ(t.prefix(), GetParam());
    EXPECT_EQ(t.lex(), "a");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_P(GenerateTest2, StringLitteral_prefix) {
    std::string s(GetParam());
    s += "\"a\"";
    Lexer l(s);
    Token t(l.next());
    EXPECT_EQ(t.prefix(), GetParam());
    EXPECT_EQ(t.lex(), "a");
    EXPECT_EQ(t.type(), Token::Type::StringLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_P(GenerateTest2, StringLitteral_raw_prefix) {
    std::string s(GetParam());
    s += "R\"cee(ab)cee\";";
    Lexer l(s);
    Token t(l.next());
    EXPECT_EQ(t.prefix(), GetParam());
    EXPECT_EQ(t.lex(), "ab");
    EXPECT_TRUE(t.raw());
    EXPECT_EQ(t.type(), Token::Type::StringLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

INSTANTIATE_TEST_SUITE_P(prefix, GenerateTest2, testing::ValuesIn(prefix));

TEST_F(LexerTest, CharLitteral_prefix_other) {
    Lexer l("=d'a';");
    EXPECT_EQ(l.next().type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    Token t(l.next());
    EXPECT_EQ(t.prefix(), "");
    EXPECT_EQ(t.lex(), "a");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

class GenerateTest3 : public testing::TestWithParam<std::string> {};

const std::vector<std::string> prefix_id{"ua", "u8b", "Lc", "Ud", "Re", "uRa", "u8Rb", "LRc", "URd"};

TEST_P(GenerateTest3, prefix_id) {
    Lexer l(GetParam());
    Token t(l.next());
    EXPECT_EQ(t.prefix(), "");
    EXPECT_EQ(t.lex(), GetParam());
    EXPECT_FALSE(t.raw());
    EXPECT_EQ(t.type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

INSTANTIATE_TEST_SUITE_P(prefix_id, GenerateTest3, testing::ValuesIn(prefix_id));

class GenerateTest4 : public testing::TestWithParam<std::string> {};

const std::vector<std::string> raw_delim{"a", ")a", "d)cee"};

TEST_P(GenerateTest4, StringLitteral_raw_delim) {
    std::string s;
    s = "=R\"cee(" + GetParam() + ")cee\";";
    Lexer l(s);
    EXPECT_EQ(l.next().type(), Token::Type::OpOrPunctuator);
    Token t(l.next());
    EXPECT_EQ(t.prefix(), "");
    EXPECT_EQ(t.lex(), GetParam());
    EXPECT_TRUE(t.raw());
    EXPECT_EQ(t.type(), Token::Type::StringLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

INSTANTIATE_TEST_SUITE_P(raw_delim, GenerateTest4, testing::ValuesIn(raw_delim));

TEST_F(LexerTest, StringLitteral_raw) {
    Lexer l("=R\"(a)\";");
    EXPECT_EQ(l.next().type(), Token::Type::OpOrPunctuator);
    Token t(l.next());
    EXPECT_EQ(t.prefix(), "");
    EXPECT_EQ(t.lex(), "a");
    EXPECT_TRUE(t.raw());
    EXPECT_EQ(t.type(), Token::Type::StringLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::OpOrPunctuator);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}
