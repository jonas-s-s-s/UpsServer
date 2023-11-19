//
// Created by xPC on 08.11.2023.
//
#include "testBaseParser.h"
#include <gtest/gtest.h>

class testParserOneMessage : public testBaseParser {
  protected:
    void SetUp() override
    {
        bufferedParser = new BufferedParser{};
    }

    void TearDown() override
    {
        delete bufferedParser;
    }
};

TEST_F(testParserOneMessage, Method_name_ok)
{
    const auto& parse = bufferedParser->parse("CONNECTED_OK\narg1:argval1\narg2:argval2\n\r\n\r\n");
    ProtocolData parsed = parse.value().at(0);
    ASSERT_EQ(parsed.method, MethodName::CONNECTED_OK);
}

TEST_F(testParserOneMessage, Arg1_ok)
{
    const auto& parse = bufferedParser->parse("CONNECTED_OK\narg1:argval1\narg2:argval2\n\r\n\r\n");
    ProtocolData parsed = parse.value().at(0);
    ASSERT_EQ(parsed.data.at("arg1"), "argval1");
}

TEST_F(testParserOneMessage, Arg2_ok)
{
    const auto& parse = bufferedParser->parse("CONNECTED_OK\narg1:argval1\narg2:argval2\n\r\n\r\n");
    ProtocolData parsed = parse.value().at(0);
    ASSERT_EQ(parsed.data.at("arg2"), "argval2");
}

TEST_F(testParserOneMessage, Fragmented_message_arg1_ok)
{
    bufferedParser->parse("CONNECTED_OK\narg1:argval1\na");
    const auto& parse = bufferedParser->parse("rg2:argval2\n\r\n\r\n");

    ProtocolData parsed = parse.value().at(0);
    ASSERT_EQ(parsed.data.at("arg1"), "argval1");
}

TEST_F(testParserOneMessage, Fragmented_message_arg2_ok)
{
    bufferedParser->parse("CONNECTED_OK\narg1:argval1\na");
    const auto& parse = bufferedParser->parse("rg2:argval2\n\r\n\r\n");

    ProtocolData parsed = parse.value().at(0);
    ASSERT_EQ(parsed.data.at("arg2"), "argval2");
}