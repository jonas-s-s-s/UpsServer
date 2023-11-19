//
// Created by xPC on 08.11.2023.
//

#include "ServerConfig.h"
#include <algorithm>
#include <fstream>
#include <sstream>

ServerConfig parseServerConfig(const std::string& configFilePath)
{
    std::ifstream file(configFilePath);

    if (file.fail())
        throw std::runtime_error("parseServerConfig failed to open the file: " + configFilePath);

    unsigned int roomCount = 0;
    unsigned int maxPlayerCount = 0;
    std::string serverIp;
    uint16_t serverPort = 0;

    std::string line;
    while (std::getline(file, line)) {
        // Continue if line is whitespace only
        if (std::all_of(line.begin(), line.end(), isspace))
            continue;

        std::string lhs;
        std::string rhs;

        // Process line
        bool isLhs = true;
        for (char const& c : line) {
            if (c == ':') {
                isLhs = false;
                continue;
            }

            if (isLhs) {
                lhs.push_back(c);
            } else {
                rhs.push_back(c);
            }
        }

        if (isLhs) {
            throw std::runtime_error("ServerConfig failed to parse file: missing \":\" delimiter in some line" + lhs);
        }

        // Inlined the former trim function - trim whitespace from both lhs and rhs
        lhs.erase(0, lhs.find_first_not_of("\t\n\v\f\r "));
        lhs.erase(lhs.find_last_not_of("\t\n\v\f\r ") + 1);

        rhs.erase(0, rhs.find_first_not_of("\t\n\v\f\r "));
        rhs.erase(rhs.find_last_not_of("\t\n\v\f\r ") + 1);

        if (lhs == "roomCount") {
            roomCount = std::stoi(rhs);
        } else if (lhs == "maxPlayerCount") {
            maxPlayerCount = std::stoi(rhs);
        } else if (lhs == "serverIp") {
            serverIp = rhs;
        } else if (lhs == "serverPort") {
            serverPort = static_cast<uint16_t>(std::stoi(rhs));
        } else {
            throw std::runtime_error("parseServerConfig failed to parse file: it contains invalid left hand side argument - " + lhs);
        }
    }
    return ServerConfig{roomCount, maxPlayerCount, serverIp, serverPort};
}

ServerConfig::ServerConfig(const unsigned int roomCount, const unsigned int maxPlayerCount, const std::string& serverIp,
                           const uint16_t serverPort)
    : roomCount(roomCount), maxPlayerCount(maxPlayerCount), serverIp(serverIp), serverPort(serverPort)
{}
