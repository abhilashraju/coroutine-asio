#include "awaiters.hpp"
#include "context.hpp"
#include "task.hpp"

#include <coroutine>
#include <iostream>
#include <sstream>

struct StdIn
{
    Context& ctx;
    int n{10};
    StdIn(Context& ctx) : ctx(ctx) {}
    void execute(auto func)
    {
        ctx.add_fd(0, [this, handler = std::move(func)]() {
            char buffer[128];
            ssize_t count = read(0, buffer, sizeof(buffer));
            std::stringstream ss;
            ss.write(buffer, count);
            ss >> n;
            handler();
        });
    }
    int value()
    {
        return n;
    }
};
struct StdOut
{
    Context& ctx;
    int n{10};
    StdOut(Context& ctx, int value) : ctx(ctx), n(value) {}
    void execute(auto func)
    {
        ctx.add_fd(1, [this, handler = std::move(func)]() {
            std::stringstream ss;
            ss << n;
            ss >> n;
            std::string out = ss.str();
            write(1, out.data(), out.length());
            handler();
        });
    }
    void value() {}
};

struct StdInOut
{
    static StdIn async_read(Context& ctx)
    {
        return StdIn(ctx);
    }
    static StdOut async_write(Context& ctx, int value)
    {
        return StdOut(ctx, value);
    }
};
Task func(Context& ctx)
{
    int n = 10;
    while (n != 0)
    {
        n = co_await StdInOut::async_read(ctx);
        std::cout << "Read ";
        co_await StdInOut::async_write(ctx, n);
        std::cout << "\n ";
    }
    ctx.stop();
}
int main()
{
    try
    {
        Context ctx;

        auto task = func(ctx);
        (void)task;
        std::cout << "End of main" << std::endl;
        ctx.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
