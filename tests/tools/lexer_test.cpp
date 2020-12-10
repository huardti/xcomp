#include <gtest/gtest.h>

#include "tools/lexer.hpp"

class TokenTest : public ::testing::Test
{
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }

};

TEST_F(TokenTest, is_true)
{
    Token t(Token::Type::Number);
    EXPECT_TRUE(t.is(Token::Type::Number));
}

TEST_F(TokenTest, is_false)
{
    Token t(Token::Type::Number);
    EXPECT_FALSE(t.is(Token::Type::Space));
}

TEST_F(TokenTest, is_not_true)
{
    Token t(Token::Type::Number);
    EXPECT_TRUE(t.is_not(Token::Type::Space));
}

TEST_F(TokenTest, is_not_false)
{
    Token t(Token::Type::Number);
    EXPECT_FALSE(t.is_not(Token::Type::Number));
}

TEST_F(TokenTest, is_one_of_true)
{
    Token t(Token::Type::Number);
    EXPECT_TRUE(t.is_one_of(Token::Type::Space, Token::Type::Number));
}

TEST_F(TokenTest, is_one_of_false)
{
    Token t(Token::Type::Number);
    EXPECT_FALSE(t.is_one_of(Token::Type::Space, Token::Type::Slash));
}

TEST_F(TokenTest, is_one_of_multiple)
{
    Token t(Token::Type::Number);
    EXPECT_TRUE(t.is_one_of(
        Token::Type::Space,
        Token::Type::Slash,
        Token::Type::Number,
        Token::Type::Minus
    ));
}

TEST_F(TokenTest, is_one_of_multiple_first)
{
    Token t(Token::Type::Number);
    EXPECT_TRUE(t.is_one_of(
        Token::Type::Number,
        Token::Type::Space,
        Token::Type::Slash,
        Token::Type::Minus
    ));
}

TEST_F(TokenTest, is_one_of_multiple_false)
{
    Token t(Token::Type::Number);
    EXPECT_FALSE(t.is_one_of(
        Token::Type::Space,
        Token::Type::Slash,
        Token::Type::Minus
    ));
}

TEST_F(TokenTest, type)
{
    Token t(Token::Type::Number);
    EXPECT_EQ(t.type(), Token::Type::Number);
}

TEST_F(TokenTest, type_setter)
{
    Token t(Token::Type::Number);
    t.type(Token::Type::Slash);
    EXPECT_EQ(t.type(), Token::Type::Slash);
}

TEST_F(TokenTest, lex)
{
    Token t(Token::Type::Number, "yo");
    EXPECT_EQ(t.lex(), "yo");
}

TEST_F(TokenTest, lex_setter)
{
    Token t(Token::Type::Number, "yo");
    t.lex("hello");
    EXPECT_EQ(t.lex(), "hello");
}
