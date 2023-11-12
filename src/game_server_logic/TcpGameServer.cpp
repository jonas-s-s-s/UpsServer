//
// Created by xPC on 08.11.2023.
//

#include "TcpGameServer.h"
#include "spdlog/spdlog.h"

void TcpGameServer::newClientConnectedHandler(int clientSocket) {
    //This method is called by the ServerSocketBase class each time a new client connects in the TCP accept loop
    //===============================================================
    //When new client connects, we add his file descriptor to this queue, which is then processed by idle thread
    clientSocketQ.push(clientSocket);
}

TcpGameServer::TcpGameServer(const std::string &address, uint16_t port) : ServerSocketBase(address, port) {}

EventfdQueue<int> &TcpGameServer::getClientSocketQ() {
    return clientSocketQ;
}

