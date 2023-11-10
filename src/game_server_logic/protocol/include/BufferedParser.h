//
// Created by surface on 9.11.23.
//

#pragma once

#include <optional>
#include "ProtocolData.h"


class BufferedParser {
public:

    /**
     * "Greedy" parsing method. Can accept either one protocol line, part of the line or multiple lines of the same message.
     * Returns a struct with the parsed protocol message once ending symbol sequence is detected. If the protocol
     * message is still incomplete after calling this method, an empty std::optional is returned.
     * Throws a runtime exception in case of a parsing error - protocol text is formatted incorrectly.
     * @param data protocol text
     */
    std::optional<ProtocolData> parse(const std::string &data);

private:
    int currentLineNum = 0;
    std::string currentLine;
    MethodName processedMethodName;
    std::unordered_map<std::string, std::variant<std::string, std::vector<std::string>>> processedData;
    enum class ParsingStatus {
        MESSAGE_PARSING_FINISHED, MESSAGE_IS_INCOMPLETE, INVALID_METHOD_NAME
    };

    ParsingStatus processLine();

    void startNewMessage();

    void startNewLine();

    static void trim(std::string &arg);

    //Circa 10 MB of data
    static constexpr int MAX_LINE_LENGTH = 10000000;
    static constexpr int MAX_METHOD_NAME_LENGTH = 32;
    static constexpr std::string_view END_OF_MESSAGE = "\r\n\r\n";
    static constexpr char PROTOCOL_DELIMITER = ':';
    static constexpr char END_OF_LINE = '\n';
};

