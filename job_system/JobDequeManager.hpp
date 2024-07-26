#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>

#include "Job.hpp"
#include "LockFreeJobDeque.hpp"

/// @brief manager memorys of job deques, conntrol by main thread
class JobDequeManager : public Uncopyable
{
private:
    std::vector<std::shared_ptr<LockFreeJobDeque<std::function<void()>>>> _jobDeques;

public:
    size_t                      _index = 0;
    std::shared_mutex           _mtx;
    std::condition_variable_any _cv;

private:
    JobDequeManager() {}

public:
    ~JobDequeManager() { clear(); }
    inline static JobDequeManager& instance()
    {
        static JobDequeManager instance;
        return instance;
    }

    inline std::shared_ptr<LockFreeJobDeque<std::function<void()>>>       operator[](int index) { return _jobDeques[index]; }
    inline const std::shared_ptr<LockFreeJobDeque<std::function<void()>>> operator[](int index) const { return _jobDeques[index]; }
    inline void                                                           clear() { _jobDeques.clear(); }

    inline int addDeque() // return index
    {
        _jobDeques.emplace_back(std::make_shared<LockFreeJobDeque<std::function<void()>>>());
        return _jobDeques.size() - 1;
    }

    template <class F, class... Args>
    auto async(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        using return_type = decltype(f(args...));
        using future_type = std::future<return_type>;
        using task_type   = std::packaged_task<return_type()>;

        auto                       bind_func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        std::shared_ptr<task_type> task      = std::make_shared<task_type>(std::move(bind_func));

        _jobDeques[_index]->push_front([task]() -> void { (*task)(); });
        // _index = (++_index) % _jobDeques.size(); // distribute jobs to different threads
        _cv.notify_all();

        future_type fut = task->get_future();
        return fut;
    }
};