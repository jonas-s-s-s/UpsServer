//
// Created by xPC on 12.11.2023.
//

#pragma once

#include <thread>
#include "EventfdQueue.h"
#include "Epoll.h"
#include "ProtocolClient.h"

class IdleUsersRoom {
public:
    explicit IdleUsersRoom(EventfdQueue<int> &newClientsQ);

    void startIdleThread();

    void joinOnIdleThread();

private:
    //The maximal amount of invalid requests, after which we'll disconnect the client
    static constexpr int MAX_REQ_DENIED_CNT = 5;
    //The epoll object used by this room
    Epoll _epoll{true};

    void _idleThreadLoop();

    void _processClientMessage(const ProtocolData& msg, ProtocolClient& client);

    ProtocolData _getRoomList();

    void _denyRequest(ProtocolClient &client);

    static void _acceptRequest(ProtocolClient &client);

    void _onNewClientConnect(int evfd);

    void _onClientWrite(int clientfd);

    void _onClientDisconnect(int clientfd);

    EventfdQueue<int> &_newClientsQueue;
    std::thread _idleThread;

    //State fields
    std::unordered_map<int, std::unique_ptr<ProtocolClient>> clientsMap;
};
