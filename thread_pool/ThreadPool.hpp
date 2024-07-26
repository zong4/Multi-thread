#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <thread>

#include "TaskQueue.hpp"
#include "Uncopyable.hpp"

class ThreadPool : public Uncopyable
{
private:
    bool                             _running = false;
    mutable std::shared_mutex        _mtx;
    std::condition_variable_any      _cond;
    std::vector<std::thread>         _threads;
    TaskQueue<std::function<void()>> _tasks;

private:
    ThreadPool(int num);

public:
    ~ThreadPool() { cancel(); }
    static ThreadPool& instance();

    bool isRunning() const;

    void terminate();
    void cancel();

    template <class F, class... Args>
    auto async(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;

private:
    void init(int num);

    void spawn();
};

template <class F, class... Args>
auto ThreadPool::async(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
{
    using return_type = decltype(f(args...));
    using future_type = std::future<return_type>;
    using task_type   = std::packaged_task<return_type()>;

    if (!isRunning())
        throw std::runtime_error("thread pool is not running");

    auto                       bind_func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    std::shared_ptr<task_type> task      = std::make_shared<task_type>(std::move(bind_func));
    _tasks.emplace([task]() -> void { (*task)(); });
    _cond.notify_one();

    future_type fut = task->get_future();
    return fut;
}