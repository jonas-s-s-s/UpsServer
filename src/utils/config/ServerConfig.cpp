//
// Created by xPC on 08.11.2023.
//

#include <fstream>
#include <sstream>
#include <algorithm>
#include "ServerConfig.h"

ServerConfig::ServerConfig(const std::string &configFilePath) {
    std::ifstream file(configFilePath);

    if (file.fail())
        throw std::runtime_error("ServerConfig failed to open the file: " + configFilePath);

    parse(file);
}

void ServerConfig::parse(std::ifstream &file) {
    std::string line;
    while (std::getline(file, line)) {
        //Continue if line is whitespace only
        if (std::all_of(line.begin(), line.end(), isspace))
            continue;

        std::string lhs;
        std::string rhs;

        //Process line (avoiding using split function form netTools.h)
        bool isLhs = true;
        for (char const &c: line) {
            if (c == ':') {
                isLhs = false;
                continue;
            }

            if (isLhs) {
                lhs.push_back(c);
            } else {
                rhs.push_back(c);
            }
        }

        if (isLhs) {
            throw std::runtime_error("ServerConfig failed to parse file: missing \":\" delimiter in some line" + lhs);
        }

        trim(lhs);
        trim(rhs);

        if (lhs == "roomCount") {
            this->roomCount_ = std::stoi(rhs);
        } else if (lhs == "maxPlayerCount") {
            this->maxPlayerCount_ = std::stoi(rhs);
        } else if (lhs == "serverIp") {
            this->serverIp_ = rhs;
        } else if (lhs == "serverPort") {
            this->serverPort_ = static_cast<uint16_t>(std::stoi(rhs));
        } else {
            throw std::runtime_error(
                    "ServerConfig failed to parse file: it contains invalid left hand side argument - " + lhs);
        }
    }
}

void ServerConfig::trim(std::string &arg) {
    arg.erase(0, arg.find_first_not_of("\t\n\v\f\r ")); // left trim
    arg.erase(arg.find_last_not_of("\t\n\v\f\r ") + 1); // right trim
}
