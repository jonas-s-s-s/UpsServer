//
// Created by surface on 9.11.23.
//

#include <stdexcept>
#include "BufferedParser.h"
#include "ProtocolData.h"

void BufferedParser::parse(const std::string &data) {
    //Keep adding chars to currentLine, until \n is detected
    for (const char &c: data) {
        if (c == '\n') {
            processLine();
        } else {
            currentLine.push_back(c);
        }
    }

    //The first line of the protocol msg (method name) cannot be longer than 32 chars
    if (currentLineNum == 0 && currentLine.length() > 32) {
        throw std::runtime_error(
                "BufferedParser::parse: Error - invalid msg format - method name cannot be longer than 32 chars.");
    } else if (currentLine.length() > MAX_LINE_LENGTH) {
        throw std::runtime_error("BufferedParser::parse: Error - max line length has been exceeded!");
    }

}

void BufferedParser::processLine() {
    //TODO: PROCESS FIRT PROTOCOL LINE


    currentLine.clear();
    currentLineNum++;
}

ProtocolData BufferedParser::GetResult() {

}

bool BufferedParser::isDone() {
    return false;
}

void BufferedParser::reset() {

}
