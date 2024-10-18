#pragma once
#include <coroutine>
#include <functional>
#include <sstream>
#include <type_traits>
#include <utility>
template <typename T>
concept Awaiter = requires(T t, std::coroutine_handle<> h) {
                      { t.await_ready() } -> std::convertible_to<bool>;
                      { t.await_suspend(h) };
                      { t.await_resume() };
                  };

template <typename T>
concept AsyncOperation = requires(T t, std::function<void()> a) {
                             { t.execute(std::move(a)) };
                             { t.value() } -> auto;
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
        handle.resume();
    }
    T value;
};

template <AsyncOperation AO>
struct AsyncOperationAwaiter
{
    bool await_ready()
    {
        return false;
    }

    void await_suspend(std::coroutine_handle<> handle)
    {
        op.execute([this, handle = std::move(handle)]() { handle.resume(); });
    }
    auto await_resume()
    {
        return op.value();
    }
    AsyncOperationAwaiter(AO op) : op(std::move(op)) {}
    AsyncOperationAwaiter(AsyncOperationAwaiter&&) = default;
    AO op;
    int n{10};
};
