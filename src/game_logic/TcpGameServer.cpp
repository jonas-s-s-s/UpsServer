//
// Created by xPC on 08.11.2023.
//

#include "TcpGameServer.h"
#include "spdlog/spdlog.h"

void TcpGameServer::newClientConnectedHandler(int clientSocket) {
    spdlog::info("C: {0}", clientSocket);
    //===============================================================
    // Send Client FD to idle thread epoll via signal?


}

TcpGameServer::TcpGameServer(const std::string &address, uint16_t port) : ServerSocketBase(address, port) {}

