#include "ThreadPool.hpp"

ThreadPool::ThreadPool(int num)
{
    init(num);
}

ThreadPool& ThreadPool::instance(int num)
{
    static ThreadPool pool(num);
    return pool;
}

bool ThreadPool::isRunning() const
{
    return _running;
}

void ThreadPool::terminate()
{
    _running = false;
    for (auto& thread : _threads)
        thread->setRunning(false);

    JobDequeManager::instance()._cv.notify_all();
    for (auto& thread : _threads)
        thread->join();
}

void ThreadPool::init(int num)
{
    _running = true;

    _threads.reserve(num);
    JobDequeManager& jobDequeManager = JobDequeManager::instance();
    for (int i = 0; i < num; ++i)
        _threads.emplace_back(std::make_unique<JobThread>(jobDequeManager.addDeque()));
}
