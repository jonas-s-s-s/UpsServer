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
    CONNECTED_OK,
    PARSING_FAILED,
    UNINITIALIZED
};

/**
 * This function is the only way of parsing STRING to ENUM VALUE in C++ without using reflection.
 * STRING -> ENUM VALUE conversion is done in O(1) time using unordered map.
 * THE INTERNAL MAP HAS TO BE UPDATED EVERY TIME A NEW MethodName ITEM IS ADDED!
 * @param name Method name to parse
 * @return An enum value (PARSING_FAILED is returned if method name cannot be parsed)
 */
MethodName parseMethodName(const std::string &name);

struct ProtocolData {
public:
    explicit ProtocolData(const MethodName method,
                          const std::unordered_map<std::string, std::variant<std::string, std::vector<std::string>>> data)
            : method(method), data(data) {}

    const MethodName method;
    const std::unordered_map<std::string, std::variant<std::string, std::vector<std::string>>> data;
};
