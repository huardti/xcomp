#include <clocale>

#include <gtest/gtest.h>

#include "xcomp/string.hpp"

class SequenceTest : public ::testing::Test {};

TEST_F(SequenceTest, ignore) {
    Token t(Token::Type::Number, "45");
    convert_escape_sequence(t);
    EXPECT_EQ(t.lex(), "45");
}

class GenerateTest : public testing::TestWithParam<std::string> {};

const std::vector<std::string> prefix{"", "u8", "u", "U", "L"};
const std::vector<std::string> CharLitteral{"a",       "\\n",     "\\x7",    "\\x41",   "\\7",     "\\100",
                                            "\\u0024", "\\u00A2", "\\u0939", "\\u20AC", "\\uD55C", "\\U00010348"};
const std::vector<std::string> CharLitteral_real{"a",    "\n",       "\x7",          "\x41",         "\7",           "\100",
                                                 "\x24", "\xC2\xA2", "\xE0\xA4\xB9", "\xE2\x82\xAC", "\xED\x95\x9C", "\xF0\x90\x8D\x88"};

TEST_P(GenerateTest, CharLitteral_prefix) {
    assert(size(CharLitteral) == size(CharLitteral_real));
    for (size_t i = 0; i < size(CharLitteral_real); ++i) {
        Token t(Token::Type::CharLitteral, CharLitteral[i]);
        t.prefix(GetParam());
        convert_escape_sequence(t);
        EXPECT_EQ(t.lex(), CharLitteral_real[i]);
    }
}

INSTANTIATE_TEST_SUITE_P(prefix, GenerateTest, testing::ValuesIn(prefix));

class GenerateDeathTest : public testing::TestWithParam<std::tuple<std::string, std::string>> {};

const std::vector<std::string> bad_sequence{"ab", "\\x45p", "\\45\\n", "\\s", "\\1234", "\\U1110FFFF", "\\uD800", "\\uDFFF"};

TEST_P(GenerateDeathTest, bad_sequence) {
    auto [bad_seq, pref] = GetParam();
    Token t(Token::Type::CharLitteral, bad_seq);
    t.prefix(pref);
    EXPECT_DEATH(convert_escape_sequence(t), "error");
}

INSTANTIATE_TEST_SUITE_P(bad_sequence, GenerateDeathTest, testing::Combine(testing::ValuesIn(bad_sequence), testing::ValuesIn(prefix)));

class GenerateDeathTest2 : public testing::TestWithParam<int> {};

const std::vector<std::string> prefi{"", "u8", "u", "U", "L", "u8"};
const std::vector<std::string> sequence{"\\x123", "\\x12345", "\\x1234567", "\\x123456789", "\\x123456789", "\\777"};

TEST_P(GenerateDeathTest2, CharLitteral_custom) {
    size_t i = static_cast<size_t>(GetParam());
    Token t(Token::Type::CharLitteral, sequence[i]);
    t.prefix(prefi[i]);
    EXPECT_DEATH(convert_escape_sequence(t), "error");
}

INSTANTIATE_TEST_SUITE_P(sequence, GenerateDeathTest2, testing::Range(0, static_cast<int>(size(sequence))));
