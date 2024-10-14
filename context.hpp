#pragma once
#include "commondef.hpp"

#include <deque>
#include <stop_token>
template <typename Poll>
struct ContextImpl
{
    ContextImpl()
    {
        int poll_fd = poll.create();
        if (poll_fd == -1)
        {
            perror("poll");
            exit(EXIT_FAILURE);
        }
    }
    ContextImpl(const ContextImpl&) = delete;
    ContextImpl& operator=(const ContextImpl&) = delete;
    ContextImpl(ContextImpl&&) = delete;
    ContextImpl& operator=(ContextImpl&&) = delete;

    ~ContextImpl() {}

    void add_fd(int fd, EventHandler handler)
    {
        poll.add_fd(fd, handler);
    }

    void run()
    {
        auto token = stopSource.get_token();
        while (!token.stop_requested())
        {
            int n = poll.wait();
            if (n == -1)
            {
                perror("poll");
                return;
            }

            poll.processEvents(n);

            // Process message queue
            while (!message_queue.empty())
            {
                auto handler = message_queue.front();
                message_queue.pop_front();
                handler();
            }
        }
    }

    void post(EventHandler handler)
    {
        message_queue.push_back(handler);
    }
    void stop()
    {
        stopSource.request_stop();
    }
    Poll poll;
    std::deque<EventHandler> message_queue;
    std::stop_source stopSource;
};
#include "kqueue.hpp"
using Context = ContextImpl<KqueueContext>;
#ifdef TEST

int main()
{
    Context context;

    // Example usage: Add a file descriptor and an event handler
    int fd = 0; // Example file descriptor (stdin)
    context.add_fd(fd, [&]() {
        char buffer[128];
        ssize_t count = read(fd, buffer, sizeof(buffer));
        if (count == -1)
        {
            perror("read");
        }
        else
        {
            std::string strbuffer(buffer, count);
            context.post([strbuffer]() {
                std::cout << "Read " << strbuffer.size()
                          << " bytes: " << strbuffer << std::endl;
            });
        }
    });

    // Post a message to the message queue

    // Run the event loop
    context.run();

    return 0;
}
#endif
