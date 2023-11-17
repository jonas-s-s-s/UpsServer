//
// Created by xPC on 17.11.2023.
//

#pragma once

#include <memory>
#include "EventfdQueue.h"
#include "ProtocolClient.h"
#include "RoomStructs.h"
#include "Epoll.h"

void startGame(EventfdQueue<std::unique_ptr<ProtocolClient>>& gameInput, EventfdQueue<GameStateChange>& gameOutput);

class Player {
public:

    std::unique_ptr<ProtocolClient> client;
};

class Game {
public:
    Game(EventfdQueue<std::unique_ptr<ProtocolClient>> &gameInput, EventfdQueue<GameStateChange> &gameOutput);

    void startGameLoop();
private:
    Player player1;
    Player player2;

    EventfdQueue<std::unique_ptr<ProtocolClient>>& _gameInput;
    EventfdQueue<GameStateChange>& _gameOutput;
    Epoll _epoll{true};

    void _onNewClientConnect(int evfd);

    void _onClientWrite(int clientfd);

    void _onClientDisconnect(int clientfd);
};
