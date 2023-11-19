//
// Created by xPC on 17.11.2023.
//

#pragma once

#include <memory>
#include "EventfdQueue.h"
#include "ProtocolClient.h"
#include "RoomStructs.h"
#include "Epoll.h"

void startGame(EventfdQueue<std::unique_ptr<ProtocolClient>> &gameInput, EventfdQueue<GameStateChange> &gameOutput);

class GameGraph {
public:
    constexpr static unsigned int GAME_NODE_NUM = 6;

    bool isValidEdge(unsigned int source, unsigned int destination);

    bool hasUndirectedEdge(unsigned int source, unsigned int destination);

    void addUndirectedEdge(unsigned int source, unsigned int destination);

    bool hasCycleOfLength3();

    unsigned int findShortestCycle();

    std::string serializeUndirectedEdges();

    void clear();
private:
    unsigned int _findShortestCycle(unsigned int node, int parent, int d, std::array<int, GAME_NODE_NUM> depth);

    unsigned int _adjacencyMatrix[GAME_NODE_NUM][GAME_NODE_NUM]{};
};

class Player {
public:
    GameGraph myGraph{};
    std::unique_ptr<ProtocolClient> client;
};

class Game {
public:
    Game(EventfdQueue<std::unique_ptr<ProtocolClient>> &gameInput, EventfdQueue<GameStateChange> &gameOutput);

    void startGameLoop();

    virtual ~Game();

    Player &getPlayer(const std::string &username);

    bool bothPlayersOnline() const;

    bool playerOneOnline() const;

    bool playerTwoOnline() const;

private:
    constexpr static int MAX_REQ_DENIED_CNT = 5;

    bool isRunning = true;

    Player _player1;
    Player _player2;
    bool _isPlayerOneTurn = true;
    bool _isPaused = false;

    EventfdQueue<std::unique_ptr<ProtocolClient>> &_gameInput;
    EventfdQueue<GameStateChange> &_gameOutput;
    Epoll _epoll{true};

    void _clearGame();

    std::unordered_map<std::string, std::string> _serializeGameData();

    void _onNewClientConnect(int evfd);

    void _onClientWrite(int clientfd);

    void _onClientDisconnect(int clientfd);

    Player &getPlayer(int clientFd);

    void _processClientMessage(const ProtocolData &data, ProtocolClient &client);

    void _acceptRequest(ProtocolClient &client);

    void _denyRequest(ProtocolClient &client);

    void _endGame();

    void _processGameCommand(const ProtocolData &data, ProtocolClient &client);

    void _clientPing(ProtocolClient &client);

    void _updateClientLastSeen(ProtocolClient &client);
};
