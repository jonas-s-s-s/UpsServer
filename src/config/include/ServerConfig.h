//
// Created by xPC on 08.11.2023.
//

#pragma once

#include <string>

struct ServerConfig {
  public:
    ServerConfig(unsigned int roomCount, unsigned int maxPlayerCount, const std::string& serverIp, uint16_t serverPort);

    const unsigned int roomCount;
    const unsigned int maxPlayerCount;
    const std::string serverIp;
    const uint16_t serverPort;
};

ServerConfig parseServerConfig(const std::string& configFilePath);
