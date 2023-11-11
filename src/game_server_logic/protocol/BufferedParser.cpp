//
// Created by surface on 9.11.23.
//

#include <stdexcept>
#include "BufferedParser.h"
#include "ProtocolData.h"
#include "spdlog/spdlog.h"

std::optional<std::vector<ProtocolData>> BufferedParser::parse(const std::string &data) {
    std::vector<ProtocolData> outputMsgs{};

    // Process each message
    unsigned long lastMsgEnd = 0;
    while (lastMsgEnd < data.length()) {
        //##############################################################################################################
        //# Extract message from data
        //##############################################################################################################
        unsigned long thisMsgStart = (lastMsgEnd > 0) ? lastMsgEnd + END_SEQUENCE.length() : 0;
        unsigned long thisMsgEnd = data.find(END_SEQUENCE, thisMsgStart);
        if (thisMsgEnd == std::string::npos) {
            thisMsgEnd = data.length();
        }
        unsigned long thisMsgLength = thisMsgEnd - thisMsgStart;

        std::string thisMsg = data.substr(thisMsgStart, thisMsgLength);
        spdlog::info("Msg: {0}", thisMsg);
        lastMsgEnd = thisMsgEnd;
        //##############################################################################################################
        //# Process current message
        //##############################################################################################################
        if (thisMsg.empty())
            continue;

        //Process each line of this message
        std::string thisLine;
        for (char const &c: thisMsg) {
            thisLine.push_back(c);
            if (c == END_OF_LINE && !thisLine.empty()) {
                const auto &output = parseLine(thisLine);
                if (output.has_value()) {
                    outputMsgs.emplace_back(output.value());
                }
                spdlog::info("MsgLine: {0}", thisLine);
                thisLine.clear();
            }
        }


    }

    //Return the vector of parsed messages, or nothing, if it's empty
    if (outputMsgs.empty()) {
        return std::nullopt;
    }
    return outputMsgs;
}

std::optional<ProtocolData> BufferedParser::parseLine(const std::string &data) {
    //######### ERROR CHECKING #########
    //##################################
    if (savedLine.length() + data.length() > MAX_LINE_LENGTH) {
        throw std::runtime_error("BufferedParser::parse: Error - max line length has been exceeded!");
    }

    if (savedLineNum == 0 && savedLine.length() + data.length() > MAX_METHOD_NAME_LENGTH) {
        throw std::runtime_error(
                "BufferedParser::parse: Error - invalid msg format - method name cannot be longer than 32 chars.");
    }

    //######### BEGIN PARSING #########
    //#################################
    savedLine += data;
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
    if (savedLineNum == 0) {
        //Parsing first line of the protocol (MethodName)
        trim(savedLine);
        MethodName parsedName = parseMethodName(savedLine);
        if (parsedName == MethodName::PARSING_FAILED)
            return ParsingStatus::INVALID_METHOD_NAME;
        else
            processedMethodName = parsedName;

    } else {
        //Check for ending sequence
        if (savedLine == END_SEQUENCE)
            return ParsingStatus::MESSAGE_PARSING_FINISHED;

        //Parse protocol "body" line (<ATTR_NAME>:<ATTR_VALUE>)
        unsigned long delimiterLocation = savedLine.find_first_of(PROTOCOL_DELIMITER);
        if (delimiterLocation == std::string::npos) {
            throw std::runtime_error(
                    "BufferedParser::parse: Error - Protocol line doesn't contain the delimiter symbol!");
        }
        std::string attrName = savedLine.substr(0, delimiterLocation - 1);
        std::string attrValue = savedLine.substr(delimiterLocation + 1);
        //Remove the last char (end of line symbol)
        attrValue.pop_back();

        processedData[attrName] = attrValue;
    }

    startNewLine();
    return ParsingStatus::MESSAGE_IS_INCOMPLETE;
}

void BufferedParser::startNewMessage() {
    savedLine.clear();
    savedLineNum = 0;
    processedMethodName = MethodName::UNINITIALIZED;
    processedData.clear();
}

void BufferedParser::startNewLine() {
    savedLine.clear();
    savedLineNum++;
}

void BufferedParser::trim(std::string &arg) {
    arg.erase(0, arg.find_first_not_of("\t\n\v\f\r ")); // left trim
    arg.erase(arg.find_last_not_of("\t\n\v\f\r ") + 1); // right trim
}
