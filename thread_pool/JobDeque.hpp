#pragma once

#include <deque>

#include "pch.h"

template <typename T>
class JobDeque : public std::deque<T>
{
private:
    mutable std::shared_mutex _headMtx;
    mutable std::shared_mutex _tailMtx;

public:
    JobDeque() = default;
    ~JobDeque() { clear(); }
    JobDeque(JobDeque const&)            = delete;
    JobDeque(JobDeque&&)                 = delete;
    JobDeque& operator=(JobDeque const&) = delete;
    JobDeque& operator=(JobDeque&&)      = delete;

    bool   empty() const;
    size_t size() const;
    void   clear();
    bool   pop_front(T& holder);
    bool   pop_back(T& holder);
    void   push_front(T const& obj);
    void   push_back(T const& obj);

    template <typename... Args>
    void emplace_front(Args&&... args);

    template <typename... Args>
    void emplace_back(Args&&... args);
};

template <typename T>
bool JobDeque<T>::empty() const
{
    rlock h_lock(_headMtx);
    rlock t_lock(_tailMtx);
    return std::deque<T>::empty();
}

template <typename T>
size_t JobDeque<T>::size() const
{
    rlock h_lock(_headMtx);
    rlock t_lock(_tailMtx);
    return std::deque<T>::size();
}

template <typename T>
void JobDeque<T>::clear()
{
    wlock h_lock(_headMtx);
    wlock t_lock(_tailMtx);
    std::deque<T>::clear();
}

template <typename T>
bool JobDeque<T>::pop_front(T& holder)
{
    wlock h_lock(_headMtx);

    if (std::deque<T>::empty())
        return false;

    holder = std::move(std::deque<T>::front());
    std::deque<T>::pop_front();
    return true;
}

template <typename T>
bool JobDeque<T>::pop_back(T& holder)
{
    wlock t_lock(_tailMtx);

    if (std::deque<T>::empty())
        return false;

    holder = std::move(std::deque<T>::back());
    std::deque<T>::pop_back();
    return true;
}

template <typename T>
void JobDeque<T>::push_front(T const& obj)
{
    wlock h_lock(_headMtx);
    std::deque<T>::push_front(obj);
}

template <typename T>
void JobDeque<T>::push_back(T const& obj)
{
    wlock t_lock(_tailMtx);
    std::deque<T>::push_back(obj);
}

template <typename T>
template <typename... Args>
void JobDeque<T>::emplace_front(Args&&... args)
{
    wlock h_lock(_headMtx);
    std::deque<T>::emplace_front(std::forward<Args>(args)...);
}

template <typename T>
template <typename... Args>
void JobDeque<T>::emplace_back(Args&&... args)
{
    wlock t_lock(_tailMtx);
    std::deque<T>::emplace_back(std::forward<Args>(args)...);
}