#pragma once

#include <chrono>
#include <thread>

#include "JobDequeManager.hpp"

class JobThread : public Uncopyable
{
private:
    bool        _running = true;
    int         _jobDequeId;
    std::thread _thread;

public:
    explicit JobThread(int jobDequeId) : _jobDequeId(jobDequeId) { _thread = std::thread(&JobThread::spawn, this); }
    ~JobThread() = default;

    inline void setRunning(bool running) { _running = running; }

    inline void join() { _thread.join(); }

private:
    void spawn()
    {
        for (;;)
        {
            JobDequeManager&      jobDequeManager = JobDequeManager::instance();
            std::function<void()> task;

            {
                rlock lock(jobDequeManager._mtx);
                jobDequeManager._cv.wait(lock, [this, &jobDequeManager, &task] {
                    if (!_running || jobDequeManager[_jobDequeId]->pop_front(task))
                        return true;
                    else if (jobDequeManager[(_jobDequeId + 1) % thread_num]->pop_back(task))
                        return true;
                    else
                        return false;

                    // return !_running || jobDequeManager[_jobDequeId]->pop_front(task) ||
                    //        jobDequeManager[(_jobDequeId + 1) % thread_num]->pop_back(task);
                });
            }

            if (!_running)
                return;

            task();

            // std::this_thread::sleep_for(std::chrono::seconds(thread_num - _jobDequeId));
        }
    }
};