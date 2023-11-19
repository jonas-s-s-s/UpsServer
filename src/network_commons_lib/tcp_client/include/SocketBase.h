//
// Created by xPC on 20.10.2023.
//

#pragma once

#include <string>

class SocketBase {
  protected:
    int socketFd = 0;

    SocketBase() = default;

  public:
    virtual std::string receiveData() const;

    virtual void sendData(const std::string& data) const;

    virtual ~SocketBase();
};
