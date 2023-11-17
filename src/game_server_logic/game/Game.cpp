//
// Created by xPC on 17.11.2023.
//

#include "Game.h"
#include "spdlog/spdlog.h"

void startGame(EventfdQueue<std::unique_ptr<ProtocolClient>> &gameInput, EventfdQueue<GameStateChange> &gameOutput) {
    Game game = Game{gameInput, gameOutput};
    game.startGameLoop();
}

Game::Game(EventfdQueue<std::unique_ptr<ProtocolClient>> &gameInput, EventfdQueue<GameStateChange> &gameOutput)
        : _gameInput(gameInput), _gameOutput(gameOutput) {}

//# Event loop
//######################################################################################################################

void Game::startGameLoop() {
    _epoll.addDescriptor(_gameInput.getEvfd());
    _epoll.addEventHandler(_gameInput.getEvfd(), EPOLLIN, [this](int evfd) {
        _onNewClientConnect(evfd);
    });

    //Infinite event loop
    for (;;) {
        _epoll.waitForEvents();
    }
    //TODO: Global exception handling
}

//# Event Handlers
//######################################################################################################################


void Game::_onNewClientConnect(int evfd) {
    spdlog::info("Game::_onNewClientConnect: A new has the joined game.");

    eventfd_t counterVal;
    eventfd_read(evfd, &counterVal);
    //Capture write and disconnect events of client
    while (!this->_gameInput.isEmpty()) {

        ProtocolClient* client;
        if (!player1.client) {
            player1.client = this->_gameInput.pop();
            client = player1.client.get();
        } else if (!player2.client) {
            player2.client = this->_gameInput.pop();
            client = player2.client.get();
        } else {
            throw std::runtime_error("Game::_onNewClientConnect: ERROR - Game already has two players.");
        }

        int fd = client->getClientFd();
        //Register this client with epoll
        _epoll.addDescriptor(fd);
        _epoll.addEventHandler(fd, EPOLLIN, [this](int client) {
            _onClientWrite(client);
        });
        _epoll.addEventHandler(fd, EPOLLRDHUP | EPOLLHUP, [this](int client) {
            _onClientDisconnect(client);
        });

        client->sendMsg(newProtocolMessage(MethodName::GAME_JOINED_OK));
    }
}

void Game::_onClientWrite(int clientfd) {
    spdlog::info("Game::_onClientWrite: Client FD{0} is ready for reading.", clientfd);

}

void Game::_onClientDisconnect(int clientfd) {
    spdlog::info("Game::_onClientDisconnect Client FD{0} has terminated connection.", clientfd);

}