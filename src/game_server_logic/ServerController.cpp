//
// Created by xPC on 08.11.2023.
//

#include "ServerController.h"
#include "ServerConfig.h"
#include "spdlog/spdlog.h"
#include "TcpGameServer.h"
#include "IdleUsersRoom.h"

void ServerController::start(const std::string &serverIp, uint16_t serverPort, unsigned int maxPlayerCount,
                             unsigned int roomCount) {
    TcpGameServer gameServer{serverIp, serverPort};

    IdleUsersRoom idleUsersRoom{gameServer.getNewClientsQueue(), roomCount, maxPlayerCount};
    idleUsersRoom.startIdleThread();

    gameServer.joinOnAcceptThread();
    idleUsersRoom.joinOnIdleThread();
}

void ServerController::start() {
    ServerConfig serverConfig{"./server_config.txt"};
    spdlog::info("Server config loaded: serverIp={0} serverPort={1} roomCount={2} maxPlayerCount={3}",
                 serverConfig.serverIp, serverConfig.serverPort, serverConfig.roomCount, serverConfig.maxPlayerCount);
    start(serverConfig.serverIp, serverConfig.serverPort, serverConfig.roomCount, serverConfig.maxPlayerCount);
}


