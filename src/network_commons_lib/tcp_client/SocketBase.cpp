//
// Created by xPC on 20.10.2023.
//
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "SocketBase.h"
#include "spdlog/spdlog.h"

constexpr unsigned int MAX_BUF_LENGTH = 4096;

std::string SocketBase::receiveData() const
{
    int bytesReceived = 0;
    std::string rcv;
    std::vector<char> buffer(MAX_BUF_LENGTH);

    bytesReceived = recv(this->socketFd, &buffer[0], buffer.size(), 0);
    if (bytesReceived == -1) {
        throw std::runtime_error("Failed to receive data on this socket. (FD" + std::to_string(this->socketFd) + ")");
    } else {
        rcv.append(buffer.cbegin(), buffer.cbegin() + bytesReceived);
        spdlog::trace("Received data on socket FD{0} data: {1}, received bytes: {2}", this->socketFd, rcv, bytesReceived);
    }

    return rcv;
}

void SocketBase::sendData(const std::string& data) const
{
    int sentBytes = send(this->socketFd, data.c_str(), data.size(), 0);
    spdlog::trace("Sent data on socket FD{0} data: {1}, sent bytes: {2}", this->socketFd, data, sentBytes);
}

SocketBase::~SocketBase()
{
    spdlog::trace("Socket FD{0} is closing.", this->socketFd);
    close(socketFd);
}