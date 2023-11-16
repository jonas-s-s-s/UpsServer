//
// Created by surface on 9.11.23.
//
#include <stdexcept>
#include "ProtocolData.h"

bool ProtocolData::hasField(const std::string &name) const {
    return data.count(name) >= 1;
}

std::string ProtocolData::getField(const std::string &name) const {
    return data.at(name);
}

MethodName parseMethodName(const std::string &name) {
    auto it = MethodNameTable.find(name);
    if (it != MethodNameTable.end()) {
        return it->second;
    } else {
        return MethodName::PARSING_FAILED;
    }
}

std::string serializeMethodName(MethodName name) {
    //Static const variable is initialized only once - the first time this function is entered
    static auto const reverseLookup = []() -> auto {
        std::unordered_map<MethodName, std::string> reversed{};
        for (const auto &kvPair: MethodNameTable) {
            reversed[kvPair.second] = kvPair.first;
        }
        return reversed;
    }();

    auto it = reverseLookup.find(name);
    if (it != reverseLookup.end()) {
        return it->second;
    } else {
        throw std::runtime_error(
                "serializeMethodName: Error - Entered method name was not found. Is it contained in MethodNameTable?");
    }

}

std::string serializeProtocolData(const ProtocolData &data) {
    static constexpr const char *const END_SEQUENCE = "\r\n\r\n";
    static constexpr const char *const LINE_DELIMITER = ":";
    static constexpr const char *const END_OF_LINE = "\n";

    std::string output;
    //First add method name
    output.append(serializeMethodName(data.method));
    output.append(END_OF_LINE);

    //Then add all attributes
    for (auto const &[attrName, attrValue]: data.data) {
        output.append(attrName);
        output.append(LINE_DELIMITER);
        output.append(attrValue);
        output.append(END_OF_LINE);
    }

    //Add message end sequence
    output.append(END_SEQUENCE);
    return output;
}

std::string serializeObjectList(const std::vector<std::vector<std::string>> &list) {
    std::string output;

    bool isFistLine = true;
    for (const std::vector<std::string>& line: list) {
        if(isFistLine) {
            isFistLine = false;
        } else {
            output += ",";
        }

        bool isFistItem = true;
        output += "{";
        for (const std::string& lineItem: line) {
            if(isFistItem) {
                isFistItem = false;
            } else {
                output += ",";
            }
            output += lineItem;
        }
        output += "}";
    }
    return output;
}


ProtocolData
newProtocolMessage(MethodName method, std::initializer_list<std::pair<std::string, std::string>> dataFields) {
    std::unordered_map<std::string, std::string> data;
    for (std::pair<std::string, std::string> pair: dataFields) {
        data.insert(pair);
    }
    return ProtocolData(method, std::move(data));
}

ProtocolData newProtocolMessage(MethodName method) {
    return ProtocolData(method, std::unordered_map<std::string, std::string>());
}



