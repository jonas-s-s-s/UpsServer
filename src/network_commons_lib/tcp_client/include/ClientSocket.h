//
// Created by xPC on 20.10.2023.
//

#pragma once

#include "SocketBase.h"
#include <string>

class ClientSocket : public SocketBase {
  public:
    ClientSocket(const std::string& address, uint16_t port);

    explicit ClientSocket(int clientSocketFd);
};