//
// Created by xPC on 20.10.2023.
//

#include <netinet/in.h>
#include <stdexcept>
#include <iostream>
#include <arpa/inet.h>

#include "ClientSocket.h"
#include "netTools.h"
#include "spdlog/spdlog.h"

/**
 * Used for testing purposes. Allows connection to our own server.
 * @param address Server's address
 * @param port Server's port
 */
ClientSocket::ClientSocket(const std::string &address, uint16_t port) {
    //First determine the serverIp (address can be either hostname or numeric ip)
    std::string serverIp = convertToValidIpV4(address, port);

    struct sockaddr_in remoteAddr{};

    int clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock <= 0)
        throw std::runtime_error("Failed to create client socket.");

    spdlog::trace("A client socket has been created: FD{0}", clientSock);

    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(port);
    remoteAddr.sin_addr.s_addr = inet_addr(serverIp.c_str());

    if (connect(clientSock, (struct sockaddr *) &remoteAddr, sizeof(struct sockaddr_in)) != 0)
        throw std::runtime_error(
                "Client socket connection to remote server has failed. Server: " + address + " Port: " +
                std::to_string(port) + " Ip: " + serverIp);

    spdlog::trace("A client socket FD{0} connected to: {1}:{2}", clientSock, serverIp, port);
    this->socketFd = clientSock;
}

ClientSocket::ClientSocket(int clientSocketFd) {
    this->socketFd = clientSocketFd;
}


