//
// Created by xPC on 12.11.2023.
//

#include <sys/epoll.h>
#include "IdleUsersRoom.h"
#include "ServerSocketBase.h"
#include "Epoll.h"
#include "spdlog/spdlog.h"

IdleUsersRoom::IdleUsersRoom(EventfdQueue<int> &newClientsQueue) : _newClientsQueue(newClientsQueue) {}

void IdleUsersRoom::startIdleThread() {
    _idleThread = std::thread(&IdleUsersRoom::_idleThreadLoop, this);
}

void IdleUsersRoom::joinOnIdleThread() {
    _idleThread.join();
}

//# Event loop
//######################################################################################################################

void IdleUsersRoom::_idleThreadLoop() {
    spdlog::info("IdleUsersRoom::_idleThreadLoop: Idle thread loop has started.");

    Epoll epoll{true};
    //Register the new client queue's eventfd in our epoll (will signal us when new items are added into queue)
    epoll.addDescriptor(_newClientsQueue.getEvfd());
    //When new client connects...
    epoll.addEventHandler(_newClientsQueue.getEvfd(), EPOLLIN, [&epoll, this](int evfd) {
            _onNewClientConnect(evfd, epoll);
    });

    //Infinite event loop
    for (;;) {
        epoll.waitForEvents();
    }
}

//# Event handlers
//######################################################################################################################

void IdleUsersRoom::_onNewClientConnect(int evfd, Epoll &epoll) {
    spdlog::info("IdleUsersRoom::_onNewClientConnect: New client connect event captured.");

    eventfd_t counterVal;
    eventfd_read(evfd, &counterVal);
    //Capture write and disconnect events of any new client
    while (!this->_newClientsQueue.isEmpty()) {
        int clientFd = this->_newClientsQueue.pop();
        epoll.addDescriptor(clientFd);

        epoll.addEventHandler(clientFd, EPOLLIN, [this](int client) {
            _onClientWrite(client);
        });

        epoll.addEventHandler(clientFd, EPOLLRDHUP | EPOLLHUP, [this](int client) {
            _onClientDisconnect(client);
        });

        //TODO: Initialize user object?
    }
}

void IdleUsersRoom::_onClientWrite(int clientfd) {
    spdlog::info("IdleUsersRoom::_onClientWrite: Client FD{0} is ready for reading.", clientfd);
    //TODO: Implement body
}

void IdleUsersRoom::_onClientDisconnect(int clientfd) {
    spdlog::info("IdleUsersRoom::_onClientDisconnect Client FD{0} has terminated connection.", clientfd);
    //TODO: Implement body
}






