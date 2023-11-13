//
// Created by xPC on 12.11.2023.
//

#pragma once

#include <set>
#include <unordered_map>
#include <functional>
#include <sys/epoll.h>

class MonitoredDescriptor {
public:
    explicit MonitoredDescriptor(int monitoredFd);

    bool isInitialized = false;
    const int monitoredFd;
    std::unordered_map<uint32_t, std::function<void(int)>> handledEvents{}; //Can be replaced with 6 attributes / array?


};

class Epoll {
public:
    Epoll(bool isEdgeTriggered);

    void addDescriptor(int fd);

    void removeDescriptor(int monitoredFd);

    void waitForEvents();

    void addEventHandler(int monitoredFd, uint32_t eventId, std::function<void(int)> eventHandler);

    void removeEventHandler(int monitoredFd, uint32_t eventId);

    const std::unordered_map<int, MonitoredDescriptor> &getMonitoredFds() const;

    int getEpollFd() const;

    int getIsEdgeTriggered() const;

private:
    std::unordered_map<int, MonitoredDescriptor> monitoredFds{};
    const int epollFd;
    const int isEdgeTriggered;

    const int maxEventsNum = 10;
    std::vector<epoll_event> eventsVector{};

    void _epollCtlAdd(int fd, uint32_t events) const;

    void _epollCtlModify(int fd, uint32_t events) const;

    static void _setNonBlocking(int fd);

    void _epollCtlDelete(int fd) const;
};


