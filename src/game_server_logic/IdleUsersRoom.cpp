//
// Created by xPC on 12.11.2023.
//

#include <sys/epoll.h>
#include <sstream>
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

    //Register the new client queue's eventfd in our epoll (will signal us when new items are added into queue)
    _epoll.addDescriptor(_newClientsQueue.getEvfd());
    //When new client connects...
    _epoll.addEventHandler(_newClientsQueue.getEvfd(), EPOLLIN, [this](int evfd) {
        _onNewClientConnect(evfd);
    });

    //Infinite event loop
    for (;;) {
        _epoll.waitForEvents();
    }
}

//# Responses to user messages
//######################################################################################################################

void IdleUsersRoom::_processClientMessage(const ProtocolData &msg, ProtocolClient &client) {
    //In idle users room we're only allowing these following types of methods:
    switch (msg.method) {
        case MethodName::TERMINATE_CONNECTION:
            _onClientDisconnect(client.getClientFd());
            return;
        case MethodName::ENTER_USERNAME:
            if (msg.hasField("username")) {
                client.setClientName(msg.getField("username"));
                _acceptRequest(client);
            } else {
                _denyRequest(client);
            }
            return;
        case MethodName::GET_ROOM_LIST:
            if (client.hasName()) {
                client.sendMsg(_getRoomList());
            } else {
                _denyRequest(client);
            }
            return;
        case MethodName::JOIN_GAME:
            if (client.hasName()) {
                //TODO: Implement game join
                _acceptRequest(client);
            } else {
                _denyRequest(client);
            }
            return;
        default:
            _denyRequest(client);
            return;
    }
}

ProtocolData IdleUsersRoom::_getRoomList() {
    //TODO: Implement this function
    return newProtocolMessage(MethodName::REQ_ACCEPTED);
}

void IdleUsersRoom::_denyRequest(ProtocolClient &client) {
    if (client.getReqDeniedCnt() >= MAX_REQ_DENIED_CNT) {
        _onClientDisconnect(client.getClientFd());
    } else {
        client.sendReqDeniedMsg();
    }
}

void IdleUsersRoom::_acceptRequest(ProtocolClient &client) {
    client.sendMsg(newProtocolMessage(MethodName::REQ_ACCEPTED));
}

//# Event handlers
//######################################################################################################################

void IdleUsersRoom::_onNewClientConnect(int evfd) {
    spdlog::info("IdleUsersRoom::_onNewClientConnect: New client connect event captured.");

    eventfd_t counterVal;
    eventfd_read(evfd, &counterVal);
    //Capture write and disconnect events of any new client
    while (!this->_newClientsQueue.isEmpty()) {
        int clientfd = this->_newClientsQueue.pop();
        _epoll.addDescriptor(clientfd);

        _epoll.addEventHandler(clientfd, EPOLLIN, [this](int client) {
            _onClientWrite(client);
        });

        _epoll.addEventHandler(clientfd, EPOLLRDHUP | EPOLLHUP, [this](int client) {
            _onClientDisconnect(client);
        });

        //Create the object representing this client
        clientsMap[clientfd] = std::make_unique<ProtocolClient>(clientfd);
        clientsMap[clientfd]->sendMsg(newProtocolMessage(MethodName::CONNECTED_OK));
    }
}

void IdleUsersRoom::_onClientWrite(int clientfd) {
    spdlog::info("IdleUsersRoom::_onClientWrite: Client FD{0} is ready for reading.", clientfd);

    std::optional<std::vector<ProtocolData>> messages;
    const auto &client = clientsMap.at(clientfd);

    //Catch any parsing exception which can be thrown by client
    try {
        messages = client->receiveMsg();
    } catch (const std::exception &ex) {
        std::ostringstream error_message;
        error_message << "Exception while reading client message.\n"
                      << "Force terminating client FD" << client->getClientFd() << " username: "
                      << client->getClientName() << "\n"
                      << "Exception name: " << typeid(ex).name() << "\n"
                      << "Exception message: " << ex.what() << "\n"
                      << "Errno: " << errno << "\n";
        spdlog::error(error_message.str());
        //Kill connection
        _onClientDisconnect(client->getClientFd());
        return;
    }

    if (messages.has_value()) {
        //Read all messages sent by this client
        for (const ProtocolData &msg: messages.value()) {
            //Response to user messages is generated in another function
            _processClientMessage(msg, *client);
        }
    }
}

void IdleUsersRoom::_onClientDisconnect(int clientfd) {
    spdlog::info("IdleUsersRoom::_onClientDisconnect Client FD{0} has terminated connection.", clientfd);
    //Remove the socket from our epoll
    _epoll.removeDescriptor(clientfd);
    //Remove client's object - SocketBase destructor will close the TCP socket
    clientsMap.erase(clientfd);
}
