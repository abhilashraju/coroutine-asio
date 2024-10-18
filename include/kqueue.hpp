#include "commondef.hpp"

#include <sys/event.h>
#include <unistd.h>

#include <functional>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
class KqueueContext
{
  public:
    int create()
    {
        return kqueue_fd = kqueue();
    }

    ~KqueueContext()
    {
        close(kqueue_fd);
    }

    int add_fd(int fd, EventHandler handler)
    {
        struct kevent event;
        EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
        if (int err = kevent(kqueue_fd, &event, 1, nullptr, 0, nullptr);
            err == -1)
        {
            perror("kevent: add_fd");
            return err;
        }
        handlers[fd] = handler;
        return 0;
    }

    void processEvents(int n)
    {
        for (int i = 0; i < n; ++i)
        {
            int fd = events[i].ident;
            if (handlers.find(fd) != handlers.end())
            {
                auto handler = std::move(handlers[fd]);
                handlers.erase(fd);
                handler();
            }
        }
    }
    int wait()
    {
        struct timespec timeout = {0, 0};
        return kevent(kqueue_fd, nullptr, 0, events.data(), events.size(),
                      &timeout);
    }

  private:
    int kqueue_fd;
    std::vector<struct kevent> events{16};
    std::unordered_map<int, EventHandler> handlers;
    std::queue<EventHandler> message_queue;
};
