//
// Created by xPC on 08.11.2023.
//
#include <gtest/gtest.h>
#include "testBaseServerOneClient.h"

class TestServerOneClient : public testBaseServerOneClient {
protected:

    void SetUp() override {
        client = new ClientSocket{"127.0.0.1", 3001};
        t1.detach();
    }

    void TearDown() override {
        delete client;
    }
};

TEST_F(TestServerOneClient, Client_can_connect) {
    const std::string &res = client->receiveData();
    ASSERT_EQ(res, "CONNECTED_OK\n\r\n\r\n");
}

TEST_F(TestServerOneClient, Client_can_enter_username) {
    client->receiveData();
    client->sendData("ENTER_USERNAME\nusername:myuser1\n\r\n\r\n");
    const std::string &res = client->receiveData();
    ASSERT_EQ(res, "REQ_ACCEPTED\n\r\n\r\n");
}

TEST_F(TestServerOneClient, Client_room_req_denied) {
    client->receiveData();
    client->sendData("GET_ROOM_LIST\n\r\n\r\n");
    const std::string &res = client->receiveData();
    ASSERT_EQ(res, "REQ_DENIED\n\r\n\r\n");
}

TEST_F(TestServerOneClient, Client_room_req_accepted) {
    client->receiveData();
    client->sendData("ENTER_USERNAME\nusername:myuser1\n\r\n\r\n");
    client->receiveData();
    client->sendData("GET_ROOM_LIST\n\r\n\r\n");
    const std::string &res = client->receiveData();
    ASSERT_TRUE(res.find("REQ_ACCEPTED") != std::string::npos);
}

TEST_F(TestServerOneClient, Client_join_game_denied) {
    client->receiveData();
    client->sendData("JOIN_GAME\n\r\n\r\n");
    const std::string &res = client->receiveData();
    ASSERT_EQ(res, "REQ_DENIED\n\r\n\r\n");
}

TEST_F(TestServerOneClient, Client_wrong_protocol_disconnect) {
    EXPECT_THROW(
            client->receiveData();
            for (int i = 0; i < 6; ++i) {
                client->sendData("qwertzuiopasdfghjklyxcbvmqwertzuiopasdfghjklyxcbvmqwertzuiopasdfghjklyxcbvm");
            }
            const std::string &res = client->receiveData();, std::runtime_error);
}