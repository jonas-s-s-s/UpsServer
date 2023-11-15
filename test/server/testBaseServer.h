//
// Created by xPC on 08.11.2023.
//

#pragma once

#include <gtest/gtest.h>
#include <thread>
#include "ServerController.h"
#include "ClientSocket.h"

class testBaseServer : public ::testing::Test {
protected:
    ServerController server = ServerController{};
    std::thread t1 = std::thread([this] { this->server.start("127.0.0.1", 3001, 3000,60); });
    ClientSocket* client;
};
