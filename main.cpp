#include "context.hpp"

#include <coroutine>
#include <iostream>
#include <sstream>
#include <type_traits>
template <typename T>
concept Awaiter = requires(T t, std::coroutine_handle<> h) {
                      { t.await_ready() } -> std::convertible_to<bool>;
                      { t.await_suspend(h) };
                      { t.await_resume() };
                  };
template <typename T>
struct ValueAwaiter
{
    T await_resume()
    {
        return value;
    }
    bool await_ready()
    {
        return true;
    }
    void await_suspend(std::coroutine_handle<> handle)
    {
        std::cout << "await_suspend" << std::endl;
        handle.resume();
    }
    T value;
};

struct AsyncRead
{
    bool await_ready()
    {
        return false;
    }
    void await_suspend(std::coroutine_handle<> handle)
    {
        ctx.add_fd(0, [this, handle = std::move(handle)]() {
            char buffer[128];
            ssize_t count = read(0, buffer, sizeof(buffer));
            std::stringstream ss;
            ss.write(buffer, count);
            ss >> n;
            handle.resume();
        });
    }
    int await_resume()
    {
        return n;
    }
    AsyncRead(Context& ctx) : ctx(ctx) {}
    AsyncRead(AsyncRead&&) = default;
    Context& ctx;
    int n{10};
};
struct Task
{
    struct promise_type
    {
        Task get_return_object()
        {
            return Task{this};
        }
        std::suspend_never initial_suspend()
        {
            return {};
        }
        std::suspend_never final_suspend() noexcept
        {
            return {};
        }
        void return_value(int value) {}
        void unhandled_exception() {}
        template <typename T>
            requires(std::is_pod_v<T>)
        ValueAwaiter<T> await_transform(T value)
        {
            return ValueAwaiter<T>{value};
        }
        template <Awaiter T>
        T& await_transform(T& value)
        {
            return value;
        }
        template <Awaiter T>
        T&& await_transform(T&& value)
        {
            return std::move(value);
        }
    };
    ~Task()
    {
        if (promise)
        {
            promise->unhandled_exception();
        }
    }
    promise_type* promise{nullptr};
};
Task func(Context& ctx)
{
    int n = 10;
    while (n != 0)
    {
        n = co_await AsyncRead(ctx);
        std::cout << "Read " << n << std::endl;
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
