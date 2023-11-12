//
// Created by xPC on 08.11.2023.
//

#pragma once

#include "ServerSocketBase.h"
#include "EventfdQueue.h"

class TcpGameServer : public ServerSocketBase {
public:
    TcpGameServer(const std::string &address, uint16_t port);

    EventfdQueue<int> &getNewClientsQueue();

private:
    void _newClientConnectedHandler(int clientSocket) override;

    EventfdQueue<int> _newClientsQueue{};
};
