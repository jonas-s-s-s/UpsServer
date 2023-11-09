//
// Created by xPC on 20.10.2023.
//

#include "DelimitedClientSocket.h"
#include "netTools.h"

DelimitedClientSocket::DelimitedClientSocket(const std::string &address, uint16_t port, std::string delimiter)
        : delimiter(std::move(delimiter)), socket(address, port) {}

std::vector<std::string> DelimitedClientSocket::receiveMsg() const {
    std::vector<std::string> msgBuffer = split(this->socket.receiveData(), delimiter);
    return msgBuffer;
}


std::vector<std::string> DelimitedClientSocket::receiveMsgRepeated(unsigned int count) const {
    std::vector<std::string> allMessages;

    //Repeat until desired amount of messages has been received
    while (allMessages.size() < count) {
        //Start receiving messages
        const std::vector<std::string> &currentMsg = this->receiveMsg();
        //Copy content of currentMsg to main vector
        allMessages.insert(allMessages.end(), currentMsg.begin(), currentMsg.end());
    }

    return allMessages;
}

void DelimitedClientSocket::sendMsg(const std::string &message) const {
    this->socket.sendData(message + delimiter);
}

DelimitedClientSocket::DelimitedClientSocket(int clientSocketFd, std::string delimiter) : delimiter(
        std::move(delimiter)), socket(clientSocketFd) {}

