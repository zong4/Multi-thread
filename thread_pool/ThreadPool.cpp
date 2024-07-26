#include "ThreadPool.hpp"

ThreadPool::ThreadPool(int num)
{
    init(num);
}

ThreadPool& ThreadPool::instance()
{
    static ThreadPool pool(4);
    return pool;
}

bool ThreadPool::isRunning() const
{
    rlock lock(_mtx);
    return _running;
}

void ThreadPool::terminate()
{
    {
        wlock lock(_mtx);
        if (_running)
            _running = false;
        else
            return;
    }

    _cond.notify_all();
    for (auto& thread : _threads)
        thread.join();
}

void ThreadPool::cancel()
{
    {
        wlock lock(_mtx);
        if (_running)
            _running = false;
        else
            return;

        _tasks.clear();
    }

    _cond.notify_all();
    for (auto& thread : _threads)
        thread.join();
}

void ThreadPool::init(int num)
{
    // wlock lock(_mtx); // only one instance

    _running = true;

    _threads.reserve(num);
    for (int i = 0; i < num; ++i)
        _threads.emplace_back(std::thread(std::bind(&ThreadPool::spawn, this)));
}

void ThreadPool::spawn()
{
    for (;;)
    {
        bool                  pop = false;
        std::function<void()> task;

        {
            wlock lock(_mtx);
            _cond.wait(lock, [this, &pop, &task] {
                pop = _tasks.pop(task);
                return !_running || pop;
            });
        }

        if (!_running)
            return;

        task();
    }
}
