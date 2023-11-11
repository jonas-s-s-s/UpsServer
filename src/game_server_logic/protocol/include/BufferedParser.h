//
// Created by surface on 9.11.23.
//

#pragma once

#include <optional>
#include "ProtocolData.h"


class BufferedParser {
public:

    std::optional<std::vector<ProtocolData>> parse(const std::string &data);

private:
    int savedLineNum = 0;
    std::string savedLine;
    MethodName processedMethodName = MethodName::UNINITIALIZED;
    std::unordered_map<std::string, std::variant<std::string, std::vector<std::string>>> processedData{};
    enum class ParsingStatus {
        MESSAGE_PARSING_FINISHED, MESSAGE_IS_INCOMPLETE, INVALID_METHOD_NAME
    };

    std::optional<ProtocolData> parseLine(const std::string &data);
    ParsingStatus processLine();

    void startNewMessage();

    void startNewLine();

    static void trim(std::string &arg);

    //Circa 10 MB of data
    static constexpr int MAX_LINE_LENGTH = 10000000;
    static constexpr int MAX_METHOD_NAME_LENGTH = 32;
    static constexpr std::string_view END_SEQUENCE = "\r\n\r\n";
    static constexpr char PROTOCOL_DELIMITER = ':';
    static constexpr char END_OF_LINE = '\n';
};

