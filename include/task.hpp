#pragma once
#include <coroutine>
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
            requires(std::is_standard_layout_v<T>)
        ValueAwaiter<T> await_transform(T value)
        {
            return ValueAwaiter<T>{value};
        }
        // template <Awaiter T>
        // T& await_transform(T& value)
        // {
        //     return value;
        // }
        // template <Awaiter T>
        // T&& await_transform(T&& value)
        // {
        //     return std::move(value);
        // }
        template <AsyncOperation T>
        AsyncOperationAwaiter<T> await_transform(T&& value)
        {
            return AsyncOperationAwaiter<T>(std::move(value));
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