//
// Created by xPC on 12.11.2023.
//

#include "IdleUsersRoom.h"
#include "Epoll.h"
#include "Game.h"
#include "ServerSocketBase.h"
#include "spdlog/spdlog.h"
#include <sstream>
#include <sys/epoll.h>

IdleUsersRoom::IdleUsersRoom(EventfdQueue<int>& newClientsQueue, unsigned int roomCount, unsigned int maxPlayerCount)
    : _roomCount(roomCount), _maxPlayerCount(maxPlayerCount), _newClientsQueue(newClientsQueue) {
    // Create room objects depending on the room count
    for (int i = 1; i <= _roomCount; ++i) {
        _gameRooms[i] = std::make_unique<GameRoom>(i);
    }
}

void IdleUsersRoom::startIdleThread() {
    _idleThread = std::thread(&IdleUsersRoom::_idleThreadLoop, this);
}

void IdleUsersRoom::joinOnIdleThread() {
    _idleThread.join();
}

// # Event loop
// ######################################################################################################################

void IdleUsersRoom::_idleThreadLoop() {
    spdlog::info("IdleUsersRoom::_idleThreadLoop: Idle thread loop has started.");

    // Register the new client queue's eventfd in our epoll (will signal us when new items are added into queue)
    _epoll.addDescriptor(_newClientsQueue.getEvfd());
    // When new client connects...
    _epoll.addEventHandler(_newClientsQueue.getEvfd(), EPOLLIN, [this](int evfd) { _onNewClientConnect(evfd); });

    // Infinite event loop
    for (;;) {
        _epoll.waitForEvents();
    }
}

// # Responses to user messages
// ######################################################################################################################

void IdleUsersRoom::_processClientMessage(const ProtocolData& msg, ProtocolClient& client) {
    // In idle users room we're only allowing these following types of methods:
    switch (msg.method) {
    case MethodName::TERMINATE_CONNECTION:
        _onClientDisconnect(client.getClientFd());
        return;
    case MethodName::ENTER_USERNAME:
        if (msg.hasField("username") && !client.hasName()) {
            _enterUsername(client, msg.getField("username"));
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
        if (msg.hasField("game_id") && client.hasName()) {
            _joinGameRoom(client, msg.getField("game_id"));
        } else {
            _denyRequest(client);
        }
        return;
    default:
        _denyRequest(client);
        return;
    }
}

void IdleUsersRoom::_enterUsername(ProtocolClient& client, const std::string& username) {
    //Check if there already is user with this name in the idle rom
    for (auto& clientCM: _clientsMap) {
        if(clientCM.second.get()->hasName() && clientCM.second.get()->getClientFd() != client.getClientFd()) {
            if( clientCM.second.get()->getClientName() == username) {
                _denyRequest(client);
                return;
            }
        }
    }

    //Check if user with this name exist in an active game room
    for (auto& pair: _gameRooms) {
        if(pair.second->hasUsername(username)) {
            _denyRequest(client);
            return;
        }
    }

    //Check if user with this name exists in a paused game room
    for (auto& gameRoom: _pausedGameRooms) {
        if(gameRoom->hasUsername(username)) {
            _denyRequest(client);
            return;
        }
    }

    client.setClientName(username);

    // Check if user is rejoining a paused game
    for (GameRoom* pausedRoom : _pausedGameRooms) {
        if (pausedRoom->hasUsername(username)) {
            _sendClientToGameThread(*pausedRoom, client);
            _setGameAsRunning(*pausedRoom);
            return;
        }
    }

    _acceptRequest(client);

}

void IdleUsersRoom::_joinGameRoom(ProtocolClient& client1, const std::string& gameIdStr) {
    int gameId;
    try {
        gameId = std::stoi(gameIdStr);
    } catch (std::logic_error& e) {
        _denyRequest(client1);
        return;
    }

    if (_gameRooms.count(gameId) < 1) {
        _denyRequest(client1);
        return;
    }

    std::unique_ptr<GameRoom>& thisRoom = _gameRooms.at(gameId);
    // Only idle game rooms can be joined
    if (thisRoom->getGameState() != IDLE) {
        _denyRequest(client1);
        return;
    }

    // Check if some client is already waiting for this game
    for (auto const& [client2, idleGame] : _waitingToJoinGame) {
        // The game can be started, two clients are waiting for it
        if (idleGame == gameId) {
            //Client is trying to join the same room again
            if(client1.getClientFd() == client2->getClientFd()) {
                _denyRequest(client1);
                return;
            }

            _newGameThread(client1, *client2, *thisRoom);
            _sendClientToGameThread(*thisRoom, client1);
            _sendClientToGameThread(*thisRoom, *client2);
            return;
        }
    }

    // Game cannot be started. Save this user as waiting to join.
    _waitingToJoinGame[&client1] = gameId;
    client1.sendMsg(newProtocolMessage(MethodName::GAME_IDLE));
}

void IdleUsersRoom::_newGameThread(const ProtocolClient& client1, const ProtocolClient& client2, GameRoom& room) {
    // Register outputQueue events in our epoll
    _epoll.addDescriptor(room.getGameOutput().getEvfd());
    _epoll.addEventHandler(room.getGameOutput().getEvfd(), EPOLLIN, [this, &room](int evfd) { _onGameStateChange(evfd, room); });

    _setGameAsRunning(room, client1.getClientName(), client2.getClientName());

    std::thread gt(startGame, std::ref(room.getGameInput()), std::ref(room.getGameOutput()));
    gt.detach();
}

void IdleUsersRoom::_sendClientToGameThread(GameRoom& room, ProtocolClient& client) {
    const int clientFd = client.getClientFd();
    _epoll.removeDescriptor(clientFd);
    _waitingToJoinGame.erase(&client);

    room.getGameInput().push(std::move(_clientsMap.at(clientFd)));
    _clientsMap.erase(clientFd);
}

void IdleUsersRoom::_setGameAsRunning(GameRoom& room, const std::string& username1, const std::string& username2) {
    room.setGameState(RUNNING);
    room.addUser(username1);
    room.addUser(username2);
    _pausedGameRooms.erase(&room);
}

void IdleUsersRoom::_setGameAsRunning(GameRoom& room) {
    room.setGameState(RUNNING);
    _pausedGameRooms.erase(&room);
}

void IdleUsersRoom::_setGameAsPaused(GameRoom& room) {
    room.setGameState(PAUSED);
    _pausedGameRooms.insert(&room);
}

void IdleUsersRoom::_setGameAsIdle(GameRoom& room) {
    room.setGameState(IDLE);
    room.removeAllUsers();
    _pausedGameRooms.erase(&room);
}

ProtocolData IdleUsersRoom::_getRoomList() {
    // Generate inverse map - this is necessary because we aren't using boost bidirectional map
    std::unordered_map<int, ProtocolClient*> waitingRooms{};
    for (const auto& kvPair : _waitingToJoinGame) {
        waitingRooms[kvPair.second] = kvPair.first;
    }

    std::vector<std::vector<std::string>> roomList;
    for (auto const& [roomId, roomPtr] : _gameRooms) {
        std::vector<std::string> line;

        if (waitingRooms.count(roomId) == 1) {
            line.emplace_back(waitingRooms.at(roomId)->getClientName());
            line.emplace_back("");
        } else {
            auto users = roomPtr->getUsers();
            line.emplace_back(users.first);
            line.emplace_back(users.second);
        }

        line.emplace_back(std::to_string(roomPtr->getRoomId()));
        line.emplace_back(std::to_string(roomPtr->getGameState()));
        roomList.emplace_back(line);
    }
    return newProtocolMessage(MethodName::REQ_ACCEPTED, {{"room_list", serializeObjectList(roomList)}});
}

void IdleUsersRoom::_addClientBack(std::unique_ptr<ProtocolClient>& client) {
    if (client) {
        int clientfd = client->getClientFd();
        _clientsMap[clientfd] = std::move(client);

        _epoll.addDescriptor(clientfd);
        _epoll.addEventHandler(clientfd, EPOLLIN, [this](int client) { _onClientWrite(client); });
        _epoll.addEventHandler(clientfd, EPOLLRDHUP | EPOLLHUP, [this](int client) { _onClientDisconnect(client); });

        _clientsMap[clientfd]->sendMsg(newProtocolMessage(MethodName::CONNECTED_OK));
    }
}

void IdleUsersRoom::_denyRequest(ProtocolClient& client) {
    if (client.getReqDeniedCnt() >= MAX_REQ_DENIED_CNT) {
        _onClientDisconnect(client.getClientFd());
    } else {
        client.sendReqDeniedMsg();
    }
}

void IdleUsersRoom::_acceptRequest(ProtocolClient& client) {
    client.sendMsg(newProtocolMessage(MethodName::REQ_ACCEPTED));
}

// # Event handlers
// ######################################################################################################################

void IdleUsersRoom::_onNewClientConnect(int evfd) {
    spdlog::info("IdleUsersRoom::_onNewClientConnect: New client connect event captured.");

    eventfd_t counterVal;
    eventfd_read(evfd, &counterVal);
    // Capture write and disconnect events of any new client
    while (!this->_newClientsQueue.isEmpty()) {
        int clientfd = this->_newClientsQueue.pop();

        // Create the object representing this client
        _clientsMap[clientfd] = std::make_unique<ProtocolClient>(clientfd);

        // To prevent exceeding the player limit possible solution is to disconnect
        if (_clientsMap.size() > _maxPlayerCount) {
            spdlog::warn("IdleUsersRoom::_onNewClientConnect: WARNING - MAX PLAYER COUNT EXCEEDED. REMOVING CLIENT.");
            _onClientDisconnect(clientfd);
            continue;
        }

        // Register this client with epoll
        _epoll.addDescriptor(clientfd);
        _epoll.addEventHandler(clientfd, EPOLLIN, [this](int client) { _onClientWrite(client); });
        _epoll.addEventHandler(clientfd, EPOLLRDHUP | EPOLLHUP, [this](int client) { _onClientDisconnect(client); });

        _clientsMap[clientfd]->sendMsg(newProtocolMessage(MethodName::CONNECTED_OK));
    }
}

void IdleUsersRoom::_onClientWrite(int clientfd) {
    spdlog::info("IdleUsersRoom::_onClientWrite: Client FD{0} is ready for reading.", clientfd);

    std::optional<std::vector<ProtocolData>> messages;
    const auto& client = _clientsMap.at(clientfd);

    // Catch any parsing exception which can be thrown by client
    try {
        messages = client->receiveMsg();
    } catch (const std::exception& ex) {
        std::ostringstream error_message;
        error_message << "Exception while reading client message.\n"
                      << "Force terminating client FD" << client->getClientFd() << " username: " << client->getClientName() << "\n"
                      << "Exception name: " << typeid(ex).name() << "\n"
                      << "Exception message: " << ex.what() << "\n"
                      << "Errno: " << errno << "\n";
        spdlog::error(error_message.str());
        // Kill connection
        _onClientDisconnect(client->getClientFd());
        return;
    }

    if (messages.has_value()) {
        // Read all messages sent by this client
        for (const ProtocolData& msg : messages.value()) {
            // Response to user messages is generated in another function
            _processClientMessage(msg, *client);
        }
    }
}

void IdleUsersRoom::_onClientDisconnect(int clientfd) {
    spdlog::info("IdleUsersRoom::_onClientDisconnect Client FD{0} has terminated connection.", clientfd);
    // Remove the socket from our epoll
    _epoll.removeDescriptor(clientfd);

    if (_clientsMap.count(clientfd) == 1) {
        // Clear any possible waiting request
        _waitingToJoinGame.erase(_clientsMap.at(clientfd).get());
        // Remove client's object - SocketBase destructor will close the TCP socket
        _clientsMap.erase(clientfd);
    }
}

void IdleUsersRoom::_onGameStateChange(int evfd, GameRoom& room) {
    eventfd_t counterVal;
    eventfd_read(evfd, &counterVal);

    while (!room.gameOutput.isEmpty()) {
        GameStateChange newState = room.getGameOutput().pop();

        switch (newState.newState) {
        case IDLE:
            spdlog::info("IdleUsersRoom::_onGameStateChange: GameRoom #{0} has changed its state to IDLE.", room.getRoomId());

            // Add any clients sent back from game room
            _addClientBack(newState.client1);
            _addClientBack(newState.client2);

            _setGameAsIdle(room);
            break;
        case RUNNING:
            spdlog::info("IdleUsersRoom::_onGameStateChange: GameRoom #{0} has changed its state to RUNNING.", room.getRoomId());
            // This is unused at the moment
            break;
        case PAUSED:
            spdlog::info("IdleUsersRoom::_onGameStateChange: GameRoom #{0} has changed its state to PAUSED.", room.getRoomId());
            _setGameAsPaused(room);
            break;
        }
    }
}