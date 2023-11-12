//
// Created by xPC on 12.11.2023.
//

#pragma once

#include <thread>
#include "EventfdQueue.h"
#include "Epoll.h"

class IdleUsersRoom {
public:
    explicit IdleUsersRoom(EventfdQueue<int> &newClientsQ);

    void startIdleThread();

    void joinOnIdleThread();

private:
    void _idleThreadLoop();

    void _onNewClientConnect(int evfd, Epoll &epoll);

    void _onClientWrite(int clientfd);

    void _onClientDisconnect(int clientfd);

    EventfdQueue<int> &_newClientsQueue;
    std::thread _idleThread;
};
