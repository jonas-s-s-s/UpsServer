//
// Created by xPC on 08.11.2023.
//

#include "ServerController.h"
#include "ServerConfig.h"
#include "spdlog/spdlog.h"
#include "TcpGameServer.h"
#include "IdleUsersRoom.h"

void ServerController::start() {
    ServerConfig serverConfig{"./server_config.txt"};
    spdlog::info("Server config loaded: serverIp={0} serverPort={1} roomCount={2} maxPlayerCount={3}",
                 serverConfig.serverIp, serverConfig.serverPort, serverConfig.roomCount, serverConfig.maxPlayerCount);

    TcpGameServer gameServer{serverConfig.serverIp, serverConfig.serverPort};

    IdleUsersRoom idleUsersRoom{gameServer.getClientSocketQ()};

    gameServer.joinOnAcceptThread();
}