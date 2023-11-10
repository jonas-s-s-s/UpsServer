//
// Created by surface on 9.11.23.
//

#include <stdexcept>
#include "BufferedParser.h"
#include "ProtocolData.h"

std::optional<ProtocolData> BufferedParser::parse(const std::string &data) {
    //######### ERROR CHECKING #########
    //##################################
    if (currentLine.length() + data.length() > MAX_LINE_LENGTH) {
        throw std::runtime_error("BufferedParser::parse: Error - max line length has been exceeded!");
    }

    if (currentLineNum == 0 && currentLine.length() + data.length() > MAX_METHOD_NAME_LENGTH) {
        throw std::runtime_error(
                "BufferedParser::parse: Error - invalid msg format - method name cannot be longer than 32 chars.");
    }

    //######### BEGIN PARSING #########
    //#################################
    currentLine += data;
    if (data.ends_with(END_OF_LINE)) {
        //Line is completed, parse it and return result
        switch (processLine()) {
            using
            enum BufferedParser::ParsingStatus;
            case MESSAGE_IS_INCOMPLETE:
                return std::nullopt;
            case INVALID_METHOD_NAME:
                throw std::runtime_error("BufferedParser::parse: Error - This method name is not allowed.");
            case MESSAGE_PARSING_FINISHED:
                MethodName name = processedMethodName;
                auto map = processedData;
                startNewMessage();
                return std::optional<ProtocolData>{ProtocolData{name, map}};
        }
    }
    //Line is incomplete, return empty optional
    return std::nullopt;
}

BufferedParser::ParsingStatus BufferedParser::processLine() {
    if (currentLineNum == 0) {
        //Parsing first line of the protocol (MethodName)
        trim(currentLine);
        MethodName parsedName = parseMethodName(currentLine);
        if (parsedName == MethodName::PARSING_FAILED)
            return ParsingStatus::INVALID_METHOD_NAME;
        else
            processedMethodName = parsedName;

    } else {
        //Check for ending sequence
        if (currentLine == END_OF_MESSAGE)
            return ParsingStatus::MESSAGE_PARSING_FINISHED;

        //Parse protocol "body" line (<ATTR_NAME>:<ATTR_VALUE>)
        unsigned long delimiterLocation = currentLine.find_first_of(PROTOCOL_DELIMITER);
        if (delimiterLocation == std::string::npos) {
            throw std::runtime_error(
                    "BufferedParser::parse: Error - Protocol line doesn't contain the delimiter symbol!");
        }
        std::string attrName = currentLine.substr(0, delimiterLocation - 1);
        std::string attrValue = currentLine.substr(delimiterLocation + 1);
        //Remove the last char (end of line symbol)
        attrValue.pop_back();

        processedData[attrName] = attrValue;
    }

    startNewLine();
    return ParsingStatus::MESSAGE_IS_INCOMPLETE;
}

void BufferedParser::startNewMessage() {
    currentLine.clear();
    currentLineNum = 0;
    processedMethodName = MethodName::UNINITIALIZED;
    processedData.clear();
}

void BufferedParser::startNewLine() {
    currentLine.clear();
    currentLineNum++;
}

void BufferedParser::trim(std::string &arg) {
    arg.erase(0, arg.find_first_not_of("\t\n\v\f\r ")); // left trim
    arg.erase(arg.find_last_not_of("\t\n\v\f\r ") + 1); // right trim
}
