//
// Created by xPC on 17.11.2023.
//

#pragma once

#include "Epoll.h"
#include "EventfdQueue.h"
#include "GameGraph.h"
#include "ProtocolClient.h"
#include "RoomStructs.h"
#include <memory>

void startGame(EventfdQueue<std::unique_ptr<ProtocolClient>>& gameInput, EventfdQueue<GameStateChange>& gameOutput);

class Player {
  public:
    constexpr static uint64_t MAX_INACTIVITY_MS = 30000;
    GameGraph myGraph{};
    std::unique_ptr<ProtocolClient> client;
    uint64_t lastSeen = 0;
};

class Game {
  public:
    Game(EventfdQueue<std::unique_ptr<ProtocolClient>>& gameInput, EventfdQueue<GameStateChange>& gameOutput);

    void startGameLoop();

    virtual ~Game();

    Player& getPlayer(const std::string& username);

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

    EventfdQueue<std::unique_ptr<ProtocolClient>>& _gameInput;
    EventfdQueue<GameStateChange>& _gameOutput;
    Epoll _epoll{true};

    void _clearGame();

    std::unordered_map<std::string, std::string> _serializeGameData();

    void _onNewClientConnect(int evfd);

    void _onClientWrite(int clientfd);

    void _onClientDisconnect(int clientfd);

    Player& getPlayer(int clientFd);

    void _processClientMessage(const ProtocolData& data, ProtocolClient& client);

    void _acceptRequest(ProtocolClient& client);

    void _denyRequest(ProtocolClient& client);

    void _endGame();

    void _processGameCommand(const ProtocolData& data, ProtocolClient& client);

    void _clientPing(ProtocolClient& client);

    void _updateClientLastSeen(ProtocolClient& client);
};
