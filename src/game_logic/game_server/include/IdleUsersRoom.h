//
// Created by xPC on 12.11.2023.
//

#pragma once

#include "Epoll.h"
#include "EventfdQueue.h"
#include "ProtocolClient.h"
#include "RoomStructs.h"
#include <thread>

class IdleUsersRoom {
  public:
    explicit IdleUsersRoom(EventfdQueue<int>& newClientsQueue, unsigned int roomCount, unsigned int maxPlayerCount);

    void startIdleThread();

    void joinOnIdleThread();

  private:
    // The maximal amount of invalid requests, after which we'll disconnect the client
    static constexpr int MAX_REQ_DENIED_CNT = 5;
    const unsigned int _roomCount;
    const unsigned int _maxPlayerCount;

    // The epoll object used by this room
    Epoll _epoll{true};

    void _idleThreadLoop();

    void _processClientMessage(const ProtocolData& msg, ProtocolClient& client);

    void _joinGameRoom(ProtocolClient& client1, const std::string& gameIdStr);

    ProtocolData _getRoomList();

    void _denyRequest(ProtocolClient& client);

    static void _acceptRequest(ProtocolClient& client);

    void _onNewClientConnect(int evfd);

    void _onClientWrite(int clientfd);

    void _onClientDisconnect(int clientfd);

    void _onGameStateChange(int evfd, GameRoom& room);

    void _setGameAsRunning(GameRoom& room, const std::string& username1, const std::string& username2);

    void _setGameAsRunning(GameRoom& room);

    void _setGameAsPaused(GameRoom& room);

    void _setGameAsIdle(GameRoom& room);

    void _enterUsername(ProtocolClient& client, const std::string& username);

    void _sendClientToGameThread(GameRoom& room, ProtocolClient& client);

    void _newGameThread(const ProtocolClient& client1, const ProtocolClient& client2, GameRoom& room);

    void _addClientBack(std::unique_ptr<ProtocolClient>& client);

    EventfdQueue<int>& _newClientsQueue;
    std::thread _idleThread;

    // State fields
    // ##################################################################################################################
    // Contains pointer to every client who is in the idle room
    std::unordered_map<int, std::unique_ptr<ProtocolClient>> _clientsMap;
    // Should be initialized with _roomCount objects when idle room is constructed
    std::map<int, std::unique_ptr<GameRoom>> _gameRooms;
    // Pointers to paused games are written here, so they can be searched when client reconnects (no need for smart ptr here)
    std::unordered_set<GameRoom*> _pausedGameRooms;
    /*Maps a clients to certain gameId - client is waiting to join this game,
    once two clients start waiting for the same game, a new game is started.
    A boost bidirectional map could possibly be a more efficient solution*/
    std::unordered_map<ProtocolClient*, int> _waitingToJoinGame;
};
