//
// Created by xPC on 08.11.2023.
//

#pragma once

#include <gtest/gtest.h>
#include "ServerController.h"
#include "ClientSocket.h"

class testBaseServer : public ::testing::Test {
protected:
    ServerController* server;
    ClientSocket* client;
};
