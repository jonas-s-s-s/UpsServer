//
// Created by xPC on 08.11.2023.
//
#include <gtest/gtest.h>
#include "testBaseParser.h"

class testParserTwoMessage : public testBaseParser {
protected:

    void SetUp() override {
        bufferedParser = new BufferedParser{};
    }

    void TearDown() override {
        delete bufferedParser;
    }
};

TEST_F(testParserTwoMessage, Msg_amount_ok) {
    const auto &parse = bufferedParser->parse(
            "CONNECTED_OK\narg1:argval1\narg2:argval2\n\r\n\r\nCONNECTED_OK\narg1_second:argval1\narg2:arg2_second\n\r\n\r\n");
    ProtocolData parsed = parse.value().at(0);
    ASSERT_EQ(parsed.data.size(), 2);
}

TEST_F(testParserTwoMessage, Msg1_arg_ok) {
    const auto &parse = bufferedParser->parse(
            "CONNECTED_OK\narg1:argval1\narg2:argval2\n\r\n\r\nCONNECTED_OK\narg1_second:argval1\narg2:arg2_second\n\r\n\r\n");
    ProtocolData parsed = parse.value().at(0);
    ASSERT_EQ(parsed.data.at("arg1"), "argval1");
}

TEST_F(testParserTwoMessage, Msg2_arg_ok) {
    const auto &parse = bufferedParser->parse(
            "CONNECTED_OK\narg1:argval1\narg2:argval2\n\r\n\r\nCONNECTED_OK\narg1_second:argval1\narg2:arg2_second\n\r\n\r\n");
    ProtocolData parsed = parse.value().at(1);
    ASSERT_EQ(parsed.data.at("arg1_second"), "argval1");
}

TEST_F(testParserTwoMessage, Fragmented_second_message_arg1_ok) {
    bufferedParser->parse("CONNECTED_OK\narg1:argval1\narg2:argval2\n\r\n\r\nCONNECTED_O");
    const auto &parse = bufferedParser->parse("K\narg1_second:argval1\narg2_second:argval2\n\r\n\r\n");

    ProtocolData parsed = parse.value().at(0);
    ASSERT_EQ(parsed.data.at("arg1_second"), "argval1");
}


TEST_F(testParserTwoMessage, Fragmented_second_message_arg2_ok) {
    bufferedParser->parse("CONNECTED_OK\narg1:argval1\narg2:argval2\n\r\n\r\nCONNECTED_O");
    const auto &parse = bufferedParser->parse("K\narg1_second:argval1\narg2_second:argval2\n\r\n\r\n");

    ProtocolData parsed = parse.value().at(0);
    ASSERT_EQ(parsed.data.at("arg2_second"), "argval2");
}

