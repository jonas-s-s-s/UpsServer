//
// Created by xPC on 20.10.2023.
//

#pragma once

#include <string>
#include <vector>

std::vector<std::string> split(const std::string &s, const std::string &delimiter);

bool isIpAddress(const std::string &address);

std::string getIpFromHostname(const std::string &hostname, const std::string &port);

std::string convertToValidIpV4(const std::string &host, uint16_t port);
