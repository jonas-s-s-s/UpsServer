//
// Created by xPC on 08.11.2023.
//

#pragma once

#include "ServerSocketBase.h"

class TcpGameServer : public ServerSocketBase {
public:
    TcpGameServer(const std::string &address, uint16_t port);

private:
    void newClientConnectedHandler(int clientSocket) override;
};