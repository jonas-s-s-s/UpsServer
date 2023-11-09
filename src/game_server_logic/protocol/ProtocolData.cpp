//
// Created by surface on 9.11.23.
//
#include <stdexcept>
#include "ProtocolData.h"

MethodName parseMethodName(const std::string &name) {
    static std::unordered_map<std::string, MethodName> const table = {
            {"CONNECTED_OK", MethodName::CONNECTED_OK}
    };
    auto it = table.find(name);
    if (it != table.end()) {
        return it->second;
    } else {
        throw std::runtime_error("parseMethodName: Error - this method name is not contained within enum.");
    }
}
