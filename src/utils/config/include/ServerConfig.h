//
// Created by xPC on 08.11.2023.
//

#pragma once

#include <string>

class ServerConfig {
public:
    const unsigned int &roomCount = roomCount_;
    const unsigned int &maxPlayerCount = maxPlayerCount_;
    const std::string &serverIp = serverIp_;
    const uint16_t &serverPort = serverPort_;

    /**
     * The configuration file should be formatted like so:
     * roomCount:<uint>
     * maxPlayerCount:<uint>
     * serverIp:<string>
     * serverPort:<uint16>
     * @param configFilePath path to this config file
     * @throws runtime_exception if the config file is invalid or impossible to read
     */
    explicit ServerConfig(const std::string &configFilePath);

private:
    unsigned int roomCount_{};
    unsigned int maxPlayerCount_{};
    std::string serverIp_;
    uint16_t serverPort_{};

    static void trim(std::string &arg);

    void parse(std::ifstream &file);
};
