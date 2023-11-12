//
// Created by xPC on 12.11.2023.
//

#pragma once

#include <thread>
#include "EventfdQueue.h"

class IdleUsersRoom {
public:
    explicit IdleUsersRoom(const EventfdQueue<int> &newClientsQ);
    void startIdleThread();
    void joinOnIdleThread();
private:
    void idleThreadLoop();
    const EventfdQueue<int> &newClientsQ;
    std::thread idleThread;
};
