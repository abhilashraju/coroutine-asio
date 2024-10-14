#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <queue>
#include <unordered_map>
#include <vector>
#include "commondef.hpp"
class EpollContext
{
public:
    int create()
    {
        return epoll_fd = epoll_create1(0);
    }
    void ~EpollContext()
    {
        ::close(epoll_fd);
    }

    int add_fd(int fd, EventHandler handler)
    {
        struct epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLIN;
        if (int err = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event); err == -1)
        {
            perror("epoll_ctl: add_fd");
            return err;
        }
        handlers[fd] = handler;
        return 0;
    }
    int wait()
    {
        return epoll_wait(epoll_fd, events.data(), events.size(), 0);
    }
    void processEvents(int n)
    {

        for (int i = 0; i < n; ++i)
        {
            int fd = events[i].data.fd;
            if (handlers.find(fd) != handlers.end())
            {
                handlers[fd]();
            }
        }
    }

private:
    int epoll_fd;
    std::vector<struct epoll_event> events{16};
    std::unordered_map<int, EventHandler> handlers;
};
