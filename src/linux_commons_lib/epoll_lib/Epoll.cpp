//
// Created by xPC on 12.11.2023.
//

#include <stdexcept>
#include <utility>
#include <fcntl.h>
#include "Epoll.h"

Epoll::Epoll(bool isEdgeTriggered) : _epollFd(epoll_create1(0)), _isEdgeTriggered(isEdgeTriggered) {
    if (_epollFd == -1) {
        throw std::runtime_error("Epoll::Epoll: ERROR - Failed to create epoll file descriptor.");
    }

    _eventsVector.reserve(_maxEventsNum * sizeof(epoll_event));
}

//######################################################################################################################

void Epoll::addDescriptor(int fd) {
    _monitoredFds.try_emplace(fd, fd);

    if (_isEdgeTriggered) {
        _setNonBlocking(fd);
    }
}

void Epoll::removeDescriptor(int monitoredFd) {
    _epollCtlDelete(monitoredFd);
    _monitoredFds.erase(monitoredFd);
}

void Epoll::waitForEvents() {
    //Start waiting for descriptor events
    int numOfEvents = epoll_wait(_epollFd, &_eventsVector[0], _maxEventsNum, -1);

    for (int i = 0; i < numOfEvents; i++) {
        uint32_t events = _eventsVector[i].events;
        int fd = _eventsVector[i].data.fd;

        //Check for all possible event types
        for (uint32_t evt: allEventTypes) {
            //Check if the handler for this event exists
            if (_monitoredFds.at(fd).hasHandler(events & evt)) {
                //Call the handler function
                _monitoredFds.at(fd).getHandler(events & evt)(fd);
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
    MonitoredDescriptor &fd = _monitoredFds.at(monitoredFd);
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

void Epoll::removeEventHandler(int monitoredFd, uint32_t eventType) {
    //Check for all possible event types
    for (uint32_t evt: allEventTypes) {
        //Check if the handler for this event exists
        if (_monitoredFds.at(monitoredFd).hasHandler(eventType & evt)) {
            //remove the handler function TODO
            _monitoredFds.at(monitoredFd).setHandler(eventType & evt, nullptr);
        }
    }

    //TODO: System call to remove event listening from this fd?
    _monitoredFds.at(monitoredFd).handledEvents.erase(eventType);
}

//######################################################################################################################

const std::unordered_map<int, MonitoredDescriptor> &Epoll::getMonitoredFds() const {
    return _monitoredFds;
}

int Epoll::getEpollFd() const {
    return _epollFd;
}

int Epoll::getIsEdgeTriggered() const {
    return _isEdgeTriggered;
}

//######################################################################################################################

void Epoll::_reloadEventHandlers(MonitoredDescriptor &monitoredDescriptor) {

}

/**
 * ADDS events to a NEW fd. If the FD is not new, _epollCtlModify must be used instead.
 */
void Epoll::_epollCtlAdd(int fd, uint32_t events) const {
    struct epoll_event ev{};
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        throw std::runtime_error("Epoll::_epollCtlAdd: ERROR - Failed adding event to descriptor.");
    }
}

/**
 * REWRITES the events of certain FD. All previously added events will be REMOVED.
 */
void Epoll::_epollCtlModify(int fd, uint32_t events) const {
    struct epoll_event ev{};
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev) == -1) {
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
    epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, &ev);
}

//######################################################################################################################

MonitoredDescriptor::MonitoredDescriptor(int monitoredFd) : monitoredFd(monitoredFd) {
}


bool MonitoredDescriptor::hasHandler(uint32_t eventType) const {
    switch (eventType) {
        case EPOLLIN:
            return IN_handler != nullptr;
        case EPOLLOUT:
            return OUT_handler != nullptr;
        case EPOLLRDHUP:
            return RDHUP_handler != nullptr;
        case EPOLLPRI:
            return PRI_handler != nullptr;
        case EPOLLERR:
            return ERR_handler != nullptr;
        case EPOLLHUP:
            return HUP_handler != nullptr;
        default:
            return false;
    }
}

void MonitoredDescriptor::setHandler(uint32_t eventType, std::function<void(int)> handler) {
    switch (eventType) {
        case EPOLLIN:
            IN_handler = std::move(handler);
            return;
        case EPOLLOUT:
            OUT_handler = std::move(handler);
            return;
        case EPOLLRDHUP:
            RDHUP_handler = std::move(handler);
            return;
        case EPOLLPRI:
            PRI_handler = std::move(handler);
            return;
        case EPOLLERR:
            ERR_handler = std::move(handler);
            return;
        case EPOLLHUP:
            HUP_handler = std::move(handler);
            return;
        default:
            return;
    }
}

std::function<void(int)> &MonitoredDescriptor::getHandler(uint32_t eventType) {
    switch (eventType) {
        case EPOLLIN:
            return IN_handler;
        case EPOLLOUT:
            return OUT_handler;
        case EPOLLRDHUP:
            return RDHUP_handler;
        case EPOLLPRI:
            return PRI_handler;
        case EPOLLERR:
            return ERR_handler;
        case EPOLLHUP:
            return HUP_handler;
        default:
            throw std::runtime_error("Epoll::MonitoredDescriptor::getHandler: ERROR - passed eventType is invalid.");
    }
}
