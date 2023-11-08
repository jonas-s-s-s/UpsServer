//
// Created by xPC on 08.11.2023.
//
#include <gtest/gtest.h>
#include "testBaseServer.h"

class TestBasicServerFunctions : public testBaseServer {
protected:

    void SetUp() override {
        server = new ServerController{};
        client = new ClientSocket{"127.0.0.1", 3001};
    }

    void TearDown() override {
        delete server;
        delete client;
    }
};

TEST_F(TestBasicServerFunctions, Clinet_can_connect) {

}
