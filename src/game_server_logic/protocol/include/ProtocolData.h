//
// Created by surface on 9.11.23.
//

#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

enum class MethodName {
    //Fundamental methods
    CONNECTED_OK,
    REQ_ACCEPTED,
    REQ_DENIED,
    TERMINATE_CONNECTION,
    //Placeholder values
    PARSING_FAILED,
    UNINITIALIZED,
    //Idle user's room methods
    ENTER_USERNAME,
    GET_ROOM_LIST,
    JOIN_GAME,
    //Game methods

};
static std::unordered_map<std::string, MethodName> const MethodNameTable = {
        {"CONNECTED_OK",         MethodName::CONNECTED_OK},
        {"REQ_ACCEPTED",         MethodName::REQ_ACCEPTED},
        {"REQ_DENIED",           MethodName::REQ_DENIED},
        {"TERMINATE_CONNECTION", MethodName::TERMINATE_CONNECTION},
        {"PARSING_FAILED",       MethodName::PARSING_FAILED},
        {"UNINITIALIZED",        MethodName::UNINITIALIZED},
        {"ENTER_USERNAME",       MethodName::ENTER_USERNAME},
        {"GET_ROOM_LIST",        MethodName::GET_ROOM_LIST},
        {"JOIN_GAME",            MethodName::JOIN_GAME}
};

/**
 * This function is the only way of parsing STRING to ENUM VALUE in C++ without using reflection.
 * STRING -> ENUM VALUE conversion is done in O(1) time using unordered map.
 * THE MethodNameTable MAP HAS TO BE UPDATED EVERY TIME A NEW MethodName ITEM IS ADDED!
 * @param name Method name to parse
 * @return An enum value (PARSING_FAILED is returned if method name cannot be parsed)
 */
MethodName parseMethodName(const std::string &name);

/**
 * Converts members of MethodName enum to string representation. Uses reversed MethodNameTable to determine return value.
 * @param name The method name to serialize
 * @return String, runtime_error is thrown if MethodNameTable doesn't contain this MethodName
 */
std::string serializeMethodName(MethodName name);

struct ProtocolData {
public:
    explicit ProtocolData(const MethodName method, const std::unordered_map<std::string, std::string> data)
            : method(method), data(data) {}

    const MethodName method;
    const std::unordered_map<std::string, std::string> data;
    bool hasField(const std::string& name) const;
    std::string getField(const std::string& name) const;
};

/**
 * Converts ProtocolData to string representation.
 * @return String, runtime_error is thrown in case or serialization failure
 */
std::string serializeProtocolData(const ProtocolData &data);

/**
 * Factory function for the ProtocolData class
 */
ProtocolData
newProtocolMessage(MethodName method, std::initializer_list<std::pair<std::string, std::string>> dataFields);

ProtocolData newProtocolMessage(MethodName method);