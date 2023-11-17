//
// Created by surface on 16.11.23.
//

#include "RoomStructs.h"

GameRoom::GameRoom(unsigned int roomId) : roomId(roomId) {}

bool GameRoom::hasUsername(const std::string &username) {
    return (this->usernameA == username) || (this->usernameB == username);
}

GameState GameRoom::getGameState() const {
    return gameState;
}

void GameRoom::setGameState(GameState gameState) {
    GameRoom::gameState = gameState;
}

unsigned int GameRoom::getUserCount() {
    if (usernameA.empty() && usernameB.empty())
        return 0;
    if (!usernameA.empty() && !usernameB.empty())
        return 2;
    return 1;
}

void GameRoom::removeUser(const std::string &username) {
    if (usernameA == username)
        usernameA = "";
    if (usernameB == username)
        usernameB = "";
}

std::pair<std::string, std::string> GameRoom::getUsers() {
    return {usernameA, usernameB};
}

const unsigned int GameRoom::getRoomId() const {
    return roomId;
}

void GameRoom::addUser(const std::string &username) {
    if (getUserCount() >= 2) {
        throw std::runtime_error(
                "GameRoom::addUser: ERROR - Cannot add user " + username + " this game already has 2 players!");
    }

    if(usernameA.empty())
        usernameA = username;
    else
        usernameB = username;
}

 EventfdQueue<std::unique_ptr<ProtocolClient>> &GameRoom::getGameInput() {
    return gameInput;
}

 EventfdQueue<GameStateChange> &GameRoom::getGameOutput() {
    return gameOutput;
}

void GameRoom::removeAllUsers() {
    usernameA = "";
    usernameB = "";
}

GameStateChange::GameStateChange(const GameState newState) : newState(newState) {}

GameStateChange::GameStateChange(const GameState newState,
                                 std::unordered_set<std::unique_ptr<ProtocolClient>> &leavingClients) : newState(
        newState), leavingClients(std::move(leavingClients)) {}
