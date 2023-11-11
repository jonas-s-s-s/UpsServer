add_executable(parser_test testBaseParser.h testParserOneMessage.cpp testParserTwoMessages.cpp testParserExceptions.cpp)
AddTests(parser_test)

target_link_libraries(parser_test PRIVATE protocol)

