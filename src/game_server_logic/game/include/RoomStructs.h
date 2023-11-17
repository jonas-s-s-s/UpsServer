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

/**
 * This class is used to signal idle room each time the game's state is somehow changed.
 */
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

/**
 * This class represents a game room object which carries all data idle users room needs to know about the game.
 * Idle users room modifies this object each time it receives GameStateChange from the game thread.
 */
class GameRoom {
public:
    explicit GameRoom(unsigned int roomId);

    bool hasUsername(const std::string &username);

    void removeUser(const std::string &username);

    void removeAllUsers();

    void addUser(const std::string &username);

    EventfdQueue<std::unique_ptr<ProtocolClient>> &getGameInput();

    EventfdQueue<GameStateChange> &getGameOutput();

    //IdleRoom can transfer clients to game room via this queue
    EventfdQueue<std::unique_ptr<ProtocolClient>> gameInput{};
    //GameRoom informs the IdleRoom of its state via this queue
    EventfdQueue<GameStateChange> gameOutput{};

    //Get + set
    //##################################################################################################################
    unsigned int getUserCount();

    std::pair<std::string, std::string> getUsers();

    GameState getGameState() const;

    const unsigned int getRoomId() const;

    void setGameState(GameState gameState);

private:
    const unsigned int roomId;
    std::string usernameA;
    std::string usernameB;
    GameState gameState = IDLE;
};
