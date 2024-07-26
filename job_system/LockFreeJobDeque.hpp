#pragma once

#include <atomic>
#include <deque>

#include "../thread_pool/pch.h"

template <typename T>
class LockFreeJobDeque : public std::deque<T>
{
private:
    std::atomic<size_t> _size;

public:
    LockFreeJobDeque() : std::deque<T>(), _size(0) {}
    ~LockFreeJobDeque()                                  = default;
    LockFreeJobDeque(LockFreeJobDeque const&)            = delete;
    LockFreeJobDeque(LockFreeJobDeque&&)                 = delete;
    LockFreeJobDeque& operator=(LockFreeJobDeque const&) = delete;
    LockFreeJobDeque& operator=(LockFreeJobDeque&&)      = delete;

    inline size_t size() const { return _size.load(std::memory_order_relaxed); }
    bool          pop_front(T& holder);     // self thread call
    bool          pop_back(T& holder);      // other thread calls
    void          push_front(T const& obj); // add job, main thread call

    template <typename... Args>
    void emplace_front(Args&&... args); // add job, main thread call
};

template <typename T>
bool LockFreeJobDeque<T>::pop_front(T& holder)
{
    if (_size.load(std::memory_order_relaxed) == 0)
        return false;

    holder = std::move(std::deque<T>::front());
    std::deque<T>::pop_front();

    _size.fetch_sub(1, std::memory_order_relaxed);
    return true;
}

template <typename T>
bool LockFreeJobDeque<T>::pop_back(T& holder)
{
    if (_size.load(std::memory_order_relaxed) <= 1)
        return false;

    holder = std::move(std::deque<T>::back());
    std::deque<T>::pop_back();

    _size.fetch_sub(1, std::memory_order_relaxed);

    std::cout << "called(stole) by other thread" << std::endl;

    return true;
}

template <typename T>
void LockFreeJobDeque<T>::push_front(T const& obj)
{
    std::deque<T>::push_front(obj);

    // add barrier
    COMPILER_BARRIER;

    _size.fetch_add(1, std::memory_order_relaxed);
}

template <typename T>
template <typename... Args>
void LockFreeJobDeque<T>::emplace_front(Args&&... args)
{
    std::deque<T>::emplace_front(std::forward<Args>(args)...);

    // add barrier
    COMPILER_BARRIER;

    _size.fetch_add(1, std::memory_order_relaxed);
}