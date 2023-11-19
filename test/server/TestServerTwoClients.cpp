//
// Created by xPC on 08.11.2023.
//
#include "testBaseServerTwoClients.h"
#include <gtest/gtest.h>

class TestServerTwoClients : public testBaseServerTwoClients {
  protected:
    void SetUp() override
    {
        client1 = new ClientSocket{"127.0.0.1", 3001};
        client2 = new ClientSocket{"127.0.0.1", 3001};
    }

    void TearDown() override
    {
        delete client1;
        delete client2;
    }
};

TEST_F(TestServerTwoClients, Clients_can_start_game)
{
    //    const std::string &res = client->receiveData();
    //    ASSERT_EQ(res, "CONNECTED_OK\n\r\n\r\n");
}