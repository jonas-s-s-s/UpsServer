//
// Created by xPC on 20.10.2023.
//

#include "ServerSocketBase.h"
#include "spdlog/spdlog.h"

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <arpa/inet.h>

ServerSocketBase::ServerSocketBase(const std::string &address, uint16_t port) {
    struct sockaddr_in localAddr{};
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(port); //Server listens on this port
    localAddr.sin_addr.s_addr = inet_addr(address.c_str()); //Server listens on this address

    //Create the socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
        throw std::runtime_error("Failed to create a server socket (system resource error?)");

    //Bind socket to port and ip
    if (bind(serverSocket, (struct sockaddr *) &localAddr, sizeof(struct sockaddr_in)) != 0)
        throw std::runtime_error(
                "Failed bind server socket. (FD" + std::to_string(serverSocket) + ") (Port: " + std::to_string(port) +
                ")");

    //Listen on bound port and ip
    if (listen(serverSocket, tcpAcceptBacklog) != 0)
        throw std::runtime_error("Failed listen on server socket. (FD" + std::to_string(serverSocket) + ")");

    spdlog::info("A new server socket FD{0} is now listening on port {1}", serverSocket, port);

    this->acceptThread = std::thread(&ServerSocketBase::tcpAcceptLoop, this);
}

void ServerSocketBase::tcpAcceptLoop() {
    spdlog::info("TCP accept thread has started for socket FD{0}", std::to_string(serverSocket));

    for (;;) {
        int clientSock; //Client's socket - client sends requests via this socket
        struct sockaddr_in remoteAddr{}; //Client's address
        socklen_t remoteAddrLen; //Length of client's address

        clientSock = accept(serverSocket, (struct sockaddr *) &remoteAddr, &remoteAddrLen);

        if (clientSock > 0) {
            spdlog::info("A new TCP client FD{0} connected to server FD{1}", clientSock, std::to_string(serverSocket));
            newClientConnectedHandler(clientSock);
        } else {
            //TODO: Remove this throw?
            throw std::runtime_error(
                    "Fatal error in accept loop of server socket FD" + std::to_string(serverSocket) +
                    " TCP accept failed.");
        }
    }
}

void ServerSocketBase::joinOnAcceptThread() {
    acceptThread.join();
}

ServerSocketBase::~ServerSocketBase() {
    close(serverSocket);
}


