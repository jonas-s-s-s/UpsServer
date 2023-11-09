//
// Created by surface on 9.11.23.
//

#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

enum MethodName {
    CONNECTED_OK
};
MethodName parseMethodName(const std::string& name);

struct ProtocolData {
public:
    ProtocolData(const MethodName method,
                 const std::unordered_map<std::string, std::variant<std::string, std::vector<std::string>>> &data) : method(
            method), data(data) {}

    const MethodName method;
    const std::unordered_map<std::string, std::variant<std::string, std::vector<std::string>>> data;
};
