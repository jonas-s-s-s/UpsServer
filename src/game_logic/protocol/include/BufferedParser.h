//
// Created by surface on 9.11.23.
//

#pragma once

#include "ProtocolData.h"
#include <optional>

class BufferedParser {
  public:
    /**
     * "Greedy" parsing method. Can accept one message, multiple messages, several lines of message, etc.
     * Returns a vector containing parsed messages. Incomplete message is stored for later in internal cache.
     * If there is no complete message in input data, an empty optional is returned.
     * Throws a runtime exception in case of a parsing error - protocol text is formatted incorrectly.
     * @param data protocol text
     */
    std::optional<std::vector<ProtocolData>> parse(const std::string& data);

  private:
    // Temporary data about unprocessed messages
    MethodName savedMethod = MethodName::UNINITIALIZED;
    std::unordered_map<std::string, std::string> savedData{};
    std::string savedLine;
    bool isSavedLineFirst = true;

    void parseMsgLines(const std::vector<std::string>& lines);

    /**
     * Special split method
     * @return Returns splitted strings which include separator
     */
    static std::vector<std::string> split(const std::string& s, const std::string& delimiter);

    // Circa 10 MB of data
    static constexpr int MAX_LINE_LENGTH = 10000000;
    static constexpr int MAX_METHOD_NAME_LENGTH = 32;
    static constexpr const char* const END_SEQUENCE = "\r\n\r\n";
    static constexpr int END_SEQUENCE_LEN = 4;
    static constexpr const char* const LINE_DELIMITER = ":";
    static constexpr const char* const END_OF_LINE = "\n";

    void clearCache();
};
