//
// Created by xPC on 12.11.2023.
//

#pragma once

#include <mutex>
#include <queue>
#include <sys/eventfd.h>

template <typename T> class EventfdQueue {
  private:
    std::queue<T> data;
    std::mutex dataMutex;
    const int evfd = eventfd(0, 0);

  public:
    EventfdQueue()
    {
        if (evfd == -1)
            throw std::runtime_error("EventfdQueue(): ERROR - Eventfd creation failed.");
    }

    void push(T&& item)
    {
        static_assert(std::is_rvalue_reference_v<T&&>);
        std::scoped_lock lock{dataMutex};
        data.push(std::move(item));

        // An event is created - signals write into queue - can be captured by epoll
        eventfd_write(evfd, 1);
    }

    void push(T& item)
    {
        static_assert(std::is_lvalue_reference_v<T&>);
        std::scoped_lock lock{dataMutex};
        data.push(item);

        // An event is created - signals write into queue - can be captured by epoll
        eventfd_write(evfd, 1);
    }

    T pop()
    {
        if constexpr (std::is_rvalue_reference_v<T&&>) {
            std::scoped_lock lock{dataMutex};
            T item = std::move(data.front());
            data.pop();
            return std::move(item);
        } else {
            std::scoped_lock lock{dataMutex};
            T item = data.front();
            data.pop();
            return item;
        }
    }

    bool isEmpty()
    {
        std::scoped_lock lock{dataMutex};
        return data.empty();
    }

    /**
     * Thread safe by default.
     * @return Returns const int representing this queue's eventfd
     */
    int getEvfd() const
    {
        return evfd;
    }
};