//
// Created by xPC on 12.11.2023.
//

#include "IdleUsersRoom.h"
#include "ServerSocketBase.h"

IdleUsersRoom::IdleUsersRoom(const EventfdQueue<int> &newClientsQ) : newClientsQ(newClientsQ) {}

void IdleUsersRoom::startIdleThread() {
    idleThread = std::thread(&IdleUsersRoom::idleThreadLoop, this);
}

void IdleUsersRoom::idleThreadLoop() {
    //Epoll{}
}

void IdleUsersRoom::joinOnIdleThread() {
    idleThread.join();
}


