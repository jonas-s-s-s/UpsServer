//
// Created by xPC on 14.11.2023.
//

#include "ProtocolClient.h"

ProtocolClient::ProtocolClient(const int clientFd) : socket(clientFd), clientFd(clientFd) {}

std::optional<std::vector<ProtocolData>> ProtocolClient::receiveMsg() {
    return parser.parse(socket.receiveData());
}

void ProtocolClient::sendMsg(const ProtocolData &msg) const {
    socket.sendData(serializeProtocolData(msg));
}

const std::string &ProtocolClient::getClientName() const {
    return clientName;
}

void ProtocolClient::setClientName(const std::string &name) {
    clientName = name;
}

bool ProtocolClient::hasName() const {
    return !clientName.empty();
}

int ProtocolClient::getClientFd() const {
    return clientFd;
}

int ProtocolClient::getReqDeniedCnt() const {
    return reqDeniedCnt;
}

void ProtocolClient::sendReqDeniedMsg() {
    reqDeniedCnt++;
    sendMsg(newProtocolMessage(MethodName::REQ_DENIED));
}
