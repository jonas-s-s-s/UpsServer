//
// Created by xPC on 12.11.2023.
//

#include <stdexcept>
#include <utility>
#include <fcntl.h>
#include "Epoll.h"

Epoll::Epoll(bool isEdgeTriggered) : epollFd(epoll_create1(0)), isEdgeTriggered(isEdgeTriggered) {
    if (epollFd == -1) {
        throw std::runtime_error("Epoll::Epoll: ERROR - Failed to create epoll file descriptor.");
    }

    eventsVector.reserve(maxEventsNum * sizeof(epoll_event));
}

void Epoll::addDescriptor(int fd) {
    monitoredFds.try_emplace(fd, fd);

    if (isEdgeTriggered) {
        _setNonBlocking(fd);
    }
}

void Epoll::removeDescriptor(int monitoredFd) {
    _epollCtlDelete(monitoredFd);
    monitoredFds.erase(monitoredFd);
}

void Epoll::waitForEvents() {
    //Start waiting for descriptor events
    int numOfEvents = epoll_wait(epollFd, &eventsVector[0], maxEventsNum, -1);

    for (int i = 0; i < numOfEvents; i++) {
        uint32_t events = eventsVector[i].events;
        int fd = eventsVector[i].data.fd;

        //Check for all possible events
        const std::array<uint32_t, 6> samples{EPOLLIN, EPOLLOUT, EPOLLRDHUP, EPOLLPRI, EPOLLERR, EPOLLHUP};
        for (uint32_t sample: samples) {
            //Check if the handler for this event exists
            if (monitoredFds.at(fd).handledEvents.count(events & sample)) {
                //Call the handler function
                monitoredFds.at(fd).handledEvents.at(events & sample)(fd);
            }
        }

        //Remove this descriptor if it's closing (this will work only if EPOLLRDHUP or EPOLLHUP events are listened for)
        if (events & (EPOLLRDHUP | EPOLLHUP)) {
            removeDescriptor(fd);
        }
    }
}

/**
 * ADD ONLY ONE eventId AT ONCE (TODO?)
 */
void Epoll::addEventHandler(int monitoredFd, uint32_t eventId, std::function<void(int)> eventHandler) {
    MonitoredDescriptor &fd = monitoredFds.at(monitoredFd);
    if (!fd.isInitialized) {
        fd.isInitialized = true;
        //Better solution than alreadyMonitoredEvents?
        fd.alreadyMonitoredEvents = eventId;
        _epollCtlAdd(monitoredFd, eventId);
    } else {
        //Better solution than alreadyMonitoredEvents?
        fd.alreadyMonitoredEvents = fd.alreadyMonitoredEvents | eventId;
        _epollCtlModify(monitoredFd, fd.alreadyMonitoredEvents);
    }

    fd.handledEvents[eventId] = std::move(eventHandler);
}

void Epoll::removeEventHandler(int monitoredFd, uint32_t eventId) {
    //TODO: System call to remove event listening from this fd?
    monitoredFds.at(monitoredFd).handledEvents.erase(eventId);
}

const std::unordered_map<int, MonitoredDescriptor> &Epoll::getMonitoredFds() const {
    return monitoredFds;
}

int Epoll::getEpollFd() const {
    return epollFd;
}

int Epoll::getIsEdgeTriggered() const {
    return isEdgeTriggered;
}

/**
 * ADDS events to a NEW fd. If the FD is not new, _epollCtlModify must be used instead.
 */
void Epoll::_epollCtlAdd(int fd, uint32_t events) const {
    struct epoll_event ev{};

    if (isEdgeTriggered)
        ev.events = events | EPOLLET;
    else
        ev.events = events;

    ev.data.fd = fd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        throw std::runtime_error("Epoll::_epollCtlAdd: ERROR - Failed adding event to descriptor.");
    }
}

/**
 * REWRITES the events of certain FD. All previously added events will be REMOVED.
 */
void Epoll::_epollCtlModify(int fd, uint32_t events) const {
    //TODO: Improve?
    struct epoll_event ev{};

    if (isEdgeTriggered)
        ev.events = events | EPOLLET;
    else
        ev.events = events;

    ev.data.fd = fd;
    if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        throw std::runtime_error("Epoll::_epollCtlModify: ERROR - Failed modifying file descriptor events.");
    }
}

void Epoll::_setNonBlocking(int fd) {
    if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) == -1) {
        throw std::runtime_error("Epoll::_setNonBlocking: ERROR - Failed to set descriptor into non-blocking mode.");
    }
}

void Epoll::_epollCtlDelete(int fd) const {
    struct epoll_event ev{};
    ev.data.fd = fd;
    epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, &ev);
}

MonitoredDescriptor::MonitoredDescriptor(int monitoredFd) : monitoredFd(monitoredFd) {
}
