//
// Created by surface on 16.11.23.
//

#pragma once

#include <string>
#include <memory>
#include <unordered_set>
#include "EventfdQueue.h"
#include "ProtocolClient.h"

enum GameState {
    IDLE, RUNNING, PAUSED
};

class GameStateChange {
public:
    explicit GameStateChange(GameState newState);

    GameStateChange(GameState newState, std::unordered_set<std::unique_ptr<ProtocolClient>> &leavingClients);

    const GameState newState;
    //One of the clients, or both can return to the idle room, their pointers are stored here
    std::unordered_set<std::unique_ptr<ProtocolClient>> leavingClients{};
    //One of the clients can disconnect from a running game, changing its state to PAUSED, we need to know his username
    std::string disconnectedUsername;
};

class GameRoom {
public:
    explicit GameRoom(unsigned int roomId);

    bool hasUsername(const std::string &username);

    unsigned int getUserCount();

    void removeUser(const std::string &username);

    std::pair<std::string, std::string> getUsers();

    GameState getGameState() const;

    void setGameState(GameState gameState);

    //IdleRoom can transfer clients to game room via this queue
    EventfdQueue<std::unique_ptr<ProtocolClient>> gameInput{};
    //GameRoom informs the IdleRoom of its state via this queue
    EventfdQueue<GameStateChange> gameOutput{};
private:
    const unsigned int roomId;
    std::string usernameA;
    std::string usernameB;
    GameState gameState = IDLE;
};
