#pragma once

#include <queue>

#include "pch.h"

template <typename T>
class TaskQueue : protected std::queue<T>
{
private:
    mutable std::shared_mutex _mtx;

public:
    TaskQueue() = default;
    ~TaskQueue() { clear(); }
    TaskQueue(TaskQueue const&)            = delete;
    TaskQueue(TaskQueue&&)                 = delete;
    TaskQueue& operator=(TaskQueue const&) = delete;
    TaskQueue& operator=(TaskQueue&&)      = delete;

    bool   empty() const;
    size_t size() const;
    void   clear();
    bool   pop(T& holder); // different from std::queue::pop()
    void   push(T const& obj);

    template <typename... Args>
    void emplace(Args&&... args);
};

template <typename T>
bool TaskQueue<T>::empty() const
{
    rlock lock(_mtx);
    return std::queue<T>::empty();
}

template <typename T>
size_t TaskQueue<T>::size() const
{
    rlock lock(_mtx);
    return std::queue<T>::size();
}

template <typename T>
void TaskQueue<T>::clear()
{
    wlock lock(_mtx);
    while (!std::queue<T>::empty())
        std::queue<T>::pop();
}

template <typename T>
bool TaskQueue<T>::pop(T& holder)
{
    wlock lock(_mtx);

    if (std::queue<T>::empty())
        return false;

    holder = std::move(std::queue<T>::front());
    std::queue<T>::pop();
    return true;
}

template <typename T>
void TaskQueue<T>::push(T const& obj)
{
    wlock lock(_mtx);
    std::queue<T>::push(obj);
}

template <typename T>
template <typename... Args>
void TaskQueue<T>::emplace(Args&&... args)
{
    wlock lock(_mtx);
    std::queue<T>::emplace(std::forward<Args>(args)...);
}