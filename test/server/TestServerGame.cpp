//
// Created by xPC on 08.11.2023.
//
#include "testBaseServerGame.h"
#include <gtest/gtest.h>

class TestServerGame : public testBaseServerGame {
  protected:
    void SetUp() override {
        client1 = new ClientSocket{"127.0.0.1", 3001};
        client2 = new ClientSocket{"127.0.0.1", 3001};
    }

    void TearDown() override {
        delete client1;
        delete client2;
    }
};

TEST_F(TestServerGame, Clients_can_start_game) {
    // CONNECTED_OK
    client1->receiveData();
    client2->receiveData();

    client1->sendData("ENTER_USERNAME\nusername:TESTuser1\n\r\n\r\n");
    client2->sendData("ENTER_USERNAME\nusername:TESTuser2\n\r\n\r\n");

    // REQ_ACCEPTED
    client1->receiveData();
    client2->receiveData();;

    client1->sendData("JOIN_GAME\ngame_id:3\n\r\n\r\n");
    std::string state1 = client1->receiveData();

    client2->sendData("JOIN_GAME\ngame_id:3\n\r\n\r\n");
    std::string state2 = client2->receiveData();

    ASSERT_TRUE((state1.find("GAME_STATE") != std::string::npos) && (state2.find("GAME_STATE") != std::string::npos));
}

TEST_F(TestServerGame, Clients_can_play_game) {
    // CONNECTED_OK
    client1->receiveData();
    client2->receiveData();

    client1->sendData("ENTER_USERNAME\nusername:TESTuser1\n\r\n\r\n");
    client2->sendData("ENTER_USERNAME\nusername:TESTuser2\n\r\n\r\n");
    // REQ_ACCEPTED
    client1->receiveData();
    client2->receiveData();

    client1->sendData("JOIN_GAME\ngame_id:3\n\r\n\r\n");
    // GAME_IDLE
    client1->receiveData();

    client2->sendData("JOIN_GAME\ngame_id:3\n\r\n\r\n");
    // JOINED_GAME
    client2->receiveData();

    // GAME_STATE
    std::string gs1 = client1->receiveData();

    if (gs1.find("on_turn:TESTuser2") != std::string::npos) {
        /**
         Loser:
        GAME_COMMAND\nadd_edge:{0,1}\n\r\n\r\n
        GAME_COMMAND\nadd_edge:{5,1}\n\r\n\r\n
        GAME_COMMAND\nadd_edge:{5,0}\n\r\n\r\n

        Winner:
        GAME_COMMAND\nadd_edge:{1,2}\n\r\n\r\n
        GAME_COMMAND\nadd_edge:{3,4}\n\r\n\r\n
         */

        client2->sendData("GAME_COMMAND\nadd_edge:{0,1}\n\r\n\r\n");
        client2->receiveData();

        client1->receiveData();
        client1->sendData("GAME_COMMAND\nadd_edge:{1,2}\n\r\n\r\n");
        client1->receiveData();

        client2->receiveData();
        client2->sendData("GAME_COMMAND\nadd_edge:{5,1}\n\r\n\r\n");
        client2->receiveData();

        client1->receiveData();
        client1->sendData("GAME_COMMAND\nadd_edge:{3,4}\n\r\n\r\n");
        client1->receiveData();

        client2->receiveData();
        client2->sendData("GAME_COMMAND\nadd_edge:{5,0}\n\r\n\r\n");
        std::string endstate = client2->receiveData();

        ASSERT_TRUE((endstate.find("winning_player:TESTuser1") != std::string::npos));
    } else {
        client1->receiveData();
        client1->sendData("GAME_COMMAND\nadd_edge:{0,1}\n\r\n\r\n");
        client1->receiveData();

        client2->receiveData();
        client2->sendData("GAME_COMMAND\nadd_edge:{1,2}\n\r\n\r\n");
        client2->receiveData();

        client1->receiveData();
        client1->sendData("GAME_COMMAND\nadd_edge:{5,1}\n\r\n\r\n");
        client1->receiveData();

        client2->receiveData();
        client2->sendData("GAME_COMMAND\nadd_edge:{3,4}\n\r\n\r\n");
        client2->receiveData();

        client1->receiveData();
        client1->sendData("GAME_COMMAND\nadd_edge:{5,0}\n\r\n\r\n");
        std::string endstate = client1->receiveData();

        ASSERT_TRUE((endstate.find("winning_player:TESTuser2") != std::string::npos));
    }
}

TEST_F(TestServerGame, Clients_can_pause_game) {
    // CONNECTED_OK
    client1->receiveData();
    client2->receiveData();

    client1->sendData("ENTER_USERNAME\nusername:TESTuser1\n\r\n\r\n");
    client2->sendData("ENTER_USERNAME\nusername:TESTuser2\n\r\n\r\n");

    // REQ_ACCEPTED
    client1->receiveData();
    client2->receiveData();

    client1->sendData("JOIN_GAME\ngame_id:3\n\r\n\r\n");
    client1->receiveData();

    client2->sendData("JOIN_GAME\ngame_id:3\n\r\n\r\n");
    client2->receiveData();

    //Disconnect the client
    client2->sendData("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string res = client1->receiveData();

    ASSERT_TRUE(res.find("PAUSED") != std::string::npos);
}

TEST_F(TestServerGame, Clients_can_leave_game) {
    // CONNECTED_OK
    client1->receiveData();
    client2->receiveData();

    client1->sendData("ENTER_USERNAME\nusername:TESTuser1\n\r\n\r\n");
    // REQ_ACCEPTED
    client1->receiveData();

    client2->sendData("ENTER_USERNAME\nusername:TESTuser2\n\r\n\r\n");
    client2->receiveData();

    client1->sendData("JOIN_GAME\ngame_id:3\n\r\n\r\n");
    client1->receiveData();

    client2->sendData("JOIN_GAME\ngame_id:3\n\r\n\r\n");
    client2->receiveData();

    client1->receiveData();

    //Client will leave game
    client2->sendData("GAME_LEAVE\n\r\n\r\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::string res1 = client2->receiveData();

    std::string res2 = client1->receiveData();

    ASSERT_TRUE(res1.find("CONNECTED_OK") != std::string::npos && res2.find("CONNECTED_OK") != std::string::npos);
}


