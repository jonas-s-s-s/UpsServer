//
// Created by xPC on 08.11.2023.
//
#include <gtest/gtest.h>
#include "testBaseParser.h"

class testParserExceptions : public testBaseParser {
protected:

    void SetUp() override {
        bufferedParser = new BufferedParser{};
    }

    void TearDown() override {
        delete bufferedParser;
    }
};

TEST_F(testParserExceptions, Max_line_length_exception) {
    EXPECT_THROW(
            bufferedParser->parse("CONNECTED_OK\n");
            for (int i = 0; i < 10000000; ++i) {
                bufferedParser->parse("randomrandomrandomrandomrandomrandomrandomrandom");
            }
            , std::runtime_error);
}

TEST_F(testParserExceptions, Method_name_too_long_exception) {
    EXPECT_THROW(bufferedParser->parse("12345678910123456789101234567891012345678910\narg1:argval1\narg2:argval2\n\r\n\r\n"), std::runtime_error);
}

TEST_F(testParserExceptions, Invalid_method_name_exception) {
    EXPECT_THROW(bufferedParser->parse("CONNE\narg1:argval1\narg2:argval2\n\r\n\r\n"), std::runtime_error);
}

TEST_F(testParserExceptions, Missing_line_separator_exception) {
    EXPECT_THROW(bufferedParser->parse("CONNECTED_OK\narg1argval1\narg2:argval2\n\r\n\r\n"), std::runtime_error);
}