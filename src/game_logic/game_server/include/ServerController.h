//
// Created by xPC on 08.11.2023.
//

#pragma once
#include <string>

class ServerController {
  public:
    void start(const std::string& serverIp, uint16_t serverPort, unsigned int maxPlayerCount, unsigned int roomCount);

    void start();
};
