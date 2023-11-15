//
// Created by xPC on 08.11.2023.
//
#include <gtest/gtest.h>
#include "testBaseServer.h"

class TestBasicServerFunctions : public testBaseServer {
protected:

    void SetUp() override {
        client = new ClientSocket{"127.0.0.1", 3001};
        t1.detach();
    }

    void TearDown() override {
        delete client;
    }
};

TEST_F(TestBasicServerFunctions, Clinet_can_connect) {
    const std::string &res = client->receiveData();
    ASSERT_EQ(res,"CONNECTED_OK\n\r\n\r\n");
}

TEST_F(TestBasicServerFunctions, Clinet_can_enter_username) {
    client->receiveData();
    client->sendData("ENTER_USERNAME\nusername:myuser1\n\r\n\r\n");
    const std::string &res = client->receiveData();
    ASSERT_EQ(res,"REQ_ACCEPTED\n\r\n\r\n");
}