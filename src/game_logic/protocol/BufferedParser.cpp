//
// Created by surface on 9.11.23.
//

#include "BufferedParser.h"
#include "ProtocolData.h"
#include <stdexcept>

std::optional<std::vector<ProtocolData>> BufferedParser::parse(const std::string& data) {
    std::vector<ProtocolData> output{};

    // # Split messages contained in data by END_SEQUENCE
    // ##################################################################################################################
    const std::vector<std::string>& messages = split(data, END_SEQUENCE);

    // # Split each message by END_OF_LINE and process it's content
    // ##################################################################################################################
    for (std::string msg : messages) {
        bool containsEndSeq = msg.ends_with(END_SEQUENCE);
        // Remove the ending sequence, if it's present
        if (containsEndSeq) {
            msg = msg.substr(0, msg.length() - END_SEQUENCE_LEN);
        }

        // Will parse each line and save parsed data to class fields
        parseMsgLines(split(msg, END_OF_LINE));

        // If end of message was detected put all cached data into output vector
        if (containsEndSeq) {
            output.emplace_back(this->savedMethod, this->savedData);
            // Clear any old cached data
            clearCache();
        }
    }

    // # Return results
    // ##################################################################################################################
    if (output.empty())
        return std::nullopt;
    return std::optional<std::vector<ProtocolData>>{output};
}

void BufferedParser::parseMsgLines(const std::vector<std::string>& lines) {
    for (std::string line : lines) {
        // # Error checking
        // ##############################################################################################################
        if (savedLine.length() + line.length() > MAX_LINE_LENGTH) {
            throw std::runtime_error("BufferedParser::parse: Error - max line length has been exceeded!");
        }

        if (isSavedLineFirst && line.length() + savedLine.length() > MAX_METHOD_NAME_LENGTH) {
            throw std::runtime_error("BufferedParser::parse: Error - invalid msg format - method name cannot be longer than 32 chars.");
        }
        // # Begin parsing
        // ##############################################################################################################

        // If we didn't receive full line, save it for later
        if (!line.ends_with(END_OF_LINE)) {
            savedLine += line;
            continue;
        }
        // \n can now be removed
        line.pop_back();

        // We have full line, add any saved value to it
        if (!savedLine.empty()) {
            line.insert(0, savedLine);
            // Clear the line cache
            savedLine.clear();
        }

        if (isSavedLineFirst) {
            // First line of the message is method name
            MethodName parsedName = parseMethodName(line);
            if (parsedName == MethodName::PARSING_FAILED)
                throw std::runtime_error("BufferedParser::parse: Error - This method name is not allowed.");
            else
                savedMethod = parsedName;

            isSavedLineFirst = false;
        } else {
            // All other lines are in the format <attr_name>:<attr_value>\n
            const std::vector<std::string>& lineFields = split(line, LINE_DELIMITER);
            if (lineFields.size() < 2) {
                throw std::runtime_error("BufferedParser::parse: Error - Invalid line, missing line delimiter.");
            }
            // Save the results
            std::string attrName = lineFields[0];
            std::string attrValue = lineFields[1];
            attrName.pop_back();
            savedData[attrName] = attrValue;
        }
    }
}

void BufferedParser::clearCache() {
    savedLine.clear();
    isSavedLineFirst = true;
    savedMethod = MethodName::UNINITIALIZED;
    savedData.clear();
}

std::vector<std::string> BufferedParser::split(const std::string& s, const std::string& delimiter) {
    size_t pos_start = 0;
    size_t pos_end;
    size_t delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, (pos_end - pos_start) + delim_len);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    // Do NOT add empty string to end of the vector
    std::string end = s.substr(pos_start);
    if (!end.empty())
        res.push_back(end);

    return res;
}
