//
// Created by xPC on 17.11.2023.
//

/**
 * TODO:
 * 1) -------Join and start playing game?
 * 2) --------Game terminated - end of thread - return users to idle room
 * 3) --------Game paused - one of the users disconnected
 * 4) Ping and idle user disconnect mechanism - will also cause paused game to be terminated after certain period
 */

#include <sstream>
#include "Game.h"
#include "spdlog/spdlog.h"

void startGame(EventfdQueue<std::unique_ptr<ProtocolClient>> &gameInput, EventfdQueue<GameStateChange> &gameOutput) {
    auto game = Game{gameInput, gameOutput};
    game.startGameLoop();
}

Game::Game(EventfdQueue<std::unique_ptr<ProtocolClient>> &gameInput, EventfdQueue<GameStateChange> &gameOutput)
        : _gameInput(gameInput), _gameOutput(gameOutput) {}

Game::~Game() {
    //Game has been terminated. Return users to idle thread. (if the player has left, his client will be nullptr)
    _gameOutput.push(GameStateChange{IDLE, _player1.client, _player2.client});
}

//# Event loop
//######################################################################################################################

void Game::startGameLoop() {
    try {
        _epoll.addDescriptor(_gameInput.getEvfd());
        _epoll.addEventHandler(_gameInput.getEvfd(), EPOLLIN, [this](int evfd) {
            _onNewClientConnect(evfd);
        });

        //Infinite event loop
        while (isRunning) {
            _epoll.waitForEvents();
        }

    } catch (const std::exception &ex) {
        std::ostringstream error_message;
        error_message << "Exception in game thread.\n"
                      << "Force terminating game thread.\n"
                      << "Exception name: " << typeid(ex).name() << "\n"
                      << "Exception message: " << ex.what() << "\n"
                      << "Errno: " << errno << "\n";
        spdlog::error(error_message.str());

        //After this the object will destruct and any clients will be returned to idle thread.
        isRunning = false;
    }

}

//# Response to client messages
//######################################################################################################################
void Game::_processClientMessage(const ProtocolData &data, ProtocolClient &client) {
    _updateClientLastSeen(client);
    switch (data.method) {
        case MethodName::GAME_COMMAND:
            _processGameCommand(data, client);
            break;
        case MethodName::GAME_LEAVE:
            _endGame();
            break;
        case MethodName::GAME_PING:
            _clientPing(client);
            break;
        default:
            _denyRequest(client);
            break;
    }
}

void Game::_endGame() {
    spdlog::info("Game::_endGame: Game is ending. One of the players sent GAME_LEAVE.");
    isRunning = false;
}

void Game::_processGameCommand(const ProtocolData &data, ProtocolClient &client) {
    spdlog::info("Game::_processGameCommand: Player {0} has sent a game commmand.", client.getClientName());
    //Don't do anything if the game is paused
    if (_isPaused) {
        _denyRequest(client);
        return;
    }

    Player &player = getPlayer(client.getClientFd());

    //Check if it's this player's turn or not
    if (!_isPlayerOneTurn && (_player1.client->getClientFd() == player.client->getClientFd())) {
        //It's not player1's turn
        _denyRequest(client);
        return;
    }
    if (_isPlayerOneTurn && (_player2.client->getClientFd() == player.client->getClientFd())) {
        //It's not player2's turn
        _denyRequest(client);
        return;
    }

    if (!data.hasField("add_edge")) {
        _denyRequest(client);
        return;
    }

    //Edge field should be in the format {<0-5>,<0-5>}
    std::string edge = data.getField("add_edge");
    std::string strEdgeSource = edge.substr(1, 1);
    std::string strEdgeDestination = edge.substr(3, 1);

    //Convert the argument into unsigned ints
    unsigned int edgeSource;
    unsigned int edgeDestination;
    try {
        int t1 = std::stoi(strEdgeSource);
        int t2 = std::stoi(strEdgeDestination);
        if (t1 >= 0 && t2 >= 0) {
            edgeSource = t1;
            edgeDestination = t2;
        } else {
            _denyRequest(client);
            return;
        }
    } catch (...) {
        _denyRequest(client);
        return;
    }

    //Is this a valid edge in current player's graph?
    if (!player.myGraph.isValidEdge(edgeSource, edgeDestination)) {
        _denyRequest(client);
        return;
    }

    //Does player1's graph or player2's graph already have this edge?
    if (_player1.myGraph.hasUndirectedEdge(edgeSource, edgeDestination) ||
        _player2.myGraph.hasUndirectedEdge(edgeSource, edgeDestination)) {
        _denyRequest(client);
        return;
    }

    player.myGraph.addUndirectedEdge(edgeSource, edgeDestination);

    //If player's graph has a cycle of length 3, then he loses the game
    bool playerLost = player.myGraph.hasCycleOfLength3();

    _isPlayerOneTurn = !_isPlayerOneTurn;
    //Send back new game state to both players
    std::unordered_map<std::string, std::string> gameData = _serializeGameData();
    if (playerLost) {
        gameData["winning_player"] = (_player1.client->getClientFd() == player.client->getClientFd())
                                     ? _player2.client->getClientName() : _player1.client->getClientName();
    }
    _player1.client.get()->sendMsg(ProtocolData{MethodName::GAME_STATE, gameData});
    _player2.client.get()->sendMsg(ProtocolData{MethodName::GAME_STATE, gameData});

    if (playerLost) {
        _clearGame();
    }
}

void Game::_clientPing(ProtocolClient &client) {
    //TODO: SEND BACK PING TO CLIENT

}

void Game::_updateClientLastSeen(ProtocolClient &client) {
    //TODO: UPDATE CLIENTS TIMESTAMP
    //HERE IS THE ONLY PLACE WHERE TIMESTAMPS AND DISCONNECTS IF TIMESTAMP EXCEEDED ARE MANAGED!
}

void Game::_denyRequest(ProtocolClient &client) {
    if (client.getReqDeniedCnt() >= MAX_REQ_DENIED_CNT) {
        _onClientDisconnect(client.getClientFd());
    } else {
        client.sendReqDeniedMsg();
    }
}

void Game::_acceptRequest(ProtocolClient &client) {
    client.sendMsg(newProtocolMessage(MethodName::REQ_ACCEPTED));
}

//# Event Handlers
//######################################################################################################################

void Game::_onNewClientConnect(int evfd) {
    spdlog::info("Game::_onNewClientConnect: A new player has the joined game.");

    eventfd_t counterVal;
    eventfd_read(evfd, &counterVal);
    //Capture write and disconnect events of client
    while (!this->_gameInput.isEmpty()) {

        ProtocolClient const *client;
        if (!playerOneOnline()) {
            _player1.client = this->_gameInput.pop();
            client = _player1.client.get();
        } else if (!playerTwoOnline()) {
            _player2.client = this->_gameInput.pop();
            client = _player2.client.get();
        } else {
            throw std::runtime_error("Game::_onNewClientConnect: ERROR - Game already has two players.");
        }

        int fd = client->getClientFd();
        //Register this client with epoll
        _epoll.addDescriptor(fd);
        _epoll.addEventHandler(fd, EPOLLIN, [this](int clientfd) {
            _onClientWrite(clientfd);
        });
        _epoll.addEventHandler(fd, EPOLLRDHUP | EPOLLHUP, [this](int clientfd) {
            _onClientDisconnect(clientfd);
        });

        if (bothPlayersOnline()) {
            _isPaused = false;
            auto gameData = _serializeGameData();
            _player1.client.get()->sendMsg(ProtocolData{MethodName::GAME_STATE, gameData});
            _player2.client.get()->sendMsg(ProtocolData{MethodName::GAME_STATE, gameData});
        } else {
            client->sendMsg(newProtocolMessage(MethodName::GAME_JOINED_OK));
        }

    }
}

void Game::_onClientWrite(int clientfd) {
    spdlog::info("Game::_onClientWrite: Client FD{0} is ready for reading.", clientfd);

    std::optional<std::vector<ProtocolData>> messages;
    ProtocolClient *client;
    client = getPlayer(clientfd).client.get();

    //Catch any parsing exception which can be thrown by client
    try {
        messages = client->receiveMsg();
    } catch (const std::exception &ex) {
        std::ostringstream error_message;
        error_message << "Game::_onClientWrite: Exception while reading client message.\n"
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

void Game::_onClientDisconnect(int clientfd) {
    spdlog::info("Game::_onClientDisconnect: Client FD{0} has terminated connection.", clientfd);

    if (playerOneOnline() && _player1.client->getClientFd() == clientfd)
        _player1.client = nullptr;

    if (playerTwoOnline() && _player2.client->getClientFd() == clientfd)
        _player2.client = nullptr;

    //Both players are gone - terminate game
    if (!playerOneOnline() && !playerTwoOnline()) {
        spdlog::info("Game::_onClientDisconnect: Game is ending, both player have left.");
        isRunning = false;
        return;
    }

    //Only one player is missing. Set the game into "PAUSED" mode
    _gameOutput.push(GameStateChange{PAUSED});
    _isPaused = true;
    if (playerOneOnline())
        _player1.client.get()->sendMsg(ProtocolData{MethodName::GAME_STATE, _serializeGameData()});
    if (playerTwoOnline())
        _player2.client.get()->sendMsg(ProtocolData{MethodName::GAME_STATE, _serializeGameData()});
}

//# Other Game class methods
//######################################################################################################################

bool Game::bothPlayersOnline() const {
    if (playerOneOnline() && playerTwoOnline()) {
        return true;
    }
    return false;
}

bool Game::playerOneOnline() const {
    if (_player1.client) {
        return true;
    }
    return false;
}

bool Game::playerTwoOnline() const {
    if (_player2.client) {
        return true;
    }
    return false;
}

Player &Game::getPlayer(const std::string &username) {
    if (playerOneOnline() && _player1.client->getClientName() == username) {
        return _player1;
    } else if (playerTwoOnline() && _player2.client->getClientName() == username) {
        return _player2;
    } else {
        throw std::runtime_error("Game::getPlayer: ERROR - Unknown username: " + username);
    }
}

Player &Game::getPlayer(int clientFd) {
    if (playerOneOnline() && _player1.client->getClientFd() == clientFd) {
        return _player1;
    } else if (playerTwoOnline() && _player2.client->getClientFd() == clientFd) {
        return _player2;
    } else {
        throw std::runtime_error("Game::getPlayer: ERROR - Unknown clientfd: " + std::to_string(clientFd));
    }
}

std::unordered_map<std::string, std::string> Game::_serializeGameData() {
    std::unordered_map<std::string, std::string> data{};
    data["game_state"] = (_isPaused) ? "PAUSED" : "RUNNING";
    data["player1_username"] = (playerOneOnline()) ? _player1.client->getClientName() : "";
    data["player2_username"] = (playerTwoOnline()) ? _player2.client->getClientName() : "";
    data["player1_edges"] = _player1.myGraph.serializeUndirectedEdges();
    data["player2_edges"] = _player2.myGraph.serializeUndirectedEdges();
    if (bothPlayersOnline())
        data["on_turn"] = (_isPlayerOneTurn) ? _player1.client->getClientName() : _player2.client->getClientName();

    return data;
}

void Game::_clearGame() {
    _player1.myGraph.clear();
    _player2.myGraph.clear();
}

//# Player class methods
//######################################################################################################################


//# GameGraph class methods
//######################################################################################################################
bool GameGraph::isValidEdge(unsigned int source, unsigned int destination) {
    //1 node cycles are not allowed
    if (source == destination)
        return false;

    if (source >= GAME_NODE_NUM || destination >= GAME_NODE_NUM)
        return false;

    return true;
}

bool GameGraph::hasUndirectedEdge(unsigned int source, unsigned int destination) {
    if (!isValidEdge(source, destination))
        throw std::runtime_error(
                "GameGraph::hasUndirectedEdge: ERROR - [" + std::to_string(source) + "," + std::to_string(destination) +
                "] is not a valid edge.");

    if (_adjacencyMatrix[source][destination] == 1 && _adjacencyMatrix[destination][source] == 1)
        return true;

    return false;
}

void GameGraph::addUndirectedEdge(unsigned int source, unsigned int destination) {
    _adjacencyMatrix[source][destination] = 1;
    _adjacencyMatrix[destination][source] = 1;
}

bool GameGraph::hasCycleOfLength3() {
    return findShortestCycle() == 3;
}

unsigned int GameGraph::_findShortestCycle(unsigned int node, int parent, int d, std::array<int, GAME_NODE_NUM> depth) {
    if (depth[node] != -1) {
        return d - depth[node];
    }
    depth[node] = d;

    unsigned int answer = INT_MAX;
    //for each neighbor of node
    for (int v = 0; v < GAME_NODE_NUM; v++) {
        //ignore parent and non-neighbors
        if (v != parent && _adjacencyMatrix[node][v] != 0) {
            answer = std::min(answer, _findShortestCycle(v, node, d + 1, depth));
        }
    }
    return answer;
}

unsigned int GameGraph::findShortestCycle() {
    std::array<int, GAME_NODE_NUM> depth{};
    depth.fill(-1);
    return _findShortestCycle(0, -1, 1, depth);
}

std::string GameGraph::serializeUndirectedEdges() {
    std::string result;
    for (int i = 0; i < GAME_NODE_NUM; ++i) {
        for (int j = 0; j < GAME_NODE_NUM; ++j) {
            if (j <= i)
                continue;
            if (!hasUndirectedEdge(i, j))
                continue;
            result += "{" + std::to_string(i) + "," + std::to_string(j) + "}";
            if (i == GAME_NODE_NUM - 2 && j == GAME_NODE_NUM - 1)
                continue;
            result += ",";
        }
    }
    return result;
}

void GameGraph::clear() {
    for (int i = 0; i < GAME_NODE_NUM; ++i) {
        for (int j = 0; j < GAME_NODE_NUM; ++j) {
            _adjacencyMatrix[i][j] = 0;
        }
    }
}
