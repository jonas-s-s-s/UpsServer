//
// Created by surface on 9.11.23.
//

#pragma once

#include "ProtocolData.h"

//Circa 10 MB of data
constexpr int MAX_LINE_LENGTH = 10000000;

class BufferedParser {
public:
    void parse(const std::string& data);

    ProtocolData GetResult();

    bool isDone();

private:
    ProtocolData processedData;
    int currentLineNum = 0;
    std::string currentLine;
    bool parsingDone = false;

    void processLine();
    void reset();
};