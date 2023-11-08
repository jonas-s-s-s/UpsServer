//
// Created by xPC on 20.10.2023.
//
#include <netinet/in.h>
#include <stdexcept>
#include <arpa/inet.h>
#include <netdb.h>
#include "netTools.h"

std::vector<std::string> split(const std::string &s, const std::string &delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    //Do NOT add empty string to end of the vector
    std::string end = s.substr(pos_start);
    if (!end.empty())
        res.push_back(end);

    return res;
}

bool isIpAddress(const std::string &address) {
    struct sockaddr_in sa{};
    int result = inet_pton(AF_INET, address.c_str(), &(sa.sin_addr));
    return result != 0;
}

std::string getIpFromHostname(const std::string &hostname, const std::string &port) {
    struct addrinfo hints = {.ai_socktype = SOCK_STREAM};
    struct addrinfo *ai;
    int err_code = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &ai);
    if (err_code != 0)
        throw std::runtime_error(
                "Failed to get ip from hostname: " + hostname + " (getaddrinfo returned " + std::to_string(err_code) +
                ")");

    //Get ip from resulting structure (we're supporting only IpV4)
    auto const *addr_in = (struct sockaddr_in *) ai->ai_addr;
    char const *s = inet_ntoa(addr_in->sin_addr);

    return s;
}

std::string convertToValidIpV4(const std::string &host, uint16_t port) {
    if (isIpAddress(host))
        return host;
    else
        return getIpFromHostname(host, std::to_string(port));
}
