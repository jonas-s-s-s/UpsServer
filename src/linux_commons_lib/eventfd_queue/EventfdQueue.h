//
// Created by xPC on 12.11.2023.
//

#pragma once

#include <queue>
#include <mutex>
#include <sys/eventfd.h>

template<typename T>
class EventfdQueue {
private:
    std::queue<T> data;
    std::mutex dataMutex;
    const int evfd = eventfd(0, 0);
public:
    EventfdQueue() {
        if (evfd == -1)
            throw std::runtime_error("EventfdQueue(): ERROR - Eventfd creation failed.");
    }

    void push(T item) {
        std::scoped_lock lock{dataMutex};
        data.push(item);

        //An event is created - signals write into queue - can be captured by epoll
        eventfd_write(evfd, 1);
    }

    T pop() {
        std::scoped_lock lock{dataMutex};
        T item = data.front();
        data.pop();
        return item;
    }

    bool isEmpty() {
        std::scoped_lock lock{dataMutex};
        return data.empty();
    }

    /**
     * Thread safe by default.
     * @return Returns const int representing this queue's eventfd
     */
    int getEvfd() const {
        return evfd;
    }
};