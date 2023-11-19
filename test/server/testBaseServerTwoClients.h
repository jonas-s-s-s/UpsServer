//
// Created by xPC on 08.11.2023.
//

#pragma once

#include "ClientSocket.h"
#include "ServerController.h"
#include <gtest/gtest.h>
#include <thread>

class testBaseServerTwoClients : public ::testing::Test {
  protected:
  public:
    static void SetUpTestSuite()
    {
        std::thread t1 = std::thread([] {
            ServerController server = ServerController{};
            server.start("127.0.0.1", 3001, 3000, 60);
        });
        t1.detach();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    static void TearDownTestSuite()
    {
        // code here
    }

  protected:
    ClientSocket* client1;
    ClientSocket* client2;
};
