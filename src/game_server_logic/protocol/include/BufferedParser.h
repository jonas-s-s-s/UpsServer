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
    //Temporary data about unprocessed messages
    MethodName savedMethod = MethodName::UNINITIALIZED;
    std::unordered_map<std::string, std::string> savedData{};
    std::string savedLine;
    bool isSavedLineFirst = true;

    void parseMsgLines(const std::vector<std::string> &lines);

    static std::vector<std::string> split(const std::string &s, const std::string &delimiter);

    //Circa 10 MB of data
    static constexpr int MAX_LINE_LENGTH = 10000000;
    static constexpr int MAX_METHOD_NAME_LENGTH = 32;
    static constexpr const char *const END_SEQUENCE = "\r\n\r\n";
    static constexpr int END_SEQUENCE_LEN = 4;
    static constexpr const char *const LINE_DELIMITER = ":";
    static constexpr const char *const END_OF_LINE = "\n";

    void clearCache();
};

