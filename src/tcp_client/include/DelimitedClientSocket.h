//
// Created by xPC on 20.10.2023.
//

#pragma once

#include <string>
#include <vector>
#include "ClientSocket.h"

class DelimitedClientSocket {
private:
    const std::string delimiter;
    ClientSocket socket;
public:
    DelimitedClientSocket(const std::string &address, uint16_t port, std::string delimiter);

    DelimitedClientSocket(int clientSocketFd, std::string delimiter);

    std::vector<std::string> receiveMsg() const;

    std::vector<std::string> receiveMsgRepeated(unsigned int count) const;

    void sendMsg(const std::string &message) const;
};