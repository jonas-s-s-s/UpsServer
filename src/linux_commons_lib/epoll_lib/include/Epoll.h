//
// Created by xPC on 12.11.2023.
//

#pragma once

#include <set>
#include <unordered_map>
#include <functional>
#include <sys/epoll.h>

constexpr static const std::array<uint32_t, 6> allEventTypes{EPOLLIN, EPOLLOUT, EPOLLRDHUP, EPOLLPRI, EPOLLERR,
                                                             EPOLLHUP};

class MonitoredDescriptor {
public:
    explicit MonitoredDescriptor(int monitoredFd);

    bool isInitialized = false;
    const int monitoredFd;

    /**
     * Checks if this eventType has a handler function assigned to it
     */
    bool hasHandler(uint32_t eventType) const;

    /**
     * Sets an event handler of a SINGLE eventType (don't use | bitwise or notation)
     */
    void setHandler(uint32_t eventType, std::function<void(int)> handler);

    /**
     * Gets the events handler associated with this SINGLE eventType
     */
    std::function<void(int)>& getHandler(uint32_t eventType);

private:
    //No need to use unordered_map since there are only 6 possible event types
    std::function<void(int)> IN_handler = nullptr;
    std::function<void(int)> OUT_handler = nullptr;
    std::function<void(int)> RDHUP_handler = nullptr;
    std::function<void(int)> PRI_handler = nullptr;
    std::function<void(int)> ERR_handler = nullptr;
    std::function<void(int)> HUP_handler = nullptr;
};

class Epoll {
public:
    Epoll(bool isEdgeTriggered);

    void addDescriptor(int fd);

    void removeDescriptor(int monitoredFd);

    void waitForEvents();

    void addEventHandler(int monitoredFd, uint32_t eventId, std::function<void(int)> eventHandler);

    void removeEventHandler(int monitoredFd, uint32_t eventType);

    const std::unordered_map<int, MonitoredDescriptor> &getMonitoredFds() const;

    int getEpollFd() const;

    int getIsEdgeTriggered() const;

private:
    std::unordered_map<int, MonitoredDescriptor> _monitoredFds{};
    const int _epollFd;
    const int _isEdgeTriggered;

    const int _maxEventsNum = 10;
    std::vector<epoll_event> _eventsVector{};

    void _reloadEventHandlers(MonitoredDescriptor& monitoredDescriptor);

    void _epollCtlAdd(int fd, uint32_t events) const;

    void _epollCtlModify(int fd, uint32_t events) const;

    static void _setNonBlocking(int fd);

    void _epollCtlDelete(int fd) const;
};


