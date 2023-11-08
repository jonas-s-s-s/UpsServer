//
// Created by xPC on 20.10.2023.
//

#pragma once

#include <cstdint>
#include <thread>

constexpr int tcpAcceptBacklog = 5;

class ServerSocketBase {
protected:
    int serverSocket; //listens for client connections (threadsafe - created in constructor, isn't modified elsewhere)

private:
    void tcpAcceptLoop();

    std::thread acceptThread;

    virtual void newClientConnectedHandler(int clientSocket) = 0;

public:
    ServerSocketBase(const std::string &address, uint16_t port);

    void joinOnAcceptThread();

    virtual ~ServerSocketBase();
};