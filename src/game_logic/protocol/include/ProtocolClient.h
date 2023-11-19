//
// Created by xPC on 14.11.2023.
//

#pragma once

#include "BufferedParser.h"
#include "ClientSocket.h"
#include "ProtocolData.h"
#include <optional>
#include <vector>

class ProtocolClient {
  public:
    explicit ProtocolClient(int clientFd);

    std::optional<std::vector<ProtocolData>> receiveMsg();

    void sendMsg(const ProtocolData& msg) const;

    const std::string& getClientName() const;

    void sendReqDeniedMsg();

    void setClientName(const std::string& name);

    bool hasName() const;

    int getClientFd() const;

    int getReqDeniedCnt() const;

  private:
    const ClientSocket socket;
    const int clientFd;
    BufferedParser parser;
    std::string clientName;

    int reqDeniedCnt = 0;
};
