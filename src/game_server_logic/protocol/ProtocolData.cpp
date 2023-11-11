//
// Created by surface on 9.11.23.
//
#include <stdexcept>
#include "ProtocolData.h"

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