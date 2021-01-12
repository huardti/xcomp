#include <gtest/gtest.h>

#include "tools/lexer.hpp"

class TokenTest : public ::testing::Test {
  protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(TokenTest, is_true) {
    Token t(Token::Type::Number);
    EXPECT_TRUE(t.is(Token::Type::Number));
}

TEST_F(TokenTest, is_false) {
    Token t(Token::Type::Number);
    EXPECT_FALSE(t.is(Token::Type::Space));
}

TEST_F(TokenTest, is_not_true) {
    Token t(Token::Type::Number);
    EXPECT_TRUE(t.is_not(Token::Type::Space));
}

TEST_F(TokenTest, is_not_false) {
    Token t(Token::Type::Number);
    EXPECT_FALSE(t.is_not(Token::Type::Number));
}

TEST_F(TokenTest, is_one_of_true) {
    Token t(Token::Type::Number);
    EXPECT_TRUE(t.is_one_of(Token::Type::Space, Token::Type::Number));
}

TEST_F(TokenTest, is_one_of_false) {
    Token t(Token::Type::Number);
    EXPECT_FALSE(t.is_one_of(Token::Type::Space, Token::Type::Slash));
}

TEST_F(TokenTest, is_one_of_multiple) {
    Token t(Token::Type::Number);
    EXPECT_TRUE(t.is_one_of(Token::Type::Space, Token::Type::Slash, Token::Type::Number, Token::Type::Minus));
}

TEST_F(TokenTest, is_one_of_multiple_first) {
    Token t(Token::Type::Number);
    EXPECT_TRUE(t.is_one_of(Token::Type::Number, Token::Type::Space, Token::Type::Slash, Token::Type::Minus));
}

TEST_F(TokenTest, is_one_of_multiple_false) {
    Token t(Token::Type::Number);
    EXPECT_FALSE(t.is_one_of(Token::Type::Space, Token::Type::Slash, Token::Type::Minus));
}

TEST_F(TokenTest, type) {
    Token t(Token::Type::Number);
    EXPECT_EQ(t.type(), Token::Type::Number);
}

TEST_F(TokenTest, type_setter) {
    Token t(Token::Type::Number);
    t.type(Token::Type::Slash);
    EXPECT_EQ(t.type(), Token::Type::Slash);
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

class LexerTest : public ::testing::Test {
  protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(LexerTest, next) {
    Lexer l("a");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
}

TEST_F(LexerTest, next_end) {
    Lexer l("");
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, next_unknown) {
    Lexer l("\xf4");
    EXPECT_EQ(l.next().type(), Token::Type::Unexpected);
}

TEST_F(LexerTest, next_tab) {
    Lexer l("\ta");
    EXPECT_EQ(l.next().type(), Token::Type::Tab);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
}

TEST_F(LexerTest, next_newline) {
    Lexer l("a\na");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Newline);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
}

TEST_F(LexerTest, next_greater_than) {
    Lexer l("4>5");
    EXPECT_EQ(l.next().type(), Token::Type::Number);
    EXPECT_EQ(l.next().type(), Token::Type::GreaterThan);
    EXPECT_EQ(l.next().type(), Token::Type::Number);
}

TEST_F(LexerTest, next_less_than) {
    Lexer l("4<5");
    EXPECT_EQ(l.next().type(), Token::Type::Number);
    EXPECT_EQ(l.next().type(), Token::Type::LessThan);
    EXPECT_EQ(l.next().type(), Token::Type::Number);
}

TEST_F(LexerTest, next_less_equal) {
    Lexer l("4<=5");
    EXPECT_EQ(l.next().type(), Token::Type::Number);
    EXPECT_EQ(l.next().type(), Token::Type::LessThan);
    EXPECT_EQ(l.next().type(), Token::Type::Equal);
    EXPECT_EQ(l.next().type(), Token::Type::Number);
}

TEST_F(LexerTest, next_plus) {
    Lexer l("4+5");
    EXPECT_EQ(l.next().type(), Token::Type::Number);
    EXPECT_EQ(l.next().type(), Token::Type::Plus);
    EXPECT_EQ(l.next().type(), Token::Type::Number);
}

TEST_F(LexerTest, next_minus) {
    Lexer l("4-5");
    EXPECT_EQ(l.next().type(), Token::Type::Number);
    EXPECT_EQ(l.next().type(), Token::Type::Minus);
    EXPECT_EQ(l.next().type(), Token::Type::Number);
}

TEST_F(LexerTest, next_times) {
    Lexer l("4*5");
    EXPECT_EQ(l.next().type(), Token::Type::Number);
    EXPECT_EQ(l.next().type(), Token::Type::Asterisk);
    EXPECT_EQ(l.next().type(), Token::Type::Number);
}

TEST_F(LexerTest, next_div) {
    Lexer l("4/5");
    EXPECT_EQ(l.next().type(), Token::Type::Number);
    EXPECT_EQ(l.next().type(), Token::Type::Slash);
    EXPECT_EQ(l.next().type(), Token::Type::Number);
}

TEST_F(LexerTest, next_or_binary) {
    Lexer l("1|0");
    EXPECT_EQ(l.next().type(), Token::Type::Number);
    EXPECT_EQ(l.next().type(), Token::Type::Pipe);
    EXPECT_EQ(l.next().type(), Token::Type::Number);
}

TEST_F(LexerTest, next_include) {
    Lexer l("#include <string>");
    EXPECT_EQ(l.next().type(), Token::Type::Hash);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Space);
    EXPECT_EQ(l.next().type(), Token::Type::LessThan);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::GreaterThan);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, next_include_quotes) {
    Lexer l("#include \"file.hpp\"");
    EXPECT_EQ(l.next().type(), Token::Type::Hash);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Space);
    EXPECT_EQ(l.next().type(), Token::Type::DoubleQuote);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Dot);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::DoubleQuote);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, next_variable_declaration) {
    Lexer l("int a;");

    Token t(l.next());
    EXPECT_EQ(t.lex(), "int");
    EXPECT_EQ(t.type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Space);
    t = l.next();
    EXPECT_EQ(t.lex(), "a");
    EXPECT_EQ(t.type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, next_variable_affectation) {
    Lexer l("int a=4;");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Space);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Equal);
    EXPECT_EQ(l.next().type(), Token::Type::Number);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, next_2_variables) {
    Lexer l("int a,b;");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Space);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Comma);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, next_variable_affectation_float) {
    Lexer l("float a=44.6;");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Space);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Equal);
    Token t(l.next());
    EXPECT_EQ(t.lex(), "44");
    EXPECT_EQ(t.type(), Token::Type::Number);
    EXPECT_EQ(l.next().type(), Token::Type::Dot);
    t = l.next();
    EXPECT_EQ(t.lex(), "6");
    EXPECT_EQ(t.type(), Token::Type::Number);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, next_function_prototype) {
    Lexer l("int function();");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Space);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::LeftParen);
    EXPECT_EQ(l.next().type(), Token::Type::RightParen);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, next_function_declaration) {
    Lexer l("int function() {}");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Space);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::LeftParen);
    EXPECT_EQ(l.next().type(), Token::Type::RightParen);
    EXPECT_EQ(l.next().type(), Token::Type::Space);
    EXPECT_EQ(l.next().type(), Token::Type::LeftCurly);
    EXPECT_EQ(l.next().type(), Token::Type::RightCurly);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, next_array_declaration) {
    Lexer l("int a[6];");

    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Space);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::LeftSquare);
    EXPECT_EQ(l.next().type(), Token::Type::Number);
    EXPECT_EQ(l.next().type(), Token::Type::RightSquare);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, next_label) {
    Lexer l("a:");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::Colon);
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

TEST_F(LexerTest, double_hash) {
    Lexer l("a##b");
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::DoubleHash);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, CharLitteral) {
    Lexer l("='a';");
    EXPECT_EQ(l.next().type(), Token::Type::Equal);
    Token t(l.next());
    EXPECT_EQ(t.lex(), "a");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, CharLitteral_multi) {
    Lexer l("'ab';");
    Token t(l.next());
    EXPECT_EQ(t.lex(), "ab");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, CharLitteral_new_line) {
    Lexer l(R"('\n';)");
    Token t(l.next());
    EXPECT_EQ(t.lex(), R"(\n)");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, CharLitteral_hexa) {
    Lexer l(R"(='\x7m';)");
    EXPECT_EQ(l.next().type(), Token::Type::Equal);
    Token t(l.next());
    EXPECT_EQ(t.lex(), R"(\x7m)");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, CharLitteral_hexa_multiple) {
    Lexer l(R"(='\x41m';)");
    EXPECT_EQ(l.next().type(), Token::Type::Equal);
    Token t(l.next());
    EXPECT_EQ(t.lex(), R"(\x41m)");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, CharLitteral_octal) {
    Lexer l(R"(='\7m';)");
    EXPECT_EQ(l.next().type(), Token::Type::Equal);
    Token t(l.next());
    EXPECT_EQ(t.lex(), R"(\7m)");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, CharLitteral_octal_multiple) {
    Lexer l(R"(='\100m';)");
    EXPECT_EQ(l.next().type(), Token::Type::Equal);
    Token t(l.next());
    EXPECT_EQ(t.lex(), R"(\100m)");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, CharLitteral_u) {
    Lexer l(R"(='\u0436m';)");
    EXPECT_EQ(l.next().type(), Token::Type::Equal);
    Token t(l.next());
    EXPECT_EQ(t.lex(), R"(\u0436m)");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, CharLitteral_U) {
    Lexer l(R"(='\U0001F996m';)");
    EXPECT_EQ(l.next().type(), Token::Type::Equal);
    Token t(l.next());
    EXPECT_EQ(t.lex(), R"(\U0001F996m)");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, CharLitteral_prefix_u) {
    Lexer l("=u'a';");
    EXPECT_EQ(l.next().type(), Token::Type::Equal);
    Token t(l.next());
    EXPECT_EQ(t.prefix(), "u");
    EXPECT_EQ(t.lex(), "a");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, CharLitteral_prefix_u8) {
    Lexer l("=u8'a';");
    EXPECT_EQ(l.next().type(), Token::Type::Equal);
    Token t(l.next());
    EXPECT_EQ(t.prefix(), "u8");
    EXPECT_EQ(t.lex(), "a");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, CharLitteral_prefix_U) {
    Lexer l("=U'a';");
    EXPECT_EQ(l.next().type(), Token::Type::Equal);
    Token t(l.next());
    EXPECT_EQ(t.prefix(), "U");
    EXPECT_EQ(t.lex(), "a");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, CharLitteral_prefix_L) {
    Lexer l("=L'a';");
    EXPECT_EQ(l.next().type(), Token::Type::Equal);
    Token t(l.next());
    EXPECT_EQ(t.prefix(), "L");
    EXPECT_EQ(t.lex(), "a");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}

TEST_F(LexerTest, CharLitteral_prefix_other) {
    Lexer l("=d'a';");
    EXPECT_EQ(l.next().type(), Token::Type::Equal);
    EXPECT_EQ(l.next().type(), Token::Type::Identifier);
    Token t(l.next());
    EXPECT_EQ(t.prefix(), "");
    EXPECT_EQ(t.lex(), "a");
    EXPECT_EQ(t.type(), Token::Type::CharLitteral);
    EXPECT_EQ(l.next().type(), Token::Type::Semicolon);
    EXPECT_EQ(l.next().type(), Token::Type::End);
}
